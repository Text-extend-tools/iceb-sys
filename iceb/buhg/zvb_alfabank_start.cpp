/*$Id: zvb_alfabank_start.c,v 5.11 2014/08/31 06:18:19 sasa Exp $*/
/*17.10.2014	15.10.2009	Белых А.И.	zvb_alfabank_start.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg.h"



int zvb_alfabank_start(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{
static class iceb_tu_str nomerb("");
static class iceb_tu_str pornom("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Формирование файлов для банка \"Альфабанк\""));

VV.VVOD_SPISOK_add_MD(gettext("Номер, присваиваемый банком............."));
VV.VVOD_SPISOK_add_MD(gettext("Порядковый номер файла за текущюю дату.."));

VV.VVOD_SPISOK_add_data(nomerb.ravno(),6);
VV.VVOD_SPISOK_add_data(pornom.ravno(),3);


naz:;

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

nomerb.new_plus(VV.VVOD_SPISOK_return_data(0));
pornom.new_plus(VV.VVOD_SPISOK_return_data(1));

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


time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaf_dbf,"ZP%05d%02d%02d%02d%02d.dbf",nomerb.ravno_atoi(),bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900-2000,pornom.ravno_atoi());
sprintf(imaf_dbf_tmp,"alfabankdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return(1);
 }
sprintf(imafr,"alfabank%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(*ffr,"%s\n",gettext("Ведомость для зачисления на карт-счета"));

fprintf(*ffr,"\
-------------------------------------------------------------------------------------------------\n");

fprintf(*ffr,gettext("\
N п/п |      Фамилия Имя Отчество              |N картсчёта   |  Сумма    |Код ИНН   |Код ЕГРПОУ|\n"));
/*********
123456 1234567890123456789012345678901234567890 12345678901234 1234567890 1234567890 1234567890
*********/
fprintf(*ffr,"\
-------------------------------------------------------------------------------------------------\n");





return(0);
}

