/* $Id: dirf8dr.c,v 5.63 2013/11/05 10:50:48 sasa Exp $ */
/*31.12.2017	11.07.2000	Белых А.И.	dirf8dr.c
Просмотр документа формы 1ДФ
*/
#include	"buhg.h"

class dirf1df_poi
 {
  public:
  class iceb_tu_str fiop;
  class iceb_tu_str innp;
  class iceb_tu_str kod_doh;
  int		metpoi;

  dirf1df_poi()
   {
    clear_data();
   }
  void clear_data()
   {
    metpoi=0;  
    fiop.new_plus("");
    innp.new_plus("");
    kod_doh.new_plus("");
    
   }
 };
 
 

void dirf1dfvs(const char *nomd);

int zagf8dr1(const char*,int,int);
int		rasf8dr(const char*,int,class spis_oth*);
void		sapka2f8dr(short,int,const char*,int,double,double,int,int,int,double,double,short);
void		vkorz(int,const char*,const char*,const char*,const char*,int,int);
void		vkorz1(int,const char*,const char*,const char*,const char*,int,int);
int		provf8dr(class dirf1df_poi*,char*,char*,char*);
void dirf8dr_2zap(const char *nomd,class dirf1df_poi *poi);
int zagf1df_csv(const char *nomd,int kolst);
int dirf8dr_zif(const char *nomd,int kolstr,int kvrt);


void dirf8dr(class iceb_tu_str *nomd)
{
long		kolstr,pozz1=0;
long		pozz=0;
SQL_str         row;
SQL_str         row1;
char		strsql[1024];
short		god;
int		kvrt;
int		vidd; //0-отчётный 1-новый отчётный 2-уточняющий
class iceb_tu_str fio("");
char		data1[32],data2[32];
short		d,m,g;
double		sumad,suman;
double		sumadn=0.,sumann=0.;
double		isumad,isuman;
double		isumadn=0.,isumann=0.;
int		K,i,kom,kom1;
short		mpn; /*Метка пустого документа*/
int		prc;
class iceb_tu_str priz("");
class iceb_tu_str lgota("");
class iceb_tu_str inn("");
char		datad[32],datan[32];
int		kolr; /*Количество физических лиц*/
short		mspz; /*метка соответствия первой записи*/
int		kolzap;
char		metkaz[8];
short		metkapr=0; //0-показать выплачено 1-начислено
int             priznak=0;
class iceb_t_mstr spis_menu(0);
class dirf1df_poi poi;
int max_dlina_fio=0;

class SQLCURSOR cur;
class SQLCURSOR cur1;
GDITE();

//Создаем класс меню
class VVOD DANET(1);
class VVOD VV(0);

nazad:;

pozz=kom=mpn=0;

/*Читаем шапку документа*/
sprintf(strsql,"select * from F8dr where nomd='%s'",nomd->ravno());
if(sql_readkey(&bd,strsql,&row,&cur) < 1)
 {
  iceb_t_soob(gettext("Не найден документ !"));
  return;
 } 
god=atoi(row[0]);
kvrt=atoi(row[1]);
vidd=atoi(row[3]);
memset(datad,'\0',sizeof(datad));
memset(datan,'\0',sizeof(datan));
d=m=g=0;
if(rsdat(&d,&m,&g,row[5],2) == 0)
 sprintf(datad,"%d.%d.%d",d,m,g);
if(rsdat(&d,&m,&g,row[6],2) == 0)
 sprintf(datan,"%d.%d.%d",d,m,g);


naz1:;
clear();
GDITE();

short kls=LINES-7;

kolzap=kolr=0;
isumad=isuman=0.;
isumadn=isumann=0.;

sprintf(strsql,"select inn,sumad,suman,priz,datap,datau,lgota,fio,sumadn,sumann,pr \
from F8dr1 where nomd='%s' order by inn asc,pr desc",nomd->ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  sapka2f8dr(god,kvrt,nomd->ravno(),vidd,isumad,isuman,kolr,poi.metpoi,kolzap,isumadn,isumann,metkapr);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  
  mpn=1;
   
  if(K == FK3 || K == FK5 || K == FK7)
   goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;

 }
else
 {
   printw("%s\n",gettext("Вычисляем сумму по документу"));
   refresh();
   max_dlina_fio=0;
   while(cur.read_cursor(&row) != 0)
    {
     if(provf8dr(&poi,row[0],row[7],row[3]) != 0)
      continue;
     isumad+=atof(row[1]);
     isuman+=atof(row[2]);
     isumadn+=atof(row[8]);
     isumann+=atof(row[9]);
     kolzap++;

     if(row[7][0] == '\0')
       {
        /*Читаем фамилию*/

        sprintf(strsql,"select fio from Kartb where inn='%s'",row[0]);
        if(readkey(strsql,&row1,&cur1) < 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),row[0]);
          iceb_t_soob(strsql);
         }
        else
          fio.new_plus(row1[0]);
       }
     else
       fio.new_plus(row[7]);

     if(max_dlina_fio < iceb_tu_strlen(fio.ravno()))
       max_dlina_fio=iceb_tu_strlen(fio.ravno());
    }
   printw("%s\n",gettext("Конец расчёта"));
   refresh();
 }

