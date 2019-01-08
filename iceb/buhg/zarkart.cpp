/*$Id: zarkart.c,v 1.50 2013/09/26 09:43:45 sasa Exp $*/
/*28.03.2018	15.02.2002	Белых А.И.	zarkart.c
Распечатка карточки по начислениям/удержаниям за период
*/
#include        <errno.h>
#include        "buhg.h"
class zarkart_ov
 {
  public:

   class masiv_din_int MVT; /*коды видов табеля*/
   class masiv_din_double MVTRD; /*рабочие дни*/
   class masiv_din_double MVTHS; /*отработанные часы*/
   class masiv_din_double MVTKD; /*календарные дни*/
   int kolmes;
 };
 
class kk
 {
  public:
  class iceb_tu_str fio;
  class iceb_tu_str dolg;
  class iceb_tu_str inn;
  int		podr;
  int		sovm;
  short		dn,mn,gn;
  short		dk,mk,gk;

  kk()
   {
    clear_data();
   }
  void clear_data()
   {
    fio.new_plus("");
    dolg.new_plus("");
    inn.new_plus("");
    podr=0;
    sovm=0;
    dn=mn=gn=0;
    dk=mk=gk=0;
   }
 };

int zarkartr(short,short,short,short,const char*,const char*,const char*,class spis_oth*);
int sprmatpom(short,short,short,short,const char*,const char*,const char*,class spis_oth*);

void   	zarkartp(long,class kk*,short,short,int,masiv_din_int*,
masiv_din_int*,masiv_din_double*,masiv_din_double*,double*,
class SPISOK*,masiv_din_double*,double*,double*,class SPISOK*,
masiv_din_double*,double*,double*,int,FILE*,FILE*,FILE*,FILE*);

void zarkartpr(short,short,int,masiv_din_int*,masiv_din_double*,double*,int,FILE*);

void vidtab(short,short,int,masiv_din_int*,masiv_din_double*,masiv_din_double*,masiv_din_double*,int,int);

void rasotrvr(short,short,int,masiv_din_int*,masiv_din_double*,int,FILE*);

void zarkartprs(short,short,int,class SPISOK*,masiv_din_double*,double*,double*,int,int,FILE*);

void zarkart_tf6(int tabn,short mn,short gn,short mk,short gk,int kolmes,class masiv_din_int *MKN,class masiv_din_double *MKND,class masiv_din_int *MKU,class masiv_din_double *MKUD,class zarkart_ov *otr_vrem,FILE *ff);

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

void zarkart(int metka) //0-карточки 1-материальная помощь
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str tabnom("");
int		kom=0,kom1=0;
class iceb_tu_str bros("");
short		mn,gn;
short		mk,gk;
class iceb_tu_str fiov("");
VVOD MENU(3);
VVOD VV(0);

if(metka == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка карточек с начислениями/удержаниями по работникам"));
if(metka == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка работников, которым начислена материальная помощь"));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);

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
kategr.new_plus(VV.data_ravno(3));
tabnom.new_plus(VV.data_ravno(4));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   if(metka == 0)
     bros.new_plus("zarp4_3_12.txt");
   if(metka == 1)
     bros.new_plus("zarp4_3_11.txt");

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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
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
        vibrek("Kateg",&kategr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kategr.ravno());
        goto naz;

      case 2 :
      
        fiov.new_plus("");
        int tmm=0;
        if(dirtab(&tmm,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tmm);
          VV.VVOD_SPISOK_zapis_data(4,tabnom.ravno());
         }
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

if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата конца !"));
  goto naz;
 }
class spis_oth oth;

if(metka == 0)
  if(zarkartr(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),tabnom.ravno(),&oth) == 0)
    iceb_t_rabfil(&oth,"");

