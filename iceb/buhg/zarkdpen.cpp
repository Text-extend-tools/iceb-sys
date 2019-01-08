/*$Id: zarkdpen.c,v 5.11 2013/08/13 05:50:02 sasa Exp $*/
/*12.04.2011	14.01.2010	Белых А.И.	zarkdpen.c
Расчёт количества календарных дней для пенсии
*/
#include <errno.h>
#include        "buhg.h"


int zarkdpen(class spis_oth *oth)
{
int kodpen=0;  /*Код пенсионных отчислений*/

char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str tabnom("");
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),112);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт календарных дней для пенсионного стажа"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Табельний номер.(,,)."));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt"));
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK3:
//        VV.data_plus(3,kod_subbal.ravno());
    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short mn,gn;
short mk,gk;

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата начала!"));
  
  goto naz;
 }
if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата конца!"));
  goto naz;
 }
iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int kolstr1=0;
char imaf[64];
FILE *ff;
short dr,mr,gr;
class iceb_tu_str fio("");
double pensmz=0.; /*Пенсионное отчисление с минимальной зарплаты*/
double pensrel=0.; /*Реально начисленное пенсионноге отчисление*/
int kolkd=0; /*количесво календарных дней в стаж*/
int kolstr2=0;

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
suma <> 0.",gn,mn,gk,mk);

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

sprintf(imaf,"kdpen%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
short dkm=0;
dpm(&dkm,&mk,&gk,5);

iceb_t_zagolov(gettext("Расчёт календарных дней для пенсионного стажа"),1,mn,gn,dkm,mk,gk,ff);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Табельный| Фамилия, имя, отчество       |  Дата |Минима- |Пенсионное|Пенсионное|Количество|Количество|\n\
 номер   |                              |       | льная  |отчисление|отчислеие |календар- |календар- |\n\
         |                              |       |зарплата|с минимал.| реальное |ных дней в|ных дней  |\n\
         |                              |       |        |зарплаты  |          |месяце    |в зачёт   |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
/*
123456789                                        12345678 1234567890 1234567890 1234567890 1234567890
*/

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;

  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  dr=1;
  mr=mn;
  gr=gn;
  while(sravmydat(1,mr,gr,1,mk,gk) <= 0)
   {
    class zar_read_tn1h nastr;
    zar_read_tn1(1,mr,gr,&nastr);

    /*Читаем пенсионное отчисление реально начисленное*/
    sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%s and prn='2' and knah=%d and suma <> 0.",
    gr,mr,gr,mr,row[0],kodpen);
    pensrel=0.;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     pensrel+=atof(row1[0]);    
    pensrel*=-1; /*делаем положительным*/
        
    /*чтение величины минимальной зарплаты и прожиточного минимума в расчётном месяце*/
//    zar_read_tn(dr,mr,gr,NULL); 
    dpm(&dkm,&mr,&gr,5); /*Получить последний день месяца*/
    /*Расчитываем пенсионное отчисление с минимальной зарплаты*/
//    pensmz=pens(atoi(row[0]),mr,gr,nastr.minzar,NULL);
    
    if(pensmz <= pensrel)
     kolkd=dkm;
    else
     {
      if(pensmz != 0.)
        kolkd=(int)(pensrel*dkm/pensmz);
      else
        kolkd=0;
     }
    fprintf(ff,"%9s %-*.*s %02d.%04d %8.2f %10.2f %10.2f %10d %10d\n",
    row[0],
    iceb_tu_kolbait(30,fio.ravno()),
    iceb_tu_kolbait(30,fio.ravno()),
    fio.ravno(),
    mr,gr,nastr.minzar,pensmz,pensrel,dkm,kolkd);
            
    dpm(&dr,&mr,&gr,3); /*Увеличиваем на месяц дату*/
   }  
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
  
 }

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт календарных дней для пенсионного стажа"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);









return(0);
}
