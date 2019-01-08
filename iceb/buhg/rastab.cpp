/* $Id: rastab.c,v 5.34 2013/09/26 09:43:39 sasa Exp $ */
/*28.03.2018    30.12.1995      Белых А.И.      rastab.c
Расчёт свода отработанного времени по работникам.
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

void saptbn(short,class iceb_tu_str*,long,long,long,class masiv_din_double*,class masiv_din_int *mkt,int,FILE*);
void rasvtn(short,short,short,short,short,short,short,class masiv_din_double*,class masiv_din_int*,class masiv_din_int *mkt,int,FILE*);

extern short    mmm,ggg;

int rastab(class spis_oth *oth)
{
struct  tm      *bf;
time_t          tmm;
short		par;
short		mn,gn,mk,gk;
short		kolvt;  /*Колличество видов табеля*/
long		koltn; /*Количество табельных номеров*/
static class iceb_tu_str tabnom("");
static class iceb_tu_str podr("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kod_tab("");
char		imaf[64],imafn[64];;
FILE		*ff;
int		i,tabn;
int		i1;
int kom1=0;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		m,g;
float		dnei,has,kdnei;
short		vt;
class iceb_tu_str naim("");
class VVOD MENU(3);
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_tab.ravno(),128);


VV.VVOD_SPISOK_add_MD(gettext("Начальная дата...(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата....(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код табеля........(,,).."));//4

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода отработанного времени по работникам"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
kod_tab.new_plus(VV.data_ravno(4));

switch(par)
 {
  case FK1:
   GDITE();
   iceb_t_pdoc("zarp4_3_5.txt");
   clear();
   goto naz;

  case FK10:
  case ESC:
    return(1);
  case FK2:
  case PLU:
    break;   

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов табеля"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :

        if(dirtab(&tabn,&naim,0,1) == 0)
          tabnom.z_plus(tabn);       
        VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
        goto naz;

      case 2 :
        vibrek("Tabel",&kod_tab,&naim);
        VV.VVOD_SPISOK_zapis_data(4,kod_tab.ravno());
        goto naz;


     }

    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(dat2.ravno()[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }

time(&tmm);
bf=localtime(&tmm);


clear();
printw("%s %d.%d%s %s %d.%d%s %s\n",
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
podr.ravno());
GDITE();

/*Создаем массив видов табеля*/
sprintf(strsql,"select kod from Tabel");
SQLCURSOR cur;
SQLCURSOR curr;
if((kolvt=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolvt == 0)
 {
  iceb_t_soob(gettext("Не введены виды табеля !"));
  return(1);
 }

//short	mkt[kolvt]; /*Массив кодов табелей*/
class masiv_din_int mkt;
mkt.make_class(kolvt);
i=0;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kod_tab.ravno(),row[0],0,0) != 0)
   continue;
  mkt.plus(atoi(row[0]),i++);

 }

/*Создаем файл табельных номеров для последующей сортировки*/
if(gn != gk)
  sprintf(strsql,"select distinct tabn from Ztab where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d) order by tabn asc",
gn,gk,
gn,mn,
gk,mk);
else
  sprintf(strsql,"select distinct tabn from Ztab where \
god = %d and mes >= %d and mes <= %d order by tabn asc",
gn,mn,mk);
/*
printw("%s\n",strsql);
refresh();
*/
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

class masiv_din_int mtn;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;

  tabn=atoi(row[0]);

  mtn.plus(atoi(row[0]),-1);
 }
koltn=mtn.kolih();

printw("%s %ld\n",gettext("Количество табельных номеров"),koltn);

if(koltn == 0)
 {
  OSTANOV();
  return(1);
 } 
/*Открывем массив для видов табелей*/
class masiv_din_double mtab;
mtab.make_class(koltn*kolvt*3);

/*Заряжаем массив табельных номеров*/


/*Заряжаем массив видов табелей*/
if(gn != gk)
  sprintf(strsql,"select tabn,kodt,dnei,has,kdnei,god,mes from Ztab where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d)",
  gn,gk,
  gn,mn,
  gk,mk);
