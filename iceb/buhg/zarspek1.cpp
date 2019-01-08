/*$Id: zarspek1.c,v 5.44 2014/06/30 06:36:35 sasa Exp $*/
/*11.01.2017	14.03.2002	Белых А.И.	zarspek1.c
Расчёт статистической отчётности 1-ПВ
в отчёт не входят совместители и работающие по договору
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

int zarspek1r(short,short,class spis_oth*);
void zarspek1_r(int kold,double suman,float dni,float dnei,int *kolrab,int *kolrab1,masiv_din_double *DIAP);
void zarspek1_ras(short mn,short gn,int kold,int *kolrab,int *kolrab1,masiv_din_double *DIAP,int,FILE *ff);

extern char	*shetb; /*Бюджетные счета начислений*/
extern short    mmm,ggg; /*Месяц год*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/

void	zarspek1()
{
static class iceb_tu_str dat1("");
int		kom=0;
short		mn=0,gn=0;

if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт статистической формы 1-ПВ"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)."));

naz:;

clear();



helstr(LINES-1,0,"F1",gettext("помощь"),
"F10",gettext("выход"),NULL);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("работа с файлом"),
"F10",gettext("выход"),NULL);


kom=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1);

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_6.txt");
   goto naz;

  case FK10:
  case ESC:
    return;

  case FK2:
  case ENTER:
  case PLU:
    break;   

  case FK3:
    
    iceb_redfil("zar1-pv.alx",0);
    goto naz;

  case FK4:
    dat1.new_plus("");
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }


class spis_oth oth;

if(zarspek1r(mn,gn,&oth) == 0)
 iceb_t_rabfil(&oth,"");

}

/**************/
/*Расчёт форми*/
/**************/

int zarspek1r(short mn,short gn,class spis_oth *oth)
{
time_t 		tmm;
FILE	*ff,*ffsovm;
char	imaf[64];
char	imafsovm[64];
int	kolstr=0,kolstr1=0,kolstr2=0;
char	strsql[512];
SQL_str row,row1;
long    tabn=0;
SQLCURSOR cur;
SQLCURSOR cur1;
double	suma;
double  suman=0.;
short   knah=0;
masiv_din_double DIAP; //Массив диапазонов
int	kold=0;

time(&tmm);
clear();
printw("%s\n",gettext("Расчёт статистической формы 1-ПВ"));
printw("%s:%d.%d%s\n",gettext("Дата"),
mn,gn,
gettext("г."));

GDITE();

class iceb_tu_str kodtabotrv(""); //Коды табеля отработанного времени

iceb_t_poldan("Коды видов табеля отработанного времени",&kodtabotrv,"zar1-pv.alx");
if(kodtabotrv.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены \"Коды видов табеля отработанного времени\" !"));
  return(1);
 }
float dni,dnei,hasov;
redkal(mn,gn,&dnei,&hasov);
if(dnei == 0.)
 {
  iceb_t_soob(gettext("Не введено количество рабочих дней в месяце !"));
  return(1);
 }
SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='zar1-pv.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zar1-pv.alx");
  iceb_t_soob(strsql);
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  suma=atof(row_alx[0]);
  if(fabs(suma) < 0.009)
    continue;
  DIAP.plus(suma,-1);
  kold++;
 }


//общий отчёт
int kolrab[kold+1]; //полностью отработанное время за месяц
memset(&kolrab,'\0',sizeof(kolrab));
int kolrab1[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1,'\0',sizeof(kolrab1));

//отчёт по хозрасчёту
int kolrab_hoz[kold+1]; //полностью отработанное время за месяц
memset(&kolrab_hoz,'\0',sizeof(kolrab_hoz));
int kolrab1_hoz[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1_hoz,'\0',sizeof(kolrab1_hoz));

//отчёт по бюджету
int kolrab_bud[kold+1]; //полностью отработанное время за месяц
memset(&kolrab_bud,'\0',sizeof(kolrab_bud));
int kolrab1_bud[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1_bud,'\0',sizeof(kolrab1_bud));