kolrab(nomd->ravno(),&kolr);

naz:;
GDITE();


mspz=2;
/*MM5.clearMEN5();*/
cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  
  pozz1++;  

  fio.new_plus("");

  if(provf8dr(&poi,row[0],row[7],row[3]) != 0)
    continue;

  if(row[7][0] == '\0')
   {
    /*Читаем фамилию*/

    sprintf(strsql,"select fio from Kartb where inn='%s'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) < 1)
     {
//      sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),row[0]);
//      iceb_t_soob(strsql);
     }
    else
      fio.new_plus(row1[0]);
   }
  else
     fio.new_plus(row[7]);

  sumad=atof(row[1]);
  suman=atof(row[2]);

  sumadn=atof(row[8]);
  sumann=atof(row[9]);
    
  memset(data1,'\0',sizeof(data1));
  memset(data2,'\0',sizeof(data2));
  if(row[4][0] != '0')
   {
    rsdat(&d,&m,&g,row[4],2);
    sprintf(data1,"%02d.%02d.%d",d,m,g);
   }
  if(row[5][0] != '0')
   {
    rsdat(&d,&m,&g,row[5],2);
    sprintf(data2,"%02d.%02d.%d",d,m,g);
   }

  memset(metkaz,'\0',sizeof(metkaz));
  if(row[7][0] != '\0')
    metkaz[0]='*';
  
  memset(strsql,'\0',sizeof(strsql));
  
  if(COLS < 100)
   {
    if(metkapr == 0)
     sprintf(strsql,"%s|%-*.*s|%9.2f|%9.2f|%-2s|%10s|%10s|%-2s|%s|%s",
     row[0],
     iceb_tu_kolbait(14,fio.ravno()),iceb_tu_kolbait(14,fio.ravno()),fio.ravno(),
     sumad,suman,row[3],data1,data2,row[6],row[10],metkaz);

    if(metkapr == 1)
     sprintf(strsql,"%s|%-*.*s|%9.2f|%9.2f|%-2s|%10s|%10s|%-2s|%s|%s",
     row[0],
     iceb_tu_kolbait(14,fio.ravno()),iceb_tu_kolbait(14,fio.ravno()),fio.ravno(),
     sumadn,sumann,row[3],data1,data2,row[6],row[10],metkaz);
   }
  else
   {
    int razmer=COLS-90;
    if(max_dlina_fio < razmer)
     razmer=max_dlina_fio;
     
    sprintf(strsql,"%s|%-*.*s|%9.2f|%9.2f|%9.2f|%9.2f|%-2s|%10s|%10s|%-2s|%s|%s",
    row[0],
    iceb_tu_kolbait(razmer,fio.ravno()),iceb_tu_kolbait(razmer,fio.ravno()),fio.ravno(),
    sumad,suman,sumadn,sumann,row[3],data1,data2,row[6],row[10],metkaz);
   }  
  spis_menu.spis_str.plus(strsql);
  i++;
   
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapka2f8dr(god,kvrt,nomd->ravno(),vidd,isumad,isuman,kolr,poi.metpoi,kolzap,isumadn,isumann,metkapr);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,4,0,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    if(provf8dr(&poi,row[0],row[7],row[3]) != 0)
      continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
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

    if(provf8dr(&poi,row[0],row[7],row[3]) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
/*
  printw("\npozz=%d kls=%d kolstr=%d i=%d\n",
  pozz,kls,kolstr,i);
  OSTANOV();
*/
  if(pozz + kls >= kolstr)
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
/*  
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n",pozz,pozz1,i,kls);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
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

    if(provf8dr(&poi,row[0],row[7],row[3]) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom == -4) /*Страница вверх*/
 {
  pozz-=kls;
  if(pozz < 0)  
   {
    beep();
    pozz=0;
   }
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&inn,1,'|');

  int nom_pole=5;
  if(COLS >= 100)
   nom_pole+=2;

  polen(str_vib.ravno(),&priz,nom_pole++,'|');
  polen(str_vib.ravno(),data1,sizeof(data1),nom_pole++,'|');
  polen(str_vib.ravno(),data2,sizeof(data2),nom_pole++,'|');
  polen(str_vib.ravno(),&lgota,nom_pole++,'|');
  polen(str_vib.ravno(),&priznak,nom_pole++,'|');
  polen(str_vib.ravno(),metkaz,sizeof(metkaz),nom_pole++,'|');
 }

vper:;
class spis_oth oth;
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_1_2_2.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case SFK1: /*Работа с шапкой документа*/

    vvodf8dr(nomd,vidd);

    goto nazad; //Перечитать шапку документа

  case FK2:
  case PLU:
    vkorz(0,nomd->ravno(),"","","",vidd,0);
    goto naz1;

  case SFK2: /*Корректировка записи*/
  case ENTER: /*Корректировка записи*/
    
    if(metkaz[0] == '\0')
     vkorz(1,nomd->ravno(),inn.ravno(),priz.ravno(),lgota.ravno(),vidd,priznak);
    else
     vkorz1(1,nomd->ravno(),inn.ravno(),priz.ravno(),lgota.ravno(),vidd,priznak);
      
    goto naz1;
  
  case FK3: /*Удалить запись в документе*/
    
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить запись в документе ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from F8dr1 where nomd='%s' and inn='%s' \
and priz='%s' and lgota='%s' and pr=%d",nomd->ravno(),inn.ravno(),priz.ravno(),lgota.ravno(),priznak);
     if(sql_zapis(strsql,0,0) != 0)
      {
       goto naz;
      }
      
     }

    goto naz1;

   case SFK3: /*Удаление всего документа*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

     sprintf(strsql,"delete from F8dr1 where nomd='%s'",nomd->ravno());
     if(sql_zapis(strsql,0,0) != 0)
        goto naz;

     sprintf(strsql,"delete from F8dr where nomd='%s'",nomd->ravno());
     if(sql_zapis(strsql,0,0) != 0)
       goto naz;

     sprintf(strsql,"delete from F1dfvs where nomd='%s'",nomd->ravno());
     sql_zapis(strsql,0,0);

      return;      

  case FK4:
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_MD(gettext("Фамилия................"));
    VV.VVOD_SPISOK_add_MD(gettext("Идентификационный код.."));
    VV.VVOD_SPISOK_add_MD(gettext("Код дохода (,,)........"));
    VV.VVOD_SPISOK_add_data(poi.fiop.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.innp.ravno(),32);
    VV.VVOD_SPISOK_add_data(poi.kod_doh.ravno(),128);
    
fk4:;
    poi.metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F10",gettext("выход"),NULL);

    kom1=VV.vvod(0,1,1,-1,-1);

    poi.fiop.new_plus(VV.VVOD_SPISOK_return_data(0));
    poi.innp.new_plus(VV.VVOD_SPISOK_return_data(1));
    poi.kod_doh.new_plus(VV.VVOD_SPISOK_return_data(2));

    switch(kom1)
     {
      case FK10:
      case ESC:
         break;                 
      case FK2:
      case PLU:
        poi.metpoi=1;
        pozz=1;
        break;
      default:
        goto fk4;
     }   

    goto naz1;
  
  case SFK4: /*Удвоение записей в документе*/
    dirf8dr_2zap(nomd->ravno(),&poi);
    goto naz1;

  case FK5: //Распечатать документ
    
    if(rasf8dr(nomd->ravno(),vidd,&oth) == 0)
       iceb_t_rabfil(&oth,"");

    goto naz;
    
  case FK6: /*Загрузка данных из файла*/
    dirf8dr_zif(nomd->ravno(),kolstr,kvrt);
    goto naz1;

  case FK7: /*Работа с военным сбором*/
    dirf1dfvs(nomd->ravno());
    goto naz1;
    
  case FK8:
    vkorz1(0,nomd->ravno(),"\0","\0","\0",vidd,priznak);
    goto naz1;

  case FK9:
    metkapr++;
    if(metkapr == 2)
     metkapr=0;
     
    goto naz;
    
  default:
   if(kolstr == 0)
     goto lll;
   else
     goto naz;
 }
 
}
/*****************************/
/*Ввод и корректировка записи*/
/*****************************/

