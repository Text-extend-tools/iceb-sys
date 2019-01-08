/* $Id: kategor.c,v 5.45 2013/09/26 09:43:35 sasa Exp $ */
/*28.03.2018	20.03.2000	Белых А.И.	kategor.c
Меню для запуска формирования свода начислений и удержаний по категориям
*/
#include        <errno.h>
#include        "buhg.h"

void		kateg(short,short,short,short,const char[],const char[],const char[],const char[],const char[],const char[],FILE*);
void            kategsh(short,short,short,short,const char[],const char[],const char[],const char[],const char[],FILE*,FILE*,FILE*);
void komplras(short mn,short gn,short mk,short gk,char imaf[],int metkar,const char tabnom[],const char shet[],char *imafhoz,char *imafbu);

extern short	mmm,ggg;
extern char	*shetb; /*Бюджетные счета начислений*/

void		kategor(int metkar)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str nahis("");
static class iceb_tu_str uder("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str shet("");
int		kom,kom1;
short		mn,gn,mk,gk;
class iceb_tu_str bros("");
FILE		*kaw=NULL,*ffhoz=NULL,*ffbu=NULL;
char		imaf[64],imafhoz[64],imafbu[64];
time_t		tmm;
int		tabn;
class iceb_tu_str fio("");

VVOD VV(0);
VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
//  char bros[128];
//  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(mmm);
  dat1.plus(".");
  dat1.plus(ggg);
 }
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и удержаний по категориям"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(uder.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления...(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код удержания....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,).."));

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
nahis.new_plus(VV.data_ravno(4));
uder.new_plus(VV.data_ravno(5));
tabnom.new_plus(VV.data_ravno(6));
shet.new_plus(VV.data_ravno(7));

class iceb_tu_str kod("");
class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   if(metkar == 0)
     bros.new_plus("zarp4_2_1.txt");
   if(metkar == 1)
     bros.new_plus("zarp4_2_2.txt");
   iceb_t_pdoc(bros.ravno());
   goto naz;

  case FK10:
  case ESC:
   return;

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка удержаний"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка настроек для комплексного расчёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kateg",&kategr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kategr.ravno());
        goto naz;

      case 2 :
        if(dirnach(1,&nahis,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;

      case 3 :
        if(diruder(1,&kod,&naim) == 0)
         uder.z_plus(kod.ravno());         
        VV.VVOD_SPISOK_zapis_data(5,uder.ravno());
        goto naz;

      case 4 :
        GDITE();
 
        iceb_redfil("zarpodrk.alx",0);        
        goto naz;

      case 5 :
      
        fio.new_plus("");
        tabn=0;
        if(dirtab(&tabn,&fio,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(6,tabnom.ravno());
         }
        goto naz;

     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  case SFK5: //Настройка комплексного расчёта
    iceb_redfil("zarpodrk.alx",0);
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
memset(imafhoz,'\0',sizeof(imafhoz));
memset(imafbu,'\0',sizeof(imafbu));

if(kom == FK5)
 {
  sprintf(imaf,"kategk%d.lst",getpid());
  sprintf(imafhoz,"kategh%d.lst",getpid());
  sprintf(imafbu,"kategb%d.lst",getpid());
  komplras(mn,gn,mk,gk,imaf,metkar,tabnom.ravno(),shet.ravno(),imafhoz,imafbu);
 }
else
 {
  sprintf(imaf,"kateg%d.lst",getpid());

  if((kaw = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

  if(metkar == 0)
    kateg(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),tabnom.ravno(),shet.ravno(),kaw);
  if(metkar == 1)
   {
    if(shetb != NULL)
     {
      sprintf(imafhoz,"kategh%d.lst",getpid());
      if((ffhoz = fopen(imafhoz,"w")) == NULL)
       {
        error_op_nfil(imafhoz,errno,"");
        return;
       }

      sprintf(imafbu,"kategb%d.lst",getpid());
      if((ffbu = fopen(imafbu,"w")) == NULL)
       {
        error_op_nfil(imafbu,errno,"");
        return;
       }
     }

    kategsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),shet.ravno(),kaw,ffhoz,ffbu);
    
    if(shetb != NULL)
     {
      fclose(ffhoz);

      fclose(ffbu);
     }
   }

  fclose(kaw);
 }

printw_vr(tmm);


OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Расчёт начислений/удержаний по категориям"));

if(shetb != NULL && metkar == 1)
 {


  oth.spis_imaf.plus(imafhoz);
/******************
  sprintf(bros,"%s (%s)",
  gettext("Расчёт начислений/удержаний по категориям"),
  gettext("Хозрасчёт"));
******************/
  bros.new_plus(gettext("Расчёт начислений/удержаний по категориям"));
  bros.plus(" (");
  bros.plus(gettext("Хозрасчёт"));
  bros.plus(")");

  oth.spis_naim.plus(bros.ravno());
  
  oth.spis_imaf.plus(imafbu);
/*****************
  sprintf(bros,"%s (%s)",
  gettext("Расчёт начислений/удержаний по категориям"),
  gettext("Бюджет"));
*******************/    
  bros.new_plus(gettext("Расчёт начислений/удержаний по категориям"));
  bros.plus(" (");
  bros.plus(gettext("Бюджет"));
  bros.plus(")");

  oth.spis_naim.plus(bros.ravno());
 }

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);
 
iceb_t_rabfil(&oth,"");

}

/**********************************************************/
/*Комплексний расчёт, данные для расчёта беруться из файла*/
/**********************************************************/
void komplras(short mn,short gn,short mk,short gk,char imaf[],int metkar,const char tabnom[],const char shet[],char *imafhoz,char *imafbu)
{
char		strsql[1024];
class iceb_tu_str bros("");
class iceb_tu_str podr("");
class iceb_tu_str kategr("");
class iceb_tu_str nahis("");
class iceb_tu_str uder("");
short		metka;
FILE		*kaw=NULL,*ffhoz=NULL,*ffbu=NULL;
class iceb_tu_str kompl("");
class iceb_tu_str komplz("");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='zarpodrk.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarpodrk.alx");
  iceb_t_soob(strsql);
  return;
 }


if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


if(shetb != NULL)
 {
  if((ffhoz = fopen(imafhoz,"w")) == NULL)
   {
    error_op_nfil(imafhoz,errno,"");
    return;
   }

  if((ffbu = fopen(imafbu,"w")) == NULL)
   {
    error_op_nfil(imafbu,errno,"");
    return;
   }
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
      if(metkar == 0)
        kateg(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),tabnom,shet,kaw);
      if(metkar == 1)
        kategsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),shet,kaw,ffhoz,ffbu);
      podr.new_plus("");
      kategr.new_plus("");
      nahis.new_plus("");
      uder.new_plus("");
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
   if(SRAV("Начисления",bros.ravno(),0) == 0)
    polen(row_alx[0],&nahis,2,'|');

  if(polen(row_alx[0],&bros,1,'|') == 0) 
   if(SRAV("Удержания",bros.ravno(),0) == 0)
    polen(row_alx[0],&uder,2,'|');

 }

if(metka > 1)
 fprintf(kaw,"\f");

printw("\n%s\n",komplz.ravno());       
fprintf(kaw,"%s\n",komplz.ravno());

if(metkar == 0)
 kateg(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),tabnom,shet,kaw);

if(metkar == 1)
  kategsh(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),nahis.ravno(),uder.ravno(),shet,kaw,ffhoz,ffbu);


fclose(kaw);

if(shetb != NULL)
 {
  fclose(ffhoz);

  fclose(ffbu);
 }

}
