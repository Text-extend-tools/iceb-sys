/*31.08.2017	10.06.2004	Белых 	raskus.c
Расчёт коммунальных услуг
*/

#include <stdio.h>
#include <iceb.h>
#include <buhg.h>
#include <unistd.h>
#include <errno.h>
class raskus_nast
 {
  public:
   class iceb_tu_str kodop;
   class iceb_tu_str kod_podr;
   
   class iceb_tu_str kod_kp;    /*код услуги*/
   class iceb_tu_str kod_kp_ei; /*единица измерения услуги*/
   class iceb_tu_str kod_kp_su; /*Счёт учёта услуги*/
   class iceb_tu_str kod_kp_ss; /*Счёт списания услуги*/

   class iceb_tu_str kod_ot;
   class iceb_tu_str kod_ot_ei;
   class iceb_tu_str kod_ot_su;
   class iceb_tu_str kod_ot_ss;

   class iceb_tu_str kod_el;
   class iceb_tu_str kod_el_ei;
   class iceb_tu_str kod_el_su;
   class iceb_tu_str kod_el_ss;

   class iceb_tu_str kod_vod;
   class iceb_tu_str kod_vod_ei;
   class iceb_tu_str kod_vod_su;
   class iceb_tu_str kod_vod_ss;

   class iceb_tu_str kod_stoki;
   class iceb_tu_str kod_stoki_ei;
   class iceb_tu_str kod_stoki_su;
   class iceb_tu_str kod_stoki_ss;

   class iceb_tu_str kod_gaz;
   class iceb_tu_str kod_gaz_ei;
   class iceb_tu_str kod_gaz_su;
   class iceb_tu_str kod_gaz_ss;

   class iceb_tu_str musor;
   class iceb_tu_str musor_ei;
   class iceb_tu_str musor_su;
   class iceb_tu_str musor_ss;
  raskus_nast()
   {
    kod_podr.plus("");
    kod_gaz.plus("");
    kodop.plus("");
    kod_kp.plus("");
    kod_ot.plus("");
    kod_el.plus("");
    kod_vod.plus("");
    kod_stoki.plus("");
    musor.plus("");

    kod_gaz_ss.plus("");
    kod_kp_ss.plus("");
    kod_ot_ss.plus("");
    kod_el_ss.plus("");
    kod_vod_ss.plus("");
    kod_stoki_ss.plus("");
    musor_ss.plus("");

    kod_gaz_su.plus("");
    kod_kp_su.plus("");
    kod_ot_su.plus("");
    kod_el_su.plus("");
    kod_vod_su.plus("");
    kod_stoki_su.plus("");
    musor_su.plus("");

    kod_gaz_ei.plus("");
    kod_kp_ei.plus("");
    kod_ot_ei.plus("");
    kod_el_ei.plus("");
    kod_vod_ei.plus("");
    kod_stoki_ei.plus("");
    musor_ei.plus("");

   }

 };

int raskus_rn(class raskus_nast *kodusl);
double raskus_so(const char *shet,const char *kontr,short mr,short gr);
int raskus_ud(short mr,short gr,const char *osnov);
void raskus_itogo(const char *koment,double *ish_dan_dom,double *summa,FILE *ff);

extern SQL_baza	bd;
extern char		*imabaz;
extern char		*putnansi;
extern float kvartp; //квартирная плата
extern float otop;   //отопление
extern float gas;    //газ
extern float vbv;    //вода без ванны
extern float vsv;    //вода с ванной
extern float sbv;    //стоки без ванны
extern float ssv;    //стоки с ванной
extern float elek;   //електроенергия
extern float nds;    //НДС
extern float cena_vodi; //Цена воды по счётчику
extern float cena_stoka; //Цена стока по счётчику
extern float musor;       /*Цена вывоза мусора на 1 человека*/
extern class iceb_tu_str shin; /*Счёт сальдо по услугам оказанных организацией*/
extern class iceb_tu_str shout; /*Счёт сальдо услугам оказанных другими организациями*/

int mfnn=1;
double	okrcn=0.01;  /*Округление цены*/
double okrg1=0.01;
void raskus()
{
class raskus_nast kodusl;

char strsql[2048];
static class iceb_tu_str nomd("");   // Номер дома
static class iceb_tu_str nomk("");   // Номер квартиры
static class iceb_tu_str data(""); /*Дата расчёта*/  
static class iceb_tu_str datdok("");
char zapros[1024];
memset (zapros,'\0',sizeof(zapros));
 
SQLCURSOR cur,cur1;
SQL_str row,row1;
int kolstr;
short mr,gr;

VVOD VV(0);
char obr[8];
memset(obr,0,sizeof(obr));

VV.VVOD_SPISOK_add_ZAG("Расчет комунальных услуг");

VV.VVOD_SPISOK_add_MD("Номер дома..............");
VV.VVOD_SPISOK_add_MD("Номер квартиры..........");
VV.VVOD_SPISOK_add_MD("Дата......(м.г).........");
VV.VVOD_SPISOK_add_MD("Дата документа (д.м.г)..");

VV.VVOD_SPISOK_add_data(nomd.ravno(),5);
VV.VVOD_SPISOK_add_data(nomk.ravno(),5);
VV.VVOD_SPISOK_add_data(data.ravno(),8);
VV.VVOD_SPISOK_add_data(datdok.ravno(),11);
 

naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Расчёт",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

nomd.new_plus(VV.VVOD_SPISOK_return_data(0));
nomk.new_plus(VV.VVOD_SPISOK_return_data(1));
data.new_plus(VV.VVOD_SPISOK_return_data(2));
datdok.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:
    break;

  case FK10:
     return;


 }