void vkorz(int metka, //0-ввод новой записи 1-корректировка
const char *nomd,const char *inn,const char *priz,
const char *lgota,
int viddok, //0-отчётный 1-новый отчётный 2-уточняющий
int priznak) //0-новая запись 1-удаляющая запись
{
SQL_str         row;
SQLCURSOR cur;
char		strsql[1024];
time_t		vrem;
short		d1,m1,g1,d2,m2,g2;
int     	N=0,K=0;
class iceb_tu_str fio("");
class iceb_tu_str tabn("");
short		d,m,g;


class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(8);//4
VV.VVOD_SPISOK_add_data(11);//5
VV.VVOD_SPISOK_add_data(11);//6
VV.VVOD_SPISOK_add_data(4);//7
VV.VVOD_SPISOK_add_data(20);//8
VV.VVOD_SPISOK_add_data(10);//9
if(viddok == 1 || viddok == 2)
  VV.VVOD_SPISOK_add_data(2);//10

if(metka == 1)
 {
  SQLCURSOR cur;
  sprintf(strsql,"select * from F8dr1 where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' \
and pr=%d",
  nomd,inn,priz,lgota,priznak);
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    VV.VVOD_SPISOK_zapis_data(1,row[1]);
    VV.VVOD_SPISOK_zapis_data(2,row[2]);
    VV.VVOD_SPISOK_zapis_data(3,row[3]);
    VV.VVOD_SPISOK_zapis_data(4,row[4]);

    rsdat(&d,&m,&g,row[5],2);
    if(d != 0)
     {
      sprintf(strsql,"%02d.%02d.%d",d,m,g);
      VV.VVOD_SPISOK_zapis_data(5,strsql);
     }
    rsdat(&d,&m,&g,row[6],2);
    if(d != 0)
     {
      sprintf(strsql,"%02d.%02d.%d",d,m,g);
      VV.VVOD_SPISOK_zapis_data(6,strsql);
     }

    VV.VVOD_SPISOK_zapis_data(7,row[7]);

    VV.VVOD_SPISOK_zapis_data(8,row[11]);

    VV.VVOD_SPISOK_zapis_data(9,row[12]);
    if(viddok == 1 || viddok == 2)
      VV.VVOD_SPISOK_zapis_data(10,row[13]);

   }
  int kt=atoi(row[8]);
  vrem=atol(row[9]);

  //Читаем табельный номер
  sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",inn);
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    VV.VVOD_SPISOK_zapis_data(0,row[0]);
    tabn.new_plus(row[0]);
    fio.new_plus(row[1]);
   }  
  kzvz(kt,vrem,&VV);

 }

