/*$Id: uosrs.c,v 5.30 2013/09/26 09:43:42 sasa Exp $*/
/*13.06.2013	24.10.2003	Белых А.И.	uosrs.c
Распределение заданной оператором суммы пропорционально балансовой стоимоти основных средств
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

int uosrs1(short dr,short mr, short gr,short dd,short md,short gd,const char *grupa,int metkabr,double sumar,const char *inv_nomer,const char *kodop,float dpnds,class iceb_tu_str *shetuh,class spis_oth *oth);
int uosrsgr(short dr,short mr, short gr,short dd,short md,short gd,const char *grupa,int metkabr,double sumar,const char *inv_nomer,const char *kodop,float dpnds,class iceb_tu_str *shetuh,class spis_oth *oth);
int uosrs_zap(short dd,short md,short gd,const char *kodop,int kod_mo,int podr,int innom,double bs,class SPISOK *kod_podr_mo,class SPISOK *snomdok,float);
void uosrs_sap(int metkabr,const char*,FILE *ff);
void uosrs_rek(short dr,short mr, short gr,short dd,short md,short gd,const char *grupa,double sumar,const char *inv_nomer,const char *kodop,const char *shetuh,int metkabr,FILE *ff);



void    uosrs()
{
int     N=0,K=0;
static int    metkabr=0;
static int    metkaor=0;
static class iceb_tu_str dat1("");
static class iceb_tu_str datdok("");
static class iceb_tu_str grupa("");
static class iceb_tu_str suma("");
static class iceb_tu_str inv_nomer("");
static class iceb_tu_str shetuh("");
char strsql[512];
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(suma.ravno(),20);
VV.VVOD_SPISOK_add_data(grupa.ravno(),112);
VV.VVOD_SPISOK_add_data(40);
VV.VVOD_SPISOK_add_data(40);
VV.VVOD_SPISOK_add_data(inv_nomer.ravno(),256);
VV.VVOD_SPISOK_add_data(datdok.ravno(),11);
VV.VVOD_SPISOK_add_data(shetuh.ravno(),112);


VV.VVOD_SPISOK_add_ZAG(gettext("Пропорциональное распределение сумм"));

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта балансовой стоимости..(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Сумма......................................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Группа......................................"));//2
VV.VVOD_SPISOK_add_MD(gettext("База для расчёта............................"));//3
VV.VVOD_SPISOK_add_MD(gettext("Способ расчёта.............................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.....(,,).................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата документов....(д.м.г).................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.........(,,)....................."));//7



naz:;

if(metkabr == 0)
 VV.VVOD_SPISOK_zapis_data(3,gettext("Остаточная балансовая стоимость"));
if(metkabr == 1)
 VV.VVOD_SPISOK_zapis_data(3,gettext("Начальная балансовая стоимость"));

if(metkaor == 0)
 VV.VVOD_SPISOK_zapis_data(4,gettext("Пообъектно"));
if(metkaor == 1)
 VV.VVOD_SPISOK_zapis_data(4,gettext("В целом по группе"));


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));


    if(N == 3) //База для расчёта
     {
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_MD(gettext("Остаточная балансовая стоимость"));
      MENU.VVOD_SPISOK_add_MD(gettext("Начальная балансовая стоимость"));
      while(menu3w(stdscr,&MENU,&metkabr,-1,-1,0) != 0);
      goto naz;
     }

    if(N == 4) //Способ расчёта
     {
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_MD(gettext("Пообъектно"));
      MENU.VVOD_SPISOK_add_MD(gettext("Не пообъектно"));
      while(menu3w(stdscr,&MENU,&metkaor,-1,-1,0) != 0);
      goto naz;
     }

    VV.vprintw_clear(N,stdscr);

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));


    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    return;
   }

  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
    iceb_t_pdoc("uos2_5.txt");
    clear();
    goto naz;
   }

  if(K == FK3) /*реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    goto naz;
   }

  if(K == FK2 || K == PLU ) /*Записываем*/
   {

    break;
   }
 }

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
suma.new_plus(VV.VVOD_SPISOK_return_data(1));
grupa.new_plus(VV.data_ravno(2));
inv_nomer.new_plus(VV.VVOD_SPISOK_return_data(5));
datdok.new_plus(VV.VVOD_SPISOK_return_data(6));
shetuh.new_plus(VV.data_ravno(7));

