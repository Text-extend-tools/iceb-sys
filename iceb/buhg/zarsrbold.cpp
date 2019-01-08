/*$Id: zarsrbold.c,v 5.15 2013/09/26 09:43:46 sasa Exp $*/
/*29.06.2013	20.11.2001	Белых А.И.	zarsrbol.c
Расчёт средней заработной платы для больничного по беременности и родам
*/
#include        <errno.h>
#include        "buhg.h"

void zarsrbol_rnd(class iceb_tu_str *kodotp,class iceb_tu_str *kodgos,class iceb_tu_str *kodtabotrv,class iceb_tu_str *kodtabotp,int *metkaknvr,class iceb_tu_str*,class iceb_tu_str*);

extern char	*shetb; /*Бюджетные счета начислений*/
extern short    *kodbl;  /*Код начисления больничного*/

int zarsrbold(short dn,short mn,short gn,short dk,short mk,short gk,const char *tabnom,const char *fam,const char *inn,class spis_oth *oth)
{
char		strsql[1024];
SQL_str		row,row1;
int		kolstr=0;
int		kolmes=0; //Количество месяцев в периоде
double		suma=0.;
time_t		tmmn;
short		kkk;
char		imaf[64],imafprot[64];
FILE		*ff,*ffprot;
class iceb_tu_str kodotp(""); //Коды отпускных
class iceb_tu_str kodgos(""); //Коды гос.обязанности
class iceb_tu_str kodtabotrv(""); //Коды табеля отработанного времени
class iceb_tu_str kodtabotp(""); //Коды табеля отпускных
class iceb_tu_str kod_ed_viplat("");
short		knah=0;
short		dr,mr,gr;
short		metkab=0; //0-не бюджет 1 бюджет

time(&tmmn);

kolmes=period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_t_soob(gettext("Количество месяцев в периоде равно нолю !"));
  return(1);

 }