if(metka == 1)
  if(sprmatpom(mn,gn,mk,gk,podr.ravno(),kategr.ravno(),tabnom.ravno(),&oth) == 0)
   iceb_t_rabfil(&oth,"");

}
/**********************/
/*Расчёт с распечаткой*/
/**********************/
int zarkartr(short mn,short gn,short mk,short gk,
const char *podr,const char *kateg,const char *tabnom,class spis_oth *oth)
{
int	        kon=0,kou=0;
char		strsql[1024];
SQL_str		row,row1;
int		kolstr=0,kolstr1=0;
short		prn=0;
int		knah=0;
short		d,m,g;
double		suma;
int		nommes=0;
long		tabn=0,tabnz=0;
short		mesqc=0;
int		nomq=0;
FILE		*ff,*ff_spiso,*ffo=NULL,*ffb=NULL;
char		imaf[64],imafo[64],imafb[64];
class kk kart;
time_t		tmmn=0;
int		kolns=0,kolus=0;
time(&tmmn);

clear();
printw("%s\n",gettext("Распечатка карточек с начислениями/удержаниями по работникам"));
printw("%d.%d => %d.%d\n",mn,gn,mk,gk);
GDITE();
/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash order by kod asc");
SQLCURSOR cur;
SQLCURSOR cur1;
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

class masiv_din_int MKN; //Массив кодов начислений
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder order by kod asc");

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

class masiv_din_int MKU(0); //Массив кодов удержаний

while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

class masiv_din_double MKND; //Данные начислений
class masiv_din_double MKUD; //Данные удержаний
class zarkart_ov otr_vrem;

otr_vrem.kolmes=period(1,mn,gn,30,mk,gk,1);

MKND.make_class(kon*otr_vrem.kolmes);
MKUD.make_class(kou*otr_vrem.kolmes);
sprintf(strsql,"select kod from Tabel order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s !",gettext("Не введён список видов табелей"));
  iceb_t_soob(strsql);
 }
//class masiv_din_int MVT(0); //Массив кодов табеля отработанного времени

while(cur.read_cursor(&row) != 0)
  otr_vrem.MVT.plus(row[0]);
//  MVT.plus(atoi(row[0]),-1);

//class masiv_din_double MVTRD; //Массив рабочих дней
//class masiv_din_double MVTHS; //Массив часов
//class masiv_din_double MVTKD; //Массив календарных дней

otr_vrem.MVTRD.make_class(kolstr*otr_vrem.kolmes);
otr_vrem.MVTHS.make_class(kolstr*otr_vrem.kolmes);
otr_vrem.MVTKD.make_class(kolstr*otr_vrem.kolmes);

class SPISOK MKNS; //массив начислений/счет
class SPISOK MKUS; //массив удержаний/счет
class masiv_din_double MKNSD; //данные начислений/счет
class masiv_din_double MKUSD; //данные удержания/счет

memset(imafo,'\0',sizeof(imafo));
memset(imafb,'\0',sizeof(imafb));

if(shetb != NULL &&  shetbu != NULL)
 {

  sprintf(strsql,"select distinct prn,knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and suma <> 0.",gn,mn,gk,mk);

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

  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    sprintf(strsql,"%s|%s",row[1],row[2]);
    if(prn == 1)
     { 
      MKNS.plus(strsql);
      kolns++;
     }
    if(prn == 2)
     {
      MKUS.plus(strsql);
      kolus++;
     }
   }
  MKNSD.make_class(kolns*otr_vrem.kolmes);
  MKUSD.make_class(kolus*otr_vrem.kolmes);

  sprintf(imafo,"krb%d-%d_%d.lst",mn,mk,getpid());

  if((ffo = fopen(imafo,"w")) == NULL)
   {
    error_op_nfil(imafo,errno,"");
    return(1);
   }

  sprintf(imafb,"krn%d-%d_%d.lst",mn,mk,getpid());

  if((ffb = fopen(imafb,"w")) == NULL)
   {
    error_op_nfil(imafb,errno,"");
    return(1);
   }
  
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"zarkart"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
fio char(112) not null,\
datz DATE not null,\
tbn int not null,\
prn smallint not null,\
knah int not null,\
suma double(14,2) not null,\
shet char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

sprintf(strsql,"select datz,tabn,prn,knah,suma,shet from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-31' and suma <> 0. \
order by tabn asc,datz asc",gn,mn,gk,mk);

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
class iceb_tu_str fio("");
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    fio.new_plus(row1[0]);

  sprintf(strsql,"insert into %s values ('%s','%s',%s,%s,%s,%s,'%s')",
  imatmptab,
  fio.ravno_filtr(),row[0],row[1],row[2],row[3],row[4],row[5]);

  sql_zapis(strsql,1,0);    

 }

