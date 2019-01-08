/*$Id: buhspk.c,v 5.18 2013/09/26 09:43:29 sasa Exp $*/
/*13.07.2015	12.06.2007	Белых А.И.	buhspk.c
Расчёт акта сверки по группе контрагентов
*/
#include        <errno.h>
#include "buhg.h"

class akt_sverki_po_grup //Для расчёта акта сверки по группе контрагента
 {
  public:
   class SPISOK data_nomd_kontr; //Дата|номер документа|код контрагента
   class SPISOK koment;    //комментарий
   class masiv_din_double suma_deb; 
   class masiv_din_double suma_kre; 
   class masiv_din_double start_saldo_deb; //сальдо по каждому контрагенту
   class masiv_din_double start_saldo_kre;
   class iceb_tu_str shet;  

   class SPISOK spisok_kontr; //Cписок контрагентов в группе
     
  akt_sverki_po_grup()
   {
    clear();    
   }
  void clear()
   {
    start_saldo_deb.clear_class();
    start_saldo_kre.clear_class();
    shet.new_plus("");
    suma_deb.free_class();
    suma_kre.free_class();
    data_nomd_kontr.free_class();
    koment.free_class();    
   }
 };

void buhspk_rsh(short dn,short mn,short gn,short dk,short mk,short gk,class akt_sverki_po_grup *akt_sv);
void buhspk_it(double deb_start,double kre_start,double deb_oborot,double kre_oborot,double deb_ob_mes,double kre_ob_mes,FILE *ff);
void buhspk_rg(short dn,short mn,short gn,class akt_sverki_po_grup *akt_sv,FILE *ff);
void buhspk_rg_k(int,short dn,short mn,short gn,class akt_sverki_po_grup *akt_sv,FILE *ff);

extern short	startgodb; /*Стартовый год*/
extern char     *sbshet; //Список бюджетных счетов

int buhspk(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
static class iceb_tu_str gkontr("");
int kom1=0;
class iceb_tu_str  naim("");
char strsql[512];

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт акта сверки по группе контрагента"));

VV.VVOD_SPISOK_add_MD(gettext("Код группы кон-та...."));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)..."));

VV.VVOD_SPISOK_add_data(gkontr.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);

naz:;

clear();
if(sbshet != NULL)
 printw("%s",sbshet);
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

gkontr.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
dat2.new_plus(VV.data_ravno(2));
sheta.new_plus(VV.data_ravno(3));

class iceb_tu_str kod("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 0 :
        vibrek("Plansh",&sheta,&naim);
        VV.data_z_plus(3,sheta.ravno());
        break;

      case 1 :
        if(vibrek("Gkont",&kod,&naim) == 0)
         gkontr.new_plus(kod.ravno());
        VV.data_plus(0,gkontr.ravno());
                 
        break;
     }
    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

GDITE();
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
class akt_sverki_po_grup akt_sv;
class iceb_tu_str naim_gr_kontr("");

//Проверяем код группы контрагента
sprintf(strsql,"select naik from Gkont where kod=%d",gkontr.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код группы"),gkontr.ravno());
  iceb_t_soob(strsql);
  goto naz; 
 }
naim_gr_kontr.new_plus(row[0]);

//создаём список контрагентов 
sprintf(strsql,"select kodkon from Kontragent where grup='%s'",gkontr.ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Нет ни одного контрагента в группе"),gkontr.ravno());
  iceb_t_soob(strsql);
  return(1);
 }

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  akt_sv.spisok_kontr.plus(row[0]);
 }

akt_sv.start_saldo_deb.make_class(akt_sv.spisok_kontr.kolih());
akt_sv.start_saldo_kre.make_class(akt_sv.spisok_kontr.kolih());

//Создаём список счетов 
sprintf(strsql,"select ns,kodkon from Skontr");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Нет ни одного контрагента в группе"),gkontr.ravno());
  iceb_t_soob(strsql);
  return(1);
 }

class SPISOK spisok_shet;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(sheta.ravno(),row[0],0,0) != 0)
   continue;
   
  if(akt_sv.spisok_kontr.find(row[1]) != 0)
   continue;

  spisok_shet.plus(row[0]);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"aspg%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 
 }

zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gkontr.ravno(),naim_gr_kontr.ravno(),ff);

FILE *ff_k;
char imaf_k[64];
sprintf(imaf_k,"aspgk%d.lst",getpid());
if((ff_k=fopen(imaf_k,"w")) == NULL)
 {
  error_op_nfil(imaf_k,errno,"");
  return(1);
 
 }

zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gkontr.ravno(),naim_gr_kontr.ravno(),ff_k);