if(fio.ravno()[0] != '\0')
 VV.VVOD_SPISOK_add_ZAG(fio.ravno());

VV.VVOD_SPISOK_add_MD(gettext("Табельний номер..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Идентификационный номер..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма выплаченного дохода."));//2
VV.VVOD_SPISOK_add_MD(gettext("Выплаченный подох.налог..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Признак дохода............"));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата приема на работу....."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата увольнения с работы.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код льготы................"));//7
VV.VVOD_SPISOK_add_MD(gettext("Сумма начисленного дохода."));//8
VV.VVOD_SPISOK_add_MD(gettext("Начисленный подох.налог..."));//9
if(viddok == 1 || viddok == 2)
  VV.VVOD_SPISOK_add_MD(gettext("Признак (0,1)............."));//10



VV.VVOD_SPISOK_zapis_data(0,tabn.ravno());

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      fio.new_plus("");
      tabn.new_plus("");
  
      /*Читаем фамилию*/
      sprintf(strsql,"select tabn,fio,inn from Kartb where tabn=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        goto naz;
       }
      else
       {
        tabn.new_plus(row[0]);
        fio.new_plus(row[1]);
        if(row[2][0] == '\0')
         {
          sprintf(strsql,"%s !",gettext("Не введён индетификационный номер"));
          iceb_t_soob(strsql);
          goto naz;
         }
        if(strlen(row[2]) != 10)
         {
          iceb_t_soob(gettext("Количество знаков не равно 10 !"));
          goto naz;
         }
        VV.VVOD_SPISOK_zapis_data(1,row[2]);
       }   
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      fio.new_plus("");
      tabn.new_plus("");
  
      /*Читаем фамилию*/
      sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        goto naz;
       }
      else
       {
        tabn.new_plus(row[0]);
        VV.VVOD_SPISOK_zapis_data(0,row[0]);
        fio.new_plus(row[1]);
        goto naz;
       }   
     }

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }


   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

     GDITE();

     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0'
     || VV.VVOD_SPISOK_return_data(3)[0] == '\0' || VV.VVOD_SPISOK_return_data(4)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }
     d1=m1=g1=d2=m2=g2=0;
     if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
      if(rsdat(&d1,&m1,&g1,VV.VVOD_SPISOK_return_data(5),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала работы !"));
        goto naz;  
       }
     if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
      if(rsdat(&d2,&m2,&g2,VV.VVOD_SPISOK_return_data(6),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата увольнения !"));
        goto naz;  
       }

     int pr=0;
     if(viddok == 1 || viddok == 2)
        pr=atoi(VV.VVOD_SPISOK_return_data(10));

     if(pr != 0 && pr != 1)
      {
       iceb_t_soob(gettext("Неправильно ввели признак записи !"));
       goto naz;  
      }

     if(SRAV(inn,VV.VVOD_SPISOK_return_data(1),0) != 0 || \
     SRAV(lgota,VV.VVOD_SPISOK_return_data(7),0) != 0 || \
     SRAV(priz,VV.VVOD_SPISOK_return_data(4),0) != 0 || \
     priznak != pr)
      {
       sprintf(strsql,"select inn from F8dr1 where nomd='%s' and inn='%s' and priz='%s' \
and lgota='%s' and pr=%d",nomd,VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(4),
       VV.VVOD_SPISOK_return_data(7),pr);
       if(sql_readkey(&bd,strsql,&row,&cur) > 0)
        {
         iceb_t_soob(gettext("Такая запись уже есть !"));
         goto naz;  
        }
      }
     
     time(&vrem);

     if(metka == 0)
      sprintf(strsql,"insert into F8dr1 \
values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%d)",
      nomd,VV.VVOD_SPISOK_return_data(1),ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(3)),VV.VVOD_SPISOK_return_data(4),
      g1,m1,d1,g2,m2,d2,VV.VVOD_SPISOK_return_data(7),iceb_t_getuid(),vrem,"\0",
      ATOFM(VV.VVOD_SPISOK_return_data(8)),ATOFM(VV.VVOD_SPISOK_return_data(9)),pr);

     if(metka == 1)
      sprintf(strsql,"update F8dr1 set \
sumad=%.2f,\
suman=%.2f,\
priz='%s',\
datap='%04d-%d-%d',\
datau='%04d-%d-%d',\
lgota='%s',\
ktoz=%d,\
vrem=%ld,\
fio='%s',\
sumadn=%.2f,\
sumann=%.2f,\
pr=%d \
where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
      ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(3)),VV.VVOD_SPISOK_return_data(4),
      g1,m1,d1,g2,m2,d2,VV.VVOD_SPISOK_return_data(7),iceb_t_getuid(),vrem,"\0",
      ATOFM(VV.VVOD_SPISOK_return_data(8)),ATOFM(VV.VVOD_SPISOK_return_data(9)),pr,
      nomd,inn,priz,lgota,priznak);
