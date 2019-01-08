/*$Id: zarpensm1.c,v 5.32 2013/12/31 11:48:12 sasa Exp $*/
/*04.09.2018 	21.01.2010	Белых А.И.	zarpensm1.c
Місячний звіт до системи персоніфікованного обліку відомостей про
застрахованих осіб

*/
#include <errno.h>
#include "buhg.h"

class zarpensm1_nastr
 {
  public: 
   class iceb_tu_str kodpfu; /*код пенсионного фонда Украины*/
   class iceb_tu_str kodfil; /*код филиала*/
   class iceb_tu_str kodzvna; /*код звания неатестованых*/
   class iceb_tu_str kod_tab_bol; /*Коды табеля больничных - тимчасової непрацездатності (допускаються від'ємні значення)*/
   class iceb_tu_str kod_tab_bsz; /*Коды табеля без сохранения зарплаты (допускаються від'ємні значення)*/
   class iceb_tu_str kod_tab_kdtv; /*Коды табеля кількість днів перебування у трудових ЦП відносинах (допускаються від'ємні значення)*/
   
   class iceb_tu_str kod_nah_berem; /*Коды начисления помощи по беременности и родам*/
   class iceb_tu_str kod_tab_berem; /*Код табеля по беременности и родам*/
      
   class masiv_din_int kodtn; /*коды типов начисления*/
   class SPISOK kodnah; /*начисления для кодов типов начислений*/

  zarpensm1_nastr()
   {
    kodpfu.new_plus("");
    kodfil.new_plus("");
    kodzvna.new_plus("");
    kod_tab_bol.new_plus("");
    kod_tab_bsz.new_plus("");
    kod_tab_kdtv.new_plus("");
    kod_nah_berem.new_plus("");
    kod_tab_berem.new_plus("");
   }
 };


int zarpensm1_read_nast(class zarpensm1_nastr *nastr);
void zarpensm1_kon_lst(int kolstr,const char *inn_ker,const char *fio_ker,const char *inn_gb,const char *fio_gb,FILE *ff);
int zarpensm1_t5(short mr,short gr,const char *inn_or,const char *naim_kontr00,const char *kodpfu,const char *imaf_tab5,const char *kodzvna,char *imaf_xml_tab5,FILE *ff_prot);
int zarpensm1_kdnp(int tabn,short m,short g,const char *kodtab,FILE *ff_prot);

void zarpensm1_str_tab6(int nomer_str_tab6,short mr,short gr,const char *inn,const char *fio,int metka_ukr_grom,int kodtipnah,const char *metka_pola,
const char *mv_gv,double suma_obh,double suma,double suma_esv_v,int metka_otk,
int metka_exp,const char *tabnom,int zo,int kd_np,int kd_nzp,int kd_ptv,int kd_berem,int nrm,double sum_diff,double sum_narah,FILE *ff_tab6,FILE *ff_xml_tab6);
 
void zarpensm1_str_tab7(int nomer_str_tab7,const char *data_prik,int metka_ukr_grom,const char *inn,const char *fio,const char *nomer_prik,
const char *kodss,short dn,short dk,short mr,short gr,const char *tabnom,const char *prizv,const char *ima,const char *othestvo,int koldn,
FILE *ff_tab7,FILE *ff_xml_tab7);

int zarpensm1_readtn(short mr,short gr,const char *tabnom,const char *nastr_kodzvna,class iceb_tu_str *fio,class iceb_tu_str *ima,class iceb_tu_str *othestvo,class iceb_tu_str *inn,
class iceb_tu_str *prizv,class iceb_tu_str *data_prik,class iceb_tu_str *nomer_prik,class iceb_tu_str *kodss,class iceb_tu_str *metka_pola,int *metka_otk,int *metka_exp,
int *zo,int *metka_inv,FILE *ff_prot);

double zarpensm1_snnz(int tabn,short mr,short gr,double *suma_esv_bol);
void zarpensm1_hd4(const char *inn_or,const char *naim_kontr00,short mr,short gr,FILE *ff_xml);
void zarpensm1_end_tab_xml(short dt,short mt,short gt,class iceb_t_fioruk_rk *kerivnik,class iceb_t_fioruk_rk *glavbuh,FILE *ff_xml);

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_bol_inv; /*Код удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код начисления единого социального взноса*/
extern int kodf_esv_bol_inv; /*Код начисления единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern short *kodbl;  /*Коды начисления больничного*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol_inv; /*Код фонда удержания единого социального взноса*/

int zarpensm1(class spis_oth *oth)
{
char imaf_tab5[64],imaf_tab6[64],imaf_tab7[64],imaf_prot[64];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0,kolstr1=0;
char strsql[1024];
int kom=0;
static class iceb_tu_str dat1("");
static class iceb_tu_str tabnom("");
class iceb_tu_str fio("");
class iceb_tu_str inn("");
class iceb_tu_str inn_or("");
class iceb_tu_str naim_kontr00(iceb_t_get_pnk("00",1));
class zarpensm1_nastr nastr; 
class iceb_tu_str edrpou("");
int kd_ptv=0;
int nrm=0;
int metka_ukr_grom=1;
int tbn=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт ЕСВ на общеобязательное государственное страхование"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),112);

VV.VVOD_SPISOK_add_MD(gettext("Дата........(м.г)....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,).."));//1

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("zarp4_7_1_1.txt"));
   goto naz;

  case FK2:
  case PLU:
    break;
      
  case FK3: //Просмотр списка табельных номеров
    tbn=0;
    fio.new_plus("");
    if(dirtab(&tbn,&fio,0,1) == 0)
     {
      VV.data_z_plus(1,tbn);
     }    
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5 :  
    iceb_redfil("zarpensm.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
    goto naz;
 }
short mr,gr;
if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата!"));
  goto naz;
 }

class SPISOK stab6; /*код типа начисления|месяц.год*/
class masiv_din_double tab6_sumao;
class masiv_din_double tab6_sumavr;
class masiv_din_double tab6_sumaesv;

class SPISOK data_boln; /*Дата больничного*/
class masiv_din_double suma_data_boln; /*сумма больничного по датам*/
class masiv_din_double suma_data_boln_esv; /*сумма единого социального взноса с больничного по датам*/

class SPISOK data_berem; /*Дата помощи по беременности*/
class masiv_din_double suma_data_berem; /*сумма помощи по беременности по датам*/
class masiv_din_double suma_data_berem_esv; /*сумма единого социального взноса с помощи по беременности по датам*/

zarpensm1_read_nast(&nastr); /*Читаем настройки*/

/*чтение величины минимальной зарплаты и прожиточного минимума*/
class zar_read_tn1h nastr_mz;
zar_read_tn1(1,mr,gr,&nastr_mz);

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesv();

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and prn='1' and suma <> 0.",
gr,mr,1,gr,mr,31);

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

if(readkey("select kod from Kontragent where kodkon='00'",&row1,&cur1) == 1)
 {
  inn_or.new_plus(row1[0]);
 } 

class iceb_rnfxml_data rek_zag_nn;

iceb_t_rnfxml(&rek_zag_nn); /*чтение данных для xml файлов*/

int period_type=0;

char imaf_xml_tab5[64];

char imaf_xml_tab6[64];
FILE *ff_xml_tab6;

if(iceb_t_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab6,iceb_t_getkoddok(1,mr,gr,8),1,&period_type,&rek_zag_nn,&ff_xml_tab6) != 0) /*открытие файла и получение имени файла*/
 return(1);

