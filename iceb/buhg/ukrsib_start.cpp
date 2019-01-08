/*$Id: ukrsib_start.c,v 5.13 2014/08/31 06:18:18 sasa Exp $*/
/*16.10.2010	29.01.2006	Белых А.И. 	ukrsib_start.c
Начало формирования данных для зачисления на карт-счета Укрсиббанка
*/

#include        <errno.h>
#include    "buhg.h"



int ukrsib_start(char *imafr,char *imaftmp,
class iceb_tu_str *operatorr,
FILE **fftmp,FILE **ffr)
{
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите фамилию оператора в банке"));

vvod1(operatorr,11,&VVOD1,NULL,stdscr,-1,-1);



time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaftmp,"ukrsib%d.tmp",getpid());
if((*fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }
sprintf(imafr,"ukrsib%d.lst",getpid());
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