sprintf(strsql,"select * from %s order by fio asc,tbn asc,datz asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }



sprintf(imaf,"krt%d-%d_%d.lst",mn,mk,getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }


char imaf_spiso[64];
sprintf(imaf_spiso,"krtso%d-%d_%d.lst",mn,mk,getpid());

if((ff_spiso = fopen(imaf_spiso,"w")) == NULL)
 {
  error_op_nfil(imaf_spiso,errno,"");
  return(1);
 }
char imaf_tf6[64];
sprintf(imaf_tf6,"tf6%d.lst",getpid());

class iceb_fopen tf6;
if(tf6.start(imaf_tf6,"w") != 0)
 return(1);
 
short dkm=1;
dpm(&dkm,&mk,&gk,5);

iceb_t_zagolov(gettext("Список работников"),1,mn,gn,dkm,mk,gk,ff_spiso);

fprintf(ff_spiso,"\
---------------------------------------------------\n");

fprintf(ff_spiso,gettext("\
  N   |Таб.ном.|Инд.нал.но| Фамилия  Имя  Отчество\n"));
fprintf(ff_spiso,"\
---------------------------------------------------\n");


double	saldon[otr_vrem.kolmes];
double	saldono1[otr_vrem.kolmes];
double	saldonb1[otr_vrem.kolmes];
double	saldono2[otr_vrem.kolmes];
double	saldonb2[otr_vrem.kolmes];
memset(saldon,'\0',sizeof(saldon));
memset(saldono1,'\0',sizeof(saldono1));
memset(saldonb1,'\0',sizeof(saldonb1));
memset(saldono2,'\0',sizeof(saldono2));
memset(saldonb2,'\0',sizeof(saldonb2));

double saldb=0;
kolstr1=0;
class iceb_tu_str tabnom1("");
class iceb_tu_str shet("");
SQLCURSOR curr;
int nomer_zap=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  tabnom1.new_plus(row[2]);

  if(proverka(tabnom,tabnom1.ravno(),0,0) != 0)
    continue;

  tabn=tabnom1.ravno_atoi();

  rsdat(&d,&m,&g,row[1],2);    


  if(tabnz != tabn)
   {
    if(tabnz != 0)
     {
      zarkartp(tabnz,&kart,mn,gn,otr_vrem.kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
      &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,++nomer_zap,
      ff,ffb,ffo,ff_spiso);

      zarkart_tf6(tabnz,mn,gn,mk,gk,otr_vrem.kolmes,&MKN,&MKND,&MKU,&MKUD,&otr_vrem,tf6.ff);
      fprintf(tf6.ff,"\f");

      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTRD,1,ff);
      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTHS,2,ff);
      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTKD,3,ff);

      fprintf(ff,"\f");


      
      if(ffo != NULL)
       fprintf(ffo,"\f");
      if(ffb != NULL)
       fprintf(ffb,"\f");

      MKND.clear_class();
      MKUD.clear_class();

      otr_vrem.MVTRD.clear_class();
      otr_vrem.MVTHS.clear_class();
      otr_vrem.MVTKD.clear_class();

      memset(&saldon,'\0',sizeof(saldon));
      memset(&saldono1,'\0',sizeof(saldono1));
      memset(&saldonb1,'\0',sizeof(saldonb1));
      memset(&saldono2,'\0',sizeof(saldono2));
      memset(&saldonb2,'\0',sizeof(saldonb2));
     }

    kart.clear_data();
    sprintf(strsql,"select * from Kartb where tabn=%s",tabnom1.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) < 1)
     {
      printw("%s %s!",gettext("Не найден табельный номер"),tabnom1.ravno());
      OSTANOV();
     }  
    else
     {    
      if(proverka(podr,row1[4],0,0) != 0)
       {
        tabnz=0;
        continue;
       }
      if(proverka(kateg,row1[5],0,0) != 0)
       {
        tabnz=0;
        continue;
       }    
      kart.fio.new_plus(row1[1]);
      kart.dolg.new_plus(row1[2]);
      kart.podr=atoi(row1[4]);
      rsdat(&kart.dn,&kart.mn,&kart.gn,row1[6],2);
      rsdat(&kart.dk,&kart.mk,&kart.gk,row1[7],2);
      kart.inn.new_plus(row1[10]);
      kart.sovm=atoi(row1[9]);    
     }    
    tabnz=tabn;
    mesqc=0;
   }

  if(mesqc != m)
   {

    nommes=period(1,mn,gn,30,m,g,1);
    saldon[nommes-1]=zarsald(1,m,g,tabn,&saldb);
    saldono1[nommes-1]=saldon[nommes-1];
    saldonb1[nommes-1]=saldb;
    saldono2[nommes-1]=saldon[nommes-1];
    saldonb2[nommes-1]=saldb;
    vidtab(m,g,tabn,&otr_vrem.MVT,&otr_vrem.MVTRD,&otr_vrem.MVTHS,&otr_vrem.MVTKD,nommes,otr_vrem.kolmes);
    mesqc=m;
   }

  prn=atoi(row[3]);
  knah=atoi(row[4]);
  suma=atof(row[5]);
  shet.new_plus(row[6]);

  if(prn == 1)
   {
    nomq=MKN.find(knah);
//    printw("prn=%d %d.%d otr_vrem.kolmes=%d nomq=%d nommes=%d\n",prn,m,g,otr_vrem.kolmes,nomq,nommes);
    MKND.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet.ravno());
      nomq=MKNS.find(strsql);
