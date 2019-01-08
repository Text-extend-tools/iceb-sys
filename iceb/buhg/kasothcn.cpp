/*$Id: kasothcn.c,v 5.21 2013/09/26 09:43:34 sasa Exp $*/
/*14.07.2015	25.09.2006	Белых А.И.	kasothcn.c
Расчёт журналов по кодам целевого назначения
*/
#include        <errno.h>
#include        "buhg.h"

void kasothcn_sap(class masiv_din_int *mkcn,FILE *ff);
void kasothcn_rm(int kolih,double *prihod,double *rashod,FILE *ff);


int kasothcn(class spis_oth *oth)
{

static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str shet("");
static class iceb_tu_str kassa("");
static class iceb_tu_str kodop("");
static class iceb_tu_str vpr("");
static class iceb_tu_str kod_cn("");
int		kom,kom1;
short		dn,mn,gn,dk,mk,gk;
class iceb_tu_str kodcn;
class iceb_tu_str naim;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка целевой кассовой книги"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kassa.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(vpr.ravno(),2);
VV.VVOD_SPISOK_add_data(kod_cn.ravno(),128);


VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код кассы....(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции.(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.........(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.(+/-)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код цел.назначения...."));

naz:;
clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kassa.new_plus(VV.data_ravno(2));
kodop.new_plus(VV.data_ravno(3));
shet.new_plus(VV.data_ravno(4));
vpr.new_plus(VV.data_ravno(5));
kod_cn.new_plus(VV.data_ravno(6));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("kas4_2.txt");
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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов целевого назначения"));
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
        naim.new_plus("");
        if(dirkas(1,&kassa,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kassa.ravno());
        goto naz;

      case 1 :
        naim.new_plus("");
        if(dirkasop(1,1,&kodop,&naim) == 0)
          VV.data_z_plus(3,kodop.ravno());
        goto naz;

      case 2 :
        naim.new_plus("");
        if(dirkasop(2,1,&kodop,&naim) == 0)
          VV.data_z_plus(3,kodop.ravno());
        goto naz;
      
      case 3 :
        vibrek("Plansh",&shet);      
        VV.VVOD_SPISOK_zapis_data(4,shet.ravno());
        goto naz;

      case 4 :
        kodcn.new_plus("");
        naim.new_plus("");
        if(dirkcn(1,&kodcn,&naim) == 0)
          VV.data_z_plus(6,kodcn.ravno());
        goto naz;
               
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }
GDITE();
if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
 goto naz;







char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

class masiv_din_int mkcn; //массив кодов целевого назначения
class masiv_din_double mscn; //массив сумм сальдо целевого назначения

//читаем список кодов целевого назначения
sprintf(strsql,"select kod from Kascn");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kod_cn.ravno(),row[0],0,0) != 0)
   continue;
  mkcn.plus(atoi(row[0]));
 }

//Читаем сальдо по кодам целевого назначения
sprintf(strsql,"select kod,ks,saldo from Kascnsl where god=%d",gn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

int kolih_kcn=mkcn.kolih();

mscn.make_class(kolih_kcn);

int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kassa.ravno(),row[0],0,0) != 0)
   continue;
  if(proverka(kod_cn.ravno(),row[1],0,0) != 0)
   continue;

  nomer=mkcn.find(atoi(row[0]));
  mscn.plus(atof(row[2]),nomer);
 }


//читаем документы

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop from Kasord where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"kokcn%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(strsql,errno,"");
  return(1);
 }
 
iceb_t_zagolov(gettext("Целевая кассовая книга"),dn,mn,gn,dk,mk,gk,ff);

