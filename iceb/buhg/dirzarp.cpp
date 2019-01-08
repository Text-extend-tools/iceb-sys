/* $Id: dirzarp.c,v 5.65 2014/02/28 05:13:47 sasa Exp $ */
/*11.04.2018    09.11.1992      Белых А.И.      dirzarp.c
Подпрограмма работы с насилениями/удержаниями
Если вернули 0 значит выбрали
            -1 значит нет нужных записей
*/
#include        "buhg.h"

void sapzr4(int tn,short prn,const char *fam,short mp,short gp,short na,int podr,double sumai,double sumaib,int dlinna_nazv);
void		zapzarn1(long,short,short,short,short,short,char[]);
void		promes(short,short,short,long);
void		zarnproc(short,short,int,double,const char*);

extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int dirzarp(short mp,short gp, //Дата
int tabn, //Табельный номер
int kp,  //Код подразделения
short prn,  //1-начисление 2-удержание
const char *fam, //Фамилия
const char *sht, //Счет учета в карточке
short dn,short mn,short gn, //Дата приема на работу
short du,short mu,short gu) //Дата увольнения с работы
{
class iceb_tu_str bros("");
int             i;
int             kom=0;
int             x,y; /*Точка начала вывода меню*/
class iceb_tu_str n("");
class iceb_tu_str n1("");
int 		K;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
short		d,m,g;
short		dk,mk,gk;
short		mm,gg;
int		prc;
class iceb_tu_str shet("");
class iceb_tu_str naim("");
double		suma=0.,sumai=0.,sumaib=0.;
int		i1;
short		mspz; /*метка соответствия первой записи*/
short dlinna_nazv=0;
short max_dlinna_nazv=0;
class iceb_tu_str nom_dok("");

SQLCURSOR curr;
//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);
VVOD VVOD1(2);

y=3; x=0;
pozz=pozz1=kom=0;

naz1:;
clear();

int             kls=LINES-5; /*Количество строк в окне*/
short		dz[kls],gz[kls],mz[kls],nz[kls],podr[kls];
short           kodna[kls]; /*Коды начисления*/

sprintf(strsql,"select * from Zarp where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and prn='%d' order by \
knah,godn,mesn,nomz asc",
gp,mp,gp,mp,tabn,prn);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(-1);
 }

sumaib=sumai=0.;
if(kolstr == 0)
 {
  if(provblokzarp(tabn,mp,gp) != 0)
    goto ddd;
  if(zapzagot(mp,gp,tabn,prn,kp) != 0)
   {

ddd:;
    sapzr4(tabn,prn,fam,mp,gp,prn,kp,sumai,sumaib,dlinna_nazv);
 
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
    K=soobshw(&SOOB,stdscr,-1,-1,1,1);

    if(K == FK3 || K == SFK3 || K == FK6 || K == ENTER || K == NOL)
      goto ddd;
    if(K == KEY_RESIZE)
     goto naz1;
           
    goto vper;
   }
  else
   goto naz1;
 }

while(cur.read_cursor(&row) != 0)
 {
  sumai+=atof(row[4]); 
  
  if(prn == 1 && shetb != NULL)
    if(pole1(shetb,row[5],',',0,&i1) == 0 || SRAV(shetb,row[5],0) == 0)
       sumaib+=atof(row[4]); 
  if(prn == 2 && shetbu != NULL)
    if(pole1(shetbu,row[5],',',0,&i1) == 0 || SRAV(shetbu,row[5],0) == 0)
       sumaib+=atof(row[4]); 

  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);
  if(iceb_tu_strlen(naim.ravno()) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(naim.ravno());
 }

naz:;
GDITE();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

/*MM5.clearMEN5();*/
cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();

dlinna_nazv=COLS-61;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;
 
