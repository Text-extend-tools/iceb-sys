/*$Id: xml_nn_ukr.c,v 5.17 2014/02/28 05:13:48 sasa Exp $*/
/*06.12.2018	16.10.2006	Белых А.И.	xml_nn_ukr.c
Выгрузка налоговых накладных для подсистемы "Учёт командировочных расходов".
*/

#include "buhg.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/

int xml_nn_ukr(int *pnd, //порядковый номер документа
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,
int period_type,
const char *vid_dok,
class iceb_tu_str *imafil_xml)
{
int tipz=1; //1-приходный документ 2-расходный

char strsql[512];
SQL_str row;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
class iceb_tu_str kontr("");
double spnv=0.;

//может быть в документе несколько записей с одинаковым номером налоговой накладной - используем цену для однозначного определения
//Читаем запись в документе
sprintf(strsql,"select kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd='%04d-%02d-%02d' and \
ndrnn='%s' and nomd='%s'",
gd,md,dd,nom_nal_nak,nomdok);

//читаем шапку документа
//sprintf(strsql,"select kontr from Ukrdok where datd='%04d-%02d-%02d' and nomd='%s'",gd,md,dd,nomdok);
if(readkey(strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Учёт командировочных расходов"));
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
class iceb_tu_str ei(row[3]);

kontr.new_plus(row[5]);
double suma_nds_v=atof(row[4]);
int lnds=0;
if(suma_nds_v == 0.)
 lnds=3;
class iceb_tu_str kod_stavki(iceb_t_get_kodstavki(lnds));
 
class iceb_tu_str eiz(row[3]);

double kolih=atof(row[1]);
double cena=atof(row[2]);
double suma=cena;
double isuma_bnds=suma;

char datop[64];
memset(datop,'\0',sizeof(datop));
sprintf(datop,"%02d%02d%04d",dd,md,gd);



short dnp=dd,mnp=md,gnp=gd;


procent_nds=iceb_t_pnds(dd,md,gd);


//читаем наименование
class iceb_tu_str naim_mat("");
sprintf(strsql,"select naim from Ukrkras where kod=%s",row[0]);
if(readkey(strsql,&row,&cur) == 1)
 naim_mat.new_plus(row[0]);


FILE *ff_xml;
char imaf_xml[512];

*pnd+=1;

//открываем файл
if(iceb_t_open_fil_nn_xml(imaf_xml,*pnd,tipz,mn,gn,&ff_xml) != 0)
 return(1);

imafil_xml->new_plus(imaf_xml);
iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(1,mn,gn,1),*pnd,period_type,&rek_zag_nn,ff_xml);

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
spnv=suma_nds;
//Заголовок документа
iceb_t_nn10_zag_xml(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,"00",kontr.ravno(),vid_dok,ff_xml);






class iceb_tu_str kod_lgoti("");
//вывод строки в файл
iceb_t_nn10_str_xml(1,kolih,cena,suma,ei.ravno(),naim_mat.ravno(),"",kod_stavki.ravno(),kod_lgoti.ravno(),0,"","",spnv,ff_xml);




//концовка файла
iceb_t_nn10_end_xml("","",lnds,suma_dok,suma_nds,0,ff_xml);


fclose(ff_xml);

return(0);
}
