/*$Id: dvos.c,v 5.44 2013/09/26 09:43:32 sasa Exp $*/
/*10.01.2018	03.07.2001	Белых А.И.	dvos.c
Распечатка списка записей в документах за любой период времени
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"

void	sapdvos(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,const char *vidop,const char *grupabuh,const char *grupanal,const char *shetu_p);
void dvos_rp(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,const char *vidop,const char *grupabuh,const char *grupanal,const char *shetu_p);


int dvos(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podrz(""); /*Код подразделения*/
static class iceb_tu_str kodolz(""); /*Коды ответственных лиц*/
static class iceb_tu_str kodop(""); /*Коды операции*/
static class iceb_tu_str vidop(""); /*Вид операции*/
static class iceb_tu_str grupabuh(""); /*Группа бух. учета*/
static class iceb_tu_str grupanal(""); /*Группа налогового учёта*/
static class iceb_tu_str shetu_p(""); //Счёт учёта
short           dn,mn,gn;
short           dk,mk,gk;
int		kom=0,kom1=0;
char		strsql[1024];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
char		imaf[64],imafgrbuh[64],imafgrnal[64];
FILE		*ff;
time_t 		tmm;
struct  tm      *bf;
double		bsn,izn,bsb,izb;
double		ibsn=0.,iizn=0.,ibsb=0.,iizb=0.;
double		ibsnp=0.,iiznp=0.,ibsbp=0.,iizbp=0.;
double		ibsnr=0.,iiznr=0.,ibsbr=0.,iizbr=0.;
double		ibsngr=0.,iizngr=0.,ibsbgr=0.,iizbgr=0.;
double		ibsngro=0.,iizngro=0.,ibsbgro=0.,iizbgro=0.;
short		d,m,g;
int		tipz=0;
class iceb_tu_str invnom("");
class iceb_tu_str vidopt("");
class iceb_tu_str naim;
class iceb_tu_str nalbs("");
class iceb_tu_str naliz("");
class iceb_tu_str buhbs("");
class iceb_tu_str buhiz("");
class iceb_tu_str nomd("");
class iceb_tu_str kodoper("");
class iceb_tu_str podr("");
class iceb_tu_str matot("");
class iceb_tu_str grbuh(""),grbuhz("");
class iceb_tu_str grnal(""),grnalz("");

VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodolz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(vidop.ravno(),2);
VV.VVOD_SPISOK_add_data(grupabuh.ravno(),128);
VV.VVOD_SPISOK_add_data(grupanal.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu_p.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции......(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Вид операции.....(+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа бух. учета.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа нал. учета.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта........(,,).."));

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка реестра документов движения основных средств"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

naz:;

clear();


helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

podrz.new_plus(VV.data_ravno(2));
kodolz.new_plus(VV.data_ravno(3));
kodop.new_plus(VV.data_ravno(4));
vidop.new_plus(VV.data_ravno(5));
grupabuh.new_plus(VV.data_ravno(6));
grupanal.new_plus(VV.data_ravno(7));
shetu_p.new_plus(VV.data_ravno(8));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");

switch(kom)
 {
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:  /*Просмотр реквизитов*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственны"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
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
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(2,kod_v.ravno());
        goto naz;

      case 1 :

        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(3,kod_v.ravno());
        goto naz;

      case 2 :
        vibrek("Uosgrup",&kod_v,&naim_v);
        VV.VVOD_SPISOK_zapis_data(7,kod_v.ravno());
        goto naz;

      case 3 :
        vibrek("Uosgrup1",&kod_v,&naim_v);
        VV.VVOD_SPISOK_zapis_data(6,kod_v.ravno());
        goto naz;

      case 4 :
        vibrek("Uospri",&kod_v,&naim_v);
        VV.VVOD_SPISOK_zapis_data(4,kod_v.ravno());
        goto naz;

      case 5 :
//          vibrek("Uosras",&kod_v,&naim_v);
        if(diruosras(1,&kod_v,&naim_v) == 0)
          VV.data_z_plus(4,kod_v.ravno());
        goto naz;


     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

GDITE();
clear();

printw("%s %d.%d.%d => %d.%d.%d\n",
gettext("Распечатка записей документов"),
dn,mn,gn,dk,mk,gk);

time(&tmm);
bf=localtime(&tmm);

sprintf(strsql,"select datd,tipz,innom,nomd,podr,kodol,bs,iz,kodop,\
bsby,izby from Uosdok1 where datd >= '%d-%d-%d' and datd <= '%d-%d-%d' \
order by datd asc",
gn,mn,dn,gk,mk,dk);

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

class iceb_t_tmptab tabtmp;
const char *imatmptab={"dvos"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));
sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
hnaby char(24) not null,\
hna char(24) not null,\
datd DATE NOT NULL,\
vidopt char(8) not null,\
innom int not null,\
naim char(112) not null,\
bsn double(16,2) not null,\
izn double(16,2) not null,\
bsb double(16,2) not null,\
izb double(16,2) not null,\
nomd char(32) not null,\
kodop char(32) not null,\
podr int not null,\
kodol int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

sprintf(imaf,"dvos%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
zagolov(gettext("Реестр документов движения основных средств"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);

sapdvos(ff,kodolz.ravno(),podrz.ravno(),kodop.ravno(),vidop.ravno(),grupabuh.ravno(),grupanal.ravno(),shetu_p.ravno());
char imaf_gos[64];
FILE *ff_gos;

sprintf(imaf_gos,"dvosg%d.lst",getpid());

if((ff_gos = fopen(imaf_gos,"w")) == NULL)
 {
  error_op_nfil(imaf_gos,errno,"");
  return(1);
 }
zagolov(gettext("Реестр основных средств"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff_gos);
dvos_rp(ff_gos,kodolz.ravno(),podrz.ravno(),kodop.ravno(),vidop.ravno(),grupabuh.ravno(),grupanal.ravno(),shetu_p.ravno());

fprintf(ff_gos,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff_gos,gettext("\
Номер п/п|Дата докум|         Наименование         |Бал.стоимость|Примечание\n"));
/**************************
123456789 1234567890|123456789012345678901234567890 1234567890123
**********************/
fprintf(ff_gos,"\
-----------------------------------------------------------------------------------\n");


int nomer_pp=0;
class poiinpdw_data rekin;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(podrz.ravno(),row[4],0,0) != 0)
    continue;
  if(proverka(kodolz.ravno(),row[5],0,0) != 0)
    continue;
  if(proverka(kodop.ravno(),row[8],0,0) != 0)
    continue;
   
  if(vidop.ravno()[0] == '-')
    if(row[1][0] == '1')
     continue;

  if(vidop.ravno()[0] == '+')
    if(row[1][0] == '2')
     continue;

  rsdat(&d,&m,&g,row[0],2);

  poiinpd(atol(row[2]),m,g,&rekin);

  if(proverka(shetu_p.ravno(),rekin.shetu.ravno(),1,0) != 0)
       continue;
  if(proverka(grupabuh.ravno(),rekin.hnaby.ravno(),0,0) != 0)
       continue;
  if(proverka(grupanal.ravno(),rekin.hna.ravno(),0,0) != 0)
       continue;

  tipz=atoi(row[1]);
  if(tipz == 1)
   vidopt.new_plus("+");
  if(tipz == 2)
   vidopt.new_plus("-");
   
  bsn=atof(row[6]);  
  izn=atof(row[7]);  
  bsb=atof(row[9]);  
  izb=atof(row[10]);  
  naim.new_plus("");
  sprintf(strsql,"select naim from Uosin where innom=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naim.new_plus(row1[0]);

  fprintf(ff,"%02d.%02d.%d %s %-*s %-*.*s %11.2f %11.2f %11.2f %11.2f \
%-*s %-*s %-5s %-5s %-*s %-*s\n",
  d,m,g,
  vidopt.ravno(),
  iceb_tu_kolbait(8,row[2]),row[2],
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  bsn,izn,bsb,izb,
  iceb_tu_kolbait(5,row[3]),row[3],
  iceb_tu_kolbait(5,row[8]),row[8],
  row[4],
  row[5],
  iceb_tu_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
  iceb_tu_kolbait(6,rekin.hna.ravno()),rekin.hna.ravno());

  for(int nom=20; nom < iceb_tu_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_adrsimv(nom,naim.ravno()));
   }

  fprintf(ff_gos,"%9d|%02d.%02d.%04d|%-*.*s|%13.2f|\n",
  ++nomer_pp,d,m,g,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  bsb);

  for(int nom=30; nom < iceb_tu_strlen(naim.ravno());nom+=30)
   fprintf(ff_gos,"%9s|%10s|%-*.*s|%13s|\n",
   "","",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
   iceb_tu_adrsimv(nom,naim.ravno()),
   "");
   

  sprintf(strsql,"insert into %s values ('%s','%s','%04d-%02d-%02d','%s',%s,'%s',%.2f,%.2f,%.2f,%.2f,'%s','%s',%s,%s)",
  imatmptab,
  rekin.hnaby.ravno(),rekin.hna.ravno(),g,m,d,vidopt.ravno(),row[2],naim.ravno_filtr(),bsn,izn,bsb,izb,
  row[3],row[8],row[4],row[5]);

  sql_zapis(strsql,1,0);    

  ibsn+=bsn;
  iizn+=izn;
  ibsb+=bsb;
  iizb+=izb;
  if(tipz == 1)
   {
    ibsnp+=bsn;
    iiznp+=izn;
    ibsbp+=bsb;
    iizbp+=izb;
   }  
  if(tipz == 2)
   {
    ibsnr+=bsn;
    iiznr+=izn;
    ibsbr+=bsb;
    iizbr+=izb;
   }  
 }
GDITE();

fprintf(ff_gos,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff_gos,"%*s:%13.2f\n",
iceb_tu_kolbait(51,gettext("Итого")),
gettext("Итого"),
ibsb);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(42,gettext("Итого")),gettext("Итого"),ibsn,iizn,ibsb,iizb);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

printw("%s :%11.2f %11.2f %11.2f %11.2f\n",
gettext("Итого"),ibsn,iizn,ibsb,iizb);
printw("\n%s+:%11.2f %11.2f %11.2f %11.2f\n",
gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);
printw("%s :%11.2f %11.2f %11.2f %11.2f\n",
gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);

podpis(ff);

fclose(ff);

podpis(ff_gos);

fclose(ff_gos);

int kolstrtmp=0;
class SQLCURSOR curtmp;
SQL_str rowtmp;
sprintf(strsql,"select * from %s order by hnaby asc,datd asc",imatmptab);

if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imafgrbuh,"dvosbh%d.lst",getpid());

