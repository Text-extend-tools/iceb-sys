/* $Id:$ */
/*22.03.2017	14.04.1997	Белых А.И.	korznkr.c
Корректировка записи в документе материального учета
*/
#include <math.h>
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/

void korznkr(int tipz,short dd,short md,short gd,int skl,
const char *nomdok,int kodm,int nk,double kolih,double cena,int sklad1,
const char *nomdokp,int nomkarp,const char *shet,const char *nomz,
time_t vremz,int ktozap,float pnds,int metka_blok)
{
class ostatokt ost;
SQL_str         row;
double          bb;
int		K,N;
char		strsql[1024];
long		kolstr;
double		kolihn;
double		kolp; /*Количество подтвержденное в карточке*/
struct OPSHET shetv;
class iceb_tu_str shetz("");
struct  tm      *bf;
short dt,mt,gt;
class SQLCURSOR cur;
int metka_imp_tov=0;
class iceb_tu_str kdstv("");
short mnz=0; /*0- не подтверждено 
              1-подтверждено одной записью на все количество
              2-подтверждено не на все количество
             */
int kol=0;
poltekdat(&dt,&mt,&gt);

/*читаем недостающие реквизиты записи*/
sprintf(strsql,"select * from Dokummat1 where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d and tipz=%d",
gd,md,dd,skl,nomdok,kodm,nk,tipz);

if((kol=readkey(strsql,&row,&cur)) == 1)
 {
  metka_imp_tov=atoi(row[18]);
  kdstv.new_plus(row[19]);
 }

class VVOD VV(0);

/*Проверяем подтверждена ли запись в карточке*/
mnz=0;
kolp=0.;
N=0;

sprintf(strsql,"select kolih from Zkart where sklad=%d and nomk=%d and \
nomd='%s' and datd='%04d-%02d-%02d'",
skl,nk,nomdok,gd,md,dd);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  if(fabs(atof(row[0]) - kolih) < 0.00001)
    mnz=1;
  else
    mnz=2;    
 }

if(ktozap != 0)
 {
  sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(ktozap));
  VV.VVOD_SPISOK_add_ZAG(strsql);             
 }