short dr,mr,gr;
if(rsdat(&dr,&mr,&gr,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

short dd=0,md=0,gd=0;

if(datdok.ravno()[0] != '\0')
  if(rsdat(&dd,&md,&gd,datdok.ravno(),1) != 0)
   {
    iceb_t_soob(gettext("Неправильно введена дата документа!"));
    goto naz;
   }



class iceb_tu_str kod_op("");
float dpnds=0.;

if(gd != 0)
 {

  iceb_t_poldan("Код операции дооценки",&kod_op,"uosnast.alx");
  if(kod_op.getdlinna() <= 1)
   {
    iceb_t_soob(gettext("В файле настройки не введено \"Код операции дооценки\"!"));
    goto naz;
   
   }
  sprintf(strsql,"select kod from Uospri where kod='%s'",kod_op.ravno());
  if(readkey(strsql) != 1)
   {
    sprintf(strsql,gettext("Не найден код операции %s !"),kod_op.ravno());
    iceb_t_soob(strsql);
    goto naz;
    
   }

  dpnds=iceb_t_pnds();
 }


class spis_oth oth;
if(metkaor == 0) /*пообъектно*/
  if(uosrs1(dr,mr,gr,dd,md,gd,grupa.ravno(),metkabr,suma.ravno_atof(),inv_nomer.ravno(),kod_op.ravno(),dpnds,&shetuh,&oth) == 0)
   iceb_t_rabfil(&oth,"");

if(metkaor == 1) /*в целом по группе*/
 if(uosrsgr(dr,mr,gr,dd,md,gd,grupa.ravno(),metkabr,suma.ravno_atof(),inv_nomer.ravno(),kod_op.ravno(),dpnds,&shetuh,&oth) == 0)
   iceb_t_rabfil(&oth,"");

}


/***********************************************/
/*Расчёт пообектный*/
/************************/

int uosrs1(short dr,short mr, short gr,
short dd,short md,short gd,
const char *grupa,
int metkabr,double sumar,const char *inv_nomer,
const char *kodop,
float dpnds,
class iceb_tu_str *shetuh,
class spis_oth *oth)
{
char    strsql[512];
int     kolstr=0,kolstr1=0;
SQL_str row;
FILE    *ff,*fftmp;
char    imaf[64],imaftmp[64];;
SQLCURSOR cur;
int     i=0;
class iceb_tu_str naim("");

sprintf(strsql,"select innom,naim from Uosin");

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

sprintf(imaf,"uosrs%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
uosrs_rek(dr,mr,gr,dd,md,gd,grupa,sumar,inv_nomer,kodop,shetuh->ravno(),metkabr,ff);

sprintf(imaftmp,"uosrs.tmp");
if((fftmp = fopen(imaftmp,"w")) == NULL)
   {
    error_op_nfil(imaftmp,errno,"");
    return(1);
   }

int podr=0;
int kodotl=0;
long innom=0;
class bsizw_data bal_st;
double obs; 
double sumanbs=0.; //Сумма начально балансовой стоимости всех основных средств
double sumaobs=0.; //Сумма остаточной балансовой стоимости всех основных средств
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(inv_nomer,row[0],0,0) != 0)
   continue;
  innom=atol(row[0]);
  if((i=poiin(innom,dr,mr,gr,&podr,&kodotl)) != 0)
   {
    fprintf(ff,"%s %ld %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не числится по дате"),mr,gr,
    gettext("Код возврата"),i);
    continue;
   }   

//  if((i=poiinpd(innom,mr,gr,uos.shetu,uos.hzt,uos.hau,uos.hna,&uos.popkf,&uos.soso,uos.nomz,&m1,&g1,uos.hnaby,&uos.popkfby)) != 0)
  if((i=poiinpd(innom,mr,gr,&rekin)) != 0)
   {
    fprintf(ff,"%s %ld %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Код возврата"),i);

    continue;

   }  

  if(proverka(shetuh->ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  if(proverka(grupa,rekin.hna.ravno(),0,0) != 0)
   {
    fprintf(ff,"%ld %s %s %s.\n",innom,
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    "Пропускаем");
    continue;
   }

  bsiz(innom,podr,dr,mr,gr,&bal_st,NULL);

  fprintf(ff,"\n\
Инвентарный номер %ld %s\n",innom,row[1]);

  fprintf(ff,"\
Стартовая балансовая стоимость %.2f\n",bal_st.sbs);

  fprintf(ff,"\
Добавленная балансовая стоимость %.2f\n",bal_st.bs);

  fprintf(ff,"\
Стартовый износ %.2f\n",bal_st.siz);
  fprintf(ff,"\
Добавленный износ %.2f\n",bal_st.iz);
  fprintf(ff,"\
Амортизация %.2f\n",bal_st.iz1);
  obs=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
  sumaobs+=obs;
  fprintf(ff,"Остаточная балансовая стоимость = %.2f *********\n",obs);
/***********
  if(metkabr == 0) //Расчёт по остаточной бал.стоимоти
   if(obs < 0.009)
     continue;  
*****************/
  sumanbs+=bal_st.sbs+bal_st.bs;

  fprintf(fftmp,"%ld|%d|%s|%d|\n",innom,podr,row[1],kodotl);
 }

podpis(ff);
fclose(ff);
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }

