/* $Id: svodnu.c,v 5.54 2013/09/26 09:43:41 sasa Exp $ */
/*28.03.2018    23.11.1992      Белых А.И.      svodnu.c
Подпрограмма получения всех начислений и удержаний по фамилиям
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

void            sappknu(long,class iceb_tu_str*,long,long,long,class masiv_din_int*,int,class masiv_din_double*,FILE*);
void		rasmesn(short,short,short,short,short,class masiv_din_int*,class masiv_din_double*,char[],class masiv_din_int *tabn,int,const char*,const char*,const char*);
void		rasmesn1(short,short,short,class iceb_tu_str*,FILE*);
void svodnu_sort(class masiv_din_int *tabn,int koltb);
void schnu1(short mn,short gn,short mk,short gk,short dia,const char *kp,const char *tabnom,const char *nahis,const char *uder,
const char *kategor,const char *shet,char *imaf2,int kkol,class masiv_din_int *tn,int koltb,class masiv_din_int *nah_ud,int kon,
class masiv_din_double *nao,FILE *kaw,FILE *ff);

extern short    mmm,ggg;

int svodnu(class spis_oth *oth)
{
int           kon;  /*Количество статей начисления*/
int           kou;  /*Количество статей удержания*/
int             koltb=0; /*Количество табельных номеров*/
struct  tm      *bf;
char            imaf[64],imaf1[64],imaf2[64];
int             i;
time_t          tmm;
short           mn,gn,mk,gk;
short           rkka;
SQL_str         row;
char		strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kp("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str nahis("");
static class iceb_tu_str uder("");
static class iceb_tu_str kategor("");
static class iceb_tu_str shet("");
static char     maxkol[4];
int		kom1;
int             kkol=16; /*Количество колонок в распечатке*/
int tmmm=0;
class iceb_tu_str fiov("");

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),8); 
VV.VVOD_SPISOK_add_data(dat2.ravno(),8); 
VV.VVOD_SPISOK_add_data(kp.ravno(),128); 
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128); 
VV.VVOD_SPISOK_add_data(nahis.ravno(),128); 
VV.VVOD_SPISOK_add_data(uder.ravno(),128); 
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(kategor.ravno(),128); 
VV.VVOD_SPISOK_add_data(shet.ravno(),128); 

if(maxkol[0] == '\0')
 VV.data_plus(6,kkol);
