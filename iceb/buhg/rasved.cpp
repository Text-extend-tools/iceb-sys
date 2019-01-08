/* $Id: rasved.c,v 5.133 2013/09/26 09:43:39 sasa Exp $ */
/*28.03.2018    17.11.1992      Белых А.И.      rasved.c
Подпрограмма распечатки ведомости на зарплату с сортировкой по
возростанию табельного номера
Вертикальная форма
*/
#include        <errno.h>
#include        "buhg.h"
#define         STR1    16   /*Количество строк на 1-ом листе*/
#define         STR2    28  /*Количество строк на 2-ом листе*/
#define         STRKOR  76  /*Количество строк на листах с корешками*/

class koreshok
 {
  public:
   class SPISOK nahis;
   class SPISOK uder;
   class SPISOK viplati;
   double i_nahis;
   double i_uder;
   double i_viplati;
   double saldo;      

  void clear()
   {
    nahis.free_class();
    uder.free_class();
    viplati.free_class();
    saldo=i_nahis=i_uder=i_viplati=0.;
   }
 };



void            razbiv(char*,char*,char*,char*);
void            srasved(short,short,FILE*,struct tm*);
void rasved_kornuv(class koreshok *kor,FILE *ff);
void rasved_iknuv(class koreshok *kor,FILE *ff);
void rasved_rbknuv(char *imaf_nuv);

extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/
extern double   okrg; /*Округление*/
extern short    mmm,ggg;
extern short	*obud; /*Обязательные удержания*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int rasved(short ps,class spis_oth *oth) //Признак сортировки 0-по таб. номерам 1-по алфав.
{
double          itsal; /*Общая сумма сальдо*/
char st[512];
char str[512];
char            bros[512];
class iceb_tu_str naipod(""); /*Наименование подразделения*/
time_t          tmm;
int            tabb;   /*Табельный номер*/
double          inah,in1;   /*Итого начислено*/
double          iu=0.;   /*Итого удержано с учётом сальдо*/
double          iub=0.;   /*Итого удержано бюджета с учётом сальдо*/
double          ius=0.,ius1=0.;   /*Итого удержано без учёта сальдо*/
double          iusb=0.;   /*Итого удержано без учёта сальдо бюджета*/
double		iusb1=0.;  /*Итого удержано без учёта сальдо бюджета */
double		nahshet=0.; //Итого начислено бюджет
double          bb=0.,bb1=0.;
double          oin;   /*Всего начислено*/
double          oiu;   /*Всего удержано*/
double          itogo,itogo1,itogoo,itogoopb=0.; /*Обший итог по подразделению*/
int             kodnah;  /*Код начисления 1- начисление 2-удержание*/
double          bbr;
int             podr,kpd;
short           listn=0,listn1=0; /*Номер листа*/
short           kolst=0,kolst1=0; /*Счетчик количества строк*/
double          prom;
short           im;
short           kd;
FILE            *kaw,*kawk1,*kawk2,*ffvedom;
FILE            *ff=NULL,*ff2=NULL,*ffpodr=NULL,*ffpodrb=NULL,*ffkor2s=NULL;
FILE *ff_nuv;
char            imaf[64],imaf1[64],imaf2[64],imaf3[64],imaftmp[64];
char		imafkor1[64],imafkor2[64];
char		imafkor1t[64],imafkor2t[64];
char		imafpodr[64],imafpodrb[64];
char		imafkor2s[64];
char imaf_nuv[64];

short           mkvk; /*0 не выдано кассой 1-выдано кассой*/
short           uhsl; /*Учет сальдо в распечатке ведомости на зарплату 0-включено 1-выключено*/
double		sal;
long		kolstr,kolstr1,kolstr2,kolstr3;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str fio("");
class iceb_tu_str fiov("");
class iceb_tu_str inn("");
short		prn;
class iceb_tu_str naim("");
short		mz,gz;
double		sym;
int		knah;
static class iceb_tu_str tabel("");
static class iceb_tu_str kodn("");
static class iceb_tu_str podrz("");
static class iceb_tu_str dat1("");
static class iceb_tu_str shetbudget("");
static class iceb_tu_str kateg("");
static class iceb_tu_str grupa("");
static class iceb_tu_str procvip("");
double 		procentvp=100.;
short		mr,gr;
int		kom,kom1;
short		d,m,g;
double		saldb;
short		metkakas=0;
int		metkavs=0; //Метка выдачи шапки
int		metkavsb=0; //Метка выдачи шапки для бюджета
double		viplata=0.;
int bros_int=0; /*не используемая пока переменная в алгоритме*/

struct  tm      *bf;
SQLCURSOR curr;
tabb=0L;
kodnah=0;
nahshet=itsal=ius=iu=inah=in1=oin=oiu=0.;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
if(procvip.ravno()[0] == '\0')
 procvip.new_plus("100");
 
if(shetb != NULL)
  shetbudget.new_plus(shetb);
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка ведомости на выплату зарплаты"));
 
VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodn.ravno(),128);
VV.VVOD_SPISOK_add_data(tabel.ravno(),128);
VV.VVOD_SPISOK_add_data(kateg.ravno(),128);
VV.VVOD_SPISOK_add_data(shetbudget.ravno(),128);
VV.VVOD_SPISOK_add_data(procvip.ravno(),16);
VV.VVOD_SPISOK_add_data(grupa.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата.............(м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Подразделение......(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код категории......(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Счёт...............(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Процент выплаты.........."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код группы под-ния..(,,)."));//7


naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
podrz.new_plus(VV.data_ravno(1));
kodn.new_plus(VV.data_ravno(2));
tabel.new_plus(VV.data_ravno(3));
kateg.new_plus(VV.data_ravno(4));
shetbudget.new_plus(VV.data_ravno(5));
procvip.new_plus(VV.data_ravno(6));
grupa.new_plus(VV.data_ravno(7));

switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("zarp4_1_1.txt");
    goto naz;

  case PLU:
  case FK2:
     break;



  case FK3:
    
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка работников"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
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
        if(dirzarpodr(2,&podrz,&naim) == 0)
          VV.data_z_plus(1,podrz.ravno());
        goto naz;

      case 1 :
        if(dirnach(1,&kodn,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(2,kodn.ravno());
        goto naz;

      case 2 :

        clear();
        tabb=0;
        fiov.new_plus("");
        if(dirtab(&tabb,&fiov,0,1) == 0)
         {
          tabel.z_plus(tabb);
          VV.VVOD_SPISOK_zapis_data(3,tabel.ravno());

         }
        goto naz;

      case 3 :
        vibrek("Kateg",&kateg,&naim);
        VV.VVOD_SPISOK_zapis_data(4,kateg.ravno());
        goto naz;

      case 4 :
        vibrek("Zargrupp",&grupa,&naim);
        VV.VVOD_SPISOK_zapis_data(7,grupa.ravno());
        goto naz;

     }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 { 
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 } 
procentvp=procvip.ravno_atof();

uhsl=0; /*Учет сальдо включен*/
if(iceb_t_poldan("Учет сальдо в распечатке",bros,"zarnast.alx") == 0)
 if(SRAV(bros,"Вкл",1) != 0)
   uhsl=1;  /*Учет сальдо выключен*/

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("С учетом сальдо ?"));

if(uhsl == 1)
 {
  if(danet(&DANET,2,stdscr) == 1)
   uhsl=0; /*Учет сальдо включен*/
 }
else
 if(danet(&DANET,2,stdscr) == 2)
  uhsl=1;/*Учет сальдо выключен*/



clear(); /*Очистить экран и закрасить фоновым цветом*/

time(&tmm);
bf=localtime(&tmm);
  
printw("%s\n",gettext("Сортируем записи"));
refresh();
sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",
gr,mr);
class SQLCURSOR cur;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr2 == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
class iceb_t_tmptab tabtmp;
const char *imatmptab={"rasvedw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
podr int not null,\
tabn int not null,\
fio char(112) not null,\
inn char(24) not null,\
dokum char(112) not null,\
vidan char(255) not null,\
dolg char(112) not null,\
kateg int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
class iceb_tu_str dokument("");
class iceb_tu_str vidan("");
class iceb_tu_str dolgn("");
kolstr3=kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr2,++kolstr1);

  if(proverka(tabel.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(podrz.ravno(),row[1],0,0) != 0)
    continue;

  if(proverka(kateg.ravno(),row[2],0,0) != 0)
    continue;

  if(grupa.ravno()[0] != '\0')
   {
    //Узнаем в какой группе подразделение
    sprintf(strsql,"select grup from Podr where kod=%s",row[1]);
    if(readkey(strsql,&row1,&curr) == 1)
     if(proverka(grupa.ravno(),row1[0],1,0) != 0)
       continue;
   }

  tabb=atol(row[0]);
  sprintf(strsql,"select fio,inn,nomp,vidan,dolg,kateg from Kartb where tabn=%d",tabb);
  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabb);
    OSTANOV();
    continue;
   }
  kolstr3++;

  dokument.new_plus(gettext("Паспорт"));
  dokument.plus(" ",row1[2]);

  fio.new_plus(row1[0]);
  vidan.new_plus(row1[3]);
  dolgn.new_plus(row1[4]);

  sprintf(strsql,"insert into %s values (%s,%d,'%s','%s','%s','%s','%s',%s)",
  imatmptab,
  row[1],tabb,fio.ravno_filtr(),row1[1],dokument.ravno_filtr(),vidan.ravno_filtr(),dolgn.ravno_filtr(),row1[5]);

  sql_zapis(strsql,1,0);    


 }

GDITE();

if(ps == 0)
  sprintf(strsql,"select * from %s order by podr asc,tabn asc",imatmptab);

if(ps == 1)
  sprintf(strsql,"select * from %s order by podr asc,fio asc",imatmptab);
SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf,"kor%d.lst",getpid());
sprintf(imaf1,"vedom%d.lst",getpid());
sprintf(imaf2,"kasord%d.lst",getpid());
sprintf(imaftmp,"tt%d.tmp",getpid());
sprintf(imafkor1t,"kor1%d.tmp",getpid());
sprintf(imafkor2t,"kor2%d.tmp",getpid());
sprintf(imafkor2s,"kor2s%d.tmp",getpid());
sprintf(imafpodr,"zpodr%d.lst",getpid());
sprintf(imaf_nuv,"kornuv%d.lst",getpid());

