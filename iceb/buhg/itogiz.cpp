/*$Id: itogiz.c,v 5.10 2013/09/26 09:43:34 sasa Exp $*/
/*11.11.2009	06.09.2006	Белых А.И.	itogiz.c
Вывод итогов расчёта
*/

#include "buhg.h"


void itogiz(double inah, //Итого начислено
double iu, //Итого удержано
double ius, //Итого удержано без учёта сальдо
double sal,  //Сальдо на начало месяца
double inahb, //Итого начислено бюджет
double iub, //Итого удержано бюджет
double iusb, //Итого удержано без учёта сальдо бюджет
double salb,  //Сальдо на начало месяца бюджет
FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s)
{
double          prom;
char            st[512],st1[512],st0[512],st2[512];
char		bros[512];
char		str[1024];
double		bbr,bbr1;
double          bbprom=0.;

sprintf(str,"%s %14s",gettext("Сальдо на начало месяца -"),prnbr(sal));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(bros," %8.2f %8.2f",sal-salb,salb);
  strcat(str,bros);

 }

fprintf(kaw,"\n%s\n",str);

strfil(str,str,1,ffkor2s);

if(kawk1 != NULL)
  fprintf(kawk1,"\n%s\n",str);

if(kawk2 != NULL)
 fprintf(kawk2,"\n\n");

sprintf(bros,"\
%s %14s",
gettext("Итого начислено         -"),
prnbr(inah));

sprintf(str,"\
%s %14s",
gettext("Итого начислено         -"),
prnbr(inah));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(st1," %8.2f %8.2f",inah-inahb,inahb);
  strcat(bros,st1);
  strcat(str,st1);
 }

strfil(bros,str,0,kaw);
strfil(bros,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

prom=iu*(-1);

sprintf(bros,"\
%s %14s",
gettext("Итого удержано          -"),
prnbr(prom));

prom=ius*(-1);
sprintf(str,"\
%s %14s",
gettext("Итого удержано          -"),
prnbr(prom));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(st1," %8.2f %8.2f",(iu-iub)*-1,iub*-1);
  strcat(bros,st1);
  sprintf(st1," %8.2f %8.2f",(ius-iusb)*-1,iusb*-1);
  strcat(str,st1);
 }

strfil(bros,str,0,kaw);
strfil(bros,bros,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

bbr=inah+iu+sal;
if(bbr < 0.)
 sprintf(bros,"\
%s %14s",
 gettext("Сальдо конечное         -"),prnbr(bbr));
else
 sprintf(bros,"\
%s %14s",
 gettext("К выдаче                -"),prnbr(bbr));
if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  bbprom=inahb+iub+salb;
  sprintf(st1," %8.2f %8.2f",bbr-bbprom,bbprom);
  strcat(bros,st1);
 }


bbr1=inah+ius;

if(bbr1 < 0.)
 sprintf(str,"\
%s %14s",
 gettext("Сальдо конечное         -"),prnbr(bbr1));
else
  sprintf(str,"\
%s %14s",
 gettext("К выдаче                -"),prnbr(bbr1));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  bbprom=inahb+iusb;
  sprintf(st1," %8.2f %8.2f",bbr1-bbprom,bbprom);
  strcat(str,st1);
 }

strfil(bros,str,0,kaw);
strfil(bros,bros,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(str,"%42s %*.*s %*.*s"," ",
  iceb_tu_kolbait(8,gettext("Хозрасчёт")),
  iceb_tu_kolbait(8,gettext("Хозрасчёт")),
  gettext("Хозрасчёт"),
  iceb_tu_kolbait(8,gettext("Бюджет")),
  iceb_tu_kolbait(8,gettext("Бюджет")),
  gettext("Бюджет"));

  strfil(str,str,0,kaw);
  strfil(str,str,0,ffkor2s);
  if(kawk1 != NULL)
    fprintf(kawk1,"%s\n",str);
  if(kawk2 != NULL)
    fprintf(kawk2,"%s\n",str);
 }

/*Преобразуем в символьную строку и выводим*/
memset(st,'\0',sizeof(st));
preobr(bbr,st,0);

memset(st1,'\0',sizeof(st1));
preobr(bbr1,st1,0);

sprintf(str,"%-*.*s",iceb_tu_kolbait(62,st),iceb_tu_kolbait(62,st),st);
sprintf(st0,"%-*.*s",iceb_tu_kolbait(62,st1),iceb_tu_kolbait(62,st1),st1);

strfil(str,st0,1,kaw);
strfil(str,str,1,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"\n%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"\n%s\n",st0);

if(iceb_tu_strlen(st1) > 62)
 {
  sprintf(str,"%s",iceb_tu_adrsimv(62,st1));
  sprintf(st0,"%s",iceb_tu_adrsimv(62,st2));
  strfil(str,st0,0,kaw);
  strfil(str,str,0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",str);
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",st0);
 }

memset(st,'\0',sizeof(st));

sprintf(str,"\
------------------------------------------------------------");
strfil(str,str,0,kaw);
strfil(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

}