FILE *ff_kbm;
char imaf_kbm[64];
sprintf(imaf_kbm,"aspgk%d.lst",getpid());
if((ff_kbm=fopen(imaf_kbm,"w")) == NULL)
 {
  error_op_nfil(imaf_kbm,errno,"");
  return(1);
 
 }

zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gkontr.ravno(),naim_gr_kontr.ravno(),ff_kbm);


kolstr1=0;

double start_deb=0.,start_kre=0.;
double oborot_deb=0.,oborot_kre=0.;

for(int ii=0; ii < spisok_shet.kolih(); ii++)
 {
  strzag(LINES-1,0,spisok_shet.kolih(),++kolstr1);
  akt_sv.shet.new_plus(spisok_shet.ravno(ii));  
  buhspk_rsh(dn,mn,gn,dk,mk,gk,&akt_sv);

  buhspk_rg(dn,mn,gn,&akt_sv,ff);
  buhspk_rg_k(0,dn,mn,gn,&akt_sv,ff_k);
  buhspk_rg_k(1,dn,mn,gn,&akt_sv,ff_kbm);

  start_deb+=akt_sv.start_saldo_deb.suma();
  start_kre+=akt_sv.start_saldo_kre.suma();

  oborot_deb+=akt_sv.suma_deb.suma();
  oborot_kre+=akt_sv.suma_kre.suma();
  
  akt_sv.clear();
 }

if(spisok_shet.kolih() > 1)
 {
  fprintf(ff,"\n%s:\n",gettext("Общий итог"));
  fprintf(ff_k,"\n%s:\n",gettext("Общий итог"));
  fprintf(ff_kbm,"\n%s:\n",gettext("Общий итог"));

  fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
  fprintf(ff_k,"\
-------------------------------------------------- ---------------------------------------------------\n");
  fprintf(ff_kbm,"\
-------------------------------------------------- ---------------------------------------------------\n");

  if(start_deb > start_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
    start_deb-start_kre," ");
    fprintf(ff_k,"%31s %10.2f %10s| |\n",
    gettext("Сальдо начальное"),start_deb-start_kre," ");
    fprintf(ff_kbm,"%31s %10.2f %10s| |\n",
    gettext("Сальдо начальное"),start_deb-start_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
    fprintf(ff_k,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
    fprintf(ff_kbm,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
   }

  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),
  oborot_deb,oborot_kre);
  
  fprintf(ff_k,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),oborot_deb,oborot_kre);

  fprintf(ff_kbm,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),oborot_deb,oborot_kre);

  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),
  start_deb+oborot_deb,start_kre+oborot_kre);
  
  fprintf(ff_k,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),start_deb+oborot_deb,start_kre+oborot_kre);
  fprintf(ff_kbm,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),start_deb+oborot_deb,start_kre+oborot_kre);

  if(start_deb+oborot_deb > start_kre+oborot_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    start_deb+oborot_deb - start_kre-oborot_kre," ");
    fprintf(ff_k,"%*s %10.2f %10s| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),start_deb+oborot_deb - start_kre-oborot_kre," ");
    fprintf(ff_kbm,"%*s %10.2f %10s| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),start_deb+oborot_deb - start_kre-oborot_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);
    fprintf(ff_k,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);
    fprintf(ff_kbm,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);

   }
 }
  
konact(ff);
fclose(ff);

konact(ff_k);
fclose(ff_k);

konact(ff_kbm);
fclose(ff_kbm);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Акт сверки по группе контрагентов"));
oth->spis_imaf.plus(imaf_k);
oth->spis_naim.plus(gettext("Акт сверки по группе контрагентов с общими суммами по документам"));
oth->spis_imaf.plus(imaf_kbm);
oth->spis_naim.plus(gettext("Акт сверки с общими суммами по документам без месячных итогов"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);
}
/*****************************/
/*Расчёт по счёту*/
/****************************/
void buhspk_rsh(short dn,short mn,short gn,
short dk,short mk,short gk,
class akt_sverki_po_grup *akt_sv)
{
char strsql[512];
int kolstr;
SQL_str row;
class SQLCURSOR cur;

short d=1,m=1,g=gn;
if(startgodb != 0 && gn > startgodb)
 g=startgodb;

//читаем сальдо по контрагентам
sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",
gn,akt_sv->shet.ravno());


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

int nomer_kontr=0;

while(cur.read_cursor(&row) != 0)
 {
  if((nomer_kontr=akt_sv->spisok_kontr.find(row[0])) < 0)
   continue;

  akt_sv->start_saldo_deb.plus(atof(row[1]),nomer_kontr);
  akt_sv->start_saldo_kre.plus(atof(row[2]),nomer_kontr);
  
 }  