class masiv_din_double SUMPODRHOZ;
class masiv_din_double SUMPODRBUD;

class masiv_din_int KOLLISTHOZ;
class masiv_din_int KOLLISTBUD;

if((ff_nuv = fopen(imaf_nuv,"w")) == NULL)
 {
  error_op_nfil(imaf_nuv,errno,"");
  return(1);
 }

fprintf(ff_nuv,"\x1B\x33%c",30); //Изменение межстрочного расстояния

if((ffpodr = fopen(imafpodr,"w")) == NULL)
 {
  error_op_nfil(imafpodr,errno,"");
  return(1);
 }


sapreestp(mr,gr,0,ffpodr);

memset(imafpodrb,'\0',sizeof(imafpodrb));
if(shetbudget.ravno()[0] != '\0')
 {
  sprintf(imafpodrb,"zpodrb%d.lst",getpid());

  if((ffpodrb = fopen(imafpodrb,"w")) == NULL)
   {
    error_op_nfil(imafpodrb,errno,"");
    return(1);
   }

  sapreestp(mr,gr,0,ffpodrb);

 }

if((kaw = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return(1);
 }
fprintf(kaw,"\x1B\x33%c",30); //Изменение межстрочного расстояния

if((ffkor2s = fopen(imafkor2s,"w")) == NULL)
 {
  error_op_nfil(imafkor2s,errno,"");
  return(1);
 }
fprintf(ffkor2s,"\x1B\x33%c",30); //Изменение межстрочного расстояния

if((kawk1 = fopen(imafkor1t,"w")) == NULL)
 {
  error_op_nfil(imafkor1t,errno,"");
  return(1);
 }
fprintf(kawk1,"\x1B\x33%c",30); //Изменение межстрочного расстояния

if((kawk2 = fopen(imafkor2t,"w")) == NULL)
 {
  error_op_nfil(imafkor2t,errno,"");
  return(1);
 }
fprintf(kawk2,"\x1B\x33%c",30); //Изменение межстрочного расстояния

if((ffvedom = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

memset(imaf3,'\0',sizeof(imaf3));
if(shetbudget.ravno()[0] != '\0')
 {
  sprintf(imaf3,"vvsh%d.lst",getpid());
  if((ff2 = fopen(imaf3,"w")) == NULL)
   {
    error_op_nfil(imaf3,errno,"");
    return(1);
   }
 }

if((ff = fopen(imaf2,"w")) == NULL) 
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

srasved(mr,gr,kaw,bf);
srasved(mr,gr,ff_nuv,bf);
srasved(mr,gr,ffkor2s,bf);
srasved(mr,gr,kawk1,bf);
srasved(mr,gr,kawk2,bf);


podr=0;
sal=itogo=itogo1=itogoo=itogoopb=0.;
kolstr1=0;
int nomer_vedom=0;
int nomer_vedom_b=0;
class koreshok kor_nuv;
int kategor=0;

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstr3,++kolstr1);
  in1=inah=nahshet=iu=ius=iub=iusb=0.;


  kpd=atoi(rowtmp[0]);
  tabb=atoi(rowtmp[1]);
  fio.new_plus(rowtmp[2]);
    
  inn.new_plus(rowtmp[3]);
  dokument.new_plus(rowtmp[4]);
  vidan.new_plus(rowtmp[5]);
  dolgn.new_plus(rowtmp[6]);
  kategor=atoi(rowtmp[7]);
  
  kor_nuv.clear();

  sal=0.;
  /*Читаем сальдо*/

  kor_nuv.saldo=sal=zarsald(1,mr,gr,tabb,&saldb);

  itsal+=sal;

  
  if(sal < -0.009 && uhsl == 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Внимание ! Отрицательное сальдо !"));
    sprintf(strsql,"%s:%d %s",gettext("Табельный номер"),tabb,fio.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }            



  if(podr != kpd)
   {
    if(podr != 0)
     {
      if(metkavs == 0)
       {
        itog(ffvedom,itogo);
        fprintf(ffvedom,"\f");
       }

      if(shetbudget.ravno()[0] != '\0')
       if(metkavsb == 0)
        {
         itog(ff2,itogo1);
         fprintf(ff2,"\f");
         fprintf(ffpodrb,"%3d %-*.*s %10.2f\n",podr,iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),itogo1);
         itogoopb+=itogo1; 
         if(itogo1 != 0.)
          {
           SUMPODRBUD.plus(itogo1,-1);
           KOLLISTBUD.plus(listn1,-1);
          }
        }

      fprintf(ffpodr,"%3d %-*.*s %10.2f\n",podr,iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),itogo);
      if(itogo != 0.)
       {
        SUMPODRHOZ.plus(itogo,-1);
        KOLLISTHOZ.plus(listn,-1);
       }      

      itogoo+=itogo; 

      nomer_vedom=nomer_vedom_b=0;
      itogo1=itogo=0.;
     }
    sprintf(strsql,"select naik from Podr where kod=%d",kpd);
    if(readkey(strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Не найден код подразделения"),kpd);
      OSTANOV();
      continue;
     }

    naipod.new_plus(row1[0]);
    
    printw("\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    fprintf(kaw,"\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    fprintf(ff_nuv,"\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    fprintf(ffkor2s,"\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    fprintf(kawk1,"\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    fprintf(kawk2,"\n%s%d %s\n",gettext("Подразделение N"),kpd,naipod.ravno());
    listn=listn1=1;
    kolst=kolst1=0;

    metkavs=metkavsb=1;
    podr=kpd;
   }

  sprintf(strsql,"select * from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and suma <> 0. order by prn,knah asc",
  gr,mr,gr,mr,tabb);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr == 0)
   {
    continue;
   }
  im=0;
  while(cur.read_cursor(&row) != 0)
   {

/*
    printw("%s %s %s %s %s %s %s %s\n",
    row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/
    
    if(im == 0)
      peh1(naipod.ravno(),fio.ravno(),tabb,inn.ravno(),mr,gr,dolgn.ravno(),kategor,kaw,kawk1,kawk2,ffkor2s,ff_nuv);

    prn=atoi(row[2]);

    if(kodnah != prn)
     {
      if(prn == 1)
       {
        sprintf(str,"\
         %s",gettext("Н а ч и с л е н и я"));
	strfil(str,str,1,kaw);
	strfil(str,str,1,ffkor2s);
        fprintf(kawk1,"\n%s\n",str);
        fprintf(kawk2,"\n%s\n",str);
       }

      if(prn == 2)
       {
        sprintf(str,"\
         %s",gettext("У д е р ж а н и я"));
	strfil(str,str,1,kaw);
	strfil(str,str,1,ffkor2s);
        fprintf(kawk1,"\n%s\n",str);
        fprintf(kawk2,"\n%s\n",str);
       }
      kodnah=prn;
     }

    if(prn == 1)
     {
      if(proverka(kodn.ravno(),row[3],0,1) == 0)
        continue;
      sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
     }

    if(prn == 2)
      sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
       
    if(readkey(strsql,&row1,&curr) != 1)
     {
      beep();
      if(prn == 1)
       printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
      if(prn == 2)
       printw("%s %s !\n",gettext("Не найден код удержания"),row[3]);
      OSTANOV();
      naim.new_plus("");
     }
    else
      naim.new_plus(row1[0]);

    knah=atoi(row[3]);
    sym=atof(row[4]);
    gz=atoi(row[6]);
    mz=atoi(row[7]);

    memset(st,'\0',sizeof(st));
    prom=sym;
    mkvk=0;
    if(prn == 2)
     {
      prom*=-1;

      if(provkod(obud,knah) < 0)
        if(gz != gr || mz != mr)
            mkvk=1;
      
     }

    kd=atoi(row[8]);

    prnb(prom,st);
    bros[0]='\0';

    if(kd != 0)
     sprintf(bros,"%d",kd);     

    sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %11s",
    knah,
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    bros,mz,gz,row[13],st);

    if(prn == 1)
     {
      kor_nuv.nahis.plus(str);
      kor_nuv.i_nahis+=sym;
     }
 
    if(prn == 2)
     {
      if(provkod(obud,knah) >= 0)
       {
        kor_nuv.uder.plus(str);
        kor_nuv.i_uder+=prom;
       }
      else
       {
        kor_nuv.viplati.plus(str);
        kor_nuv.i_viplati+=prom;
       }
     }     


    if(prn == 2 && mkvk == 1)
     {
      
      strfil(str," ",0,kaw);
      strfil(str,str,0,ffkor2s);

      rsdat(&d,&m,&g,row[0],2);
      sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %12s %02d.%02d.%d %-5s %s",
      knah,
      iceb_tu_kolbait(35,naim.ravno()),iceb_tu_kolbait(35,naim.ravno()),naim.ravno(),
      bros,mz,gz,row[13],st,d,m,g,row[5],row[12]);

      fprintf(kawk1,"%s\n",str);
      fprintf(kawk2,"\n");
     }
    else
     {
      strfil(str,str,0,kaw);
      strfil(str,str,0,ffkor2s);

      rsdat(&d,&m,&g,row[0],2);
      sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %12s %02d.%02d.%d %-5s %s",
      knah,
      iceb_tu_kolbait(35,naim.ravno()),iceb_tu_kolbait(35,naim.ravno()),naim.ravno(),
      bros,mz,gz,row[13],st,d,m,g,row[5],row[12]);

      fprintf(kawk1,"%s\n",str);
      fprintf(kawk2,"%s\n",str);
     }

    if(prn == 1)
     {

      in1+=sym;
      inah+=sym;
      oin+=sym;

      if(proverka(shetbudget.ravno(),row[5],0,1) == 0)
         nahshet+=sym;
     }

    if(prn == 2)
     {
      iu+=sym;
      if(proverka(shetbu,row[5],0,1) == 0)
         iub+=sym;
      
      if(gz == gr && mz == mr)
       {
	ius+=sym;

        if(proverka(shetbu,row[5],0,1) == 0)
          iusb+=sym;
       }
      else
       {
        if(provkod(obud,knah) >= 0)
         {
          ius+=sym;
          if(proverka(shetbu,row[5],0,1) == 0)
            iusb+=sym;
         }
       }
      oiu+=sym;


     }

    im++;
   }

  rasved_kornuv(&kor_nuv,ff_nuv);
  
  if(im > 0 || sal != 0.)
   {
    if(im == 0)
      peh1(naipod.ravno(),fio.ravno(),tabb,inn.ravno(),mr,gr,dolgn.ravno(),kategor,kaw,kawk1,kawk2,ffkor2s,ff_nuv);

    bbr=inah+iu+sal;
    itogiz(inah,iu,ius,sal,nahshet,iub,iusb,saldb,kaw,kawk1,kawk2,ffkor2s);
    rasved_iknuv(&kor_nuv,ff_nuv);

    if(uhsl == 0)
      bbr=in1+iu+sal;

    if(uhsl == 1)
     {
      ius1=prbper(mr,gr,mmm,ggg,tabb,&iusb1,0);
      bbr=in1+ius+ius1;
     }
/*
    printw("2dolg=%.2f bbr=%.2f\n",dolg,bbr);
    getch();
*/

    if(bbr > 0.009)
     {
      char osnovan[512];
      sprintf(osnovan,"%s %d.%d%s",
      gettext("Заработная плата"),mr,gr,
      gettext("г."));
      sprintf(bros,"%s%d",ICEB_ZAR_PKTN,tabb);
      kasord2("","",0,bf->tm_mon+1,bf->tm_year+1900,shrpz.ravno(),bros,bbr,fio.ravno(),osnovan,"","","",ff);
      if(metkakas == 0)
       {
        metkakas++;
        fprintf(ff,"\n\n\n");
       }
      else
       {
        metkakas=0;
        fprintf(ff,"\f");
       }
      
      bb=bbr;

      if(shetbu == NULL)
       {
        if(nahshet != 0.)
         {
          bb=(inah-nahshet)*bbr/inah;
          bb=okrug(bb,okrg);        
         }
        if(bb > 0.009)
         {
          if(metkavs == 1)
           {
            if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ffvedom) != 0)
              return(1);
            sspp1(ffvedom,listn,&bros_int);
            metkavs=0;
           }         

          if(procentvp != 100.)
           {
            viplata=bb*procentvp/100.;                    
            viplata=okrug(viplata,okrg);
           }
          else
            viplata=bb;

          itogo+=viplata;
          kolst++;
          
          zarstved(&nomer_vedom,tabb,fio.ravno(),viplata,ffvedom);
         }

        if(shetbudget.ravno()[0] != '\0' && nahshet != 0.)
         {
          bb1=bbr-bb;
          if(procentvp != 100.)
           {
            bb1=bb1*procentvp/100.;                    
            bb1=okrug(bb1,okrg);
           }
          if(bb1 > 0.009)
           {
            if(metkavsb == 1)
             {
              if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ff2) != 0)
                return(1);
              sspp1(ff2,listn1,&bros_int);
              metkavsb=0;
             }
            itogo1+=bb1;
            kolst1++;
            zarstved(&nomer_vedom_b,tabb,fio.ravno(),bb1,ff2);
           }
         }
       }
     }

    if(shetbu != NULL)
     {
      if(uhsl == 0 ) /*С учетом сальдо*/
       {
        bb=bbr-(nahshet+iub+saldb);
       }               
      if(uhsl == 1 ) /*Без учета сальдо*/
       {
        bb=in1-nahshet+(ius-iusb)+(ius1-iusb1);

       }
       
      if(bb > 0.009)
       {
        if(metkavs == 1)
         {
            if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ffvedom) != 0)
              return(1);
          sspp1(ffvedom,listn,&bros_int);
          metkavs=0;
         }         
        if(procentvp != 100.)
         {
          viplata=bb*procentvp/100.;                    
          viplata=okrug(viplata,okrg);
         }
        else
          viplata=bb;
        itogo+=viplata;
        kolst++;
        
        zarstved(&nomer_vedom,tabb,fio.ravno(),viplata,ffvedom);
       }
      bb=bbr-bb;
      if(procentvp != 100.)
       {
        bb=bb*procentvp/100.;                    
        bb=okrug(bb,okrg);
       }
      if(bb > 0.009)
       {
        if(metkavsb == 1)
          {
           if(zarstfn53(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ff2) != 0)
                return(1);
           sspp1(ff2,listn1,&bros_int);
           metkavsb=0;
          }
        itogo1+=bb;
        kolst1++;

        zarstved(&nomer_vedom_b,tabb,fio.ravno(),bb,ff2);
       }

     }

    printw("%8.2f\n",bbr);
   }



  if(listn == 1 && kolst >= STR1)
   {
    listn++;
    kolst=1;
    sspp1(ffvedom,listn,&bros_int);
   }
  if(listn > 1 && kolst > STR2)
   {
    listn++;
    kolst=1;
    sspp1(ffvedom,listn,&bros_int);
   }

  if(shetbudget.ravno()[0] != '\0')
   {
    if(listn1 == 1 && kolst1 >= STR1)
     {
      listn1++;
      kolst1=1;
      sspp1(ff2,listn1,&bros_int);
     }
    if(listn1 > 1 && kolst1 > STR2)
     {
      listn1++;
      kolst1=1;
      sspp1(ff2,listn1,&bros_int);
     }
   }
 }