unlink(imaftmp);

char imaftb[64];
sprintf(imaftb,"uosrstb%d.lst",getpid());
if((ff = fopen(imaftb,"w")) == NULL)
 {
  error_op_nfil(imaftb,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
uosrs_rek(dr,mr,gr,dd,md,gd,grupa,sumar,inv_nomer,kodop,shetuh->ravno(),metkabr,ff);


if(metkabr == 1)
 fprintf(ff,"\nОбщая начальная балансовая стоимость=%.2f\n",sumanbs);
if(metkabr == 0)
 fprintf(ff,"\nОбщая остаточная балансовая стоимость=%.2f\n",sumaobs);

fprintf(ff,"Сумма для распределения=%.2f\n",sumar);
double kofrnbs=0.;
if(sumanbs != 0.)
  kofrnbs=sumar/sumanbs;

if(metkabr == 1)
 fprintf(ff,"Коэффициент распределения для начальной балансовой стоимости %.2f/%.2f=%f\n",sumar,sumanbs,kofrnbs);

double kofrobs=0.;
if(sumaobs != 0.)
  kofrobs=sumar/sumaobs;

if(metkabr == 0)
 fprintf(ff,"Коэффициент распределения для остаточной балансовой стоимости %.2f/%.2f=%f\n",sumar,sumaobs,kofrobs);

uosrs_sap(metkabr,inv_nomer,ff);

double itog1=0.,itog2=0.,itog3=0.,itog4=0.;
double ras1,ras2;
class SPISOK kod_podr_mo;
class SPISOK snomdok;

while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
 {

  innom=atoi(strsql);
  polen(strsql,&podr,2,'|');
  polen(strsql,&naim,3,'|');
  polen(strsql,&kodotl,4,'|');

  bsiz(innom,podr,dr,mr,gr,&bal_st,NULL);

//  fprintf(ff,"Для начальной стоимости %f*%f=%f\n",sbs,kofrnbs,sbs*kofrnbs);

  obs=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
//  fprintf(ff,"Для остаточной балансовой стоимости %f*%f=%f\n",obs,kofrobs,obs*kofrobs);

  ras1=(bal_st.sbs+bal_st.bs)*kofrnbs;
  ras1=okrug(ras1,0.01);

  ras2=obs*kofrobs;
  ras2=okrug(ras2,0.01);

  if(metkabr == 0)
   fprintf(ff,"%11ld %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_tu_kolbait(33,naim.ravno()),iceb_tu_kolbait(33,naim.ravno()),naim.ravno(),
   obs,ras2);

  if(metkabr == 1)
   fprintf(ff,"%11ld %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_tu_kolbait(33,naim.ravno()),iceb_tu_kolbait(33,naim.ravno()),naim.ravno(),
   bal_st.sbs+bal_st.bs,ras1);

  if(gd != 0)
   {
    double bsvr=0.;
    if(metkabr == 0)
      bsvr=ras2;
    if(metkabr == 1)
      bsvr=ras1;
    if(bsvr != 0.)
     uosrs_zap(dd,md,gd,kodop,kodotl,podr,innom,bsvr,&kod_podr_mo,&snomdok,dpnds);
   }
  itog1+=bal_st.sbs+bal_st.bs;
  itog2+=ras1;
  itog3+=obs;
  itog4+=ras2;
 }

fprintf(ff,"\
--------------------------------------------------------------------\n");
if(metkabr == 0)
 fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itog3,itog4);
if(metkabr == 1)
 fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itog1,itog2);