iceb_t_rnn_sap_xml(1,gr,mr,iceb_t_getkoddok(1,mr,gr,8),1,period_type,&rek_zag_nn,ff_xml_tab6); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or.ravno(),naim_kontr00.ravno(),mr,gr,ff_xml_tab6); /*теги с шапкой таблицы*/

char imaf_xml_tab7[64];
FILE *ff_xml_tab7;

if(iceb_t_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab7,iceb_t_getkoddok(1,mr,gr,9),1,&period_type,&rek_zag_nn,&ff_xml_tab7) != 0) /*открытие файла и получение имени файла*/
 return(1);

iceb_t_rnn_sap_xml(1,gr,mr,iceb_t_getkoddok(1,mr,gr,9),1,period_type,&rek_zag_nn,ff_xml_tab7); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or.ravno(),naim_kontr00.ravno(),mr,gr,ff_xml_tab7); /*теги с шапкой таблицы*/




sprintf(imaf_tab5,"E04T05_%d.lst",getpid());

sprintf(imaf_tab6,"E04T06_%d.lst",getpid());
class iceb_fopen ff_tab6;
if(ff_tab6.start(imaf_tab6,"w") != 0)
 return(1);

sprintf(imaf_tab7,"E04T07_%d.lst",getpid());
class iceb_fopen ff_tab7;
if(ff_tab7.start(imaf_tab7,"w") != 0)
 return(1);

sprintf(imaf_prot,"prot%d.lst",getpid());
class iceb_fopen ff_prot;
if(ff_prot.start(imaf_prot,"w") != 0)
 return(1);

if(ff_prot.ff != NULL)
 {
  fprintf(ff_prot.ff,"Коды не входящие в расчёт единого социального взноса:%s\n",knvr_esv_r.ravno());
  for(int nom=0; nom < nastr.kodtn.kolih(); nom++)
    fprintf(ff_prot.ff,"Коды начислений для кода типа начисления %d:%s\n",nastr.kodtn.ravno(nom),nastr.kodnah.ravno(nom));
  fprintf(ff_prot.ff,"Коды званий неатестованных:%s\n",nastr.kodzvna.ravno());
  fprintf(ff_prot.ff,"Коды табеля больничных:%s\n",nastr.kod_tab_bol.ravno());
  fprintf(ff_prot.ff,"Коды табеля без сохранения зарплаты:%s\n",nastr.kod_tab_bsz.ravno());
  fprintf(ff_prot.ff,"Коды табеля пребывания в трудовых отношениях:%s\n",nastr.kod_tab_kdtv.ravno());

  fprintf(ff_prot.ff,"Коды больничных:");
  if(kodbl != NULL)
    for(int nom=1; nom <= kodbl[0]; nom++)
      fprintf(ff_prot.ff,"%d ",kodbl[nom]);
 }

short dkm=1;
dpm(&dkm,&mr,&gr,5);

 


iceb_t_zagolov("Таблиця 6. Відомості про нарахування заробітку (доходу) застрахованим особам",ff_tab6.ff);
fprintf(ff_tab6.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6.ff,"\
  N  |Г|С|Номер облі|   Прізвище та ініціали ЗО    |Код |Тип |Місяць |  |  |  |  |Загальна  |Сума нарах|Сума різ|Сума утр|Сума нарах|Озна|Озна|\n\
     |р|т|кової карт|                              |кате|нара|та рік |  |  |  |  |сума нарах|увань заро|ниці між|иманого |ованого єд|ка  |ка  |\n\
     |о|а|ки застра-|                              |грії|хува|за який|  |  |  |  |ованої зар|бітку/дохо|розміром|єдиного |иного внес|наяв|наяв|\n\
     |м|т|хованої   |                              | ЗО |нь**|проведе|  |  |  |  |обітної пл|ду, в межа|мінімаль|внеску  |ку за звіт|ност|ност|\n\
     |а|ь|особи (ЗО)|                              |    |    |но нара|  |  |  |  |ати       |х максимал|ної заро|за звітн|ний місяць|і тр|і сп|\n\
     |д| |за ДРФО   |                              |    |    |хування|  |  |  |  |          |ьної велич|бітної  |ий місяц|          |удов|ецст|\n\
     |я| |   ДПА    |                              |    |    |       |  |  |  |  |          |ини на яку|плати та|ь       |          |ої к|ажу |\n\
     |н| |          |                              |    |    |       |  |  |  |  |          |нараховуют|фактично|        |          |нижк|(1-т|\n\
     |и| |          |                              |    |    |       |  |  |  |  |          |ься эдиний|нарахова|        |          |и   |ак,0|\n\
     |н| |          |                              |    |    |       |  |  |  |  |          |внесок    |ною заро|        |          |(1-т|-ні)|\n\
     | | |          |                              |    |    |       |  |  |  |  |          |          |бітною п|        |          |ак,0|    |\n\
     | | |          |                              |    |    |       |  |  |  |  |          |          |латою   |        |          |-ні)|    |\n");
fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");

double itogo_tab6[5];
memset(itogo_tab6,'\0',sizeof(itogo_tab6));



iceb_t_zagolov("Таблиця 7. Наявність підстав для обліку стажу окремим категоріям осіб відповідно до законодавства",ff_tab7.ff);
fprintf(ff_tab7.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab7.ff,"\
  N  |Г|Податковий|Код підста|   Прізвище та ініціали ЗО    |Поч|Кін|Кільк|Кількі|Норма|N нак|Дата наказ|Озна|\n\
     |р|номер або |ви для обл|                              |ато|нец|ість |сть го|трива|азу п|у про пров|ка  |\n\
     |о|серія та  |іку спецст|                              |к п|ь п|днів |дин,  |лості|ро пр|едення ате|СЕЗО|\n\
     |м|номер пасп|ажу       |                              |ері|ері|     |хвилин|робот|оведе|стації роб| Н  |\n\
     |а|орту      |          |                              |оду|оду|     |      |и для|ння а|очого місц|    |\n\
     |д|          |          |                              |   |   |     |      |її за|теста|я         |    |\n\
     |я|          |          |                              |   |   |     |      |рахув|ції р|          |    |\n\
     |н|          |          |                              |   |   |     |      |ання |обочо|          |    |\n\
     |и|          |          |                              |   |   |     |      |за по|го мі|          |    |\n\
     |н|          |          |                              |   |   |     |      |вний |сця  |          |    |\n\
     | |          |          |                              |   |   |     |      |місяц|     |          |    |\n\
     | |          |          |                              |   |   |     |      |ь    |     |          |    |\n");
fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");

double suma_nah=0.;   

double suma=0.;
double suma_esv_v=0.; /*Cумма единого социального взноса не больничного*/
double suma_esv_v_bol=0.; /*Cумма единого социального взноса больничного*/

