/*$Id: rasrnn.c,v 5.70 2014/07/31 07:09:17 sasa Exp $*/
/*15.07.2015	30.08.2005	Белых А.И.	rasrnn.c
Расчёт реестра полученных и выданных налоговых накладных
*/
#include        <errno.h>
#include "buhg.h"
#include "xrnn_poi.h"
#include "rnn_d5.h"

int rasrnn_p(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5);
int rasrnn_v(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5);

int rasrnn_sap_xml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_xml_dot,char *imaf_xml_rozkor,int *period_type,double proc_dot,int nomer_porcii,FILE **ff_xml,FILE **ff_xml_dot,FILE **ff_xml_rozkor);
int rnnrd5(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char*,char*,double,class rnn_d5 *dod5);


extern class iceb_rnfxml_data rek_zag_nn;

class iceb_tu_str kat_for_nal_nak;

int rasrnn(class spis_oth *oth)
{
static class xrnn_poi data;

static  class iceb_tu_str dat1(""),dat2("");
int kom=0;
int kom1=0;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать реестро налоговых накладных"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)...................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)...................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код группы полученных документов.(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код группы выданных документов...(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Полученные/выданные документы...(+/-).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Процент для реестра перер-ющего пр-ва.."));//5
VV.VVOD_SPISOK_add_MD(gettext("Номер порции..........................."));//6

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(data.kodgr.ravno(),128); 
VV.VVOD_SPISOK_add_data(data.kodgr1.ravno(),128); 
VV.VVOD_SPISOK_add_data(data.pr_ras.ravno(),2); 
VV.VVOD_SPISOK_add_data(data.proc_dot.ravno(),32); 
VV.VVOD_SPISOK_add_data(data.nomer_porcii.ravno(),16); 

naz:;

 
helstr(LINES-1,0,\
"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("груп."),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
data.kodgr.new_plus(VV.data_ravno(2));
data.kodgr1.new_plus(VV.data_ravno(3));
data.pr_ras.new_plus(VV.data_ravno(4));
data.proc_dot.new_plus(VV.data_ravno(5));
data.nomer_porcii.new_plus(VV.data_ravno(6));

class iceb_tu_str kod("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("rnn3.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп полученных документов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп выданных документов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
         kod.new_plus(VV.data_ravno(2));
         if(vibrek("Reegrup1",&kod) == 0)
          VV.data_plus(2,kod.ravno());
         break;
      case 1 :
         kod.new_plus(VV.data_ravno(3));
         if(vibrek("Reegrup2",&kod) == 0)
          VV.data_plus(3,kod.ravno());
         break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  case FK5: //Настройка
    iceb_redfil("rnn_nast.alx",0);
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&data.dn,&data.mn,&data.gn,dat1.ravno(),&data.dk,&data.mk,&data.gk,dat2.ravno()) != 0)
 goto naz;

if(rasrnn_r(&data,oth) != 0)
 goto naz;
