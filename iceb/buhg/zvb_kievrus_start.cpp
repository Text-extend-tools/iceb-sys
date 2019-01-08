/*$Id: zvb_kievrus_start.c,v 5.6 2013/09/26 09:43:46 sasa Exp $*/
/*18.10.2014	15.10.2009	Белых А.И.	zvb_kievrus_start.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg.h"

int zvb_kievrus_start(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{



sprintf(imaf_dbf,"kievrusbank.dbf");
sprintf(imaf_dbf_tmp,"kievrus%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return(1);
 }
sprintf(imafr,"kievrus%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Ведомость для зачисления на карт-счета"),*ffr);

fprintf(*ffr,"\
---------------------------------------------------------------------------------------------\n");

fprintf(*ffr,"\
N п/п |Таб.н.|      Прізвище,Ім'я,Побатькові          |   ІНН    |N картрахунку |  Сума     |\n");
/*********
123456 123456 1234567890123456789012345678901234567890 1234567890 12345678901234 1234567890 
*********/
fprintf(*ffr,"\
---------------------------------------------------------------------------------------------\n");

return(0);

}

