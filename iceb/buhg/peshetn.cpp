/* $Id: peshetn.c,v 5.21 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015    25.10.1994      Белых А.И.      peshetn.c
Печать счетов
*/
#include        <errno.h>
#include        "buhg.h"

extern double   okrg1; /*Округление*/
extern short    shetg; /*1-счет для железной дороги 0-нет*/

void            peshetn()
{
char str[1024];
class iceb_tu_str naior(""); /*Наименование организации*/
class iceb_tu_str gorod(""); /*Город*/
class iceb_tu_str kod("");   /*Почтовый код*/
class iceb_tu_str naiban("");/*Наименование банка*/
class iceb_tu_str mfo("");   /*МФО*/
class iceb_tu_str nsh("");   /*Номер счета в банке*/

class iceb_tu_str naior1(""); /*Наименование организации*/
class iceb_tu_str gorod1(""); /*Город*/
class iceb_tu_str kod1("");   /*Почтовый код*/
class iceb_tu_str naiban1("");/*Наименование банка*/
class iceb_tu_str mfo1("");   /*МФО*/
class iceb_tu_str nsh1("");   /*Номер счета в банке*/

char            imaf[64];
FILE            *ff;
short           kk;
class iceb_tu_str sh("");
double          kol,cena,itog;
double          nac;  /*Наценка*/
class iceb_tu_str datt("");
short           np;
double          bb;
double nds1=0.;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);


clear(); /*Очистить экран и закрасить фоновым цветом*/
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Печать счетов"));

VV.VVOD_SPISOK_add_MD(gettext("Номер счета.."));
VV.VVOD_SPISOK_add_MD(gettext("Наценка......"));
VV.VVOD_SPISOK_add_MD(gettext("Дата........."));
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(5);
VV.VVOD_SPISOK_add_data(20);

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

kk=VV.vvod(0,1,1,-1,-1);

if(kk != FK2 && kk != PLU)
  return;

clear(); /*Очистить экран и закрасить фоновым цветом*/

sh.new_plus(VV.VVOD_SPISOK_return_data(0));
nac=ATOFM(VV.VVOD_SPISOK_return_data(1));
datt.new_plus(VV.VVOD_SPISOK_return_data(2));

nds1=iceb_t_pnds(datt.ravno());

iceb_redfil("platpor.alx",0);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='platpor.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"platpor.alx");
  iceb_t_soob(strsql);
  return;
 }



for(int nom=0; nom < 6 ; nom++)
  cur_alx.read_cursor(&row_alx);

if(polen(row_alx[0],&naior,2,'|') != 0)
 {
  beep();
  printw(gettext("Файл организаций искажен !!!\nАварийное завершение.\n"));
  OSTANOV();
  return;
 }

polen(row_alx[0],&gorod,3,'|');

polen(row_alx[0],&kod,4,'|');

polen(row_alx[0],&naiban,5,'|');

polen(row_alx[0],&mfo,6,'|');

polen(row_alx[0],&nsh,7,'|');

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(row_alx[0][0] == '+')
   {
    polen(row_alx[0],&naior1,2,'|');

    polen(row_alx[0],&gorod1,3,'|');

    polen(row_alx[0],&kod1,4,'|');

    polen(row_alx[0],&naiban1,5,'|');

    polen(row_alx[0],&mfo1,6,'|');

    polen(row_alx[0],&nsh1,7,'|');
    break;
   }
 }

sprintf(imaf,"sh%s.lst",sh.ravno());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

fprintf(ff,"\
%s:%s\n\
%s %s\n\
%s%s\n\
%s %s %s %s %s %s\n",
gettext("Поставщик"),
naior.ravno(),
gettext("Код:"),
kod.ravno(),
gettext("Расчётный счёт N"),
nsh.ravno(),
gettext("в банке"),
naiban.ravno(),
gettext("г."),
gorod.ravno(),
gettext("МФО"),
mfo.ravno());

fprintf(ff,"\
==================================================================\n");

fprintf(ff,"\
		 %s  %s\n\
		  %s %s%s\n\
%s %d%s\n",
gettext("С ч е т N"),
sh.ravno(),
gettext("от"),
datt.ravno(),
gettext("г."),
gettext("Заказ/Наряд N ___________ от __________"),
gt,gettext("г."));