GDITE();

if(metkavs == 0)
  itog(ffvedom,itogo);
fprintf(ffpodr,"%3d %-*.*s %10.2f\n",podr,iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),itogo);
SUMPODRHOZ.plus(itogo,-1);
KOLLISTHOZ.plus(listn,-1);

itogoo+=itogo;
itogoopb+=itogo1;

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,"%*s %10.2f\n",iceb_tu_kolbait(44,gettext("Итого")),gettext("Итого"),itogoo);

if(shetbudget.ravno()[0] != '\0')
 {
  fprintf(ffpodrb,"%3d %-*.*s %10.2f\n",podr,iceb_tu_kolbait(40,naipod.ravno()),iceb_tu_kolbait(40,naipod.ravno()),naipod.ravno(),itogo1);
  SUMPODRBUD.plus(itogo1,-1);
  KOLLISTBUD.plus(listn1,-1);
  fprintf(ffpodrb,"\
--------------------------------------------------------\n");
  fprintf(ffpodrb,"%*s %10.2f\n",iceb_tu_kolbait(44,gettext("Итого")),gettext("Итого"),itogoopb);
  if(metkavsb == 0)
   itog(ff2,itogo1);

  podpis(ffpodrb);
  fclose(ffpodrb);

  fclose(ff2);

  sumprved(&SUMPODRBUD,&KOLLISTBUD,imaf3);
 }
