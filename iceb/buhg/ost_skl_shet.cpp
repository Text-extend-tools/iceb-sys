/*$Id: ost_skl_shet.c,v 5.24 2013/09/26 09:43:36 sasa Exp $*/
/*15.07.2015	08.12.2005	Белых А.И.	ost_skl_shet.c
Получение сводной таблицы остатков по складам и счётам учёта
*/

#include        <errno.h>
#include        "buhg.h"

int ost_skl_shet_prov(SQL_str row,const char *sklad,const char *grup_mat,const char *kodmat,const char *shetz);
void ost_skl_shet_str(int kol,int metka,FILE *ff);
void ost_skl_shet_rm_skl(class masiv_din_int *sp_skl,class SPISOK *sp_shetu,class masiv_din_double *m_kolih,class masiv_din_double *m_suma,FILE *ff);
void ost_skl_shet_rm_shet(class masiv_din_int *sp_skl,class SPISOK *sp_shetu,class masiv_din_double *m_kolih,class masiv_din_double *m_suma,FILE *ff);
void ost_skl_shet_rekpoi(const char *sklad,const char *grup_mat,const char *kodmat,const char *shetz,FILE *ff);


int ost_skl_shet(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str sklad("");
static class iceb_tu_str grup_mat("");
static class iceb_tu_str kodmat("");
static class iceb_tu_str shetz("");
char bros[512];
int kodgr=0;
int kodmati=0;
int kom1=0;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт остатков по складам и счетам учёта"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(sklad.ravno(),128);//1
VV.VVOD_SPISOK_add_data(grup_mat.ravno(),128);//2
VV.VVOD_SPISOK_add_data(kodmat.ravno(),128);//3
VV.VVOD_SPISOK_add_data(shetz.ravno(),128);//4

VV.VVOD_SPISOK_add_MD(gettext("Дата остатка (д.м.г)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Склад.........(,,)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Группа........(,,)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код материалла..(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета.....(,,).."));//4

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
sklad.new_plus(VV.data_ravno(1));
grup_mat.new_plus(VV.data_ravno(2));
kodmat.new_plus(VV.data_ravno(3));
shetz.new_plus(VV.data_ravno(4));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_22.txt");
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 8 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_z_plus(1,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&grup_mat,&naims);
        VV.VVOD_SPISOK_zapis_data(2,grup_mat.ravno());
        goto naz;


      case 2 :
        bros[0]='\0';
        if(dirmatr(&kodgr,&kodmati,&naims,1,0,bros,0,0) == 0)
         {
          VV.data_z_plus(3,kodmati);
         }
        
        goto naz;

      case 6 :
        vibrek("Plansh",&shetz,&naims);
        VV.VVOD_SPISOK_zapis_data(4,shetz.ravno());
        goto naz;


     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

short dk,mk,gk;

if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата !"));
  goto naz;
 }

GDITE();

char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr=0;
int kolstr1=0;


sprintf(strsql,"select sklad,nomk,kodm,shetu,cena from Kart");

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
FILE *fftmp;
char imaftmp[64];
sprintf(imaftmp,"ost_skl_shet%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }


//Создаём нужные списки
class ostatokt ost;
class masiv_din_int sp_skl;
class SPISOK sp_shetu;
int kolstr2=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(ost_skl_shet_prov(row,sklad.ravno(),grup_mat.ravno(),kodmat.ravno(),shetz.ravno()) != 0)
   continue;

  ostkar(1,1,gk,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);
  if(ost.ostg[3] == 0.)
    continue;  

  if(sp_shetu.find(row[3]) < 0)
   sp_shetu.plus(row[3]);

  if(sp_skl.find(row[0]) < 0)
   sp_skl.plus(row[0],-1);

  fprintf(fftmp,"%s|%s|%s|%10.10g|\n",row[0],row[3],row[4],ost.ostg[3]);
  kolstr2++;  
 }
fclose(fftmp);

class masiv_din_double m_kolih;
class masiv_din_double m_suma;

m_kolih.make_class(sp_shetu.kolih()*sp_skl.kolih());
m_suma.make_class(sp_shetu.kolih()*sp_skl.kolih());

int nom_shet=0;
int nom_skl=0;
double suma=0.;
int kol_shet=sp_shetu.kolih();
cur.poz_cursor(0);
kolstr1=0;
if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }
unlink(imaftmp);

char stroka[1024];
class iceb_tu_str sklads("");
class iceb_tu_str shet("");
class iceb_tu_str cena("");
class iceb_tu_str kolih("");
kolstr1=0;
while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  strzag(LINES-1,0,kolstr2,++kolstr1);
  polen(stroka,&sklads,1,'|');
  polen(stroka,&shet,2,'|');
  polen(stroka,&cena,3,'|');
  polen(stroka,&kolih,4,'|');

  nom_skl=sp_skl.find(sklads.ravno());
  nom_shet=sp_shetu.find(shet.ravno());

  suma=kolih.ravno_atof()*cena.ravno_atof();
  m_kolih.plus(kolih.ravno_atof(),nom_skl*kol_shet+nom_shet);
  m_suma.plus(suma,nom_skl*kol_shet+nom_shet);
 }
fclose(fftmp);

GDITE();

FILE *ff;