podpis(ff);
fclose(ff);
fclose(fftmp);

if(snomdok.kolih() > 0)
 {
  sprintf(strsql,"%s:%d",gettext("Количество документов"),snomdok.kolih());
  iceb_t_soob(strsql);
  
 }


oth->spis_imaf.plus(imaftb);
oth->spis_naim.plus(gettext("Пропорциональное распределение сумм"));

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);

}

/***********************************************/
/*Расчёт по группе*/
/************************/
int uosrsgr(short dr,short mr, short gr,
short dd,short md,short gd,
const char *grupa,int metkabr,double sumar,const char *inv_nomer,
const char *kodop,
float dpnds,
class iceb_tu_str *shetuh,
class spis_oth *oth)
{
char strsql[512];
class iceb_tu_str bros("");
class iceb_tu_str bros1("");
FILE *ff;
char imaf[64];
int  kolstr=0;
class bsizw_data bal_st;
int i=0;
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select innom,naim from Uosin");
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


//sprintf(bros1,"Стартовая балансовая стоимость для группы %s",grupa);
bros1.new_plus("Стартовая балансовая стоимость для группы ");
bros1.plus(grupa);
if(iceb_t_poldan(bros1.ravno(),&bros,"uosnast.alx") != 0)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена стартовая балансовая стоимость для группы"),grupa);
  iceb_t_soob(strsql);
  return(1);
 }

bal_st.sbs=bros.ravno_atof();

//sprintf(bros1,"Стартовый износ для группы %s",grupa);
bros1.new_plus("Стартовый износ для группы ");
bros1.plus(grupa);

iceb_t_poldan(bros1.ravno(),&bal_st.siz,"uosnast.alx");
//bal_st.siz=ATOFM(bros);

bsiz23(grupa,dr,mr,gr,&bal_st.bs,&bal_st.iz,&bal_st.iz1,NULL);

