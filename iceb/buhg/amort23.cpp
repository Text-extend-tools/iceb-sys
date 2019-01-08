/* $Id: amort23.c,v 5.25 2013-04-07 10:22:41 sasa Exp $ */
/*13.07.2015    22.08.1997      Белых А.И.      amort23.c
Расчёт амортизации для группы 2 и 3
*/
#include        "buhg.h"
void  amort23b(int in,short gr,int podd,int kodmo,const char *hzt,double bs,double iz,double bs1,double iz1,double kof,double koff);


void amort23(int in,int podd,
int kodmo,
short kvrt, //Квартал
short gr,const char *hzt,FILE *ff_prot)
{
double          bs=0.,iz=0.,bs1=0.,iz1=0.,iz11=0.;
double          koff=1,kof=1;
char            grp[64];
double		amort[4];
short		mr,dr1,mr1;
short		gr1;
short		i,i1;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
time_t		vrem;
double          sbs=0.,siz=0.;

if(ff_prot != NULL)
 fprintf(ff_prot,"Расчёт амортизации для налогового учёта в целом по группе.\n%d %d %d %d %s\n",in,podd,kvrt,gr,hzt);
//printw("amort23-%d %d %d %d %s\n",in,podd,kvrt,gr,hzt);
//OSTANOV();
mr=0;
if(kvrt == 1)
  mr=1;
if(kvrt == 2)
  mr=4;
if(kvrt == 3)
  mr=7;
if(kvrt == 4)
  mr=10;

for(i=0; i < 4 ; i++)
 amort[i]=0.;
if(in < 0)
 {
  koff=1;
  char stroka[128];
  sprintf(stroka,"Поправочный коэффициент для группы %d",in*-1);
  if(iceb_t_poldan(stroka,strsql,"uosnast.alx") == 0)
   koff=ATOFM(strsql);
  
  sbs=0.;
  sprintf(stroka,"Стартовая балансовая стоимость для группы %d",in*-1);
  if(iceb_t_poldan(stroka,strsql,"uosnast.alx") == 0)
    sbs=atof(strsql);
  siz=0.;
  sprintf(stroka,"Стартовый износ для группы %d",in*-1);
  if(iceb_t_poldan(stroka,strsql,"uosnast.alx") == 0)
    siz=atof(strsql);
 }

GDITE();
dr1=1;
mr1=mr;
gr1=gr;
if(kvrt == 0) //Для бюджетных организаций
 {
  //Берем балансувую стоимость на конец года
  mr1=1;
  gr1++; 
 }

bs=sbs;
iz=siz;

if(in < 0)
  sprintf(grp,"%d",in*-1);

bsiz23(grp,dr1,mr1,gr1,&bs1,&iz1,&iz11,ff_prot);
iz1+=iz11;

/*Читаем коэффициент*/
kof=0.;
sprintf(strsql,"select kof from Uosgrup where kod='%s'",grp);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
   sprintf(strsql,"%s %s !",gettext("Не найдено код группы"),grp);
   iceb_t_soob(strsql);
   return;
 }
else
 kof=atof(row[0]);

if(kvrt == 0)
 {
  amort23b(in,gr,podd,kodmo,hzt,bs,iz,bs1,iz1,kof,koff);
  return;
 }

//sprintf(strsql,"delete from Uosamor where innom='%d' and god=%d and mes >=%d and mes <= %d",in,gr,mr,mr+2);
sprintf(strsql,"delete from Uosamor where innom='%d' and da >= '%04d-%02d-01' and da <= '%04d-%02d-31'",in,gr,mr,gr,mr+2);

sql_zapis(strsql,1,0); 

/*
printw("\nbs=%.2f iz=%.2f bs1=%.2f iz1=%.2f us1.kof=%.2f koff=%.2f\n",
bs,iz,bs1,iz1,us1.kof,koff);
*/

VVOD SOOB(1);

sprintf(strsql,"%s:%.2f %s:%.2f (%.2f/%.2f)",
gettext("Б.с."),
bs,
gettext("Износ"),
iz,bs1,iz1);
SOOB.VVOD_SPISOK_add_MD(strsql);

amort[0]=(bs+bs1-iz-iz1)*kof/100/12.*3*koff;

sprintf(strsql,"(%.2f+%.2f-%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f",
bs,bs1,iz,iz1,kof,koff,amort[0]);

if(amort[0] < 0.)
 amort[0]=0.;
 
SOOB.VVOD_SPISOK_add_MD(strsql);

amort[0]=okrug(amort[0],0.01); /*Квартальная норма*/
sprintf(strsql,"%s: %.2f",
gettext("Квартальная норма"),
amort[0]);

SOOB.VVOD_SPISOK_add_MD(strsql);

amort[1]=amort[0]/3;
amort[1]=okrug(amort[1],0.01);
sprintf(strsql,"%s: %.2f",
gettext("Месячная норма"),
amort[1]);

SOOB.VVOD_SPISOK_add_MD(strsql);

amort[2]=amort[1];
amort[3]=amort[0]-amort[1]-amort[2];

sprintf(strsql,"%s: %.2f",
gettext("Амортизация последнего месяца квартала"),
amort[3]);

SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);

i1=1;
time(&vrem);
for(i=mr; i < mr+3 ; i++)
 {
  sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-%02d-01',%d,'%s','%d',%.2f,%d,%ld,%d)",in,gr,i,podd,hzt,in*(-1),amort[i1++],iceb_t_getuid(),vrem,kodmo);

  sql_zapis(strsql,1,0); 
 }


}
/***********************************/
/*Для бюджетных организаций*/
/****************************/
void  amort23b(int in,
short gr,
int podd,
int kodmo,
const char *hzt,
double bs,
double iz,
double bs1,
double iz1,
double kof,
double koff)
{
char strsql[512];

//sprintf(strsql,"delete from Uosamor where innom='%d' and god=%d",in,gr);
sprintf(strsql,"delete from Uosamor where innom='%d' and da >= '%04d-01-01' and da <= '%04d-12-31'",in,gr,gr);

sql_zapis(strsql,1,0); 

/*
printw("\nbs=%.2f iz=%.2f bs1=%.2f iz1=%.2f us1.kof=%.2f koff=%.2f\n",
bs,iz,bs1,iz1,us1.kof,koff);
*/

VVOD SOOB(1);

sprintf(strsql,"%s:%.2f %s:%.2f (%.2f/%.2f)",
gettext("Б.с."),
bs,
gettext("Износ"),
iz,bs1,iz1);
SOOB.VVOD_SPISOK_add_MD(strsql);

double amort=(bs+bs1-iz-iz1)*kof/100*koff;

sprintf(strsql,"(%.2f+%.2f-%.2f-%.2f)*%.2f/100*%.2f=%.2f",
bs,bs1,iz,iz1,kof,koff,amort);

if(amort < 0.)
 amort=0.;
 
SOOB.VVOD_SPISOK_add_MD(strsql);

amort=okrug(amort,1.); //Округляем до гривны
sprintf(strsql,"%s: %.2f",
gettext("Годовая норма"),
amort);

SOOB.VVOD_SPISOK_add_MD(strsql);


soobshw(&SOOB,stdscr,-1,-1,0,1);
time_t vrem;
time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-12-01',%d,'%s','%d',%.2f,%d,%ld,%d)",in,gr,podd,hzt,in*(-1),amort,iceb_t_getuid(),vrem,kodmo);

sql_zapis(strsql,1,0); 

}