//     printw("\n%s\n",strsql);
//     OSTANOV();

     if(sql_zapis(strsql,1,0) != 0)
       goto naz;

     break;
    }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

}




/******************/
/*  Шапка         */
/******************/

void sapka2f8dr(short god,int kvrt,const char *nomd,int vidd,
double isumad,double isuman,
int kolr, //Количество физических лиц
int metpoi, //0-поиск заказан 1-нет
int kolzap,
double isumadn,double isumann,short metkapr)
{

clear();
printw("%s:%s %s:%d %s:%d",gettext("Документ"),nomd,
gettext("Год"),god,
gettext("Квартал"),kvrt);

if(metkapr == 0)
 printw(" %s",gettext("Выплачено"));

if(metkapr == 1)
 printw(" %s",gettext("Начислено"));
 
if(metpoi == 1)
 {
  attron(A_BLINK);
  attron(A_BOLD);
  printw(" %s !",gettext("Поиск"));
  attroff(A_BLINK);
  attroff(A_BOLD);
 }

move(1,0);
printw("%s:%.2f %s:%.2f",
gettext("Сумма выплаченого дохода"),isumad,
gettext("Сумма выплаченого налога"),isuman);

move(2,0);
printw("%s:%.2f %s:%.2f",
gettext("Сумма начисленого дохода"),isumadn,
gettext("Сумма начисленого налога"),isumann);

move(3,0);
printw("%s:%d %s:%d",
gettext("Физических лиц"),kolr,
gettext("Количество записей"),kolzap);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("загруз."),
"F7",gettext("военный сбор"),
"F8",gettext("запись"),
"F9",gettext("пер."),
"F10",gettext("вых."),NULL);

}
/**********************************/
/*Определяем количество работников*/
/**********************************/