char imaf[64];
sprintf(imaf,"ostskl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,ff);

ost_skl_shet_rekpoi(sklad.ravno(),grup_mat.ravno(),kodmat.ravno(),shetz.ravno(),ff);

ost_skl_shet_rm_skl(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff);

podpis(ff);

fclose(ff);

char imaf2[64];
sprintf(imaf2,"ostshet%d.lst",getpid());

if((ff = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,ff);

ost_skl_shet_rekpoi(sklad.ravno(),grup_mat.ravno(),kodmat.ravno(),shetz.ravno(),ff);

ost_skl_shet_rm_shet(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff);

podpis(ff);

fclose(ff);





oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт остатков по складам и счетам учёта"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Расчёт остатков по складам и счетам учёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
} 

/**********************************/
/*Проверка на условия поиска*/
/*********************************/

int ost_skl_shet_prov(SQL_str row,
const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz)
{

if(proverka(sklad,row[0],0,0) != 0)
  return(1);
if(proverka(kodmat,row[2],0,0) != 0)
  return(1);
if(proverka(shetz,row[3],0,0) != 0)
  return(1);
if(grup_mat[0] != '\0') //Узнаём группу материалла
 {
  SQL_str row1;
  SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select kodgr from Material where kodm=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[2]);
    iceb_t_soob(strsql);
    return(1);

   }
  if(proverka(grup_mat,row1[0],0,0) != 0)
   return(1);
 }     

return(0);
}

/*****************************/
/*Распечатка массивов по складам*/
/*****************************/
void ost_skl_shet_rm_skl(class masiv_din_int *sp_skl,
class SPISOK *sp_shetu,
class masiv_din_double *m_kolih,
class masiv_din_double *m_suma,
FILE *ff)
{
char strsql[512];
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_shet,0,ff);


fprintf(ff,"%-*.*s|%-*s|",
iceb_tu_kolbait(5,gettext("Код")),iceb_tu_kolbait(5,gettext("Код")),gettext("Код"),
iceb_tu_kolbait(20,gettext("Склад")),gettext("Склад"));
//          12345 12345678901234567890
//Пишем первую строку со счетами
for(int ii=0; ii < kolih_shet; ii++)
 {
  //узнаём наименование 
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(ii));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",sp_shetu->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,gettext("Итого")),iceb_tu_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"     |                    |");

//строка разделения

for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                    |");


for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(9,gettext("Количество")),iceb_tu_kolbait(9,gettext("Количество")),gettext("Количество"),
  iceb_tu_kolbait(10,gettext("Сумма")),iceb_tu_kolbait(10,gettext("Сумма")),gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_shet,0,ff);

//Распечатываем массив
for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  //узнаём наименование 
  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(s_skl));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus(row[0]);
   
  fprintf(ff,"%-5d|%-*.*s|",
  sp_skl->ravno(s_skl),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_shet,0,ff);

fprintf(ff,"%*s|",iceb_tu_kolbait(26,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kolih_skl; skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_shet+ii);
    itogo_po_kol_suma+=m_suma->ravno(skontr*kolih_shet+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}

/*****************************/
/*Распечатка массивов по счетам*/
/*****************************/
void ost_skl_shet_rm_shet(class masiv_din_int *sp_skl,
class SPISOK *sp_shetu,
class masiv_din_double *m_kolih,
class masiv_din_double *m_suma,
FILE *ff)
{
char strsql[512];
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_skl,1,ff);


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_tu_kolbait(7,gettext("Счёт")),
iceb_tu_kolbait(7,gettext("Счёт")),
gettext("Счёт"),
iceb_tu_kolbait(20,gettext("Наименование")),
iceb_tu_kolbait(20,gettext("Наименование")),
gettext("Наименование"));

//Пишем первую строку со счетами
for(int ii=0; ii < kolih_skl; ii++)
 {
  //узнаём наименование 
  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(ii));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");
  sprintf(strsql,"%d %s",sp_skl->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,gettext("Итого")),iceb_tu_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"       |                    |");

//строка разделения

for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"       |                    |");


for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(9,gettext("Количество")),iceb_tu_kolbait(9,gettext("Количество")),gettext("Количество"),
  iceb_tu_kolbait(10,gettext("Сумма")),iceb_tu_kolbait(10,gettext("Сумма")),gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_skl,1,ff);

//Распечатываем массив
for(int s_shet=0; s_shet < kolih_shet; s_shet++)
 {
  //узнаём наименование 
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(s_shet));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");
   
  fprintf(ff,"%-*s|%-*.*s|",
  iceb_tu_kolbait(7,sp_shetu->ravno(s_shet)),sp_shetu->ravno(s_shet),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int s_skl=0; s_skl < kolih_skl; s_skl++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_skl,1,ff);

fprintf(ff,"%*s|",iceb_tu_kolbait(28,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
                                     
    itogo_po_kol_kolih+=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    itogo_po_kol_suma+=m_suma->ravno(s_skl*kolih_shet+s_shet);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");
}

/********************************/
/*Строка подчёркивания*/
/********************************/
void ost_skl_shet_str(int kol,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"---------------------------");
if(metka == 1)
 fprintf(ff,"-----------------------------");

for(int ii=0; ii < kol+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}

/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void ost_skl_shet_rekpoi(const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz,
FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(grup_mat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grup_mat);
if(kodmat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat);
if(shetz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetz);

}