if(vremz > 0)
 {
  bf=localtime(&vremz);
  sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
  gettext("Дата записи :"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

//Читаем наименование материалла
sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  sprintf(strsql,"%s:%d %s",gettext("Материал"),kodm,row[0]);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }
sprintf(strsql,"%s:%d",gettext("Номер карточки"),nk);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%.10g",kolih);
VV.VVOD_SPISOK_add_data(strsql,20);
if(tipz == 2) /*реализация*/
 {
  bb=cena+cena*pnds/100.;
  bb=okrug(bb,okrg1);

  sprintf(strsql,"%.10g",bb);
  VV.VVOD_SPISOK_add_data(strsql,20);

  sprintf(strsql,"%.10g",cena);
  VV.VVOD_SPISOK_add_data(strsql,20);

  VV.VVOD_SPISOK_add_data(shet,20);

  VV.VVOD_SPISOK_add_data(nomz,20);

  if(metka_imp_tov == 1)
   VV.VVOD_SPISOK_add_data("X",3);
  else
   VV.VVOD_SPISOK_add_data("",3);

  VV.VVOD_SPISOK_add_data(kdstv.ravno(),32);
    
  VV.VVOD_SPISOK_add_MD(gettext("Количество......................."));//0
  VV.VVOD_SPISOK_add_MD(gettext("Цена с НДС......................."));//1
  VV.VVOD_SPISOK_add_MD(gettext("Цена без НДС....................."));//2
  VV.VVOD_SPISOK_add_MD(gettext("Счёт списания...................."));//3
  VV.VVOD_SPISOK_add_MD(gettext("Номер заказа....................."));//4
  VV.VVOD_SPISOK_add_MD(gettext("Признак импорта.......(X)........"));//5
  VV.VVOD_SPISOK_add_MD(gettext("Код вида сель.тов-производителя.."));//6
 }
else
 {
  VV.VVOD_SPISOK_add_data(shet,20);


  VV.VVOD_SPISOK_add_MD(gettext("Количество......"));
  VV.VVOD_SPISOK_add_MD(gettext("Счёт получения.."));
 }

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Количество*/
     {
     }
    if(tipz == 2 && N == 1) /*Ввод цены с ндс*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      bb=bb-bb*pnds/(100.+pnds);
      bb=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      VV.data_plus(2,strsql);
      goto naz;
     }
    if(tipz == 2 && N == 2) /*Ввод цены без НДС*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      bb=okrug(bb,okrcn);
      bb=bb+bb*pnds/100.;
      bb=okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      VV.data_plus(1,strsql);
      goto naz;
            
     }
    if(tipz == 2 && N == 3) //Счёт
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       if(prsh1(VV.VVOD_SPISOK_return_data(N),&shetv) != 0)
        goto naz;
     }

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
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     GDITE();

     if(metka_blok != 0)
      {
       iceb_t_soob(gettext("Документ заблокирован!"));
       goto naz;
      }     

     if(mu_pvku(ktozap) != 0) /*проверка хозяина записи*/
       goto naz;

     if(nk != 0)
      {
       /*Читаем подтвержденное количество*/
       bb=ATOFM(VV.VVOD_SPISOK_return_data(0));
       kolp=readkolk(skl,nk,dd,md,gd,nomdok);  

       if(mnz == 0)
       if(bb-kolp < 0.000001)
        {
         sprintf(strsql,"%s (%.10g > %.10g)",gettext("Уже подверждено в карточке больше !"),bb,kolp);
         iceb_t_soob(strsql);
         goto naz;

        }
       if(mnz != 0)
        {
         ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
         if(tipz == 1)
          if((ost.ostg[3] - kolp + bb) < -0.00000000009)
           {
            iceb_t_soob(gettext("Отрицательный остаток в карточке. Корректировка не возможна !"));
            goto naz;
           } 

         if(tipz == 2)
          if(ost.ostg[3] + kolp - bb  < -0.00000000009)
           {
            iceb_t_soob(gettext("Отрицательный остаток в карточке. Корректировка не возможна !"));
            goto naz;
           } 
        }
      }
    
     if(tipz == 1 && VV.VVOD_SPISOK_return_data(1)[0] != '\0')
      {
       shetz.new_plus(VV.VVOD_SPISOK_return_data(1));
       if(prsh1(shetz.ravno(),&shetv) != 0)
         goto naz;
      }
     if(tipz == 2 && VV.VVOD_SPISOK_return_data(3)[0] != '\0')
      {
       shetz.new_plus(VV.VVOD_SPISOK_return_data(3));
       if(prsh1(shetz.ravno(),&shetv) != 0)
         goto naz;
      }
     kolihn=ATOFM(VV.VVOD_SPISOK_return_data(0));
     if(tipz == 2)
      {
       cena=ATOFM(VV.VVOD_SPISOK_return_data(2));
       cena=okrug(cena,okrcn);
      }

     if(tipz == 2)
      {
       kdstv.new_plus(VV.data_ravno(6));
       if(VV.data_ravno(5)[0] != '\0')
        metka_imp_tov=1;
       else
        metka_imp_tov=0;
      }
     else
      metka_imp_tov=0;
            
     if(sklad1 != 0 && nomdokp[0] != '\0')
      {
       //Корректируем в парном документе
       if(tipz == 1)
         sprintf(strsql,"update Dokummat1 \
set \
kolih=%.10g,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d and tipz=2",
         kolihn,iceb_t_getuid(),time(NULL),
         gd,md,dd,sklad1,nomdokp,kodm,nomkarp);

       if(tipz == 2)
         sprintf(strsql,"update Dokummat1 \
set \
kolih=%.10g,\
ktoi=%d,\
vrem=%ld,\
mi=%d,\
kdstv='%s' \
where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d and tipz=1",
         kolihn,iceb_t_getuid(),time(NULL),metka_imp_tov,kdstv.ravno_filtr(),
           
         gd,md,dd,sklad1,nomdokp,kodm,nomkarp);



       if(sql_zap(&bd,strsql) != 0)
           msql_error(&bd,gettext("Ошибка записи !"),strsql);
/*************
       printw("\nstrsql=%s\n",strsql);
       OSTANOV(); 
**************/
       //Проверка подтверждения документа
       podvdok(dd,md,gd,nomdokp,sklad1);
              
      }

     sprintf(strsql,"update Dokummat1 \
set \
kolih=%.10g,\
cena=%.10g,\
ktoi=%d,\
vrem=%ld,\
shet='%s',\
nomz='%s',\
mi=%d,\
kdstv='%s' \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
     kolihn,cena,iceb_t_getuid(),time(NULL),shetz.ravno(),VV.VVOD_SPISOK_return_data(4),
     metka_imp_tov,kdstv.ravno_filtr(),
     gd,md,dd,skl,nomdok,kodm,nk);
/*
     printw("\nstrsql=%s\n",strsql);
     OSTANOV(); 
*/
     sql_zapis(strsql,0,0);

     /*Проверяем если подтверждено все количество то исправляем в 
     карточке тоже
     */

     if(nk == 0)
       break;
       
     sprintf(strsql,"select kolih from Zkart \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d and nomk=%d",
     gd,md,dd,nomdok,skl,nk);
     if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      {
       printw("%s\n",sql_error(&bd));
       OSTANOV();
      }

     if(kolstr == 1)
      {
       cur.read_cursor(&row);
       bb=atof(row[0]);
/*
       printw("row=%s bb=%.6g\n",row[0],bb);
       OSTANOV();     
*/
       if(fabs(bb-kolih) < 0.0001)
        {
         sprintf(strsql,"update Zkart \
set \
kolih=%.10g,\
cena=%.10g \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d and nomk=%d",
         kolihn,cena,
         gd,md,dd,nomdok,skl,nk);

         sql_zapis(strsql,1,0);
         }
        }     
      break;
     }
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CFM));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
}