else
 VV.data_plus(6,maxkol); 

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и удержаний по работникам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала периода..(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца периода...(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения....(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер......(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.......(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код удержания........(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Мак. кол. колонок в отчёте.."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории........(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.................(,,)..."));

naz:;

clear();



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

rkka=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kp.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
nahis.new_plus(VV.data_ravno(4));
uder.new_plus(VV.data_ravno(5));
strncpy(maxkol,VV.VVOD_SPISOK_return_data(6),sizeof(maxkol)-1);
kkol=atoi(maxkol);
kategor.new_plus(VV.data_ravno(7));
shet.new_plus(VV.data_ravno(8));

class iceb_tu_str kodv("");
class iceb_tu_str naim("");
switch(rkka)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_6.txt");
   clear();
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка удержаний"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр плана счетов"));
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
        if(dirzarpodr(2,&kp,&naim) == 0)
         VV.data_z_plus(2,kp.ravno());
        goto naz;

      case 1 :
        if(dirnach(1,&nahis,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;

      case 2 :
        if(diruder(1,&kodv,&naim) == 0)
         uder.z_plus(kodv.ravno());
         
        VV.VVOD_SPISOK_zapis_data(5,uder.ravno());
        goto naz;
      
      case 3 :
      
        fiov.new_plus("");
        if(dirtab(&tmmm,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tmmm);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
         }
        goto naz;

      case 4 :
        vibrek("Kateg",&kategor);
        VV.VVOD_SPISOK_zapis_data(7,kategor.ravno());
        goto naz;

      case 5 :
        vibrek("Plansh",&kodv);
        VV.VVOD_SPISOK_zapis_data(8,kodv.ravno());
        goto naz;

     }  

  case FK4: /*Очистить меню*/
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

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по табельным номерам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по фамилии"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int sort=0;
while(menu3w(stdscr,&MENU,&sort,-1,-1,0) != 0);
if(sort == -1 || sort == 2 )
  goto naz;


time(&tmm);
bf=localtime(&tmm);

clear(); /*Очистить экран и закрасить фоновым цветом*/
printw("%s %d.%d %s %d.%d\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk);
GDITE();
/*Определяем количество начислений и удержаний*/

kon=kou=0;
sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kon == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);
 }
class masiv_din_int nah_ud;

while(cur.read_cursor(&row) != 0)
  nah_ud.plus(atoi(row[0]),-1);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kou == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  nah_ud.plus(atoi(row[0]),-1);

printw("%s:%d\n",
gettext("Количество начислений и удержаний"),nah_ud.kolih());
refresh();

/*Узнаем количество людей*/
if(gn != gk)
   sprintf(strsql,"select distinct tabn from Zarn where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d) order by tabn asc",
gn,gk,gn,mn,gk,mk);
else
   sprintf(strsql,"select distinct tabn from Zarn where god=%d and \
mes >= %d and mes <= %d order by tabn asc",gn,mn,mk);

if((koltb=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(koltb == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
 }
 
printw("%s\n",gettext("Вычисляем количество работников."));
refresh();

class masiv_din_int tn;
tn.make_class(koltb);


i=0;
while(cur.read_cursor(&row) != 0)
  tn.plus(atoi(row[0]),i++);

if(sort == 1)
 svodnu_sort(&tn,koltb);

printw("%s:%d\n",gettext("Количество табельных номеров"),koltb);
refresh();

class masiv_din_double nao;
nao.make_class((nah_ud.kolih()+3)*koltb);

memset(imaf2,'\0',sizeof(imaf2));
sprintf(imaf,"nu%d.lst",getpid());
FILE *kaw;
if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
sprintf(imaf1,"nuf%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }



fprintf(ff,"%s\n\n\
	     	    %s\n\
 		    %s 1.%d.%d%s %s 31.%d.%d%s\n\
		    %s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Свод начислений и удержаний"),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" П/н| Т/н |Фамилия , Имя , Отчество      |Сальдо на нач.|  Начислено   |   Удержано   |Сальдо конечное|\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(kaw,"%s\n\n\
	     	    %s\n\
		    %s 1.%d.%d%s %s 31.%d.%d%s\n\
		    %s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Свод начислений и удержаний."),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kp.ravno()[0] == '\0')
 fprintf(kaw,"%s\n",gettext("По организации"));
else
  fprintf(kaw,"%s%s\n",gettext("По подразделению:"),kp.ravno());
if(tabnom.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Табельные номера вошедшие в расчёт"),tabnom.ravno());
if(nahis.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Коды начислений вошедшие в расчёт"),nahis.ravno());
if(uder.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Коды удержаний вошедшие в расчёт"),uder.ravno());
if(kategor.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kategor.ravno());
if(shet.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Счёт"),shet.ravno());

schnu1(mn,gn,mk,gk,0,kp.ravno(),tabnom.ravno(),nahis.ravno(),uder.ravno(),kategor.ravno(),shet.ravno(),imaf2,kkol,&tn,koltb,&nah_ud,kon,&nao,kaw,ff);

podpis(kaw);
podpis(ff);

fclose(kaw);
fclose(ff);


printw_vr(tmm);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод начислений и удержаний."));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Общие итоги"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Свод начислений по месяцам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);
}


/********/
/*Поиск */
/********/
void schnu1(short mn,short gn,short mk,short gk,short dia,
const char *kp, //Коды подразделений
const char *tabnom, //Табельные номера
const char *nahis, //Коды начислений
const char *uder, //Коды удержаний
const char *kategor, //коды категорий
const char *shet,
char *imaf2,
int kkol,
class masiv_din_int *tn,
int koltb,
class masiv_din_int *nah_ud,
int kon, //количество начислений
class masiv_din_double *nao,
FILE *kaw,
FILE *ff)
{
short		kolm; /*Количество месяцев в периоде*/
class masiv_din_int mdat; /*массив дат*/
class masiv_din_double mnah; /*Массив начислений*/
unsigned short	porn; /*Порядковый номер*/
int             i,i1,i2;
double          br1,br2;
class iceb_tu_str st("");
long            skn;
int             kkk=0;
double          sn;
long            ii;
short           m,g;
double		it1,it2,it3,it4;
short		dd,mm,gg;
short		prn,knah;
double		sym;
class iceb_tu_str fio("");
int		tbn;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[1024];
short		metka;
double		saldb;

kolm=period(1,mn,gn,1,mk,gk,1);
if(kolm > 0)
 {
  mnah.make_class(kolm*koltb);
  mdat.make_class(kolm*2);

  m=mn;
  i=0;
  for(g=gn; g<=gk ; g++)
   for( ;     ; m++)
    {
     if(g == gk && m > mk)
       break;
     if(m == 13)
      {
       m=1;
       break;
      }
     mdat.plus(m,i);
     mdat.plus(g,i+1);
     i+=2;
    }

 }


porn=0;
g=gn;
it1=it2=it3=it4=0.;
int kka=nah_ud->kolih();
for(m=mn; ; m++)
 {
  if(m==13)
   {
    m=1;
    g++;
   }
  if(g>gk)
    break;
  if(g == gk && m > mk)
    break;
  GDITE();  
  printw("%d.%d%s\n",m,g,gettext("г."));
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    break;
   }

  if(kolstr == 0)
    continue;

    
//  mnc=katt=0;
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    strzag(LINES-1,0,kolstr,++kolstr1);

    if(proverka(kp,row[1],0,0) != 0)
      continue;
    if(proverka(tabnom,row[0],0,0) != 0)
      continue;
    if(proverka(kategor,row[2],0,0) != 0)
      continue;

    tbn=atoi(row[0]);
    skn=0;
    for(i=0 ; i < koltb ; i++)
     if(tn->ravno(i) == tbn)
      {
       skn=i;
       break;
      }

    if(i == koltb)
     {
      beep();
      printw(gettext("Не нашли табельный номер %d в массиве !"),tbn);
      printw("\n");
      OSTANOV();
      continue;
     }
/*
    if(dia == 0)
     {
      printw("%d.%d %d %d %ld- %d\n",
      zr7.ms,zr7.gd,zr7.kpd,zr7.kkat,zr7.tbn,zr7.shet);
     }
  */

    /*Читаем сальдо*/
    if(m == mn && g == gn)
      nao->plus(zarsald(1,m,g,tbn,&saldb),skn*(kka+3));

    sprintf(strsql,"select datz,prn,knah,suma,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d and suma <> 0. order by prn,knah asc",
    g,m,g,m,tbn);
    SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    if(kolstr2 == 0)
     {
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      prn=atoi(row1[1]);
      if(prn == 1 && nahis[0] == '-')
         continue;

      if(prn == 1 && nahis[0] != '\0')
       if(proverka(nahis,row1[2],0,0) != 0)
         continue;

      if(prn == 2 && uder[0] == '-')
         continue;

      if(prn == 2 && uder[0] != '\0')
       if(proverka(uder,row1[2],0,0) != 0)
         continue;

      if(proverka(shet,row1[4],1,0) != 0)
       continue;
              
      rsdat(&dd,&mm,&gg,row1[0],2);
      knah=atoi(row1[2]);
      sym=atof(row1[3]);     

      if(prn == 1)
       {
	for(i=0 ; i < kon ; i++)
	  if(nah_ud->ravno(i) == knah)
	   {
	    kkk=i+1; /*С учетом колонки сальдо*/
	    break;
	   }
	nao->plus(sym,(skn*(kka+3))+kon+1);
        if(kolm > 0)
         {
          for(i=0 ; i < kolm; i++)
            if(mdat.ravno(i*2) == mm && mdat.ravno(i*2+1) == gg)
             {
              mnah.plus(sym,skn*kolm+i);
              break;
             }
         }
       }
      if(prn == 2)
       {
	for(i=kon; i < kka ; i++)
	  if(nah_ud->ravno(i) == knah)
	   {
	    kkk=i+2; /*С учетом колонки сальдо и итого начислено*/
	    break;
	   }
	nao->plus(sym,(skn*(kka+3))+kka+2);
       }

      nao->plus(sym,(skn*(kka+3))+kkk);
     }

   }
 }
GDITE();
rasmesn(mn,gn,mk,gk,kolm,&mdat,&mnah,imaf2,tn,koltb,kp,kategor,tabnom);

/*Распечатываем*/

printw("%s\n-------------------------\n",
gettext("Распечатываем результат"));
refresh();
SQLCURSOR curr;
sn=0.;
for(ii=0 ; ii < kka+3; ii += kkol)
 {
//  printw("ii-%d kka-%d korr-%d kkol=%d\n",ii,kka,korr,kkol);
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii; i<ii+kkol+i2 && i < kka+3;i++)
   {
    metka=0;
    br1=0.;
    for(skn=i;skn< (kka+3)*(koltb-1)+ii+kkol && skn<(kka+3)*koltb; skn+=(kka+3))
     {
      if(fabs(nao->ravno(skn)) > 0.009)
       {
        metka=1;
        break;
       }      
     }
    if(metka == 0)
     {
      i2++;
     }
    else
     {
      br2++;
     }
   }

  if(br2 == 0.)
    continue;   

  
  sappknu(kka+3,&st,ii,ii+kkol,koltb,nah_ud,kon,nao,kaw);

  br1=0.;
  for(i=0 ; i<koltb;i++)
   {
    /*Проверяем если нет равных нолю позиций печатаем*/
    br1=0.;
    for(skn=0;skn< kka+3 ; skn++)
     {
      if(fabs(nao->ravno(skn+i*(kka+3))) > 0.009)
       {
        br1++;
        break;
       }
     }

    if(br1 == 0)
      continue;

    sprintf(strsql,"select fio from Kartb where tabn=%d",tn->ravno(i));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Не найден табельный номер"),tn->ravno(i));
      OSTANOV();
      fio.new_plus("");
     }
    else
      fio.new_plus(row1[0]);

    porn++;
    fprintf(kaw,"%4d %5d %-*.*s",porn,tn->ravno(i),
    iceb_tu_kolbait(30,fio.ravno()),
    iceb_tu_kolbait(30,fio.ravno()),
    fio.ravno());
//    printw("%4d %5ld %-30.30s ii=%d\n",porn,tn[i],fio.ravno(),ii);

    if(ii == 0)
     {
      fprintf(ff,"%4d %5d %-*.*s",porn,tn->ravno(i),iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno());
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)));
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)+kon+1));
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)+kka+2));
      fprintf(ff," %14.2f\n",nao->ravno(i*(kka+3))+ nao->ravno(i*(kka+3)+kon+1) + nao->ravno(i*(kka+3)+kka+2));
      it1+=nao->ravno(i*(kka+3));
      it2+=nao->ravno(i*(kka+3)+kon+1);
      it3+=nao->ravno(i*(kka+3)+kka+2);
      it4+=nao->ravno(i*(kka+3))+ nao->ravno(i*(kka+3)+kon+1) + nao->ravno(i*(kka+3)+kka+2);
     }
    /*Предыдущая информация*/
