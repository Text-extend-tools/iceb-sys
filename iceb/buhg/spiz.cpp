/*$Id:$*/
/*22.03.2017	21.11.2003	Белых А.И.	spiz.c
Автоматическое списание материалов входящих в изделие
*/
#include        <math.h>
#include        "buhg.h"

void  spizr(int,short,short,short,const char *,const char *,int,int);


void spiz(int kodiz)
{
char strsql[1024];
class iceb_tu_str naimizd("");
SQL_str  row;
class iceb_tu_str dat1("");
class iceb_tu_str kodskl("");
class iceb_tu_str kolizd("");
int   kom=0;
class iceb_tu_str kodop("");
class iceb_tu_str nomdok("");
short ds,ms,gs;
int kom1=0;
SQLCURSOR cur;
sprintf(strsql,"select naimat from Material where kodm=%d",kodiz);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  naimizd.new_plus(row[0]);


VVOD MENU(3);

VVOD VV(0);
sprintf(strsql,"%d %s",kodiz,naimizd.ravno());
VV.VVOD_SPISOK_add_ZAG(gettext("Списание материалла с карточек."));
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Дата списания.(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код склада............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа........"));//2
VV.VVOD_SPISOK_add_MD(gettext("Количество изделий....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код операции списания.."));//4

VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(32);

VV.VVOD_SPISOK_zapis_data(3,"1");

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("склады"),
"F4",gettext("очистить"),
"F5",gettext("номер докум."),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
kodskl.new_plus(VV.VVOD_SPISOK_return_data(1));
nomdok.new_plus(VV.VVOD_SPISOK_return_data(2));
kolizd.new_plus(VV.VVOD_SPISOK_return_data(3));
kodop.new_plus(VV.VVOD_SPISOK_return_data(4));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(kom)
 {
  case FK1:   //Помощь
   GDITE();
   iceb_t_pdoc("spec4.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

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
        if(dirrashod(1,&kods,&naims) == 0)
         VV.data_plus(4,kods.ravno());
         
        goto naz;
     }  
    goto naz;
    
  case FK4: //Очистить меню
    VV.VVOD_clear_data();  
    goto naz;

  case FK5: //Номер документа

    /*Автоматическая установка номера накладной*/

    if(rsdat(&ds,&ms,&gs,dat1.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto naz;
     }

    if(kodskl.ravno()[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён код склада !"));
      goto naz;
     }

    iceb_t_nomnak(gs,kodskl.ravno(),&nomdok,2,0,0);
    VV.VVOD_SPISOK_zapis_data(2,nomdok.ravno());
    goto naz;
    
  default:
     goto naz;
 }

if(rsdat(&ds,&ms,&gs,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(kodskl.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код склада !"));
  goto naz;
 }

//Проверяем код склада

sprintf(strsql,"select naik from Sklad where kod=%s",kodskl.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),kodskl.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }

if(kolizd.ravno_atoi() < 1)
 {
  iceb_t_soob(gettext("Количество изделий меньше единицы !"));
  goto naz;

 }

//Проверяем есть ли этот код операции в расходах
sprintf(strsql,"select kod from Rashod where kod='%s'",kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

if(nomdok.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён номер документа !"));
  goto naz;
 }


//Проверяем номер документа
//Уникальный номер документа для прихода и расхода
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
gs,gs,kodskl.ravno(),nomdok.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

spizr(kodiz,ds,ms,gs,nomdok.ravno(),kodop.ravno(),kodskl.ravno_atoi(),kolizd.ravno_atoi());


}

/**********************************/
/*Списание изделия                */
/**********************************/

void  spizr(int kodiz,      //Код изделия
short ds,short ms,short gs, //Дата списания
const char *nomdok,               //Номер документа
const char *kodop,                //Код операции
int kodskl,               //Код склада
int koliz)                //Количестов изделий 
{
char strsql[512];
SQL_str row;
time_t vrem;

time(&vrem);

float pnds=iceb_t_pnds(ds,ms,gs);


if(zap_s_mudok(2,ds,ms,gs,kodskl,"00",nomdok,"",kodop,0,0,"",0,0,0,1,"00",pnds) != 0)
 return;
double  kolndet=0.;
int     kodm;
double  kolih=0;

class iceb_t_razuz_data data;
data.kod_izd=kodiz;
if((kolndet=iceb_t_razuz_kod(&data)) == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Не найдено ни одной детали для изделия"),kodiz);
  iceb_t_soob(strsql);
  return;  
 }

int kolkar=0;
double cena=0.;
class iceb_tu_str ei("");

double ndss=iceb_t_pnds();

SQLCURSOR cur;
kolndet=data.kod_det_ei.kolih();
for(int sss=0; sss < kolndet; sss++)
 {
  if(data.metka_mu.ravno(sss) == 1)
   continue;

   
  polen(data.kod_det_ei.ravno(sss),&kodm,1,'|');
  polen(data.kod_det_ei.ravno(sss),&ei,2,'|');
  
  kolih=data.kolih_det_ei.ravno(sss)*koliz;

  cena=0.;
  //Читаем цену
  sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    cena=atof(row[0]);

  class masiv_din_int nomkars;
  class masiv_din_double kolpriv;
    
  if((kolkar=findkar(ds,ms,gs,kodskl,kodm,kolih,&nomkars,&kolpriv,0,cena,"")) <= 0)
    zapvdok(ds,ms,gs,kodskl,0,kodm,nomdok,kolih,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"");
  else
   {
    double kolihpi=0.;
    for(int sht=0; sht < kolkar; sht++)
     {
      int nomkar=nomkars.ravno(sht);
      double kolihp=kolpriv.ravno(sht);
      kolihpi+=kolihp;
      zapvdok(ds,ms,gs,kodskl,nomkar,kodm,nomdok,kolihp,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"");
     }
    if(fabs(kolih-kolihpi) > 0.000001)
      zapvdok(ds,ms,gs,kodskl,0,kodm,nomdok,kolih-kolihpi,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"");
   }


 }

podtdok1(ds,ms,gs,kodskl,nomdok,2,ds,ms,gs,1);
podvdok(ds,ms,gs,nomdok,kodskl);
}