sprintf(imaf,"uosrsg%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

char imaf_prot[64];
sprintf(imaf_prot,"uosrsgp%d.lst",getpid());
FILE *ffprot;
if((ffprot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }


fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

uosrs_rek(dr,mr,gr,dd,md,gd,grupa,sumar,inv_nomer,kodop,shetuh->ravno(),metkabr,ff);

fprintf(ff,"Группа %s Стартовая б.c. %.2f  Стартовый износ %.2f\n\
Бал. ст. %.2f Износ %.2f \n\
Амортизация %.2f\n",grupa,bal_st.sbs,bal_st.siz,bal_st.bs,bal_st.iz,bal_st.iz1);


fprintf(ffprot,"%s\n",iceb_t_get_pnk("00",0));
uosrs_rek(dr,mr,gr,dd,md,gd,grupa,sumar,inv_nomer,kodop,shetuh->ravno(),metkabr,ffprot);

fprintf(ffprot,"Группа %s Стартовая б.c. %.2f  Стартовый износ %.2f\n\
Бал. ст. %.2f Износ %.2f \n\
Амортизация %.2f\n",grupa,bal_st.sbs,bal_st.siz,bal_st.bs,bal_st.iz,bal_st.iz1);

 
int podr=0;
long innom;
int kolstr1=0;
int kodotl=0;
double obs=0.;

double kofrnbs=0.;
double kofrobs=0.;

if(bal_st.sbs != 0.)
 kofrnbs=sumar/bal_st.sbs;
double ost_st=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
if(ost_st != 0.)
 kofrobs=sumar/ost_st;

fprintf(ff,"Сумма для распределения=%.2f\n",sumar);

fprintf(ff,"Коэффициент распределения для начальной балансовой стоимости %.2f/%.2f=%f\n",sumar,bal_st.sbs,kofrnbs);
fprintf(ff,"Коэффициент распределения для остаточной балансовой стоимости %.2f/%.2f=%f\n",sumar,ost_st,kofrobs);

uosrs_sap(metkabr,inv_nomer,ff);
  
double itog1=0.,itog2=0.,itog3=0.,itog4=0.;
double ras1,ras2;
class SPISOK kod_podr_mo;
class SPISOK snomdok;
class poiinpdw_data rekin;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(inv_nomer,row[0],0,0) != 0)
   continue;
  innom=atol(row[0]);
  if((i=poiin(innom,(short)31,mr,gr,&podr,&kodotl)) != 0)
   {

    fprintf(ffprot,"%s %ld %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не числится по дате"),mr,gr,
    gettext("Код возврата"),i);
    continue;
   }   

  if((i=poiinpd(innom,mr,gr,&rekin)) != 0)
   {
    fprintf(ffprot,"%s %ld %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Код возврата"),i);

    continue;

   }  
  if(proverka(shetuh->ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;
  if(SRAV(rekin.hna.ravno(),grupa,0) != 0 )
   {
    
    fprintf(ffprot,"%ld %s %s %s.\n",innom,
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    "Пропускаем");
    
    continue;
   }

  bsiz(innom,podr,dr,mr,gr,&bal_st,NULL);


  ras1=bal_st.sbs*kofrnbs;
  ras1=okrug(ras1,0.01);

  obs=bal_st.sbs+bal_st.bs-bal_st.siz-bal_st.iz-bal_st.iz1;
  ras2=obs*kofrobs;
  ras2=okrug(ras2,0.01);

  if(metkabr == 0)
   fprintf(ff,"%11ld %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_tu_kolbait(33,row[1]),iceb_tu_kolbait(33,row[1]),row[1],
   obs,ras2);

  if(metkabr == 1)
   fprintf(ff,"%11ld %-*.*s %10.2f %10.2f\n",
   innom,
   iceb_tu_kolbait(33,row[1]),iceb_tu_kolbait(33,row[1]),row[1],
   bal_st.sbs,ras1);

  if(gd != 0)
   {
    double bs=0.;
    if(metkabr == 0)
      bs=ras2;
    if(metkabr == 1)
      bs=ras1;
    if(bs != 0.)
     uosrs_zap(dd,md,gd,kodop,kodotl,podr,innom,bs,&kod_podr_mo,&snomdok,dpnds);
   }

  itog1+=bal_st.sbs;
  itog2+=ras1;
  itog3+=obs;
  itog4+=ras2;

 }

fprintf(ff,"\
--------------------------------------------------------------------\n");
if(metkabr == 0)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itog3,itog4);
if(metkabr == 1)
  fprintf(ff,"%*s %10.2f %10.2f\n",iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itog1,itog2);

