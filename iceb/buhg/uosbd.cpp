/*$Id: uosbd.c,v 5.11 2013/09/26 09:43:41 sasa Exp $*/
/*16.07.2015	06.05.2010	Белых А.И.	uosbd.c
Дооценка балансовой стоимости и амортизации для бюджетных организаций Украины
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

int uosbd_zap(short dd,short md,short gd,const char *kodop,int kod_mo,int podr,int innom,double bs,double iz,
class SPISOK *kod_podr_mo,class SPISOK *snomdok,float dpnds);

int uosbd1(short dr,short mr, short gr,short dd,short md,short gd,const char *grupa,double koef_bs,
double koef_iz,const char *inv_nomer,const char *kodop,float dpnds,const char *shetuh,const char *kod_mo,class spis_oth *oth);


void    uosbd()
{
int     N=0,K=0;
static class iceb_tu_str dat1("");
static class iceb_tu_str datdok("");
static class iceb_tu_str grupa("");
static class iceb_tu_str koef_bs("");
static class iceb_tu_str koef_iz("");
static class iceb_tu_str inv_nomer("");
class iceb_tu_str kod_mo("");
class iceb_tu_str shetuh("");
char strsql[512];
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(koef_bs.ravno(),20);
VV.VVOD_SPISOK_add_data(koef_iz.ravno(),20);
VV.VVOD_SPISOK_add_data(grupa.ravno(),112);
VV.VVOD_SPISOK_add_data(inv_nomer.ravno(),256);
VV.VVOD_SPISOK_add_data(datdok.ravno(),11);
VV.VVOD_SPISOK_add_data(shetuh.ravno(),112);
VV.VVOD_SPISOK_add_data(kod_mo.ravno(),112);


VV.VVOD_SPISOK_add_ZAG(gettext("Дооценка основных средств для бюджетных организаций"));

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта балансовой стоимости..(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Коэффициент для балансовой стоимости........"));//1
VV.VVOD_SPISOK_add_MD(gettext("Коэффициент для износа......................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Группа......................................"));//3
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.....(,,).................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата документов....(д.м.г).................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.........(,,)....................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код материально-ответственного...(,,)......."));//7



naz:;



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


/**************
    if(N == 5) //Способ расчёта
     {
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_MD(gettext("Пообъектно"));
      MENU.VVOD_SPISOK_add_MD(gettext("Не пообъектно"));
      while(menu3w(stdscr,&MENU,&metkaor,-1,-1,0) != 0);
      goto naz;
     }
*************/
    VV.vprintw_clear(N,stdscr);

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

  if(K == FK2 || K == PLU ) /*Записываем*/
   {

    break;
   }
 }

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
koef_bs.new_plus(VV.VVOD_SPISOK_return_data(1));
koef_iz.new_plus(VV.VVOD_SPISOK_return_data(2));
grupa.new_plus(VV.data_ravno(3));
inv_nomer.new_plus(VV.VVOD_SPISOK_return_data(4));
datdok.new_plus(VV.VVOD_SPISOK_return_data(5));
shetuh.new_plus(VV.data_ravno(6));
kod_mo.new_plus(VV.data_ravno(7));

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



class iceb_tu_str kodop("");
float dpnds=0.;

if(gd != 0)
 {

  iceb_t_poldan("Код операции дооценки",&kodop,"uosnast.alx");

  if(kodop.ravno()[0] == '\0')
   {
    iceb_t_soob(gettext("В файле настройки не введено \"Код операции дооценки\"!"));
    goto naz;
   
   }
  sprintf(strsql,"select kod from Uospri where kod='%s'",kodop.ravno());
  if(readkey(strsql) != 1)
   {
    sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
    iceb_t_soob(strsql);
    goto naz;
    
   }
/*************
  if(iceb_t_poldan("Н.Д.С.",strsql,"matnast.alx") != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("В файле настройки matnast.alx не найдено \"Н.Д.С.\""));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
  dpnds=ATOFM(strsql);
****************/
  dpnds=iceb_t_pnds();
 }

class spis_oth oth;
if(uosbd1(dr,mr,gr,dd,md,gd,grupa.ravno(),koef_bs.ravno_atoi(),koef_iz.ravno_atof(),inv_nomer.ravno(),kodop.ravno(),dpnds,shetuh.ravno(),kod_mo.ravno(),&oth) == 0)
   iceb_t_rabfil(&oth,"");

}
/***********************************************/
/*Расчёт пообектный*/
/************************/