sprintf(strsql,"select tabn,sovm from Zarn where god=%d and mes=%d",
gn,mn);

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

sprintf(imafsovm,"f1-sv%d.lst",getpid());

if((ffsovm = fopen(imafsovm,"w")) == NULL)
 {
  error_op_nfil(imafsovm,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Список работников не вошедших в расчёт"),0,0,0,0,mn,gn,ffsovm);

fprintf(ffsovm,"\
--------------------------------------------------------------\n");
fprintf(ffsovm,"\
 N | Т/н |       Фамилия, имя, отчество           |  Сумма   |\n");
/*
          1234567890123456789012345678901234567890
*/
fprintf(ffsovm,"\
--------------------------------------------------------------\n");
class iceb_tu_str fio("");
double sumanah=0.;
double itsovm=0.;
short  nomerpp=0;
kolstr1=0;
SQLCURSOR curr;
double suman_hoz;
double suman_bud;
int metka_dog=0;
int metka_isk=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  tabn=atol(row[0]);

  metka_dog=zarprtn(mn,gn,tabn,3); //Метка работника работающего по договору

  if(row[1][0] == '1' && metka_dog == 1) /*совместитель или работающий по договору подряда*/
   {
    sumanah=0.;
    sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      fio.new_plus(row1[0]);
    else
      fio.new_plus("");

    sprintf(strsql,"select suma from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and tabn=%s and prn='1'",
    gn,mn,1,gn,mn,31,row[0]);

    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    while(cur1.read_cursor(&row1) != 0)
      sumanah+=atof(row1[0]);
    itsovm+=sumanah;          

    fprintf(ffsovm,"%3d %5s %-*.*s %10.2f\n",
    ++nomerpp,row[0],
    iceb_tu_kolbait(40,fio.ravno()),
    iceb_tu_kolbait(40,fio.ravno()),
    fio.ravno(),
    sumanah);
    
    printw("%s %s\n",gettext("Исключён"),fio.ravno());
    refresh();
    metka_isk=1;
    continue;
   }




  sprintf(strsql,"select kodt,dnei from Ztab where tabn=%ld and god=%d \
and mes=%d",tabn,gn,mn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
     msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr2 == 0)
    continue;
  dni=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(kodtabotrv.ravno(),row1[0],0,1) == 0)
      dni+=atof(row1[1]);
   }

  if(dni < dnei/2)
    continue;
    
  sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%ld and prn='1' and suma <> 0.",
  gn,mn,gn,mn,tabn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr2 == 0)
    continue;
  suman=0.;
  suman_hoz=0.;
  suman_bud=0.;
  
  while(cur1.read_cursor(&row1) != 0)
   {
    knah=atoi(row1[0]);
    suma=atof(row1[1]);
    if(provkod(knvr,knah) >= 0)
      continue;
    suman+=suma;
    if(proverka(shetb,row[2],0,0) == 0)
     suman_bud+=suma;
    else
     suman_hoz+=suma;
   }

  zarspek1_r(kold,suman,dni,dnei,kolrab,kolrab1,&DIAP);
  zarspek1_r(kold,suman_hoz,dni,dnei,kolrab_hoz,kolrab1_hoz,&DIAP);
  zarspek1_r(kold,suman_bud,dni,dnei,kolrab_bud,kolrab1_bud,&DIAP);

 }
fprintf(ffsovm,"\
--------------------------------------------------------------\n");
fprintf(ffsovm,"%*s:%10.2f\n",iceb_tu_kolbait(50,gettext("Итого")),gettext("Итого"),itsovm);
podpis(ffsovm);
fclose(ffsovm);

