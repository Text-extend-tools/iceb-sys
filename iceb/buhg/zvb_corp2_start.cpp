/*$Id: zvb_corp2_start.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*18.10.2014	18.08.2014	Белых А.И.	zvb_corp2_start.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg.h"



int zvb_corp2_start(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{

sprintf(imaf_dbf,"corp2.dbf");
sprintf(imaf_dbf_tmp,"corp2dbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return(1);
 }
sprintf(imafr,"corp2%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Ведомость для зачисления на карт-счета"),*ffr);

fprintf(*ffr,"\
---------------------------------------------------------------------------------------------\n");

fprintf(*ffr,gettext("\
N п/п | Т.н. |      Фамилия Имя Отчество              |N картсчёта   |Код ИНН   |  Сумма    |\n"));
/*********
123456 123456 1234567890123456789012345678901234567890 12345678901234 1234567890 1234567890 1234567890
*********/
fprintf(*ffr,"\
---------------------------------------------------------------------------------------------\n");





return(0);
}