void kolrab(const char *nomd,//номер документа
int *kolr) //*Количество физических лиц*/
{
char strsql[512];
SQLCURSOR       cur;
sprintf(strsql,"select distinct inn from F8dr1 where nomd='%s'",nomd);

if((*kolr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

}

/**************************/
/*Проверка условий поиска*/
/**************************/
int		provf8dr(class dirf1df_poi *poi,
char inn1[],char fioz[],char *kod_doh)
{
SQL_str         row1;
SQLCURSOR cur1;
char		strsql[1024];

if(poi->metpoi == 0)
 return(0);



if(poi->fiop.getdlinna() > 1)
 {
  class iceb_tu_str fio1("");
  if(fioz[0] == '\0')
   {
    /*Читаем фамилию*/

    sprintf(strsql,"select fio from Kartb where inn='%s'",inn1);
    if(sql_readkey(&bd,strsql,&row1,&cur1) < 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),inn1);
      iceb_t_soob(strsql);
     }
    else
      fio1.new_plus(row1[0]);
   }
  else
     fio1.new_plus(fioz);

  if(strstrm(fio1.ravno(),poi->fiop.ravno()) == 0)
   return(1);
 }

if(poi->innp.getdlinna() > 1)
 if(strstrm(inn1,poi->innp.ravno()) == 0)
   return(2);
if(proverka(poi->kod_doh.ravno(),kod_doh,0,0) != 0)
  return(3);
  
 return(0);

}

/***************************************************/
/*Ввод и корректировка записи без табельного номера*/
/***************************************************/

