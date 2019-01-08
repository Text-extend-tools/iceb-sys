/* $Id: zarspek.c,v 5.52 2013/09/26 09:43:46 sasa Exp $ */
/*28.03.2018	15.12.1998 	Белых А.И.	zarspek.c
Получение спектра начисления зарплаты
*/
#include        <errno.h>
#include        "buhg.h"


extern short    mmm,ggg; /*Месяц год*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short    *kodmp;   /*Коды материальной помощи*/

int zarspek(class spis_oth *oth)
{
int kodsocstr=0;  /*Код отчисления на соц-страх*/
int kodpen=0;  /*Код пенсионных отчислений*/
char		bros[512];
short		par;
short		kold;/*Количество диапазонов*/
int		nd,ndz;  /*Номер диапазона*/
double		*md; /*Массив диапазонов*/
short		m,g;
int		i,i1;
char		imaf1[64],imaf2[64];
FILE		*ff1,*ff2;
double		sumn,itog;
int		tabn=0,tbn=0;
int		np;
struct  tm      *bf;
time_t          tmm;
static class iceb_tu_str spek("");
static class iceb_tu_str kodnr("");
static class iceb_tu_str podr("");
static class iceb_tu_str dat1("");
class iceb_tu_str kodnr1("");
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[512];
int		kom1;
int		knah;
double		sym;
short		metkanvr=0;
short		metkaras=0;
double		sumapens=0.;
double		nmatpom=0.,inmatpom=0.,matpom=0.;
int		klst=0;

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Спектр..(,,)................"));
VV.VVOD_SPISOK_add_MD(gettext("Дата..(м.г)................."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение..........(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Коды не вход. в расчёт.(,,)."));

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт спектра распределения зарплаты"));
VV.VVOD_SPISOK_add_data(spek.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),8);

VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kodnr.ravno(),128);

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5/-",gettext("расчёт"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

spek.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kodnr.new_plus(VV.data_ravno(3));

kodnr1.new_plus("-,");
kodnr1.plus(kodnr.ravno());

class iceb_tu_str naimp("");

switch(par)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_2.txt");
   goto naz;

  case FK10:
  case ESC:
    return(1);

  case FK2:
  case PLU:
    metkanvr=0;
    break;   

  case FK5:
  case OTN:
    metkanvr=1;
    break;   

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
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
        if(dirzarpodr(2,&podr,&naimp) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
        if(dirnach(1,&kodnr,&naimp) == 0)
         VV.VVOD_SPISOK_zapis_data(3,kodnr.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(metkanvr == 0)
if(ATOFM(dat1.ravno()) == 0.)
 {
  iceb_t_soob(gettext("Не введён спектр распределения зарплаты !"));
  goto naz;
 } 
if(rsdat1(&m,&g,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

iceb_t_poldan("Код фонда социального страхования",&kodsocstr,"zarnast.alx");

class SQLCURSOR cur;

//читаем настройки
//zar_read_tn(1,m,g,NULL);
double	ogrzp[2]={0.,0.}; /*Разграниечение зарплаты для расчёта пенсионных*/
sprintf(strsql,"select pm,ms from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",g,m,1);
if(readkey(strsql,&row,&cur) > 0)
 {

  if(g <= 2006)
   ogrzp[0]=150;
  else
    ogrzp[0]=atof(row[0]);

  ogrzp[1]=atof(row[1]);

 }

if(metkanvr == 1)
 {
  clearstr(LINES-1,0);
  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_MD(gettext("Рассчет спектра рапределения пенсионного начисления"));
  MENU.VVOD_SPISOK_add_MD(gettext("Рассчет спектра рапределения начисления соц-страха"));
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
pov:;
  kom1=0;
  while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
  switch (kom1)
   {
    case 2 :
    case -1 :
     goto naz;

    case 0 :
     metkaras=kom1+1;
     sprintf(strsql,"%.2f,%.2f",ogrzp[0],ogrzp[1]);
     spek.new_plus(strsql);
     break;

    case 1 :
     metkaras=kom1+1;
     sprintf(strsql,"%.2f,%.2f",ogrzp[0],ogrzp[1]);
     spek.new_plus(strsql);
     break;

    default:
      goto pov;
   }
 }
 
time(&tmm);
bf=localtime(&tmm);


clear();
GDITE();
if(metkaras == 1)
  printw("%s\n",gettext("Рассчет спектра рапределения пенсионного начисления"));
if(metkaras == 2)
  printw("%s\n",gettext("Рассчет спектра рапределения начисления соц-страха"));

kold=pole2(spek.ravno(),',');
if(kold == 0)
 kold=2;
else
 kold++;
  
printw("Кількість діапазонів - %d\n",kold);
refresh();
if((md=(double *)calloc(kold,sizeof(double))) == NULL)
 {
  beep();
  printw("Не могу выделить память для md !!!");
  OSTANOV();
  return(1);
 }

/*Заряжаем массив*/
if(kold == 2)
 {
  md[0]=ATOFM(spek.ravno());
  printw("%d %.2f\n",1,md[0]);
 }
else 
 for(i=0; i < kold ;i++ )
  {
   if(POLE(spek.ravno(),bros,i+1,',') != 0)
    break;
   md[i]=ATOFM(bros);
   printw("%d %.2f\n",i+1,md[i]);
  }
sprintf(strsql,"select tabn,podr from Zarn where god=%d and mes=%d",
g,m);
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
const char *imatmptab={"zarspek"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
nd int not null,\
sum double(12,2) not null,\
tbn int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");

SQLCURSOR curr;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(proverka(podr.ravno(),row[1],0,0) != 0)
    continue;

  tbn=atoi(row[0]);  

  if(metkanvr == 1)
   {
    if(metkaras == 1)
     sprintf(strsql,"select knah from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and prn='2' \
and knah=%d",g,m,g,m,tbn,kodpen);

    if(metkaras == 2)
     sprintf(strsql,"select suma from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and prn='2' \
and knah=%d",g,m,g,m,tbn,kodsocstr);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 0)
     {
      class iceb_tu_str fio("");
      sprintf(strsql,"select fio from Kartb where tabn=%d",tbn);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        fio.new_plus(row1[0]);
      else
        fio.new_plus("");
      printw("%s %s нет удержания\n",row[0],fio.ravno());

      if(metkaras == 2)
       {
        if(zarprtn(m,g,tbn,1) != 0)
         {
          printw("Пенсионер !\n");
          goto vpered;
         }
       }
      continue;
     }
    if(atof(row1[0]) == 0.)
     {
      class iceb_tu_str fio("");
      sprintf(strsql,"select fio from Kartb where tabn=%d",tbn);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        fio.new_plus(row1[0]);
      else
        fio.new_plus("");
      printw("%s %s Нулевая сумма !\n",row[0],fio.ravno());
     }
   }

vpered:;

  sumn=0.;

  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d and prn='1' and suma <> 0.",
  g,m,g,m,tbn);
  SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr2 == 0)
    continue;
  matpom=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(kodnr1.ravno(),row1[0],0,0) != 0)
     continue;
    knah=atoi(row1[0]);
    sym=atof(row1[1]);
   
    if(metkanvr == 1)
     {
      if(kodmp != NULL)
      for(i1=1; i1<= kodmp[0]; i1++)
       if(kodmp[i1] == knah)
         matpom+=sym;

      if(knvr != NULL)
      for(i1=1; i1<= knvr[0]; i1++)
       if(knvr[i1] == knah)
         goto vper;
      }              
    sumn+=sym;
vper:;

   }

  if(sumn != 0)
   {
    nmatpom=0.;
    if(metkanvr == 1 && matpom != 0.)
     {
      nmatpom=0.;
      sumn-=nmatpom;
      inmatpom+=nmatpom;
     }
    for(nd=0; nd < kold; nd++)
     {
      if(md[nd] == 0.)
        break;
      if(sumn <= md[nd])
       break;
     }  
    sprintf(strsql,"insert into %s values (%d,%.2f,%d)",
    imatmptab,
    nd,sumn,tbn);    
    sql_zapis(strsql,1,0);    

    if(nmatpom != 0.)

      printw("%d %s:%.2f\n",
      tbn,
      gettext("Не облагаемая материальная помощь"),
      nmatpom);
      
    if(zarprtn(m,g,tbn,1) != 0)
     {
      sumapens+=sumn;
     }
   }
 }

SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
sprintf(strsql,"select * from %s order by nd asc,sum asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf1,"spk%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
   {
    error_op_nfil(imaf1,errno,"");
    return(1);
   }

sprintf(imaf2,"spki%d.lst",getpid());
if((ff2 = fopen(imaf2,"w")) == NULL)
   {
    error_op_nfil(imaf2,errno,"");
    return(1);
   }


fprintf(ff1,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff2,"%s\n",iceb_t_get_pnk("00",0));

if(spek.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s: %s\n",gettext("Спектр"),spek.ravno());
  fprintf(ff2,"%s: %s\n",gettext("Спектр"),spek.ravno());
 }

if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Дата"),VV.VVOD_SPISOK_return_data(1));
  fprintf(ff2,"%s:%s\n",gettext("Дата"),VV.VVOD_SPISOK_return_data(1));
 }
if(podr.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr.ravno());
  printcod(ff1,"Podr","kod","naik",0,podr.ravno(),&klst);
  fprintf(ff2,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr.ravno());
  printcod(ff2,"Podr","kod","naik",0,podr.ravno(),&klst);
 }