if(shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shet.ravno());
if(kassa.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Касса"),kassa.ravno());
if(kodop.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop.ravno());
if(vpr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Приход/расход"),vpr.ravno());
if(kod_cn.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код целевого назначения"),kod_cn.ravno());
 

kasothcn_sap(&mkcn,ff);
int kolstr1=0;
short d,m,g;
double prihod[kolih_kcn];
double rashod[kolih_kcn];
memset(prihod,'\0',sizeof(prihod));
memset(rashod,'\0',sizeof(rashod));

double suma=0.,sumapod=0.;
double prih_itogo=0.;
double rash_itogo=0.;

int tipz=0;
short metka_saldo=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kassa.ravno(),row[0],0,0) != 0)
   continue;
  
  if(proverka(shet.ravno(),row[4],0,0) != 0)
   continue;

  if(proverka(kodop.ravno(),row[5],0,0) != 0)
   continue;
  
  tipz=atoi(row[1]);
  if(vpr.ravno()[0] == '+' && tipz == 2)
   continue;

  if(vpr.ravno()[0] == '-' && tipz == 1)
   continue;

  //читаем код целевого назначения этой операции
  if(tipz == 1)
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[5]);
  if(tipz == 2)
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[5]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
   if(atoi(row1[0]) != 0)
    {
     if(proverka(kod_cn.ravno(),row1[0],0,0) != 0)
      continue;
    }
   else 
    continue;
   }
  if((nomer=mkcn.find(atoi(row1[0]))) < 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,"Не найден код %d в массиве кодов целевого назначения !",atoi(row1[0]));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }
  rsdat(&d,&m,&g,row[2],2);
  
  sumkasor(row[0],tipz,row[3],g,&suma,&sumapod);
  
  if(sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    if(tipz == 2)
      sumapod*=-1; 
    mscn.plus(sumapod,nomer);
   }
  else //период
   {
    if(metka_saldo == 0)
     {
      memset(prihod,'\0',sizeof(prihod));
      memset(rashod,'\0',sizeof(rashod));
      for(int ii=0; ii < kolih_kcn; ii++)
       {
        if(mscn.ravno(ii) < 0)
         rashod[ii]=mscn.ravno(ii);
        else
         prihod[ii]=mscn.ravno(ii);
       }

      fprintf(ff,"%*s ",iceb_tu_kolbait(19,gettext("Сальдо")),gettext("Сальдо"));
      
      kasothcn_rm(kolih_kcn,prihod,rashod,ff);
      if(mscn.suma() > 0)
       {
        fprintf(ff,"%8.2f\n",mscn.suma());
        prih_itogo+=mscn.suma();
       }
      else 
       {
        fprintf(ff,"%8s %8.2f\n","",mscn.suma());
        rash_itogo+=mscn.suma();
       }
      metka_saldo=1;
     }

    memset(prihod,'\0',sizeof(prihod));
    memset(rashod,'\0',sizeof(rashod));
    if(tipz == 1)
     {
      prihod[nomer]=sumapod;    
      prih_itogo+=sumapod;
     }
    if(tipz == 2)
     {
      rashod[nomer]=sumapod;    
      rash_itogo+=sumapod;
     }
    
    fprintf(ff,"%02d.%02d.%04d %-7s ",d,m,g,row[3]);

    kasothcn_rm(kolih_kcn,prihod,rashod,ff);
    if(tipz == 1)
      fprintf(ff,"%8.2f\n",sumapod);
    if(tipz == 2)
      fprintf(ff,"%8s %8.2f\n","",sumapod);
    
    if(tipz == 2)
      sumapod*=-1; 
    mscn.plus(sumapod,nomer);
   }

 }

memset(prihod,'\0',sizeof(prihod));
memset(rashod,'\0',sizeof(rashod));

for(int ii=0; ii < kolih_kcn; ii++)
 {
  if(mscn.ravno(ii) < 0)
   rashod[ii]=mscn.ravno(ii);
  else
   prihod[ii]=mscn.ravno(ii);
 }

fprintf(ff,"%*s ",iceb_tu_kolbait(19,gettext("Сальдо")),gettext("Сальдо"));

kasothcn_rm(kolih_kcn,prihod,rashod,ff);

if(prih_itogo > rash_itogo)
 fprintf(ff,"%8.2f\n",prih_itogo-rash_itogo);
else
 fprintf(ff,"%8s %8.2f\n","",prih_itogo-rash_itogo);
 

podpis(ff);
fclose(ff);

iceb_t_ustpeh(imaf,0);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Целевая кассовая книга"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),1);
return(0);
}





/************************/
/*Распечатка шапки*/
/**********************/

void kasothcn_sap(class masiv_din_int *mkcn,FILE *ff)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

int kol_kcn=mkcn->kolih();

class iceb_tu_str strok_p;
strok_p.plus("--------------------");
for(int ii=0; ii <= kol_kcn; ii++)
 strok_p.plus("------------------");

fprintf(ff,"%s\n",strok_p.ravno());

fprintf(ff,"%s",gettext("  Дата    | Номер  |"));
class iceb_tu_str naim("");
for(int ii=0; ii < kol_kcn; ii++)
 {
  sprintf(strsql,"select naik from Kascn where kod=%d",mkcn->ravno(ii));

  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(17,naim.ravno()),iceb_tu_kolbait(17,naim.ravno()),naim.ravno());
 }
fprintf(ff,"%-*.*s|\n",iceb_tu_kolbait(17,gettext("Итого")),iceb_tu_kolbait(17,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s",gettext("          |докумен.|"));
for(int ii=0; ii <= kol_kcn; ii++) 
 {
  fprintf(ff,"%s",gettext(" Приход | Расход |"));
 }
fprintf(ff,"\n");

fprintf(ff,"%s\n",strok_p.ravno());

}
/*****************************/
/*распечатка массива*/
/***********************/

void kasothcn_rm(int kolih,double *prihod,double *rashod,FILE *ff)
{
for(int ii=0; ii < kolih; ii++)
 {
  if(prihod[ii] == 0.)
   fprintf(ff,"%8s ","");
  else
   fprintf(ff,"%8.2f ",prihod[ii]);

  if(rashod[ii] == 0.)
   fprintf(ff,"%8s ","");
  else
   fprintf(ff,"%8.2f ",rashod[ii]);
  
 }

}
