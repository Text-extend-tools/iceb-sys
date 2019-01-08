/*$Id: dirzardk.c,v 5.34 2013/09/26 09:43:31 sasa Exp $*/
/*28.03.2018	29.05.2006	Белых А.И.	dirzardk.c
Работа со списком документов в подсистеме "Заработная плата"
*/
#include <errno.h>
#include        "buhg.h"

class poi_dirzardk
 {
  public:
  class iceb_tu_str datan;
  class iceb_tu_str datak;
  class iceb_tu_str datansd;
  class iceb_tu_str dataksd;
  class iceb_tu_str nomdok;
  class iceb_tu_str podr;
  class iceb_tu_str koment;
  short metka_poi; //0-без поиска 1- с поиском
  
  poi_dirzardk()
   {
    clear_data();
   }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    datansd.new_plus("");
    dataksd.new_plus("");
    podr.new_plus("");
    nomdok.new_plus("");
    koment.new_plus("");
    metka_poi=0;
   }
 };

int dirzardk_prov(SQL_str row,class poi_dirzardk *poisk);
void dirzardk_poi(class poi_dirzardk *poisk);
void dirzardk_sap(class poi_dirzardk*);
int zapzardk(short dd,short md,short gd,const char *nomdok,int podr,const char *koment,short prn,short dsd,short msd,short gsd);
void dirzardk_ras(SQLCURSOR *cur,int kolstr,class poi_dirzardk *poisk);

extern short	startgodz; /*Стаpтовый год для расчёта сальдо*/

void dirzardk()
{
long pozz=0,pozz1=0;
int  x=0,y=3; /*Точка начала вывода меню*/
int  kom=0,kom1=0;
int K=0;
SQLCURSOR cur;
SQL_str row;
SQLCURSOR cur1;
SQL_str row1;
char strsql[1024];
int kolstr;
class iceb_t_mstr spis_menu(0);
short dn=1,mn=1,gn=startgodz;
short dk=0,mk=0,gk=0;
short dsd=0,msd=0,gsd=0;
short dd,md,gd;
short d,m,g;
short dv=0,mv=0,gv=0; //Дата только что введённого документа
class iceb_tu_str nomdok_v(""); //номер документа только что введённого
short mspz; /*метка соответствия первой записи*/
int prc=0;
class iceb_tu_str nomdok("");
double suma_dok=0.;
class poi_dirzardk poisk;
short prn=0;
class iceb_tu_str zapros("");
short god_vz=0; /*год в выбранной записи*/

if(gn == 0)
 {
  poltekdat(&d,&m,&g);
  gn=g;
 }
VVOD MENU(3);

naz1:;

clear();

int             kls=LINES-y-3; /*Количество строк в окне*/
int god_dok[kls];

memset(strsql,'\0',sizeof(strsql));
if(dk != 0)
  sprintf(strsql,"select * from Zardok where (datsd >='%04d-%02d-%02d' and datsd <= '%04d-%02d-%02d')",gn,mn,dn,gk,mk,dk);
else
  sprintf(strsql,"select * from Zardok where datsd >='%04d-%02d-%02d'",gn,mn,dn);

zapros.new_plus(strsql);
memset(strsql,'\0',sizeof(strsql));

if(poisk.metka_poi == 1)
 {
  if(poisk.datak.getdlinna() > 1)
   {
    sprintf(strsql," and (datd >='%s' and datd <= '%s')",poisk.datan.ravno_sqldata(),poisk.datak.ravno_sqldata());
    zapros.plus(strsql);
   }
  else
   {
    if(poisk.datan.getdlinna() > 1)
     sprintf(strsql," and datd >='%s'",poisk.datan.ravno_sqldata());
    zapros.plus(strsql);
   }
 }
zapros.plus(" order by datsd,nomd desc");

if((kolstr=cur.make_cursor(&bd,zapros.ravno())) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

   dirzardk_sap(&poisk);
 
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
    K=soobshw(&SOOB,stdscr,-1,-1,1,1);

    if(K == FK3 || K == SFK3 || K == FK5 || K == ENTER)
      goto lll;
    if(K == KEY_RESIZE)
     goto naz1;
           
    goto vper;

 }

if(dv != 0) //поиск только что введённого документа
 {
  poisk.metka_poi=0;
  int nomer_str=0;
  while(cur.read_cursor(&row) != 0)
   {
    rsdat(&d,&m,&g,row[1],2);
    if(sravmydat(d,m,g,dv,mv,gv) == 0  && SRAV(nomdok_v.ravno(),row[2],0) == 0)
     {
      pozz=nomer_str;
      break;
     }
    nomer_str++;
   }
 }
dv=mv=gv=0;
nomdok_v.new_plus("");

naz:;
GDITE();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
int i;
spis_menu.spis_str.free_class();
char metka_zap[4];
for(i=0; i<kls ; )
 {
  if(cur.read_cursor(&row) == 0)
    break;


  pozz1++;

  if(dirzardk_prov(row,&poisk) != 0)
   continue;

  rsdat(&d,&m,&g,row[1],2);
  
  god_dok[i]=atoi(row[0]);
  
  suma_dok=0.;
  //Узнаём сумму по документа
  sprintf(strsql,"select SUM(suma) from Zarp where datz='%s' and nd='%s'",row[1],row[2]);
  
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(row1[0] != NULL)
     suma_dok=atof(row1[0]);
   }
  memset(metka_zap,'\0',sizeof(metka_zap));
  
  if(row[7][0] == '2')
   strcpy(metka_zap,"-");
  else
   strcpy(metka_zap,"+");  /*именно так потому что были документы 0 до доработки*/
   
  sprintf(strsql,"%s|%02d.%02d.%04d|%-*s|%8.2f|%-4s|%s",
  iceb_tu_datzap(row[8]),
  d,m,g,
  iceb_tu_kolbait(4,row[2]),row[2],
  suma_dok,row[3],metka_zap);

  spis_menu.spis_str.plus(strsql);
  i++;


  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));