if((ff = fopen(imafgrbuh,"w")) == NULL)
 {
  error_op_nfil(imafgrbuh,errno,"");
  return(1);
 }
zagolov(gettext("Распечатка записей документов"),bf,dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);

fprintf(ff,"%s\n",gettext("Сортировка по группам бухгалтерского учёта"));

sapdvos(ff,kodolz.ravno(),podrz.ravno(),kodop.ravno(),vidop.ravno(),grupabuh.ravno(),grupanal.ravno(),shetu_p.ravno());

double ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
double ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  grbuh.new_plus(rowtmp[0]);
  if(SRAV(grbuhz.ravno(),grbuh.ravno(),0) != 0)
   {
    if(grbuhz.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz.ravno());
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);
      
      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;
      
      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    grbuhz.new_plus(grbuh.ravno());
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }

  grnal.new_plus(rowtmp[1]);
  rsdat(&d,&m,&g,rowtmp[2],2);
  vidopt.new_plus(rowtmp[3]);
  invnom.new_plus(rowtmp[4]);
  naim.new_plus(rowtmp[5]);
  nalbs.new_plus(rowtmp[6]);
  naliz.new_plus(rowtmp[7]);
  buhbs.new_plus(rowtmp[8]);
  buhiz.new_plus(rowtmp[9]);
  nomd.new_plus(rowtmp[10]);
  kodoper.new_plus(rowtmp[11]);
  podr.new_plus(rowtmp[12]);
  matot.new_plus(rowtmp[13]);

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt.ravno(),invnom.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  nalbs.ravno(),naliz.ravno(),buhbs.ravno(),buhiz.ravno(),
  iceb_tu_kolbait(5,nomd.ravno()),nomd.ravno(),
  iceb_tu_kolbait(5,kodoper.ravno()),kodoper.ravno(),
  podr.ravno(),matot.ravno(),grbuh.ravno(),grnal.ravno());

  for(int nom=20; nom < iceb_tu_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_adrsimv(nom,naim.ravno()));
   }

  ibsngr+=nalbs.ravno_atof();
  iizngr+=naliz.ravno_atof();
  ibsbgr+=buhbs.ravno_atof();
  iizbgr+=buhiz.ravno_atof();
  if(vidopt.ravno()[0] == '+')
   {
    ibsnp+=nalbs.ravno_atof();
    iiznp+=naliz.ravno_atof();
    ibsbp+=buhbs.ravno_atof();
    iizbp+=buhiz.ravno_atof();

    ibsnpo+=nalbs.ravno_atof();
    iiznpo+=naliz.ravno_atof();
    ibsbpo+=buhbs.ravno_atof();
    iizbpo+=buhiz.ravno_atof();
   }  
  if(vidopt.ravno()[0] == '-')
   {
    ibsnr+=nalbs.ravno_atof();
    iiznr+=naliz.ravno_atof();
    ibsbr+=buhbs.ravno_atof();
    iizbr+=buhiz.ravno_atof();

    ibsnro+=nalbs.ravno_atof();
    iiznro+=naliz.ravno_atof();
    ibsbro+=buhbs.ravno_atof();
    iizbro+=buhiz.ravno_atof();
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz.ravno());
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*.*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(42,gettext("Итого по всем группам")),iceb_tu_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),
ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