clear();
printw("%s\n",gettext("Расчёт больничных по беременности и родам"));
printw("%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,tabnom,fam);
printw("%s:%d\n",gettext("Количество месяцев"),kolmes);


GDITE();
double nash[kolmes]; //Всего начислено
double otpusk[kolmes]; //Отпускные
double gosob[kolmes]; //Оплата гособязанности
int    kolkk[kolmes]; //Количество календарных дней
int kolih_kk_v_mes[kolmes]; /*Количество каледарных дней в месяце*/
float kolrd_mes[kolmes];  //Количество рабочих дней в месяце по календарю
float kolrh_mes[kolmes]; //Количесво рабочих часов в месяце
int    kolotp[kolmes]; //Количество отпускных
int kolddpev[kolmes];  /*Количество дней для перерасчёта единоразовых выплат*/

double sum_ed_viplat[kolmes]; //Суммы единоразовых выплат
double sum_ed_viplat_b[kolmes]; //Суммы единоразовых выплат - бюджет
double sum_ed_viplat_h[kolmes]; //Суммы единоразовых выплат - хозрасчёт


double bnash[kolmes]; //Всего начислено (бюджет)
double botpusk[kolmes]; //Отпускные  (бюджет)
double bgosob[kolmes]; //Оплата гособязанности  (бюджет)

double nbnash[kolmes]; //Всего начислено  (небюджет)
double nbotpusk[kolmes]; //Отпускные  (небюджет)
double nbgosob[kolmes]; //Оплата гособязанности  (небюджет)

double metka_bol[kolmes]; //Метка больничного общая
double metka_bol_hoz[kolmes]; //Метка больничного для хозрасчёта
double metka_bol_bd[kolmes]; //Метка больничного для бюджета

memset(kolrd_mes,'\0',sizeof(kolrd_mes));
memset(kolrh_mes,'\0',sizeof(kolrh_mes));
memset(kolih_kk_v_mes,'\0',sizeof(kolih_kk_v_mes));
memset(metka_bol,'\0',sizeof(metka_bol));
memset(metka_bol_hoz,'\0',sizeof(metka_bol_hoz));
memset(metka_bol_bd,'\0',sizeof(metka_bol_bd));

memset(sum_ed_viplat,'\0',sizeof(sum_ed_viplat));
memset(sum_ed_viplat_b,'\0',sizeof(sum_ed_viplat_b));
memset(sum_ed_viplat_h,'\0',sizeof(sum_ed_viplat_h));

memset(nash,'\0',sizeof(nash));
memset(otpusk,'\0',sizeof(otpusk));
memset(gosob,'\0',sizeof(gosob));
memset(kolkk,'\0',sizeof(kolkk));
memset(kolddpev,'\0',sizeof(kolddpev));
memset(kolotp,'\0',sizeof(kolotp));

memset(bnash,'\0',sizeof(bnash));
memset(botpusk,'\0',sizeof(botpusk));
memset(bgosob,'\0',sizeof(bgosob));

memset(nbnash,'\0',sizeof(nbnash));
memset(nbotpusk,'\0',sizeof(nbotpusk));
memset(nbgosob,'\0',sizeof(nbgosob));


int metkaknvr=0;
class iceb_tu_str kodtdpev("");
//Читаем настройки
zarsrbol_rnd(&kodotp,&kodgos,&kodtabotrv,&kodtabotp,&metkaknvr,&kod_ed_viplat,&kodtdpev);


//Читаем календарь за период и заполняем массивы количества рабочих часов и дней
short d=1,m=mn,g=gn;
for(int shmes=0; shmes < kolmes ; shmes++)
 {
  dpm(&d,&m,&g,5); /*получить последний день*/
  kolih_kk_v_mes[shmes]=d;
  dpm(&d,&m,&g,3); /*увеличть дату на месяц*/
 }
//Читаем календарь за период и заполняем массивы количества рабочих часов и дней
d=1;
m=mn;
g=gn;
for(int shmes=0; shmes < kolmes ; shmes++)
 {
  redkal(m,g,&kolrd_mes[shmes],&kolrh_mes[shmes]);
  dpm(&d,&m,&g,3);
 }

sprintf(imaf,"sz%s_%d.lst",tabnom,getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imafprot,"szp%s_%d.lst",tabnom,getpid());
if((ffprot = fopen(imafprot,"w")) == NULL)
 {
  error_op_nfil(imafprot,errno,"");
  return(1);
 }
 
iceb_t_zagolov(gettext("Расчёт больничных по беременности и родам"),dn,mn,gn,dk,mk,gk,ff);
fprintf(ff,"%s %s\n",tabnom,fam);
fprintf(ff,"%s:%d\n",gettext("Количество месяцев"),kolmes);

if(kodgos.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды не вошедшие в расчёт"),kodgos.ravno());
  int klst=0;
  if(metkaknvr == 1)
   printcod(ff,"Nash","kod","naik",0,kodgos.ravno(),&klst);
 }
if(kod_ed_viplat.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n","Коды единоразовых выплат",kod_ed_viplat.ravno());
 
 }
if(kodtdpev.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Коды табеля для перерасчёта одноразовых выплат"),kodtdpev.ravno());

fprintf(ffprot,"\x1b\x6C%c",5); /*Установка левого поля*/
iceb_t_zagolov(gettext("Расчёт больничных по беременности и родам"),dn,mn,gn,dk,mk,gk,ffprot);
fprintf(ffprot,"%s %s\n",tabnom,fam);
fprintf(ffprot,"%s:%d\n",gettext("Количество месяцев"),kolmes);

SQLCURSOR cur;
SQLCURSOR curr;
dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(ffprot,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%s and \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and prn='1' and suma <> 0.",
  tabnom,gr,mr,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
      knah=atoi(row[0]);
      suma=atof(row[1]);    
      
      
      sprintf(strsql,"select naik from Nash where kod=%d",knah);
      sql_readkey(&bd,strsql,&row1,&curr);
      
      fprintf(ffprot,"%2d %-40s %8.2f\n",knah,row1[0],suma);
             

      metkab=0;
      if(proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(proverka(kodotp.ravno(),row[0],0,1) == 0)
       {
        fprintf(ffprot,"%s !\n",gettext("Отпускные"));      
        otpusk[kkk]+=suma;
        if(metkab == 1)
          botpusk[kkk]+=suma;
        else        
          nbotpusk[kkk]+=suma;
       }
      
      if(proverka(kod_ed_viplat.ravno(),row[0],0,1) == 0) //коды единоразовых выплат
       {
        sum_ed_viplat[kkk]+=suma;
        if(metkab == 1)
         sum_ed_viplat_b[kkk]+=suma;
        else
         sum_ed_viplat_h[kkk]+=suma;
       }

      if(proverka(kodgos.ravno(),row[0],0,1) == 0) //коды не входящие в расчёт
       {
        gosob[kkk]+=suma;
        if(metkab == 1)
         bgosob[kkk]+=suma;
        else
         nbgosob[kkk]+=suma;
       }

      nash[kkk]+=suma;    
      if(metkab == 1)
       {
        bnash[kkk]+=suma;    
       }
      else      
       {
        nbnash[kkk]+=suma;    
       }
     }

    fprintf(ffprot,"%33s:%8.2f\n",gettext("Итого"),nash[kkk]);
   }  
  //Узнаем количество отработанных дней и количество дней в отпуске
  sprintf(strsql,"select kodt,dnei,kdnei,has from Ztab where \
tabn=%s and god=%d and mes=%d",tabnom,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   { 
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
    sql_readkey(&bd,strsql,&row1,&curr);
    fprintf(ffprot,"%-2s %-30s %2s %2s\n",row[0],row1[0],row[1],row[2]);

    if(proverka(kodtabotrv.ravno(),row[0],0,1) == 0)
     {
      kolkk[kkk]+=atoi(row[2]);
     }
    if(proverka(kodtabotp.ravno(),row[0],0,1) == 0)
      kolotp[kkk]+=atoi(row[2]);
    if(proverka(kodtdpev.ravno(),row[0],0,1) == 0)
      kolddpev[kkk]+=atoi(row[1]);
   }  
  dpm(&dr,&mr,&gr,3);
 }

/*смотрим есть ли больничные в счёт месяцев расчёта*/
fprintf(ffprot,"Определяем больничные в счёт месяцев расчёта\n");
dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(ffprot,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet,datz from Zarp where tabn=%s and \
godn=%d and mesn = %d and prn='1' and suma <> 0.",tabnom,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
   
      if(provkod(kodbl,row[0]) < 0)
       continue;

      fprintf(ffprot,"%s %2s %10s %6s\n",row[3],row[0],row[1],row[2]);      
      metka_bol[kkk]=1;

      metkab=0;
      if(proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(metkab == 1)
       {
        metka_bol_bd[kkk]=metka_bol[kkk];
       }
      else      
       {
        metka_bol_hoz[kkk]=metka_bol[kkk];
       }

     }
   }
   dpm(&dr,&mr,&gr,3);
 }
//Распечатываем результат


fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчёт |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

int iotr=0;
int iotr_mes=0;
int iotp=0;
double isumn=0.;
double iotpn=0.;
double igos=0.;
double ivras=0.;
double vras=0;
double ivras_h=0.;
double vras_h=0;
dr=1; mr=mn; gr=gn;
char metka_rd[8];
double suma_ed_vras;
double isuma_ed_vras=0;
double isum_ed_viplat=0;
short dnr=1,mnr=mn,gnr=gn;
class iceb_tu_str boll("");
class zar_read_tn1h nastr;

for(kkk=0; kkk < kolmes; kkk++)
 {
  suma_ed_vras=sum_ed_viplat[kkk];

  if(sum_ed_viplat[kkk] != 0.)
   {
    fprintf(ffprot,"\nЕсть сумма единоразовой выплаты %.2f в %d месяце\n\
Количество рабочих дней в месяце:%f\n\
Количество дней для перерасчёта одноразовой выплаты=%d\n",
   sum_ed_viplat[kkk],kkk+1,kolrd_mes[kkk],kolddpev[kkk]);

    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kold == 0)
       fprintf(ffprot,"Количество дней для перерасчёта одноразовой выплаты равно нолю!\n");
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat[kkk]/kolrd_mes[kkk]*kold;
      fprintf(ffprot,"*Пересчитываем единоразовую выплату=%.2f/%f*%f=%.2f\n",
      sum_ed_viplat[kkk],kolrd_mes[kkk],kold,suma_ed_vras);
     }
    }
  vras_h=vras=nash[kkk]-otpusk[kkk]-gosob[kkk]-sum_ed_viplat[kkk]+suma_ed_vras;

  if(vras < 0.)
   vras_h=vras=0.;

  //читаем настройки на этот месяц
  zar_read_tn1(1,mr,gr,&nastr);
  dpm(&dnr,&mnr,&gnr,3);
  
  if(vras > nastr.max_sum_for_soc)
    vras_h=vras=nastr.max_sum_for_soc;
  
  memset(metka_rd,'\0',sizeof(metka_rd));
  metka_rd[0]=' ';    
  if(kolkk[kkk] < kolih_kk_v_mes[kkk])
    metka_rd[0]='*';
     
  
  boll.new_plus("");
  if(metka_bol[kkk] == 1)
   boll.new_plus(gettext("Больничный")); 

  fprintf(ff,"%02d.%d %-3d%s/%5d %-7d %-3d/%4d %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
  mr,gr,kolkk[kkk]+kolotp[kkk],metka_rd,0,kolotp[kkk],kolih_kk_v_mes[kkk],0,nash[kkk],otpusk[kkk],gosob[kkk],
  sum_ed_viplat[kkk],suma_ed_vras,
  vras,boll.ravno());
  
  iotr+=kolkk[kkk]+kolotp[kkk];
  iotr_mes+=(int)kolih_kk_v_mes[kkk];
  iotp+=kolotp[kkk];
  isumn+=nash[kkk];
  iotpn+=otpusk[kkk];
  igos+=gosob[kkk];
  ivras+=vras;
  ivras_h+=vras_h;
  isuma_ed_vras+=suma_ed_vras;
  isum_ed_viplat+=sum_ed_viplat[kkk];
  
  dpm(&dr,&mr,&gr,3);
 }