//      printw("kolns=%d nomq=%d nommes=%d\n",kolns,nomq,nommes);
      MKNSD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
     }
   }
  if(prn == 2)
   {
    nomq=MKU.find(knah);
//    printw("prn=%d %d.%d otr_vrem.kolmes=%d nomq=%d nommes=%d\n",prn,m,g,otr_vrem.kolmes,nomq,nommes);
    MKUD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet.ravno());
      nomq=MKUS.find(strsql);
//      printw("kolus=%d nomq=%d nommes=%d\n",kolus,nomq,nommes);
      MKUSD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
     }
   }
 }

if(tabnz != 0)
 {
  zarkartp(tabnz,&kart,mn,gn,otr_vrem.kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
  &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,++nomer_zap,
  ff,ffb,ffo,ff_spiso);

  zarkart_tf6(tabnz,mn,gn,mk,gk,otr_vrem.kolmes,&MKN,&MKND,&MKU,&MKUD,&otr_vrem,tf6.ff);

  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTRD,1,ff);
  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTHS,2,ff);
  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTKD,3,ff);


 }
podpis(ff);
podpis(ff_spiso);

fclose(ff);
fclose(ff_spiso);
tf6.end();

if(shetb != NULL &&  shetbu != NULL)
 {
  podpis(ffo);

  podpis(ffb);

  fclose(ffo);
  fclose(ffb);
 }


printw_vr(tmmn);


oth->spis_imaf.plus(imaf);

oth->spis_naim.plus(gettext("Распечатка карточек с начислениями/удержаниями по работникам"));

oth->spis_imaf.plus(imaf_tf6);

oth->spis_naim.plus(gettext("Типовая форма N6"));

oth->spis_imaf.plus(imaf_spiso);

oth->spis_naim.plus(gettext("Список работников"));

if(shetb != NULL &&  shetbu != NULL)
 {
  oth->spis_imaf.plus(imafo);
  oth->spis_naim.plus(gettext("Хозрасчёт"));
  oth->spis_imaf.plus(imafb);
  oth->spis_naim.plus(gettext("Бюджет"));
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);

}