printw("\n%s %15s\n",
gettext("Сальдо          -"),prnbr(itsal));

prnb(oin,st);

printw("\
%s %15s\n\
%s %15s\n",
gettext("Итого начислено -"),st,
gettext("Итого удержано  -"),prnbr(oiu));

printw("%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

fprintf(kaw,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

fprintf(kawk1,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

fprintf(kawk2,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

printw("%s: %.2f\n",
gettext("Общий итог с учётом сальдо"),itsal+oin+oiu);


//Установка левого поля в ноль делается ниже

fclose(kaw);
fclose(ffkor2s);
fclose(ff_nuv);
fclose(kawk1);
fclose(kawk2);
fclose(ffvedom);

sumprved(&SUMPODRHOZ,&KOLLISTHOZ,imaf1);

podpis(ffpodr);

fclose(ffpodr);

fclose(ff);


razbiv(imaf,imafkor1,imafkor2,imafkor2s);
/*разбивка корешков с начислениями удержаниями и выплатами на листы*/
rasved_rbknuv(imaf_nuv);



oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Корешки (1-с сальдо 2-без сальдо)"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Ведомости на получение заработной платы"));

oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Кассовые ордера на получение заработной платы"));

oth->spis_imaf.plus(imafkor1);
oth->spis_naim.plus(gettext("Корешки с сальдо развёрнутые"));

oth->spis_imaf.plus(imafkor2);
oth->spis_naim.plus(gettext("Корешки без сальдо развёрнутые"));

oth->spis_imaf.plus(imafpodr);
oth->spis_naim.plus(gettext("Свод по подразделениям"));

oth->spis_imaf.plus(imafkor2s);
oth->spis_naim.plus(gettext("Корешки (1-с сальдо 2-с сальдо)"));

oth->spis_imaf.plus(imaf_nuv);
oth->spis_naim.plus(gettext("Корешки c разбивкой на начисления, удержания выплаты"));

if(shetbudget.ravno()[0] != '\0')
 {
  oth->spis_imaf.plus(imafpodrb);
  oth->spis_naim.plus(gettext("Свод по бюджетным подразделениям"));

  oth->spis_imaf.plus(imaf3);
  oth->spis_naim.plus(gettext("Ведомости на получение бюджетной зарплаты"));

 }
for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);

printw_vr(tmm);
OSTANOV();
 
return(0);
}