if (rsdat1(&mr,&gr,data.ravno())!=0)
 {
  iceb_t_soob("Дата введена не правильно!");
  goto naz;
 }
if(datdok.getdlinna() > 1)
 {
   if(datdok.prov_dat() != 0)
    {
     iceb_t_soob("Неправильно введена дата документа!");
     goto naz;
    }
  clear();
 }

GDITE();
/*Читаем настройки для создания документов в Учёте услуг*/
if(datdok.getdlinna() > 1)
 {
  if(iceb_t_pbpds(datdok.ravno()) != 0)
   goto naz;
  if(iceb_t_pvglkni(datdok.ravno()) != 0)
   goto naz;
    
  if(raskus_rn(&kodusl) != 0)
   goto naz;
 }

class iceb_tu_str naim_00("");
class iceb_tu_str telef("");
sprintf(zapros,"select naikon,telef from Kontragent where kodkon='00'");
if(readkey(zapros,&row,&cur) == 1)
 {
  naim_00.new_plus(row[0]);
  telef.new_plus(row[1]);
 }


int metka=0;
sprintf(zapros,"select * from Kuskvart");
char bros[100];

if(nomd.ravno()[0]!='\0')
 {
  
  sprintf(bros," where nd='%s'",nomd.ravno());
  strcat(zapros,bros);    
  metka++;
 
 }
if(nomk.ravno()[0]!='\0')
 {
  if(metka == 0)
   sprintf(bros," where nk=%s",nomk.ravno());
  else
   sprintf(bros," and nk=%s",nomk.ravno());

  strcat(zapros,bros);    

 }
strcat(zapros," order by nd,fio asc");

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
if (kolstr == 0)
 {
  iceb_t_soob("Не найдено ни одной записи!!!");
  return; 
 }


class iceb_fopen fil_sh;
char imaf_sh[64];
sprintf(imaf_sh,"shet%d.lst",getpid());
if(fil_sh.start(imaf_sh,"w") != 0) 
 return;

char imaf[100];
class iceb_fopen fil_ff;
sprintf(imaf,"kusnast%d.lst",getpid());

if(fil_ff.start(imaf,"w") != 0) 
 return;

char imaf_k[100];
sprintf(imaf_k,"kusnastk%d.lst",getpid());
class iceb_fopen fil_k;
if(fil_k.start(imaf_k,"w") != 0) 
 return;

fprintf (fil_ff.ff,"Дата:%d.%d\n",mr,gr);
fprintf (fil_ff.ff,"Квартплата:%.3f\n", kvartp);
fprintf (fil_ff.ff,"Отоплення:%.3f\n",otop);
fprintf (fil_ff.ff,"Газ:%.3f\n",gas);
fprintf (fil_ff.ff,"Вода без ванни:%.3f\n",vbv);
fprintf (fil_ff.ff,"Вода з ванной:%.3f\n",vsv);
fprintf (fil_ff.ff,"Стоки без ванни:%.3f\n",sbv);
fprintf (fil_ff.ff,"Стоки з ванной:%.3f\n",ssv);
fprintf (fil_ff.ff,"Ціна води по лічільнику:%.3f\n",cena_vodi);
fprintf (fil_ff.ff,"Ціна стоків по лічільнику:%.3f\n",cena_stoka);
fprintf (fil_ff.ff,"Електроенергия:%.3f\n",elek);
fprintf (fil_ff.ff,"Сміття:%.3f\n",musor);
fprintf (fil_ff.ff,"ПДВ:%.3f\n",nds);
/***************
fprintf(fil_ff.ff,"\
-------------------------------------------------------------------------------------------------------------- --------------------------------------------------------------------------\n\
N Дома |N кв |Контр.|  Прізвище, І'мя, По-батькові      |К.ж.|Пл. кв.|Пл.кух.|Пл.бал.|Ванна|Ел.счётчик| Вода | |Утрим.буд|Опалення|Газ.плита|  Вода  |Електрі.| ТПВ  | Стоки  |  РАЗОМ  |\n\
-------------------------------------------------------------------------------------------------------------- --------------------------------------------------------------------------\n");
****************/
fprintf(fil_ff.ff,"\
-------------------------------------------------------------------------------------------------------------- --------------------------------------------------------------------------\n\
N Дома |N кв |Контр.|  Прізвище, І'мя, По-батькові      |К.ж.|Пл. кв.|Пл.кух.|Пл.бал.|Ванна|Ел.счётчик| Вода | |Утрим.буд|Опалення|Газ.плита|  Вода  | Стоки  |Електрі.| ТПВ  |  РАЗОМ  |\n\
-------------------------------------------------------------------------------------------------------------- --------------------------------------------------------------------------\n");