void		vkorz1(int metka, //0-ввод новой записи 1-корректировка
const char *nomd,const char *inn,const char *priz,
const char *lgota,
int viddok,
int priznak)
{
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
time_t		vrem;
short		d1,m1,g1,d2,m2,g2;
int       	N,K;
short		d,m,g;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(50);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(4);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);
if(viddok == 1 || viddok == 2)
  VV.VVOD_SPISOK_add_data(2);

if(metka == 1)
 {
  sprintf(strsql,"select * from F8dr1 where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' \
and pr=%d",
  nomd,inn,priz,lgota,priznak);
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    VV.VVOD_SPISOK_zapis_data(0,row[10]);
    VV.VVOD_SPISOK_zapis_data(1,row[1]);
    VV.VVOD_SPISOK_zapis_data(2,row[2]);
    VV.VVOD_SPISOK_zapis_data(3,row[3]);
    VV.VVOD_SPISOK_zapis_data(4,row[4]);

    rsdat(&d,&m,&g,row[5],2);
    if(d != 0)
     {
      sprintf(strsql,"%02d.%02d.%d",d,m,g);
      VV.VVOD_SPISOK_zapis_data(5,strsql);
     }

    rsdat(&d,&m,&g,row[6],2);
    if(d != 0)
     {
      sprintf(strsql,"%02d.%02d.%d",d,m,g);
      VV.VVOD_SPISOK_zapis_data(6,strsql);
     }
    VV.VVOD_SPISOK_zapis_data(7,row[7]);

    VV.VVOD_SPISOK_zapis_data(8,row[11]);

    VV.VVOD_SPISOK_zapis_data(9,row[12]);
    if(viddok == 1 || viddok == 2)
      VV.VVOD_SPISOK_zapis_data(10,row[13]);

    int kt=atoi(row[8]);
    vrem=atol(row[9]);
    kzvz(kt,vrem,&VV);
   }

 }

N=K=0;