/************************/
/*Распечатка содержимого*/
/************************/
void zarkartp(long tabn,
class kk *kart,
short mn,short gn,
int kolmes,
class masiv_din_int *MKN,
class masiv_din_int *MKU,
class masiv_din_double *MKND,
class masiv_din_double *MKUD,
double *saldon,
class SPISOK *MKNS,
class masiv_din_double *MKNSD,
double *saldono1,
double *saldonb1,
class SPISOK *MKUS,
class masiv_din_double *MKUSD,
double *saldono2,
double *saldonb2,
int nomer_zap,
FILE *ff,
FILE *ffb,
FILE *ffo,
FILE *ff_spiso)
{

printw("\n%5ld %s\n",tabn,kart->fio.ravno());

fprintf(ff,"%s\n%s: %ld %s\n",iceb_t_get_pnk("00",0),gettext("Табельный номер"),
tabn,kart->fio.ravno());

fprintf(ff_spiso,"%6d %8ld %10s %s\n",nomer_zap,tabn,kart->inn.ravno(),kart->fio.ravno());

fprintf(ff,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
kart->inn.ravno());
fprintf(ff,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
if(kart->sovm == 1)
  fprintf(ff,"%s\n",gettext("Совместитель"));

if(kart->dn != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
  kart->dn,kart->mn,kart->gn,gettext("г."));

if(kart->dk != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
  kart->dk,kart->mk,kart->gk,gettext("г."));
  
zarkartpr(mn,gn,kolmes,MKN,MKND,saldon,1,ff);
zarkartpr(mn,gn,kolmes,MKU,MKUD,saldon,2,ff);

if(shetb != NULL &&  shetbu != NULL)
 {
  fprintf(ffb,"%s: %ld %s\n",gettext("Табельный номер"),
  tabn,kart->fio.ravno());

  fprintf(ffb,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart->inn.ravno());
  fprintf(ffb,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
  if(kart->sovm == 1)
    fprintf(ffb,"%s\n",gettext("Совместитель"));

  if(kart->dn != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart->dn,kart->mn,kart->gn,gettext("г."));

  if(kart->dk != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart->dk,kart->mk,kart->gk,gettext("г."));
  fprintf(ffb,"%s.\n",gettext("Бюджет"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono1,saldonb1,1,1,ffb);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono1,saldonb1,2,1,ffb);

  fprintf(ffo,"%s: %ld %s\n",gettext("Табельный номер"),
  tabn,kart->fio.ravno());

  fprintf(ffo,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart->inn.ravno());
  fprintf(ffo,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
  if(kart->sovm == 1)
    fprintf(ffo,"%s\n",gettext("Совместитель"));

  if(kart->dn != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart->dn,kart->mn,kart->gn,gettext("г."));

  if(kart->dk != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart->dk,kart->mk,kart->gk,gettext("г."));
  fprintf(ffo,"%s.\n",gettext("Небюджет"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono2,saldonb2,1,2,ffo);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono2,saldonb2,2,2,ffo);

  MKNSD->clear_class();
  MKUSD->clear_class();
 }

}
/********************************/
/*Распечатка конкретного массива*/
/********************************/
void zarkartpr(short mn,short gn,
int kolmes,
class masiv_din_int *MKN,
class masiv_din_double *MKND,
double *saldo,
int metka, //1-начисления 2-удержания
FILE *ff)
{
class iceb_tu_str stroka;
short		d,m,g;
int		kon=0;
int		i,ii;
int		nome=0;
char		strsql[1024];
SQL_str		row;
class iceb_tu_str naim("");
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
SQLCURSOR curr;

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("---------");


fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
  fprintf(ff,gettext("Код|   Наименование начисления    |"));
if(metka == 2)
  fprintf(ff,gettext("Код|   Наименование удержания     |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",
  iceb_tu_kolbait(34,gettext("Сальдо начальное")),iceb_tu_kolbait(34,gettext("Сальдо начальное")),gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  nome=MKN->ravno(i);
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",nome);
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);      
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno());
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

if(metka == 1)
 fprintf(ff,"%*s|",iceb_tu_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_tu_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   saldo[ii]+=sumai[ii];
  if(metka == 2)
   saldo[ii]-=sumai[ii];
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka.ravno());
  fprintf(ff,"%*.*s|",
  iceb_tu_kolbait(34,gettext("Сальдо конечное")),iceb_tu_kolbait(34,gettext("Сальдо конечное")),gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

}
/***********************************/
/*Определение отработанного времени*/
/***********************************/

void vidtab(short mr,short gr,int tabnom,
class masiv_din_int *MVT,
class masiv_din_double *MVTRD,
class masiv_din_double *MVTHS,
class masiv_din_double *MVTKD,
int nommes,
int kolmes)
{
char		strsql[1024];
SQL_str		row;
int		kolstr;
int		kodt=0;
int		nomq=0;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and mes=%d and tabn=%d",gr,mr,tabnom);
class SQLCURSOR cur;
//printw("%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  kodt=atoi(row[0]);
  nomq=MVT->find(kodt);
  MVTRD->plus(atof(row[1]),kolmes*nomq+nommes-1);
  MVTHS->plus(atof(row[2]),kolmes*nomq+nommes-1);
  MVTKD->plus(atof(row[3]),kolmes*nomq+nommes-1);
 }

}
/**********************************/
/*Распечатка отработанного времени*/
/**********************************/
void rasotrvr(short mn,short gn,int kolmes,
class masiv_din_int *MVT,
class masiv_din_double *MVTRD,
int metka, //1-рабочие дни 2-часы 3-календарные дни
FILE *ff)
{
class iceb_tu_str stroka;
short		d,m,g;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
int		i=0,ii=0;
int		koltb;
double		suma=0.;
char		strsql[1024];
SQL_str		row;
class iceb_tu_str naim("");
int		nome;
SQLCURSOR curr;

koltb=MVT->kolih();
for(i=0; i < koltb*kolmes; i++)
 if((suma=MVTRD->ravno(i)) != 0.)
     break;
if(suma == 0.)
  return;
suma=0.;  
if(metka == 1)
 fprintf(ff,"%s:\n",gettext("Свод отработанных рабочих дней"));
if(metka == 2)
 fprintf(ff,"%s:\n",gettext("Свод отработанных часов"));
if(metka == 3)
 fprintf(ff,"%s:\n",gettext("Свод календарных дней"));

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("--------");


fprintf(ff,"%s\n",stroka.ravno());
fprintf(ff,gettext("Код|   Наименование табеля        |"));

d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff,"%.02d.%d|",m,g);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",stroka.ravno());
memset(&sumai,'\0',sizeof(sumai));


for(i=0; i < koltb; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MVTRD->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  nome=MVT->ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno());
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MVTRD->ravno(kolmes*i+ii);
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%7.f|",suma);
   }
  fprintf(ff,"%7.f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"%*s|",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%7.f|",sumai[ii]);
 }
fprintf(ff,"%7.f|\n\n",sumagori);

//MVTRD->clear_class();

}
/********************************/
/*Распечатка конкретного массива счет*/
/********************************/
void   zarkartprs(short mn,short gn,
int kolmes,
class SPISOK *MKN,
class masiv_din_double *MKND,
double *saldo,
double *saldob,
int metka, //1-начисления 2-удержания
int metkab, //1-бюджет 2-небюджет
FILE *ff)
{
class iceb_tu_str stroka;
short		d,m,g;
int		kon=0;
int		i,ii;
char		strsql[1024];
SQL_str		row;
class iceb_tu_str naim("");
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
class iceb_tu_str bros("");
class iceb_tu_str knah("");
class iceb_tu_str shet("");
SQLCURSOR curr;
  

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("---------");


fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
  fprintf(ff,gettext("Код|Наименование начисления |Счёт |"));
if(metka == 2)
  fprintf(ff,gettext("Код|Наименование удержания  |Счёт |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",
  iceb_tu_kolbait(34,gettext("Сальдо начальное")),
  iceb_tu_kolbait(34,gettext("Сальдо начальное")),
  gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldob[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  bros.new_plus(MKN->ravno(i));
  polen(bros.ravno(),&knah,1,'|');
  polen(bros.ravno(),&shet,2,'|');

  if(metka == 1) //начисления
   if(metkab == 1) //бюджет
    if(proverka(shetb,shet.ravno(),0,0) != 0)
      continue;
  if(metka == 1) //начисления
   if(metkab == 2) //не бюджет
    if(proverka(shetb,shet.ravno(),0,0) == 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 1) //бюджет
    if(proverka(shetbu,shet.ravno(),0,0) != 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 2) //Не бюджет
    if(proverka(shetbu,shet.ravno(),0,0) == 0)
      continue;

    
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",knah.ravno());
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",knah.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3s %-*.*s|%-*s|",
  knah.ravno(),
  iceb_tu_kolbait(24,naim.ravno()),iceb_tu_kolbait(24,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,shet.ravno()),shet.ravno());
  
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

if(metka == 1)
 fprintf(ff,"%*s|",iceb_tu_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_tu_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   {
    saldo[ii]+=sumai[ii];
    if(metkab == 1)
      saldob[ii]+=sumai[ii];
   }
  if(metka == 2)
   {
    saldo[ii]-=sumai[ii];
    if(metkab == 1)
      saldob[ii]-=sumai[ii];
   }
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka.ravno());
  fprintf(ff,"%*.*s|",
  iceb_tu_kolbait(34,gettext("Сальдо конечное")),
  iceb_tu_kolbait(34,gettext("Сальдо конечное")),
  gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldo[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

}

/***********************************/
/*распечатка типовой формы 6*/
/*********************************/
void zarkart_tf6(int tabn,
short mn,short gn,
short mk,short gk,
int kolmes,
class masiv_din_int *MKN,
class masiv_din_double *MKND,
class masiv_din_int *MKU,
class masiv_din_double *MKUD,
class zarkart_ov *otr_vrem,

FILE *ff)
{
double suma=0.;
class masiv_din_int kod_nah;
class masiv_din_int kod_uder;
class masiv_din_double suma_nah;
class masiv_din_double suma_uder;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str naim("");

int kon=MKN->kolih();

for(int i=0; i < kon; i++)
 {
  suma=0.;

  for(int ii=0 ; ii < kolmes; ii++)
    suma+=MKND->ravno(kolmes*i+ii);

  if(suma == 0.)
    continue;
  kod_nah.plus(MKN->ravno(i));
  suma_nah.plus(suma);
 }

kon=MKU->kolih();

for(int i=0; i < kon; i++)
 {
  suma=0.;
  for(int ii=0 ; ii < kolmes; ii++)
    suma+=MKUD->ravno(kolmes*i+ii);
  if(suma == 0.)
    continue;
  kod_uder.plus(MKU->ravno(i));
  suma_uder.plus(suma);
 }
class iceb_tu_str naim_kontr(iceb_t_get_pnk("00",0));

fprintf(ff,"%-*s Типова форма N П-6\n",
iceb_tu_kolbait(60,naim_kontr.ravno()),
naim_kontr.ravno());

fprintf(ff,"%60s ЗАТВЕРДЖЕНО\n","");
fprintf(ff,"%60s наказом Деркомстату України\n","");
fprintf(ff,"%60s від 5 грудня 2008 р. N 489\n\n","");

class iceb_tu_str fio("");
class iceb_tu_str dolg("");
class iceb_tu_str indkod("");
  
fprintf(ff,"%30s РОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ ПРАЦІВНИКА\n","");


sprintf(strsql,"select fio,dolg,inn from Kartb where tabn=%d",tabn);
if(readkey(strsql,&row,&cur) == 1)
 {
  fio.new_plus(row[0]);
  dolg.new_plus(row[1]);
  indkod.new_plus(row[2]);
 }
 
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта включён
fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
fprintf(ff,"%14s %s\n","",fio.ravno());
fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
fprintf(ff,"\x1B\x46"); //режим выделенного шрифта выключен

fprintf(ff,"%30s Табельний номер:%d Ідентифікаційний код:%s\n","",tabn,indkod.ravno());
fprintf(ff,"%30s Посада:%s\n","",dolg.ravno());

short d;
dpm(&d,&mk,&gk,5);

fprintf(ff,"%30s за період з 01.%02d.%d р. по %02d.%02d.%d р.\n","",mn,gn,d,mk,gk);
double suma_str[3];

class iceb_tu_str naim_tabel("");
for(int nom_kod_tab=0; nom_kod_tab < otr_vrem->MVT.kolih(); nom_kod_tab++)
 {
   suma=0.;
   for(int nom_mes=0 ; nom_mes < otr_vrem->kolmes; nom_mes++)
     suma+=otr_vrem->MVTRD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes)+otr_vrem->MVTHS.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes)+otr_vrem->MVTKD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);

   if(suma == 0.)
     continue;

   suma_str[0]=suma_str[1]=suma_str[2]=0.;

   for(int nom_mes=0 ; nom_mes < otr_vrem->kolmes; nom_mes++)
    {
     suma_str[0]+=otr_vrem->MVTRD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
     suma_str[1]+=otr_vrem->MVTHS.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
     suma_str[2]+=otr_vrem->MVTKD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
    }


  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%d",otr_vrem->MVT.ravno(nom_kod_tab));

  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код табеля"),otr_vrem->MVT.ravno(nom_kod_tab));
    iceb_t_soob(strsql);
    naim_tabel.new_plus("");
    continue;
   }
  else
    naim_tabel.new_plus(row[0]);
  



  fprintf(ff,"%*.*s: %4.4g/%6.6g/%4.4g\n",
  iceb_tu_kolbait(40,naim_tabel.ravno()),  
  iceb_tu_kolbait(40,naim_tabel.ravno()),  
  naim_tabel.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2]);
 }
