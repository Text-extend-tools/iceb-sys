/*$Id: rasalk.c,v 5.16 2013/08/13 05:49:52 sasa Exp $*/
/*16.04.2017	07.06.2007	Белых А.И.	rasalk.c
Распечатка товарно-транспортной накладной на перемещение алкогольных напитков
*/
#include        <errno.h>
#include	"buhg.h"

class tov_tr_nak
 {
  public:
  class iceb_tu_str p_list;
  class iceb_tu_str marka_avt;
  class iceb_tu_str gos_nomer;
  class iceb_tu_str pricep;
  class iceb_tu_str perevoz;
  class iceb_tu_str zakaz;
  class iceb_tu_str voditel;
  class iceb_tu_str punkt_zag;
  class iceb_tu_str punkt_roz;
  class iceb_tu_str dover;
  class iceb_tu_str data_dov;
  class iceb_tu_str akciz_sbor;
  
  class iceb_tu_str naim00;
  class iceb_tu_str adres00;
  class iceb_tu_str mfo00;
  class iceb_tu_str r_shet00;
  class iceb_tu_str naimb00;
  class iceb_tu_str edrpou00;
  class iceb_tu_str nomer_lic00;
  class iceb_tu_str datan_lic00;
  class iceb_tu_str datak_lic00;

  class iceb_tu_str naim;
  class iceb_tu_str adres;
  class iceb_tu_str mfo;
  class iceb_tu_str r_shet;
  class iceb_tu_str naimb;
  class iceb_tu_str edrpou;
  class iceb_tu_str nomer_lic;
  class iceb_tu_str datan_lic;
  class iceb_tu_str datak_lic;

  float pnds;
  short lnds;  //Льготы по НДС
  char imaf[64];
  short tipz;  
  tov_tr_nak()
   {
    clear_data();
    pnds=0.;
    lnds=0;
    naim00.plus("");
    adres00.plus("");
    mfo00.plus("");
    r_shet00.plus("");
    naimb00.plus("");
    edrpou00.plus("");
    nomer_lic00.plus("");
    datan_lic00.plus("");
    datak_lic00.plus("");
    
    naim.plus("");
    adres.plus("");
    mfo.plus("");
    r_shet.plus("");
    naimb.plus("");
    edrpou.plus("");
    nomer_lic.plus("");
    datan_lic.plus("");
    datak_lic.plus("");
    memset(imaf,'\0',sizeof(imaf));  
   }

  void clear_data()
   {
    p_list.new_plus("");
    marka_avt.new_plus("");
    gos_nomer.new_plus("");
    pricep.new_plus("");
    perevoz.new_plus("");
    zakaz.new_plus("");
    voditel.new_plus("");
    punkt_zag.new_plus("");
    punkt_roz.new_plus("");
    dover.new_plus("");
    data_dov.new_plus("");
    akciz_sbor.new_plus("");
   }
 };

int rasalk_zap(short gg,int skl,const char *nomdok,class tov_tr_nak *rek_nak);
int rasalk_read_rk(short,short,short,const char *nomdok,int skl,const char *kontr,class iceb_tu_str *naim,class iceb_tu_str *adres,class iceb_tu_str *mfo,class iceb_tu_str *r_shet,class iceb_tu_str *naimb,class iceb_tu_str *edrpou,class iceb_tu_str *nomer_lic,class iceb_tu_str *datan_lic,class iceb_tu_str *datak_lic);
void rasalk_rs(short dg,short mg,short gg,int skl,const char *nomdok,class tov_tr_nak *rek_nak);
 
extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/

