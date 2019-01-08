/*$Id: ukrras.c,v 5.47 2013/09/26 09:43:41 sasa Exp $*/
/*30.03.2016	01.11.2002	Белых А.И.	ukrras.c
Получение свода по контрагентам для учета командировочных расходов
*/
#include        <errno.h>
#include        "buhg.h"

int ukrras1(short,short,short,short,short,short,const char*,const char*,const char*,const char*,const char*,class spis_oth*);
void ukrras_rm(int,short dn,short mn,short gn,short dk,short mk,short gk,int kolkr,int kolkont,double *msum,short *mkkr,SPISOK *KONTR,int *mkolkom,const char*,const char*,const char*,const char*,const char*,const char *imaf);
void ukrras_rmks(short dn,short mn,short gn,short dk,short mk,short gk,int kolkr,int kolkont,double *msum,double *mkol,short *mkkr,class SPISOK *KONTR,int *mkolkom,const char*,const char*,const char*,const char*,const char*,const char *imaf);
void ukrras_rekp(const char *kontrp,const char *vkomp,const char *kodras,const char*,const char*,FILE *ff);



int ukrras(class spis_oth *oth)
{
short           dn,mn,gn;
short           dk,mk,gk;
int           kom,kom1;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kontr("");
static class iceb_tu_str vkom("");
static class iceb_tu_str kodras("");
static class iceb_tu_str kodgrpod("");
static class iceb_tu_str shetu("");

VVOD VV(0);
VVOD MENU(3);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(vkom.ravno(),128);
VV.VVOD_SPISOK_add_data(kodras.ravno(),128);
VV.VVOD_SPISOK_add_data(kodgrpod.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..........(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Вид кома-ировки..........(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код расходов.............(,,)."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код группы подразделения.(,,)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт.....................(,,)."));//7

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка свода по контрагентам"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kontr.new_plus(VV.data_ravno(2));
vkom.new_plus(VV.data_ravno(3));
kodras.new_plus(VV.data_ravno(4));
kodgrpod.new_plus(VV.data_ravno(5));
shetu.new_plus(VV.data_ravno(6));

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("ukr4_2.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;

      case 1 :
        vibrek("Ukrvkr",&vkom);
        VV.VVOD_SPISOK_zapis_data(3,vkom.ravno());
        goto naz;

      case 2 :
        naim.new_plus("");
        if(dirkkr(&kod,&naim,1) == 0)
         {
          VV.data_z_plus(5,kod.ravno());
         }
        goto naz;

      case 3:
        vibrek("Plansh",&shetu);
        VV.VVOD_SPISOK_zapis_data(6,shetu.ravno());
        goto naz;
               
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

return(ukrras1(dn,mn,gn,dk,mk,gk,kontr.ravno(),vkom.ravno(),kodras.ravno(),kodgrpod.ravno(),shetu.ravno(),oth));

}
/*************************************/
/* Расчёт                            */
/*************************************/
int ukrras1(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
class spis_oth *oth)
{
char		strsql[512];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
int		kolkont=0;
int		kolkr=0;
int		i=0;
time_t		timen;
SQLCURSOR curr;
clear();
time(&timen);
printw("%s\n",gettext("Распечатка ведомости по контрагентам"));
printw("%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);

GDITE();

//Определяем количество командировочных расходов
sprintf(strsql,"select kod from Ukrkras");

class SQLCURSOR cur;

if((kolkr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolkr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }    
i=0;
short mkkr[kolkr]; //Массив кодов командировочных расходов
while(cur.read_cursor(&row) != 0)
  mkkr[i++]=atoi(row[0]);  

//Определяем количество контрагентов
sprintf(strsql,"select distinct kont from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d'",gn,mn,dn,gk,mk,dk);

if((kolkont=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolkont == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }    
int  mkolkom[kolkont]; //Массив количества командировок
memset(mkolkom,'\0',sizeof(mkolkom));

i=0;
SPISOK KONTR; //Массив кодов контрагентов
while(cur.read_cursor(&row) != 0)
  KONTR.plus(row[0]);  

double msum[kolkont*kolkr]; //Массив сум
memset(&msum,'\0',sizeof(msum));

double mkol[kolkont*kolkr]; //Массив количества
memset(&mkol,'\0',sizeof(mkol));

sprintf(strsql,"select datd,nomd,kont,vkom from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d'",gn,mn,dn,gk,mk,dk);

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

SQLCURSOR cur1;
int pozkr=0;
int pozkont=0;
double suma=0.;
int kodkr=0;

FILE *ffprot;
char imafprot[64];
sprintf(imafprot,"ukrkp%d.lst",getpid());

if((ffprot = fopen(imafprot,"w")) == NULL)
 {
  error_op_nfil(imafprot,errno,"");
  return(1);
 }

short kolsimv=iceb_tu_strlen(ICEB_ZAR_PKTN);


iceb_t_zagolov(gettext("Протокол расчёта"),dn,mn,gn,dk,mk,gk,ffprot);
double kolih=0.;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {

  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kontrp,row[2],0,0) != 0)
    continue;

  if(proverka(vkomp,row[3],0,0) != 0)
     continue;

  if((pozkont=KONTR.find_r(row[2])) == -1)
    continue;

  if(kodgrpod[0] != '\0')
   {
    SQL_str row1;
    class SQLCURSOR cur1;
    class iceb_tu_str podr("");
    short d,m,g;
    
    rsdat(&d,&m,&g,row[0],2);
    
    class iceb_tu_str tabnom;
    
//    sprintf(tabnom,"%s",&row[2][2]);
    tabnom.plus(iceb_tu_adrsimv(kolsimv,row[2]));    

    sprintf(strsql,"select podr from Zarn where tabn=%s and god=%d and mes=%d",tabnom.ravno(),g,m);
    if(readkey(strsql,&row1,&cur1) == 1)
      podr.new_plus(row1[0]);
    else
     {
      sprintf(strsql,"select podr from Kartb where tabn=%s",tabnom.ravno());
      if(readkey(strsql,&row1,&cur1) == 1)
        podr.new_plus(row1[0]);
     }    

    sprintf(strsql,"select grup from Podr where kod=%d",podr.ravno_atoi());
    if(readkey(strsql,&row1,&cur1) == 1)
      if(proverka(kodgrpod,row1[0],0,0) != 0)
       continue;
   
   }

  
  fprintf(ffprot,"\n%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  mkolkom[pozkont]++;
      
  sprintf(strsql,"select kodr,kolih,cena,snds,shet,ss from Ukrdok1 where datd='%s' and nomd='%s'",
  row[0],row[1]);   
//  printw("%s\n",strsql);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(kodras,row1[0],0,0) != 0)
      continue;
    if(proverka(shetu,row1[4],1,0) != 0)
      continue;
    fprintf(ffprot," %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
      
    kodkr=atoi(row1[0]);
    suma=atof(row1[2])+atof(row1[3])+atof(row1[5]);
    kolih=atof(row1[1]);
        
    for(pozkr=0 ; pozkr < kolkr; pozkr++)
     if(mkkr[pozkr] == kodkr)
      break;
    msum[pozkont*kolkr+pozkr]+=suma;
    mkol[pozkont*kolkr+pozkr]+=kolih;
   }
 }
GDITE(); 

char imaf[64];
sprintf(imaf,"ukrk%d.lst",getpid());

ukrras_rm(0,dn,mn,gn,dk,mk,gk,kolkr,kolkont,msum,mkkr,&KONTR,mkolkom,kontrp,vkomp,kodras,kodgrpod,shetu,imaf);

char imafkol[64];
sprintf(imafkol,"ukrkkol%d.lst",getpid());

ukrras_rm(1,dn,mn,gn,dk,mk,gk,kolkr,kolkont,mkol,mkkr,&KONTR,mkolkom,kontrp,vkomp,kodras,kodgrpod,shetu,imafkol);

char imaf_ks[64];
sprintf(imaf_ks,"ukrks%d.lst",getpid());
ukrras_rmks(dn,mn,gn,dk,mk,gk,kolkr,kolkont,msum,mkol,mkkr,&KONTR,mkolkom,kontrp,vkomp,kodras,kodgrpod,shetu,imaf_ks);

podpis(ffprot);
fclose(ffprot);


oth->spis_imaf.plus(imaf_ks);

sprintf(strsql,"%s (%s+%s)",
gettext("Распечатка ведомости по контрагентам"),
gettext("Количество"),
gettext("Сумма"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf);

sprintf(strsql,"%s (%s)",
gettext("Распечатка ведомости по контрагентам"),
gettext("Сумма"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imafkol);

sprintf(strsql,"%s (%s)",
gettext("Распечатка ведомости по контрагентам"),
gettext("Количество"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imafprot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);

}
/***************************************/
/*Распечатка массивов*/
/*****************************/
void ukrras_rm(int metka, //0-распечатка сумм 1-распечатка количества
short dn,short mn,short gn,
short dk,short mk,short gk,
int kolkr,
int kolkont,
double *msum,
short *mkkr,
SPISOK *KONTR,
int *mkolkom,
const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
const char *imaf)
{
char strsql[512];
SQL_str row;
SQLCURSOR curr;

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка ведомости по контрагентам"),dn,mn,gn,dk,mk,gk,ff);

if(metka == 0)
 fprintf(ff,"%s\n",gettext("Распечатка сумм"));

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Распечатка количества"));

ukrras_rekp(kontrp,vkomp,kodras,kodgrpod,shetu,ff);
 
 
class iceb_tu_str naikont("");
class iceb_tu_str naivkom("");
int	i1=0;
double  itogo=0.;
double  itogoo=0.;
double bb=0.;
class iceb_tu_str strpod;
int     itkolkom=0;

strpod.plus("----------------------------------------");
for(int i=0; i < kolkr+2; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=msum[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  strpod.plus("-----------");
 } 
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,gettext("Код конт|  Фамилия, имя, отчество      |"));

fprintf(ff,"%-*.*s|",
iceb_tu_kolbait(10,gettext("Кол.коман.")),
iceb_tu_kolbait(10,gettext("Кол.коман.")),
gettext("Кол.коман."));

for(int i=0; i < kolkr; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=msum[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  sprintf(strsql,"select naim from Ukrkras where kod=%d",
  mkkr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naivkom.new_plus(row[0]);
  else
   naivkom.new_plus("");

  fprintf(ff,"%-*.*s|",
  iceb_tu_kolbait(10,naivkom.ravno()),
  iceb_tu_kolbait(10,naivkom.ravno()),
  naivkom.ravno());
 }
fprintf(ff,"%-*.*s|\n",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(ff,"%s\n",strpod.ravno());

for(int i=0; i < kolkont; i++)
 {
  //если по строке все равно нулю то строку не печатаем
  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
    itogo+=msum[i*kolkr+i1];
  if(itogo == 0.)
   continue;
   


  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  KONTR->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naikont.new_plus(row[0]);
  else
   naikont.new_plus("");
     
  fprintf(ff,"%*s %-*.*s %10d",
  iceb_tu_kolbait(8,KONTR->ravno(i)),KONTR->ravno(i),
  iceb_tu_kolbait(30,naikont.ravno()),iceb_tu_kolbait(30,naikont.ravno()),naikont.ravno(),
  mkolkom[i]);
  
  itkolkom+=mkolkom[i];

  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
   {
    bb=0.;
    for(int i2=0; i2 < kolkont; i2++)
      bb+=msum[i2*kolkr+i1];
    if(bb == 0.)
      continue;

    fprintf(ff," %10.2f",msum[i*kolkr+i1]);
    itogo+=msum[i*kolkr+i1];
   }
  fprintf(ff," %10.2f\n",itogo);

 }
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,"%*s: %10d",iceb_tu_kolbait(38,gettext("Итого")),gettext("Итого"),itkolkom);

for(int i=0; i < kolkr; i++)
 {
  bb=0.;
  for(i1=0; i1 < kolkont; i1++)
    bb+=msum[i1*kolkr+i];
  if(bb == 0.)
    continue;
  itogo=0.;
  for(i1=0; i1 < kolkont; i1++)
    itogo+=msum[i1*kolkr+i];
  fprintf(ff," %10.2f",itogo);
  itogoo+=itogo;
 }

fprintf(ff," %10.2f\n",itogoo);

podpis(ff);
fclose(ff);

}
/***************************/
/*Распечатка количество+сумма*/
/*****************************/
void ukrras_rmks(short dn,short mn,short gn,
short dk,short mk,short gk,
int kolkr,
int kolkont,
double *msum,
double *mkol,
short *mkkr,
class SPISOK *KONTR,
int *mkolkom,
const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
const char *imaf)
{
char strsql[512];
SQL_str row;
SQLCURSOR curr;


FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка ведомости по контрагентам"),dn,mn,gn,dk,mk,gk,ff);


ukrras_rekp(kontrp,vkomp,kodras,kodgrpod,shetu,ff);
 

class iceb_tu_str naikont("");
class iceb_tu_str naivkom("");
int	i1=0;
double  itogo=0.;
double  itogoo=0.;
double bb=0.;
class iceb_tu_str strpod;
int     itkolkom=0;

strpod.plus("----------------------------------------------");
for(int i=0; i < kolkr+1; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  strpod.plus("-----------------");
 } 

fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,gettext("Код конт|  Фамилия, имя, отчество      |"));
fprintf(ff,"%-*.*s|",
iceb_tu_kolbait(5,gettext("Кол.")),
iceb_tu_kolbait(5,gettext("Кол.")),
gettext("Кол."));


for(int i=0; i < kolkr; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  sprintf(strsql,"select naim from Ukrkras where kod=%d",
  mkkr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naivkom.new_plus(row[0]);
  else
   naivkom.new_plus("");
  fprintf(ff,"%-*.*s|",
  iceb_tu_kolbait(16,naivkom.ravno()),iceb_tu_kolbait(16,naivkom.ravno()),naivkom.ravno());
 }
fprintf(ff,"%-*.*s|\n",
iceb_tu_kolbait(16,gettext("Итого")),
iceb_tu_kolbait(16,gettext("Итого")),
gettext("Итого"));

fprintf(ff,"        |                              |%*.*s|",
iceb_tu_kolbait(5,gettext("кома.")),
iceb_tu_kolbait(5,gettext("кома.")),
gettext("кома."));

for(int i=0; i < kolkr+1; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(5,gettext("Кол.")),iceb_tu_kolbait(5,gettext("Кол.")),gettext("Кол."),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));

 } 
fprintf(ff,"\n");

fprintf(ff,"%s\n",strpod.ravno());
double itogo_k=0.;
for(int i=0; i < kolkont; i++)
 {
  //если по строке все равно нулю то строку не печатаем
  itogo_k=itogo=0.;
  
  for(i1=0; i1 < kolkr; i1++)
    itogo+=mkol[i*kolkr+i1];
  if(itogo == 0.)
   continue;
   


  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  KONTR->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naikont.new_plus(row[0]);
     
  fprintf(ff,"%*s %-*.*s %5d",
  iceb_tu_kolbait(8,KONTR->ravno(i)),KONTR->ravno(i),
  iceb_tu_kolbait(30,naikont.ravno()),
  iceb_tu_kolbait(30,naikont.ravno()),
  naikont.ravno(),
  mkolkom[i]);
  
  itkolkom+=mkolkom[i];

  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
   {
    bb=0.;
    for(int i2=0; i2 < kolkont; i2++)
      bb+=mkol[i2*kolkr+i1];
    if(bb == 0.)
      continue;

    fprintf(ff," %5.5g %10.2f",mkol[i*kolkr+i1],msum[i*kolkr+i1]);
    itogo+=msum[i*kolkr+i1];
    itogo_k+=mkol[i*kolkr+i1];
   }
  fprintf(ff," %5.5g %10.2f\n",itogo_k,itogo);

 }
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,"%*s: %5d",iceb_tu_kolbait(38,gettext("Итого")),gettext("Итого"),itkolkom);

double itogoo_k=0.;
for(int i=0; i < kolkr; i++)
 {
  bb=0.;
  for(i1=0; i1 < kolkont; i1++)
    bb+=mkol[i1*kolkr+i];
  if(bb == 0.)
    continue;
  itogo=0.;
  itogo_k=0.;
  for(i1=0; i1 < kolkont; i1++)
   {
    itogo+=msum[i1*kolkr+i];
    itogo_k+=mkol[i1*kolkr+i];
   }
  fprintf(ff," %5.5g %10.2f",itogo_k,itogo);
  itogoo+=itogo;
  itogoo_k+=itogo_k;
 }

fprintf(ff," %5.5g %10.2f\n",itogoo_k,itogoo);

podpis(ff);
fclose(ff);

}

/*********************/
/*Распечатка реквизитов поиска*/
/*******************************/
void ukrras_rekp(const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
FILE *ff)
{
if(kontrp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kontrp);
if(vkomp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код вида командировки"),vkomp);
if(kodras[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код расхода"),kodras);
if(kodgrpod[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код группы подразделения"),kodgrpod);
if(shetu[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shetu);

}