//    printw("Ітого0\n");
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*(kka+3); skn<i*(kka+3)+ii && skn < (kka+3)*koltb ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	/*Пропускаем итого удержано*/
	if(skn == i*(kka+3)+kka+2 )
	  continue;
	/*Пропускаем итого начислено*/
	if(skn == i*(kka+3)+kon+1)
	  continue;
	 br1+=nao->ravno(skn);
       }
      /*printw("br1=%.2f\n",br1);*/
     }
    i2=0;
    for(skn=i*(kka+3)+ii; skn<i*(kka+3)+ii+kkol+i2 && skn < i*(kka+3)+(kka+3) ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      metka=0;
      br2=0.;
      for(i1=skn-(i*(kka+3)); i1< (kka+3)*koltb ; i1+=(kka+3))
       {
	 br2+=nao->ravno(i1);
        if(fabs(nao->ravno(i1)) > 0.009)
          metka=1;
       }
      if(metka == 1)
       {
/*
	printw("skn-%d br2=%.2f %d %.2f %d %d\n",
	skn,br2,(kka+3)*koltb,nao[skn],kka,koltb);
*/
	fprintf(kaw," %12.2f",nao->ravno(skn));
       }
      else
       {
	i2++;
       }
      /*Пропускаем итого удержано*/
      if(skn == i*(kka+3)+kka+2 )
	continue;
      /*Пропускаем итого начислено*/
      if(skn == i*(kka+3)+kon+1)
        continue;

      br1+=nao->ravno(skn);
     }
    if(skn == i*(kka+3)+kka+3)
     {
      fprintf(kaw," %12.2f\n",br1);
     }
    else
      fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st.ravno());
  fprintf(kaw,"%-*s",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"));
  porn=0;
  i2=0;
  /*printw("kkol=%d ii=%d kka=%d koltb=%d\n",kkol,ii,kka,koltb);*/
  for(i=ii; i<ii+kkol+i2 && i < (kka+3) ;i++)
   {
    /*printw("i=%d i2=%d  %d\n",i,i2,kka*(koltb-1)+ii+kkol);*/
    metka=0;
    br1=0.;
    for(skn=i;skn < (kka+3)*(koltb-1)+ii+kkol+i2 && skn< (kka+3)*koltb ; skn+=(kka+3))
     {
  /*    printw("skn=%d\n",skn);*/
      br1+=nao->ravno(skn);
      if(fabs(nao->ravno(skn)) > 0.009)
        metka=1;
     }
    if(metka == 1)
       fprintf(kaw," %12.2f",br1);
    else
      i2++;

    /*Пропускаем итого удержано*/
    if(i == kka+2 )
     {
      continue;
     }
    /*Пропускаем итого начислено*/
    if(i == kon+1)
     {
      continue;
     } 
    sn+=br1;
   }
  if(i == kka+3)
   {
    fprintf(kaw," %12.2f\n",sn);
   }
  else
   {
    fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st.ravno());

  if(i == kka+3)
   break;
  ii+=i2;
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %14.2f %14.2f %14.2f %14.2f",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),it1,it2,it3,it4);

}