char nv[2];
memset (nv,0,sizeof(nv));

#define ITOGO_SUMA 8
double summa[ITOGO_SUMA];
memset(summa,0,sizeof(summa));

double isumma[ITOGO_SUMA];
memset(isumma,0,sizeof(isumma));

double isummad[ITOGO_SUMA];
memset(isummad,0,sizeof(isummad));

int ps=0;

class iceb_tu_str nomdomz("");

#define ITOGO_DOM 7
double ish_dan[ITOGO_DOM];
memset(ish_dan,'\0',sizeof(ish_dan));

double ish_dan_dom[ITOGO_DOM];
memset(ish_dan_dom,'\0',sizeof(ish_dan_dom));

int shet_kor=0; /*Счётчик для корешков*/
double kolih=0.;
double cena=0.;
short dd,md,gd;
rsdat(&dd,&md,&gd,datdok.ravno(),1);
float voda=0;
char dopol[50];
sprintf(dopol,"за %02d.%d р.",mr,gr);
int mnds=3; /*Освобождено от НДС*/
float pnds=20.;
class iceb_tu_str naimkon("");
int nomer_sh=0;
class iceb_tu_str nomshet1("");
class iceb_tu_str nomshet2("");
int kolstr1=0;
short drs=31,mrs=mr,grs=gr;
dpm(&drs,&mrs,&grs,4);
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  /*Проверяем код контрагента*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[10]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Не найден код контрагента %s для квартиры %s в доме %s",
    row[10],row[1],row[0]);           
    iceb_t_soob(strsql);
    continue;
   }

  naimkon.new_plus(row1[0]);
    
  if(SRAV(nomdomz.ravno(),row[0],0) != 0)
   {
    if(nomdomz.ravno()[0] != '\0')
     {
      raskus_itogo("Разом по дому",ish_dan_dom,isummad,fil_ff.ff);
     }

    memset(isummad,0,sizeof(isummad));
    
    nomdomz.new_plus(row[0]);
   }
   

  sprintf(zapros,"select ps,voda from Kuspm where nd='%s' and nk='%s' and data='%d-%d-%d'",
  row[0],row[1],gr,mr,1);
  
  ps=0;
  voda=0.;
  if(sql_readkey(&bd,zapros,&row1,&cur1) > 0)
   {
    ps=atoi(row1[0]);
    voda=atof(row1[1]);
   }     

  memset(summa,'\0',sizeof(summa));

  summa[0]=(atof(row[4])+atof(row[5])+atof(row[11]))*kvartp;
  summa[0]=okrug(summa[0],0.01);
  
  summa[1]=0.;  
  if(row[12][0] == '0')
   {  
    summa[1]=(atof(row[4])+atof(row[5]))*otop;
    summa[1]=okrug(summa[1],0.01);
   }
  
  
  summa[2]=atof(row[3])*gas;
  summa[2]=okrug(summa[2],0.01);  
  
  if(row[6][0] == '0')
   {
    strcpy(nv,"-");
    summa[3]=atoi(row[3])*vbv;
    summa[4]=atoi(row[3])*sbv;
   }
  if(row[6][0] == '1')
   {
    strcpy(nv, "+");
    summa[3]=atoi(row[3])*vsv;  
    summa[4]=atoi(row[3])*ssv;  
   }

  if(atoi(row[13]) != 0) /*Если есть счётчик воды*/
   {
    summa[3]=voda*cena_vodi;
    summa[4]=voda*cena_stoka;
   }   

  summa[3]=okrug(summa[3],0.01);
  summa[4]=okrug(summa[4],0.01);
  summa[5]=ps*elek;

  summa[6]=musor*atoi(row[3]);;  
  
  
  summa[7]=summa[0]+summa[1]+summa[2]+summa[3]+summa[4]+summa[5]+summa[6];

 for(int nom=0; nom < ITOGO_SUMA ; nom++)
   {
    isumma[nom]+=summa[nom];
    isummad[nom]+=summa[nom];
   }

  fprintf(fil_ff.ff,"%-7s %-5s %-*s %-*.*s %4d %7.2f %7.2f %7.2f %5s %10d %6.2f   %9.2f %8.2f %9.2f %8.2f %8.2f %6.2f %8.2f %9.2f\n",
  row[0],row[1],
  iceb_tu_kolbait(6,row[10]),
  row[10],
  iceb_tu_kolbait(35,row[2]),
  iceb_tu_kolbait(35,row[2]),
  row[2],
  atoi(row[3]), atof(row[4]),atof(row[5]),atof(row[11]), nv, ps,voda,
  summa[0],summa[1],summa[2],summa[3],summa[4],summa[5],summa[6],summa[7]);

  ish_dan_dom[0]+=atof(row[3]);
  ish_dan_dom[1]+=atof(row[4]);
  ish_dan_dom[2]+=atof(row[5]);
  ish_dan_dom[3]+=atof(row[11]);
  if (row[6][0] == '1')
    ish_dan_dom[4]+=1.;

  ish_dan_dom[5]+=ps;
  ish_dan_dom[6]+=voda;

  ish_dan[0]+=atof(row[3]);
  ish_dan[1]+=atof(row[4]);
  ish_dan[2]+=atof(row[5]);
  ish_dan[3]+=atof(row[11]);
  if (row[6][0] == '1')
    ish_dan[4]+=1.;

  ish_dan[5]+=ps;
  ish_dan[6]+=voda;

 
 

  shet_kor++;
  if(shet_kor > 3)
   {
    fprintf(fil_k.ff,"\f");
    shet_kor=1;
   }   

  fprintf(fil_k.ff,"%s Тел. %s\n",naim_00.ravno(),telef.ravno());
  fprintf(fil_k.ff,"Дата:%d.%d\n",mr,gr);
  fprintf(fil_k.ff,"Дім %s кв. %s %s %s\n",row[0],row[1],row[10],row[2]);

  fprintf(fil_k.ff,"Вартість квартплати:%.3f Вартіст опалення:%.3f\n",kvartp,otop);
  fprintf(fil_k.ff,"Ціна газа:%.3f Ціна воды без ванни:%.3f Ціна воды з ванной:%.3f\n",gas,vbv,vsv);
  fprintf(fil_k.ff,"Ціна электроенергії:%.3f Ціна радіоточки:%.3f\n",elek,musor);
  fprintf(fil_k.ff,"Кількість жильців:%s Ванна:%s Кількість води:%.2f\n",row[3],nv,voda);
  fprintf(fil_k.ff,"Показання електро-лічільника:%d Площа опалення:%s Площа кухні:%s\n",
  ps,row[4],row[5]);
  
  fprintf(fil_k.ff,"\nКварт.плата   %10.2f=(%s+%s)*%.2f\n",summa[0],row[4],row[5],kvartp);
  if(row[12][0] == '0')
   fprintf(fil_k.ff,"Опалення      %10.2f=(%s+%s)*%.2f\n",summa[1],row[4],row[5],otop);
  else
   fprintf(fil_k.ff,"Опалення      %10.2f=Автономне\n",0.);
  
  fprintf(fil_k.ff,"Газ.плита     %10.2f=%s*%.2f\n",summa[2],row[3],gas);
  fprintf(fil_k.ff,"Сміття        %10.2f=%.2f*%d\n",summa[5],musor,atoi(row[3]));

  if(voda != 0)
   {
    fprintf(fil_k.ff,"Вода          %10.2f=%.2f*%.2f\n",summa[3],voda,cena_vodi);
    fprintf(fil_k.ff,"Стоки         %10.2f=%.2f*%.2f\n",summa[6],voda,cena_stoka);
   }
  else
   {
    if (row[6][0] == '0')
     {
      fprintf(fil_k.ff,"Вода          %10.2f=%s*%.2f\n",summa[3],row[3],vbv);
      fprintf(fil_k.ff,"Стоки         %10.2f=%s*%.2f\n",summa[6],row[3],sbv);
     }

    if (row[6][0] == '1')
     {
      fprintf(fil_k.ff,"Вода          %10.2f=%s*%.2f\n",summa[3],row[3],vsv);
      fprintf(fil_k.ff,"Стоки         %10.2f=%s*%.2f\n",summa[6],row[3],ssv);
     }
   }  
  fprintf(fil_k.ff,"Разом         %10.2f\n",summa[7]);

  fprintf(fil_k.ff,"\n-----------------------------------------------------------\n\n");


  #define RAZMER_LG 5
  double lgoti[RAZMER_LG];
  memset(lgoti,'\0',sizeof(lgoti));
    
  /*читаем льготы по квартире*/
  sprintf(strsql,"select ub,op,ee,vd,gz from Kuslg where nd=%s and nk='%s' and dt <= '%04d-%02d-01' order by dt desc limit 1",row[0],row[1],gr,mr);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    for(int nom=0; nom < RAZMER_LG; nom++)
      lgoti[nom]=atof(row1[nom]);
   }    

  nomshet1.new_plus("");
  nomshet2.new_plus("");  
  
  if(datdok.getdlinna() > 1) /*Создание документов*/
   {
    sprintf(strsql,"Наші послуги за %02d.%04d р. Дом %s Кв. %s",mr,gr,row[0],row[1]);
    class iceb_tu_str osnov1(strsql);

    sprintf(strsql,"Сторонні послуги за %02d.%04d р. Дом %s Кв. %s",mr,gr,row[0],row[1]);
    class iceb_tu_str osnov2(strsql);
   
       
    if(raskus_ud(md,gd,osnov1.ravno()) != 0)
     return;
    if(raskus_ud(md,gd,osnov2.ravno()) != 0)
     return;


    class iceb_tu_str nomdok("");
    /*создаём документ на услуги которые оказывает организация*/
    class lock_tables kkkk("LOCK TABLES Usldokum WRITE");

    iceb_t_nomnak(datdok.ravno_god(),kodusl.kod_podr.ravno(),&nomdok,2,1,0);

    nomshet1.new_plus(nomdok.ravno());

    sprintf(strsql,"insert into Usldokum (tp,datd,podr,kontr,nomd,kodop,nds,ktoi,vrem,pn,osnov)\
 values(2,'%s',%d,'%s','%s','%s',%d,%d,%ld,%.2f,'%s')",
    datdok.ravno_sqldata(),kodusl.kod_podr.ravno_atoi(),row[10],nomdok.ravno(),kodusl.kodop.ravno(),mnds,
    getuid(),time(NULL),pnds,osnov1.ravno());
    
    if(sql_zapis(strsql,1,0) != 0)
     continue;
    
    kkkk.unlock();

    printw("Номер документа-%s\n",nomdok.ravno());
    refresh();
    if(summa[0] != 0.) /*Квартирная плата*/
     {    
//      cena=kvartp;
//      kolih=atof(row[4])+atof(row[5]);
      kolih=1.;
      cena=summa[0]-lgoti[0];        
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_kp.ravno_atoi(),kolih,cena,kodusl.kod_kp_ei.ravno(),kodusl.kod_kp_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_kp_ss.ravno(),dopol);

     }

    if(summa[1] != 0.) /*Отопление*/
     {    
//      cena=otop;
//      kolih=atof(row[4])+atof(row[5]);
      cena=summa[1]-lgoti[1];
      kolih=1.;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_ot.ravno_atoi(),kolih,cena,kodusl.kod_ot_ei.ravno(),kodusl.kod_ot_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_ot_ss.ravno(),dopol);

     }

    /*подтверждаем записи в документе*/
    uslpoddok(dd,md,gd,dd,md,gd,nomdok.ravno(),kodusl.kod_podr.ravno_atoi(),2,1);
    /*Проверка подтверждения документа*/
    podudok(dd,md,gd,nomdok.ravno(),kodusl.kod_podr.ravno_atoi(),2);

    prosprus(1,kodusl.kod_podr.ravno_atoi(),dd,md,gd,nomdok.ravno(),2,mnds,kodusl.kodop.ravno(),pnds);
    /*Выполнение проводок*/
    avtprousl1(2,dd,md,gd,nomdok.ravno(),NULL);

    /*Проверка выполнения проводок*/

    prosprus(0,kodusl.kod_podr.ravno_atoi(),dd,md,gd,nomdok.ravno(),2,mnds,kodusl.kodop.ravno(),pnds);

    /*создаём документ на услуги которые оказывают сторонние организации*/
    kkkk.lock("LOCK TABLES Usldokum WRITE");

    iceb_t_nomnak(datdok.ravno_god(),kodusl.kod_podr.ravno(),&nomdok,2,1,0);

    nomshet2.new_plus(nomdok.ravno());

    sprintf(strsql,"insert into Usldokum (tp,datd,podr,kontr,nomd,kodop,nds,ktoi,vrem,pn,osnov)\
 values(2,'%s',%d,'%s','%s','%s',%d,%d,%ld,%.2f,'%s')",
    datdok.ravno_sqldata(),kodusl.kod_podr.ravno_atoi(),row[10],nomdok.ravno(),kodusl.kodop.ravno(),mnds,
    getuid(),time(NULL),pnds,osnov2.ravno());
    
    if(sql_zapis(strsql,1,0) != 0)
     continue;

    printw("Номер документа-%s\n",nomdok.ravno());
    
    kkkk.unlock();

    if(summa[2] != 0.) /*Газ*/
     {    
//      cena=gas;
//      kolih=atof(row[3]);
      cena=summa[2]-lgoti[4];
      kolih=1.;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_gaz.ravno_atoi(),kolih,cena,kodusl.kod_gaz_ei.ravno(),kodusl.kod_gaz_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_gaz_ss.ravno(),dopol);

     }
    if(summa[4] != 0.) /*Електричество*/
     {
//      cena=elek;
//      kolih=ps;
      cena=summa[4]-lgoti[2];
      kolih=1.;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_el.ravno_atoi(),kolih,cena,kodusl.kod_el_ei.ravno(),kodusl.kod_el_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_el_ss.ravno(),dopol);
     }
 
    if(summa[5] != 0.) //Мусор
     {    
      cena=summa[5];
      kolih=1;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.musor.ravno_atoi(),kolih,cena,kodusl.musor_ei.ravno(),kodusl.musor_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.musor_ss.ravno(),dopol);

     }

    if(summa[3] != 0.) /*Вода*/
     {    
      cena=summa[3]-lgoti[3];
      kolih=1.;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_vod.ravno_atoi(),kolih,cena,kodusl.kod_vod_ei.ravno(),kodusl.kod_vod_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_vod_ss.ravno(),dopol);

     }
    if(summa[6] != 0.) /*Стоки*/
     {    
      cena=summa[6];
      kolih=1.;
      if(cena > 0.)
       zapuvdok(2,dd,md,gd,nomdok.ravno(),1,kodusl.kod_stoki.ravno_atoi(),kolih,cena,kodusl.kod_stoki_ei.ravno(),kodusl.kod_stoki_su.ravno(),kodusl.kod_podr.ravno_atoi(),kodusl.kod_stoki_ss.ravno(),dopol);

     }

    /*подтверждаем записи в документе*/
    uslpoddok(dd,md,gd,dd,md,gd,nomdok.ravno(),kodusl.kod_podr.ravno_atoi(),2,1);
    /*Проверка подтверждения документа*/
    podudok(dd,md,gd,nomdok.ravno(),kodusl.kod_podr.ravno_atoi(),2);

    prosprus(1,kodusl.kod_podr.ravno_atoi(),dd,md,gd,nomdok.ravno(),2,mnds,kodusl.kodop.ravno(),pnds);
    /*Выполнение проводок*/
    avtprousl1(2,dd,md,gd,nomdok.ravno(),NULL);

    /*Проверка выполнения проводок*/

    prosprus(0,kodusl.kod_podr.ravno_atoi(),dd,md,gd,nomdok.ravno(),2,mnds,kodusl.kodop.ravno(),pnds);
    
   }

  /*формирование распечатки счетов*/
  double deb=0.,kre=0.;
  double saldon=0.;  
  double ioplata=0.;
  double oplata=0.;
  double suma_opl=0.;

  if(nomer_sh > 0)
   fprintf(fil_sh.ff,"\f");
  nomer_sh++;

  fprintf(fil_sh.ff,"\
                         Рахунок %s\n\
           за надані комунальні послуги за %d.%d р.\n\n\
Платник: %s\n",nomshet1.ravno(),mr,gr,naimkon.ravno());
  fprintf(fil_sh.ff,"\n\
Отримувач: %s\n\
Рахунок:31250219207918\nМФО:820172\nБанк:Державна казначейська служба України\nКод:35725833\n",naim_00.ravno());   

  /*узнаём сальдо*/
  memset(strsql,'\0',sizeof(strsql));
  salork(shin.ravno(),row[10],&deb,&kre,0,strsql,strsql,"\0",1,1,grs,31,mrs,grs,"\0",0,strsql,strsql);
  saldon=deb-kre;
 
    


  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  fprintf(fil_sh.ff,"\
Найменування послуги|Нараховано| Пільга   |  Разом   |\n");
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");

  sprintf(strsql,"Сальдо на 01.%02d.%04d",mr,gr);
  fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,strsql),strsql,saldon);
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  

  oplata=summa[0]-lgoti[0];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Утримання будинку"),"Утримання будинку",summa[0],lgoti[0],oplata);

  oplata=summa[1]-lgoti[1];

  if(oplata < 0.)
   oplata=0.;
  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Опалення"),"Опалення",summa[1],lgoti[1],oplata);
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Разом"),"Разом",ioplata);  

  suma_opl=raskus_so(shin.ravno(),row[10],mr,gr);

  fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Оплачено"),"Оплачено",suma_opl);
  double saldok=0.;
  saldok=saldon+ioplata-suma_opl;
  if(saldok > 0.)
   fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"До оплати"),"До оплати",saldok);
  else
   fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Сальдо"),"Сальдо",saldok);
     


  fprintf(fil_sh.ff,"\n\n\
                         Рахунок %s\n\
           за надані комунальні послуги за %d.%d р.\n\n\
Платник: %s\n",nomshet2.ravno(),mr,gr,naimkon.ravno());
  fprintf(fil_sh.ff,"\n\
Отримувач: %s\n\
Рахунок:35229219007918\nМФО:820172\nБанк:Державна казначейська служба України\nКод:35725833\n",naim_00.ravno());   
  
  /*узнаём сальдо*/
  memset(strsql,'\0',sizeof(strsql));
  deb=kre=0.;
  salork(shout.ravno(),row[10],&deb,&kre,0,strsql,strsql,"\0",1,1,grs,31,mrs,grs,"\0",0,strsql,strsql);
  saldon=deb-kre;