sprintf(imaf,"f1-pv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

//Распечатываем общий свод
zarspek1_ras(mn,gn,kold,kolrab,kolrab1,&DIAP,0,ff);

fclose(ff);

char imaf_hoz[64];
char imaf_bud[64];

/*********************/
if(shetb != NULL)
 {
  sprintf(imaf_hoz,"f1-pvh%d.lst",getpid());

  if((ff = fopen(imaf_hoz,"w")) == NULL)
   {
    error_op_nfil(imaf_hoz,errno,"");
    return(1);
   }

  //Распечатываем хозрасчёт
  zarspek1_ras(mn,gn,kold,kolrab_hoz,kolrab1_hoz,&DIAP,1,ff);

  fclose(ff);

  sprintf(imaf_bud,"f1-pvb%d.lst",getpid());

  if((ff = fopen(imaf_bud,"w")) == NULL)
   {
    error_op_nfil(imaf_bud,errno,"");
    return(1);
   }

  //Распечатываем бюджет
  zarspek1_ras(mn,gn,kold,kolrab_bud,kolrab1_bud,&DIAP,2,ff);

  fclose(ff);

 }
if(metka_isk > 0)
 OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт статистической формы 1-ПВ."));

oth->spis_imaf.plus(imafsovm);
oth->spis_naim.plus(gettext("Список работников не вошедших в расчёт"));
if(shetb != NULL)
 {
  oth->spis_imaf.plus(imaf_hoz);
  
  sprintf(strsql,"%s (%s)",
  gettext("Расчёт статистической формы 1-ПВ."),
  gettext("Хозрасчёт"));
  
  oth->spis_naim.plus(strsql);  

  oth->spis_imaf.plus(imaf_bud);
  sprintf(strsql,"%s (%s)",
  gettext("Расчёт статистической формы 1-ПВ."),
  gettext("Бюджет"));
  
  oth->spis_naim.plus(strsql);  

 }
for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
return(0);
}
/****************************/
/*Расчёт*/
/*********************/
void zarspek1_r(int kold,double suman,float dni,float dnei,
int *kolrab,
int *kolrab1,
masiv_din_double *DIAP) //Массив диапазонов
{
int i=0;
double suma=0.;
for(i=0; i < kold; i++)
 {
  suma=DIAP->ravno(i);

  if(suman > suma)
    continue;
  else
   {
    if(dni == dnei)
      kolrab[i]+=1;
    else
      kolrab1[i]+=1;
    break;
   }
 }
if(i == kold)
 {
  if(dni == dnei)
    kolrab[kold]+=1;
  else
    kolrab1[kold]+=1;
 }  


}
/***************************/
/*Распечатка*/
/*************************/
void zarspek1_ras(short mn,short gn,
int kold,
int *kolrab,
int *kolrab1,
masiv_din_double *DIAP, //Массив диапазонов
int metka, //0-общий свод 1-хозрасчёт 2-бюджет
FILE *ff)
{

iceb_t_zagolov(gettext("Расчёт статистической формы 1-ПВ"),0,0,0,0,mn,gn,ff);

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Хозрасчёт"));

if(metka == 2)
 fprintf(ff,"%s\n",gettext("Бюджет"));

fprintf(ff,"\
---------------------------------\n");
fprintf(ff,gettext("\
  Размер зарплаты   |Дней |Дней |\n"));
fprintf(ff,"\
                    |100%% | 50%% |\n");
fprintf(ff,"\
---------------------------------\n");

int ikol=0;
int ikol1=0;
double suma=0.;
double suman=0.;
for(int i=0; i < kold; i++)
 {
  suma=DIAP->ravno(i);
//  printw("suma=%.2f\n",suma);
  if(i == 0)
    fprintf(ff,"%8.2f => %8.2f %5d %5d\n",0.,suma,kolrab[i],kolrab1[i]);
  else
    fprintf(ff,"%8.2f => %8.2f %5d %5d\n",suman,suma,kolrab[i],kolrab1[i]);
  suman=suma;
  ikol+=kolrab[i];
  ikol1+=kolrab1[i];
 }
fprintf(ff,"%8.2f => %-8s %5d %5d\n",suma," ",kolrab[kold],kolrab1[kold]);
ikol+=kolrab[kold];
ikol1+=kolrab1[kold];
fprintf(ff,"\
---------------------------------\n");
fprintf(ff,"%*s %5d %5d\n",iceb_tu_kolbait(20,gettext("Итого")),gettext("Итого"),ikol,ikol1);

podpis(ff);

}