void rasalk(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok) //Номер документа
{
char strsql[512];
class spis_oth oth;
class tov_tr_nak rek_nak;
class SQLCURSOR cur;
SQL_str row;
class iceb_tu_str kontr_pol("");

class iceb_tu_str printer("lpr -r");

iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");



if(rasalk_read_rk(dg,mg,gg,nomdok,skl,"00",&rek_nak.naim00,&rek_nak.adres00,&rek_nak.mfo00,&rek_nak.r_shet00,&rek_nak.naimb00,\
&rek_nak.edrpou00,&rek_nak.nomer_lic00,&rek_nak.datan_lic00,&rek_nak.datak_lic00) != 0)
 return;


//читаем шапку документа

sprintf(strsql,"select tip,kontr,pn from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gg,mg,dg,skl,nomdok);
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob("Не найдена шапка документа !");
  return;    
 } 
rek_nak.tipz=atoi(row[0]);
kontr_pol.new_plus(row[1]);
rek_nak.pnds=atof(row[2]);

if(rasalk_read_rk(dg,mg,gg,nomdok,skl,kontr_pol.ravno(),&rek_nak.naim,&rek_nak.adres,&rek_nak.mfo,&rek_nak.r_shet,&rek_nak.naimb,\
&rek_nak.edrpou,&rek_nak.nomer_lic,&rek_nak.datan_lic,&rek_nak.datak_lic) != 0)
 return;

//читаем дополнительные реквизиты шапки документа
sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s'",gg,skl,nomdok);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int nomerz=0;
while(cur.read_cursor(&row) != 0)
 {
  nomerz=atoi(row[0]);

  if(nomerz == 1) //Доверенность#дата выдачи доверенности
   {
    polen(row[1],&rek_nak.dover,1,'#');
    polen(row[1],&rek_nak.data_dov,2,'#');
    continue;
   }  
  if(nomerz == 10) //Гос-номер машины
   {
    rek_nak.gos_nomer.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 11) //льготы по НДС
   {
    rek_nak.lnds=atoi(row[1]);
    continue;
   }  
  if(nomerz == 15) //Номер путевого листа
   {
    rek_nak.p_list.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 16) //марка автомобиля
   {
    rek_nak.marka_avt.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 17) //марка и номер прицепа
   {
    rek_nak.pricep.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 18) //Перевозчик груза
   {
    rek_nak.perevoz.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 19) //Заказчик плательщик
   {
    rek_nak.zakaz.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 20) //Фамилия водителя
   {
    rek_nak.voditel.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 21) //Пункт загрузки
   {
    rek_nak.punkt_zag.new_plus(row[1]);
    continue;
   }  
  if(nomerz == 22) //Пункт разгрузки
   {
    rek_nak.punkt_roz.new_plus(row[1]);
    continue;
   }  

  if(nomerz == 23) //Акцизный сбор
   {
    rek_nak.akciz_sbor.new_plus(row[1]);
    continue;
   }  

 }
 
int metka_blok=0;
if(iceb_t_pblok(mg,gg,ICEB_PS_MU) != 0)
  metka_blok=1;


VVOD VV(0);
sprintf(strsql,"Накладная N%s от %d.%d.%dг.",nomdok,dg,mg,gg);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_ZAG(rek_nak.naim00.ravno());
sprintf(strsql,"%s %s-%s",rek_nak.nomer_lic00.ravno(),rek_nak.datan_lic00.ravno(),rek_nak.datak_lic00.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s %s",kontr_pol.ravno(),rek_nak.naim.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s %s-%s",rek_nak.nomer_lic.ravno(),rek_nak.datan_lic.ravno(),rek_nak.datak_lic.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD("Путевой лист...........");//0
VV.VVOD_SPISOK_add_MD("Марка автомобиля.......");//1
VV.VVOD_SPISOK_add_MD("Гос-номер автомобиля...");//2
VV.VVOD_SPISOK_add_MD("Марка и номер прицепа..");//3
VV.VVOD_SPISOK_add_MD("Перевозчик.............");//4
VV.VVOD_SPISOK_add_MD("Заказчик (плательщик)..");//5
VV.VVOD_SPISOK_add_MD("Водитель...............");//6
VV.VVOD_SPISOK_add_MD("Пункт загрузки.........");//7
VV.VVOD_SPISOK_add_MD("Пункт разгрузки........");//8
VV.VVOD_SPISOK_add_MD("Доверенность...........");//9
VV.VVOD_SPISOK_add_MD("Дата доверенности......");//10
VV.VVOD_SPISOK_add_MD("Акцизный сбор..........");//11


VV.VVOD_SPISOK_add_data(rek_nak.p_list.ravno(),40);
VV.VVOD_SPISOK_add_data(rek_nak.marka_avt.ravno(),40);
VV.VVOD_SPISOK_add_data(rek_nak.gos_nomer.ravno(),40);
VV.VVOD_SPISOK_add_data(rek_nak.pricep.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.perevoz.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.zakaz.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.voditel.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.punkt_zag.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.punkt_roz.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.dover.ravno(),64);
VV.VVOD_SPISOK_add_data(rek_nak.data_dov.ravno(),11);
VV.VVOD_SPISOK_add_data(rek_nak.akciz_sbor.ravno(),20);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F5 4",gettext("печать"),
"F6",gettext("печать"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

if(metka_blok == 0)
 {
  rek_nak.p_list.new_plus(VV.VVOD_SPISOK_return_data(0));
  rek_nak.marka_avt.new_plus(VV.VVOD_SPISOK_return_data(1));
  rek_nak.gos_nomer.new_plus(VV.VVOD_SPISOK_return_data(2));
  rek_nak.pricep.new_plus(VV.VVOD_SPISOK_return_data(3));
  rek_nak.perevoz.new_plus(VV.VVOD_SPISOK_return_data(4));
  rek_nak.zakaz.new_plus(VV.VVOD_SPISOK_return_data(5));
  rek_nak.voditel.new_plus(VV.VVOD_SPISOK_return_data(6));
  rek_nak.punkt_zag.new_plus(VV.VVOD_SPISOK_return_data(7));
  rek_nak.punkt_roz.new_plus(VV.VVOD_SPISOK_return_data(8));
  rek_nak.dover.new_plus(VV.VVOD_SPISOK_return_data(9));
  rek_nak.data_dov.new_plus(VV.VVOD_SPISOK_return_data(10));
  rek_nak.akciz_sbor.new_plus(VV.VVOD_SPISOK_return_data(11));
 }


switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu2_1_3.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2: //Запись данных
    if(metka_blok == 1)
     {
      sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),mg,gg);
      iceb_t_soob(strsql);
      goto naz;
     }
    rasalk_zap(gg,skl,nomdok,&rek_nak);
    goto naz;

  case FK5: //Печать 
    if(metka_blok == 0)
     rasalk_zap(gg,skl,nomdok,&rek_nak);
    rasalk_rs(dg,mg,gg,skl,nomdok,&rek_nak);
    pehf(rek_nak.imaf,4,printer.ravno());
    unlink(rek_nak.imaf);
    return;

  case FK6: //Печать 
    if(metka_blok == 0)
      rasalk_zap(gg,skl,nomdok,&rek_nak);
    rasalk_rs(dg,mg,gg,skl,nomdok,&rek_nak);
    oth.clear();
    oth.spis_imaf.plus(rek_nak.imaf);
    oth.spis_naim.plus("Товарно-транспортная накладная");
    iceb_t_rabfil(&oth,"");
    return;
    
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


}
/************************/
/*Распечатка*/
/*************************/
void rasalk_rs(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok,
class tov_tr_nak *rek_nak)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mg,dg,skl,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;
double cena;
double kolih=0.;
double suma=0;
double itogo_bnds=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  itogo_bnds+=suma;
 }