//  printw("%s-deb=%.2f kre=%.2f %s %s\n",__FUNCTION__,deb,kre,shout.ravno(),row[10]);
  

  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  fprintf(fil_sh.ff,"\
Найменування послуги|Нараховано| Пільга   |  Разом   |\n");
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  sprintf(strsql,"Сальдо на 01.%02d.%04d",mr,gr);
  fprintf(fil_sh.ff,"\
%*s|%10.2f|\n",iceb_tu_kolbait(42,strsql),strsql,saldon);
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");



  ioplata=0.;
  oplata=summa[4]-lgoti[2];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Електроенергія"),"Електроенергія",summa[4],lgoti[2],oplata);

  oplata=summa[3]-lgoti[3];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Вода"),"Вода",summa[3],lgoti[3],oplata);

  oplata=summa[6];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Стоки"),"Стоки",summa[6],0.,oplata);


  oplata=summa[2]-lgoti[4];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Газ"),"Газ",summa[2],lgoti[4],oplata);

  oplata=summa[5];

  if(oplata < 0.)
   oplata=0.;

  ioplata+=oplata;   

  fprintf(fil_sh.ff,"%-*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(20,"Вивезення ТПВ"),"Вивезення ТПВ",summa[5],0.,oplata);
  fprintf(fil_sh.ff,"\
------------------------------------------------------\n");
  fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Разом"),"Разом",ioplata);  

  suma_opl=raskus_so(shout.ravno(),row[10],mr,gr);

  fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Оплачено"),"Оплачено",suma_opl);
  saldok=0.;
  saldok=saldon+ioplata-suma_opl;
  if(saldok > 0.)
   fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"До оплати"),"До оплати",saldok);
  else
   fprintf(fil_sh.ff,"%*s|%10.2f|\n",iceb_tu_kolbait(42,"Сальдо"),"Сальдо",saldok);

 }
