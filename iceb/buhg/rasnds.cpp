/* $Id: rasnds.c,v 5.50 2013/11/24 08:23:03 sasa Exp $ */
/*01.12.2016	14.01.1998	Белых А.И.	rasnds.c
Подсчет приходов-расходов с ндс и без.
Распределение НДС по видам операций
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"


void rasmasop(SPISOK *spopr,masiv_din_double *mkm,masiv_din_double *mkm1_1r,masiv_din_double *mkm1_1k,short tip,FILE *ff);

extern	double  okrg1;
extern short	vtara;
extern double	okrcn;

int rasnds(class spis_oth *oth)
{
time_t  	tmm;
struct  tm      *bf;
short           kolopp;  /*Количество операций приходов*/
short           kolopr;  /*Количество операций расходов*/
short           dn,mn,gn,dk,mk,gk;
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklz("");
static short    tipzz;     /*Приход/расход*/
static class iceb_tu_str kopz("");  /*Код операции*/
static class iceb_tu_str kodoz(""); /*Код организации*/
static class iceb_tu_str kgrz(""); /*Код группы материала*/
static class iceb_tu_str kodmz("");  /*Код материалла*/
static class iceb_tu_str shuz(""); /*Счёта учета*/
short		mnds;
double		suma,sumak,sumaok,sumao,kol;
int		i;
char		imaf[64],imaf1[64],imaf2[64];
FILE		*ff,*ff1,*ff2;
short		kgrm;
double		bb;
double		itog=0.,itog1=0.;
double		sumkor;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1,row2;
char		strsql[512];
short		vtr;
class iceb_tu_str shu("");
int		skl,vo,nk;
class iceb_tu_str kop("");
short		tp;
short		d,m,g;
int		kgr,kodm;
double		kolih,cena,cenak;
int		nomop=0;
SPISOK           spoprp;/*Массив с перечнем операций приходов*/
SPISOK           spoprr;/*Массив с перечнем операций расходов*/
masiv_din_double mkm1p; /*Массив приходов*/
masiv_din_double mkm1r; /*Массив расходов в  ценах накладной*/
masiv_din_double mkm1_1r; /*Массив расходов в  ценах карточек*/
masiv_din_double mkm1_1k; /*Массив кореектировки сумм*/

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт рапределения НДС по видам операций"));
VV.VVOD_SPISOK_add_ZAG(gettext("По не введенным реквизитам поиск не выполняется"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(2);
if(tipzz == 1)
  VV.VVOD_SPISOK_zapis_data(3,"+");
if(tipzz == 2)
  VV.VVOD_SPISOK_zapis_data(3,"-");
VV.VVOD_SPISOK_add_data(kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(kgrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),128);
VV.VVOD_SPISOK_add_data(shuz.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.............."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..............."));
VV.VVOD_SPISOK_add_MD(gettext("Склад...(,,)............."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/Расход...(+/-)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)......."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код группы мат-ла...(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код материалла..(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета.....(,,)......"));

naz:;
clear();

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("пол.отчёт"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sklz.new_plus(VV.data_ravno(2));
tipzz=0;
if(VV.VVOD_SPISOK_return_data(3)[0] == '+')
  tipzz=1;
if(VV.VVOD_SPISOK_return_data(3)[0] == '-')
  tipzz=2;
kopz.new_plus(VV.data_ravno(4));
kodoz.new_plus(VV.data_ravno(5));
kgrz.new_plus(VV.data_ravno(6));
kodmz.new_plus(VV.data_ravno(7));
shuz.new_plus(VV.data_ravno(8));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_10.txt");
    clear();
    goto naz;

  case PLU:
  case FK2:
  case FK5:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kodoz,&naims);
        VV.VVOD_SPISOK_zapis_data(5,kodoz.ravno());
        goto naz;

      case 2 :
        vibrek("Grup",&kgrz,&naims);
        VV.VVOD_SPISOK_zapis_data(6,kgrz.ravno());
        goto naz;

      case 3 :