for(i=0;i<=kls-1;i++)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn='%d' and\
  knah=%s",tabn,prn,row[3]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    dk=mk=gk=0;
   }
  else
    rsdat(&dk,&mk,&gk,row1[0],2);

  if(prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
  if(prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    if(prn == 1)
     printw("%s %s\n",gettext("Не найден код начисления"),row[3]);
    if(prn == 2)
     printw("%s %s\n",gettext("Не найден код удержания"),row[3]);
    OSTANOV();
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);

  
  kodna[i]=atoi(row[3]);

  rsdat(&d,&m,&g,row[0],2);
  dz[i]=d;
  mm=atoi(row[7]);
  gg=atoi(row[6]);
  mz[i]=mm;
  gz[i]=gg;
  nz[i]=atoi(row[9]);
  podr[i]=atoi(row[13]);

  if(gk != 0)
   sprintf(strsql,"%3s|%-*.*s|%2d|%02d.%04d|%9.2f|%-*s|%d|%-3s|%-*.*s|%*s|%d.%d.%d",
   row[3],
   iceb_tu_kolbait(dlinna_nazv,naim.ravno()),iceb_tu_kolbait(dlinna_nazv,naim.ravno()),naim.ravno(),
   d,mm,gg,
   atof(row[4]),
   iceb_tu_kolbait(5,row[5]),row[5],
   atoi(row[9]),row[13],
   iceb_tu_kolbait(1,row[12]),
   iceb_tu_kolbait(10,row[12]),
   row[12],
   iceb_tu_kolbait(1,row[14]),
   row[14],
   dk,mk,gk);
  else
   sprintf(strsql,"%3s|%-*.*s|%2d|%02d.%04d|%9.2f|%-*s|%d|%-3s|%-*.*s|%s",
   row[3],
   iceb_tu_kolbait(dlinna_nazv,naim.ravno()),iceb_tu_kolbait(dlinna_nazv,naim.ravno()),naim.ravno(),
   d,mm,gg,
   atof(row[4]),
   iceb_tu_kolbait(5,row[5]),row[5],
   atoi(row[9]),row[13],
   iceb_tu_kolbait(1,row[12]),
   iceb_tu_kolbait(10,row[12]),
   row[12],
   row[14]);


  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


sapzr4(tabn,prn,fam,mp,gp,prn,kp,sumai,sumaib,dlinna_nazv);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);

if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
//    if(provprov(row,dn,mn,gn,dk,mk,gk,sh,shk,kor,kto,nn,kop,debc,krec,raz,met) != 0)
//      continue;

    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

//    if(provprov(row,dn,mn,gn,dk,mk,gk,sh,shk,kor,kto,nn,kop,debc,krec,raz,met) != 0)
//      continue;

    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  GDITE();
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

//    if(provprov(row,dn,mn,gn,dk,mk,gk,sh,shk,kor,kto,nn,kop,debc,krec,raz,met) != 0)
//      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  
  if(SRAV(gettext("Конец списка"),str_out.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_out.ravno(),&n,2,'|');
  polen(str_out.ravno(),&n1,3,'|');

  polen(str_out.ravno(),&suma,5,'|');

  polen(str_out.ravno(),&shet,6,'|');
  polen(str_out.ravno(),&nom_dok,10,'|');
 } 

vper:;