fprintf(ff,"\
==================================================================\n");

fprintf(ff,"\
%s %s\n\
%s %s\n\
%s%s\n\
%s %s %s %s %s %s\n",
gettext("Плательщик:"),
naior1.ravno(),
gettext("Код:"),
kod1.ravno(),
gettext("Расчётный счёт N"),
nsh1.ravno(),
gettext("в банке"),
naiban1.ravno(),
gettext("г."),
gorod1.ravno(),
gettext("МФО"),
mfo1.ravno());

if(shetg == 1)
 {
  fprintf(ff,"\
------------------------------------------------------------------\n");
  fprintf(ff,gettext("На ст._________________ со ст. _______________ Квит.нак. N________\n"));
  fprintf(ff,gettext("Отпр.____________ 20___  г. Способ отпр._________ Контр.вагон N_____\n"));
  fprintf(ff,gettext("Упаковка_________________ Число месяц____________ Вес ____________\n"));
 }
fprintf(ff,"\x0F");
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" N |                 Наименование                     |Ед.из|Количество|        Цена        |     Сумма\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

itog=0.;
np=0;
VV.VVOD_delete();
VV.VVOD_SPISOK_add_ZAG(gettext("Печать счетов."));

VV.VVOD_SPISOK_add_MD(gettext("Наименование........"));
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));
VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));
VV.VVOD_SPISOK_add_MD(gettext("Цена................"));
VV.VVOD_SPISOK_add_data(50);
VV.VVOD_SPISOK_add_data(5);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(20);

for(;;)
 {

  helstr(LINES-1,0,"F2/+",gettext("запись"),
  "F10",gettext("выход"),NULL);
//  kk=vvod(&LL,0,0,3);
  kk=VV.vvod(0,1,1,-1,-1);

  if(kk == ESC || kk == FK10)
    break;

  if(kk == FK2 || kk == PLU)
   {
    np++;
    cena=ATOFM(VV.VVOD_SPISOK_return_data(3));
    kol=ATOFM(VV.VVOD_SPISOK_return_data(2));
    bb=kol*cena;
    bb=okrug(bb,okrg1);
    itog+=bb;
    
    fprintf(ff,"%3d %-*s %-*s %10.10g %20s %20.2f\n",
    np,
    iceb_tu_kolbait(50,VV.VVOD_SPISOK_return_data(0)),VV.VVOD_SPISOK_return_data(0),
    iceb_tu_kolbait(5,VV.VVOD_SPISOK_return_data(1)),VV.VVOD_SPISOK_return_data(1),
    kol,
    prcn(cena),
    bb);
    
    VV.VVOD_clear_data();
   }
 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n\
%*s: %15.2f\n",iceb_tu_kolbait(96,gettext("Итого")),gettext("Итого"),itog);

if(nac > 0.)
 {
  sprintf(str,"%s %.2f%% :",gettext("Наценка"),nac);
  bb=itog*nac/100.;
  bb=okrug(bb,okrg1);
  fprintf(ff,"%*s %15.2f\n",iceb_tu_kolbait(97,str),str,bb);
  itog+=bb;
  fprintf(ff,"%*s %15.2f\n",iceb_tu_kolbait(97,gettext("Итого")),gettext("Итого"),itog);
 }

  bb=itog*nds1/100.;
  sprintf(str,"%s %.f%% :",gettext("НДС"),nds1);
  bb=okrug(bb,okrg1);
  fprintf(ff,"%*s %15.2f\n",iceb_tu_kolbait(79,str),str,bb);
  itog+=bb;
  fprintf(ff,"%*s %15.2f\n",iceb_tu_kolbait(97,gettext("Итого к оплате:")),gettext("Итого к оплате:"),itog);


memset(str,'\0',sizeof(str));
preobr(itog,str,0);

fprintf(ff,"\n%s: %s\n\n",gettext("Сумма прописью"),str);


fprintf(ff,"\n\n\
     %s\n\n\
     %s\n",
gettext("     Руководитель________________"),
gettext("Главный бухгалтер________________"));



fclose(ff);

move(19,0);
printw("%s %s\n",gettext("Распечатка выгружена в файл"),imaf);
OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Счёт для оплаты"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
iceb_t_rabfil(&oth,"");

}