VV.VVOD_SPISOK_add_MD(gettext("Фамилия, имя, отчество...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Идентификационный номер..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма выплаченного дохода."));//2
VV.VVOD_SPISOK_add_MD(gettext("Выплаченный подох.налог..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Признак дохода............"));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата приема на работу....."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата увольнения с работы.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код льготы................"));//7
VV.VVOD_SPISOK_add_MD(gettext("Сумма начисленного дохода."));//8
VV.VVOD_SPISOK_add_MD(gettext("Начисленный подох.налог..."));//9
if(viddok == 1 || viddok == 2)
  VV.VVOD_SPISOK_add_MD(gettext("Признак (0,1)............."));//10

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      if(strlen(VV.VVOD_SPISOK_return_data(N)) != 10)
       {
        attroff(VV.VVOD_return_cs(iceb_CFS));
        attron(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Количество знаков в идентификационном коде не равно 10 !"));
       }

     }
     
     
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }


   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0'
     || VV.VVOD_SPISOK_return_data(3)[0] == '\0' || VV.VVOD_SPISOK_return_data(4)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }
     d1=m1=g1=d2=m2=g2=0;
     if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
      if(rsdat(&d1,&m1,&g1,VV.VVOD_SPISOK_return_data(5),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала работы !"));
        goto naz;  
       }
     if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
      if(rsdat(&d2,&m2,&g2,VV.VVOD_SPISOK_return_data(6),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата увольнения !"));
        goto naz;  
       }
     if(strlen(VV.VVOD_SPISOK_return_data(1)) != 10)
      {
       iceb_t_soob(gettext("Количество знаков в идентификационном коде не равно 10 !"));
      }
     
     if(SRAV(inn,VV.VVOD_SPISOK_return_data(1),0) != 0 || \
     SRAV(lgota,VV.VVOD_SPISOK_return_data(7),0) != 0 || \
     SRAV(priz,VV.VVOD_SPISOK_return_data(4),0) != 0)
      {
       sprintf(strsql,"select inn from F8dr1 where nomd='%s' and inn='%s' and priz='%s' \
and lgota='%s' and pr=%d",nomd,VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(4),
       VV.VVOD_SPISOK_return_data(7),priznak);
       if(readkey(strsql) > 0)
        {
         iceb_t_soob(gettext("Такая запись уже есть !"));
         goto naz;  
        }
      }

     GDITE();
     time(&vrem);
     int pr=0;
     if(viddok == 1 || viddok == 2)
        pr=atoi(VV.VVOD_SPISOK_return_data(10));
     if(pr != 0 && pr != 1)
      {
       iceb_t_soob(gettext("Неправильно ввели признак записи !"));
       goto naz;  
      }


     if(metka == 0)
      sprintf(strsql,"insert into F8dr1 \
values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%d)",
      nomd,VV.VVOD_SPISOK_return_data(1),ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(3)),VV.VVOD_SPISOK_return_data(4),
      g1,m1,d1,g2,m2,d2,VV.VVOD_SPISOK_return_data(7),iceb_t_getuid(),vrem,VV.VVOD_SPISOK_return_data_filtr(0),
      ATOFM(VV.VVOD_SPISOK_return_data(8)),ATOFM(VV.VVOD_SPISOK_return_data(9)),pr);

     if(metka == 1)
      sprintf(strsql,"update F8dr1 set \
inn='%s',\
sumad=%.2f,\
suman=%.2f,\
priz='%s',\
datap='%04d-%d-%d',\
datau='%04d-%d-%d',\
lgota='%s',\
ktoz=%d,\
vrem=%ld,\
fio='%s',\
sumadn=%.2f,\
sumann=%.2f,\
pr=%d \
where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
      VV.VVOD_SPISOK_return_data(1),ATOFM(VV.VVOD_SPISOK_return_data(2)),ATOFM(VV.VVOD_SPISOK_return_data(3)),VV.VVOD_SPISOK_return_data(4),
      g1,m1,d1,g2,m2,d2,VV.VVOD_SPISOK_return_data(7),iceb_t_getuid(),vrem,VV.VVOD_SPISOK_return_data_filtr(0),
      ATOFM(VV.VVOD_SPISOK_return_data(8)),ATOFM(VV.VVOD_SPISOK_return_data(9)),pr,
      nomd,inn,priz,lgota,priznak);
//     printw("\n%s\n",strsql);
//     OSTANOV();

     if(sql_zapis(strsql,1,0) != 0)
       goto naz;

     break;
    }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

}
/***************************/
/*удвоение записей с противоположным признаком записи*/
/********************************************/
void dirf8dr_2zap(const char *nomd,class dirf1df_poi *poi)
{
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD("Удвоить записи? Вы уверены?");

if(danet(&DANET,2,stdscr) == 2)
 return;

GDITE();
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
sprintf(strsql,"select * from F8dr1 where nomd='%s' order by inn asc,pr desc",nomd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int priznak=0;
int kto=iceb_t_getuid();
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(provf8dr(poi,row[0],row[7],row[3]) != 0)
      continue;
  
  if(atoi(row[13]) == 0)
   priznak=1;
  else
   priznak=0;
  
  sprintf(strsql,"insert into F8dr1 values ('%s','%s',%s,%s,'%s','%s','%s','%s',%d,%ld,'%s',%s,%s,%d)",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],kto,time(NULL),row[10],row[11],row[12],priznak);
  
  
  sql_zapis(strsql,1,0);
  

 }

}
/**********************************************/
/*загрузка из файла*/
/**********************************************/
int dirf8dr_zif(const char *nomd,int kolstr,int kvrt)
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Загрузка данных из файла"));

MENU.VVOD_SPISOK_add_MD(gettext("Загрузка данных из файла в формате csv"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Загрузка данных из файла в формате dbf"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  switch (kom)
   {
    case 2:
    case -1 :
      return(1);

    case 0:
     return(zagf1df_csv(nomd,kolstr));


    case 1:
      return(zagf8dr1(nomd,kolstr,kvrt));

    default:
      kom=0;
      break;
   }
 }
return(0);
}
