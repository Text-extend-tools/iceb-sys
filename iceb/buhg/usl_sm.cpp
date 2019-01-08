/*$Id: usl_sm.c,v 5.29 2014/07/31 07:09:17 sasa Exp $*/
/*01.12.2016	15.02.2005	Белых А.И.	usl_sm.c
Списание материалла в мат-учёте
*/
#include "buhg.h"


void usl_sm(int tipz,
short dd,short md,short gd,
int podr,
int lnds,
const char *kontr,
const char *nomdok,
float pnds)
{
char strsql[512];
class iceb_tu_str sklz("");
class iceb_tu_str kodop("");
class iceb_tu_str datdok("");
class iceb_tu_str nomdok_mat("");
short dm,mm,gm;
int kom=0;
int kom1=0;
int nom_str=0;
SQL_str row;
SQLCURSOR cur;

GDITE();
//Проверяем документ на наличие материалов
sprintf(strsql,"select tp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0 limit 1",gd,md,dd,podr,nomdok,tipz);
if(sql_readkey(&bd,strsql) <= 0)
 {
  iceb_t_soob(gettext("В документе не введено ни одного материалла !"));
  return;
 }


char osnov[512];
sprintf(osnov,"USL%02d.%02d.%d N%s",dd,md,gd,nomdok);

//Проверяем может уже материалы списаны
sprintf(strsql,"select sklad,nomd from Dokummat2 where god=%d and nomerz=2 and sodz='%s'",
gd,osnov);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  VVOD SOOB(1);

  SOOB.VVOD_SPISOK_add_MD(gettext("Материалы уже списаны."));

  sprintf(strsql,"%s %s",gettext("Номер документа"),row[1]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s %s",gettext("Склад"),row[0]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;

 }


sprintf(strsql,"%d.%d.%d",dd,md,gd);
datdok.new_plus(strsql);
VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Дата............."));
VV.VVOD_SPISOK_add_MD(gettext("Склад............"));
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции....."));
VV.VVOD_SPISOK_add_data(datdok.ravno(),11);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);



naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("спис."),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F10",gettext("выход"),NULL);

kom=VV.vvod(nom_str,1,1,-1,-1);

datdok.new_plus(VV.VVOD_SPISOK_return_data(0));
sklz.new_plus(VV.VVOD_SPISOK_return_data(1));
nomdok_mat.new_plus(VV.VVOD_SPISOK_return_data(2));
kodop.new_plus(VV.VVOD_SPISOK_return_data(3));

class iceb_tu_str kods("");
class iceb_tu_str naims("");

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("usl2_1_1_3.txt");
    goto naz;

  case FK10:
  case ESC:
     return;

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    if(tipz == 1)
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    if(tipz == 2)
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_plus(1,kods.ravno());
        goto naz;

      case 1 :
        if(tipz == 1)
         dirprihod(1,&kods,&naims);
        if(tipz == 2)
         dirrashod(1,&kods,&naims);
        VV.VVOD_SPISOK_zapis_data(3,kods.ravno());
        nom_str=3;
        goto naz;

     }
    goto naz;

  case FK4:
    if(rsdat(&dm,&mm,&gm,VV.VVOD_SPISOK_return_data(0),1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto naz;
     }

    if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён склад !"));
      goto naz;
     }
     
    iceb_t_nomnak(gm,VV.VVOD_SPISOK_return_data(1),&nomdok_mat,tipz,0,0);
    VV.VVOD_SPISOK_zapis_data(2,nomdok_mat.ravno());
    nom_str=2;
    goto naz;

  default:
    goto naz;
 }

GDITE();

if(rsdat(&dm,&mm,&gm,VV.VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён склад !"));
  goto naz;
 }

//Проверяем номер склада в списке складов
sprintf(strsql,"select kod from Sklad where kod=%s",sklz.ravno());
if(sql_readkey(&bd,strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),sklz.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }
 
//Проверяем код операции в списке операций
if(tipz == 1)
 sprintf(strsql,"select kod from Prihod where kod='%s'",kodop.ravno());
if(tipz == 2)
 sprintf(strsql,"select kod from Rashod where kod='%s'",kodop.ravno());

if(sql_readkey(&bd,strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

//Блокируем таблицу
class lock_tables lock("LOCK TABLES Dokummat WRITE,icebuser READ");

/*Проверяем есть ли такой номер документа*/
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
gm,gm,sklz.ravno(),nomdok_mat.ravno());
if(sql_readkey(&bd,strsql) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok_mat.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

if(zap_s_mudok(tipz,dm,mm,gm,sklz.ravno_atoi(),kontr,nomdok_mat.ravno(),"",kodop.ravno(),0,0,"",0,0,0,0,"00",pnds) != 0)
 goto naz;

GDITE();

//Разблокируем все таблицы
lock.unlock();

int kolstr=0;

sprintf(strsql,"select kodzap,kolih,cena,ei,shetu,dnaim,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0",gd,md,dd,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

//Записываем через кого (есть в документе на приход и на расход)
sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",gm,sklz.ravno(),nomdok_mat.ravno(),2,osnov);

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка записи !"),strsql);


//программа позволяет записать один и тотже материал любое количество раз
//поэтому нужно просумировать количества одинаковых кодов материалов с тем 
//чтобы списать в материальном учёте одной записью
masiv_din_int kod_mat;
masiv_din_double kolih_mat;
int kodm;
double kolih;
int nomer;
while(cur.read_cursor(&row) != 0)
 {
  kodm=atoi(row[0]);
  kolih=atof(row[1]);
  
  if((nomer=kod_mat.find(kodm)) < 0)
   kod_mat.plus(kodm,nomer);
  kolih_mat.plus(kolih,nomer);
  
 }
double ndss=pnds;
if(lnds != 0)
 ndss=0.;
 
for(int i=0; i < kod_mat.kolih() ; i++)
 {

//  printw("%d %.2f\n",kod_mat.ravno(i),kolih_mat.ravno(i));
//  refresh();
  
  sprintf(strsql,"select cena,ei,shetu,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0 and kodzap=%d limit 1",
  gd,md,dd,podr,nomdok,tipz,kod_mat.ravno(i));
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_t_soob(gettext("Не найдена запись в документе !"));
    continue;
   }

  if(tipz == 1)
   zap_prihod(dm,mm,gm,nomdok_mat.ravno(),sklz.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[1],row[2],ndss,0,0,"",row[3]);
  if(tipz == 2)
    priv_k_kart(dm,mm,gm,nomdok_mat.ravno(),sklz.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[2],row[2],ndss,0,0,"",0,row[3]);

 
 }
podtdok1(dm,mm,gm,sklz.ravno_atoi(),nomdok_mat.ravno(),tipz,dm,mm,gm,1);
podvdok(dm,mm,gm,nomdok_mat.ravno(),sklz.ravno_atoi());

//OSTANOV();
}
