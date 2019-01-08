/*$Id: zvb_ibank2ua_startw.c,v 1.13 2014/04/30 06:14:59 sasa Exp $*/
/*03.10.2016	18.09.2008	Белых А.И. 	zvb_ibank2ua_startw.c
Начало формирования данных для зачисления на карт-счета для системы iBank2 UA
Если вернули 0-шапка сформирована
             1-нет
*/

#include        <errno.h>
#include    "buhg_g.h"
#include "zvb_ibank2ua.h"

int zvb_ibank2ua_start_v(class zvb_ibank2ua_rek *rek,GtkWidget *wpredok);


int zvb_ibank2ua_startw(char *imafr,//имя файла распечатки
char *imaf, //имя файла в электронном виде
class iceb_u_str *vidnahis,
const char *kod00,
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{
short mz=0,gz=0;
short d_pp=0,m_pp=0,g_pp=0;

class zvb_ibank2ua_rek rek;

if(zvb_ibank2ua_start_v(&rek,wpredok) != 0)
 return(1);

vidnahis->new_plus(rek.koment.ravno());

iceb_u_rsdat(&d_pp,&m_pp,&g_pp,rek.datpp.ravno(),1);
iceb_u_rsdat1(&mz,&gz,rek.meszp.ravno());

time_t vrem;
//struct tm *bf;
time(&vrem);
//bf=localtime(&vrem);


//int suma_kop=(int)(suma*100.);
sprintf(imaf,"ibank2ua.txt");
if((*ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"ibank2ua%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_get_pnk("00",0,wpredok));



fprintf(*ffr,"\
          ЗВЕДЕНА ВІДОМІСТЬ,\n\
      працівників %s \n\
      по заробітній платі за %02d місяць %d р.,\n\
      перерахований за платіжним дорученням N%s\n\
      від %02d.%02d.%d р.\n",iceb_get_pnk("00",0,wpredok),mz,gz,rek.nomerpp.ravno(),d_pp,m_pp,g_pp);

fprintf(*ffr,"\
--------------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п | Прізвище,і'мя, по батькові   |   ІНН    |Номер рахунку|  Сумма    |\n");
/*
12345 123456789012345678901234567890 1234567890 1234567890123 1234567890
*/
fprintf(*ffr,"\
--------------------------------------------------------------------------\n");
class iceb_u_str naim_kontr("");
class iceb_u_str mfo("");
class iceb_u_str naim_bank("");
class iceb_u_str shet_sp("");
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
/*узнаём реквизиты организации*/
sprintf(strsql,"select naikon,naiban,mfo,nomsh from Kontragent where kodkon='%s'",kod00);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
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
rek.nomerpp.ravno(),
naim_kontr.ravno(),
mfo.ravno(),
naim_bank.ravno(),
shet_sp.ravno(),
mz,
gz,
rek.koment.ravno());





return(0);
}
