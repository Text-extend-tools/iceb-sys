/*29.03.2017	26.11.2016	Белых А.И.	fort_an.c
работа с архивом нарядов
*/
#include <buhl.h>
#include "fort.h"

class fort_an_poisk
 {
  public:
   class iceb_tu_str datn;
   class iceb_tu_str datk;
   class iceb_tu_str nn;
   class iceb_tu_str pod;
   class iceb_tu_str zak;
   class iceb_tu_str isp;
   class iceb_tu_str kom;
   int metka_poi;   

   short mn,gn;
   short mk,gk;
   
   double norma_vrem;
   double sumapn;
   
   fort_an_poisk()
    {
     clear();
    }

   void clear()
    {
     metka_poi=0;
     datn.new_plus("");
     datk.new_plus("");
     nn.new_plus("");
     pod.new_plus("");
     zak.new_plus("");
     isp.new_plus("");
     kom.new_plus("");
    }
 };
 
void fort_an_h(class fort_an_poisk *rp);
int fort_an_poi(class fort_an_poisk *rp);
int fort_an_prov(SQL_str row,class fort_an_poisk *rp);
void fort_an_ras(SQLCURSOR *cur,class fort_an_poisk *rp);
void fort_an_spn(const char *data_nar,const char *nom_nar,double *otr_vrem,double *sumapn);