podpis(ff);

fclose(ff);

sprintf(strsql,"select * from %s order by hna asc,datd asc",imatmptab);

kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


sprintf(imafgrnal,"dvosnl%d.lst",getpid());

if((ff = fopen(imafgrnal,"w")) == NULL)
 {
  error_op_nfil(imafgrnal,errno,"");
  return(1);
 }
zagolov(gettext("Распечатка записей документов"),bf,dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);

fprintf(ff,"%s.\n",gettext("Сортировка по группам налогового учёта"));

sapdvos(ff,kodolz.ravno(),podrz.ravno(),kodop.ravno(),vidop.ravno(),grupabuh.ravno(),grupanal.ravno(),shetu_p.ravno());
grnalz.new_plus("");
ibsngr=iizngr=ibsbgr=iizbgr=0.;
ibsngro=iizngro=ibsbgro=iizbgro=0.;
ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  grbuh.new_plus(rowtmp[0]);
  grnal.new_plus(rowtmp[1]);


  if(SRAV(grnalz.ravno(),grnal.ravno(),0) != 0)
   {
    if(grnalz.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz.ravno());
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    grnalz.new_plus(grnal.ravno());
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }
  rsdat(&d,&m,&g,rowtmp[2],2);
  vidopt.new_plus(rowtmp[3]);
  invnom.new_plus(rowtmp[4]);
  naim.new_plus(rowtmp[5]);
  nalbs.new_plus(rowtmp[6]);
  naliz.new_plus(rowtmp[7]);
  buhbs.new_plus(rowtmp[8]);
  buhiz.new_plus(rowtmp[9]);
  nomd.new_plus(rowtmp[10]);
  kodoper.new_plus(rowtmp[11]);
  podr.new_plus(rowtmp[12]);
  matot.new_plus(rowtmp[13]);

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt.ravno(),invnom.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  nalbs.ravno(),naliz.ravno(),buhbs.ravno(),buhiz.ravno(),
  iceb_tu_kolbait(5,nomd.ravno()),nomd.ravno(),
  iceb_tu_kolbait(5,kodoper.ravno()),kodoper.ravno(),
  podr.ravno(),matot.ravno(),grbuh.ravno(),grnal.ravno());

  for(int nom=20; nom < iceb_tu_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_adrsimv(nom,naim.ravno()));
   }

  ibsngr+=nalbs.ravno_atof();
  iizngr+=naliz.ravno_atof();
  ibsbgr+=buhbs.ravno_atof();
  iizbgr+=buhiz.ravno_atof();
  if(vidopt.ravno()[0] == '+')
   {
    ibsnp+=nalbs.ravno_atof();
    iiznp+=naliz.ravno_atof();
    ibsbp+=buhbs.ravno_atof();
    iizbp+=buhiz.ravno_atof();

    ibsnpo+=nalbs.ravno_atof();
    iiznpo+=naliz.ravno_atof();
    ibsbpo+=buhbs.ravno_atof();
    iizbpo+=buhiz.ravno_atof();
   }  
  if(vidopt.ravno()[0] == '-')
   {
    ibsnr+=nalbs.ravno_atof();
    iiznr+=naliz.ravno_atof();
    ibsbr+=buhbs.ravno_atof();
    iizbr+=buhiz.ravno_atof();

    ibsnro+=nalbs.ravno_atof();
    iiznro+=naliz.ravno_atof();
    ibsbro+=buhbs.ravno_atof();
    iizbro+=buhiz.ravno_atof();
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz.ravno());
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_tu_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*.*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(42,gettext("Итого по всем группам")),iceb_tu_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_tu_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

