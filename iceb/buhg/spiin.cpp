/* $Id: spiin.c,v 5.54 2013/09/26 09:43:40 sasa Exp $ */
/*28.03.2018    14.06.1993      Белых А.И.      spiin.c
Распечатка списка работников имеющих определенное начисление или
удержание
*/
#include        <errno.h>
#include        "buhg.h"

extern short    mmm,ggg;

int spiin(class spis_oth *oth)
{
int		kom,kom1;
struct  tm      *bf;
time_t          tmm;
char            imaf[64];
FILE            *kaw,*ffsum;
double          sum;
int             kolp; /*Количесво людей*/
class iceb_tu_str st("");
short           mnh,gnh;
long		kolstr,kolstr1;
SQL_str         row,row2;
char		strsql[1024];
double		sym;
class iceb_tu_str fio("");
short		mz,gz;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str nahis("");
static class iceb_tu_str uder("");
static class iceb_tu_str nahud("");
static class iceb_tu_str sheta("");
static class iceb_tu_str kategor("");
short		dn,mn,gn,dk,mk,gk;
long		tabn,tabnz;
class iceb_tu_str kodpodr("");
short		prn;
int		klst=0;
class iceb_tu_str fiov("");

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"1.%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Получение списка работников, которые имеют начисление/удержание."));
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(uder.ravno(),128);
VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(nahud.ravno(),2);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(kategor.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.....(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD( gettext("Код подразделения.(,,).."));//2
VV.VVOD_SPISOK_add_MD( gettext("Табельний номер...(,,).."));//3
VV.VVOD_SPISOK_add_MD( gettext("Код начисления....(,,).."));//4
VV.VVOD_SPISOK_add_MD( gettext("Код удержания.....(,,).."));//5
VV.VVOD_SPISOK_add_MD( gettext("В счёт месяца...(м.г)..."));//6
VV.VVOD_SPISOK_add_MD(gettext("Нач-ния/уде-ния.(+/-)..."));//7
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,)..."));//8
VV.VVOD_SPISOK_add_MD( gettext("Код категории....(,,)..."));//9

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
podr.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
nahis.new_plus(VV.data_ravno(4));
uder.new_plus(VV.data_ravno(5));
nahud.new_plus(VV.data_ravno(7));
sheta.new_plus(VV.data_ravno(8));
kategor.new_plus(VV.data_ravno(9));

class iceb_tu_str kodv("");
class iceb_tu_str naim("");

switch(kom)
 {
  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("zarp4_3_3.txt");
    goto naz;

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка удержаний"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
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
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
       {
        fiov.new_plus("");
        int tmm=0;
        if(dirtab(&tmm,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tmm);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
          goto naz;
         }
        goto naz;
      }
      
      case 2 :
        if(dirnach(1,&nahis,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;
      
      case 3 :
        if(diruder(1,&kodv,&naim) == 0)
         uder.z_plus(kodv.ravno());
        VV.VVOD_SPISOK_zapis_data(5,uder.ravno());
        goto naz;

      case 4 :
        vibrek("Kateg",&kategor,&naim);
        VV.VVOD_SPISOK_zapis_data(9,kategor.ravno());
        goto naz;
      

     }
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
    goto naz;
mnh=gnh=0;
if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
 if(rsdat1(&mnh,&gnh,VV.VVOD_SPISOK_return_data(6)) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата начисления/удержания !"));
   goto naz;
  }
if(nahud.ravno()[0] != '\0' && nahud.ravno()[0] != '+' && nahud.ravno()[0] != '-')
 {
  iceb_t_soob(gettext("Не верно введён вид начисления/удержания (+/-) !"));
  goto naz;

 }


clear(); /*Очистить экран и закрасить фоновым цветом*/
GDITE();
sprintf(strsql,"select tabn,prn,knah,suma,mesn,godn,shet,datz,kdn from Zarp where \
datz >= '%d-%d-%d' and datz <= '%d-%d-%d' and suma <> 0. \
order by tabn asc",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
SQLCURSOR curr;
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

sprintf(imaf,"spi%d.lst",getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


char imaf_sum[64];
sprintf(imaf_sum,"spiis%d.lst",getpid());
if((ffsum = fopen(imaf_sum,"w")) == NULL)
 {
  error_op_nfil(imaf_sum,errno,"");
  return(1);
 }

time(&tmm);
bf=localtime(&tmm);

printw("%s %d.%d.%d%s > %d.%d.%d%s %s %d.%d%s\n",
gettext("Дата поиска"),dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."),
gettext("дата начисления/удержания"),
mnh,gnh,gettext("г."));

fprintf(kaw,"%s %d.%d.%d%s > %d.%d.%d%s %s %d.%d%s\n",
gettext("Дата поиска"),dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."),
gettext("дата начисления/удержания"),
mnh,gnh,gettext("г."));

fprintf(kaw,"%s %d.%d.%d%s %s: %d:%d %s - %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
gettext("Текущая дата"),
mmm,ggg);

iceb_t_zagolov(gettext("Список работников, которые имеют нач./удерж"),dn,mn,gn,dk,mk,gk,ffsum);

if(podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr.ravno());
  printcod(kaw,"Podr","kod","naik",0,podr.ravno(),&klst);
  printcod(ffsum,"Podr","kod","naik",0,podr.ravno(),&klst);
 }
else
 fprintf(kaw,gettext("По всем подразделениям.\n"));

if(nahis.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Коды начислений вошедшие в расчёт"),nahis.ravno());
  printcod(kaw,"Nash","kod","naik",0,nahis.ravno(),&klst);
  fprintf(ffsum,"%s:%s\n",gettext("Коды начислений вошедшие в расчёт"),nahis.ravno());
  printcod(ffsum,"Nash","kod","naik",0,nahis.ravno(),&klst);
 }
if(uder.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Коды удержаний вошедшие в расчёт"),uder.ravno());
  printcod(kaw,"Uder","kod","naik",0,uder.ravno(),&klst);
  fprintf(ffsum,"%s:%s\n",gettext("Коды удержаний вошедшие в расчёт"),uder.ravno());
  printcod(ffsum,"Uder","kod","naik",0,uder.ravno(),&klst);
 }
if(tabnom.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Табельные номера вошедшие в расчёт"),tabnom.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Табельные номера вошедшие в расчёт"),tabnom.ravno());
 }
if(sheta.ravno()[0] != '\0')
 { 
  fprintf(kaw,"%s:%s\n",gettext("Счёт"),sheta.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Счёт"),sheta.ravno());
 }
if(nahud.ravno()[0] == '+')
 { 
  fprintf(kaw,"%s.\n",gettext("ТОЛЬКО НАЧИСЛЕНИЯ"));
  fprintf(ffsum,"%s.\n",gettext("ТОЛЬКО НАЧИСЛЕНИЯ"));

 }
if(nahud.ravno()[0] == '-')
 {
  fprintf(kaw,"%s.\n",gettext("ТОЛЬКО УДЕРЖАНИЯ"));
  fprintf(ffsum,"%s.\n",gettext("ТОЛЬКО УДЕРЖАНИЯ"));
 }
if(kategor.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Категория"),kategor.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Категория"),kategor.ravno());
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(ffsum,"-----------------------------------------------------------------------\n");

/****************************************/


kolp=0;
sum=0.;
tabn=tabnz=0;
kolstr1=0;
SPISOK sp_tab;
masiv_din_double sum_tab;
int nomer;
short d,m,g;
class iceb_tu_str kateg_z("");

while(cur.read_cursor(&row) != 0)
 {

  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);

  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(sheta.ravno(),row[6],0,0) != 0)
    continue;

  tabn=atol(row[0]);    
  if(tabn != tabnz)
   {
    
    sprintf(strsql,"select fio,podr,kateg from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не найден табельный номер"),row[0]);
      OSTANOV();
      fio.new_plus("");
      kateg_z.new_plus("");
     }
    else
     {
      fio.new_plus(row2[0]);
      kodpodr.new_plus(row2[1]);
      kateg_z.new_plus(row2[2]);
     }
    tabnz=tabn;
   
   }

  if(proverka(podr.ravno(),kodpodr.ravno(),0,0) != 0)
    continue;

  if(proverka(kategor.ravno(),kateg_z.ravno(),0,0) != 0)
    continue;

  prn=atoi(row[1]);

  if(prn == 1 && nahud.ravno()[0] == '-')
     continue;
  if(prn == 2 && nahud.ravno()[0] == '+')
     continue;

  if(prn == 1 && nahis.ravno()[0] != '\0')
   if(proverka(nahis.ravno(),row[2],0,0) != 0)
     continue;

     
  if(prn == 2 && uder.ravno()[0] != '\0')
   if(proverka(uder.ravno(),row[2],0,0) != 0)
     continue;

  sym=atof(row[3]);
  mz=atoi(row[4]);
  gz=atoi(row[5]);

  if(mnh != 0 && (mnh != mz || gnh != gz))
    continue;
    
  if(prn == 2 && nahis.ravno()[0] == '\0')
    sym*=-1;
  rsdat(&d,&m,&g,row[7],2);
  prnb(sym,&st);
  
  printw("%-7s %-*.*s %9s %d.%d %02d.%02d.%d\n",
  row[0],
  iceb_tu_kolbait(25,fio.ravno()),iceb_tu_kolbait(25,fio.ravno()),fio.ravno(),
  st.ravno(),mz,gz,d,m,g);

  fprintf(kaw,"%-7s %-*.*s %9s %d.%d %-6s %02d.%02d.%d %s %s\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  st.ravno(),mz,gz,row[6],d,m,g,row[8],row[2]);

  sum+=sym;
  kolp++;
  
  if((nomer=sp_tab.find(row[0])) == -1)
   sp_tab.plus(row[0]);
  sum_tab.plus(sym,nomer);
  
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(kaw,"%s - %.2f\n%s - %d",
gettext("Итого"),sum,
gettext("Количество работников"),kolp);


printw("----------------------------------------------\n");

printw("%s - %.2f\n%s - %d\n",
gettext("Итого"),sum,
gettext("Количество работников"),kolp);

podpis(kaw); 
fclose(kaw);


sum=0.;
int kolzap=sp_tab.kolih();
for(int i=0; i < kolzap; i++)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",sp_tab.ravno(i));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   fio.new_plus(row[0]);
  else
   fio.new_plus("");
   
  fprintf(ffsum,"%-7s %-*.*s %10.2f\n",
  sp_tab.ravno(i),
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  sum_tab.ravno(i));
  sum+=sum_tab.ravno(i);
 }
fprintf(ffsum,"-----------------------------------------------------------------------\n");
fprintf(ffsum,"%*s:%10.2f",iceb_tu_kolbait(38,gettext("Итого")),gettext("Итого"),sum);

podpis(ffsum); 
fclose(ffsum);



OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список работников, которые имеют нач./удерж"));
oth->spis_imaf.plus(imaf_sum);
oth->spis_naim.plus(gettext("Ощие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
 
return(0);
}