int nomer_str_tab6=0;
double suma_obh=0.;
class iceb_tu_str metka_pola(""); /*чоловік Ч жінка Ж*/
int metka_otk=1; /*метка наличия трудовой книжки 0-нет трудовой 1-есть*/
int metka_exp=0; /*метка наличия спецстажа 0-нет 1-есть*/
int nomer_vsp=0;
int nomer_str_tab7=0;
int kodtipnah=0; /*Код типа начисления*/
int nomer_str_tab5=0;
class iceb_tu_str prizv("");
class iceb_tu_str ima("");
class iceb_tu_str othestvo("");
class iceb_tu_str kodss("");
class iceb_tu_str data_prik("");
class iceb_tu_str nomer_prik("");
short dn=1;
short dk=1;
dpm(&dk,&mr,&gr,5);
short koldn=dk-dn+1;
class SPISOK t6_tabnom; /*Список табельных номеров по которым сделана запись в таблицу 6*/
int zo=1;
int metka_inv=0;
char data_zap[16]; /*месяц и год в счёт которых выплачено*/
int kd_nzp=0;
int tabn=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  tabn=atoi(row[0]);
  /*сортировка по убыванию даты для того чтобы записи в счёт месяца расчёта попадались первыми*/
  sprintf(strsql,"select knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='1' and suma <> 0. order by godn desc,mesn desc,knah asc",
  gr,mr,1,gr,mr,31,row[0]);

  if(ff_prot.ff != NULL)
   {
    fprintf(ff_prot.ff,"\n\nТабельный номер %s\n",row[0]);
    fprintf(ff_prot.ff,"%s\n",strsql);
   }
  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  suma_obh=0.;
  suma=0.;
  stab6.free_class();
  tab6_sumao.free_class();  
  tab6_sumavr.free_class();  
  tab6_sumaesv.free_class();  

  data_boln.free_class();
  suma_data_boln.free_class();
  suma_data_boln_esv.free_class();

  data_berem.free_class();
  suma_data_berem.free_class();
  suma_data_berem_esv.free_class();

  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot.ff != NULL)
      fprintf(ff_prot.ff,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    if(proverka(knvr_esv_r.ravno(),row1[0],0,1) == 0)
     {
      if(ff_prot.ff != NULL)
        fprintf(ff_prot.ff,"Не входит в расчёт\n");
      continue;
     }    

    suma_nah=atof(row1[1]);   
    kodtipnah=0;
    
    if(provkod(kodbl,row1[0]) >= 0) /*больничные*/
     {
      if(ff_prot.ff != NULL)
       fprintf(ff_prot.ff,"Больничный\n");
       
      sprintf(strsql,"%s.%s",row1[3],row1[2]);
      
      if((nomer_vsp=data_boln.find(strsql)) < 0)
       data_boln.plus(strsql);
      suma_data_boln.plus(suma_nah,nomer_vsp);
      suma_data_boln_esv.plus(0.,nomer_vsp);
      continue;      
     }

    if(proverka(nastr.kod_nah_berem.ravno(),row1[0],0,1) == 0) /*сумма помощи по беременности и родам*/
     {
      if(ff_prot.ff != NULL)
       fprintf(ff_prot.ff,"Сумма помощи по беременности и родам\n");
       
      sprintf(strsql,"%s.%s",row1[3],row1[2]);
      
      if((nomer_vsp=data_berem.find(strsql)) < 0)
       data_berem.plus(strsql);
      suma_data_berem.plus(suma_nah,nomer_vsp);
      suma_data_berem_esv.plus(0.,nomer_vsp);
      continue;      
     }

    for(int nom=0; nom < nastr.kodtn.kolih(); nom++)
     {
      if(proverka(nastr.kodnah.ravno(nom),row1[0],0,1) == 0)
       {

        kodtipnah=nastr.kodtn.ravno(nom);
        if(ff_prot.ff != NULL)
          fprintf(ff_prot.ff,"Подходит для кода типа начисления %d\n",kodtipnah);
       }
     }


    sprintf(strsql,"%d|%s.%s",kodtipnah,row1[3],row1[2]);

    if((nomer_vsp=stab6.find(strsql)) < 0)
     stab6.plus(strsql);

    tab6_sumao.plus(suma_nah,nomer_vsp);

    tab6_sumavr.plus(0.,nomer_vsp);

    tab6_sumaesv.plus(0.,nomer_vsp);

   }



  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='2'and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  int kod_zap=0;
  suma_esv_v=suma_esv_v_bol=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"%s %s\n",row1[0],row1[1]);    

    kod_zap=atoi(row1[0]);        

    if(kod_zap == kod_esv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_bol || kod_zap == kod_esv_bol_inv)
       suma_esv_v_bol+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_inv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_dog)
       suma_esv_v+=atof(row1[1])*-1;   
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"suma_esv_v=%.2f\n",suma_esv_v);     
   }

  if(ff_prot.ff != NULL)
   fprintf(ff_prot.ff,"Единый соц.взнос = %.2f\n\
Единый соц.взнос с больничного=%.2f\n",suma_esv_v,suma_esv_v_bol);
/********************   
  if(suma_esv_v+suma_esv_v_bol == 0.)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"Нет единой социальной выплаты!\n");
    continue;
   }
****************/
  double suma_vr_tab=0.;
  int kolih_nah=stab6.kolih();
  /*Определяем общюю сумму взятую в расчёт*/
  for(int nom=0; nom < kolih_nah; nom++)
   {  
    suma=tab6_sumao.ravno(nom);
    if(suma > nastr_mz.max_sum_for_soc)
     suma=nastr_mz.max_sum_for_soc;
    suma_vr_tab+=suma;
    tab6_sumavr.plus(suma,nom);
   }

  /*разбираемся с небольничными начилениями*/  
  suma=tab6_sumavr.suma();
  double proc_per=0.;
  double suma_per=0.;
  for(int nom=0; nom < kolih_nah; nom++)
   {
    suma=tab6_sumao.ravno(nom);

    proc_per=tab6_sumavr.ravno(nom)*100./suma_vr_tab;

    suma_per=suma_esv_v*proc_per/100.;
    tab6_sumaesv.plus(suma_per,nom);
    if(ff_prot.ff != NULL)
     {
      
      fprintf(ff_prot.ff,"Процент = %.2f*100./%.2f=%.2f\n",tab6_sumavr.ravno(nom),suma_vr_tab,proc_per);
      fprintf(ff_prot.ff,"Расчёт = %.2f*%.2f/100.=%.2f\n",suma_esv_v,proc_per,suma_per);
 
     }
   }

  /*разбираемся с больничными начислениями*/
  kolih_nah=data_boln.kolih();
  suma=suma_data_boln.suma();
  for(int nom=0; nom < kolih_nah; nom++)
   {
    proc_per=suma_data_boln.ravno(nom)*100./suma;
    suma_per=suma_esv_v_bol*proc_per/100.;
    suma_data_boln_esv.plus(suma_per,nom);
   }

  /*разбираемся с помощью по беременности и родам*/
  kolih_nah=data_berem.kolih();
  suma=suma_data_berem.suma();
  for(int nom=0; nom < kolih_nah; nom++)
   {
    proc_per=suma_data_berem.ravno(nom)*100./suma;
    suma_per=suma_esv_v_bol*proc_per/100.;
    suma_data_berem_esv.plus(suma_per,nom);
   }


  if(zarpensm1_readtn(mr,gr,row[0],nastr.kodzvna.ravno(),&fio,&ima,&othestvo,&inn,&prizv,&data_prik,&nomer_prik,&kodss,
  &metka_pola,&metka_otk,&metka_exp,&zo,&metka_inv,ff_prot.ff) != 0)
   continue;

  kd_nzp=0;
  kd_ptv=0;
  nrm=0;
  kd_nzp=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_bsz.ravno(),ff_prot.ff);
  kd_ptv=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_kdtv.ravno(),ff_prot.ff);


  /*Узнаём сумму начислений на зарплату*/
  double suma_narah_bol=0.;
  double sum_narah=zarpensm1_snnz(atoi(row[0]),mr,gr,&suma_narah_bol);
  
  itogo_tab6[4]+=sum_narah+suma_narah_bol;
  
  double sum_diff=0.;

  if(tab6_sumao.suma()+ suma_data_boln.suma()+suma_data_berem.suma() < nastr_mz.minzar)
   {
    sum_diff=nastr_mz.minzar-tab6_sumao.suma()- suma_data_boln.suma()-suma_data_berem.suma();
    /*проверяем принят/уволен*/
    sprintf(strsql,"select datn,datk,sovm from Kartb where tabn=%d",tabn);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(atoi(row1[2]) == 0) /*не совместитель*/
       {
        short dnr=0,mnr=0,gnr=0;
        short dkr=0,mkr=0,gkr=0;
        rsdat(&dnr,&mnr,&gnr,row1[0],2);
        rsdat(&dkr,&mkr,&gkr,row1[1],2);
        if(sravmydat(1,mr,gr,1,mnr,gnr) == 0 || sravmydat(1,mr,gr,1,mkr,gkr) == 0)
         sum_diff=0.;
        else
         {  
          if(zarprtn(mr,gr,tabn,3) == 0)/*не работает по договору подряда*/
           {
            double sum_narahv=sum_narah*sum_diff/(tab6_sumao.suma()+sum_diff);
  
            zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,13,metka_pola.ravno(),
            data_zap,0.,0.,0.,metka_otk,metka_exp,row[0],zo,0,0,0,0,nrm,sum_diff,sum_narahv,ff_tab6.ff,ff_xml_tab6);
            sum_narah-=sum_narahv;      
           }
         }
      }
     }
   }

  itogo_tab6[2]+=sum_diff;  
  /*не больничные начисления*/
  
  for(int nom=0; nom < stab6.kolih(); nom++)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"tab6_sumao[%d]=%.2f\n\
tab6_sumavr[%d]=%.2f\n\
tab6_sumaesv[%d]=%.2f\n",
     nom,tab6_sumao.ravno(nom),
     nom,tab6_sumavr.ravno(nom),
     nom,tab6_sumaesv.ravno(nom));

    suma_obh=tab6_sumao.ravno(nom);
    itogo_tab6[0]+=suma_obh;

    double sum_narahv=sum_narah*suma_obh/tab6_sumao.suma();

    itogo_tab6[1]+=suma=tab6_sumavr.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=tab6_sumaesv.ravno(nom);
    polen(stab6.ravno(nom),&kodtipnah,1,'|');      
    polen(stab6.ravno(nom),data_zap,sizeof(data_zap),2,'|');      

    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_zap,suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,0,kd_nzp,kd_ptv,0,nrm,0.,sum_narahv,ff_tab6.ff,ff_xml_tab6);

    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     
    kd_nzp=kd_ptv=0; /*обнуляем чтобы дни печатались только в первой записи ecли записей больше одной*/
   }
  
  /*больничные начисления*/
  for(int nom=0; nom < data_boln.kolih(); nom++)
   {
    itogo_tab6[0]+=suma_obh=suma_data_boln.ravno(nom);
    itogo_tab6[1]+=suma=suma_data_boln.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=suma_data_boln_esv.ravno(nom);
    kodtipnah=0;

    if(metka_inv == 0)
      zo=29;
    else
      zo=36;
    
    short m,g;
    rsdat1(&m,&g,data_boln.ravno(nom));
    int kd_np=zarpensm1_kdnp(atoi(row[0]),m,g,nastr.kod_tab_bol.ravno(),ff_prot.ff);
    double suma_narah_bolv=suma_narah_bol*suma_obh/suma_data_boln.suma();
    
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_boln.ravno(nom),suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,kd_np,0,0,0,0,0.,suma_narah_bolv,ff_tab6.ff,ff_xml_tab6);


    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     

   }

  /*cумма помощи по беременности и родам*/

  for(int nom=0; nom < data_berem.kolih(); nom++)
   {
    itogo_tab6[0]+=suma_obh=suma_data_berem.ravno(nom);
    itogo_tab6[1]+=suma=suma_data_berem.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=suma_data_berem_esv.ravno(nom);
    kodtipnah=0;

    if(metka_inv == 0)
      zo=42;
    else
      zo=43;
    
    short m,g;
    rsdat1(&m,&g,data_berem.ravno(nom));
    int kd_berem=zarpensm1_kdnp(atoi(row[0]),m,g,nastr.kod_tab_berem.ravno(),ff_prot.ff);
    
    double suma_narah_bolv=suma_narah_bol*suma_obh/suma_data_berem.suma();
    
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_berem.ravno(nom),suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,0,0,0,kd_berem,0,0.,suma_narah_bolv,ff_tab6.ff,ff_xml_tab6);


    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     

   }
   

  if(ff_prot.ff != NULL)
   fprintf(ff_prot.ff,"Код спецстажа=%s %s %s\n",kodss.ravno(),data_prik.ravno(),nomer_prik.ravno());
   
  if(kodss.ravno()[0] != '\0') /*Таблица 7*/
   {

    zarpensm1_str_tab7(++nomer_str_tab7,data_prik.ravno(),metka_ukr_grom,inn.ravno(),fio.ravno(),nomer_prik.ravno(),kodss.ravno(),dn,dk,mr,gr,row[0],prizv.ravno(),ima.ravno(),othestvo.ravno(),koldn,ff_tab7.ff,ff_xml_tab7);

 
 
   }
    
 }