podpis(ff);

fclose(ff);


printw_vr(tmm);


OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр документов движения основных средств"));

oth->spis_imaf.plus(imafgrbuh);
oth->spis_naim.plus(gettext("Реестр документов с сортировкой по группам бух.уч"));
oth->spis_imaf.plus(imafgrnal);
oth->spis_naim.plus(gettext("Реестр документов с сортировкой по группам нал.уч"));

oth->spis_imaf.plus(imaf_gos);
oth->spis_naim.plus(gettext("Реестр основных средств"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
 
}
/*************************/
/*Реквизиты поиска*/
/**************************/
void dvos_rp(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,
const char *vidop,const char *grupabuh,const char *grupanal,const char *shetu_p)
{
if(kodolz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
if(podrz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
if(kodop[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
if(vidop[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),vidop);
if(grupabuh[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),grupabuh);
if(grupanal[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),grupanal);
if(shetu_p[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Счёт учёта"),shetu_p);
}
/**************/
/*Шапка файлов*/
/***************/
void	sapdvos(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,
const char *vidop,const char *grupabuh,const char *grupanal,const char *shetu_p)
{

dvos_rp(ff,kodolz,podrz,kodop,vidop,grupabuh,grupanal,shetu_p);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата докум. |Инвент. |  Наименование      |  Налоговый учёт       |Бухгалтерский учёт     |Ном. |Код. |Подр-|Мат. |Группа|Группа|\n"));
fprintf(ff,gettext("\
            | номер  |                    |Бал.стоим. |   Износ   |Бал.стоим. |   Износ   |докум|опер.|аздел|отвец|бух.уч|нал.уч|\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
}