/***************/
/*Шапка таблицы*/
/***************/
void sappknu(long kka, //Количество начислений и удержаний
class iceb_tu_str *st,
long pn, //Позиция начала
long pk, //Позиция конца
long konu, //Количество людей
class masiv_din_int *nah_ud,
int kon, //количество начислений
class masiv_din_double *nao,
FILE *kaw)
{
int             i,i1,i2;
class iceb_tu_str nai("");
double          br2;
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;

/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
st->new_plus("------------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) > 0.01 )
    st->plus("-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
if(i == kka)
  st->plus("-------------");

fprintf(kaw,"\n%s\n",st->ravno());

fprintf(kaw,"П.н.| Т/Н |%-*s",iceb_tu_kolbait(30,gettext("Фамилия")),gettext("Фамилия"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) < 0.01)
   {
    i2++;
    continue;
   }
  if(i == kon+1)
   {
    fprintf(kaw,"| %-*.*s",
    iceb_tu_kolbait(11,gettext("Итого начис.")),
    iceb_tu_kolbait(11,gettext("Итого начис.")),
    gettext("Итого начис."));
    continue;
   }
  if(i == kka-1)
   {
    fprintf(kaw,"| %-*.*s",
    iceb_tu_kolbait(11,gettext("Итого удерж.")),
    iceb_tu_kolbait(11,gettext("Итого удерж.")),
    gettext("Итого удерж."));
    continue;
   }
  if(i != 0)
   {

    if(i <= kon)
     {
      sprintf(strsql,"select naik from Nash where kod=%d",nah_ud->ravno(i-1));
     }
    else
     {
      sprintf(strsql,"select naik from Uder where kod=%d",nah_ud->ravno(i-2));
     }
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      if(i <= kon)
       printw("%s %d !\n",gettext("Не найден код начисления"),nah_ud->ravno(i-1));
      else
       printw("%s %d !\n",gettext("Не найден код удержания"),nah_ud->ravno(i-2));
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);

    fprintf(kaw,"| %-*.*s",
    iceb_tu_kolbait(11,nai.ravno()),
    iceb_tu_kolbait(11,nai.ravno()),
    nai.ravno());
   }
  else
    fprintf(kaw,"| %-*.*s",
    iceb_tu_kolbait(11,gettext("Сальдо нач.")),
    iceb_tu_kolbait(11,gettext("Сальдо нач.")),
    gettext("Сальдо нач."));

 }
if(i == kka)
  fprintf(kaw,"| %-*.*s|\n",
  iceb_tu_kolbait(11,gettext("Сальдо кон.")),
  iceb_tu_kolbait(11,gettext("Сальдо кон.")),
  gettext("Сальдо кон."));
else
  fprintf(kaw,"|\n");

fprintf(kaw,"    |     |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) > 0.01)
   {
    if(i == kon+1)
     {
      fprintf(kaw,"| %-11s","**********");
      continue;
     }
    if(i == kka-1)
     {
      fprintf(kaw,"| %-11s","**********");
      continue;
     }
    if(i != 0)
     {
      if(i <= kon)
        fprintf(kaw,"| %-11d",nah_ud->ravno(i-1));
      else
        fprintf(kaw,"| %-11d",nah_ud->ravno(i-2));
      
     }
    else
      fprintf(kaw,"| %-11s","**********");
   }
  else
    i2++;
 }

if(i == kka)
  fprintf(kaw,"| %-11.11s|\n"," ");
else
  fprintf(kaw,"|\n");

fprintf(kaw,"%s\n",st->ravno());
/*
printw("Закончили шапку\n");
refresh();
*/
}

