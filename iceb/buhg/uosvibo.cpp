/*$Id: uosvibo.c,v 5.11 2013/09/26 09:43:42 sasa Exp $*/
/*16.04.2017	31.01.2010	Белых А.И.	uosvibo.c
Расчёт ведомости износа для бюджетных организаций
*/
#include <math.h>
#include        <errno.h>
#include "buhg.h"
#include "poiinpd.h"

class uosvibo_poi
 {
  public:
  class iceb_tu_str god;
  class iceb_tu_str podrz; /*Код подразделения*/
  class iceb_tu_str kodolz; /*Коды ответственных лиц*/
  class iceb_tu_str hnaz;
  class iceb_tu_str hnabyz;
  class iceb_tu_str hauz;
  class iceb_tu_str shetupoi;
  class iceb_tu_str innomp;
  class iceb_tu_str hztz;
  class iceb_tu_str sosob;
  class iceb_tu_str kod_op;

  uosvibo_poi()
   {
    clear();
   }
  void clear()
   {
    god.new_plus("");
    podrz.new_plus("");
    kodolz.new_plus("");
    hnaz.new_plus("");
    hnabyz.new_plus("");
    hauz.new_plus("");
    shetupoi.new_plus("");
    innomp.new_plus("");
    hztz.new_plus("");
    sosob.new_plus("");
    kod_op.new_plus("");
   }
 };
void uosvibo_start(class uosvibo_poi *rp,int metr,int metka_sort,int *kst,int metkaost,FILE *ff);
void uosvibo_itogo(const char *naim_itogo,double *itogo,FILE *ff);