GDITE();

/*Cмотрим табельные номера у которых введены календарные дни*/
sprintf(strsql,"select distinct tabn from Ztab where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(ff_prot.ff != NULL)
  fprintf(ff_prot.ff,"\n\nПроверяем работников у которых небыло начилений но были введены календарные дни %d\n\
--------------------------------------------------------------------------------------------------------\n",kolstr);

while(cur.read_cursor(&row) != 0)
 {
  if(t6_tabnom.find(row[0]) >= 0)
   continue;
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(zarpensm1_readtn(mr,gr,row[0],nastr.kodzvna.ravno(),&fio,&ima,&othestvo,&inn,&prizv,&data_prik,&nomer_prik,&kodss,
  &metka_pola,&metka_otk,&metka_exp,&zo,&metka_inv,ff_prot.ff) != 0)
   continue;

  kd_nzp=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_bsz.ravno(),ff_prot.ff);
  kd_ptv=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_kdtv.ravno(),ff_prot.ff);

  sprintf(data_zap,"%02d.%d",mr,gr);  

  zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),data_zap,0.,0.,0.,
  metka_otk,metka_exp,row[0],zo,0,kd_nzp,kd_ptv,0,nrm,0.,0.,ff_tab6.ff,ff_xml_tab6);
  
  if(kodss.getdlinna() > 1)
   {
    zarpensm1_str_tab7(++nomer_str_tab7,data_prik.ravno(),metka_ukr_grom,inn.ravno(),fio.ravno(),nomer_prik.ravno(),kodss.ravno(),dn,dk,mr,gr,row[0],prizv.ravno(),ima.ravno(),othestvo.ravno(),koldn,ff_tab7.ff,ff_xml_tab7);
   }
 }

class iceb_t_fioruk_rk kerivnik;
class iceb_t_fioruk_rk glavbuh;
iceb_t_fioruk(1,&kerivnik);
iceb_t_fioruk(2,&glavbuh);


fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6.ff,"%*s:%10.2f %10.2f %8.2f %8.2f %10.2f\n",
iceb_tu_kolbait(78,gettext("Итого")),
gettext("Итого"),
itogo_tab6[0],itogo_tab6[1],itogo_tab6[2],itogo_tab6[3],itogo_tab6[4]);


