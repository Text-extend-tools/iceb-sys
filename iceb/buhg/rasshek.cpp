/* $Id: rasshek.c,v 5.20 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	25.01.2001	Белых А.И.	rasshek.c
Распечатка чека на кассовом регистраторе
Вернули 0-чек распечатан
        1-нет
*/
#include        <errno.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/

int 	rasshek(SQLCURSOR *cur, //Курсор с записями документа
short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int skl,  //Склад
const char *nomdok, //Номер документа
int lnds,
KASSA kasr,  //кассовый регистратор
float pnds)
{
int loginrash=0; //Логин распечатавшего чек
double		suma,isuma,kolih,cena,bb;
class iceb_tu_str naimat("");
SQL_str		row,row1;
char		strsql[512];
int		vozvr;
int		vidnal;
double		sum_nal; //наличные
double          sum_tara; //тара
double          sum_pchek; //платежный чек
double          sum_cred;  //кредит
SQLCURSOR curr;
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d \
and nomd='%s' and nomerz=5",gd,skl,nomdok);
if(sql_readkey(&bd,strsql,&row,&curr) > 0)
 {
  loginrash=atoi(row[0]);
  if(loginrash != 0)
   {
    sprintf(strsql,"%s %d",gettext("Чек уже распечатан !"),loginrash);
    iceb_t_soob(strsql);
    return(1);
   }
 }
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Распечатать чек ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return(1);

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Печатаем чек на кассовом регистраторе"));
soobsh(&SOOBN,LINES-6,0,1);

GDITE();

vidnal=1;
if(lnds != 0)
 vidnal=2;  
isuma=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
//  refresh();
  
  /*Читаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
    {
     beep();
     printw(gettext("Не найдено наименование материалла !"));
     printw("\n");
     OSTANOV();
     naimat.new_plus("");
    }
  else
   {
    naimat.new_plus(row1[2]);
   }  
  kolih=atof(row[5]);
  cena=atof(row[6]);

  if(lnds == 0)
    cena=cena+cena*pnds/100.;

  cena=okrug(cena,okrg1);
  bb=kolih*cena;
  suma=okrug(bb,okrg1);
  isuma+=suma;
  sprintf(strsql,"FSTART: ADD_CHECK\n\
%s\n\
%.2f\n\
%.2f\n\
%.6g\n\
%d\n\
FEND:\n",
  naimat.ravno(),suma,cena,kolih,vidnal);  
  if (iceb_cmd(kasr, strsql)) {
     return(1); 
  }  

 }
if(tipz == 2)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
kasr.prodavec,kasr.parol,1,gettext("К оплате"));  

if(tipz == 1)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
kasr.prodavec,kasr.parol,1,gettext("К возврату"));  
iceb_cmd(kasr, strsql);

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
kasr.prodavec,kasr.parol,2,isuma);  
iceb_cmd(kasr, strsql);

vozvr=0;
if(tipz == 1)
 vozvr=1;
/*************************
sum_tara - Тара           |
sum_pchek- Платежный чек  |=> Безналичная форма оплаты
sum_cred - Кредит         |
--------------------
sum_nal  - Наличные

Если сумма безналичной оплаты поркрывает общий результат операций по 
чеку, то все движение средств представляет собой безналичную оплату.
В противном случае недостающая до покрытия часть средств считается
наличной.
*************************/

sum_nal=isuma;
sum_tara=sum_pchek=sum_cred=0.;
sprintf(strsql,"FSTART: MARIA_PRCHECK\n\
%s\n\
%s\n\
%d\n\
%d\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
FEND:\n",
kasr.prodavec,kasr.parol,skl,vozvr,isuma,sum_tara,sum_pchek,sum_cred,sum_nal);

int	voz=1;

if( iceb_cmd(kasr, strsql) == 0)
 {
  //Ставим отметку о распечатанном чеке
  sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%d')",
gd,skl,nomdok,5,iceb_t_getuid());

  sql_zapis(strsql,1,0);

  voz=0;
 }

//OSTANOV();
return(voz);
}
