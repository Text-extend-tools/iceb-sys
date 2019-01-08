/*$Id: zvb_hreshatik_start.c,v 5.17 2013/09/26 09:43:46 sasa Exp $*/
/*29.06.2013	29.01.2006	Белых А.И. 	zvb_hreshatik_start.c
Начало формирования данных для зачисления на карт-счета банка Крещатик
Если вернули 0-шапка сформирована
             1-нет
*/

#include        <errno.h>
#include    "buhg.h"


int zvb_hreshatik_start(char *imafr,//имя файла распечатки
char *imaf, //имя файла в электронном виде
double suma, //Сумма
short *d_pp,short *m_pp,short *g_pp, //Дата платёжки
FILE **ff,  //Файл в электонном виде
FILE **ffr) //Файл распечатки
{
static class iceb_tu_str data_pp("");
static class iceb_tu_str nomer_pp("");
static class iceb_tu_str data_zp("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Формирование файлов для банка \"Крещатик\""));

VV.VVOD_SPISOK_add_MD(gettext("Дата платёжного поручения..."));
VV.VVOD_SPISOK_add_MD(gettext("Номер платёжного поручения.."));
VV.VVOD_SPISOK_add_MD(gettext("Заработная плата за (м.г)..."));

VV.VVOD_SPISOK_add_data(data_pp.ravno(),11);
VV.VVOD_SPISOK_add_data(nomer_pp.ravno(),32);
VV.VVOD_SPISOK_add_data(data_zp.ravno(),8);


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

if(rsdat(d_pp,m_pp,g_pp,VV.data_ravno(0),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата платёжного поручения !"));
  goto naz;
   
 }
short mz,gz;
if(rsdat1(&mz,&gz,VV.data_ravno(2)) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата зарплаты !"));
  goto naz;
   
 }

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);


sprintf(imaf,"PA030_%.f_POLTEH_048.%d",suma*100.,bf->tm_yday);
if((*ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
sprintf(imafr,"hreshatik%d.txt",getpid());
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
      від %02d.%02d.%d р.\n",iceb_t_get_pnk("00",0),mz,gz,nomer_pp.ravno(),*d_pp,*m_pp,*g_pp);

fprintf(*ffr,"\
-------------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п | Прізвище,і'мя, по батькові   |   ІНН    |Номер рахунку|  Сумма    |\n");
/*
12345 123456789012345678901234567890 1234567890 1234567890123 1234567890
*/
fprintf(*ffr,"\
-------------------------------------------------------------------------\n");


fprintf(*ff,"00\
%04d%02d%02d\
%02d%02d%02d\
%1s\
%12s\
%-8s\
%04d%02d%02d\
%012.2f\
00\n",
bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday,
bf->tm_hour,bf->tm_min,bf->tm_sec,
" ",
" ",
nomer_pp.ravno(),
*g_pp,*m_pp,*d_pp,
suma);

return(0);
}