zarpensm1_kon_lst(nomer_str_tab6,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab6.ff);


fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");

zarpensm1_kon_lst(nomer_str_tab7,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab7.ff);


    

short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);


fprintf(ff_xml_tab6,"   <R01G17>%.2f</R01G17>\n",itogo_tab6[0]);
fprintf(ff_xml_tab6,"   <R01G18>%.2f</R01G18>\n",itogo_tab6[1]);
fprintf(ff_xml_tab6,"   <R01G19>%.2f</R01G19>\n",itogo_tab6[2]);
fprintf(ff_xml_tab6,"   <R01G20>%.2f</R01G20>\n",itogo_tab6[3]);
fprintf(ff_xml_tab6,"   <R01G21>%.2f</R01G21>\n",itogo_tab6[4]);

zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab6);/*концоквка таблицы*/

fclose(ff_xml_tab6);

zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab7);/*концоквка таблицы*/

fclose(ff_xml_tab7);

podpis(ff_tab6.ff);
podpis(ff_tab7.ff);

ff_tab6.end();
ff_tab7.end();


time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

char kodedrp[16];
memset(kodedrp,'\0',sizeof(kodedrp));
strncpy(kodedrp,inn_or.ravno(),10);
for(int nom=0; nom < 10; nom++)
 if(kodedrp[nom] == '\0')
  kodedrp[nom]='Z';
char datadok[16];
sprintf(datadok,"%02d%02d%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
char vremdok[16];
sprintf(vremdok,"%02d%02d%02d",bf->tm_hour,bf->tm_min,bf->tm_sec);


char imaf_tmp[64];
sprintf(imaf_tmp,"zarpensm1%d.tmp",getpid());

nomer_str_tab5=zarpensm1_t5(mr,gr,inn_or.ravno(),naim_kontr00.ravno(),nastr.kodpfu.ravno(),imaf_tab5,nastr.kodzvna.ravno(),imaf_xml_tab5,ff_prot.ff);

if(ff_prot.ff != NULL)
 ff_prot.end();

oth->spis_imaf.plus(imaf_tab5);
oth->spis_naim.plus("Таблиця 5");

oth->spis_imaf.plus(imaf_tab6);
oth->spis_naim.plus("Таблиця 6");

oth->spis_imaf.plus(imaf_tab7);
oth->spis_naim.plus("Таблиця 7");



if(imaf_prot[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_prot);
  oth->spis_naim.plus("Протокол хода расчёта");
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

if(nomer_str_tab5 > 0)
 {
  oth->spis_imaf.plus(imaf_xml_tab5);
  oth->spis_naim.plus("Таблица 5 в формате xml");
 }
oth->spis_imaf.plus(imaf_xml_tab6);
oth->spis_naim.plus("Таблица 6 в формате xml");

if(nomer_str_tab7 > 0)
 {
  oth->spis_imaf.plus(imaf_xml_tab7);
  oth->spis_naim.plus("Таблиця 7 в форматі xml");
 }
else
 unlink(imaf_xml_tab7);
 
return(0);

}
/********************************/
/*Чтение настроек*/
/********************************/
int zarpensm1_read_nast(class zarpensm1_nastr *nastr)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
class iceb_tu_str naim("");
int kodtn=0;
sprintf(strsql,"select str from Alx where fil='zarpensm.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarpensm1.alx");
  iceb_t_soob(strsql);
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&naim,1,'|') != 0)
   continue;  
  if(SRAV("Код отделения пенсионного фонда Украины",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kodpfu,2,'|');
    continue;
   }
  if(SRAV("Код филиала пенсионного фонда",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kodfil,2,'|');
    continue;
   }

  if(SRAV("Коды начислений для кода типа начисления",naim.ravno(),1) == 0)
   {
    if(polen(naim.ravno(),&kodtn,7,' ') == 0)
     {
      if(kodtn == 0)
       continue;
      nastr->kodtn.plus(kodtn,-1);
   
      polen(row_alx[0],&naim,2,'|');
      nastr->kodnah.plus(naim.ravno());
     }   
    continue;
   }
  if(SRAV("Коды звания неатестованных",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kodzvna,2,'|');
    continue;
   }
  if(SRAV("Коды табеля больничных",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kod_tab_bol,2,'|');
    continue;
   }
  if(SRAV("Коды табеля без сохранения зарплаты",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kod_tab_bsz,2,'|');
    continue;
   }
  if(SRAV("Коды табеля прeбывания в трудовых отношениях",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kod_tab_kdtv,2,'|');
    continue;
   }

  if(SRAV("Коды начисления помощи по беременности и родам",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kod_nah_berem,2,'|');
    continue;
   }

  if(SRAV("Коды табеля по беременности и родам",naim.ravno(),0) == 0)
   {
    polen(row_alx[0],&nastr->kod_tab_berem,2,'|');
    continue;
   }
 }
return(0);
}
/************************************/
/*концовка распечатки*/
/*************************************/

void zarpensm1_kon_lst(int kolstr,
const char *inn_ker,
const char *fio_ker,
const char *inn_gb,
const char *fio_gb,
FILE *ff)
{
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ff,"\
Кількість заповнених рядків %d\n\
Дата формування звіту у страхувальника %d.%d.%d \n\n\
Керівник           %-10s _______________ %s\n\n\
M.П.\n\n\
Головний бухгалтер %-10s ________________ %s\n",
kolstr,
dt,mt,gt,
inn_ker,
fio_ker,
inn_gb,
fio_gb); 


}

