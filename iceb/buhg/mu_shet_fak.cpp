/*$Id: mu_shet_fak.c,v 5.12 2013/05/17 14:56:05 sasa Exp $*/
/*15.07.2015	21.12.2005	Белых А.И.	mu_shet_fak.c
Распечатка счёта-фактуры для материального учёта
*/
#include        <errno.h>
#include	"buhg.h"


extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short	vtara; /*Код группы возвратная тара*/

void mu_shet_fak(short dd,short md,short gd, //Дата документа
int sklad, //склад
const char *nomdok, //Номер документа
char *imafil, //имя файла с распечаткой
int tipz)  //1-приход 2-расход
{
class iceb_tu_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_tu_str pkod(""),pkod1(""); //Код ЕДРПУО
class iceb_tu_str mfo(""),mfo1("");  /*МФО*/
class iceb_tu_str nsh(""),nsh1("");  /*Номер счета*/
char		strsql[1024];
SQL_str row;
SQLCURSOR cur;
int		kolstr;
class iceb_tu_str kodop("");
class iceb_tu_str kontr("");
class iceb_tu_str kodkontr00("");
class iceb_tu_str bros("");
short lnds=0; //Льготы по НДС
class iceb_tu_str dover("");
class iceb_tu_str sherez("");
short		mvnpp=0;
float pnds=0.;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tip=%d",
gd,md,dd,sklad,nomdok,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));

  sprintf(strsql,gettext("Дата=%d.%d.%d Документ=%s Склад=%d"),
  dd,md,gd,nomdok,sklad);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

pnds=atof(row[13]);
kontr.new_plus(row[3]);
kodop.new_plus(row[6]);



sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=11",
gd,sklad,nomdok);

if(readkey(strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);


sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=1",
gd,sklad,nomdok);

if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],&dover,1,'#');
 }

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=2",
gd,sklad,nomdok);

if(readkey(strsql,&row,&cur) == 1)
 sherez.new_plus(row[0]);

if(tipz == 1)
 sprintf(strsql,"select vido from Prihod where kod='%s'",kodop.ravno());

if(tipz == 2)
 sprintf(strsql,"select vido from Rashod where kod='%s'",kodop.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код операции"),kodop.ravno());
  iceb_t_soob(strsql);

 }
else
 {
  mvnpp=atoi(row[0]);
 }

kodkontr00.new_plus("00");
if(tipz == 2)
 {
  if(iceb_t_poldan(kodop.ravno(),&bros,"matnast.alx") == 0)
   {
    kodkontr00.new_plus(bros.ravno());
   }
 }
/*Читаем реквизиты организации свои */


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  nmo.new_plus(row[1]);
  pkod.new_plus(row[5]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
 } 



/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(kontr.ravno_atof() != 0. || kontr.ravno()[0] != '0')
 {

  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
    iceb_t_soob(strsql);
   }
  else
   {
    if(row[16][0] == '\0')
      nmo1.new_plus(row[1]);
    else
      nmo1.new_plus(row[16]);
    pkod1.new_plus(row[5]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    
   }
 }
sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,sklad,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
sprintf(imafil,"mshfk%d.lst",getpid());
FILE *ff;

if((ff = fopen(imafil,"w")) == NULL)
 {
  error_op_nfil(imafil,errno,"");
  return;
 }
startfil(ff);


fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

if(tipz == 1)
if(shet_fak_sap(dd,md,gd,nomdok,nsh1.ravno(),mfo1.ravno(),pkod1.ravno(),nmo.ravno(),dover.ravno(),sherez.ravno(),ff) != 0)
 {
  fclose(ff);
  unlink(imafil);
  return;
 }

if(tipz == 2)
if(shet_fak_sap(dd,md,gd,nomdok,nsh.ravno(),mfo.ravno(),pkod.ravno(),nmo1.ravno(),dover.ravno(),sherez.ravno(),ff) != 0)
 {
  fclose(ff);
  unlink(imafil);
  return;
 }


SQLCURSOR cur1;
SQL_str row1;
double kolih;
double cena;
double cenasnd;
double itogo=0.;
double suma;
class iceb_tu_str naim("");
int nom_str=0;
int mnt=0;
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование материалла*/
  naim.new_plus("");
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[4]);
    iceb_t_soob(strsql);
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }

  int kgrm=atoi(row1[1]);
  int vtr=atoi(row[8]);

  if(vtr != 0. || (vtara != 0 && kgrm == vtara))
   {
    mnt++;
    continue;
   }

  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  itogo+=suma;
  cenasnd=cena+cena*pnds/100.;
  cenasnd=okrug(cenasnd,okrg1);

  fprintf(ff," %3d %-*.*s %6.6g %-*.*s %8.8g %10.2f\n",
  ++nom_str,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  kolih,
  iceb_tu_kolbait(6,row[4]),iceb_tu_kolbait(6,row[4]),row[4],
  cena,suma);

  printw(" %3d %-*.*s %6.6g %-*.*s %8.8g %10.2f\n",
  nom_str,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  kolih,
  iceb_tu_kolbait(6,row[4]),iceb_tu_kolbait(6,row[4]),row[4],
  cena,suma);

  if(iceb_tu_strlen(naim.ravno()) > 30)
     fprintf(ff,"%3s %s\n"," ",iceb_tu_adrsimv(30,naim.ravno()));


 }

fprintf(ff,"\
---------------------------------------------------------------------\n");
int dlina=58;
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(dlina,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
  dok_end_mu(tipz,dd,md,gd,nomdok,sklad,dlina-1,itogo,pnds,lnds,mnt,3,ff);

shet_fak_end(ff); //распечатка концовки документа

fclose(ff);
 
}