/***********************************/
/*Распечатка помесячных начислений*/
/***********************************/
void rasmesn(short mn,short gn,short mk,short gk,short kolm,
class masiv_din_int *mdat,class masiv_din_double *mnah,char imaf[],class masiv_din_int *tn,int koltb,const char *kp,const char *kategor,const char *tabnom)
{
struct  tm      *bf;
FILE		*ff;
short		m,g;
short		kol,i,i1,ip=0;
short		pm;
class iceb_tu_str st("");
short		mnn=0,mkk=0;
double		itogs,it;
time_t		tmm;
class iceb_tu_str fio("");
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;
time(&tmm);
bf=localtime(&tmm);

sprintf(imaf,"rspn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


fprintf(ff,"%s\n\n\
%s %d.%d%s %s %d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Свод начислений за период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),kp);

if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
 

if(kategor[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код категории"),kategor);

fprintf(ff,"%s %d\n",gettext("Количество месяцев"),kolm);
/*
for(i=0; i< kolm; i++)
 fprintf(ff,"%d.%d\n",mdat[i*2],mdat[i*2+1]);
*/
pm=0;
for(g=gn; g<=gk ; g++)
 {
   if(g == gn )
     mnn=mn;
   if(gn != gk && g == gk)
    mnn=1;

  if(g == gn && gn == gk)
   mkk=mk;
  if(g == gn && gn != gk)
   mkk=12;
  if(g == gk )
   mkk=mk;
  if(g != gk  && g != gn)
   mkk=12;
  
  rasmesn1(mnn,mkk,g,&st,ff);
  int nomer=0;
  for(kol=0; kol < koltb; kol++)
   {
    sprintf(strsql,"%d",tn->ravno(kol));
    
    if(proverka(tabnom,strsql,0,0) != 0)
      continue;
    /*Проверяем были ли начисления*/
    sprintf(strsql,"select fio,podr,kateg from Kartb where tabn=%d",tn->ravno(kol));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d /%d/%d\n",gettext("Не найден табельный номер"),tn->ravno(kol),kol,koltb);
      OSTANOV();
      fio.new_plus("");
     }
    else
      fio.new_plus(row1[0]);

    if(proverka(kp,row1[1],0,0) != 0)
      continue;
    if(proverka(kategor,row1[2],0,0) != 0)
      continue;

    //Проверяем есть ли начисления   
    m=mnn;
    ip=pm;   
    itogs=0;
    for( ;     ; m++)
     {

      if(g == gk && m > mk)
       {
        break;
       }
      if(m == 13)
       {
        m=1;
        break;
       }
      itogs+=mnah->ravno(kol*kolm+ip);
      if(itogs != 0.)
       break;
      ip++;
     }
    if(itogs == 0.)
     continue;

    fprintf(ff,"%4d|%5d|%-*.*s|",
    ++nomer,
    tn->ravno(kol),
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno());
   
    m=mnn;
    ip=pm;   
    itogs=0;
    for( ;     ; m++)
     {

      if(g == gk && m > mk)
       {
        fprintf(ff,"%10.2f|\n",itogs);
        break;
       }
      if(m == 13)
       {
        m=1;
        fprintf(ff,"%10.2f|\n",itogs);
        break;
       }
      fprintf(ff,"%8.2f|",mnah->ravno(kol*kolm+ip));
      itogs+=mnah->ravno(kol*kolm+ip);
      ip++;
     }
   
   }

  /*Горизонтальное Итого*/
  fprintf(ff,"%s\n",st.ravno());
  fprintf(ff,"%*s|",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"));
  it=0.;
  for(i=pm; i <= pm+(mkk-mnn) ;i++)
   {
    itogs=0.;
    for(i1=0; i1 < koltb; i1++)
      itogs+=mnah->ravno(i1*kolm+i);
    fprintf(ff,"%8.2f|",itogs);
    it+=itogs;
   }
  fprintf(ff,"%10.2f|\n%s\n",it,st.ravno());
  pm=ip;
 }
podpis(ff);

fclose(ff);

}
/*******************************************/
/*Распечатка шапки для начислений по месяцу*/
/*******************************************/
void rasmesn1(short mn,short mk,short gg,class iceb_tu_str *st,FILE *ff)
{
short		i;

/*Полка над наименованием*/
st->new_plus("------------------------------------------");
for(i=mn; i <= mk; i++)
 st->plus("---------");

/*Полка над Итого*/
st->plus("-----------");

fprintf(ff,"%s\n",st->ravno());

fprintf(ff,"П.н.| Т/Н |%-*s|",iceb_tu_kolbait(30,gettext("Фамилия")),gettext("Фамилия"));
for(i=mn; i <= mk; i++)
 fprintf(ff," %02d.%d|",i,gg);
fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",st->ravno());
 

}
/********************************************/
/*Cортировка по фамилиям*/
/****************************/
void svodnu_sort(class masiv_din_int *tabn,int koltb)
{


class iceb_t_tmptab tabtmp;
const char *imatmptab={"zarsnu"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
tbn int not null,\
fio char(112) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return;
 }  

char strsql[512];
SQL_str row;
SQLCURSOR cur;
for(int i=0 ; i < koltb; i++)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    continue;
   }
   
//  fprintf(ff,"%d|%s|\n",tabn[i],row[0]);

  sprintf(strsql,"insert into %s values (%d,'%s')",
  imatmptab,
  tabn->ravno(i),iceb_tu_sqlfiltr(row[0]));  

  sql_zapis(strsql,1,1);    
     
 }
int kolstr=0;
sprintf(strsql,"select * from %s order by fio asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return;
 }

int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  tabn->new_plus(atoi(row[0]),nomer++);
  
 }


}