/*********************************************/
/*вывод в таблицу 6*/
/*******************************/
void zarpensm1_str_tab6(int nomer_str_tab6,
short mr,short gr, /*дата расчёта*/
const char *inn,
const char *fio,
int metka_ukr_grom,
int kodtipnah,
const char *metka_pola,
const char *mv_gv, /*Месяц и год в счёт которых выплачено*/
double suma_obh,
double suma,
double suma_esv_v,
int metka_otk,
int metka_exp,
const char *tabnom,
int zo,
int kd_np,
int kd_nzp,
int kd_ptv,
int kd_berem,
int nrm, 
double sum_diff,
double sum_narah,
FILE *ff_tab6,
FILE *ff_xml_tab6)
{
short mv=0,gv=0;
class iceb_tu_str prizv("");
class iceb_tu_str ima("");
class iceb_tu_str othestvo("");

if(polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
 
polen(fio,&ima,2,' ');
polen(fio,&othestvo,3,' ');

rsdat1(&mv,&gv,mv_gv);

fprintf(ff_tab6,"%5d|%d|%s|%-*s|%-*.*s|%4d|%4d|%02d.%04d|%2d|%2d|%2d|%2d|%10.2f|%10.2f|%8.2f|%8.2f|%10.2f|%4d|%4d|%s\n",
nomer_str_tab6,
metka_ukr_grom,
metka_pola,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(30,fio), 
iceb_tu_kolbait(30,fio), 
fio,
zo,
kodtipnah,
mv,gv,
kd_np,
kd_nzp,
kd_ptv,
kd_berem,
suma_obh,
suma,
sum_diff,
suma_esv_v,
sum_narah,
metka_otk,metka_exp,
tabnom);

int metka_pola_int=0;

if(SRAV(metka_pola,"Ч",0) == 0)
 metka_pola_int=1;

int metka_nrc=0; /*метка если 0 полное рабочее время 1- не полное рабочее время*/


/*формат налоговой*/
fprintf(ff_xml_tab6,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab6,metka_ukr_grom); //6
fprintf(ff_xml_tab6,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab6,metka_pola_int); //7

if(inn[0] != '\0')
 fprintf(ff_xml_tab6,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",nomer_str_tab6,inn);          //8

fprintf(ff_xml_tab6,"   <T1RXXXXG9 ROWNUM=\"%d\">%d</T1RXXXXG9>\n",nomer_str_tab6,zo);             //9
fprintf(ff_xml_tab6,"   <T1RXXXXG10 ROWNUM=\"%d\">%d</T1RXXXXG10>\n",nomer_str_tab6,kodtipnah);    //10
fprintf(ff_xml_tab6,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab6,mv);         //11
fprintf(ff_xml_tab6,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab6,gv);         //11

fprintf(ff_xml_tab6,"   <T1RXXXXG121S ROWNUM=\"%d\">%s</T1RXXXXG121S>\n",nomer_str_tab6,iceb_tu_filtr_xml(prizv.ravno()));   //12
fprintf(ff_xml_tab6,"   <T1RXXXXG122S ROWNUM=\"%d\">%s</T1RXXXXG122S>\n",nomer_str_tab6,iceb_tu_filtr_xml(ima.ravno()));     //12
fprintf(ff_xml_tab6,"   <T1RXXXXG123S ROWNUM=\"%d\">%s</T1RXXXXG123S>\n",nomer_str_tab6,iceb_tu_filtr_xml(othestvo.ravno()));//12

fprintf(ff_xml_tab6,"   <T1RXXXXG13 ROWNUM=\"%d\">%d</T1RXXXXG13>\n",nomer_str_tab6,kd_np); //13 кількість календарних днів тимчасової непрацездатності
fprintf(ff_xml_tab6,"   <T1RXXXXG14 ROWNUM=\"%d\">%d</T1RXXXXG14>\n",nomer_str_tab6,kd_nzp); //14 Кількість календарних днів без збереження заробітної плати
fprintf(ff_xml_tab6,"   <T1RXXXXG15 ROWNUM=\"%d\">%d</T1RXXXXG15>\n",nomer_str_tab6,kd_ptv); //15 кількість календарних днів перебування у трудових/цівільно-правових  відносинах протягом звітного місяця
fprintf(ff_xml_tab6,"   <T1RXXXXG16 ROWNUM=\"%d\">%d</T1RXXXXG16>\n",nomer_str_tab6,kd_berem);  /*16 кількість календарних днів відпустки в зв’язку з вагітністю та пологами*/

fprintf(ff_xml_tab6,"   <T1RXXXXG17 ROWNUM=\"%d\">%.2f</T1RXXXXG17>\n",nomer_str_tab6,suma_obh);  //17
fprintf(ff_xml_tab6,"   <T1RXXXXG18 ROWNUM=\"%d\">%.2f</T1RXXXXG18>\n",nomer_str_tab6,suma);      //18
fprintf(ff_xml_tab6,"   <T1RXXXXG19 ROWNUM=\"%d\">%.2f</T1RXXXXG19>\n",nomer_str_tab6,sum_diff);  //19
fprintf(ff_xml_tab6,"   <T1RXXXXG20 ROWNUM=\"%d\">%.2f</T1RXXXXG20>\n",nomer_str_tab6,suma_esv_v);//20
fprintf(ff_xml_tab6,"   <T1RXXXXG21 ROWNUM=\"%d\">%.2f</T1RXXXXG21>\n",nomer_str_tab6,sum_narah); //21

fprintf(ff_xml_tab6,"   <T1RXXXXG22 ROWNUM=\"%d\">%d</T1RXXXXG22>\n",nomer_str_tab6,metka_otk); //22
fprintf(ff_xml_tab6,"   <T1RXXXXG23 ROWNUM=\"%d\">%d</T1RXXXXG23>\n",nomer_str_tab6,metka_exp); //23
fprintf(ff_xml_tab6,"   <T1RXXXXG24 ROWNUM=\"%d\">%d</T1RXXXXG24>\n",nomer_str_tab6,metka_nrc); //24
fprintf(ff_xml_tab6,"   <T1RXXXXG25 ROWNUM=\"%d\">%d</T1RXXXXG25>\n",nomer_str_tab6,nrm);       //25

}

/**********************************************/
/*Вывод строки в таблицу 5*/
/********************************/
void zarpensm1_str_tab5(int tabnom,int nomer_str_tab5,
const char *inn,
const char *fio,
short dpnr,
short dusr,
short mr,short gr,
int metka_ukr_grom,
int zo, /*категория работника*/
FILE *ff_tab5,
FILE *ff_xml_tab5)
{
class iceb_tu_str prizv("");
class iceb_tu_str ima("");
class iceb_tu_str othestvo("");
if(polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
polen(fio,&ima,2,' ');
polen(fio,&othestvo,3,' ');

fprintf(ff_tab5,"%5d|%-*s|%-*.*s|%3d|%7d|%7d|\n",
nomer_str_tab5,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(40,fio), 
iceb_tu_kolbait(40,fio), 
fio,
zo,
dpnr,
dusr);

/*формат налоговой*/
#if 0
############################################################
предыдушая форма

fprintf(ff_xml_tab5,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab5,metka_ukr_grom); //6
fprintf(ff_xml_tab5,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab5,zo); //7
fprintf(ff_xml_tab5,"   <T1RXXXXG8 ROWNUM=\"%d\">%d</T1RXXXXG8>\n",nomer_str_tab5,0); //8 договір ЦПХ 1-за основним місцем роботи 0-за сумісництвом
if(inn[0] != '\0')
   fprintf(ff_xml_tab5,"   <T1RXXXXG9S ROWNUM=\"%d\">%s</T1RXXXXG9S>\n",nomer_str_tab5,inn); //9

fprintf(ff_xml_tab5,"   <T1RXXXXG101S ROWNUM=\"%d\">%s</T1RXXXXG101S>\n",nomer_str_tab5,prizv.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG102S ROWNUM=\"%d\">%s</T1RXXXXG102S>\n",nomer_str_tab5,ima.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG103S ROWNUM=\"%d\">%s</T1RXXXXG103S>\n",nomer_str_tab5,othestvo.ravno_filtr_xml()); //10

if(dpnr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab5,dpnr); //11
if(dusr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab5,dusr); //11

#########################################################
#endif
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];

class iceb_tu_str voen_zvanie("");
class iceb_tu_str dolgn("");

sprintf(strsql,"select dolg,zvan from Kartb where tabn=%d",tabnom);
if(readkey(strsql,&row,&cur) == 1)
 {
  voen_zvanie.new_plus(row[1]);
  dolgn.new_plus(row[0]);
 }
else
 {
  sprintf(strsql,"%s-%s %d!",__FUNCTION__,gettext("Не найдена карточка для табельного номера"),tabnom);
  iceb_t_soob(strsql);
 }



fprintf(ff_xml_tab5,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab5,metka_ukr_grom); //6
fprintf(ff_xml_tab5,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab5,0); //7 договір ЦПХ 1-за основним місцем роботи 0-за сумісництвом
fprintf(ff_xml_tab5,"   <T1RXXXXG8 ROWNUM=\"%d\">%d</T1RXXXXG8>\n",nomer_str_tab5,zo); //8

if(inn[0] != '\0')
   fprintf(ff_xml_tab5,"   <T1RXXXXG9S ROWNUM=\"%d\">%s</T1RXXXXG9S>\n",nomer_str_tab5,inn); //9

fprintf(ff_xml_tab5,"   <T1RXXXXG101S ROWNUM=\"%d\">%s</T1RXXXXG101S>\n",nomer_str_tab5,prizv.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG102S ROWNUM=\"%d\">%s</T1RXXXXG102S>\n",nomer_str_tab5,ima.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG103S ROWNUM=\"%d\">%s</T1RXXXXG103S>\n",nomer_str_tab5,othestvo.ravno_filtr_xml()); //10

if(dpnr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab5,dpnr); //11
if(dusr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab5,dusr); //11

if(dolgn.getdlinna() > 1)
 fprintf(ff_xml_tab5,"   <T1RXXXXG15S ROWNUM=\"%d\">%s</T1RXXXXG15S>\n",nomer_str_tab5,dolgn.ravno_filtr_xml()); //15

sprintf(strsql,"select naik from Zvan where kod=%d",voen_zvanie.ravno_atoi());
if(readkey(strsql,&row,&cur) == 1)
 voen_zvanie.new_plus(row[0]);
else
 voen_zvanie.new_plus("");
 
if(voen_zvanie.getdlinna() > 1)
  fprintf(ff_xml_tab5,"   <T1RXXXXG19S ROWNUM=\"%d\">%s</T1RXXXXG19S>\n",nomer_str_tab5,voen_zvanie.ravno_filtr_xml()); //15
}



/******************************************/
/*Таблица 5 принятые на работу и уволенные*/
/*Возвращает количество человек в таблице*/
/******************************************/
int zarpensm1_t5(short mr,short gr,
const char *inn_or,
const char *naim_kontr00,
const char *kodpfu,
const char *imaf_tab5,
const char *kodzvna,
char *imaf_xml_tab5,
FILE *ff_prot)
{
int nomer_str_tab5=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int metka_ukr_grom=1;

char strsql[1024];
short dpnr=0;
short dusr=0;
short d,m,g;
class iceb_tu_str inn("");
class iceb_tu_str fio("");
class masiv_din_int sp_tabn;

class iceb_t_fioruk_rk kerivnik;
class iceb_t_fioruk_rk glavbuh;
iceb_t_fioruk(1,&kerivnik);
iceb_t_fioruk(2,&glavbuh);


sprintf(strsql,"select tabn,datn,datk from Zarn where god=%d and mes=%d and ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr,gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в списке месячных настроек\n");
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"\n\nФормирование таблицы 5\n");

class iceb_fopen ff_tab5;
if(ff_tab5.start(imaf_tab5,"w") != 0)
 return(0);


class iceb_rnfxml_data rek_zag_nn;

iceb_t_rnfxml(&rek_zag_nn); /*чтение данных для xml файлов*/

FILE *ff_xml_tab5;
int period_type=0;

if(iceb_t_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab5,iceb_t_getkoddok(1,mr,gr,7),1,&period_type,&rek_zag_nn,&ff_xml_tab5) != 0) /*открытие файла и получение имени файла*/
 return(1);

iceb_t_rnn_sap_xml(1,gr,mr,iceb_t_getkoddok(1,mr,gr,7),1,period_type,&rek_zag_nn,ff_xml_tab5); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or,naim_kontr00,mr,gr,ff_xml_tab5); /*теги с шапкой таблицы*/

