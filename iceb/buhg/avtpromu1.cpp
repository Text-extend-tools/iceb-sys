/*$Id: avtpromu1.c,v 5.13 2013/11/24 08:23:03 sasa Exp $*/
/*29.03.2017	14.04.2009	Белых А.И	avtpromu1.c
Автоматическое выполнение проводок 
*/
#include "buhg.h"

extern double   okrg1; /*Округление*/
extern short    vtara; /*Код группы возвратная тара*/
extern masiv_din_double snn; /*Суммы по накладных*/
extern double	*kofusl; //Коэффициенты распределения услуг
extern double	sumusl; //Сумма услуг по документа

void avtpromu1(int tipz,const char *datdok,const char *nomdok,FILE *ff_prot)
{
short dd=0,md=0,gd=0;
rsdat(&dd,&md,&gd,datdok,1);
avtpromu1(tipz,dd,md,gd,nomdok,ff_prot);
}
/*****************************************/
void avtpromu1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot)
{
class iceb_t_avp avp;

SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s\n\
%s:%d.%d.%d %s:%s\n",
__FUNCTION__,
gettext("Автоматическое выполнение проводок"),
gettext("Дата документа"),
dd,md,gd,
gettext("Номер документа"),
nomdok);

avp.metka_avk=2;
if(iceb_t_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"matnast.alx") == 0)
 {
   if(SRAV("Вкл",strsql,1) == 0)
    avp.metka_avk=2;
   else
    avp.metka_avk=0;
 }

 
/*Читаем шапку документа*/
sprintf(strsql,"select sklad,kontr,kodop,nomon,pn from Dokummat where datd='%04d-%02d-%02d' and nomd='%s' and tip=%d",
gd,md,dd,nomdok,tipz);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d.%d.%d %s",gettext("Не найдена шапка документа!"),dd,md,gd,nomdok);
  iceb_t_soob(strsql);
  return;
 }

int sklad=atoi(row[0]);

if(SRAV(row[1],"00-",1) == 0)
  avp.kontr_v_dok.new_plus("00");
else
  avp.kontr_v_dok.new_plus(row[1]);

avp.kodop.new_plus(row[2]);
class iceb_tu_str nomdok_par(row[3]);
float pnds=atof(row[4]);

/*Номер документа поставщика*/
avp.nomer_dok.new_plus(row[3]);