raskus_itogo("Разом по дому",ish_dan_dom,isummad,fil_ff.ff);

raskus_itogo("Загальний підсумок",ish_dan,isumma,fil_ff.ff);

podpis(fil_ff.ff);

podpis(fil_k.ff);


fil_ff.end();
fil_k.end();
fil_sh.end();

if(datdok.getdlinna() > 1)
 OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Разрахунок комунальних послуг");
oth.spis_imaf.plus(imaf_k);
oth.spis_naim.plus("Корешки");
oth.spis_imaf.plus(imaf_sh);
oth.spis_naim.plus("Рахунки");


for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);


iceb_t_rabfil(&oth,"");

}


/***************************/
/*Чтение настроек*/
/************************/
int raskus_rn(class raskus_nast *kodusl)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

char stroka[1024];
FILE *ff;

if(iceb_t_alxout("kusnast.alx") != 0)
 return(1);

if((ff=fopen("kusnast.alx","r")) == NULL)
 {
  error_op_nfil("kusnast.alx",errno,0);
  return(1);
 }

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#')
   continue;

  if(SRAV("Код оплаты за квартиру|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_kp,2,'|');
    polen(stroka,&kodusl->kod_kp_ss,3,'|');

    
   }
  if(SRAV("Код oплаты за отопление|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_ot,2,'|');
    polen(stroka,&kodusl->kod_ot_ss,3,'|');
   }
  if(SRAV("Код оплаты за газ|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_gaz,2,'|');
    polen(stroka,&kodusl->kod_gaz_ss,3,'|');
   }
  if(SRAV("Код оплаты за воду|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_vod,2,'|');
    polen(stroka,&kodusl->kod_vod_ss,3,'|');
   }
  if(SRAV("Код оплаты за стоки|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_stoki,2,'|');
    polen(stroka,&kodusl->kod_stoki_ss,3,'|');
   }
  if(SRAV("Код оплаты за електричество|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_el,2,'|');
    polen(stroka,&kodusl->kod_el_ss,3,'|');
   }
  if(SRAV("Код оплаты за мусор|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->musor,2,'|');
    polen(stroka,&kodusl->musor_ss,3,'|');
   }
  if(SRAV("Код операции|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kodop,2,'|');
   }
  if(SRAV("Код подразделения|",stroka,1) == 0)
   {
    polen(stroka,&kodusl->kod_podr,2,'|');
   }
 }

fclose(ff);
unlink("kusnast.alx");


sprintf(strsql,"select kod from Usloper2 where kod='%s'",kodusl->kodop.ravno());
if(readkey(strsql,&row,&cur) != 1)
  {
   sprintf(strsql,"Не найден код операции %s !",kodusl->kodop.ravno());
  iceb_t_soob(strsql);
   return(1);
  }

sprintf(strsql,"select kod from Uslpodr where kod=%d",kodusl->kod_podr.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
  {
   sprintf(strsql,"Не найден код подразделения %d !",kodusl->kod_podr.ravno_atoi());
  iceb_t_soob(strsql);
   return(1);
  }

/*кварт-плата*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_kp.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_kp.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_kp.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_kp.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
OPSHET reksh;
if(kodusl->kod_kp_ss.getdlinna() > 1)
if(prsh1(kodusl->kod_kp_ss.ravno(),&reksh) != 0)
 return(1);

kodusl->kod_kp_ei.new_plus(row[0]);
kodusl->kod_kp_su.new_plus(row[1]);


/*отопление*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_ot.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_ot.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_ot.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_ot.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->kod_ot_ss.getdlinna() > 1)
if(prsh1(kodusl->kod_ot_ss.ravno(),&reksh) != 0)
 return(1);

kodusl->kod_ot_ei.new_plus(row[0]);
kodusl->kod_ot_su.new_plus(row[1]);


/*газ*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_gaz.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_gaz.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_gaz.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_gaz.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->kod_gaz_ss.getdlinna() > 1)
if(prsh1(kodusl->kod_gaz_ss.ravno(),&reksh) != 0)
 return(1);
kodusl->kod_gaz_ei.new_plus(row[0]);
kodusl->kod_gaz_su.new_plus(row[1]);


/*вода*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_vod.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_vod.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_vod.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_vod.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->kod_vod_ss.getdlinna() > 1)
if(prsh1(kodusl->kod_vod_ss.ravno(),&reksh) != 0)
 return(1);
kodusl->kod_vod_ei.new_plus(row[0]);
kodusl->kod_vod_su.new_plus(row[1]);

/*стоки*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_stoki.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_stoki.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_stoki.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_stoki.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->kod_stoki_ss.getdlinna() > 1)
if(prsh1(kodusl->kod_stoki_ss.ravno(),&reksh) != 0)
 return(1);
kodusl->kod_stoki_ei.new_plus(row[0]);
kodusl->kod_stoki_su.new_plus(row[1]);


/*мусор*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->musor.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->musor.ravno_atoi());
  iceb_t_soob(strsql);
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->musor.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->musor.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->musor_ss.getdlinna() > 1)
if(prsh1(kodusl->musor_ss.ravno(),&reksh) != 0)
 return(1);
kodusl->musor_ei.new_plus(row[0]);
kodusl->musor_su.new_plus(row[1]);


/*Електроэнергия*/
sprintf(strsql,"select ei,shetu from Uslugi where kodus=%d",kodusl->kod_el.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден код услуги %d!",kodusl->kod_el.ravno_atoi());
  iceb_t_soob(strsql);
  
  return(1);
 }

if(row[0][0] == '\0')
 {
  sprintf(strsql,"Не введена единица измерения для услуги %s !",kodusl->kod_el.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(row[1][0] == '\0')
 {
  sprintf(strsql,"Не введён счёт учёта для услуги %s !",kodusl->kod_el.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
if(kodusl->kod_el_ss.getdlinna() > 1)
 if(prsh1(kodusl->kod_el_ss.ravno(),&reksh) != 0)
  return(1);
kodusl->kod_el_ei.new_plus(row[0]);
kodusl->kod_el_su.new_plus(row[1]);

return(0);
}

/*********************************************/
/*определяем сумму оплаты*/
/*************************************/
double raskus_so(const char *shet,const char *kontr,short mr,short gr)
{
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
sprintf(strsql,"select kre from Prov where val=0 and datp >= '%04d-%02d-01' and datp <= '%04d-%02d-31' and \
sh='%s' and kodkon='%s' and kre <> 0.",gr,mr,gr,mr,shet,kontr);


//printw("%s-%s\n",__FUNCTION__,strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0.);
 }
if(kolstr == 0)
  return(0.);


while(cur.read_cursor(&row) != 0)
 {
//  printw("%s-%s\n",__FUNCTION__,row[0]);
  
  suma+=atof(row[0]);
 }
return(suma);
}
/*******************************************/
/*Удаление документов*/
/*****************************************/
int raskus_ud(short mr,short gr,const char *osnov)
{

char strsql[1024];
int kolstr=0.;
SQL_str row;
class SQLCURSOR cur;
short d=0,m=0,g=0;
sprintf(strsql,"select datd,podr,nomd,tp from Usldokum where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' \
and osnov='%s'",gr,mr,gr,mr,osnov);
//printw("%s-%s\n",__FUNCTION__,strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
printw("%s-kolstr=%d\n",__FUNCTION__,kolstr);
refresh();
if(kolstr == 0)
  return(0); 

while(cur.read_cursor(&row) != 0)
 {
//  printw("Удаляем документ %s %s\n",iceb_tu_datzap(row[0]),row[2]);
  rsdat(&d,&m,&g,row[0],2);
  if(udprgr(gettext("УСЛ"),d,m,g,row[2],atoi(row[1]),atoi(row[3])) != 0)
   return(1);       
  if(uduvdok(atoi(row[3]),d,m,g,row[2],atoi(row[1]),0,0,0) != 0)
   return(1);
 }

return(0);
}
/***********************************/
/*распечатка строки итога*/
/*********************************/

void raskus_itogo(const char *koment,double *ish_dan_dom,double *summa,FILE *ff)
{

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------- --------------------------------------------------------------------------\n");

fprintf(ff,"%*s %4.4g %7.2f %7.2f %7.2f %5.5g %10.10g %6.2f   %9.2f %8.2f %9.2f %8.2f %8.2f %6.2f %8.2f %9.2f\n\n",
iceb_tu_kolbait(56,koment), 
koment, 
ish_dan_dom[0],ish_dan_dom[1],ish_dan_dom[2],ish_dan_dom[3],ish_dan_dom[4],ish_dan_dom[5],ish_dan_dom[6],
summa[0], summa[1], summa[2], summa[3],
summa[4], summa[5], summa[6],summa[7]);

memset(ish_dan_dom,'\0',sizeof(double)*ITOGO_DOM);

}