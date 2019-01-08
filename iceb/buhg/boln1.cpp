/* $Id: boln1.c,v 5.16 2014/02/28 05:13:47 sasa Exp $ */
/*02.09.2016    01.02.2011      Белых А.И.      boln1.c
Перерасчёт подоходного налога с учётом больничных
*/
#include        <math.h>
#include        "buhg.h"


extern double   findnah(long,short,short);

extern short    kodpen;  /*Код пенсионных отчислений*/
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern float    procsoc; /*Процент отчисления в соц-страх*/
extern char	*imabaz;
extern double   okrg;
extern short    kodpn;   /*Код подоходного налога*/
extern short kodpn_sbol; /*Код подоходного с больничного*/
extern class iceb_tu_str kodpn_all;
extern short   *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	*kodsocstrnv; //Коды не входящие в расчёт соцстраха
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    *kodmp;   /*Коды материальной помощи*/
extern short	*kodnvpen; /*Коды не входящие в расчёт пенсионного отчисления*/

extern short	*kodsocstrnv; //Коды не входящие в расчёт соцстраха
extern short	*kodbzrnv; //Коды не входящие в расчёт отчислений на безработицу

double boln1(int tn, //Табельный номер
short mp,short gp,
int podr,
const char *uder_only)
{
int             i,i1;
short           den;
double          sn;
class iceb_tu_str shet("");
SQL_str         row;
class SQLCURSOR cur;
char		strsql[1024];
short		kolzap=0;
class ZARP     zp;
double bbb=0.;
int kolstr=0;


if(kodbl == NULL)
 {
  sprintf(strsql,"%s-Не введён код больничного в файле настройки zarnast.alx!\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return(0);
 }

sprintf(strsql,"%s-%d.%d %d\n",__FUNCTION__,mp,gp,tn);
zar_pr_ins(strsql);

dpm(&den,&mp,&gp,5);
int kodzap=kodpn;
if(kodpn_sbol != 0)
 kodzap=kodpn_sbol;
 
zp.dz=den;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

class masiv_din_int MES;
class masiv_din_int GOD;
class masiv_din_double SUM;
class masiv_din_int KNAH;
/***************************
for(i=0;i<razm;i++)
 {
  if(zar.prnu[i] == 2 )
   break;
  if(fabs(zar.sm[i]) < 0.01)
   continue;
  if(provkod(kodbl,zar.knu[i]) >= 0 && zar.mes[i] != mp)
   {
    if((i1=MES.find(zar.mes[i])) == -1)
     {

      MES.plus(zar.mes[i],-1);
      GOD.plus(zar.god[i],-1);
      SUM.plus(zar.sm[i],-1);
      KNAH.plus(zar.knu[i],-1);
      kolzap++;
     }      
    else
     {
      SUM.plus(zar.sm[i],i1);
     }
   }
 }
*************************/

/*ищем больничные не в счёт месяца расчёта*/
sprintf(strsql,"select knah,suma,godn,mesn from Zarp where tabn=%d and datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
and prn='1' and mesn != %d and suma <> 0.",tn,gp,mp,gp,mp,mp);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

if(kolstr == 0)
 {
  return(0.);
 }
double nah_suma_bol=0.;
short mes_bol;
short god_bol;
int knah=0;
while(cur.read_cursor(&row) != 0)
 {
  knah=atoi(row[0]);
  if(provkod(kodbl,knah) >= 0)
   {
    mes_bol=atoi(row[3]);    
    nah_suma_bol=atof(row[1]);
    god_bol=atoi(row[2]);
    
    if((i1=MES.find(mes_bol)) == -1)
     {

      MES.plus(mes_bol,-1);
      GOD.plus(god_bol,-1);
      SUM.plus(nah_suma_bol,-1);
      KNAH.plus(knah,-1);
      kolzap++;
     }      
    else
     {
      SUM.plus(nah_suma_bol,i1);
     }
   }

 }

if(kolzap == 0)
  return(0.);
  

sn=0.;
short	mes=0,god=0;
double sumbol=0.;
for(i=0;i<kolzap;i++)
 {
  mes=MES.ravno(i);
  god=GOD.ravno(i);
  sumbol=SUM.ravno(i);
//  printw("mes=%d sumbol=%.2f\n",mes,sumbol);
//  OSTANOV();
      
    sprintf(strsql,"\n%s-Перерасчёт подоходного налога для больничного\n\
---------------------------------------------------\n",__FUNCTION__);
    zar_pr_ins(strsql);

    sprintf(strsql,"%s-Дата перерасчёта:%d.%d Сумма больничного:%.2f\n",__FUNCTION__,mes,god,sumbol);
    zar_pr_ins(strsql);


  /*чтение величины минимальной зарплаты и прожиточного минимума*/
//  zar_read_tn(1,mes,god,ff_prot); 

  if(sravmydat(1,mes,god,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
   {
//    boln1_pa(tn,podr,mp,gp,mes,god,sumbol,KNAH.ravno(i),uder_only,ff_prot);
    continue;
   }

  double suma_podoh=0.;
  /*Смотрим какая там сумма подоходного уже посчитана берём только в счёт расчётного месяца*/
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and prn='2' and suma <> 0. and godn=%d and mesn=%d",
  god,mes,god,mes,tn,god,mes);

  if(cur.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(0.);
   }
  while(cur.read_cursor(&row) != 0)
   {
    if(proverka(kodpn_all.ravno(),row[0],0,1) == 0)
      suma_podoh+=atof(row[1]);
       
   }  
  sprintf(strsql,"%s-Сумма подоходного за %d.%d - %.2f\n",__FUNCTION__,mes,god,suma_podoh);
  zar_pr_ins(strsql);
  sprintf(strsql,"%s-Сумма больничного - %.2f\n",__FUNCTION__,sumbol);
  zar_pr_ins(strsql);
   

  //расчёт подоходного налога
    
  bbb=ras_podoh1(tn,mes,god,podr,1,sumbol,uder_only); /*Обязательно -1 так как не должно переписывать протокол*/

  sprintf(strsql,"%s-Сумма подоходного пересчитанная %.2f\n\
Разница %.2f-%.2f=%.2f\n",__FUNCTION__,bbb,bbb,suma_podoh,bbb-suma_podoh);
  zar_pr_ins(strsql);
  bbb-=suma_podoh;      

  sprintf(strsql,"select shet from Uder where kod=%d",kodzap);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw("\n%s-%s %d\n",__FUNCTION__,gettext("Не найден код удержания"),kodzap);
    OSTANOV();
   }
  else
    shet.new_plus(row[0]);

  zp.mesn=mes; zp.godn=god;
  strcpy(zp.shet,shet.ravno());
  if(proverka(uder_only,kodzap,0,0) != 0)
   {
    sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kodzap);
    zar_pr_ins(strsql);
   }
  else
   {
    zapzarp(den,mp,gp,tn,2,kodzap,bbb,shet.ravno(),mes,god,0,0, gettext("Перерасчёт больничного"),podr,"",zp); 

   }
   
 }
//OSTANOV();
return(sn);
}