avp.suma_nds_vv=0.; /*Сумма НДС введённая в документ вручную*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=6",
gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  avp.suma_nds_vv=atof(row[0]); 

double suma_korrekt=0.; /*Сумма корректировки к документа*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=13",
gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  suma_korrekt=atof(row[0]); 

int metka_nds=0; /*Метка документа с НДС*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=11",
gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  metka_nds=atoi(row[0]); 

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"%s-%s:%d %s:%s %s:%s %s:%f %s:%s\n\
%s:%.2f %s:%.2f\n\
%s=%d",  
  __FUNCTION__,
  gettext("Склад"),
  sklad,
  gettext("Контрагент"),
  avp.kontr_v_dok.ravno(),
  gettext("Код операции"),
  avp.kodop.ravno(),
  gettext("Процент НДС"),
  pnds,
  gettext("Номер парного документа"),
  nomdok_par.ravno(),
  gettext("Сумма НДС введённая вручную"),
  avp.suma_nds_vv,
  gettext("Сумма корректировки к документу"),
  suma_korrekt,
  gettext("Метка НДС"),
  metka_nds);
 }
int kolstr=0;
sprintf(strsql,"select nomkar,kolih,cena,nds,kodm,nomkarp,shet from \
Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",gd,md,dd,sklad,nomdok);  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s\n",__FUNCTION__,gettext("Не найдено ни одной записи в документе!"));
  return;
 }
double prockor=0.;
if(suma_korrekt != 0.)
  prockor=suma_korrekt*100./snn.suma();


class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;
int kgrm=0;
int nomkart=0;
double kolihpod=0.;
double cena_uheta=0.;
double suma_uheta=0.;
double i_suma_uheta=0.;
double cena_v_dok=0.;
double suma_po_dok=0.;
double i_suma_po_dok=0.;
double itogo_pod=0.;
short dp1=0,mp1=0,gp1=0;
double nds=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"%s=%s %s %s %s %s %s %s\n\
-------------------------------------------------------------------------\n",
    gettext("Прочитали запись в документе"),row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
   }
  /*Читаем карточку материалла*/
  nomkart=atoi(row[0]);

  if(nomkart == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-%s\n",__FUNCTION__,gettext("Запись не привязана к карточке! Пропускаем"));
    continue;

   }
  nds=atof(row[3]);
  cena_v_dok=atof(row[2]);
  avp.shetsp.new_plus(row[6]);  


  sprintf(strsql,"select shetu,krat,cena from Kart where sklad=%d and nomk=%d",
  sklad,nomkart);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-Не нашли карточки %d склад %d\n",__FUNCTION__,nomkart,sklad);
    msql_error(&bd,strsql,strsql);
   }

  cena_uheta=atof(row1[2]);
  avp.shetuh.new_plus(row1[0]);	


  /*Определяем группу материалла*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[4]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s %s",__FUNCTION__,gettext("Не найден код материалла"),row[4]);
    msql_error(&bd,strsql,strsql);
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",row1[1]);     
  kgrm=atoi(row1[0]);


  /*Читаем количество в карточке*/
  
     
  sprintf(strsql,"select datdp,kolih from Zkart where sklad=%d and \
nomk=%d and nomd='%s' and datd='%d-%02d-%02d' order by datdp asc",
  sklad,nomkart,nomdok,gd,md,dd);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    continue;
   }

  if(kolstr == 0)
    continue;
  i_suma_uheta=0.;
  i_suma_po_dok=0.;

  kolihpod=0.;

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s-%s:\n",__FUNCTION__,gettext("Подтверждено"));

  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-%s %s\n",__FUNCTION__,row1[0],row1[1]);
    kolihpod=atof(row1[1]);
    suma_uheta=kolihpod*cena_uheta;
    suma_uheta=okrug(suma_uheta,okrg1);
    i_suma_uheta+=suma_uheta;

    suma_po_dok=atof(row1[1])*cena_v_dok;
    suma_po_dok=okrug(suma_po_dok,okrg1);
    i_suma_po_dok+=suma_po_dok;
 
    rsdat(&dp1,&mp1,&gp1,row1[0],2);

    if(sravmydat(avp.dp,avp.mp,avp.gp,dp1,mp1,gp1) < 0)
     {
      avp.dp=dp1; avp.mp=mp1; avp.gp=gp1;
     }
   }

  itogo_pod+=i_suma_uheta;

  double kofusl=i_suma_po_dok/snn.suma();
  avp.suma_bez_nds=i_suma_po_dok+sumusl*kofusl;
  avp.suma_bez_nds+=avp.suma_bez_nds*prockor/100.;
  avp.suma_uslug_bez_nds=sumusl*kofusl;

  if(nds < 0. || (kgrm == vtara && vtara != 0) || metka_nds > 0)
    avp.suma_nds=0.;
  else
    avp.suma_nds=i_suma_po_dok*pnds/100.;

  avp.suma_uheta=i_suma_uheta;
  avp.suma_s_nds=avp.suma_bez_nds+avp.suma_nds;

  if(tipz == 1)
   iceb_t_avp(&avp,"avtpromu1.alx",&sp_prov,&sum_prov_dk,ff_prot);
  if(tipz == 2)
   iceb_t_avp(&avp,"avtpromu2.alx",&sp_prov,&sum_prov_dk,ff_prot);
  
 }
zapmpr1(nomdok,avp.kodop.ravno(),sklad,time(NULL),ICEB_MP_MATU,dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot);
if(itogo_pod == 0.)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Документ не подтверждён!"),dd,md,gd,nomdok);
  iceb_t_soob(strsql);
  return;
 }
 
}

