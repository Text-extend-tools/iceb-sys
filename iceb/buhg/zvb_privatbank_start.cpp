/*$Id: zvb_privatbank_start.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*17.10.2014	17.10.2014	Белых А.И.	zvb_privatbank_start.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg.h"



int zvb_privatbank_start(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
class iceb_tu_str *branch,
class iceb_tu_str *zpkod,
class iceb_tu_str *rlkod,
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Формирование файлов для Приват банка"));

VV.VVOD_SPISOK_add_MD(gettext("Карточный бранч.........."));
VV.VVOD_SPISOK_add_MD(gettext("Код зарплатного проэкта.."));
VV.VVOD_SPISOK_add_MD(gettext("Назначение зачисления...."));

VV.VVOD_SPISOK_add_data(branch->ravno(),3);
VV.VVOD_SPISOK_add_data(zpkod->ravno(),3);
VV.VVOD_SPISOK_add_data(rlkod->ravno(),30);


naz:;

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

branch->new_plus(VV.VVOD_SPISOK_return_data(0));
zpkod->new_plus(VV.VVOD_SPISOK_return_data(1));
rlkod->new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

sprintf(imaf_dbf,"privatbank.dbf");
sprintf(imaf_dbf_tmp,"privatbankdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return(1);
 }
sprintf(imafr,"privatbank%d.txt",getpid());
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

