/*$Id: xml_nn_mat.c,v 5.23 2014/02/28 05:13:48 sasa Exp $*/
/*21.04.2018	12.10.2006	Белых А.И.	xml_nn_mat.c
Выгруз в файл налоговой накладной в xml формате из подсистемы "Материальный учёт".
Если вернули 0- документ найден
             1- не найден
*/
#include "buhg.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/

int xml_nn_mat(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,                   //Месяц конца периода
int period_type,
const char *vid_dok,
class iceb_tu_str *if_xml)  /*Имя файла налоговой накладной в формате xml*/
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
class iceb_tu_str kontr("");
int sklad=0;
class iceb_tu_str kodop("");
short dnp=0,mnp=0,gnp=0; //Дата получения/выписки налоговой накладной
class iceb_tu_str kod_stavki("");
class iceb_tu_str kod_lgoti("");
class iceb_tu_str kontr00("00");

if(nom_nal_nak[0] == '\0')
 {
  sprintf(strsql,"%s\n%s\n%s\n%s:%d.%d.%d %s:%s %s:%s",
  gettext("Материальный учёт"),
  __FUNCTION__,
  gettext("Не введён номер налоговой накладной!"),
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер"),
  nomdok,
  gettext("Налоговый номер"),
  nom_nal_nak);
  
  

  iceb_t_soob(strsql);
  return(1);
 }

//читаем шапку документа
sprintf(strsql,"select sklad,kontr,pn,kodop,datpnn from Dokummat where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tip=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);

if(readkey(strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Материальный учёт"));
  SOOB.VVOD_SPISOK_add_MD(__FUNCTION__);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));

  if(tipz == 1)
   sprintf(strsql,"%s",gettext("Приход"));
  if(tipz == 2)
   sprintf(strsql,"%s",gettext("Расход"));

  SOOB.VVOD_SPISOK_add_MD(strsql);
   
  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%s",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер"),
  nomdok,
  gettext("Налоговый номер"),
  nom_nal_nak);

  SOOB.VVOD_SPISOK_add_MD(strsql);
  
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  
  return(1);
 }

sklad=atoi(row[0]);
kontr.new_plus(row[1]);
procent_nds=atof(row[2]);
kodop.new_plus(row[3]);
rsdat(&dnp,&mnp,&gnp,row[4],2);
int metka_imp_tov=0;
if(tipz == 2)
 {
  sprintf(strsql,"select kk from Rashod where kod='%s'",kodop.ravno_filtr());
  if(readkey(strsql,&row,&cur) == 1)
   kontr00.new_plus(row[0]);
 }

class iceb_tu_str uslov_prod("");
class iceb_tu_str forma_rash("");


class iceb_tu_str kod_form_opl("");

int lnds=0;
double sumkor=0.;

char		datop[64];
memset(datop,'\0',sizeof(datop));

int kolstr=0;

sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%d",gd,nomdok,sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
int nomer=0;
double suma_nds_v=0.;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  nomer=atoi(row[0]);

  if(nomer == 6)
   if(tipz == 1)
    suma_nds_v=atof(row[1]);
  if(nomer == 7)
     uslov_prod.new_plus(row[1]);
  if(nomer == 8)
    kod_form_opl.new_plus(row[1]);

  if(nomer == 9)
   {
    short d,m,g;
    rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && sravmydat(d,m,g,dd,md,gd) < 0)
      sprintf(datop,"%02d%02d%04d",d,m,g);
   }

  if(nomer == 11)
    lnds=atoi(row[1]);

  if(nomer == 13)
   sumkor=atof(row[1]);  
 }

kod_stavki.new_plus(iceb_t_get_kodstavki(lnds));

if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%04d",dnp,mnp,gnp);
 
if(kod_form_opl.ravno()[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_form_opl.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   forma_rash.new_plus(row[0]);
 } 


FILE *ff_xml;
char imaf_xml[512];

*pnd+=1;

//открываем файл
if(iceb_t_open_fil_nn_xml(imaf_xml,*pnd,tipz,mn,gn,&ff_xml) != 0)
 return(1);

if_xml->new_plus(imaf_xml);

iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(1,mn,gn,1),*pnd,period_type,&rek_zag_nn,ff_xml);





//Заголовок документа
iceb_t_nn10_zag_xml(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,kontr00.ravno(),kontr.ravno(),vid_dok,ff_xml);





//читаем записи в документе
sprintf(strsql,"select kodm,kolih,cena,ei,dnaim,voztar,mi,kdstv from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,sklad,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
nomer=0;


class iceb_tu_str naim_mat("");
double kolih;
double cena;
double suma;
double spnv=0.; /*cума податку на додану вартість*/

double isuma_bnds=0.;
double suma_voz_tar=0.;
int kod_grup=0;
int nomer_str=0;
class iceb_tu_str kdstv("");

while(cur.read_cursor(&row) != 0)
 {
  nomer++;


  kod_grup=0;
  sprintf(strsql,"select kodgr,naimat,ku from Material where kodm=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    kod_grup=atoi(row1[0]);
    naim_mat.new_plus(row1[1]);
    if(row[4][0] != '\0')
     {
      naim_mat.plus(" ",row[4]);
     }
   }
  else
   {
    naim_mat.new_plus("");
   }
   

  metka_imp_tov=atoi(row[6]);
  kdstv.new_plus(row[7]);
  
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=okrug(suma,okrg1);

  if(kod_grup == rek_zag_nn.kod_gr_voz_tar || atoi(row[5]) == 1)
   {
    suma_voz_tar+=suma;
    continue;
   }

  isuma_bnds+=suma;
  spnv=suma*procent_nds/100.;

  //вывод строки в файл
  iceb_t_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),row1[2],kod_stavki.ravno(),kod_lgoti.ravno(),metka_imp_tov,"",kdstv.ravno(),spnv,ff_xml);
    

 }


sprintf(strsql,"select ei,kolih,cena,naimu,ku from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class iceb_tu_str kod_usl("");
while(cur.read_cursor(&row) != 0)
 {
  nomer++;
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=okrug(suma,okrg1);

  isuma_bnds+=suma;
  spnv=suma*procent_nds/100.;
  
  if(atoi(row[4]) != 0)
   {
    sprintf(strsql,"select naius,ku from Uslugi where kodus=%s",row[4]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      kod_usl.new_plus(row1[1]);
      naim_mat.new_plus(row1[0]);
     }
    else
     {
      kod_usl.new_plus("");
      naim_mat.new_plus("");
     }
   }
   
  if(row[3][0] != '\0')
   {
    if(naim_mat.ravno()[0] != '\0')
     naim_mat.plus(" ");
    naim_mat.plus(row[4]);
   }
  //вывод строки в файл
  iceb_t_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),"",kod_stavki.ravno(),kod_lgoti.ravno(),0,kod_usl.ravno(),"",spnv,ff_xml);
 }

double suma_dok=0;
double suma_nds=0;


/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(isuma_bnds+sumkor)*procent_nds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(isuma_bnds+sumkor)*procent_nds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(tipz == 1 && suma_nds_v != 0.)
  suma_nds=suma_nds_v;

/*Загальна сума з ПДВ*/
suma_dok=isuma_bnds+sumkor+suma_nds;

//концовка файла
iceb_t_nn10_end_xml(kodop.ravno(),"matnast.alx",lnds,suma_dok,suma_nds,suma_voz_tar,ff_xml);


fclose(ff_xml);

return(0);

}

