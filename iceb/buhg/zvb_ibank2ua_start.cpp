/*$Id: zvb_ibank2ua_start.c,v 5.16 2014/04/30 06:12:27 sasa Exp $*/
/*03.10.2016	18.09.2008	Белых А.И. 	zvb_ibank2ua_start.c
Начало формирования данных для зачисления на карт-счета для системы iBank2 UA
Если вернули 0-шапка сформирована
             1-нет
*/

#include        <errno.h>
#include    "buhg.h"



int zvb_ibank2ua_start(char *imafr,//имя файла распечатки
char *imaf, //имя файла в электронном виде
class iceb_tu_str *vidnahis,
const char *kod00,
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{
short d_pp=0,m_pp=0,g_pp=0; //Дата платёжки
static class iceb_tu_str data_pp("");
static class iceb_tu_str nomer_pp("");
static class iceb_tu_str data_zp("");
static class iceb_tu_str vid_nah(gettext("Заработная плата и авансы"));


VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Формирование файлов для системы iBank2 UA"));

VV.VVOD_SPISOK_add_MD(gettext("Дата документа............."));
VV.VVOD_SPISOK_add_MD(gettext("Номер документа............"));
VV.VVOD_SPISOK_add_MD(gettext("Заработная плата за (м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Вид начисления............."));

VV.VVOD_SPISOK_add_data(data_pp.ravno(),11);
VV.VVOD_SPISOK_add_data(nomer_pp.ravno(),16);
VV.VVOD_SPISOK_add_data(data_zp.ravno(),8);
VV.VVOD_SPISOK_add_data(vid_nah.ravno(),64);


naz:;

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

data_pp.new_plus(VV.VVOD_SPISOK_return_data(0));
nomer_pp.new_plus(VV.VVOD_SPISOK_return_data(1));
data_zp.new_plus(VV.VVOD_SPISOK_return_data(2));
vid_nah.new_plus(VV.VVOD_SPISOK_return_data(3));

vidnahis->new_plus(vid_nah.ravno());

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

if(rsdat(&d_pp,&m_pp,&g_pp,VV.data_ravno(0),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата документа!"));
  goto naz;
   
 }
short mz,gz;
if(rsdat1(&mz,&gz,VV.data_ravno(2)) != 0)
 {
  iceb_t_soob("Не правильно введена дата зарплаты!");
  goto naz;
   
 }

sprintf(imaf,"ibank2ua.txt");
if((*ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
sprintf(imafr,"ibank2ua%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  error_op_nfil(imafr,errno,"");
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_t_get_pnk("00",0));



fprintf(*ffr,"\
          ЗВЕДЕНА ВІДОМІСТЬ,\n\
      працівників %s \n\
      по заробітній платі за %02d місяць %d р.,\n\
      перерахований за платіжним дорученням N%s\n\
      від %02d.%02d.%d р.\n",iceb_t_get_pnk("00",0),mz,gz,nomer_pp.ravno(),d_pp,m_pp,g_pp);

fprintf(*ffr,"\
--------------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п | Прізвище,і'мя, по батькові   |   ІНН    |Номер рахунку|  Сумма    |\n");
/*
12345 123456789012345678901234567890 1234567890 1234567890123 1234567890
*/
fprintf(*ffr,"\
--------------------------------------------------------------------------\n");
class iceb_tu_str naim_kontr("");
class iceb_tu_str mfo("");
class iceb_tu_str naim_bank("");
class iceb_tu_str shet_sp("");
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
/*узнаём реквизиты организации*/
sprintf(strsql,"select naikon,naiban,mfo,nomsh from Kontragent where kodkon='%s'",kod00);
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_kontr.new_plus(row[0]);
  naim_bank.new_plus(row[1]);
  mfo.new_plus(row[2]);
  shet_sp.new_plus(row[3]);
 }

fprintf(*ff,"\
Content-Type=doc/pay_sheet\n\n\
DATE_DOC=%02d.%02d.%04d\n\
NUM_DOC=%s\n\
CLN_NAME=%s\n\
PAYER_BANK_MFO=%s\n\
PAYER_BANK_NAME=%s\n\
PAYER_ACCOUNT=%s\n\
BEGIN_MONTH=%02d\n\
BEGIN_YEAR=%d\n\
ONFLOW_TYPE=%s\n",
d_pp,m_pp,g_pp,
nomer_pp.ravno(),
naim_kontr.ravno(),
mfo.ravno(),
naim_bank.ravno(),
shet_sp.ravno(),
mz,
gz,
vid_nah.ravno());





return(0);
}