int uosvibo(class spis_oth *oth)
{
char imaf_prot[64];
FILE *ff_prot=NULL;
class iceb_tu_str hna1("");
short		mvsh=0; /*0-шапку не печатать, если больше печатать*/
short		mvsi=0; /*0-шифр не печатать*/
class iceb_tu_str naigrup("");
class iceb_tu_str naipod("");
class iceb_tu_str naimos("");
int pod=0,podrp=0,podz=0;
int poddz=0,kodotl=0;
char strsql[512];
short dn,mn,gn,dk,mk,gk;
int           kom,kom1;
static class uosvibo_poi rp;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(rp.god.ravno(),5);
VV.VVOD_SPISOK_add_data(rp.hnaz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.hnabyz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.podrz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.kodolz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.hauz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.shetupoi.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.innomp.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.hztz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp.sosob.ravno(),2);

VV.VVOD_SPISOK_add_MD(gettext("Год....................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Группа нал.учета..(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Группа бух.учета..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Подразделение (,,)......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный (,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Шифр анал. учета..(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.(,,).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат.......(,,).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Состояние объекта.(+/-)."));//9

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт ведомости начисления износа для бюджетных организаций"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rp.god.new_plus(VV.VVOD_SPISOK_return_data(0));

rp.hnaz.new_plus(VV.VVOD_SPISOK_return_data(1));

rp.hnabyz.new_plus(VV.VVOD_SPISOK_return_data(2));

rp.podrz.new_plus(VV.VVOD_SPISOK_return_data(3));
rp.kodolz.new_plus(VV.VVOD_SPISOK_return_data(4));
rp.hauz.new_plus(VV.VVOD_SPISOK_return_data(5));
rp.shetupoi.new_plus(VV.VVOD_SPISOK_return_data(6));
rp.innomp.new_plus(VV.VVOD_SPISOK_return_data(7));
rp.hztz.new_plus(VV.VVOD_SPISOK_return_data(8));
rp.sosob.new_plus(VV.VVOD_SPISOK_return_data(9));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch(kom)
 {
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:  /*Просмотр реквизитов*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственны"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учета"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uosgrup",&rp.hnaz);
        VV.VVOD_SPISOK_zapis_data(1,rp.hnaz.ravno());
        goto naz;

      case 1 :
        vibrek("Uosgrup1",&rp.hnabyz);
        VV.VVOD_SPISOK_zapis_data(2,rp.hnabyz.ravno());
        goto naz;

      case 2 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(3,kod_v.ravno());
        goto naz;

      case 3 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(4,kod_v.ravno());
        goto naz;

      case 4 :
        vibrek("Uoshau",&rp.hauz);
        VV.VVOD_SPISOK_zapis_data(5,rp.hauz.ravno());
        goto naz;

      case 5 :
        vibrek("Uoshz",&rp.hztz);
        VV.VVOD_SPISOK_zapis_data(8,rp.hztz.ravno());
        goto naz;
     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

time_t tmmn=time(NULL);

dn=1;mn=1;gn=rp.god.ravno_atoi();
dk=31;mk=12;gk=gn;

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по подразделениям"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по мат.-ответственным"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int metr=0;
while(menu3w(stdscr,&MENU,&metr,-1,-1,0) != 0);
if(metr == -1 || metr == 2)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Все основные средства"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с нулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с ненулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

int metkaost=0;

while(menu3w(stdscr,&MENU,&metkaost,-1,-1,0) != 0);

if(metkaost == -1 || metkaost == 3)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по группам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по счетам учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int metka_sort=0;
while(menu3w(stdscr,&MENU,&metka_sort,-1,-1,0) != 0);
if(metka_sort == -1 || metka_sort == 2)
 return(1);
 
clear();
refresh();
printw(" %d.%d.%d%s\n",
dk,mk,gk,
gettext("г."));
printw("%s\n",gettext("Сортируем"));

GDITE(); 
int kolstr;
SQL_str row,row1;
SQLCURSOR cur,curr;

sprintf(strsql,"select innom,naim from Uosin");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);  
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosvibo"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
hna char(64) not null,\
shetu char(64) not null,\
podr int not null,\
innom int not null,\
naim char(255) not null,\
kodmo int not null,\
nomz char(255) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

char pole1[64];
int kolstr3=0,kolstr1=0;
class SQLCURSOR cur1;
int in=0;
kolstr1=0;
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);  
  if(proverka(rp.innomp.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atoi(row[0]);

  if(poiin(in,dk,mk,gk,&poddz,&kodotl) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода
  
  if(proverka(rp.podrz.ravno(),poddz,0,0) != 0)
    continue;

  if(proverka(rp.kodolz.ravno(),kodotl,0,0) != 0)
    continue;

//  poiinpd(in,mk,gk,shetu,hzt,hau,hna,&popkf,&soso,nomz,&m1,&g1,hnaby,&popkfby);

  poiinpd(in,mk,gk,&rekin);

  if(uosprovgr(rekin.hna.ravno()) == 1) /*Пропускаем все основные средства если для них амортизазия считается в целом по группе*/
    continue;

  if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
     if(rp.sosob.ravno()[0] == '-')
      continue;
  if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаетcя
     if(rp.sosob.ravno()[0] == '+')
      continue;
    
  if(proverka(rp.shetupoi.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  if(proverka(rp.hztz.ravno(),rekin.hzt.ravno(),0,0) != 0)
    continue;

  if(proverka(rp.hnaz.ravno(),rekin.hna.ravno(),0,0) != 0)
    continue;

  if(proverka(rp.hnabyz.ravno(),rekin.hnaby.ravno(),0,0) != 0)
    continue;

  if(proverka(rp.hauz.ravno(),rekin.hau.ravno(),0,0) != 0)
    continue;

  kolstr3++;

  memset(pole1,'\0',sizeof(pole1));      

  strcpy(pole1,rekin.hna.ravno());

  if(pole1[0] == '\0')
   strcpy(pole1,"???");


  sprintf(strsql,"insert into %s values ('%s','%s',%d,%d,'%s',%d,'%s')",
  imatmptab,
  pole1,rekin.shetu.ravno(),poddz,atoi(row[0]),iceb_tu_sqlfiltr(row[1]),kodotl,rekin.nomz.ravno());

  sql_zapis(strsql,1,0);    

 }
GDITE(); 

SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
if(metr == 0) //По подразделениям
 {
  if(metka_sort == 0)
    sprintf(strsql,"select * from %s order by hna asc,podr,innom asc",imatmptab);
  if(metka_sort == 1)
    sprintf(strsql,"select * from %s order by shetu asc,podr,innom asc",imatmptab);
 }
if(metr == 1) //По материально-ответственным
 {
  if(metka_sort == 0)
    sprintf(strsql,"select * from %s order by hna asc,kodmo,innom asc",imatmptab);
  if(metka_sort == 1)
    sprintf(strsql,"select * from %s order by shetu asc,kodmo,innom asc",imatmptab);
 }

if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

char imaf[64];
sprintf(imaf,"uosbd%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
int kst=0;
uosvibo_start(&rp,metr,metka_sort,&kst,metkaost,ff);

double balst=0.;  /*Первичная балансовая стоимость*/
double iznoss=0.; /*износ на начало года*/
double iziznosa=0.; /*изменение износа за год*/
double amort=0.;    /*амортизация за год*/
double ostst=0.; /*остаточная стоимость*/
int nomer_str=0;
double itogoo[5];
memset(itogoo,'\0',sizeof(itogoo));
double itogop[5];
memset(itogop,'\0',sizeof(itogop));
double itogopodr[5];
memset(itogopodr,'\0',sizeof(itogopodr));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
  No |Інвентарни|Номер |Найменування основних засобів |Первісна  |Річна|       Нараховано знос (грн.коп.)          |\n\
 п/п |  номер   |субрах|                              |(балансова|норма|на початок|       за рік        |на кінець |\n\
     |          |унку  |                              |)вартість |зносу|  року    |  зміна   |нарахована|  року    |\n\
     |          |      |                              |          |     |          |  зносу   |амор-зація|          |\n");

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

kolstr1=0;
class iceb_tu_str hna("");
class iceb_tu_str nomz("");
class iceb_tu_str shetu("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,++kolstr1);
  if(metr == 0) //По подразделениям
   {
    pod=atoi(rowtmp[2]);
    if(metka_sort == 0)
     {
     hna.new_plus(rowtmp[0]);
    }
    if(metka_sort == 1)
     {
      hna.new_plus(rowtmp[1]);
      
     }
   }
  if(metr == 1) //По материально-ответственным
   {
    pod=atoi(rowtmp[5]);
    if(metka_sort == 0)
      hna.new_plus(rowtmp[0]);
    if(metka_sort == 1)
      hna.new_plus(rowtmp[1]);
   }
  
  podrp=pod;
  in=atoi(rowtmp[3]);
  naimos.new_plus(rowtmp[4]);
  
  if(metr == 1) /*По мат-отв*/
   {
    podrp=atoi(rowtmp[2]);
   }

  nomz.new_plus(rowtmp[6]);
  shetu.new_plus(rowtmp[1]);
  
  class bsiz_data data_per;
  
  bsizp(in,podrp,0,dn,mn,gn,dk,mk,gk,&data_per,ff_prot);
  
  balst=data_per.nu.bs_kon_per; //Балансовая стоимость на начало периода
  iznoss=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
  iziznosa=data_per.nu.izmen_iznosa_raznica;
  amort=data_per.nu.amort_raznica;
  ostst=balst-iznoss;      
  
        
  if(fabs(balst) < 0.01 ) //Значит уже продан или списан или передан в другое подразделение
   {
/***********
    if(in == 7001090)
     { 
      printw("in=%d %d.%d.%d\n",in,dk,mk,gk);
      printw("pr1=%.2f pr2=%.2f\n",pr1,pr2);
      OSTANOV();
     }
*************/
    continue;
   }


  if(podz != pod)
   {
    if(podz != 0)
     {
      if(metr == 0)
        sprintf(strsql,"%s %d",gettext("Итого по подразделению"),podz);
      if(metr == 1)
        sprintf(strsql,"%s %d",gettext("Итого по мат.ответственному"),podz);
      uosvibo_itogo(strsql,itogopodr,ff);
     }

    if(metr == 0)
     {
      /*Читаем наименование подразделения*/
      naipod.new_plus("");
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(readkey(strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),pod);
        fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        OSTANOV();
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(metr == 1)
     {
      /*Читаем мат.-ответственного*/
      naipod.new_plus("");
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(readkey(strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        fprintf(ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        OSTANOV();
       }
      else
        naipod.new_plus(row1[0]);
     }

    mvsh++;
             
    podz=pod;
    if(metr == 0)
     printw("\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod.ravno());
    if(metr == 1)
     printw("\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());
   }

  if(SRAV(hna1.ravno(),hna.ravno(),0) != 0)
   {
    if(hna1.ravno()[0] != '\0')
     {
      if(metka_sort == 0)
        sprintf(strsql,"%s %s",gettext("Итого по шифру"),hna1.ravno());
      if(metka_sort == 1)
        sprintf(strsql,"%s %s",gettext("Итого по счёту"),hna1.ravno());
      uosvibo_itogo(strsql,itogop,ff);
     }
    
    if(metka_sort == 0)
     {
      /*Уэнаем наименование шифра нормы аморт. отчисления*/
      naigrup.new_plus("");
      if(SRAV(hna.ravno(),"???",0) != 0)
       {
        sprintf(strsql,"select naik from Uosgrup where kod='%s'",rowtmp[0]);

        if(readkey(strsql,&row1,&curr) != 1)
         {
          beep();
          printw("%s %s !\n",gettext("Не найдено группу"),rowtmp[0]);
          fprintf(ff,"%s %s !\n",gettext("Не найдено группу"),rowtmp[0]);
          OSTANOV();
         }
        else
          naigrup.new_plus(row1[0]);
       }
      else
       naigrup.new_plus("Не введена в карточку группа");
      printw("\n%s %s \"%s\"\n",
      gettext("Группа"),hna.ravno(),naigrup.ravno());
     }
    if(metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna.ravno());
      naigrup.new_plus("");
      if(readkey(strsql,&row1,&curr) == 1)
        naigrup.new_plus(row1[0]);
     
      printw("\n%s %s \"%s\"\n",
      gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
     }
    mvsi++;


    hna1.new_plus(hna.ravno());

   }
  if(mvsh > 0 || mvsi > 0) 
   {  
    if(mvsi > 0)
     {
      if(metka_sort == 0)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Группа"),hna.ravno(),naigrup.ravno());
       }
      if(metka_sort == 1)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
       }
      mvsi=0;
      kst+=2;
     } 
    mvsh=0;
   }

  if(balst-iznoss >  0.009 && metkaost == 1)
   continue;

  if(balst-iznoss ==  0. && metkaost == 2)
   continue;


  if(fabs(balst+iznoss+ostst) > 0.009)
   {
    itogop[0]+=balst;
    itogop[1]+=iznoss;
    itogop[2]+=iziznosa;
    itogop[3]+=amort;
    itogop[4]+=iznoss+iziznosa+amort;

    itogopodr[0]+=balst;
    itogopodr[1]+=iznoss;
    itogopodr[2]+=iziznosa;
    itogopodr[3]+=amort;
    itogopodr[4]+=iznoss+iziznosa+amort;

    itogoo[0]+=balst;
    itogoo[1]+=iznoss;
    itogoo[2]+=iziznosa;
    itogoo[3]+=amort;
    itogoo[4]+=iznoss+iziznosa+amort;
    char gna[64];
    memset(gna,'\0',sizeof(gna));
    
    /*узнаём годовую норму амортизации*/
    sprintf(strsql,"select kof from Uosgrup where kod='%s'",rowtmp[0]);
    if(readkey(strsql,&row1,&curr) == 1)
      strncpy(gna,row1[0],sizeof(gna)-1);
             
    fprintf(ff,"%5d %-10d %-*s %-*.*s %10.2f %5s %10.2f %10.2f %10.2f %10.2f\n",
    ++nomer_str,
    in,
    iceb_tu_kolbait(6,shetu.ravno()),shetu.ravno(),
    iceb_tu_kolbait(30,naimos.ravno()),
    iceb_tu_kolbait(30,naimos.ravno()),
    naimos.ravno(),
    balst,
    gna,
    iznoss,
    iziznosa,
    amort,
    iznoss+iziznosa+amort);
    
   }
 }

GDITE();

if(metr == 0)
  sprintf(strsql,"%s %d",gettext("Итого по подразделению"),podz);
if(metr == 1)
  sprintf(strsql,"%s %d",gettext("Итого по мат.ответственному"),podz);
uosvibo_itogo(strsql,itogopodr,ff);

if(metka_sort == 0)
  sprintf(strsql,"%s %s",gettext("Итого по шифру"),hna1.ravno());
if(metka_sort == 1)
  sprintf(strsql,"%s %s",gettext("Итого по счёту"),hna1.ravno());
uosvibo_itogo(strsql,itogop,ff);




uosvibo_itogo(gettext("Общий итог"),itogoo,ff);

podpis(ff);
fclose(ff);
if(ff_prot != NULL)
 fclose(ff_prot);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт ведомости начисления износа для бюджетных организаций"));

if(ff_prot != NULL)
 {
  oth->spis_imaf.plus(imaf_prot);
  oth->spis_naim.plus(gettext("Протокол расчёта"));
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

printw_vr(tmmn);

OSTANOV();

return(0);
}
/***************************************/
/*Итоговая строка*/
/**************************************/
void uosvibo_itogo(const char *naim_itogo,
double *itogo,
FILE *ff)
{

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s:%10.2f %5s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(54,naim_itogo),
naim_itogo,
itogo[0],"",itogo[1],itogo[2],itogo[3],itogo[4]);

itogo[0]=itogo[1]=itogo[2]=itogo[3]=itogo[4]=0.;
}

/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvibo_start(class uosvibo_poi *rp,
int metr,
int metka_sort,
int *kst,
int metkaost,
FILE *ff)
{

*kst=5;

zagolov(gettext("Ведомость начисления износа на основные средства"),0,0,0,0,0,rp->god.ravno_atoi(),iceb_t_get_pnk("00",0),ff);

*kst+=1;


int klst=0;
if(rp->podrz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),rp->podrz.ravno());
  printcod(ff,"Uospod","kod","naik",0,rp->podrz.ravno(),&klst);
  *kst+=klst+1;
 }

if(rp->hnaz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),rp->hnaz.ravno());
  printcod(ff,"Uosgrup","kod","naik",1,rp->hnaz.ravno(),&klst);
  *kst+=klst+1;
 }
if(rp->hnabyz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),rp->hnabyz.ravno());
  printcod(ff,"Uosgrup1","kod","naik",1,rp->hnabyz.ravno(),&klst);
  *kst+=klst+1;
 }
if(rp->kodolz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),rp->kodolz.ravno());
  printcod(ff,"Uosol","kod","naik",1,rp->kodolz.ravno(),&klst);
  *kst+=klst+1;
 }
if(rp->hauz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),rp->hauz.ravno());
  printcod(ff,"Uoshau","kod","naik",0,rp->hauz.ravno(),&klst);
  *kst+=klst+1;
 }
if(rp->shetupoi.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),rp->shetupoi.ravno());
  *kst+=1;
 }

if(rp->innomp.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),rp->innomp.ravno());
  *kst+=1;
 }

if(rp->hztz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),rp->hztz.ravno());
  *kst+=1;
 }

if(rp->sosob.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),rp->sosob.ravno());
  *kst+=1;
 }

if(metkaost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(metkaost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(metr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(metr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}