switch(K)
 {
  case ESC:
  case FK10:
    GDITE();
    return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_2.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto ddd;
     
  case FK2: /*Просмотр начислений*/
    clear();
    prn = 1;
    kom=0;
    clear();
    goto naz1;

  case FK3:  /*Удаление записи*/
    if(provblokzarp(tabn,mp,gp) != 0)
     goto naz;
    if(nom_dok.ravno()[0] != '\0')
     {
      iceb_t_soob(gettext("Записи сделанные из нарядов, удаляются только в нарядах !"));
      goto naz;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;    

    GDITE();

    zarudnu(gp,mp,dz[kom],tabn,prn,kodna[kom],gz[kom],mz[kom],podr[kom],shet.ravno(),nz[kom]);

    clear();
    goto naz1;

  case SFK3:  /*Обнуление записей*/
    if(provblokzarp(tabn,mp,gp) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Обнулить все записи ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;    

    sprintf(strsql,"update Zarp \
set \
suma=0. \
where datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
prn=%d and nd = ''",gp,mp,gp,mp,tabn,prn);

    sql_zapis(strsql,0,0);

    clear();
    goto naz1;

  case FK4: /*Просмотр удержаний*/
    clear();
    prn = 2;
    kom=0;
    clear();
    goto naz1;


  case FK5: /*Добавление новой записи*/
    if(provblokzarp(tabn,mp,gp) != 0)
     {
     if(kolstr > 0)
       goto naz;
     else
       goto ddd;
     }
fk5:;
    n1.new_plus("");
    if(prn == 1)
     {
      if(dirnach(2,&n,&n1) != 0)
       goto naz1;
     }
    if(prn == 2)
     {
      if(diruder(2,&n,&n1) != 0)
       goto naz1;
     }

kkk:;
    if(prn == 1)
      sprintf(strsql,"select naik,shet from Nash where kod=%d",n.ravno_atoi());
    if(prn == 2)
      sprintf(strsql,"select naik,shet from Uder where kod=%d",n.ravno_atoi());
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      if(prn == 1)
       sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),n.ravno_atoi());
      if(prn == 2)
       sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),n.ravno_atoi());

      iceb_t_soob(strsql);

      naim.new_plus("");
      shet.new_plus("");
      goto naz;
     }
    else
     {
      naim.new_plus(row1[0]);
      shet.new_plus(row1[1]);
     }
    vnu(tabn,n.ravno_atoi(),prn,0,mp,gp,mp,gp,kp,shet.ravno(),0);

    clear();
    goto naz1;

      
  case SFK5: /*Добавдение новой записи*/
  case PLU:
    if(provblokzarp(tabn,mp,gp) != 0)
     {
     if(kolstr > 0)
       goto naz;
     else
       goto ddd;
     }  
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);


    n.new_plus("");
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код"));

    if((i=vvod1(&n,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;

    if(i == ESC)
      goto naz;
    if(n.getdlinna() <= 1)
      goto fk5;
      
    goto kkk;

  case FK6: /*Автоматизированный расчёт*/

    if(provblokzarp(tabn,mp,gp) != 0)
     {
      if(kolstr > 0)
       goto naz;
      else
       goto ddd;
     }


  
    /*Читаем настройки для расчёта зарплаты*/    
    zarreadn(1,mp,gp,prn);
    rasshetz(tabn,mp,gp,kp,prn,"",""); /*Расчёт*/

    goto naz1;

  case FK7: /*Просмотр прошлого месяца*/ 
    promes(mp,gp,prn,tabn);
    if(kolstr > 0)
      goto naz;
    else
      goto ddd;
      
  case FK8:  /*НСИ*/
    vnsi(1);
    clear();
    goto naz;

  case SFK8:  /*НСИ*/
    vnsi(0);
    clear();
//    zar_read_tn(1,mp,gp,NULL); //Чтение изменяющихся по датам настроек
    goto naz;

  case FK9:
    clear();
    GDITE();
  
//    prosf(imafprot);
    zar_pr_show(prn,tabn,mp,gp);
    clear();
    goto naz;

  case ENTER:  /*корректировка*/

    vnu(tabn,kodna[kom],prn,dz[kom],mp,gp,mz[kom],gz[kom],podr[kom],shet.ravno(),nz[kom]);

    clear();
    goto naz1;

  case NOL:  //Введение нового начисления как процент от выбранного
    if(prn == 2)
      goto naz;
    zarnproc(mp,gp,tabn,suma,shet.ravno());
    goto naz1;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto ddd;
 }

}

/**************/
/*Выдача шапки*/
/**************/

void sapzr4(int tn,short prn,const char *fam,short mp,short gp,short na,
int podr,double sumai,double sumaib,int dlinna_nazv)
{

move(0,0);
if( na == 1)
 printw("%s %d.%d%s",gettext("Ввод начислений за"),mp,gp,
 gettext("г."));
else
 printw("%s %d.%d%s",gettext("Ввод удержаний за"),mp,gp,
 gettext("г."));

printw(" %s:%d",gettext("Подразделение"),podr);

move(1,0);
printw("%s %d %s:%.2f",fam,tn,gettext("Сумма всех записей"),sumai);
if(na == 1 && shetb != NULL)
 printw("/%.2f/%.2f",sumaib,sumai-sumaib);

if(na == 2 && shetbu != NULL)
 printw("/%.2f/%.2f",sumaib,sumai-sumaib);

int kk=(dlinna_nazv-1)/2;

mvprintw(2,0,"   1   %*s2%*s 3    4        5       6   7  8  9  10",kk,"",kk,"");


int Y=2;
int X=COLS-20;

mvprintw(Y++,X,"%s:",gettext("Наименование полей"));
if(prn == 1)
 { 
  mvprintw(Y++,X,"1-%s",gettext("код начисления"));
  mvprintw(Y++,X,"2-%s",gettext("на-вание начисления"));
  mvprintw(Y++,X,"3-%s",gettext("день начисления"));
 }

if(prn == 2)
 {
  mvprintw(Y++,X,"1-%s",gettext("код удержания"));
  mvprintw(Y++,X,"2-%s",gettext("на-вание удержания"));
  mvprintw(Y++,X,"3-%s",gettext("день удержания"));
 }

mvprintw(Y++,X,"4-%s",gettext("в счёт даты"));
mvprintw(Y++,X,"5-%s",gettext("сумма"));
mvprintw(Y++,X,"6-%s",gettext("счёт"));
mvprintw(Y++,X,"7-%s",gettext("номер записи"));
mvprintw(Y++,X,"8-%s",gettext("код подразделения"));
mvprintw(Y++,X,"9-%s",gettext("комментарий"));
mvprintw(Y++,--X,"10-%s",gettext("номер документа"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("начис."),
"F3",gettext("удалить"),
"F4",gettext("удерж."),
"F5",gettext("запись"),
"F6",gettext("расч."),
"F7",gettext("пред."),
"F8",gettext("НСИ"),
"F9",gettext("про."),
"F10",gettext("выход"),NULL);
}
/**********************************/
/*Начисление по процентам         */
/**********************************/
void	zarnproc(short mp,short gp,int tabn,double suma,const char *shet)
{
static class iceb_tu_str kodnah("");
static class iceb_tu_str procent("");
int		kom=0;
char		strsql[512];
SQL_str		row;
double		sumar=0.;
double		proc=0.;
int 		knah;
int		i=0;
class ZARP     zp;
short		dz=0,mz=0,gz=0;
short		nomzap=0;
short		godn=gp,mesn=mp;
int		podr=0;
short		dd=0,md=0,gd=0;
class iceb_tu_str koment("");
SQLCURSOR curr;

if(suma == 0.)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Сумма начисления равна нолю !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  return;
 }

VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Процент........."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.."));
VV.VVOD_SPISOK_add_MD(gettext("Сумма..........."));