else
  sprintf(strsql,"select tabn,kodt,dnei,has,kdnei,god,mes from Ztab where \
god = %d and mes >= %d and mes <= %d",
  gn,mn,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(proverka(kod_tab.ravno(),row[1],0,0) != 0)
   continue;

  tabn=atoi(row[0]);

  if(podr.ravno()[0] != '\0')
   {
    m=atoi(row[6]);
    g=atoi(row[5]);

    sprintf(strsql,"select podr from Zarn where god=%d and mes=%d and tabn=%d",g,m,tabn);

    if(readkey(strsql,&row1,&curr) != 1)
       continue;

    if(proverka(podr.ravno(),row1[0],0,0) != 0)
      continue;
   }
   
  vt=atoi(row[1]);
  dnei=atof(row[2]);
  has=atof(row[3]);
  kdnei=atof(row[4]);
  
  if((i=mtn.find(tabn)) < 0)
   {

    beep();
    printw(gettext("Не найден табельный номер %d в массиве табельных номеров !"),tabn);
    printw("\n");
    OSTANOV();
    continue;
   }         
  for(i1=0; i1 < kolvt; i1++)
    if(vt == mkt.ravno(i1))
      break;

  if(i1 == kolvt)
   {
    beep();
    printw(gettext("Не найден код табеля %d в массиве видов табелей !"),vt);
    printw("\n");
    OSTANOV();
    continue;
   }         
  mtab.plus(dnei,i*kolvt*3+i1*3);
  mtab.plus(has,i*kolvt*3+i1*3+1);
  mtab.plus(kdnei,i*kolvt*3+i1*3+2);
 }