if(iotr != 0)
 printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,ivras/iotr);
else
 printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,0.);

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

fprintf(ff,"%-7.7s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
gettext("Итого"),
iotr,0.,iotp,iotr_mes,0.,isumn,iotpn,igos,
isum_ed_viplat,isuma_ed_vras,
ivras);

if(iotr != 0)
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,ivras/iotr);
else
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,0.);


if(shetb != NULL)
 {
  isum_ed_viplat=0;
  isuma_ed_vras=0;
  iotr=0;
  iotp=0;
  isumn=0.;
  iotpn=0.;
  igos=0.;
  ivras=0.;
  vras=0;
  printw("%s\n",gettext("Бюджет"));
  fprintf(ff,"\n%s\n",gettext("Бюджет"));

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчёт |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  dr=1; mr=mn; gr=gn;
  for(kkk=0; kkk < kolmes; kkk++)
   {
    suma_ed_vras=sum_ed_viplat_b[kkk];
    if(suma_ed_vras != 0.)
    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat_b[kkk]/kolrd_mes[kkk]*kold;
     }


    vras_h=vras=bnash[kkk]-botpusk[kkk]-bgosob[kkk]-sum_ed_viplat_b[kkk]+suma_ed_vras;
    if(vras < 0.)
     vras_h=vras=0.;

    zar_read_tn1(1,mr,gr,&nastr);
    if(vras > nastr.max_sum_for_soc)
      vras_h=vras=nastr.max_sum_for_soc;

    memset(metka_rd,'\0',sizeof(metka_rd));
      
    metka_rd[0]=' ';    
    if(kolkk[kkk] < kolih_kk_v_mes[kkk])
      metka_rd[0]='*';
      

    boll.new_plus("");	
    if(metka_bol_bd[kkk] == 1)
     boll.new_plus(gettext("Больничный")); 
      
    fprintf(ff,"%02d.%d %-8d%s %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
    mr,gr,kolkk[kkk]+kolotp[kkk],metka_rd,kolotp[kkk],kolih_kk_v_mes[kkk],0.,bnash[kkk],botpusk[kkk],bgosob[kkk],
    sum_ed_viplat[kkk],suma_ed_vras,
    vras,boll.ravno());

    iotr+=kolkk[kkk]+kolotp[kkk];
    iotp+=kolotp[kkk];
    isumn+=bnash[kkk];
    iotpn+=botpusk[kkk];
    igos+=bgosob[kkk];
    ivras+=vras;
    isuma_ed_vras+=suma_ed_vras;
    isum_ed_viplat+=sum_ed_viplat_b[kkk];
    dpm(&dr,&mr,&gr,3);
   }

  if(iotr != 0)
   printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,0.);

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
  iceb_tu_kolbait(7,gettext("Итого")),iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,0.,iotp,iotr_mes,0.,isumn,iotpn,igos,
  isum_ed_viplat,isuma_ed_vras,
  ivras);


  if(iotr != 0)
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,0.);


  isum_ed_viplat=0;
  isuma_ed_vras=0;
  iotr=0;
  iotp=0;
  isumn=0.;
  iotpn=0.;
  igos=0.;
  ivras=0.;
  vras=0;

  printw("%s\n",gettext("Небюджет"));
  fprintf(ff,"\n%s\n",gettext("Небюджет"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчёт |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  dr=1; mr=mn; gr=gn;
  for(kkk=0; kkk < kolmes; kkk++)
   {
    suma_ed_vras=sum_ed_viplat_h[kkk];
    if(suma_ed_vras != 0.)
    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat_h[kkk]/kolrd_mes[kkk]*kold;
     }

    vras_h=vras=nbnash[kkk]-nbotpusk[kkk]-nbgosob[kkk]-sum_ed_viplat_h[kkk]+suma_ed_vras;
    if(vras < 0.)
     vras_h=vras=0.;
    zar_read_tn1(1,mr,gr,&nastr);
    if(vras > nastr.max_sum_for_soc)
      vras_h=vras=nastr.max_sum_for_soc;
      
    memset(metka_rd,'\0',sizeof(metka_rd));
      
    metka_rd[0]=' ';    
    if(kolkk[kkk] < kolih_kk_v_mes[kkk])
      metka_rd[0]='*';
      

    boll.new_plus("");
    if(metka_bol_hoz[kkk] == 1)
     boll.new_plus(gettext("Больничный")); 


    fprintf(ff,"%02d.%d %-8d%s %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
    mr,gr,kolkk[kkk]+kolotp[kkk],metka_rd,kolotp[kkk],kolih_kk_v_mes[kkk],0.,nbnash[kkk],nbotpusk[kkk],nbgosob[kkk],
    sum_ed_viplat[kkk],suma_ed_vras,
    vras,boll.ravno());

    iotr+=kolkk[kkk]+kolotp[kkk];
    iotp+=kolotp[kkk];
    isumn+=nbnash[kkk];
    iotpn+=nbotpusk[kkk];
    igos+=nbgosob[kkk];
    ivras+=vras;
    isuma_ed_vras+=suma_ed_vras;
    isum_ed_viplat+=sum_ed_viplat_h[kkk];
    dpm(&dr,&mr,&gr,3);
   }

  if(iotr != 0)
   printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),

   ivras,iotr,0.);

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
  iceb_tu_kolbait(7,gettext("Итого")),iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,0.,iotp,iotr_mes,0.,isumn,iotpn,igos,
  isum_ed_viplat,isuma_ed_vras,
  ivras);



  if(iotr != 0)
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,0.);
 }