/******************************/
/*Разбивка распечатки на листы*/
/******************************/
void razbiv(char *imaf,char *imafkor1,char *imafkor2,char *imafkor2s)
{
FILE            *ff,*ff1,*ff2,*ffk1,*ffk2,*ffk1t,*ffk2t;
FILE		*ffkor2s,*ffkor2ss;
short           dlkr;
short           mok;
short           i;
char		str[1024];
char		bros[1024];
char		imaf1[64],imaf2[64];
char		imafkor1t[64],imafkor2t[64];
char		imafkor2ss[64];

if(iceb_t_poldan("Распечатка корешков на рулоне",bros,"zarnast.alx") == 0)
 if(SRAV(bros,"Вкл",1) == 0)
  {
   printw("%s.\n",gettext("Распечатка корешков на рулоне"));
   refresh();
   sprintf(imaf1,"tt%d.tmp",getpid());
   rename(imaf1,imaf);
   if((ff = fopen(imaf,"a")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      return;
     }
   fclose(ff);

   sprintf(imafkor1t,"kor1%d.tmp",getpid());
   sprintf(imafkor1,"kor1_%d.lst",getpid());
   rename(imafkor1t,imafkor1);
   if((ff = fopen(imafkor1,"a")) == NULL)
     {
      error_op_nfil(imafkor1,errno,"");
      return;
     }
   fclose(ff);

   sprintf(imafkor2ss,"kor2s%d.tmp",getpid());
   sprintf(imafkor2s,"kor2s%d.lst",getpid());
   rename(imafkor2ss,imafkor2s);
   if((ff = fopen(imafkor2s,"a")) == NULL)
     {
      error_op_nfil(imafkor2s,errno,"");
      return;
     }
   fclose(ff);


   sprintf(imafkor2t,"kor2%d.tmp",getpid());
   sprintf(imafkor2,"kor2_%d.lst",getpid());
   rename(imafkor2t,imafkor2);

   if((ff = fopen(imafkor2,"a")) == NULL)
     {
      error_op_nfil(imafkor2,errno,"");
      return;
     }
   fclose(ff);

   return;   
  }



sprintf(imaf1,"tt%d.tmp",getpid());
if((ff= fopen(imaf1,"r")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

sprintf(imaf2,"t%d.tmp",getpid());
if((ff1= fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return;
 }

mok=dlkr=0;
while(fgets(str,sizeof(str),ff) != 0)
 {
  dlkr++;
  if(SRAV(str,"------",1) == 0)
   mok++;
  if(mok == 2)
   {
    fprintf(ff1,"%d\n",dlkr);
    mok=dlkr=0;
   }

 }

fclose(ff1);
rewind(ff);

if((ff1= fopen(imaf2,"r")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return;
 }

sprintf(imafkor1t,"kor1%d.tmp",getpid());
if((ffk1t= fopen(imafkor1t,"r")) == NULL)
 {
  error_op_nfil(imafkor1t,errno,"");
  return;
 }

sprintf(imafkor2t,"kor2%d.tmp",getpid());
if((ffk2t= fopen(imafkor2t,"r")) == NULL)
 {
  error_op_nfil(imafkor2t,errno,"");
  return;
 }

sprintf(imafkor2ss,"kor2s%d.tmp",getpid());
if((ffkor2ss= fopen(imafkor2ss,"r")) == NULL)
 {
  error_op_nfil(imafkor2ss,errno,"");
  return;
 }



if((ff2= fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
sprintf(imafkor1,"kor1_%d.lst",getpid());
if((ffk1= fopen(imafkor1,"w")) == NULL)
 {
  error_op_nfil(imafkor1,errno,"");
  return;
 }

sprintf(imafkor2,"kor2_%d.lst",getpid());
if((ffk2= fopen(imafkor2,"w")) == NULL)
 {
  error_op_nfil(imafkor2,errno,"");
  return;
 }

sprintf(imafkor2s,"kor2s%d.lst",getpid());
if((ffkor2s= fopen(imafkor2s,"w")) == NULL)
 {
  error_op_nfil(imafkor2s,errno,"");
  return;
 }

mok=dlkr=0;
while(fgets(str,sizeof(str),ff1) != NULL)
 {
  dlkr=atoi(str);
  mok+=dlkr;
  if(mok > STRKOR)
   {
    fprintf(ff2,"\f");
    fprintf(ffk1,"\f");
    fprintf(ffk2,"\f");
    fprintf(ffkor2s,"\f");
    mok=dlkr;
   }

  for(i=0; i < dlkr; i++)
   {
    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ff) != NULL)
     fprintf(ff2,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffk1t) != NULL)
     fprintf(ffk1,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffk2t) != NULL)
     fprintf(ffk2,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffkor2ss) != NULL)
     fprintf(ffkor2s,"%s",str);

   }

 }

while(fgets(str,sizeof(str),ff) != NULL)
  fprintf(ff2,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffk1t) != NULL)
 fprintf(ffk1,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffk2t) != NULL)
  fprintf(ffk2,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffkor2ss) != NULL)
 fprintf(ffkor2s,"%s",str);



fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ffk1);
fclose(ffk2);
fclose(ffk1t);
fclose(ffk2t);
fclose(ffkor2ss);
fclose(ffkor2s);

unlink(imaf1);
unlink(imaf2);
unlink(imafkor1t);
unlink(imafkor2t);
unlink(imafkor2ss);

}

/*************************************/
/*Шапка для рееста по подразделениям*/
/*************************************/
void	sapreestp(short mr,short gr,
int metka, //0-не бюджет 1-бюджет
FILE *ffpodr)
{
fprintf(ffpodr,"%s\n\n",iceb_t_get_pnk("00",0));
if(metka == 1)
  fprintf(ffpodr,"%s",gettext("Бюджет"));

fprintf(ffpodr,"%s %02d.%d%s\n",
gettext("Реестр платежных ведомостей за"),mr,gr,
gettext("г."));

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,gettext("\
Код|     Наименование подразделения         |  Сумма   |\n"));
fprintf(ffpodr,"\
--------------------------------------------------------\n");
}
/***************************/
/*Шапка распечаток корешков*/
/***************************/
void srasved(short mr,short gr,FILE *kaw,
struct tm *bf)
{
fprintf(kaw,"\
%s\n\
%s %d.%d%s\n\
%s %d.%d.%d%s  %s: %d.%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Ведомость на зарплату за"),
mr,gr,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
/***************************************/
/*Распечатка начислений, удержаний и выплат в корешок*/
/***************************************************/

void rasved_kornuv(class koreshok *kor,FILE *ff)
{
char str[512];
int kol=kor->nahis.kolih();
if(kol > 0)
 {
  sprintf(str,"\
         %s",gettext("Н а ч и с л е н и я"));
  strfil(str,str,1,ff);
  for(int ii=0; ii < kol;ii++)
   {
    strncpy(str,kor->nahis.ravno(ii),sizeof(str)-1);
    strfil(str,str,0,ff);

   }

 }
kol=kor->uder.kolih();
if(kol > 0)
 {
  sprintf(str,"\
         %s",gettext("У д е р ж а н и я"));
  strfil(str,str,1,ff);
  for(int ii=0; ii < kol;ii++)
   {
    strncpy(str,kor->uder.ravno(ii),sizeof(str)-1);
    strfil(str,str,0,ff);

   }

 }

kol=kor->viplati.kolih();
if(kol > 0)
 {
  sprintf(str,"\
         %s",gettext("В ы п л а т ы"));
  strfil(str,str,1,ff);
  for(int ii=0; ii < kol;ii++)
   {
    strncpy(str,kor->viplati.ravno(ii),sizeof(str)-1);
    strfil(str,str,0,ff);

   }

 }

}
/******************************/
/*итог по корешку с начислениями удержаниями выплатами*/
/**************************************************/
void rasved_iknuv(class koreshok *kor,FILE *ff)
{
char str[512];

sprintf(str,"%s %14s",gettext("Сальдо на начало месяца -"),prnbr(kor->saldo));
strfil(str,str,1,ff);

sprintf(str,"%s %14s",gettext("Итого начислено         -"),prnbr(kor->i_nahis));

strfil(str,str,0,ff);

sprintf(str,"%s %14s",gettext("Итого удержано          -"),prnbr(kor->i_uder));

strfil(str,str,0,ff);

sprintf(str,"%s %14s",gettext("Итого выплачено         -"),prnbr(kor->i_viplati));

strfil(str,str,0,ff);

double bbr=kor->saldo+kor->i_nahis-kor->i_uder-kor->i_viplati;

if(bbr < 0.)
 sprintf(str,"%s %14s",gettext("Сальдо конечное        -"),prnbr(bbr));
else
 sprintf(str,"%s %14s",gettext("К выдаче               -"),prnbr(bbr));

strfil(str,str,0,ff);

sprintf(str,"------------------------------------------------------------");
strfil(str,str,0,ff);

}

/*****************************************************************/
/*разбивка на листы корешков с начислениями удержаниями выплатами*/
/******************************************************************/

void rasved_rbknuv(char *imaf_nuv)
{
char str[1024];
class iceb_tu_str bros("");
char imaf_dat[64];
char imaf_tmp[64];
FILE *ff;
FILE *ff_dat;
FILE *ff_tmp;

if(iceb_t_poldan("Распечатка корешков на рулоне",&bros,"zarnast.alx") == 0)
 if(SRAV(bros.ravno(),"Вкл",1) == 0)
  {
   if((ff = fopen(imaf_nuv,"a")) == NULL)
     {
      error_op_nfil(imaf_nuv,errno,"");
      return;
     }
   fclose(ff);
   return;
  }


if((ff= fopen(imaf_nuv,"r")) == NULL)
 {
  error_op_nfil(imaf_nuv,errno,"");
  return;
 }

sprintf(imaf_dat,"kordat%d.tmp",getpid());

if((ff_dat= fopen(imaf_dat,"w")) == NULL)
 {
  error_op_nfil(imaf_dat,errno,"");
  return;
 }

int dlkr=0,mok=0;
while(fgets(str,sizeof(str),ff) != 0)
 {
  dlkr++;
  if(SRAV(str,"------",1) == 0)
   mok++;
  if(mok == 2)
   {
    fprintf(ff_dat,"%d\n",dlkr);
    mok=dlkr=0;
   }

 }

fclose(ff_dat);
rewind(ff);


sprintf(imaf_tmp,"kortmp%d.tmp",getpid());

if((ff_tmp= fopen(imaf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_tmp,errno,"");
  return;
 }

if((ff_dat= fopen(imaf_dat,"r")) == NULL)
 {
  error_op_nfil(imaf_dat,errno,"");
  return;
 }


while(fgets(str,sizeof(str),ff_dat) != NULL)
 {
  dlkr=atoi(str);
  mok+=dlkr;
  if(mok > STRKOR)
   {
    fprintf(ff_tmp,"\f");
    mok=dlkr;
   }

  for(int i=0; i < dlkr; i++)
   {
    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ff) != NULL)
     fprintf(ff_tmp,"%s",str);


   }

 }

while(fgets(str,sizeof(str),ff) != NULL)
  fprintf(ff_tmp,"%s",str);


fclose(ff_dat);
fclose(ff_tmp);
fclose(ff);

rename(imaf_tmp,imaf_nuv);
unlink(imaf_dat);
}
