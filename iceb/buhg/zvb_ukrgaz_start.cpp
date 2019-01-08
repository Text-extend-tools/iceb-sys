/*$Id: zvb_ukrgaz_start.c,v 5.8 2013/09/26 09:43:46 sasa Exp $*/
/*16.10.2009	15.10.2009	Белых А.И.	zvb_ukrgaz_start.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif
//#include        "dbfhead.h"



int zvb_ukrgaz_start(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{



sprintf(imaf_dbf,"ukrgazbank.dbf");
sprintf(imaf_dbf_tmp,"ukrgazdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return(1);
 }
sprintf(imafr,"ukrgaz%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(*ffr,"Відомість для зарахування на карт-рахунки\n");

fprintf(*ffr,"\
--------------------------------------------------------------------------------------------\n");

fprintf(*ffr,"\
N п/п |Таб.н.|      Прізвище Ім'я Побатькові          |   ІНН    |N картрахунку |  Сумма    |\n");
/*********
123456 123456 1234567890123456789012345678901234567890 1234567890 12345678901234 1234567890 
*********/
fprintf(*ffr,"\
--------------------------------------------------------------------------------------------\n");





return(0);
}

