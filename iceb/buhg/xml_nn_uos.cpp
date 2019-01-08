/*$Id: xml_nn_uos.c,v 5.18 2014/07/31 07:09:18 sasa Exp $*/
/*06.12.2018	13.10.2006	Белых А.И.	xml_nn_uos.c
Выгрузка в xml файл налоговой накладной из "Учёта основных средств"
*/
#include "buhg.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/

int xml_nn_uos(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,
int period_type,
const char *vid_dok,
class iceb_tu_str *if_xml)
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
class iceb_tu_str kontr("");
int podr=0;
class iceb_tu_str kodop("");
class iceb_tu_str kod_stavki("");
class iceb_tu_str kod_lgoti("");
class iceb_tu_str kontr00("00");


//читаем шапку документа
sprintf(strsql,"select kontr,podr,forop,datpnn,datop,nds,pn,kodop from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tipz=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Учёт основных средств"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));

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

podr=atoi(row[1]);
kontr.new_plus(row[0]);
int lnds=atoi(row[5]);
kodop.new_plus(row[7]);

kod_stavki.new_plus(iceb_t_get_kodstavki(lnds));

class iceb_tu_str kod_forop(row[2]);

char datop[64];
memset(datop,'\0',sizeof(datop));
short dop,mop,gop;
rsdat(&dop,&mop,&gop,row[4],2);
if(dop != 0)
 if(sravmydat(dop,mop,gop,dd,md,gd) < 0)
  sprintf(datop,"%02d%02d%d",dop,mop,gop);
if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%d",dd,md,gd);
 


short dnp,mnp,gnp;
rsdat(&dnp,&mnp,&gnp,row[3],2);


procent_nds=atof(row[6]);

class iceb_tu_str forma_rash("");

if(kod_forop.ravno()[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_forop.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   forma_rash.new_plus(row[0]);
 } 

if(tipz == 2)
 {
  sprintf(strsql,"select kk from Uosras where kod='%s'",kodop.ravno_filtr());
  if(readkey(strsql,&row,&cur) == 1)
   kontr00.new_plus(row[0]);
 }

FILE *ff_xml;
char imaf_xml[512];

*pnd+=1;

//открываем файл
if(iceb_t_open_fil_nn_xml(imaf_xml,*pnd,tipz,mn,gn,&ff_xml) != 0)
 return(1);


if_xml->new_plus(imaf_xml);

//Заголовок файла
iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(1,mn,gn,1),*pnd,period_type,&rek_zag_nn,ff_xml);



//Заголовок документа
iceb_t_nn10_zag_xml(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,kontr00.ravno(),kontr.ravno(),vid_dok,ff_xml);


int kolstr=0;
//читаем записи в документе
sprintf(strsql,"select innom,bs,bsby,cena from Uosdok1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s'",gd,md,dd,podr,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
int nomer=0;
class iceb_tu_str naim_mat("");
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double bs=0.;
double bsby=0.;
int nomer_str=0;
double spnv=0.;

while(cur.read_cursor(&row) != 0)
 {
  nomer++;

  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
    naim_mat.new_plus(row1[0]);
  else
    naim_mat.new_plus("");
  

  bs=atof(row[1]);
  bsby=atof(row[2]);
  kolih=1.;

  if(tipz == 2)
    cena=atof(row[3]);
  else
   {
    if(bs != 0.)
     cena=bs;
    else
     cena=bsby;
   }

  cena=okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=okrug(suma,okrg1);


  isuma_bnds+=suma;
  spnv=suma*procent_nds/100.;    
  //вывод строки в файл
  iceb_t_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),row1[0],kod_stavki.ravno(),kod_lgoti.ravno(),0,"","",spnv,ff_xml);


}

double suma_dok=0;
double suma_nds=0;
/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(isuma_bnds)*procent_nds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(isuma_bnds)*procent_nds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=isuma_bnds+suma_nds;


//концовка файла
iceb_t_nn10_end_xml(kodop.ravno(),"uosnast.alx",lnds,suma_dok,suma_nds,0,ff_xml);


fclose(ff_xml);

return(0);


 }