//        vibrek("Prihod",&kopz,&naims);
       if(dirprihod(1,&kopz,&naims) == 0)
         VV.data_z_plus(4,kopz.ravno());
        goto naz;

      case 4 :
        if(dirrashod(1,&kopz,&naims) == 0)
          VV.data_z_plus(4,kopz.ravno());
        goto naz;
        
      case 5 :
        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(7,kodm);
         }
        
        goto naz;

      case 6 :
        vibrek("Plansh",&shuz,&naims);
        VV.VVOD_SPISOK_zapis_data(8,shuz.ravno());
        goto naz;
               
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();    
    goto naz;

  default:
    goto naz;
 }
if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;
 
tipzz=0;
if(VV.VVOD_SPISOK_return_data(3)[0] != '\0' )
 {
  if(SRAV(VV.VVOD_SPISOK_return_data(3),"+",1) == 0)
    tipzz=1;
  if(SRAV(VV.VVOD_SPISOK_return_data(3),"-",1) == 0)
    tipzz=2;
  if(tipzz == 0)
   {
    iceb_t_soob(gettext("Не верно введён Приход/Расход !"));
    goto naz;
   }
 }

clear(); /*Очистить экран и закрасить фоновым цветом*/
GDITE();
time(&tmm);
bf=localtime(&tmm);

move(0,0);

printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Расчёт распределения НДС по видам операций\nза период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

/*Создаем массивы операций-материалов*/
sprintf(strsql,"select kod from Prihod");

//dlmp=dlmr=kolopp=kolopr=0;
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  spoprp.plus(row[0]);

sprintf(strsql,"select kod from Rashod");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  spoprr.plus(row[0]);
kolopp=spoprp.kolih();
kolopr=spoprr.kolih();

printw(gettext("Количество операций приходов - %d\n"),kolopp);
printw(gettext("Количество операций расходов - %d\n"),kolopr);
refresh();
if(spoprp.kolih() == 0 && spoprr.kolih() == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи кодов операций !"));
  return(1);
 }
//Маcсивы приходов
mkm1p.make_class(kolopp*6);

//*Маcсивы расходов
mkm1r.make_class(kolopr*6);
mkm1_1r.make_class(kolopr*6);
mkm1_1k.make_class(kolopr*6);


/*Просматриваем документы и заполняем массивы*/


sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' and \
datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа !"));
  return(1);
 }
 