SQL_str row1;
class SQLCURSOR cur1;

  /*Читаем сумму корректировки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,13);

double sumkor=0.;

if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
 {
  sumkor=atof(row1[0]);
  sumkor=okrug(sumkor,0.01);
 }
 

double itogo_k_oplate=0.;
double suma_nds=0.;
if(rek_nak->lnds == 0 )
 {
  suma_nds=(itogo_bnds+sumkor)*rek_nak->pnds/100.;

  suma_nds=okrug(suma_nds,0.01);

 }

double sumt=0.;
      
itogo_k_oplate=(itogo_bnds+sumkor)+suma_nds+sumt;
class iceb_tu_str suma_prop("");
preobr(itogo_k_oplate,&suma_prop,0);


FILE *ff;

sprintf(rek_nak->imaf,"alkog%d.lst",getpid());
if((ff = fopen(rek_nak->imaf,"w")) == NULL)
 {
  error_op_nfil(rek_nak->imaf,errno,"");
  return;
 }
startfil(ff);
startfil(ff);

class iceb_tu_str naim_mes("");
mesc(mg,1,&naim_mes);

fprintf(ff,"\n\n\n\n\n\n\n\n%29s %02d %-13s%02d %20s%s\n","",dg,naim_mes.ravno(),gg-2000,"",rek_nak->p_list.ravno());

sprintf(strsql,"%s:%s",rek_nak->marka_avt.ravno(),rek_nak->gos_nomer.ravno());
fprintf(ff,"\n%12s %-*s %s\n","",iceb_tu_kolbait(46,strsql),strsql,rek_nak->pricep.ravno());

fprintf(ff,"\n%12s %-*s %-*s %s\n","",
iceb_tu_kolbait(36,rek_nak->perevoz.ravno()),rek_nak->perevoz.ravno(),
iceb_tu_kolbait(30,rek_nak->zakaz.ravno()),rek_nak->zakaz.ravno(),
rek_nak->voditel.ravno());

fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/
fprintf(ff,"\x1B\x33%c\n",14); /*Уменьшаем межстрочный интервал*/