void fort_an()
{
static class fort_an_poisk rp;

char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row;
class SQLCURSOR cur;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
short mes_nar=0;
short god_nar=0;
short d=0,mn=0,gn=0;
short mz=0,gz=0;
short mk=0,gk=0;
double otr_vrem=0.;
double sumapn=0.;
class iceb_tu_str datav("");
class iceb_tu_str nnv("");
int podrv=0;
class iceb_tu_str zapros("");

poltekdat(&d,&mn,&gn);

sprintf(strsql,"%02d.%d",mn,gn);

if(rp.datn.getdlinna() <= 1)
 rp.datn.new_plus(strsql);

dpm(&d,&mn,&gn,4);/*уменьшаем на месяц*/
dpm(&d,&mn,&gn,4);
mz=mn; gz=gn;
//Создаем класс меню
//VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


zapros.new_plus("select * from Forthn where");
if(mn != 0)
 {
  sprintf(strsql," datan >= '%04d-%02d-01'",gn,mn);
  zapros.plus(strsql);
 }

if(mk != 0)
 {
  if(mn != 0)
   zapros.plus(" and");
   
  sprintf(strsql," datak <= '%04d-%02d-01'",gk,mk);
  zapros.plus(strsql);
 }

zapros.plus(" order by datan,nn desc");
/*********
printw("\n%s-%s\n",__FUNCTION__,zapros.ravno());
OSTANOV();
************/
if((kolstr=cur.make_cursor(&bd,zapros.ravno())) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

rp.norma_vrem=rp.sumapn=0.;

if(kolstr == 0)
 {
lll:;

  fort_an_h(&rp);
  
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }
else
 {
   while(cur.read_cursor(&row) != 0)
    {
     if(fort_an_prov(row,&rp) != 0)
      continue;
     fort_an_spn(iceb_tu_datzap(row[0]),row[1],&otr_vrem,&sumapn);
     rp.norma_vrem+=otr_vrem;
     rp.sumapn+=sumapn;
    }
 }
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(fort_an_prov(row,&rp) != 0)
   continue;
 
  fort_an_spn(iceb_tu_datzap(row[0]),row[1],&otr_vrem,&sumapn);
  
  sprintf(strsql,"%s|%-4s|%-3s|%10.2f|%10.2f|%-*.*s|%-*.*s|%s",
  iceb_tu_datzap_mg(row[0]),
  row[1],
  row[2],
  otr_vrem,
  sumapn,
  iceb_tu_kolbait(10,row[3]),
  iceb_tu_kolbait(10,row[3]),
  row[3],
  iceb_tu_kolbait(10,row[3]),
  iceb_tu_kolbait(10,row[4]),
  row[4],
  row[5]);  

  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


fort_an_h(&rp);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(fort_an_prov(row,&rp) != 0)
     continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);
    if(fort_an_prov(row,&rp) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(fort_an_prov(row,&rp) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&datav,1,'|');
  rsdat1(&mes_nar,&god_nar,datav.ravno());
  
  polen(str_vib.ravno(),&nnv,2,'|');
  polen(str_vib.ravno(),&podrv,3,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER: /*работа с записями наряда*/
    
    fort_anz(mes_nar,god_nar,nnv.ravno(),podrv);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    goto naz1;


  case FK4:       /*Поиск*/

     if(fort_an_poi(&rp) == 1)
      {
       mn=gn=mk=gk=0;
       if(rp.mn > 0)
        {
         mn=rp.mn; gn=rp.gn;
        }
       if(rp.mk > 0)
        {
         mk=rp.mk; gk=rp.gk;
        }
      }
     else
      {
       mn=mz; gn=gz;
       mk=gk=0;
      }

     GDITE();

     clear();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    fort_an_ras(&cur,&rp);
    goto naz;


  default:
    goto naz;
    break;
 }



}
/**************************************/
/*шапка*/
/*************************/
void fort_an_h(class fort_an_poisk *rp)
{
short		y=2;
short           x=COLS-20;
clear();
printw("%s %s:%.2f %s %s:%.2f грн.\n",
gettext("Архив нарядов"),
gettext("Норма времени"),
rp->norma_vrem/60.,
gettext("час."),
gettext("Сумма"),
rp->sumapn/100.);


mvprintw(2,0,"     1      2    3       4          5         6        7        8");
mvprintw(y++,x,"%s:",gettext("Наименования полей"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("наряд"));
mvprintw(y++,x,"3-%s",gettext("подразделение"));
mvprintw(y++,x,"4-%s",gettext("норма времени"));
mvprintw(y++,x,"5-%s",gettext("сумма"));
mvprintw(y++,x,"6-%s",gettext("заказ"));
mvprintw(y++,x,"7-%s",gettext("исполнитель"));
mvprintw(y++,x,"8-%s",gettext("коментарий"));



helstr(LINES-1,0,"Enter",gettext("выбор"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

if(rp->metka_poi == 1)
 {
  move(y+=2,x=COLS-20);
  attron(A_BLINK);

  mvprintw(y++,x,"%s:",gettext("Поиск"));

  if(rp->datn.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("дата начала"),rp->datn.ravno());
  if(rp->datk.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("дата конца"),rp->datk.ravno());
  if(rp->nn.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("номер наряда"),rp->nn.ravno());
  if(rp->zak.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("заказ"),rp->zak.ravno());
  if(rp->isp.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("исполнитель"),rp->isp.ravno());
  if(rp->kom.getdlinna() > 1)
    mvprintw(y++,x,"%s %s",gettext("коментарий"),rp->kom.ravno());

  attroff(A_BLINK);
 }


}
/*****************************/
/*ввод реквизитов поиска*/
/****************************/
int fort_an_poi(class fort_an_poisk *rp)
{
class VVOD VV(0);
int kl=0;

VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер наряда...(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код под-деления(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Заказ..........(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Исполнитель....(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.....(,,).."));//6

VV.VVOD_SPISOK_add_data(rp->datn.ravno(),8);
VV.VVOD_SPISOK_add_data(rp->datk.ravno(),8);
VV.VVOD_SPISOK_add_data(rp->nn.ravno(),128);
VV.VVOD_SPISOK_add_data(rp->pod.ravno(),128);
VV.VVOD_SPISOK_add_data(rp->zak.ravno(),128);
VV.VVOD_SPISOK_add_data(rp->isp.ravno(),128);
VV.VVOD_SPISOK_add_data(rp->kom.ravno(),128);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
//"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kl=VV.vvod(0,1,1,-1,-1);

rp->datn.new_plus(VV.data_ravno(0));
rp->datk.new_plus(VV.data_ravno(1));
rp->nn.new_plus(VV.data_ravno(2));
rp->pod.new_plus(VV.data_ravno(3));
rp->zak.new_plus(VV.data_ravno(4));
rp->isp.new_plus(VV.data_ravno(5));
rp->kom.new_plus(VV.data_ravno(6));

switch(kl)
 {
  case FK10:
  case ESC:
    rp->metka_poi=0; 
    break;

  case FK2:
    rp->mn=rp->gn=rp->mk=rp->gk=0;

    if(rp->datn.getdlinna() <= 1)
     {
       iceb_t_soob(gettext("Не введена дата начала!"));
       goto naz;
   
     }
    else
     if(rsdat1(&rp->mn,&rp->gn,rp->datn.ravno()) != 0)
      {
       iceb_t_soob(gettext("Не правильно введена дата начала!"));
       goto naz;
      }
    if(rp->datk.getdlinna() > 1)
    if(rsdat1(&rp->mk,&rp->gk,rp->datk.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата конца!"));
      goto naz;
     }

    rp->metka_poi=1; 
    break;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
               
  default:
    goto naz;
 }

return(rp->metka_poi);
}
/******************************/
/*проверка реквизитов поиска*/
/******************************/
int fort_an_prov(SQL_str row, //Строка для проверки 
class fort_an_poisk *rp)
{
if(rp->metka_poi == 0)
 return(0);

if(proverka(rp->nn.ravno(),row[1],4,0) != 0)
 return(1);
if(proverka(rp->pod.ravno(),row[2],4,0) != 0)
 return(1);
if(proverka(rp->zak.ravno(),row[4],4,0) != 0)
 return(1);
if(proverka(rp->isp.ravno(),row[5],4,0) != 0)
 return(1);
if(proverka(rp->kom.ravno(),row[6],4,0) != 0)
 return(1);
 
return(0);
}
/******************/
/*печать*/
/*********************/
void fort_an_ras(SQLCURSOR *cur,class fort_an_poisk *rp)
{
char	imaf[64];
SQL_str row;


sprintf(imaf,"nar%d.lst",getpid());
class iceb_fopen nar;
if(nar.start(imaf,"w") != 0)
 return;


iceb_t_zagolov(gettext("Распечатка архива нарядов"),rp->datn.ravno(),rp->datk.ravno(),nar.ff);

if(rp->metka_poi == 1)
 {
  if(rp->nn.getdlinna() > 1)
   fprintf(nar.ff,"%s:%s\n",gettext("Номер наряда"),rp->nn.ravno());
  if(rp->pod.getdlinna() > 1)
   fprintf(nar.ff,"%s:%s\n",gettext("Код подразделения"),rp->pod.ravno());
  if(rp->zak.getdlinna() > 1)
   fprintf(nar.ff,"%s:%s\n",gettext("Заказ"),rp->zak.ravno());
  if(rp->isp.getdlinna() > 1)
   fprintf(nar.ff,"%s:%s\n",gettext("Исполнитель"),rp->isp.ravno());
  if(rp->kom.getdlinna() > 1)
   fprintf(nar.ff,"%s:%s\n",gettext("Коментарий"),rp->kom.ravno());
 }

fprintf(nar.ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(nar.ff,"\
  Дата |Номер н.|Код підр.|       Заказ        |      Виконавець    |      Коментар      |   Час    |  Сума    | Дата і час запису | Хто записав  |\n");
/*
1234567 12345678 123456789 12345678901234567890 12345678901234567890 12345678901234567890 1234567890 1234567890
*/
fprintf(nar.ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);
double otr_vrem=0.;
double sumapn=0.;
double itogo[2];
itogo[0]=itogo[1]=0.;

while(cur->read_cursor(&row) != 0)
 {
  if(fort_an_prov(row,rp) != 0)
    continue;
 
  fort_an_spn(iceb_tu_datzap(row[0]),row[1],&otr_vrem,&sumapn);
  
  itogo[0]+=otr_vrem/60.;
  itogo[1]+=sumapn;
  
  fprintf(nar.ff,"%s %-8s %-9s %-*.*s %-*.*s %-*.*s %10.2f %10.2f %s %s\n",
  iceb_tu_datzap_mg(row[0]),
  row[1],
  row[2],
  iceb_tu_kolbait(20,row[3]),
  iceb_tu_kolbait(20,row[3]),
  row[3],
  iceb_tu_kolbait(20,row[4]),
  iceb_tu_kolbait(20,row[4]),
  row[4],
  iceb_tu_kolbait(20,row[5]),
  iceb_tu_kolbait(20,row[5]),
  row[5],
  otr_vrem/60.,
  sumapn,
  iceb_tu_vremzap(row[7]),
  iceb_t_kszap(row[6]));
 }

fprintf(nar.ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(nar.ff,"%*s %10.2f %10.2f\n",
iceb_tu_kolbait(89,gettext("Итого")),
gettext("Итого"), 
itogo[0],
itogo[1]);

podpis(nar.ff);
nar.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка архива нарядов"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");
}
/***************************/
/*Получить суммы по наряду*/
/***************************/
void fort_an_spn(const char *data_nar,const char *nom_nar,double *otr_vrem,double *sumapn)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

*otr_vrem=*sumapn=0.;

sprintf(strsql,"select nv,rc,kol from Fortzn where datan='%s' and nn='%s'",iceb_tu_tosqldat(data_nar),nom_nar);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

while(cur.read_cursor(&row) != 0)
 {
  *otr_vrem+=atof(row[0])*atof(row[2]);
  *sumapn+=atof(row[1])*atof(row[2]);
 }
}