podpis(ff);
podpis(ffprot);


fclose(ff);
fclose(ffprot);

printw_vr(tmmn);


OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт больничных по беременности и родам"));
oth->spis_imaf.plus(imafprot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}
/************************/
/*Чтение настроек*/
/*************************/
void zarsrbol_rnd(class iceb_tu_str *kodotp,class iceb_tu_str *kodgos,class iceb_tu_str *kodtabotrv,class iceb_tu_str *kodtabotp,int *metkaknvr,class iceb_tu_str *kod_ed_viplat,
class iceb_tu_str *kodtdpev) /*Коды табеля для перерасчёта единоразовой выплаты*/
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str stroka1("");

sprintf(strsql,"select str from Alx where fil='zarsrbold.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsrbold.alx");
  iceb_t_soob(strsql);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  polen(row_alx[0],&stroka1,1,'|');

  if(SRAV(stroka1.ravno(),"Коды начисления отпускных",0) == 0)
   {
    polen(row_alx[0],kodotp,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Коды не входящие в расчёт средней зарплаты для начисления больничного",0) == 0)
   {
    polen(row_alx[0],kodgos,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Коды видов табеля отработанного времени",0) == 0)
   {
    polen(row_alx[0],kodtabotrv,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Коды табеля для перерасчёта одноразовых выплат",0) == 0)
   {
    polen(row_alx[0],kodtdpev,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Коды видов табеля отпускных",0) == 0)
   {
    polen(row_alx[0],kodtabotp,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Коды единоразовых выплат",0) == 0)
   {
    polen(row_alx[0],kod_ed_viplat,2,'|');
    continue;
   }
  if(SRAV(stroka1.ravno(),"Печатать коды невходящие в расчёт",0) == 0)
   {
    polen(row_alx[0],&stroka1,2,'|');
    if(SRAV(stroka1.ravno(),"Вкл",1) == 0)
      *metkaknvr=1; 
    continue;
   }

 }
}