if(rek_nak->tipz == 1)
 {

  fprintf(ff,"\n%24s %s %s МФО %s Р/р %s в %s\n",
  "",rek_nak->naim.ravno(),rek_nak->adres.ravno(),rek_nak->mfo.ravno(),rek_nak->r_shet.ravno(),rek_nak->naimb.ravno());

  fprintf(ff,"\n%24s Код %s Ліцензія %s дійсна з %s по %s\n",
  " ",rek_nak->edrpou.ravno(),rek_nak->nomer_lic.ravno(),rek_nak->datan_lic.ravno(),rek_nak->datak_lic.ravno());

  fprintf(ff,"\n%24s %s %s МФО %s Р/р %s в %s\n",
  "",rek_nak->naim00.ravno(),rek_nak->adres00.ravno(),rek_nak->mfo00.ravno(),rek_nak->r_shet00.ravno(),rek_nak->naimb00.ravno());
  fprintf(ff,"\x1B\x33%c\n",28); /*Уменьшаем межстрочный интервал*/

  fprintf(ff,"\n%24s Код %s Ліцензія %s дійсна з %s по %s\n",
  " ",rek_nak->edrpou00.ravno(),rek_nak->nomer_lic00.ravno(),rek_nak->datan_lic00.ravno(),rek_nak->datak_lic00.ravno());

 }

if(rek_nak->tipz == 2)
 {
  fprintf(ff,"\n%24s %s %s МФО %s Р/р %s в %s\n",
  "",rek_nak->naim00.ravno(),rek_nak->adres00.ravno(),rek_nak->mfo00.ravno(),rek_nak->r_shet00.ravno(),rek_nak->naimb00.ravno());
  fprintf(ff,"\x1B\x33%c\n",28); /*Уменьшаем межстрочный интервал*/

  fprintf(ff,"\n%24s Код %s Ліцензія %s дійсна з %s по %s\n",
  " ",rek_nak->edrpou00.ravno(),rek_nak->nomer_lic00.ravno(),rek_nak->datan_lic00.ravno(),rek_nak->datak_lic00.ravno());

  fprintf(ff,"\n%24s %s %s МФО %s Р/р %s в %s\n",
  "",rek_nak->naim.ravno(),rek_nak->adres.ravno(),rek_nak->mfo.ravno(),rek_nak->r_shet.ravno(),rek_nak->naimb.ravno());

  fprintf(ff,"\n%24s Код %s Ліцензія %s дійсна з %s по %s\n",
  " ",rek_nak->edrpou.ravno(),rek_nak->nomer_lic.ravno(),rek_nak->datan_lic.ravno(),rek_nak->datak_lic.ravno());
 }
 
fprintf(ff,"\n%40s %-*s %s\n",
"",
iceb_tu_kolbait(100,rek_nak->punkt_zag.ravno()),rek_nak->punkt_zag.ravno(),
rek_nak->punkt_roz.ravno());

short ddov,mdov,gdov;
rsdat(&ddov,&mdov,&gdov,rek_nak->data_dov.ravno(),1);

mesc(mdov,1,&naim_mes);
class iceb_tu_str seriq("");
class iceb_tu_str nomer_dov("");

polen(rek_nak->dover.ravno(),&seriq,1,' ');

polen(rek_nak->dover.ravno(),&nomer_dov,2,' ');

fprintf(ff,"\n%64s %-*s %-*s %02d    %-*s %02d\n",
"",
iceb_tu_kolbait(16,seriq.ravno()),seriq.ravno(),
iceb_tu_kolbait(23,nomer_dov.ravno()),nomer_dov.ravno(),
ddov,
iceb_tu_kolbait(25,naim_mes.ravno()),naim_mes.ravno(),
gdov-2000);

for(int ii=0; ii < 15; ii++)
 fprintf(ff,"\n");

fprintf(ff,"%-50s %10.10g %50s %s\n","",kolih,"",suma_prop.ravno());
 