double saldo_start_b=0.;

double saldo_start=zarsald(1,mn,gn,tabn,&saldo_start_b);
fprintf(ff,"Caльдо на 01.%02d.%d: %.2f\n",mn,gn,saldo_start);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n\
| N |    Нараховано за видами оплат                 | N |    Утримано                                   |\n\
|п/п|-----------------------------------------------|п/п|-----------------------------------------------|\n\
|   |Код|вид оплати                    | нараховано |   |Код|вид утримання                 |  утримано  |\n\
|   |   |                              |    грн.    |   |   |                              |    грн.    |\n\
---------------------------------------------------------------------------------------------------------\n");

int kolih_nah=kod_nah.kolih();
int kolih_uder=kod_uder.kolih();

int kol=kolih_nah;
if(kolih_nah < kolih_uder)
 kol=kolih_uder;


double itogo_n=0;
double itogo_u=0;
for(int nom=0; nom < kol; nom++)
 {
  class SPISOK sn;
  class SPISOK su;
  
  if(nom < kolih_nah)
   {
    sprintf(strsql,"select naik from Nash where kod=%d",kod_nah.ravno(nom));

    if(readkey(strsql,&row,&cur) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");

    sprintf(strsql,"|%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_nah.ravno(nom),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma_nah.ravno(nom));

    sn.plus(strsql);
    for(int nom1=30; nom1 < iceb_tu_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"|%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_adrsimv(nom1,naim.ravno()),
      "");
  
      sn.plus(strsql);
     }   

    itogo_n+=suma_nah.ravno(nom);
   
       
   }  
  else
   {
    sprintf(strsql,"|%3s|%3s|%30s|%12s|","","","",""); sn.plus(strsql);
    sn.plus(strsql);
   }

  if(nom < kolih_uder)
   {
    sprintf(strsql,"select naik from Uder where kod=%d",kod_uder.ravno(nom));
    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");

    sprintf(strsql,"%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_uder.ravno(nom),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma_uder.ravno(nom));

    su.plus(strsql);

    for(int nom1=30; nom1 < iceb_tu_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_adrsimv(nom1,naim.ravno()),
      "");
  
      su.plus(strsql);
     }   
    itogo_u+=suma_uder.ravno(nom);



   }  
  else
   {
    sprintf(strsql,"%3s|%3s|%30s|%12s|","","","","");
    su.plus(strsql);    
   } 

//  fprintf(ff,"\n");
  int kolstrn=sn.kolih();
  int kolstru=su.kolih();
  int kolstr=kolstrn;
  if(kolstrn < kolstru)
   kolstr=kolstru;
  for(int nom1=0; nom1 < kolstr; nom1++)
   {
    if(nom1 < kolstrn)
     fprintf(ff,"%s",sn.ravno(nom1));
    else
     fprintf(ff,"|%3s|%3s|%30s|%12s|","","","","");
    if(nom1 < kolstru)
     fprintf(ff,"%s\n",su.ravno(nom1));
    else
     fprintf(ff,"%3s|%3s|%30s|%12s|\n","","","","");
   }   

  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

 }
fprintf(ff,"|%*s|%12.2f|%*s|%12.2f|\n",
iceb_tu_kolbait(38,"Разом"),
"Разом",
itogo_n,
iceb_tu_kolbait(38,"Разом"),
"Разом",
itogo_u);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"Cальдо на %02d.%02d.%d: %.2f\n",d,mk,gk,saldo_start+itogo_n+itogo_u);

class iceb_t_fioruk_rk gb;

iceb_t_fioruk(2,&gb);


//fprintf(ff,"\n%20sГоловний бухгалтер __________________________%s\n","",gb.fio.ravno());
fprintf(ff,"\n%20s         Бухгалтер __________________________%s\n","",iceb_t_getfioop());

podpis(ff);


}