if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }



dirzardk_sap(&poisk);

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

    if(dirzardk_prov(row,&poisk) != 0)
     continue;

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


    if(dirzardk_prov(row,&poisk) != 0)
     continue;

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


    if(dirzardk_prov(row,&poisk) != 0)
     continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),strsql,sizeof(strsql),1,'|');
  rsdat(&dsd,&msd,&gsd,strsql,1);

  polen(str_vib.ravno(),strsql,sizeof(strsql),2,'|');
  rsdat(&dd,&md,&gd,strsql,1);
  polen(str_vib.ravno(),&nomdok,3,'|');
  polen(str_vib.ravno(),metka_zap,sizeof(metka_zap),6,'|');
  prn=0;
  if(metka_zap[0] == '+')
   prn=1;
  if(metka_zap[0] == '-')
   prn=2;
  god_vz=god_dok[kom];      
 } 

vper:;

switch(K)
 {

  case ENTER:
   dirzardkz(prn,god_vz,dd,md,gd,&nomdok);
   goto naz1;

  case ESC:
  case FK10:
    GDITE();
    return;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_11.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case PLU: //ввод нового документа
  case FK2: //ввод нового документа
  
  
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Ввод начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Ввод удержаний"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 0 :
        prn=1;
        break;

      case 1 :
        prn=2;
        break;
     }
  
    dv=mv=gv=0;
    nomdok_v.new_plus("");  

    dirzardk_vvod(prn,&dv,&mv,&gv,&nomdok_v);

    poisk.metka_poi=0;

    goto naz1;

  case FK4: //поиск
    dirzardk_poi(&poisk);
    if(poisk.metka_poi == 1)
     {
      if(poisk.datansd.getdlinna() > 1)
        rsdat(&dn,&mn,&gn,poisk.datansd.ravno(),1);
      if(poisk.dataksd.getdlinna() > 1)
        rsdat(&dk,&mk,&gk,poisk.dataksd.ravno(),1);
     }
    else
     {
      dn=1; mn=1; gn=startgodz;
      dk=0; mk=0; gk=0;
     }
    pozz=0;
    goto naz1;


  case FK5: //печать
    dirzardk_ras(&cur,kolstr,&poisk);
    goto naz;
      
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

}
/**************/
/*Выдача шапки*/
/**************/