fprintf(ff,"\n%20s %10.2f %110s %s\n","",suma_nds,"",rek_nak->akciz_sbor.ravno());
fprintf(ff,"\n%165s Накладна %s від %02d.%02d.%d\n","",nomdok,dg,mg,gg);

fprintf(ff,"\f"); //Прогон листа
fprintf(ff,"\x1B\x33%c\n",28); /*Уменьшаем межстрочный интервал*/

int max_kolih_str=21; //Максимальное количество строк которые можно заполнить в документе
int kolih_r=0; //количество распечатаных строк

for(int ii=0; ii < 14; ii++)
 fprintf(ff,"\n");

fprintf(ff,"\x1B\x33%c",21); /*Уменьшаем межстрочный интервал*/
 
int nom_kart=0;
class iceb_tu_str reg_nom("");
short dr,mr,gr;
char data_reg[64];
float krat=0.;
class iceb_tu_str naim_prod("");
double suma_spdv=0.;

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  nom_kart=atoi(row[3]);
  reg_nom.new_plus("");
  memset(data_reg,'\0',sizeof(data_reg));  
  krat=0.;

  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  suma_spdv=suma+suma*rek_nak->pnds/100.;
  
  suma_spdv=okrug(suma_spdv,0.01);

  if(nom_kart != 0)
   {
    sprintf(strsql,"select krat,datv,rnd from Kart where sklad=%d and nomk=%d",skl,nom_kart);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      rsdat(&dr,&mr,&gr,row1[1],2);
      sprintf(data_reg,"%02d.%02d.%d",dr,mr,gr);
      reg_nom.new_plus(row1[2]);
     }
   }
  //читаем наименование материалла
  sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_prod.new_plus(row1[0]);
  else
   naim_prod.new_plus("");  
  //читаем в карточке регистрационный номер
  fprintf(ff,"%12s %2d  %-*.*s %10.10g %6s %-25.25s %-26s %10.10g %5s %10.2f %4s %10.2f\n\n",
  "",
  ++kolih_r,
  iceb_tu_kolbait(30,naim_prod.ravno()),iceb_tu_kolbait(30,naim_prod.ravno()),naim_prod.ravno(),
  krat,"",reg_nom.ravno(),data_reg,kolih,"",suma,"",suma_spdv);
 }

for(int ii=kolih_r; ii < max_kolih_str; ii++)
 fprintf(ff,"\n\n");

fprintf(ff,"%148s %10.2f\n\n","",itogo_bnds+suma_nds);
fprintf(ff,"%148s %10.2f\n","",suma_nds);

fclose(ff);

class iceb_tu_str printer;

iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");

pehf(rek_nak->imaf,4,printer.ravno());
}



/**************************/
/*Запрись реквизитов шапки накладной*/
/**************************************/

int rasalk_zap(short gg, //дата документа
int skl,  //Склад
const char *nomdok,
class tov_tr_nak *rek_nak)
{
char strsql[1024];

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=1",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->dover.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s#%s')",gg,skl,nomdok,1,rek_nak->dover.ravno(),rek_nak->data_dov.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=10",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->gos_nomer.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,10,rek_nak->gos_nomer.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=15",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->p_list.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,15,rek_nak->p_list.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=16",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->marka_avt.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,16,rek_nak->marka_avt.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=17",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->pricep.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,17,rek_nak->pricep.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=18",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->perevoz.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,18,rek_nak->perevoz.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=19",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->zakaz.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,19,rek_nak->zakaz.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=20",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->voditel.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,20,rek_nak->voditel.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=21",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->punkt_zag.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,21,rek_nak->punkt_zag.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=22",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->punkt_roz.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,22,rek_nak->punkt_roz.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=23",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->akciz_sbor.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s')",gg,skl,nomdok,23,rek_nak->akciz_sbor.ravno());
  sql_zapis(strsql,0,0);
 }

sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=24",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->akciz_sbor.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s|%s-%s')",gg,skl,nomdok,24,
  rek_nak->nomer_lic00.ravno(),rek_nak->datan_lic00.ravno(),rek_nak->datak_lic00.ravno());
  sql_zapis(strsql,0,0);
 }
sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=25",gg,skl,nomdok);
sql_zapis(strsql,0,0);
if(rek_nak->akciz_sbor.ravno()[0] != '\0')
 {
  sprintf(strsql,"insert into Dokummat2 values(%d,%d,'%s',%d,'%s|%s-%s')",gg,skl,nomdok,25,
  rek_nak->nomer_lic.ravno(),rek_nak->datan_lic.ravno(),rek_nak->datak_lic.ravno());
  sql_zapis(strsql,0,0);
 }

return(0);
}

/*********************/
/*чтение реквизитов контрагента*/
/*******************************/
int rasalk_read_rk(short dg,short mg,short gg,
const char *nomdok,
int skl,
const char *kontr,
class iceb_tu_str *naim,
class iceb_tu_str *adres,
class iceb_tu_str *mfo,
class iceb_tu_str *r_shet,
class iceb_tu_str *naimb,
class iceb_tu_str *edrpou,
class iceb_tu_str *nomer_lic,
class iceb_tu_str *datan_lic,
class iceb_tu_str *datak_lic)
{
SQL_str row;
class SQLCURSOR cur;

if(SRAV(kontr,"00",0) != 0)
if(SRAV(kontr,"00-",1) == 0)
 {
  iceb_t_soob("Для внутреннего перемещения этот документ не печатается !");
  return(1);    
  
 }

char strsql[1024];
sprintf(strsql,"select naikon,naiban,adres,kod,mfo,nomsh,innn,pnaim from Kontragent where kodkon='%s'",kontr);
if(readkey(strsql,&row,&cur) != 1)
 return(1);
 
naim->new_plus(row[7]);
if(naim->ravno()[0] == '\0')
 naim->new_plus(row[0]);
adres->new_plus(row[2]);
mfo->new_plus(row[4]);
r_shet->new_plus(row[5]);
naimb->new_plus(row[1]);
edrpou->new_plus(row[3]);

int nomer_zap=24;
if(SRAV("00",kontr,0) != 0)
 nomer_zap=25;
 
sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,nomer_zap);
if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],nomer_lic,1,'|');

  POLE(row[0],strsql,2,'|');
  polen(strsql,datan_lic,1,'-');
  polen(strsql,datak_lic,2,'-');
 }
else  
 {
  sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like '%s%%'",
  kontr,
  "Лицензия на алкогольную продукцию");
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не найден номер лицензии для контрагента %s !",kontr);
    iceb_t_soob(strsql);
    return(1);    
   
   }

  polen(row[0],nomer_lic,2,'|');
  if(nomer_lic->ravno()[0] == '\0')
   {
    sprintf(strsql,"Не введён номер лицензии для контрагента %s !",kontr);
    iceb_t_soob(strsql);
    return(1);    
   }

  sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like '%s%%'",
  kontr,
  "Период действия лицензии на алкогольную продукцию");
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не найден период действия лицензии для контрагента %s !",kontr);
    iceb_t_soob(strsql);
    return(1);    
   
   }
  POLE(row[0],strsql,2,'|');
  polen(strsql,datan_lic,1,'-');
  polen(strsql,datak_lic,2,'-');
 }

if(datan_lic->ravno()[0] == '\0')
 {
  sprintf(strsql,"Не введена дата начала действия лицензии для контрагента %s !",kontr);
  iceb_t_soob(strsql);
  return(1);    
 }
if(datak_lic->ravno()[0] == '\0')
 {
  sprintf(strsql,"Не введена дата конца действия лицензии для контрагента %s !",kontr);
  iceb_t_soob(strsql);
  return(1);    
 }
short dn,mn,gn;
short dk,mk,gk;

if(rsdat(&dn,&mn,&gn,datan_lic->ravno(),1) != 0)
 {
  iceb_t_soob("Не правильно введена дата начала действия лицензии !");
  return(1);    

 }

if(rsdat(&dk,&mk,&gk,datak_lic->ravno(),1) != 0)
 {
  iceb_t_soob("Не правильно введена дата конца действия лицензии !");
  return(1);    

 }

if(sravmydat(dg,mg,gg,dn,mn,gn) < 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,"Не начался срок действия лиценции лицензии для контрагента %s !",kontr);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%d.%d.%d-%d.%d.%d",dn,mn,gn,dk,mk,gk);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);    
 } 

if(sravmydat(dg,mg,gg,dk,mk,gk) > 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,"Закончился срок действия лицензии для контрагента %s !",kontr);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  
  sprintf(strsql,"%d.%d.%d-%d.%d.%d",dn,mn,gn,dk,mk,gk);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);    
 } 

return(0);
}