int nomer_prov=0;
class iceb_tu_str koment("");

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"select kodkon,nomd,deb,kre,komen from Prov where datp='%04d-%02d-%02d' and sh='%s'",g,m,d,akt_sv->shet.ravno());
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  
  
  while(cur.read_cursor(&row) != 0)
   {
    if((nomer_kontr=akt_sv->spisok_kontr.find(row[0])) < 0)
     continue;

    if(sravmydat(dn,mn,gn,d,m,g) > 0)
     {
      akt_sv->start_saldo_deb.plus(atof(row[2]),nomer_kontr);
      akt_sv->start_saldo_kre.plus(atof(row[3]),nomer_kontr);
      continue;     
     }
    
    koment.new_plus(row[1]);
    koment.plus(" ",row[4]);

    sprintf(strsql,"%d.%d.%d|%s|%s",d,m,g,row[1],row[0]);


    if(row[1][0] == '\0')
       nomer_prov=-1;
    else
       nomer_prov=akt_sv->data_nomd_kontr.find(strsql);

    if(nomer_prov < 0)
     {
      akt_sv->data_nomd_kontr.plus(strsql);
      if(strstrm(koment.ravno(),"-ПДВ") == 1) //есть образец в строке
       {
        polen(koment.ravno(),&akt_sv->koment,1,'-');
//        akt_sv->koment.plus(strsql);
        
       }
      else
        akt_sv->koment.plus(koment.ravno());
     }

    akt_sv->suma_deb.plus(atof(row[2]),nomer_prov);        
    akt_sv->suma_kre.plus(atof(row[3]),nomer_prov);        
   }  

  dpm(&d,&m,&g,1);
 }




}
/******************************/
/*Распечатка целиком по cчёту*/
/**************************/
void buhspk_rg(short dn,short mn,short gn,
class akt_sverki_po_grup *akt_sv,FILE *ff)
{

zagsh(akt_sv->shet.ravno(),ff);

double start_deb=0.;
double start_kre=0.;

start_deb=akt_sv->start_saldo_deb.suma();
start_kre=akt_sv->start_saldo_kre.suma();

if(start_deb > start_kre)
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
  dn,mn,gn,
  iceb_tu_kolbait(20,gettext("Сальдо начальное")),
  iceb_tu_kolbait(20,gettext("Сальдо начальное")),
  gettext("Сальдо начальное"),
  start_deb-start_kre," ");
else
 fprintf(ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
 dn,mn,gn,
 iceb_tu_kolbait(20,gettext("Сальдо начальное")),
 iceb_tu_kolbait(20,gettext("Сальдо начальное")),
 gettext("Сальдо начальное"),
 " ",start_kre-start_deb);


fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
short d,m,g;
short mz=0,gz=0;
double deb_oborot=0.;
double kre_oborot=0.;
double deb_ob_mes=0.;
double kre_ob_mes=0.;
double deb,kre;
 
for(int ii=0; ii < akt_sv->data_nomd_kontr.kolih(); ii++)
 {
  rsdat(&d,&m,&g,akt_sv->data_nomd_kontr.ravno(ii),1);
  if(mz != m || gz != g)
   {
     if(mz != 0 )
       buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);

    deb_ob_mes=kre_ob_mes=0.;    
    mz=m;
    gz=g;
   }     
  
  deb=akt_sv->suma_deb.ravno(ii);
  kre=akt_sv->suma_kre.ravno(ii);
  
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
  d,m,g,
  iceb_tu_kolbait(20,akt_sv->koment.ravno(ii)),
  iceb_tu_kolbait(20,akt_sv->koment.ravno(ii)),
  akt_sv->koment.ravno(ii),
  deb,kre);

  fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
  deb_oborot+=deb;
  kre_oborot+=kre;
  deb_ob_mes+=deb;
  kre_ob_mes+=kre;
 }

buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);



}

