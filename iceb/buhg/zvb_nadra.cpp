/*$Id: zvb_nadra.c,v 5.24 2013/08/13 05:50:04 sasa Exp $*/
/*18.10.2014	26.09.2003	Белых А.И.	zvb_nadra.c
Получение распечатки для передачи данных по зарплате в банк Надра
*/
#include        <errno.h>
#include    "buhg.h"


void	zvb_nadra(short prn,char *zapros,struct dirnupoi *poisk)
{
char		strsql[512];
int		kolstr=0;
SQL_str		row,row1;
SQLCURSOR curr;
class iceb_tu_str bros("");
class iceb_tu_str rukov("");
class iceb_tu_str glavbuh("");
FILE		*ff;
char            imaf[64];


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

sprintf(imaf,"nadra%d.lst",iceb_t_getuid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"\
                                      до Договору про надання послуг\n\
                                      N___________\"____\"________2003р.\n\n\
      Підприємство %s\n\n\
                     Зведена відомість N_____від \"_____\"____________р.\n\n",iceb_t_get_pnk("00",0));
                     

fprintf(ff,"\
----------------------------------------------------------------------------\n\
 N  |N картрахунку|  Тип картки  |          ПІБ                 |   Сумма  |\n\
----------------------------------------------------------------------------\n"); 
double suma=0;
class iceb_tu_str fio("");
class iceb_tu_str kshet("");
double itogo=0.;
class iceb_tu_str tipkk("");

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
    tipkk.new_plus(row1[2]);
   }
  else
   {
    kshet.new_plus("");
   }
  suma=atof(row[4]);
  
  if(prn == 2)
   suma*=-1;
   
  itogo+=suma;
    
  fprintf(ff,"%-4d %-13s %-*s %-*.*s %10.2f\n",
  ++nomer,kshet.ravno(),iceb_tu_kolbait(14,tipkk.ravno()),tipkk.ravno(),
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  suma);

 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");

fprintf(ff,"\
Загальна сумма до виплати, грн.                                   %10.2f\n",itogo);

fprintf(ff,"\
Сумма коміссії Банку, грн.\n\
Загальна сумма для перерахування в банк, грн.\n\
Дата виплати заробітної плати,\n\n");

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov.ravno(),glavbuh.ravno());

fclose(ff);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}

