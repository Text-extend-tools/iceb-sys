/*$Id: zvb_ukrsoc.c,v 5.26 2014/08/31 06:18:19 sasa Exp $*/
/*30.06.2013	11.02.2004	Белых А.И.	zvb_ukrsoc.c
Выгрузка для Укрсоцбанка
*/
#include        <time.h>
#include        <errno.h>
#include    "buhg.h"


void	zvb_ukrsoc(short prn,short knah,short mv,\
short gv,
char *zapros,
struct dirnupoi *poisk)
{
char strsql[1024];
int		kolstr=0;
SQL_str		row,row1;
SQLCURSOR curr;
class iceb_tu_str bros("");
class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
FILE		*ff;
FILE		*fftmp;
char            imaf[64];
char            imaftmp[64];
class iceb_tu_str imafzag("ukrsoc.txt");
time_t          vrem;
int             kom1;

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD("Введите имя файла.");

if((kom1=vvod1(&imafzag,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

time(&vrem);
struct tm *bf=localtime(&vrem);


iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    glavbuh.new_plus(row1[0]);
 }


SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
sprintf(imaftmp,"ukrsoc%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }

sprintf(imaf,"ukrsoc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"\
   Зведена відомість сум для зарахування на спеціальні карткові рахунки N_____\n\
------------------------------------------------------------------------------\n\
| Дата                             |             %02d.%02d.%04d                  |\n\
| Назва організації                |%-41.41s|\n\
| Назва та N філії АКБ \"Укрсоцбанк\"|Вінницька облана філія N050              |\n\
| Додаткова інформація             |                                         |\n\
------------------------------------------------------------------------------\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
iceb_t_get_pnk("00",0));

fprintf(ff,"\
--------------------------------------------------------------------------------------\n\
 N  |     Прізвище, ім'я по батькові         | Картковий счёт |Сумма до зарахування|\n\
--------------------------------------------------------------------------------------\n"); 

double suma=0;
class iceb_tu_str fio("");
class iceb_tu_str kshet("");
double itogo=0.;

int kolstr1=0;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;

  //Читем карточку
  sprintf(strsql,"select fio,bankshet,tipkk from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    kshet.new_plus(row1[1]);
   }

  if(kshet.ravno()[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не введён счёт !"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  suma=atof(row[4])*(-1);
  itogo+=suma;
    
  fprintf(ff,"%-4d %-*.*s %-19.19s %10.2f\n",
  ++nomer,
  iceb_tu_kolbait(40,fio.ravno()),iceb_tu_kolbait(40,fio.ravno()),fio.ravno(),
  kshet.ravno(),suma);
  
  fprintf(fftmp,"@%s,0,%.2f\n",kshet.ravno(),suma);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------\n"); 

fprintf(ff,"\
                                                          Всього: %10.2f\n",itogo);

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov.ravno(),glavbuh.ravno());

fprintf(ff,"\n\
Відмітки банку:\n\
Дата приймання Зведеної відомості на паперовому носії та магнітному носії_____________\n\
Посада та призвище, ім'я та по батькові працівника Банку, який прийняв Зведену відомість\n\n\
_____________________________________________________\n\n\
_____________________________________________________\n\n\
підпис_______________________________________________\n");


podpis(ff);

fclose(ff);
fclose(fftmp);






if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }

if((ff = fopen(imafzag.ravno(),"w")) == NULL)
 {
  error_op_nfil(imafzag.ravno(),errno,"");
  return;
 }

fprintf(ff,"@29244050010001,0,%.2f\n\
@302010,0,0,0.00\n",itogo);

while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
  fprintf(ff,"%s",strsql);
  
fclose(ff);
fclose(fftmp);
unlink(imaftmp);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

iceb_t_ustpeh(imaf,3);

oth.spis_imaf.plus(imafzag.ravno());
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_rabfil(&oth,"");


}