/******************************/
/*Распечатка по cчёту в разре по контрагентам*/
/**************************/
void buhspk_rg_k(int metka_mi,
short dn,short mn,short gn,
class akt_sverki_po_grup *akt_sv,FILE *ff)
{

zagsh(akt_sv->shet.ravno(),ff);

double start_deb=0.;
double start_kre=0.;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str naim_kontr("");
class iceb_tu_str kod_kontr("");
double i_start_deb=0.;
double i_start_kre=0.;
double i_oborot_deb=0.;
double i_oborot_kre=0.;
int kolih_r_k=0;
for(int kkk=0; kkk < akt_sv->spisok_kontr.kolih(); kkk++)
 {
  //проверяем есть ли этот код в списке счёта
  sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",akt_sv->shet.ravno(),akt_sv->spisok_kontr.ravno(kkk));
  if(readkey(strsql) != 1)
   continue;
   
  naim_kontr.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",akt_sv->spisok_kontr.ravno(kkk));
  if(readkey(strsql,&row,&cur) == 1)
   naim_kontr.new_plus(row[0]);
   
  
  fprintf(ff,"\n%s %s\n",akt_sv->spisok_kontr.ravno(kkk),naim_kontr.ravno());
  
  start_deb=akt_sv->start_saldo_deb.ravno(kkk);
  start_kre=akt_sv->start_saldo_kre.ravno(kkk);

  if(start_deb > start_kre)
    fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
    dn,mn,gn,
    iceb_tu_kolbait(20,gettext("Сальдо начальное")),
    iceb_tu_kolbait(20,gettext("Сальдо начальное")),
    gettext("Сальдо начальное"),
    start_deb-start_kre," ");
  else
   fprintf(ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
   dn,mn,gn,
   iceb_tu_kolbait(20,gettext("Сальдо начальное")),
   iceb_tu_kolbait(20,gettext("Сальдо начальное")),
   gettext("Сальдо начальное"),
   " ",start_kre-start_deb);


  fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
  short d,m,g;
  short mz=0,gz=0;
  double deb_oborot=0.;
  double kre_oborot=0.;
  double deb_ob_mes=0.;
  double kre_ob_mes=0.;
  double deb,kre;
   
  for(int ii=0; ii < akt_sv->data_nomd_kontr.kolih(); ii++)
   {
    polen(akt_sv->data_nomd_kontr.ravno(ii),&kod_kontr,3,'|');
    if(SRAV(kod_kontr.ravno(),akt_sv->spisok_kontr.ravno(kkk),0) != 0)
     continue;
    
    rsdat(&d,&m,&g,akt_sv->data_nomd_kontr.ravno(ii),1);
    if(mz != m || gz != g)
     {
      if(metka_mi == 0)
        if(mz != 0)
         buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
      deb_ob_mes=kre_ob_mes=0.;    
      mz=m;
      gz=g;
     }     
    
    deb=akt_sv->suma_deb.ravno(ii);
    kre=akt_sv->suma_kre.ravno(ii);
    
    fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
    d,m,g,
    iceb_tu_kolbait(20,akt_sv->koment.ravno(ii)),
    iceb_tu_kolbait(20,akt_sv->koment.ravno(ii)),
    akt_sv->koment.ravno(ii),
    deb,kre);

    fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
    deb_oborot+=deb;
    kre_oborot+=kre;
    deb_ob_mes+=deb;
    kre_ob_mes+=kre;
   }

  if(metka_mi == 1)
    deb_ob_mes=kre_ob_mes=0.;
  buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
  kolih_r_k++;
  i_oborot_deb+=deb_oborot;
  i_oborot_kre+=kre_oborot;
  i_start_deb+=start_deb;
  i_start_kre+=start_kre;
  
 }

if(kolih_r_k > 1)
 {
  fprintf(ff,"%s %s:\n",gettext("Общий итог по счёту"),akt_sv->shet.ravno());
  fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");

  if(i_start_deb > i_start_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),i_start_deb-i_start_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",
    iceb_tu_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",i_start_kre-i_start_deb);
   }

  buhspk_it(i_start_deb,i_start_kre,i_oborot_deb,i_oborot_kre,0.,0.,ff);
 }

}


/*********************/
/*Выдача итога*/
/*********************/

void buhspk_it(double deb_start,double kre_start,
double deb_oborot,double kre_oborot,
double deb_ob_mes,double kre_ob_mes,
FILE *ff)
{

double brr=deb_start+deb_oborot;
double krr=kre_start+kre_oborot;

if(deb_ob_mes != 0. || kre_ob_mes != 0.)
 {
  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Оборот за месяц")),
  gettext("Оборот за месяц"),
  deb_ob_mes,kre_ob_mes);
 }

 fprintf(ff,"%*s %10.2f %10.2f| |\n",
 iceb_tu_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),deb_oborot,kre_oborot);


 fprintf(ff,"%*s %10.2f %10.2f| |\n",
 iceb_tu_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(brr >= krr)
 {
  brr=brr-krr;
   fprintf(ff,"%*s %10.2f %10s| |\n",
   iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=krr-brr;
  fprintf(ff,"%*s %10s %10.2f| |\n",
  iceb_tu_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);
 }
}
