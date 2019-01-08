/*$Id: sumprved.c,v 5.15 2013-01-15 10:17:56 sasa Exp $*/
/*12.01.2018	18.02.2003	Белых А.И. 	sumprved.c
Вставка суммы прописью в шапку ведомости
*/
#include <errno.h>
#include <math.h>
#include "buhg.h"

void sumprved_1(class masiv_din_double *SUMPODR,class masiv_din_int *KOLLIST,char *imaf);

void sumprved(class masiv_din_double *SUMPODR, //Суммы по ведомостям
class masiv_din_int *KOLLIST, //Количество листов в распечатках
char *imaf)
{
/*************
if(sravmydat(31,mr,gr,5,1,2018) < 0)
 sumprved_0(SUMPODR,KOLLIST,imaf);
else
******************/
 sumprved_1(SUMPODR,KOLLIST,imaf);

}
/********************************************/
/********************************************/
void sumprved_1(class masiv_din_double *SUMPODR, //Суммы по ведомостям
class masiv_din_int *KOLLIST, //Количество листов в распечатках
char *imaf)
{
char	imaftmp[64];
class iceb_tu_str stroka("");
class iceb_tu_str strokasum("");
FILE	*fftmp,*ff;
double  suma=0.;
int	shethik=0;
int	shethikl=0;
char    bros[512];

if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

sprintf(imaftmp,"prom%d.tmp",getpid());


if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }
class iceb_t_fioruk_rk fio_vip;
class iceb_t_fioruk_rk fio_prov;
iceb_t_fioruk(3,&fio_vip);
iceb_t_fioruk(4,&fio_prov);

class iceb_tu_str naim_kontr_00(iceb_t_get_pnk("00",0));

double suma_cel=0.;
double suma_drob=0.;
int nomstr=0;
while(iceb_tu_fgets(&stroka,ff) == 0)
 {
  nomstr++;

  if(strstrm(stroka.ravno(),naim_kontr_00.ravno()) == 1) /*строка с наименованием организации*/
   {
    nomstr=5; /*В одном файле может быть несколько ведомостей*/
   }

  switch(nomstr)
   {
    case 14:
   
     suma=SUMPODR->ravno(shethik++);
     preobr(suma,&strokasum,0);

     suma_drob=modf(suma,&suma_cel)*100;

     sprintf(bros,"(%.f грн. %02.f коп.)",suma_cel,suma_drob);

     strokasum.plus(" ",bros);

     iceb_tu_vstav(&stroka,strokasum.ravno(),7,39,1);
     break;

    case 15:
      if(iceb_tu_strlen(strokasum.ravno()) > 33)
        iceb_tu_vstav(&stroka,iceb_tu_adrsimv(33,strokasum.ravno()),0,39,1);
      break;

    case 16:
      if(iceb_tu_strlen(strokasum.ravno()) > 73)
       iceb_tu_vstav(&stroka,iceb_tu_adrsimv(73,strokasum.ravno()),0,39,1);
      break;
    case 23:
      sprintf(bros,"%d",KOLLIST->ravno(shethikl++));
      iceb_tu_vstav(&stroka,bros,71,79,1);
      break;

    case 31: /*фамилия выплатившего*/
      iceb_tu_vstav(&stroka,fio_vip.famil_inic.ravno(),55,80,1);
      break;

    case 33: /*фамилия проверившего*/
      iceb_tu_vstav(&stroka,fio_prov.famil_inic.ravno(),55,80,1);
      break;
   }
        
  fprintf(fftmp,"%s",stroka.ravno());

 }
fclose(ff);
fclose(fftmp);
unlink(imaf);
rename(imaftmp,imaf);

}