podpis(ff);
fclose(ff);
podpis(ffprot);
fclose(ffprot);

if(snomdok.kolih() > 0)
 {
  sprintf(strsql,"%s:%d",gettext("Количество документов"),snomdok.kolih());
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  
 }


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Пропорциональное распределение сумм"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));
for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
 
return(0);
}

/**************************/
/*Запись шапки документа (если нужно) и запись в документ*/
/************************/
/*Если вернули 0-всё записано*/
int uosrs_zap(short dd,short md,short gd,
const char *kodop,
int kod_mo,
int podr,
int innom,
double bs,
class SPISOK *kod_podr_mo, /*код материально-ответственного|код подразделения*/
class SPISOK *snomdok,   /*список номеров документов*/
float dpnds) /*Процент НДС действовавший на момент создания документа*/
{
char strsql[1024];
class iceb_tu_str nom_dok(""); /*Номер документа в который нужно записывать инвентарный номер*/

int nomer_par=0;
sprintf(strsql,"%d|%d",kod_mo,podr);
if((nomer_par=kod_podr_mo->find(strsql)) >= 0)
 {
  nom_dok.new_plus(snomdok->ravno(nomer_par));
 }
else
 {
  
  kod_podr_mo->plus(strsql);

  class lock_tables kkk("LOCK TABLE Uosdok WRITE,icebuser READ");

  uosnd(gd,1,&nom_dok);

  snomdok->plus(nom_dok.ravno());

  sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,podt,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%d,%d,%d,%d,%d,%ld,%.2f)",
  gd,md,dd,1,kodop,"00",nom_dok.ravno(),podr,kod_mo,0,0,iceb_t_getuid(),time(NULL),dpnds);

  if(sql_zapis(strsql,1,0) != 0)
   return(1);
 }
 
sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,kodop,ktoz,vrem) \
values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,'%s',%d,%ld)",
gd,md,dd,1,0,innom,nom_dok.ravno(),podr,kod_mo,0,bs,kodop,iceb_t_getuid(),time(NULL));

return(sql_zapis(strsql,1,0));
}


/**************************************/
/*Шапка распечатки*/
/*********************************/
void uosrs_sap(int metkabr,const char *inv_nomer,FILE *ff)
{

if(inv_nomer[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),inv_nomer);

if(metkabr == 0)
  fprintf(ff,"\
--------------------------------------------------------------------\n\
Инвентарный| Наименование основного средства |Ост. бал. |Распреде- |\n\
  номер    |                                 |стоимость | лено     |\n\
--------------------------------------------------------------------\n");
if(metkabr == 1)
  fprintf(ff,"\
--------------------------------------------------------------------\n\
Инвентарный| Наименование основного средства |Нач. бал. |Распреде- |\n\
  номер    |                                 |стоимость |  лено    |\n\
--------------------------------------------------------------------\n");

}
/********************/
/*Реквизиты*/
/******************/
void uosrs_rek(short dr,short mr, short gr,
short dd,short md,short gd,
const char *grupa,
double sumar,const char *inv_nomer,
const char *kodop,
const char *shetuh,
int metkabr,
FILE *ff)
{
if(metkabr == 0)
 fprintf(ff,"%s\n",gettext("Расчёт с использованием остаточной балансовой стоимости"));
if(metkabr == 1)
 fprintf(ff,"%s\n",gettext("Расчёт с использованием начальной балансовой стоимости"));

fprintf(ff,"%s:%d.%d.%d\n",gettext("Дата расчёта балансовой стоимости"),dr,mr,gr);
if(dd != 0)
 fprintf(ff,"%s:%d.%d.%d\n",gettext("Дата создания документов"),dd,md,gd);
if(grupa[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Группа"),grupa);
fprintf(ff,"%s:%.2f\n",gettext("Сумма для распределения"),sumar);
if(inv_nomer[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),inv_nomer);
fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop); 
if(shetuh[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetuh);

}