sprintf(imaf,"vtn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

short dk=0;
dpm(&dk,&mk,&gk,5);
zagolov(gettext("Свод видов табеля"),bf,1,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);

if(podr.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Подразделение"),podr.ravno());
if(tabnom.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Табельный номер"),tabnom.ravno());
if(kod_tab.getdlinna() > 1)
  fprintf(ff,"%s : %s\n",gettext("Код табеля"),kod_tab.ravno());
  
rasvtn(1,mn,gn,dk,mk,gk,kolvt,&mtab,&mtn,&mkt,0,ff);

podpis(ff);
fclose(ff);

//Распечатываем с начислениями
sprintf(imafn,"vtnn%d.lst",getpid());
if((ff = fopen(imafn,"w")) == NULL)
 {
  error_op_nfil(imafn,errno,"");
  return(1);
 }

zagolov(gettext("Свод видов табеля"),bf,1,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);

if(podr.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Подразделение"),podr.ravno());
if(tabnom.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Табельный номер"),tabnom.ravno());
if(kod_tab.getdlinna() > 1)
  fprintf(ff,"%s : %s\n",gettext("Код табеля"),kod_tab.ravno());
   
rasvtn(1,mn,gn,dk,mk,gk,kolvt,&mtab,&mtn,&mkt,1,ff);

podpis(ff);
fclose(ff);


printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод видов табеля"));
oth->spis_imaf.plus(imafn);
oth->spis_naim.plus(gettext("Свод по видам табеля с дополнительной информацией"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}

/*********************************/
/*Расапечатка результатов расчёта*/
/*********************************/

void rasvtn(short dn,short mn,short gn,
short dk,short mk,short gk,
short kka, //Количество видов табелей
class masiv_din_double *nao,
class masiv_din_int *tn, //Массив табельных номеров
class masiv_din_int *mkt,
int metkar, //0-без начислений 1-с начислениями
FILE *kaw)
{
int  koltb=tn->kolih(); //Количество табельных номеров
long		i,i1,i2,ii,skn;
short           kkol=12; /*Количество колонок в распечатке*/
double		br1,br2;
short           korr;
double          sn1,sn2,sn3;
class iceb_tu_str st("");
unsigned short	porn; /*Порядковый номер*/
double		it1,it2,it3;
class iceb_tu_str fio("");
SQL_str         row1;
char		strsql[512];
class iceb_tu_str dolg("");
double		suman=0.;
class iceb_tu_str kodpodr("");
class iceb_tu_str naimpodr("");
SQLCURSOR curr;
if(metkar == 1)
 kkol=9999;

/*Распечатываем*/
printw("%s %d\n-------------------------\n",
gettext("Распечатываем"),metkar);
refresh();
porn=korr=0;
sn1=sn2=sn3=0.;
for(ii=0 ; ii < kka; ii += kkol)
 {
  /*printw("ii-%d kka-%d korr-%d\n",ii,kka,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii*3; i<ii*3+kkol+i2 && i < kka*3;i+=3)
   {
    br1=0.;
    for(skn=i;skn< kka*3*koltb+ii*3+kkol && skn<kka*3*koltb; skn+=kka*3)
      if(fabs((br1=nao->ravno(skn)+nao->ravno(skn+1)+nao->ravno(skn+2))) > 0.009)
        break;
    if(fabs(br1) < 0.01)
     i2++;
    br2+=br1;
   }
  if(fabs(br2) < 0.01)
    continue;
  
  saptbn(kka,&st,ii,ii+kkol,koltb,nao,mkt,metkar,kaw);
  br1=0.;
  for(i=0 ; i<koltb;i++)
   {
    /*Проверяем если нет равных нолю позиций печатаем*/
    br1=0.;
    for(skn=0;skn< kka*3 ; skn++)
      if(fabs(nao->ravno(skn+i*kka*3)) > 0.009)
        break;

    if(fabs(nao->ravno(skn+i*kka*3)) < 0.01)
      continue;

    naimpodr.new_plus("");
    kodpodr.new_plus("");
    
    sprintf(strsql,"select fio,dolg,podr from Kartb where tabn=%d",tn->ravno(i));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Не найден табельный номер"),tn->ravno(i));
      OSTANOV();
      continue;
     }
       
    fio.new_plus(row1[0]);
    dolg.new_plus(row1[1]);
    kodpodr.new_plus(row1[2]);

    if(metkar == 1) //Читаем должность и сумму начислений
     {
      sprintf(strsql,"select dolg,podr from Zarn where tabn=%d and god=%d and mes=%d",
      tn->ravno(i),gn,mn);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
       {
        dolg.new_plus(row1[0]);
        kodpodr.new_plus(row1[1]);
       }
      sprintf(strsql,"select naik from Podr where kod=%s",kodpodr.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        naimpodr.new_plus(row1[0]);
      
      sprintf(strsql,"%d",tn->ravno(i));
      suman=sosnsum(strsql,dn,mn,gn,dk,mk,gk);
     }

    porn++;
    if(metkar == 0)
     fprintf(kaw,"%4d %5d %-*.*s",porn,tn->ravno(i),iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno());
    if(metkar == 1)
     fprintf(kaw,"%4d %5d %-*.*s %-*.*s %-3s %-*.*s %10.2f",
     porn,tn->ravno(i),
     iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
     iceb_tu_kolbait(20,dolg.ravno()),iceb_tu_kolbait(20,dolg.ravno()),dolg.ravno(),
     kodpodr.ravno(),
     iceb_tu_kolbait(20,naimpodr.ravno()),
     iceb_tu_kolbait(20,naimpodr.ravno()),
     naimpodr.ravno(),
     suman);

    /*Предыдущая информация*/
    it1=it2=it3=0.;
    if(ii > 0 )
     {
      for(skn=i*kka*3; skn<i*kka*3+ii*3 && skn < kka*3*koltb ;skn+=3)
       {
	/*printw(" skn=%d",skn);*/
	 it1+=nao->ravno(skn);
	 it2+=nao->ravno(skn+1);
	 it3+=nao->ravno(skn+1);
       }
     }

    i2=0;
    for(skn=i*kka*3+ii*3; skn<i*kka*3+ii*3+kkol*3+i2*3 && skn < i*kka*3+kka*3 ;skn+=3)
     {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kka*3); i1< kka*3*koltb ; i1+=kka*3)
	if(fabs((br2+=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
	  break;

      if(fabs(br2) > 0.01)
       {
	fprintf(kaw," %5.5g %7.7g %5.5g",nao->ravno(skn),nao->ravno(skn+1),nao->ravno(skn+2));
        it1+=nao->ravno(skn);
        it2+=nao->ravno(skn+1);
        it3+=nao->ravno(skn+2);
       }
      else
       {
	i2++;
       }

     }
/*    printw("skn=%d i=%d kka=%d  %d koltb=%ld\n",skn,i,kka,i*kka*3+kka*3-1-8,koltb);*/
    if(skn == (i+1)*kka*3)
     {
      fprintf(kaw," %5.5g %7.7g %5.5g\n",it1,it2,it3);
     }
    else
      fprintf(kaw,"\n");
   }

  fprintf(kaw,"%s\n",st.ravno());
  if(metkar == 0)
    fprintf(kaw,"%-*s",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"));
  if(metkar == 1)
    fprintf(kaw,"%-*s",iceb_tu_kolbait(98,gettext("Итого")),gettext("Итого"));
  porn=0;
  i2=0;
  /*printw("kkol=%d ii=%d kka=%d koltb=%d\n",kkol,ii,kka,koltb);*/
  for(i=ii; i<ii+kkol+i2 && i < kka ;i++)
   {
    it1=it2=it3=0.;
/*    for(skn=i*3;skn < kka*3*(koltb-1)+ii*3+kkol+i2 && skn< kka*3*koltb ; skn+=kka*3)*/
    for(skn=i*3; skn < kka*3*koltb ; skn+=kka*3)
     {
      it1+=nao->ravno(skn);
      it2+=nao->ravno(skn+1);
      it3+=nao->ravno(skn+2);
     }

    if(fabs(it1+it2+it3) > 0.01)
      fprintf(kaw," %5.5g %7.7g %5.5g",it1,it2,it3);
    else
      i2++;

    sn1+=it1;
    sn2+=it2;
    sn3+=it3;
   }
  if(i == kka)
   {
    fprintf(kaw," %5.5g %7.7g %5.5g\n",sn1,sn2,sn3);
   }
  else
   {
    fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st.ravno());

  if(i == kka)
   break;
  ii+=i2;
 }

//printw("Розпечать завершено.\n");
refresh();

}

/***************/
/*Шапка таблицы*/
/***************/
void saptbn(short kka, //Количество видов табелей
class iceb_tu_str *st,
long pn, //Позиция начала
long pk, //Позиция конца
long konu, //Количество людей
class masiv_din_double *nao,
class masiv_din_int *ka, //Массив видов табелей
int metkar, //0-без начислений 1-с начислениями
FILE *kaw)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;
//printw("Печатьуем шапку\n");

/*printw("kka=%d pn=%d pk=%d konu=%ld\n",kka,pn,pk,konu);*/

refresh();

/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
if(metkar == 0)
  st->new_plus("------------------------------------------");
if(metkar == 1)
  st->new_plus("---------------------------------------------------------------------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1< kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;

  if(fabs(br2) > 0.01 )
    st->plus("--------------------");
  else
   i2++;
 }


/*Полка над ИТОГО*/
if(i == kka)
  st->plus("--------------------");

fprintf(kaw,"\n%s\n",st->ravno());
if(metkar == 0)
 fprintf(kaw,gettext("Н/п | Т/н | Фамилия Имя Отчество         "));
if(metkar == 1)
 fprintf(kaw,gettext("Н/п | Т/н | Фамилия Имя Отчество         |    Должность       |К.п|Наим. подразделения |   Сумма  "));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1 < kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;
  if(fabs(br2) < 0.01)
   {
    i2++;
    continue;
   }
  sprintf(strsql,"select naik from Tabel where kod=%d",ka->ravno(i));
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден код табеля"),ka->ravno(i-1));
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);
  fprintf(kaw,"| %-*.*s",iceb_tu_kolbait(18,nai.ravno()),iceb_tu_kolbait(18,nai.ravno()),nai.ravno());
 }

if(i == kka)
  fprintf(kaw,"| %-*.*s|\n",iceb_tu_kolbait(18,gettext("Итого")),iceb_tu_kolbait(18,gettext("Итого")),gettext("Итого"));
else
  fprintf(kaw,"|\n");

if(metkar == 0)
  fprintf(kaw,"    |     |%-30s"," ");
if(metkar == 1)
  fprintf(kaw,"    |     |%-30s|%20s|%3s|%20s|%10s"," "," "," "," "," ");

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1< kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;
  if(fabs(br2) > 0.01)
   {
    fprintf(kaw,"| %-18d",ka->ravno(i));
   }
  else
    i2++;
 }

if(i == kka)
  fprintf(kaw,"| %-18.17s|\n"," ");
else
  fprintf(kaw,"|\n");

fprintf(kaw,"%s\n",st->ravno());

}
