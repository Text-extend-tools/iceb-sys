/*$Id: ukreksim_hap.c,v 5.9 2013/09/26 09:43:41 sasa Exp $*/
/*16.07.2015	25.01.2006	Белых А.И.	ukreksim_hap.c
Начало формирования распечатки и файла для записи на дискету для Укрэксимбанка
*/
#include <errno.h>
#include "buhg.h"


int ukreksim_hap(char *imafr,char *imaftmp,FILE **fftmp,FILE **ffr)
{

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaftmp,"ukreksim%d.tmp",getpid());
if((*fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }
sprintf(imafr,"ukreksim%d.lst",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_t_get_pnk("00",0));

fprintf(*ffr,"\
          Відомість для зарахування на карт-рахунки N_____\n\
                від %d.%d.%dр.\n\n",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

fprintf(*ffr,"\
-----------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п|Таб.номер| Карт-счёт |  Сумма    | Прізвище,і'мя, по батькові\n");
fprintf(*ffr,"\
-----------------------------------------------------------------------\n");
return(0);
}

