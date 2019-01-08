/*$Id: zar_sdpf.c,v 5.17 2013/09/26 09:43:45 sasa Exp $*/
/*22.03.2018	01.11.2005	Белых А.И.	zar_sdpf.c
Справка для пенсионного фонда Украины
*/
#include <errno.h>
#include "buhg.h"

void  zar_sdpf_ps(short mr,short gr,class masiv_din_int *sppi,int *kolih_inv);


int zar_sdpf(class spis_oth *oth)
{
char strsql[512];
int kom1=0;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

static class iceb_tu_str dat1("");

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mt,gt);
  dat1.new_plus(strsql);
 }


VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Распечатать отчёт по пенсионерам и инвалидам"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите месяц и год (м.г)"));

naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F10",gettext("выход"),NULL);

if((kom1=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return(1);
//if(kom1 == ESC)
//  return(1);

switch(kom1)
 {
  case ESC:
  case FK10:
   return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_7.txt");
   goto naz;
 
  case ENTER:
  case FK2:
  case PLU:
    break;
  default:
     goto naz;
 }

time_t tmmn=time(NULL);

short mr,gr;
if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
  goto naz;
clear();
GDITE();

FILE *ff;
char imaf[64];
sprintf(imaf,"sdpf%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
int kolih_inv=0;
class masiv_din_int sppi;
zar_sdpf_ps(mr,gr,&sppi,&kolih_inv);

int kolih=sppi.kolih();

fprintf(ff,"\
                               Список працюючих пенсионерів та інвалідів по\n\
                               %s\n\
                                        Станом на %d.%d.%d\n",iceb_t_get_pnk("00",0),dt,mt,gt);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |  Прізвище, ім'я по батькові  |     Домашня адреса           |   ІПН    |Дата наро.|Дата прий.|З/п за %02d.%d\n",
 mr,gr);
/*
1234 123456789012345678901234567890 123456789012345678901234567890 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
int nomer=0;
class iceb_tu_str fio("");
double suma=0.;
short drg,mrg,grg;
short dp,mp,gp;
class iceb_tu_str inn("");
SQL_str row;
SQLCURSOR cur;
class iceb_tu_str adres("");
int kolstr=0;
double itogo=0.;
int kolstr1=0;
double itogo_inv_pen=0;
for(int ii=0; ii < kolih; ii++)
 {
  strzag(LINES-1,0,kolih,++kolstr1);
  if(ii == 0 && kolih_inv > 0)
   fprintf(ff,"%s\n",gettext("Инвалиды"));

  if(ii == kolih_inv)
   {
    fprintf(ff,"%*s:%10.2f\n",
    iceb_tu_kolbait(99,gettext("Итого по инвалидам")),gettext("Итого по инвалидам"),itogo_inv_pen);
    fprintf(ff,"%s\n",gettext("Пенсионеры"));
    itogo_inv_pen=0;
   }  
  sprintf(strsql,"select fio,datn,inn,adres,denrog from Kartb where tabn=%d",sppi.ravno(ii));
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),sppi.ravno(ii));
    iceb_t_soob(strsql);
    continue;
   }  

  fio.new_plus(row[0]);
  rsdat(&dp,&mp,&gp,row[1],2);
  inn.new_plus(row[2]);
  adres.new_plus(row[3]);
  rsdat(&drg,&mrg,&grg,row[4],2);
  //Узнаём сумму начисленную
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d and prn='1'",
  gr,mr,1,gr,mr,31,sppi.ravno(ii));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  suma=0.;  
  while(cur.read_cursor(&row) != 0)
   {
    suma+=atof(row[0]);    
   }
  printw("%-5d %-*.*s %8.2f\n",sppi.ravno(ii),
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  suma);

  if(suma == 0.)
   continue;
  itogo+=suma;
  itogo_inv_pen+=suma;  
  fprintf(ff,"%-4d %-*.*s %-*.*s %-10s %02d.%02d.%04d %02d.%02d.%04d %10.2f     %d\n",
  ++nomer,
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  iceb_tu_kolbait(30,adres.ravno()),iceb_tu_kolbait(30,adres.ravno()),adres.ravno(),
  inn.ravno(),drg,mrg,grg,dp,mp,gp,suma,
  sppi.ravno(ii));
  
  if(iceb_tu_strlen(adres.ravno()) > 30)
    fprintf(ff,"%4s %30s %s\n","","",iceb_tu_adrsimv(30,adres.ravno()));
  
 }

fprintf(ff,"%*s:%10.2f\n",
 iceb_tu_kolbait(99,gettext("Итого по пенсионерам")),gettext("Итого по пенсионерам"),itogo_inv_pen);


class iceb_tu_str bros("");

iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");

class iceb_tu_str fioruk("");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw("Не знайдено табельного номера %s керівника !\n",bros.ravno());
    OSTANOV();
   }
  else
   {
    fioruk.new_plus(row[0]);
   }
 } 

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");
class iceb_tu_str fiobuh("");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw("Не знайдено табельного номера %s головного бухгалтера!\n",bros.ravno());
    OSTANOV();
   }
  else
   {
    fiobuh.new_plus(row[0]);

   }
 }


fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(99,gettext("Итого")),gettext("Итого"),itogo);
fprintf(ff,"\n\
                      %*s__________________%s\n\n",iceb_tu_kolbait(30,gettext("Руководитель")),gettext("Руководитель"),fioruk.ravno());
fprintf(ff,"\
                      %*s__________________%s\n\n",iceb_tu_kolbait(30,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fiobuh.ravno());
                         
podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Отчёт по пенсионерам и инвалидам"));

iceb_t_ustpeh(imaf,3);

printw_vr(tmmn);
OSTANOV();

return(0);
}
/*******************************/
/*получение списка пенсионеров и инвалидов*/
/*******************************************/

void  zar_sdpf_ps(short mr,short gr,class masiv_din_int *sppi,int *kolih_inv)
{
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

int metka_s=0;
for(int ii=0; ii < 2; ii++)
 {
  if(ii == 0)
   metka_s=2;
//    imaf.new_plus("zarinv.alx");
  if(ii == 1)
   metka_s=1;
//    imaf.new_plus("zarpens.alx");
/**************  
  sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
**************/
  sprintf(strsql,"select tn,dnd,dkd,kom from Zargr where kg=%d order by dnd <= %04d-%02d-01",metka_s,gr,mr);
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }

  if(kolstr == 0)
   {
    return;
   }

  
  
  
  int tabn;

  while(cur_alx.read_cursor(&row_alx) != 0)
   {

    tabn=atoi(row_alx[0]);
    if(row_alx[2][0] != '0')
     {
      short d=0,m=0,g=0;
      rsdat(&d,&m,&g,row_alx[2],2);
      if(sravmydat(1,mr,gr,1,m,g) > 0)
       continue;
     }
    sppi->plus(tabn,-1);

    if(ii == 0)
     *kolih_inv+=1;         
   }
 }



return;
}
