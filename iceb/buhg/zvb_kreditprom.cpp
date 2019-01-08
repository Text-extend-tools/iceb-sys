/*$Id: zvb_kreditprom.c,v 5.25 2013/09/26 09:43:46 sasa Exp $*/
/*18.10.2014	31.05.2005	Белых А.И.	zvb_kreditprom.c
Выгрузка данных для Кредитпром банка
*/
#include        <math.h>
#include        <errno.h>
#include    "buhg.h"


void	zvb_kreditprom(short prn,char *zapros,struct dirnupoi *poisk)
{
char strsql[512];
class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
class iceb_tu_str bros("");
SQL_str row1;
SQLCURSOR cur1;
class iceb_tu_str imaf_d("");
short dt,mt,gt;
int kom1=0;

poltekdat(&dt,&mt,&gt);

sprintf(strsql,"Pa3%02d0001.iss",mt);
imaf_d.new_plus(strsql);

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD("Введите имя файла.");

if((kom1=vvod1(&imaf_d,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    glavbuh.new_plus(row1[0]);
 }


SQLCURSOR cur;
int kolstr;
if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),zapros);
  return;
 }
char imaf_r[64];
FILE *ffr;
FILE *ffd;
sprintf(imaf_r,"kpb%d.lst",getpid());

if((ffd = fopen(imaf_d.ravno(),"w")) == NULL)
 {
  error_op_nfil(imaf_d.ravno(),errno,"");
  return;
 }

if((ffr = fopen(imaf_r,"w")) == NULL)
 {
  error_op_nfil(imaf_r,errno,"");
  return;
 }

fprintf(ffr,"%s\n\n",iceb_t_get_pnk("00",0));

fprintf(ffr,"\
          Відомість для зарахування на карт-рахунки N_____\n\
                від %d.%d.%dр.\n\n",dt,mt,gt);

fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"\
 Н/п|Таб.номер| Карт-счёт |  Сумма    | Прізвище,і'мя, по батькові\n");
fprintf(ffr,"\
-----------------------------------------------------------------------\n");

fprintf(ffd,"%s%04d%02d%02d\n",imaf_d.ravno(),gt,mt,dt);


int kolstr1=0;
SQL_str row;
double sumai=0.;
class iceb_tu_str fio("");
class iceb_tu_str kartshet("");
double suma=0.;
int nomer=0;
int suma_kop=0;
int isuma_kop=0;
double suma_cel;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(poidirnu(row,poisk,&fio) != 0)
     continue;
  
  sprintf(strsql,"select fio,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    kartshet.new_plus(row1[1]);
   }
  else
   kartshet.new_plus("");
  if(kartshet.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }

  suma=atof(row[4]);
  if(prn == 2)
    suma*=(-1);   
  suma_cel=suma=okrug(suma,0.01);
  suma_cel*=100.;
    
  modf(suma_cel,&suma_cel);

  suma_kop=(int)suma_cel;
  isuma_kop+=suma_kop;
   
  fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",
  ++nomer,row[1],kartshet.ravno(),suma,fio.ravno());
  sumai+=suma;
  
  fprintf(ffd,"%-14s%12dUAH%04d%02d%02d\n",kartshet.ravno(),suma_kop,gt,mt,dt);
 }
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(29,"Разом"),"Разом",sumai);

fprintf(ffd,"%s%-8d%-14d\n",imaf_d.ravno(),nomer+2,isuma_kop);

fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov.ravno(),glavbuh.ravno());

podpis(ffr);

fclose(ffr);
fclose(ffd);

class spis_oth oth;

oth.spis_imaf.plus(imaf_r);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета."));

iceb_t_ustpeh(imaf_r,3);

oth.spis_imaf.plus(imaf_d.ravno());
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_t_rabfil(&oth,"");
 
}