iceb_t_zagolov("Таблиця 5. Відомості про трудові відносини осіб та період проходження військової служби",ff_tab5.ff);
fprintf(ff_tab5.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff_tab5.ff,"\
  N  |Ном.обл.к.|       Прізвище, ім'я, по батькові      |Кат|Трудові віднос.|\n\
     |          |                                        |ег.|Дата п.|Дата к.|\n");

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
int zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s\n",row[0],row[1],row[2]);
   
  dpnr=dusr=0;
  fio.new_plus("");
  inn.new_plus("");
  sprintf(strsql,"select fio,inn,zvan,sovm from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден табельный номер"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }

  if(proverka(kodzvna,row1[2],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row1[0]);
    continue;
   }

  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row1[3]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtn(mr, gr,atoi(row[0]),3) == 1)
    zo=3;

  fio.new_plus(row1[0]);
  inn.new_plus(row1[1]);
     
  if(row[1][0] != '0')
   {
    rsdat(&d,&m,&g,row[1],2);  

    if(sravmydat(1,m,g,1,mr,gr) == 0)
     dpnr=d;
   }
  if(row[2][0] != '0')
   {
    rsdat(&d,&m,&g,row[2],2);  

    if(sravmydat(1,m,g,1,mr,gr) == 0)
     dusr=d;
   }
  sp_tabn.plus(row[0],-1);

 
  nomer_str_tab5++;
  

  zarpensm1_str_tab5(atoi(row[0]),nomer_str_tab5,inn.ravno(),fio.ravno(),dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5.ff,ff_xml_tab5);


 }

/*Если в карточке небыло начислений то промежуточной записи по этому месяцу небудет. Смотрим в карточке даты приёма увольнения*/

sprintf(strsql,"select tabn,fio,datn,datk,inn,zvan,sovm from Kartb where ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в списке карточек\n");
 }

while(cur.read_cursor(&row) != 0)
 {
  if(sp_tabn.find(row[0]) >= 0)
    continue;
  dpnr=dusr=0;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(proverka(kodzvna,row[5],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row[1]);
    continue;
   }
  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row[6]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtn(mr, gr,atoi(row[0]),3) == 1)
    zo=3;

  if(row[2][0] != '0')
   rsdat(&dpnr,&m,&g,row[2],2);  
  if(row[3][0] != '0')
   rsdat(&dusr,&m,&g,row[3],2);  
  
  nomer_str_tab5++;
  zarpensm1_str_tab5(atoi(row[0]),nomer_str_tab5,row[4],row[1],dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5.ff,ff_xml_tab5);

 }

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
zarpensm1_kon_lst(nomer_str_tab5,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab5.ff);

podpis(ff_tab5.ff);

ff_tab5.end();

short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);
zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab5);/*концоквка таблицы*/
fclose(ff_xml_tab5);

if(nomer_str_tab5 == 0)
 {
//  unlink(imaf_tab5);
  unlink(imaf_xml_tab5);
 }

return(nomer_str_tab5);

}
/***************************/
/*получить количество дней*/
/***************************/
int zarpensm1_kdnp(int tabn,short m,short g,const char *kodtab,FILE *ff_prot)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int kolstr=0;

if(kodtab[0] == '\0')
 return(0);

sprintf(strsql,"select kodt,kdnei from Ztab where tabn=%d and god=%d and mes=%d",tabn,g,m);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Ищем табеля для %d из списка-%s\n",__FUNCTION__,tabn,kodtab);
int voz=0;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kodtab,row[0],0,1) == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s %s\n",__FUNCTION__,row[0],row[1]);
    voz+=atoi(row[1]);
   }
 }
return(voz);
}
/****************************************/
/*чтение данных на работника*/
/***********************************/
int zarpensm1_readtn(short mr,short gr,
const char *tabnom,
const char *nastr_kodzvna,
class iceb_tu_str *fio,class iceb_tu_str *ima,class iceb_tu_str *othestvo,
class iceb_tu_str *inn,
class iceb_tu_str *prizv,
class iceb_tu_str *data_prik,
class iceb_tu_str *nomer_prik,
class iceb_tu_str *kodss,
class iceb_tu_str *metka_pola,
int *metka_otk,
int *metka_exp,
int *zo,
int *metka_inv,
FILE *ff_prot)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
fio->new_plus("");
inn->new_plus("");
prizv->new_plus("");
ima->new_plus("");
othestvo->new_plus("");
  