VV.VVOD_SPISOK_add_data(procent.ravno(),10);
VV.VVOD_SPISOK_add_data(kodnah.ravno(),10);
sprintf(strsql,"%.2f",suma);
VV.VVOD_SPISOK_add_data(strsql,20);

naz:;


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


kom=VV.vvod(0,1,1,-1,-1);

procent.new_plus(VV.VVOD_SPISOK_return_data(0));
kodnah.new_plus(VV.VVOD_SPISOK_return_data(1));
sumar=atof(VV.VVOD_SPISOK_return_data(2));
switch(kom)
 {
  case FK1:   /*Помощь*/
   clear();
   GDITE();
   iceb_t_pdoc("zarp2_2_1.txt");
   
   goto naz;

  case FK10:
  case ESC:
   clear();
   return;

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK4: /*Очистить меню*/
  
    procent.new_plus("");
    kodnah.new_plus("");
    goto naz;

  default:
    goto naz;
 }

if(procent.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Процент не введён !"));
  goto naz;
 }

if(kodnah.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код начисления !"));
  goto naz;
 }

knah=kodnah.ravno_atoi();
proc=procent.ravno_atof();
sumar=sumar*proc/100.;

 

sprintf(strsql,"select datz,godn,mesn,podr,nomz from Zarp \
where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and prn='1' and knah=%d",gp,mp,gp,mp,tabn,knah);
if(sql_readkey(&bd,strsql,&row,&curr) > 0)
 {
  rsdat(&dz,&mz,&gz,row[0],2);
  zp.dz=dz;
  godn=zp.godn=atoi(row[1]);
  mesn=zp.mesn=atoi(row[2]);
  nomzap=zp.nomz=atoi(row[4]);
  podr=zp.podr=atoi(row[3]);
  strcpy(zp.shet,shet);
 }

if(podr == 0)
 {
  sprintf(strsql,"select podr from Zarn where tabn=%d and god=%d \
and mes=%d",tabn,gp,mp);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
    podr=atoi(row[0]);
 }
sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn='1' \
and knah=%d",tabn,knah);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
    rsdat(&dd,&md,&gd,row[0],2);

if(dz == 0)
 dpm(&dz,&mp,&gp,5);

//sprintf(koment,"%s/%s",VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(2));
koment.new_plus(VV.VVOD_SPISOK_return_data(0));
koment.plus("/",VV.VVOD_SPISOK_return_data(2));
if((i=zapzarpv(dz,mp,gp,tabn,1,knah,sumar,shet,mesn,godn,0,nomzap,koment.ravno(),podr,dd,md,gd,"",zp)) == 1)
  goto naz;
if(i == 3)
  return;
if(i != 0)
  goto naz;
clear();
}
