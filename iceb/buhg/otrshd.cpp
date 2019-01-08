/* $Id: otrshd.c,v 5.30 2013/09/26 09:43:36 sasa Exp $ */
/*28.03.2018	21.03.2000	Белых А.И.	otrshd.c
Меню для запуска формирования отчётов по отработанному времени
*/
#include        <errno.h>
#include        "buhg.h"

void otrsh(short mn,short gn,short mk,short gk,const char podr[],const char kateg[],const char tabelq[],const char tabnom[],FILE *kaw);
void		komplrasov(short,short,short,short,char[]);

extern short	mmm,ggg;

int otrshd(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str tabelq("");
static class iceb_tu_str tabnom("");
int		kom,kom1;
short		mn,gn,mk,gk;
FILE		*kaw;
char		imaf[64];
time_t		tmm;
class iceb_tu_str fio("");
VVOD VV(0);
VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);  
 }
VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabelq.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт отработанного времени по категориям"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.......(м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца........(м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории......(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код табеля.........(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....(,,)."));

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("ком.расч."),
"F10",gettext("выход"),NULL);


kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kategr.new_plus(VV.data_ravno(3));
tabelq.new_plus(VV.data_ravno(4));
tabnom.new_plus(VV.data_ravno(5));

class iceb_tu_str naimp("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_2_3.txt");
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов табеля"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка настроек для комплексного расчёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naimp) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;
      
      case 1 :
        vibrek("Kateg",&kategr,&naimp);
        VV.VVOD_SPISOK_zapis_data(3,kategr.ravno());
        goto naz;

      case 2 :
        vibrek("Tabel",&tabelq,&naimp);
        VV.VVOD_SPISOK_zapis_data(4,tabelq.ravno());
        goto naz;

      case 3 :
      {
        fio.new_plus("");
        int tmm=0;
        if(dirtab(&tmm,&fio,0,1) == 0)
         {
          tabnom.z_plus(tmm);
          VV.VVOD_SPISOK_zapis_data(5,tabnom.ravno());
         }
        goto naz;
      }
      case 4 :
        GDITE();
        iceb_redfil("zarpodrv.alx",0);        
        goto naz;

     }
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }


time(&tmm);
clear();

if(kom == FK5)
 {
  sprintf(imaf,"ovk%d.lst",mmm);
  komplrasov(mn,gn,mk,gk,imaf);
 }
else
 {
  sprintf(imaf,"ov%d.lst",getpid());

  if((kaw = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

  otrsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),tabelq.ravno(),tabnom.ravno(),kaw);

  podpis(kaw);
  fclose(kaw);
 }

printw_vr(tmm);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Ведомость отработанного времени по категориям"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);

return(0);
}
/**********************************************************/
/*Комплексний расчёт, данные для расчёта беруться из файла*/
/**********************************************************/
void komplrasov(short mn,short gn,short mk,short gk,char imaf[])
{
class iceb_tu_str bros("");
class iceb_tu_str podr("");
class iceb_tu_str kategr("");
class iceb_tu_str tabelq("");
class iceb_tu_str tabnom("");
short		metka;
FILE		*kaw;
class iceb_tu_str kompl("");
class iceb_tu_str komplz("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarpodrv.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarpodrv.alx");
  iceb_t_soob(strsql);
  return;
 }



if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


metka=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&kompl,1,':') == 0)
   {
    if(metka != 0)
     {
      if(metka > 1)
       fprintf(kaw,"\f");
      fprintf(kaw,"%s\n",komplz.ravno());       
      printw("\n%s\n",komplz.ravno());       
      
      otrsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),tabelq.ravno(),tabnom.ravno(),kaw);
      
      podr.new_plus("");
      kategr.new_plus("");
      tabelq.new_plus("");
      tabnom.new_plus("");
     }
    metka++;    
    komplz.new_plus(kompl.ravno());
   }

  if(polen(row_alx[0],&bros,1,'|') == 0) 
   if(SRAV("Подразделения",bros.ravno(),0) == 0)
    polen(row_alx[0],&podr,2,'|');

  if(polen(row_alx[0],&bros,1,'|') == 0) 
   if(SRAV("Категории",bros.ravno(),0) == 0)
    polen(row_alx[0],&kategr,2,'|');

  if(polen(row_alx[0],&bros,1,'|') == 0) 
   if(SRAV("Коды табеля",bros.ravno(),0) == 0)
    polen(row_alx[0],&tabelq,2,'|');

  if(polen(row_alx[0],&bros,1,'|') == 0) 
   if(SRAV("Табельные номера",bros.ravno(),0) == 0)
    polen(row_alx[0],&tabnom,2,'|');

 }
if(metka > 1)
 fprintf(kaw,"\f");

printw("\n%s\n",komplz.ravno());       

fprintf(kaw,"%s\n",komplz.ravno());

otrsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),tabelq.ravno(),tabnom.ravno(),kaw);

podpis(kaw);
fclose(kaw);

}
