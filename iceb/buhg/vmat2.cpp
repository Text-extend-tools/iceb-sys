/*$Id:$ */
/*22.03.2017	16.02.1999 	Белых А.И.	vmat2.c
Воод материалов в накладную без привязки к карточкам
*/
#include        <ctype.h>
#include        "buhg.h"

extern short	konost;   /*Контроль остатка 0-включен 1-выключен*/

void vmat2(short dd,short md,short gd,int skl,const char *nomdok,int tipz,float pnds)
{
class iceb_tu_str bros("");
SQL_str         row;
char		strsql[1024];
extern double	okrcn;  /*Округление цены*/
class iceb_tu_str mat("");
int		i;
double		krt;
double		bb;
int      	N,K;
double		nds;
double		ost;
int		skan;
class iceb_tu_str naim("");
int		kgrm;
int		kodm;
double		cen,cena,kolih;
class iceb_tu_str ei("");
double		ndscen=0.;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
N=0;

helstr(LINES-1,0,"Enter",gettext("материалы"),
"F10",gettext("выход"),"  ",
gettext(" + перед числом - поиск по числу в наименовании"),NULL);

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование материалла"));

//if((skan=vvod1(&mat,32,&VVOD1,&readscan,stdscr,-1,-1)) == FK10)
if((skan=vvod1(&mat,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
if(skan == ESC)
  return;

GDITE();
//readscan(NULL); /*Сброс сканерного ввода*/
kgrm=0;
kodm=mat.ravno_atoi();

if(mat.ravno()[0] == '\0' || isdigit(mat.ravno()[0]) == 0)
 {
  clear();

  if(polen(mat.ravno(),&bros,2,'+') != 0)
    bros.new_plus(mat.ravno());
  kodm=0;
  if(isdigit(bros.ravno()[0]) != 0)
     kodm=bros.ravno_atoi();
  naim.new_plus(mat.ravno());
  if(kodm != 0)
    naim.new_plus(kodm);

  if((i=dirmatr1(&kgrm,&kodm,&naim,skl,"\0",0,1,0)) != 0)
   {
    clear();
    return;
   }
 }

GDITE();
/*Проверяем есть ли такая запись*/

sprintf(strsql,"select datd from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=0 and datd='%d-%d-%d'",
skl,nomdok,kodm,gd,md,dd);
if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
 {
  iceb_t_soob(gettext("Такая запись в документе уже есть !"));
  return;
 }

/*Проверяем введён код или нет*/

if(skan != -1) 
  sprintf(strsql,"select kodm,naimat,kodgr,cenapr,krat,ei,nds from Material \
where kodm=%d",kodm);
else
  sprintf(strsql,"select kodm,naimat,kodgr,cenapr,krat,ei,nds from Material \
where strihkod=%s",mat.ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  iceb_t_soob(strsql);
  return;
 }

kodm=atoi(row[0]);
naim.new_plus(row[1]);
kgrm=atoi(row[2]);
cen=atof(row[3]);
cen=okrug(cen,okrcn);
krt=atof(row[4]);
ei.new_plus(row[5]);
ndscen=atof(row[6]);

if(cen < 0.01)
 {
  iceb_t_soob(gettext("Не введена цена !"));
  return;
 }

ost=ostdok1(dt,mt,gt,skl,kodm);
if(tipz == 2)
if(ost <= 0 && konost == 0)
 {
  iceb_t_soob(gettext("Нулевой остаток !"));
  return;
 }

bb=cena=cen;

/*Извлекаем НДС*/

//if(bb != 0. && cnsnds == 1 && ndscen == 0)
if(bb != 0.  && ndscen == 0)
 {
  bb=bb-bb*pnds/(100.+pnds);
  cena=okrug(bb,okrcn);
 }
VVOD VV(0);

if(ndscen == 1.)
 {
//  attron(A_BLINK);
  move(LINES-3,0);
  VV.VVOD_SPISOK_add_ZAG(gettext("Внимание материал без НДС !!!"));
//  attroff(A_BLINK);
 }

sprintf(strsql,gettext("Материал %d %s"),kodm,naim.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s %.10g %s: %.10g",
gettext("Цена с НДС:"),cen,
gettext("Без НДС"),cena);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s: %.10g %s: %.10g",
gettext("Кратность"),krt,
gettext("Остаток"),ost);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Количество......"));
VV.VVOD_SPISOK_add_MD(gettext("Количество тары."));
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);


naz:;


clear(); /*Очистить экран и закрасить фоновым цветом*/


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,1);

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

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    if(N == 0) /*Количество*/
     {
      if(krt != 0.)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
        bb=bb/krt;
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(1,strsql);
        goto naz;
       }       
     }

    if(N == 1) /*Кратность*/
     {
      if(krt != 0.)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
        bb=bb*krt;
        sprintf(strsql,"%.10g",bb);
        VV.VVOD_SPISOK_zapis_data(0,strsql);
        goto naz;
        
       }       
     }
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
/*    N++;*/
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

     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все реквизиты !"));
       goto naz;
      }

    kolih=ATOFM(VV.VVOD_SPISOK_return_data(0));
    if(tipz == 2)
     {
      if(konost == 0)
       {
        ost=ostdok1(dt,mt,gt,skl,kodm);
        if(kolih > ost)
         {
          iceb_t_soob(gettext("Нет нужного количества на остатке !"));
          goto naz;
         }
       }
     }    
    nds=pnds;
    nds=okrug(nds,0.01);

    zapvdok(dd,md,gd,skl,0,kodm,nomdok,kolih,cena,ei.ravno(),nds,0,0,2,0,"","",0,"");
    break;
  }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

}