sprintf(imaf,"op%d.lst",getpid());
sprintf(imaf1,"oh%d.lst",getpid());
sprintf(imaf2,"ohb%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

fprintf(ff1,"%s\n\n%s.\n",iceb_t_get_pnk("00",0),gettext("Контроль ошибок"));
fprintf(ff1,"\n\
--------------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
fprintf(ff1,"\
--------------------------------------------------------------------------\n");

fprintf(ff2,"%s\n\n%s.\n",iceb_t_get_pnk("00",0),gettext("Контроль ошибок"));
fprintf(ff2,"\n\
--------------------------------------------------------------------------\n");
fprintf(ff2,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
fprintf(ff2,"\
--------------------------------------------------------------------------\n");

fprintf(ff,"\n");

fprintf(ff,"%s\n%s %d.%d.%d%s %s %d.%d.%d%s\n",
iceb_t_get_pnk("00",0),
gettext("Расчёт распределения НДС по видам операций\nза период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

fprintf(ff,"%s %d.%d.%d%s %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(sklz.ravno()[0] != '\0')
 fprintf(ff,gettext("Склад - %s\n"),sklz.ravno());

if(tipzz != 0)
  fprintf(ff,gettext("Тип операции - %s\n"),VV.VVOD_SPISOK_return_data(3));

if(kopz.ravno()[0] != '\0')
 fprintf(ff,"%s - %s\n",gettext("Код операции"),kopz.ravno());

if(kodoz.ravno()[0] != '\0')
 fprintf(ff,gettext("Код контрагента - %s\n"),kodoz.ravno());

if(kgrz.ravno()[0] != '\0')
 fprintf(ff,gettext("Код группы - %s\n"),kgrz.ravno());

if(kodmz.ravno()[0] != '\0')
 fprintf(ff,gettext("Код материалла - %s\n"),kodmz.ravno());

if(shuz.ravno()[0] != '\0')
 fprintf(ff,gettext("Счета учета - %s\n"),shuz.ravno());
kolstr1=0;
itog=0.; 
SQLCURSOR cur2;
float pnds=0.;
while(cur.read_cursor(&row) != 0)
 {
/*Если документ не подтвержден то его не смотрим*/
//  printw("%s %s %s\n",row[1],row[2],row[4]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(atoi(row[7]) == 0)
    continue;

  tp=atoi(row[0]);
  skl=atoi(row[2]);
  //kor.new_plus(row[3]);
  kop.new_plus(row[6]);

  if(proverka(sklz.ravno(),row[2],0,0) != 0)
    continue;  

  if(tipzz != 0 && tipzz != tp)
     continue;

  if(proverka(kopz.ravno(),row[6],0,0) != 0)
    continue;  


  if(proverka(kodoz.ravno(),row[3],0,0) != 0)
    continue;  
  pnds=atof(row[13]);
  
  /*Читем код операции*/
  if(tp == 1)
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
  if(tp == 2)
    sprintf(strsql,"select vido from Rashod where kod='%s'",kop.ravno());
  vo=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw(gettext("Не найден код операции %s списке операций !"),kop.ravno());
    printw("\n");
    OSTANOV();
   }
  else
   vo=atoi(row1[0]);
  
  /*Читаем запис в накладной*/
  sprintf(strsql,"select * from Dokummat1 where datd='%s' and \
  sklad=%s and nomd='%s'",row[1],row[2],row[4]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    beep();
    printw(gettext("Нет записей в документе %s !\n"),row[4]);
    continue;
   }
   if(tp == 1)
    {
     if((nomop=spoprp.find(kop.ravno())) < 0 )
      {
       beep();
       printw(gettext("Не найден код операции %s в массиве операций приходов !\n"),kop.ravno());
       OSTANOV(); 
       continue;
      }
    }
   if(tp == 2)
    {
     if((nomop=spoprr.find(kop.ravno())) < 0 )
      {
       beep();
       printw(gettext("Не найден код операции %s в массиве операций расходов !\n"),kop.ravno());
       OSTANOV(); 
       continue;
      }
    }   
   rsdat(&d,&m,&g,row[1],2);    
   sumao=sumaok=0.;

    
   while(cur1.read_cursor(&row1) != 0)
    {
     if(proverka(kodmz.ravno(),row1[4],0,0) != 0)
       continue;
     nk=atoi(row1[3]);
     cena=atof(row1[6]);
     cena=okrug(cena,okrcn);
     vtr=atoi(row1[8]);     
     kolih=atof(row1[5]);

     
     /*Читаем карточку материалла*/
     if(nk > 0)
      {
       sprintf(strsql,"select cena,shetu from Kart where sklad=%d and nomk=%d",
       skl,nk);
       if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
        {
         beep();
         printw(gettext("Не нашли карточку %d склад %d !\n"),
         nk,skl);
         continue;
        }
      }
     cenak=atof(row2[0]);
     cenak=okrug(cenak,okrcn);
     shu.new_plus(row2[1]);

     if(cenak-cena > 0.009)
      {
       printw("%02d.%02d.%d %-*s %*s %10.10g %10.10g\n",
       d,m,g,
       iceb_tu_kolbait(11,row[4]),row[4],
       iceb_tu_kolbait(5,row1[4]),row1[4],
       cena,cenak);
       
       bb=kolih*cenak - kolih*cena;
       fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_tu_kolbait(11,row[4]),row[4],
       iceb_tu_kolbait(4,kop.ravno()),kop.ravno(),
       iceb_tu_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog+=bb;
      }
     if(cenak-cena < 0.009)
      {
       bb=kolih*cena - kolih*cenak;
       fprintf(ff2,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_tu_kolbait(11,row[4]),row[4],
       iceb_tu_kolbait(4,kop.ravno()),kop.ravno(),
       iceb_tu_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog1+=bb;
      }
     
     if(vo == 1)
      {
        if(fabs(cenak-cena) > 0.0009)
         {
          printw("%02d.%02d.%d %-11s %5s %10.10g %10.10g\n",
          d,m,g,row[4],row1[4],cena,cenak);
          bb=kolih*cenak - kolih*cena;
          fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
          d,m,g,
           iceb_tu_kolbait(11,row[4]),row[4],
           iceb_tu_kolbait(4,kop.ravno()),kop.ravno(),
           iceb_tu_kolbait(5,row1[4]),row1[4],
          kolih,cena,cenak,bb);
         }
      }

     sprintf(strsql,"select kodgr from Material where kodm=%s",row1[4]);
     if(readkey(strsql,&row2,&cur2) != 1)
      {
       beep();
       printw("%s %s !\n",gettext("Не найден материал"),row1[4]);
       OSTANOV();
      }

     if(proverka(kgrz.ravno(),row2[0],0,0) != 0)
       continue;  
     kgrm=atoi(row2[0]);


     if(proverka(shuz.ravno(),shu.ravno(),0,0) != 0)
       continue;  
  

     kol=readkolk(skl,nk,d,m,g,row[4]);

     suma=kol*cena;
     suma=okrug(suma,okrg1);
     sumao+=suma;
    
     if(tp == 1)       
     if(vtr == 1 || (kgrm == vtara && vtara != 0))
      {
       mkm1p.plus(suma,6*nomop+5);
      }

     if(tp == 2)       
      {
       sumak=kol*cenak;
       sumak=okrug(sumak,okrg1);
       sumaok+=sumak;
       /*Возвратная тара*/
       if(vtr == 1 || (kgrm == vtara && vtara != 0))
        {
         mkm1r.plus(suma,6*nomop+5);
         mkm1_1r.plus(suma,6*nomop+5);
        }
      }
    } 


/* Читаем как была оформлена накладная*/
   mnds=0;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=11",
   g,row[4],skl);

   if((i=sql_readkey(&bd,strsql,&row2,&cur2)) == 1)
    mnds=atoi(row2[0]);

/* Читаем сумму корректировки*/

   sumkor=0.;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
   g,row[4],skl);

   if((i=sql_readkey(&bd,strsql,&row2,&cur2)) == 1)
     sumkor=atof(row2[0]);
  
   if(kom == FK5)
    {
     printw("%2d %02d.%02d.%d %-*s ндс-%d %10.2f\n",
     skl,d,m,g,
     iceb_tu_kolbait(8,row[4]),row[4],
     mnds,sumao);
     refresh();
     fprintf(ff,"%2d %02d.%02d.%d %-*s ндс-%d %10.2f\n",
     skl,d,m,g,
     iceb_tu_kolbait(8,row[4]),row[4],mnds,sumao);
    } 
           
   if(tp == 1)
    {
     mkm1p.plus(sumao,6*nomop+mnds);
   
     if(mnds == 0)
      { 
       //Читаем вручную введенную сумму НДС
       sprintf(strsql,"select sodz from Dokummat2 \
 where god=%d and nomd='%s' and sklad=%d and nomerz=6",
       g,row[4],skl);
       if(readkey(strsql,&row2,&cur2) == 1)
         suma=atof(row2[0]);
       else
         suma=sumao*pnds/100.;   

       suma=okrug(suma,okrg1);
       mkm1p.plus(suma,6*nomop+4);
      }
    }
 
   if(tp == 2)
    {
      

     mkm1r.plus(sumao,6*nomop+mnds);
     mkm1_1r.plus(sumaok,6*nomop+mnds);
     mkm1_1k.plus(sumkor,6*nomop+mnds);
  
     if(mnds == 0)
      { 
       suma=sumao*pnds/100.;   
       suma=okrug(suma,okrg1);
       mkm1r.plus(suma,6*nomop+4);

       sumak=sumaok*pnds/100.;
       sumak=okrug(sumak,okrg1);

       mkm1_1r.plus(sumak,6*nomop+4);
       mkm1_1k.plus(sumkor,6*nomop+4);
      }
    }

 }

fprintf(ff1,"%*s: %10.10g\n",iceb_tu_kolbait(61,gettext("Итого")),gettext("Итого"),itog);
fprintf(ff2,"%*s: %10.10g\n",iceb_tu_kolbait(61,gettext("Итого")),gettext("Итого"),itog1);

printw(gettext("Распечатываем\n"));
GDITE();

/*Распечатываем результат*/
rasmasop(&spoprp,&mkm1p,NULL,NULL,1,ff);
rasmasop(&spoprr,&mkm1r,&mkm1_1r,&mkm1_1k,2,ff);

podpis(ff);
podpis(ff1);
podpis(ff2);


fclose(ff);
fclose(ff1);
fclose(ff2);

suma=sumao=0.;

printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распределение НДС по видам операций"));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Контроль ошибок по документам"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Контроль ошибок по записям в документах"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}
/********************/
/*Распечатка массивов*/
/*********************/
void		rasmasop(SPISOK *spopr,masiv_din_double *mkm,
masiv_din_double *mkm1_1r,
masiv_din_double *mkm1_1k,
short tip,FILE *ff)
{
double		suma;
double          its,it1,it2,it3,it4,it5,it6;
double          itsu,it1u,it2u,it3u,it4u,it5u,it6u;
double          itsk,it1k,it2k,it3k,it4k,it5k,it6k;
short		i;
class iceb_tu_str naimo("");
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];
class iceb_tu_str bros("");
short kolopr=spopr->kolih();

float pnds=iceb_t_pnds();
suma=0.;
suma=mkm->suma();

it1=it2=it3=it4=it5=it6=0.;
it1u=it2u=it3u=it4u=it5u=it6u=0.;
it1k=it2k=it3k=it4k=it5k=it6k=0.;
if(suma > 0.009)
 {  
  if(tip == 1)
   {
    printw("%s\n",gettext("Приходы"));
    fprintf(ff,"%s\n",gettext("Приходы"));
   }
  if(tip == 2)
   {
    printw("%s\n",gettext("Расходы"));
    fprintf(ff,"%s\n",gettext("Расходы"));
   }
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Опер.|   Наименование операции       |      20%% НДС       |0%% Украина|0%% Експорт|0%% Cт.5   |Оборотная | Итого    |\n"));
fprintf(ff,gettext("     |                               | С у м м а|   НДС   |          |          |          |  тара    |          |\n"));

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

  for(i=0; i < kolopr ;i++)
   {
    bros.new_plus(spopr->ravno(i));
    if(bros.ravno()[0] == '\0')
     break;
    its=mkm->ravno(i*6)+mkm->ravno(i*6+1)+mkm->ravno(i*6+2)+mkm->ravno(i*6+3)+mkm->ravno(i*6+4)+mkm->ravno(i*6+5);
    if(its < 0.01)
     continue;      

    if(tip == 1)
      sprintf(strsql,"select naik from Prihod where kod='%s'",bros.ravno());
    if(tip == 2)
      sprintf(strsql,"select naik from Rashod where kod='%s'",bros.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      beep();
      if(tip == 1)
       {
        printw(gettext("Не нашли приход %s !"),bros.ravno());
        printw("\n");
        refresh();
        fprintf(ff,gettext("Не нашли приход %s !"),bros.ravno());
        fprintf(ff,"\n");
        OSTANOV();
        continue;
       } 
      if(tip == 2)
       {
        printw(gettext("Не нашли расход %s !"),bros.ravno());
        printw("\n");
        refresh();
        fprintf(ff,gettext("Не нашли расход %s !"),bros.ravno());
        fprintf(ff,"\n");
        OSTANOV();
        continue;
       } 
     }
    naimo.new_plus(row[0]);
    it1+=mkm->ravno(i*6);
    it2+=mkm->ravno(i*6+1);
    it3+=mkm->ravno(i*6+2);
    it4+=mkm->ravno(i*6+3);
    it5+=mkm->ravno(i*6+4);
    it6+=mkm->ravno(i*6+5);


    fprintf(ff,"%-*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    iceb_tu_kolbait(5,bros.ravno()),bros.ravno(),
    iceb_tu_kolbait(30,naimo.ravno()),iceb_tu_kolbait(30,naimo.ravno()),naimo.ravno(),
    mkm->ravno(i*6),mkm->ravno(i*6+4),mkm->ravno(i*6+1),mkm->ravno(i*6+2),
    mkm->ravno(i*6+3),mkm->ravno(i*6+5),its);

    if(tip == 2)
     {
      itsu=mkm1_1r->ravno(i*6)+mkm1_1r->ravno(i*6+1)+mkm1_1r->ravno(i*6+2)+mkm1_1r->ravno(i*6+3)+mkm1_1r->ravno(i*6+4)+mkm1_1r->ravno(i*6+5);
      it1u+=mkm1_1r->ravno(i*6);
      it2u+=mkm1_1r->ravno(i*6+1);
      it3u+=mkm1_1r->ravno(i*6+2);
      it4u+=mkm1_1r->ravno(i*6+3);
      it5u+=mkm1_1r->ravno(i*6+4);
      it6u+=mkm1_1r->ravno(i*6+5);
      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_tu_kolbait(30,gettext("В учётных ценах")),iceb_tu_kolbait(30,gettext("В учётных ценах")),gettext("В учётных ценах"),
      mkm1_1r->ravno(i*6),mkm1_1r->ravno(i*6+4),mkm1_1r->ravno(i*6+1),mkm1_1r->ravno(i*6+2),
      mkm1_1r->ravno(i*6+3),mkm1_1r->ravno(i*6+5),itsu);

      itsk=mkm1_1k->ravno(i*6)+mkm1_1k->ravno(i*6+1)+mkm1_1k->ravno(i*6+2)+mkm1_1k->ravno(i*6+3)+\
      mkm1_1k->ravno(i*6+4)*pnds/100.+mkm1_1k->ravno(i*6+5);
      it1k+=mkm1_1k->ravno(i*6);
      it2k+=mkm1_1k->ravno(i*6+1);
      it3k+=mkm1_1k->ravno(i*6+2);
      it4k+=mkm1_1k->ravno(i*6+3);
      it5k+=mkm1_1k->ravno(i*6+4)*pnds/100.;
      it6k+=mkm1_1k->ravno(i*6+5);
      
      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_tu_kolbait(30,gettext("Суммы коректировок")),iceb_tu_kolbait(30,gettext("Суммы коректировок")),gettext("Суммы коректировок"),
      mkm1_1k->ravno(i*6),mkm1_1k->ravno(i*6+4)*pnds/100.,mkm1_1k->ravno(i*6+1),mkm1_1k->ravno(i*6+2),
      mkm1_1k->ravno(i*6+3),mkm1_1k->ravno(i*6+5),itsk);
     }
   }
  its=it1+it5+it2+it3+it4+it6;
  itsk=it1k+it5k+it2k+it3k+it4k+it6k;
  itsu=it1u+it5u+it2u+it3u+it4u+it6u;

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_tu_kolbait(30,gettext("Итого")),iceb_tu_kolbait(30,gettext("Итого")),gettext("Итого"),
  it1,it5,it2,it3,it4,it6,its);
  if(tip == 2)
   {
    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    " ",
    iceb_tu_kolbait(30,gettext("Итого корректировок")),iceb_tu_kolbait(30,gettext("Итого корректировок")),gettext("Итого корректировок"),
    it1k,it5k,it2k,it3k,it4k,it6k,itsk);

    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    " ",
    iceb_tu_kolbait(30,gettext("Итого в учётных ценах")),iceb_tu_kolbait(30,gettext("Итого в учётных ценах")),gettext("Итого в учётных ценах"),
    it1u,it5u,it2u,it3u,it4u,it6u,itsu);

    fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    " ",
    iceb_tu_kolbait(30,gettext("Разница")),iceb_tu_kolbait(30,gettext("Разница")),gettext("Разница"),
    it1+it1k-it1u,
    it5+it5k-it5u,
    it2+it2k-it2u,
    it3+it3k-it3u,
    it4+it4k-it4u,
    it6+it6k-it6u,
    its+itsk-itsu);

   }
 }
}