else
 {
  fprintf(ff1,"%s\n",gettext("По всем подразделениям"));
  fprintf(ff2,"%s\n",gettext("По всем подразделениям"));
 }
if(VV.VVOD_SPISOK_return_data(3)[0] != '\0')
 {
  fprintf(ff1,"%s: %s\n",gettext("Коды не вошедшие в расчёт"),VV.VVOD_SPISOK_return_data(3));
  fprintf(ff2,"%s: %s\n",gettext("Коды не вошедшие в расчёт"),VV.VVOD_SPISOK_return_data(3));
 }

if(metkanvr == 1)
 {
  if(metkaras == 1)
   {
    fprintf(ff2,"%s\n",gettext("Рассчет спектра рапределения пенсионного начисления"));
    fprintf(ff1,"%s\n",gettext("Рассчет спектра рапределения пенсионного начисления"));
   }
  if(metkaras == 2)
   {
    fprintf(ff2,"%s\n",gettext("Рассчет спектра рапределения начисления соц-страха"));
    fprintf(ff1,"%s\n",gettext("Рассчет спектра рапределения начисления соц-страха"));
   }
  fprintf(ff1,"%s:\n",
  gettext("Коды не вошедшие в расчёт"));
  fprintf(ff2,"%s:\n",
  gettext("Коды не вошедшие в расчёт"));

  memset(strsql,'\0',sizeof(strsql));
  
  if(knvr != NULL)
   for(i=1; i <= knvr[0]; i++)
    {
     sprintf(bros,"%d,",knvr[i]);
     strcat(strsql,bros);
    }  
  if(strsql[0] != '\0')
   {
    printcod(ff1,"Nash","kod","naik",0,strsql,&klst);
    printcod(ff2,"Nash","kod","naik",0,strsql,&klst);
   }
 }