return(0); 
}
/***************************************/
/*Распечатка реестров*/
/***************************/
int rasrnn_r(class xrnn_poi *data,class spis_oth *oth)
{

/*Создаём каталог*/
if(iceb_t_make_kat(&kat_for_nal_nak,"nal_nak_xml") != 0)
 return(1);

//Чистим каталог от файлов
iceb_t_delfil(kat_for_nal_nak.ravno(),"xml");

FILE *ff_xml;
char imaf_xml[64];

FILE *ff_lst_rozkor=NULL;
char imaf_lst_rozkor[64]; /*розрахунок коригування сумм ПДВ*/
sprintf(imaf_lst_rozkor,"rk%d.lst",getpid());

FILE *ff_xml_rozkor=NULL;
char imaf_xml_rozkor[64]; 

/************
  из списка выданных накладных делается Коригування податковых зобов'язань
  из списка полученных накладных делается Коригування податкового кредиту 
*************/


FILE *ff_xml_dot=NULL;
char imaf_xml_dot[64];

memset(imaf_xml_dot,'\0',sizeof(imaf_xml_dot));

int period_type=0;

if(rasrnn_sap_xml(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,
imaf_xml,imaf_xml_dot,imaf_xml_rozkor,&period_type,data->proc_dot.ravno_atof(),data->nomer_porcii.ravno_atoi(),&ff_xml,&ff_xml_dot,&ff_xml_rozkor) != 0)
 return(1);


if((ff_lst_rozkor=fopen(imaf_lst_rozkor,"w")) == NULL)
 {
  error_op_nfil(imaf_lst_rozkor,errno,"");
  return(1);
 }

zagolov("Розрахунок коригування сум ПДВ до податкової\nдекларації з податку на додану вартість",data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_t_get_pnk("00",1),ff_lst_rozkor);

FILE *ff;
char imaf[112];
sprintf(imaf,"rnn%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

FILE *ff_dot=NULL;
char imaf_dot[112];
memset(imaf_dot,'\0',sizeof(imaf_dot));

if(data->proc_dot.ravno_atof() != 0.)
 {
  sprintf(imaf_dot,"rnnd%d.lst",getpid());

  if((ff_dot=fopen(imaf_dot,"w")) == NULL)
   {
    error_op_nfil(imaf_dot,errno,"");
    return(1);
   }

 }
 
FILE *ff_prot;
char imaf_prot[112];
sprintf(imaf_prot,"rnnp%d.lst",getpid());

if((ff_prot=fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

class rnn_d5 dod5;

if(sravmydat(data->dn,data->mn,data->gn,1,1,2011) < 0)
 {
  if(data->pr_ras.ravno()[0] == '\0' || data->pr_ras.ravno()[0] == '+')
   {
    if(rasrnn_p(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,&dod5) == 0)
     if(data->pr_ras.ravno()[0] == '\0' )
      {
       fprintf(ff,"\f");
       if(ff_dot != NULL)
         fprintf(ff_dot,"\f");
      }
   }
  
  if(data->pr_ras.ravno()[0] == '\0' || data->pr_ras.ravno()[0] == '-')
   {

    rasrnn_v(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,&dod5); 
   }
 }

if(sravmydat(data->dn,data->mn,data->gn,1,1,2011) >= 0)
 {

  if(data->pr_ras.ravno()[0] == '\0' || data->pr_ras.ravno()[0] == '-')
   {
    if(rasrnn_v(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,&dod5) == 0)
     if(data->pr_ras.ravno()[0] == '\0' )
      {
       fprintf(ff,"\f");
       if(ff_dot != NULL)
         fprintf(ff_dot,"\f");
      }
   }

  if(data->pr_ras.ravno()[0] == '\0' || data->pr_ras.ravno()[0] == '+')
   {
    rasrnn_p(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,&dod5);
   }
 }

podpis(ff_lst_rozkor);
fclose(ff_lst_rozkor);


class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glbuh;
iceb_t_fioruk(1,&rukov);
iceb_t_fioruk(2,&glbuh);

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ff_xml_rozkor,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml_rozkor,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml_rozkor,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
fprintf(ff_xml_rozkor,"   <HFO xsi:nil=\"true\"></HFO>\n");
fprintf(ff_xml_rozkor," </DECLARBODY>\n");
fprintf(ff_xml_rozkor,"</DECLAR>\n");
fclose(ff_xml_rozkor);

fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
fprintf(ff_xml,"  </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);

if(ff_xml_dot != NULL)
 {
  fprintf(ff_xml_dot,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
  fprintf(ff_xml_dot,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
  fprintf(ff_xml_dot,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
  fprintf(ff_xml_dot,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
  fprintf(ff_xml_dot,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
  fprintf(ff_xml_dot,"  </DECLARBODY>\n");
  fprintf(ff_xml_dot,"</DECLAR>\n");
  fclose(ff_xml_dot);
 }
podpis(ff);
fclose(ff);

if(ff_dot != NULL)
 {
  podpis(ff_dot);
  fclose(ff_dot);

 }
podpis(ff_prot);
fclose(ff_prot);

char imaf_dod5_xml[112];
char imaf_svod_naim[512];

char imaf_dod5_xml_dot[112];
char imaf_svod_dot[64];
memset(imaf_dod5_xml_dot,'\0',sizeof(imaf_dod5_xml_dot));
memset(imaf_svod_dot,'\0',sizeof(imaf_svod_dot));

rnnrd5(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,imaf_dod5_xml,imaf_svod_naim,imaf_dod5_xml_dot,imaf_svod_dot,data->proc_dot.ravno_atof(),&dod5);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus("Реєстр податкових накладних (Загальний)");
if(imaf_dot[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_dot);
  oth->spis_naim.plus("Реэстр податкових накладных (Переробного в-ва)");
 }

if(imaf_svod_dot[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_svod_dot);
  oth->spis_naim.plus("Додаток 2");
 }
oth->spis_imaf.plus(imaf_svod_naim);
oth->spis_naim.plus("Додаток 5 з найменуванням контрагентів");

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

oth->spis_imaf.plus(imaf_lst_rozkor);
oth->spis_naim.plus("Додаток 1");


for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imaf_xml);
oth->spis_naim.plus("Реєстр податкових накладних (Загальний) в форматі xml");

oth->spis_imaf.plus(imaf_xml_rozkor);
oth->spis_naim.plus("Додаток 1 в форматі xml");
 
if(imaf_xml_dot[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_xml_dot);
  oth->spis_naim.plus("Реєстр податкових накладних в форматі xml (Переробного в-ва)");
 }


oth->spis_imaf.plus(imaf_dod5_xml);
oth->spis_naim.plus("Додаток 5 в форматі xml");
if(imaf_dod5_xml_dot[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_dod5_xml_dot);
  oth->spis_naim.plus("Додаток 2 в форматі xml");
 }

return(0);
}

/***********************************/
/*Шапка xml файла*/
/**************************/

int rasrnn_sap_xml(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,
char *imaf_xml_dot,
char *imaf_xml_rozkor,
int *period_type,double proc_dot,
int nomer_porcii,
FILE **ff_xml,
FILE **ff_xml_dot,
FILE **ff_xml_rozkor)
{
iceb_t_rnfxml(&rek_zag_nn); /*чтение данных для xml файлов*/

if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,iceb_t_getkoddok(dn,mn,gn,2),nomer_porcii,period_type,&rek_zag_nn,ff_xml) != 0)  /*Основной реестр*/
 return(1);
iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(dn,mn,gn,2),nomer_porcii,*period_type,&rek_zag_nn,*ff_xml); /*Основной реестр*/

fprintf(*ff_xml," <DECLARBODY>\n");
fprintf(*ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml,"   <HNP>%d</HNP>\n",nomer_porcii); /*Номер порції реестру*/
fprintf(*ff_xml,"   <HZKV></HZKV>\n");
fprintf(*ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
fprintf(*ff_xml,"   <HLOC>%s</HLOC>\n",iceb_tu_filtr_xml(rek_zag_nn.adres.ravno()));
//fprintf(*ff_xml,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
fprintf(*ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
fprintf(*ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
fprintf(*ff_xml,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
fprintf(*ff_xml,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
fprintf(*ff_xml,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
fprintf(*ff_xml,"   <H02G02S>1</H02G02S>\n"); //номер документа
fprintf(*ff_xml,"   <H03G02I>1</H03G02I>\n"); // количество страниц
fprintf(*ff_xml,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах



/*Расчёт корректировки сумм НДС к налоговой декларации с налога на добавленную стоимость-додаток 1*/

if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_rozkor,iceb_t_getkoddok(dn,mn,gn,3),nomer_porcii,period_type,&rek_zag_nn,ff_xml_rozkor) != 0)  /*Основной реестр*/
 return(1);
iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(dn,mn,gn,3),nomer_porcii,*period_type,&rek_zag_nn,*ff_xml_rozkor); /*Основной реестр*/


fprintf(*ff_xml_rozkor," <DECLARBODY>\n");
fprintf(*ff_xml_rozkor,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml_rozkor,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml_rozkor,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml_rozkor,"   <HZYP xsi:nil=\"true\"></HZYP>\n");
fprintf(*ff_xml_rozkor,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
fprintf(*ff_xml_rozkor,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
fprintf(*ff_xml_rozkor,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
fprintf(*ff_xml_rozkor,"   <HTIN>%s</HTIN>\n",rek_zag_nn.kod_edrpuo.ravno());
//rintf(*ff_xml_rozkor,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.innn);
fprintf(*ff_xml_rozkor,"   <HDDGVSD xsi:nil=\"true\"></HDDGVSD>\n");
fprintf(*ff_xml_rozkor,"   <HNDGVSD xsi:nil=\"true\"></HNDGVSD>\n");
//fprintf(*ff_xml_rozkor,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.nspnds);









if(proc_dot != 0.)
 {
  /*отличается от основного только порядковым номером документа*/

  if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,iceb_t_getkoddok(dn,mn,gn,2),nomer_porcii+1,period_type,&rek_zag_nn,ff_xml_dot) != 0) /*Дополнительный*/
     return(1);
  iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(dn,mn,gn,2),nomer_porcii+1,*period_type,&rek_zag_nn,*ff_xml_dot); /*Дополнительный*/

  fprintf(*ff_xml_dot," <DECLARBODY>\n");
  fprintf(*ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(*ff_xml_dot,"   <HZ>1</HZ>\n"); /*Звітний*/
  fprintf(*ff_xml_dot,"   <HNP>%d</HNP>\n",nomer_porcii); /*Номер порції реестру*/
  fprintf(*ff_xml_dot,"   <HZKV></HZKV>\n");
  fprintf(*ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(*ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
  fprintf(*ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_tu_filtr_xml(rek_zag_nn.adres.ravno()));
  //fprintf(*ff_xml_dot,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
  fprintf(*ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
  fprintf(*ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
  fprintf(*ff_xml_dot,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
  fprintf(*ff_xml_dot,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
  fprintf(*ff_xml_dot,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
  fprintf(*ff_xml_dot,"   <H02G02S>1</H02G02S>\n"); //номер документа
  fprintf(*ff_xml_dot,"   <H03G02I>1</H03G02I>\n"); // количество страниц
  fprintf(*ff_xml_dot,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах
 }



class iceb_tu_str bros("");
if(iceb_t_poldan("Код группы возвратная тара",&bros,"matnast.alx") == 0)
  rek_zag_nn.kod_gr_voz_tar=bros.ravno_atoi();

return(0);

}
