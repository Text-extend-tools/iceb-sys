/*$Id:$*/
/*08.09.2016	05.09.2016	Белых А.И.	zarsrsl.c
получить список рабоников для которых применялась социальная льгота
*/
#include "buhg.h"

extern short	mmm,ggg;
extern class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/

int zarsrsl(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
int		kom1;
char		strsql[1024];
short		mr,gr;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Распечатать список работников у которых применялась социальноя льгота"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)."));
 
naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"Enter",gettext("расчитать"),
"F10",gettext("выход"),NULL);

kom1=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1);

switch(kom1)
 {
  case FK1:
   GDITE();
//   iceb_t_pdoc("");
   goto naz;
  
  case FK10:
  case ESC:
   return(1);

  case FK2:
  case ENTER:
   break;

  default:
    goto naz;   
 }

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

GDITE();
class SQLCURSOR cur,cur1;
SQL_str row,row1;

int kolstr=0;


sprintf(strsql,"select tabn,lgot from Zarn where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

class zar_read_tn1h nastr;
zar_read_tn1(1,mr,gr,&nastr);

class iceb_fopen fil;
char imaf[64];
sprintf(imaf,"srsl%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Список работников для которых применялась социальная льгота"),0,mr,gr,0,0,0,fil.ff);
fprintf(fil.ff,"%s %d.%d %s\n",gettext("Дата"),mr,gr,gettext("г."));
fprintf(fil.ff,"\
------------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
Таб.н.|                              |Код льготы|Сумма начиcленная|\n"));
/**
123456 123456789012345678901234567890 1234567890 12345678901234567
***/
fprintf(fil.ff,"\
------------------------------------------------------------------\n");




class iceb_fopen fils;
char imafs[64];
sprintf(imafs,"srsls%d.lst",getpid());
if(fils.start(imafs,"w") != 0)
 return(1);


iceb_t_zagolov(gettext("Список работников с превышением максимальной суммы для расчёта единого соц. отчисления"),0,0,0,0,mr,gr,fils.ff);
fprintf(fils.ff,"%s %d.%d %s\n",gettext("Дата"),mr,gr,gettext("г."));

fprintf(fils.ff,"%s:%.2f\n",gettext("Максимальная сумма"),nastr.max_sum_for_soc);
fprintf(fils.ff,"\
----------------------------------------------------------------------\n");
fprintf(fils.ff,gettext("\
Таб.н.|          Фамилия,имя,отчество          |Начислено |Превышение|\n"));
fprintf(fils.ff,"\
----------------------------------------------------------------------\n");






double suma=0.;
int kolstr2=0;
int kolstr1=0;
class iceb_tu_str fio("");
double itogo_lg=0.;
double suma_for_esv=0.;
double itogo_esv[2];
memset(itogo_esv,'\0',sizeof(itogo_esv));

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"select knah,suma from Zarp where tabn=%s and datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",row[0],gr,mr,gr,mr);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(kolstr2 == 0)
   {
    continue;
   }
  suma=0.;  
  suma_for_esv=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    suma+=atof(row1[1]);
    if(proverka(knvr_esv_r.ravno(),row1[0],0,1) != 0)
     {
      suma_for_esv+=atof(row1[1]);
     }
   }

  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);

  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  else
   fio.new_plus("");

  if(row[1][0] != '\0' && suma != 0.)
   {     
    fprintf(fil.ff,"%6s %-*.*s %10s %17.2f\n",
    row[0],
    iceb_tu_kolbait(30,fio.ravno()),
    iceb_tu_kolbait(30,fio.ravno()),
    fio.ravno(),
    row[1],
    suma);

    itogo_lg+=suma;

   }

  if(suma_for_esv > nastr.max_sum_for_soc)
   {
    itogo_esv[0]+=suma_for_esv;
    itogo_esv[1]+=suma_for_esv-nastr.max_sum_for_soc;

    fprintf(fils.ff,"%6s %-*.*s %10.2f %10.2f\n",
    row[0],
    iceb_tu_kolbait(40,fio.ravno()),
    iceb_tu_kolbait(40,fio.ravno()),
    fio.ravno(),
    suma_for_esv,
    suma_for_esv-nastr.max_sum_for_soc);

   }

 }

fprintf(fil.ff,"\
------------------------------------------------------------------\n");
fprintf(fil.ff,"%*s %17.2f\n",
iceb_tu_kolbait(48,gettext("Итого")),
gettext("Итого"),
itogo_lg);

fprintf(fils.ff,"\
----------------------------------------------------------------------\n");
fprintf(fils.ff,"%*s %10.2f %10.2f\n",
iceb_tu_kolbait(47,gettext("Итого")),
gettext("Итого"),
itogo_esv[0],
itogo_esv[1]);


podpis(fil.ff);
fil.end();
podpis(fils.ff);
fils.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список работников для которых применялась социальная льгота"));

oth->spis_imaf.plus(imafs);
oth->spis_naim.plus(gettext("Список работников с превышением максимальной суммы для расчёта ЕСВ"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}