int uosbd1(short dr,short mr, short gr,
short dd,short md,short gd,
const char *grupa,
double koef_bs,
double koef_iz,
const char *inv_nomer,
const char *kodop,
float dpnds,
const char *shetuh,
const char *kod_mo,
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

sprintf(imaftmp,"uosrs.tmp");
if((fftmp = fopen(imaftmp,"w")) == NULL)
   {
    error_op_nfil(imaftmp,errno,"");
    return(1);
   }

int podr=0;
int kodotl=0;
int innom=0;
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
  innom=atoi(row[0]);
  if((i=poiin(innom,dr,mr,gr,&podr,&kodotl)) != 0)
   {
    fprintf(ff,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не числится по дате"),mr,gr,
    gettext("Код возврата"),i);
    continue;
   }   

  if((i=poiinpd(innom,mr,gr,&rekin)) != 0)
   {
    fprintf(ff,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),innom,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Код возврата"),i);

    continue;

   }  

  if(proverka(kod_mo,kodotl,0,0) != 0)
   continue;
  if(proverka(shetuh,rekin.shetu.ravno(),0,0) != 0)
   continue;

  if(proverka(grupa,rekin.hna.ravno(),0,0) != 0)
   {
    fprintf(ff,"%d %s %s %s.\n",innom,
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    "Пропускаем");
    continue;
   }

  bsiz(innom,podr,dr,mr,gr,&bal_st,NULL);

  fprintf(ff,"\n\
Инвентарный номер %d %s\n",innom,row[1]);

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
  sumanbs+=bal_st.sbs+bal_st.bs;

  fprintf(fftmp,"%d|%d|%s|%d|\n",innom,podr,row[1],kodotl);
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

char imaftb[50];
sprintf(imaftb,"uosrstb%d.lst",getpid());
if((ff = fopen(imaftb,"w")) == NULL)
 {
  error_op_nfil(imaftb,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

fprintf(ff,"Дата расчёта балансовой стоимости:%d.%d.%d\n",dr,mr,gr);
fprintf(ff,"%s:%s\n",gettext("Группа"),grupa);


fprintf(ff,"Коэффициент для балансовой стоимости=%f\n",koef_bs);
fprintf(ff,"Коэффициент для износа=%f\n",koef_iz);
if(kod_mo[0] != '\0')
 fprintf(ff,"Код материально-ответственного:%s\n",kod_mo);
if(kod_mo[0] != '\0')
 fprintf(ff,"Код материально-ответственного:%s\n",kod_mo);
if(shetuh[0] != 0)
 fprintf(ff,"Счёт учёта:%s\n",shetuh);
if(grupa[0] != 0)
 fprintf(ff,"Группа:%s\n",grupa);
if(inv_nomer[0] != 0)
 fprintf(ff,"Инвентарный номер:%s\n",inv_nomer);
   
fprintf(ff,"\
------------------------------------------------------------------------------------------\n\
Инвентарный| Наименование основного средства |Балансовая стоимость |         Износ       |\n\
  номер    |                                 |                     |                     |\n\
------------------------------------------------------------------------------------------\n");

double itog1=0.,itog2=0.,itog3=0.,itog4=0.;
double ras1,ras2;
class SPISOK kod_podr_mo;
class SPISOK snomdok;
double balst=0.;
double iznos=0.;
double balst_r=0.;
double iznos_r=0.;
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
  balst=(bal_st.sbs+bal_st.bs);
  ras1=balst*koef_bs;
  ras1=okrug(ras1,1.);
  balst_r=ras1-balst;
  balst_r=okrug(balst_r,1.);
  
  iznos=bal_st.siz+bal_st.iz+bal_st.iz1;
  ras2=iznos*koef_iz;
  ras2=okrug(ras2,1.);
  iznos_r=ras2-iznos;
  iznos_r=okrug(iznos_r,1.);
  
  fprintf(ff,"%11d %-*.*s %10.2f %10.2f %10.2f %10.2f\n",
  innom,
  iceb_tu_kolbait(33,naim.ravno()),iceb_tu_kolbait(33,naim.ravno()),naim.ravno(),
  bal_st.sbs+bal_st.bs,ras1,bal_st.siz+bal_st.iz+bal_st.iz1,ras2);

  if(gd != 0)
   {
    if(ras1 != 0)
     uosbd_zap(dd,md,gd,kodop,kodotl,podr,innom,balst_r,iznos_r,&kod_podr_mo,&snomdok,dpnds);
   }
  itog1+=bal_st.sbs+bal_st.bs;
  itog2+=ras1;
  itog3+=bal_st.siz+bal_st.iz+bal_st.iz1;
  itog4+=ras2;
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),
itog1,itog2,itog3,itog4);


podpis(ff);
fclose(ff);
fclose(fftmp);

if(snomdok.kolih() > 0)
 {
  sprintf(strsql,"%s:%d",gettext("Количество документов"),snomdok.kolih());
  iceb_t_soob(strsql);
  
 }


oth->spis_imaf.plus(imaftb);
oth->spis_naim.plus(gettext("Дооценка основных средств для бюджетных организаций"));

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);

}
/**************************/
/*Запись шапки документа (если нужно) и запись в документ*/
/************************/
/*Если вернули 0-всё записано*/
int uosbd_zap(short dd,short md,short gd,
const char *kodop,
int kod_mo,
int podr,
int innom,
double bs,
double iz,
class SPISOK *kod_podr_mo, /*код материально-ответственного|код подразделения*/
class SPISOK *snomdok,   /*список номеров документов*/
float dpnds) /*Процент НДС действовавший на момент создания документа*/
{
char strsql[512];
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

  memset(strsql,'\0',sizeof(strsql));
  uosnd(gd,1,&nom_dok);

//  nom_dok.new_plus(strsql);
  snomdok->plus(nom_dok.ravno());

  sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,podt,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%d,%d,%d,%d,%d,%ld,%.2f)",
  gd,md,dd,1,kodop,"00",nom_dok.ravno(),podr,kod_mo,0,0,iceb_t_getuid(),time(NULL),dpnds);

  if(sql_zapis(strsql,1,0) != 0)
   return(1);
 }
 
sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,iz,kodop,ktoz,vrem) \
values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,'%s',%d,%ld)",
gd,md,dd,1,0,innom,nom_dok.ravno(),podr,kod_mo,0,bs,iz,kodop,iceb_t_getuid(),time(NULL));

return(sql_zapis(strsql,1,0));
}