void dirzardk_sap(class poi_dirzardk *poisk)
{
int		Y,X;

move(0,0);
printw("%s %s %d",gettext("Просмотр списка документов"),gettext("Стартовый год"),startgodz);

move(2,0);
printw("      1          2       3      4      5    6");

mvprintw(Y=1,X=COLS-20,gettext("Расшифровка полей:"));
mvprintw(++Y,X,"1-%s",gettext("дата документа"));
mvprintw(++Y,X,"2-%s",gettext("дата начисления"));
mvprintw(++Y,X,"3-%s",gettext("ном.док"));
mvprintw(++Y,X,"4-%s",gettext("сумма"));
mvprintw(++Y,X,"5-%s",gettext("код подразделения"));
mvprintw(++Y,X,"6-%s",gettext("+ нач., - удер."));

if(poisk->metka_poi == 1)
 {
//  attron(A_BLINK);
  mvprintw(Y+=2,X,gettext("Поиск:"));
  if(poisk->datansd.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Дата н.д."),poisk->datansd.ravno());
  if(poisk->dataksd.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Дата к.д."),poisk->dataksd.ravno());
  if(poisk->datan.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Дата н.н."),poisk->datan.ravno());
  if(poisk->datak.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Дата к.н."),poisk->datak.ravno());
  if(poisk->nomdok.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Документ"),poisk->nomdok.ravno());
  if(poisk->podr.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Подр."),poisk->podr.ravno());
  if(poisk->koment.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%.12s",gettext("Комент."),poisk->koment.ravno());

//  attroff(A_BLINK);
  
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("ввод"),
"F4",gettext("поиск"),
"F10",gettext("выход"),NULL);
}




/**************************/
/*Ввод и корректировка документа*/
/********************************/
//Если вернули 0- записали 1-нет
int dirzardk_vvod(short prn,short *dd,short *md,short *gd,class iceb_tu_str *nomdk)
{
char strsql[1024];
class iceb_tu_str kod("");
class iceb_tu_str naim("");
short d,m,g;
SQLCURSOR cur;
SQL_str row;
static class iceb_tu_str dat1("");
short ddv=0,mdv=0,gdv=0; /*дата записи в карточки*/
short dsd=0,msd=0,gsd=0; /*дата создания документа*/
class iceb_tu_str data_sd("");
VVOD VV(0);

VV.VVOD_SPISOK_add_data(11); //0 дата создания документа
VV.VVOD_SPISOK_add_data(11); //1 дата документа начислений-удержаний
VV.VVOD_SPISOK_add_data(10); //2 номер документа
VV.VVOD_SPISOK_add_data(10); //3 подразделение
VV.VVOD_SPISOK_add_data(100);//4 комментарий

if(nomdk->getdlinna() > 1)
 {
  sprintf(strsql,"select * from Zardok where nomd='%s' and datd='%04d-%02d-%02d'",nomdk->ravno(),*gd,*md,*dd);
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено запись для корректировки !"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,1,1);
  
    return(1);
   }  
  rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"%d.%d.%d",d,m,g);
  data_sd.new_plus(strsql);
  VV.data_plus(0,strsql);
  VV.data_plus(1,row[2]);
  VV.data_plus(2,row[3]);
  VV.data_plus(3,row[4]);
  VV.data_plus(4,iceb_tu_datzap(row[8]));
    
  time_t vrem=atol(row[6]);
    
  kzvz(atoi(row[5]),vrem,&VV);
 }
else
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  
  if(dat1.ravno()[0] == '\0')
   {
    sprintf(strsql,"%d.%d.%d",dt,mt,gt);
    dat1.new_plus(strsql);
   }
  VV.data_plus(0,dat1.ravno());

  sprintf(strsql,"%d",iceb_t_nomdok(gt,"Zardok"));
  VV.data_plus(2,strsql);
  
 }

VV.VVOD_SPISOK_add_MD(gettext("Дата документа.(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата начисления.(д.м.г)."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.............."));//4


naz:;
    

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("подразделения"),
"F4",gettext("ном.док."),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

GDITE();

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_11_1.txt");
   goto naz;

  case FK2:
  case PLU:
     
    break;

  case FK3:
    
    if(dirzarpodr(2,&kod,&naim) == 0)
     VV.data_plus(3,kod.ravno());
    
    goto naz;

  case FK4: //получение нового номера документа
    if(rsdat(&ddv,&mdv,&gdv,VV.data_ravno(0),1) != 0)
     {
      iceb_t_soob(gettext("Неправильно введена дата !"));
      goto naz;
     }
    
    sprintf(strsql,"%d",iceb_t_nomdok(gdv,"Zardok"));
    VV.data_plus(2,strsql);
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
   goto naz;
 }



if(rsdat(&ddv,&mdv,&gdv,VV.data_ravno(1),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата документа !"));
  goto naz;
 }
dsd=msd=gsd=0;

if(rsdat(&dsd,&msd,&gsd,VV.data_ravno(0),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата начисления !"));
  goto naz;
 }


/*проверяем новую дату начислений*/
if(dirnuprovb(mdv,gdv,0) != 0)
   goto naz;

if(*dd != 0)
 {
  /*проверяем старую дату начислений*/
  if(dirnuprovb(*md,*gd,0) != 0)
     goto naz;
 }

/*проверяем новую дату документа*/
if(iceb_t_pbpds(msd,gsd) != 0)
 goto naz;

/*проверяем старую дату документа*/
if(data_sd.getdlinna() > 1)
 if(iceb_t_pbpds(data_sd.ravno()) != 0)
  goto naz;

//проверяем подразделение
if(VV.data_ravno_atoi(3) != 0)
 {
  sprintf(strsql,"select kod from Podr where kod=%d",VV.data_ravno_atoi(3));
  if(readkey(strsql) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),VV.data_ravno_atoi(3));
    iceb_t_soob(strsql);
    goto naz;
   }
 }
 
class lock_tables blok_tabl("LOCK TABLE Zardok WRITE,icebuser READ");
if(*gd != gdv || SRAV(nomdk->ravno(),VV.data_ravno(1),0) != 0 )
 {  
  //Проверяем можен документ с таким номером уже есть 
  sprintf(strsql,"select god from Zardok where nomd='%s' and god=%d",VV.data_ravno(2),gsd);
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Документ с таким номером уже есть !"));
    goto naz;
   }
 }

if(*dd == 0) //ввод новой записи
 {
  //фильтрование внутри подпрограммы
  if(zapzardk(ddv,mdv,gdv,VV.data_ravno(2),VV.data_ravno_atoi(3),VV.data_ravno(4),prn,dsd,msd,gsd) != 0)
   goto naz;
 }
else //корректировка старой
 {
  time_t vrem;
  time(&vrem);
  
  char koment1[strlen(VV.data_ravno(4))*2];
  strcpy(koment1,VV.data_ravno(4));
  sqlfiltr(koment1,sizeof(koment1));
  
  sprintf(strsql,"update Zardok set \
god=%d,\
datd='%04d-%02d-%02d',\
nomd='%s',\
pd=%d,\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
td=%d,\
datsd='%04d-%02d-%02d' \
where god=%d and nomd='%s'",
  gsd,
  gdv,mdv,ddv,
  VV.data_ravno(2),
  VV.data_ravno_atoi(3),
  koment1,
  iceb_t_getuid(),
  vrem,
  prn,
  gsd,msd,dsd,
  *gd,nomdk->ravno());  


  if(sql_zapis(strsql,0,0) != 0)
   goto naz;
  
  blok_tabl.unlock();
  
  //Корректируем записи в документе
  if(sravmydat(*dd,*md,*gd,ddv,mdv,gdv) != 0 || SRAV(nomdk->ravno(),VV.data_ravno(1),0) != 0 )
   {
    sprintf(strsql,"select datz,tabn,prn,knah,godn,mesn,podr,shet,nomz from Zarp where \
datz='%04d-%02d-%02d' and nd='%s' and prn='%d'", *gd,*md,*dd,nomdk->ravno(),prn);
    int kolstr=0;
    
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    if(kolstr > 0)
     while(cur.read_cursor(&row) != NULL)
      {

       for(int nomer=atoi(row[8]); nomer < 1000; nomer++)
        {          
         sprintf(strsql,"update Zarp set \
datz='%04d-%02d-%02d',\
nd='%s',\
nomz=%d \
where datz='%s' and tabn=%s and prn='%s' and knah=%s and godn=%s and mesn=%s \
and podr=%s and shet='%s' and nomz=%s",
         gdv,mdv,ddv,
         VV.data_ravno(1),
         nomer,
         row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);

         
         if(sql_zapis(strsql,1,1) == 0)
          break; 
        }

      }


   if(sravmydat(*dd,*md,*gd,ddv,mdv,gdv) != 0 ) //Корректируем проводки
    {
     //дата и номер документа уже изменены
     sprintf(strsql,"select distinct tabn from Zarp where datz='%04d-%02d-%02d' and nd='%s' and prn='%d'",
     gdv,mdv,ddv,VV.data_ravno(1),prn);

     if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
     if(kolstr > 0)
      {
       while(cur.read_cursor(&row) != 0)
        {          
         zarsoc(mdv,gdv,atoi(row[0]));
         zaravpr(atol(row[0]),mdv,gdv,NULL);
         if(*md != mdv || *gd != gdv)
          {
           zarsoc(*md,*gd,atoi(row[0]));
           zaravpr(atol(row[0]),*md,*gd,NULL);
          }
        }
      }         
    }

   }
 }
*dd=ddv;
*md=mdv;
*gd=gdv;
nomdk->new_plus(VV.data_ravno(1));
return(0);
}

/****************************/
/*Поиск*/
/***************************/
void dirzardk_poi(class poi_dirzardk *poisk)
{
short ddv=0,mdv=0,gdv=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
VVOD VV(0);

VV.data_sp_plus(poisk->datansd.ravno(),11);//0
VV.data_sp_plus(poisk->dataksd.ravno(),11);//1
VV.data_sp_plus(poisk->datan.ravno(),11);//2
VV.data_sp_plus(poisk->datak.ravno(),11);//3
VV.data_sp_plus(poisk->nomdok.ravno(),128);//4
VV.data_sp_plus(poisk->podr.ravno(),128);//5
VV.data_sp_plus(poisk->koment.ravno(),128);//6

VV.VVOD_SPISOK_add_ZAG(gettext("Поиск документов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала (документы)..(д.м.г)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца (документы)...(д.м.г)."));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата начала (начисления).(д.м.г)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата конца (начисления)..(д.м.г)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Номер документа............(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..........(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.................(,,).."));//6


naz:;
    

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("пошук"),
"F3",gettext("подразделения"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

GDITE();

poisk->datansd.new_plus(VV.data_ravno(0));
poisk->dataksd.new_plus(VV.data_ravno(1));
poisk->datan.new_plus(VV.data_ravno(2));
poisk->datak.new_plus(VV.data_ravno(3));
poisk->nomdok.new_plus(VV.data_ravno(4));
poisk->podr.new_plus(VV.data_ravno(5));
poisk->koment.new_plus(VV.data_ravno(6));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_11_2.txt");
   goto naz;

  case FK2:
  case PLU:

    if(poisk->datan.ravno()[0] != '\0')
    if(rsdat(&ddv,&mdv,&gdv,poisk->datan.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Неправильно введена дата начала (начисления)!"));
      goto naz;
     }

    if(poisk->datak.ravno()[0] != '\0')
    if(rsdat(&ddv,&mdv,&gdv,poisk->datak.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Неправильно введена дата конца (начисления)!"));
      goto naz;
     }
    
    if(poisk->datansd.ravno()[0] != '\0')
    if(rsdat(&ddv,&mdv,&gdv,poisk->datansd.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Неправильно введена дата начала (документы)!"));
      goto naz;
     }

    if(poisk->dataksd.ravno()[0] != '\0')
    if(rsdat(&ddv,&mdv,&gdv,poisk->dataksd.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Неправильно введена дата конца (документы)!"));
      goto naz;
     }
    
    poisk->metka_poi=1;     
    return;

  case FK3:
    if(dirzarpodr(2,&kod,&naim) == 0)
     VV.data_z_plus(5,kod.ravno());
    
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
    
  case FK10:
  case ESC:
    poisk->metka_poi=0;
    return;

  default:
   goto naz;
 }






}

/************************************/
/*Проверка на условия поиска*/
/****************************/

int dirzardk_prov(SQL_str row,class poi_dirzardk *poisk)
{
if(poisk->metka_poi == 0)
 return(0);
 
if(proverka(poisk->nomdok.ravno(),row[2],4,0) != 0)
 return(1);

if(proverka(poisk->koment.ravno(),row[4],4,0) != 0)
 return(1);

if(proverka(poisk->podr.ravno(),row[3],0,0) != 0)
 return(1);

return(0);

}

/**************************/
/*распечатка списка нарядов*/
/**************************/
void dirzardk_ras(SQLCURSOR *cur,int kolstr,class poi_dirzardk *poisk)
{

FILE *ff;
char imaf[56];
sprintf(imaf,"snar%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
iceb_t_zagolov(gettext("Распечатка списка документов"),0,0,0,0,0,0,ff);


fprintf(ff,"\
------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Дата док. |Дата начи.|Номер д.|   Сумма    |Код п.|Наименование подразделения\n"));
/*
1234567890 12345678 1234567890 123456
*/

fprintf(ff,"\
------------------------------------------------------------------------------\n");

class iceb_tu_str naim_podr("");
double suma_dok=0.;
char strsql[512];
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
short d,m,g;
int kolstr1=0;
double itogo=0.;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != NULL)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(dirzardk_prov(row,poisk) != 0)
   continue;

  rsdat(&d,&m,&g,row[1],2);

  naim_podr.new_plus("");
  sprintf(strsql,"select naik from Podr where kod=%s",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_podr.new_plus(row1[0]);
   
   
  
  suma_dok=0.;
  //Узнаём сумму по документа
  sprintf(strsql,"select SUM(suma) from Zarp where datz='%s' and nd='%s'",row[1],row[2]);
  
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(row1[0] != NULL)
     suma_dok=atof(row1[0]);
   }

  fprintf(ff,"%s %02d.%02d.%04d %-8s %12.2f %-6s %s\n",iceb_tu_datzap(row[8]),d,m,g,row[2],suma_dok,row[3],naim_podr.ravno());
  itogo+=suma_dok;
 }

fprintf(ff,"\
-------------------------------------------------------------------\n");

fprintf(ff,"\
%35s %12.2f\n",gettext("Итого"),itogo);

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка спискa документов"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