*metka_otk=1;  /*1-трудовая книжка на предпиятии 0- нет*/

int kodzvna=0;  
kodss->new_plus("");
metka_pola->new_plus("");
sprintf(strsql,"select fio,datn,datk,sovm,inn,zvan,kss,pl from Kartb where tabn=%s",tabnom);
if(readkey(strsql,&row,&cur) == 1)
 {
  fio->new_plus(row[0]);
  inn->new_plus(row[4]);
  if(atoi(row[3]) == 1)
   *metka_otk=0;
  kodzvna=atoi(row[5]);

  if(pole3(row[0],prizv,1,' ') != 0)
   prizv->new_plus(row[0]);

  pole3(row[0],ima,2,' ');
  pole3(row[0],othestvo,3,' ');

  if(polen(row[6],kodss,1,' ') != 0)
    kodss->new_plus(row[6]);
  polen(row[6],data_prik,2,' ');
  polen(row[6],nomer_prik,3,' ');
  
  
  if(atoi(row[7]) == 0)
   metka_pola->new_plus("Ч");
  else
   metka_pola->new_plus("Ж");     
 }
else
 {
   if(ff_prot != NULL)
    fprintf(ff_prot,"Не найдена карточка работника %d!\n",atoi(row[0]));
  return(1);
 }
 
if(proverka(nastr_kodzvna,kodzvna,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row[0]);
  return(1);
 }


if(inn->getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s\n%s",row[0],fio->ravno(),gettext("Не введён идентификационный номер"));
  iceb_t_soob(strsql);
 }

*metka_exp=0;
if(kodss->ravno()[0] != '\0')
 *metka_exp=1;   
*zo=1;
*metka_inv=0;
if(zarprtn(mr,gr,atoi(tabnom),2) == 1)
 {
  *metka_inv=1;
  *zo=2;
 }
if(zarprtn(mr,gr,atoi(tabnom),3) == 1)
 {
  *zo=26;
  *metka_otk=0;
 }
return(0);
}

/****************************************************/
void zarpensm1_str_tab7(int nomer_str_tab7,const char *data_prik,int metka_ukr_grom,const char *inn,const char *fio,
const char *nomer_prik,
const char *kodss,
short dn,short dk,
short mr,short gr,
const char *tabnom,
const char *prizv,
const char *ima,
const char *othestvo,
int koldn,
FILE *ff_tab7,
FILE *ff_xml_tab7)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];

/*Если уволили то день конца периода это день увольнения*/
sprintf(strsql,"select datk from Kartb where tabn=%d",atoi(tabnom));

if(readkey(strsql,&row,&cur) == 1)
 {
  if(row[0][0] != '0')
   {
    short d,m,g;
    rsdat(&d,&m,&g,row[0],2);
    if(d != 0)
      dk=d;
   }
 }


short dprik=0,mprik=0,gprik=0;

rsdat(&dprik,&mprik,&gprik,data_prik,1);


fprintf(ff_tab7,"\
%5d|%d|%-*s|%-*s|%-*.*s|%3d|%3d|     |      |     |%-*s|%02d.%02d.%04d|    |%s\n",
nomer_str_tab7,
metka_ukr_grom,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(10,kodss),
kodss,
iceb_tu_kolbait(30,fio),
iceb_tu_kolbait(30,fio),
fio,
dn,
dk,
iceb_tu_kolbait(5,nomer_prik),
nomer_prik,
dprik,mprik,gprik,    
tabnom);

/*формат налоговой*/

fprintf(ff_xml_tab7,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab7,metka_ukr_grom); //6
if(inn[0] != '\0')
   fprintf(ff_xml_tab7,"   <T1RXXXXG7S ROWNUM=\"%d\">%s</T1RXXXXG7S>\n",nomer_str_tab7,inn); //7
fprintf(ff_xml_tab7,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",nomer_str_tab7,kodss); //8

fprintf(ff_xml_tab7,"   <T1RXXXXG91S ROWNUM=\"%d\">%s</T1RXXXXG91S>\n",nomer_str_tab7,iceb_tu_filtr_xml(prizv)); //9
fprintf(ff_xml_tab7,"   <T1RXXXXG92S ROWNUM=\"%d\">%s</T1RXXXXG92S>\n",nomer_str_tab7,iceb_tu_filtr_xml(ima)); //9
fprintf(ff_xml_tab7,"   <T1RXXXXG93S ROWNUM=\"%d\">%s</T1RXXXXG93S>\n",nomer_str_tab7,iceb_tu_filtr_xml(othestvo)); //9

fprintf(ff_xml_tab7,"   <T1RXXXXG10 ROWNUM=\"%d\">%d</T1RXXXXG10>\n",nomer_str_tab7,dn); //10
fprintf(ff_xml_tab7,"   <T1RXXXXG11 ROWNUM=\"%d\">%d</T1RXXXXG11>\n",nomer_str_tab7,dk); //11
fprintf(ff_xml_tab7,"   <T1RXXXXG12 ROWNUM=\"%d\">%d</T1RXXXXG12>\n",nomer_str_tab7,koldn); //12

fprintf(ff_xml_tab7,"   <T1RXXXXG141 ROWNUM=\"%d\">%d</T1RXXXXG141>\n",nomer_str_tab7,koldn); //14

if(nomer_prik[0] != '\0')
 fprintf(ff_xml_tab7,"   <T1RXXXXG15 ROWNUM=\"%d\">%s</T1RXXXXG15S>\n",nomer_str_tab7,nomer_prik); //15
if(dprik != 0)
 fprintf(ff_xml_tab7,"   <T1RXXXXG16D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG16D>\n",nomer_str_tab7,dprik,mprik,gprik);//16

}
/*******************************/
/*Узнаём сумму начислений на зарплату*/
/*************************************/
double zarpensm1_snnz(int tabn,short mr,short gr,double *suma_esv_bol)
{
double suma=0.;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
*suma_esv_bol=0;
sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%d",gr,mr,tabn);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
int kodz=0;
while(cur.read_cursor(&row) != 0)
 {
  kodz=atoi(row[0]);
  if(kodf_esv_bol == kodz || kodf_esv_bol_inv == kodz)
   *suma_esv_bol+=atof(row[1]);
  else
   suma+=atof(row[1]);
 }
return(suma);

}
/***************************************/
/*шапка всех додатка 4 для таблиц 5,6,7*/
/****************************************/
void zarpensm1_hd4(const char *inn_or,const char *naim_kontr00,short mr,short gr,FILE *ff_xml)
{
class iceb_tu_str naim(naim_kontr00);

fprintf(ff_xml," <DECLARBODY>\n");
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",inn_or);
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",naim.ravno_filtr_xml());
fprintf(ff_xml,"   <HZM>%d</HZM>\n",mr);
fprintf(ff_xml,"   <HZY>%d</HZY>\n",gr);
fprintf(ff_xml,"   <HZB>1</HZB>\n"); /*тип початкова*/
}
/**************************************/
/*концовка таблиц*/
/*****************************************/
void zarpensm1_end_tab_xml(short dt,short mt,short gt,
class iceb_t_fioruk_rk *kerivnik,
class iceb_t_fioruk_rk *glavbuh,
FILE *ff_xml)
{
fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",kerivnik->fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",kerivnik->inn.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",glavbuh->fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",glavbuh->inn.ravno());
fprintf(ff_xml,"  </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
}