fprintf(ff1,"\
%s %d.%d.%d%s  %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff1,"----------------------------------------------------------\n");

fprintf(ff2,"\
%s %d.%d.%d%s  %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff2,"----------------------------------------------------------\n");
 
itog=0.;
ndz=-1;
nd=np=0;
kolstr1=0;
/***************
while(fscanf(ff,"%d %lf %d\n",&nd,&sumn,&tabn) != EOF)
 {
***********************/
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  nd=atoi(rowtmp[0]);
  sumn=atof(rowtmp[1]);
  tabn=atoi(rowtmp[2]);
  
  if(nd != ndz)
   {
    if(ndz != -1)
     { 
      printw("%s: %d %s: %.2f\n",
      gettext("Количество работников"),
      np,gettext("Сумма (грн)"),itog);
      fprintf(ff2,"%s: %d %s: %.2f\n",
      gettext("Количество работников"),
      np,gettext("Сумма (грн)"),itog);

      fprintf(ff1,"%50s: %10.2f\n",gettext("Итого"),itog);
     }
    if(md[nd] != 0)
     {
      if(nd == 0)
       {
        printw(gettext("Диапазон до %.2f включительно"),md[nd]);
        fprintf(ff2,gettext("Диапазон до %.2f включительно"),md[nd]);
        fprintf(ff1,gettext("Диапазон до %.2f включительно"),md[nd]);
       }
      if(nd != 0 )
       {
        printw(gettext("Диапазон от %.2f до %.2f включительно"),
        md[nd-1],md[nd]);
        fprintf(ff2,gettext("Диапазон от %.2f до %.2f включительно"),
        md[nd-1],md[nd]);
        fprintf(ff1,gettext("Диапазон от %.2f до %.2f включительно"),
        md[nd-1],md[nd]);
       }
      fprintf(ff1,"\n");
      fprintf(ff2,"\n");
      printw("\n");
       
     }
    else
     {
      printw("%s %.2f\n",gettext("Диапазон больше"),md[nd-1]);
      fprintf(ff1,"%s %.2f\n",
      gettext("Диапазон больше"),md[nd-1]);
      fprintf(ff2,"%s %.2f\n",
      gettext("Диапазон больше"),md[nd-1]);
     }
    ndz=nd;
    itog=0.;
    np=0;
   }  

  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabn);
    OSTANOV();
    continue;
   }
  np++;
  fprintf(ff1,"%4d %-5d %-*s %10.2f\n",
  np,tabn,
  iceb_tu_kolbait(40,row1[0]),row1[0],
  sumn);
  itog+=sumn;
 }

printw("%s: %d %s: %.2f\n",
gettext("Количество работников"),
np,gettext("Сумма (грн)"),itog);

fprintf(ff2,"%s: %d %s: %.2f\n",
gettext("Количество работников"),
np,gettext("Сумма (грн)"),itog);

if(metkanvr == 1)
 {
  printw("%s:%.2f\n",
  gettext("Не облагаемая материальная помощь"),  
  inmatpom);
  fprintf(ff2,"%s:%.2f\n",
  gettext("Не облагаемая материальная помощь"),
  inmatpom);
 }
if(metkaras == 2)
 {
  printw("Пенсионеры:%.2f\n",sumapens);
  fprintf(ff2,"Пенсионеры:%.2f\n",sumapens);
 }
fprintf(ff1,"%*s: %10.2f\n",iceb_tu_kolbait(50,gettext("Итого")),gettext("Итого"),itog);

fclose(ff1);
fclose(ff2);

OSTANOV();

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Спектр распределения зарплаты"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}
