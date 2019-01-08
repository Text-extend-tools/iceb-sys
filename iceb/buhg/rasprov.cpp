/* $Id: rasprov.c,v 5.43 2013/09/26 09:43:39 sasa Exp $ */
/*10.03.2016	27.04.1999	Белых А.И.	rasprov.c
Распечатка проводок в файл
*/
#include        <errno.h>
#include        "buhg.h"

void raspitog(double,double,int,FILE*);


int rasprov(SQLCURSOR *cur,
int kolstr,
class poiprov_rek *poi)
{
SQL_str         row;
char		imaf[64],imafden[64],imafsvod[64];
FILE		*ff1,*ffden,*ffsvod;
double		dbt,krt;
double		dbtden=0.,krtden=0.;
double		dbtmes=0.,krtmes=0.;
class iceb_tu_str naikon("");
char		strsql[512];
class iceb_tu_str shet("");
time_t tmm;
struct tm *bf;
class iceb_tu_str shetk("");
long		kol;
short		d,m,g;
short		denz,mesz;
double		debet,kredit;
int		nomer=0;
class iceb_tu_str koment("");
short dn,mn,gn;
short dk,mk,gk;
rsdat(&dn,&mn,&gn,poi->dat1.ravno(),1);
rsdat(&dk,&mk,&gk,poi->dat2.ravno(),1);


SQLCURSOR       cur1;
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Подготовка распечаток"));
soobsh(&SOOBN,-1,-1,1);

move(LINES-1,0);
GDITE();

sprintf(imaf,"prov%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imafden,"provd%d.lst",getpid());
if((ffden = fopen(imafden,"w")) == NULL)
 {
  error_op_nfil(imafden,errno,"");
  return(1);
 }

sprintf(imafsvod,"provs%d.lst",getpid());
if((ffsvod = fopen(imafsvod,"w")) == NULL)
 {
  error_op_nfil(imafsvod,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Распечатка проводок"),dn,mn,gn,dk,mk,gk,ff1);
iceb_t_zagolov(gettext("Распечатка проводок"),dn,mn,gn,dk,mk,gk,ffden);
iceb_t_zagolov(gettext("Свод по счетам"),dn,mn,gn,dk,mk,gk,ffsvod);


fprintf(ff1,"%s:\n",gettext("Реквизиты поиска"));
fprintf(ffden,"%s:\n",gettext("Реквизиты поиска"));
fprintf(ffsvod,"%s:\n",gettext("Реквизиты поиска"));
if(poi->sh.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Счёт %s\n"),poi->sh.ravno());
  fprintf(ffden,gettext("Счёт %s\n"),poi->sh.ravno());
  fprintf(ffsvod,gettext("Счёт %s\n"),poi->sh.ravno());
 }
if(poi->shk.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Счёт корреспондент %s\n"),poi->shk.ravno());
  fprintf(ffden,gettext("Счёт корреспондент %s\n"),poi->shk.ravno());
  fprintf(ffsvod,gettext("Счёт корреспондент %s\n"),poi->shk.ravno());
 }
if(poi->kor.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Код контрагента %s\n"),poi->kor.ravno());
  fprintf(ffden,gettext("Код контрагента %s\n"),poi->kor.ravno());
  fprintf(ffsvod,gettext("Код контрагента %s\n"),poi->kor.ravno());
 }
if(poi->raz.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Коментарий %s\n"),poi->raz.ravno());
  fprintf(ffden,gettext("Коментарий %s\n"),poi->raz.ravno());
  fprintf(ffsvod,gettext("Коментарий %s\n"),poi->raz.ravno());
 }
if(poi->deb.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Дебет %s\n"),poi->deb.ravno());
  fprintf(ffden,gettext("Дебет %s\n"),poi->deb.ravno());
  fprintf(ffsvod,gettext("Дебет %s\n"),poi->deb.ravno());
 }
if(poi->kre.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Кредит %s\n"),poi->kre.ravno());
  fprintf(ffden,gettext("Кредит %s\n"),poi->kre.ravno());
  fprintf(ffsvod,gettext("Кредит %s\n"),poi->kre.ravno());
 }
if(poi->kto.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Метка проводки %s\n"),poi->kto.ravno());
   fprintf(ffden,gettext("Метка проводки %s\n"),poi->kto.ravno());
  fprintf(ffsvod,gettext("Метка проводки %s\n"),poi->kto.ravno());
 }
if(poi->nn.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Номер документа %s\n"),poi->nn.ravno());
  fprintf(ffden,gettext("Номер документа %s\n"),poi->nn.ravno());
  fprintf(ffsvod,gettext("Номер документа %s\n"),poi->nn.ravno());
 }
if(poi->kop.ravno()[0] != '\0')
 {
  fprintf(ff1,gettext("Код операции %s\n"),poi->kop.ravno());
  fprintf(ffden,gettext("Код операции %s\n"),poi->kop.ravno());
  fprintf(ffsvod,gettext("Код операции %s\n"),poi->kop.ravno());
 }  
if(poi->grupa.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Группа"),poi->grupa.ravno());
  fprintf(ffden,"%s:%s\n",gettext("Группа"),poi->grupa.ravno());
  fprintf(ffsvod,"%s:%s\n",gettext("Группа"),poi->grupa.ravno());
 }  

if(poi->status.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Статус счетов"),poi->status.ravno());
  fprintf(ffden,"%s:%s\n",gettext("Статус счетов"),poi->status.ravno());
  fprintf(ffsvod,"%s:%s\n",gettext("Статус счетов"),poi->status.ravno());
 }  

if(poi->kekv_poi.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("КЭКЗ"),poi->kekv_poi.ravno());
  fprintf(ffden,"%s:%s\n",gettext("КЭКЗ"),poi->kekv_poi.ravno());
  fprintf(ffsvod,"%s:%s\n",gettext("КЭКЗ"),poi->kekv_poi.ravno());
 }  
if(poi->dat1z.ravno()[0] != '\0')
 {
  fprintf(ff1,"%s:%s => %s\n",gettext("Поиск по дате записи"),poi->dat1z.ravno(),poi->dat2z.ravno());
  fprintf(ffden,"%s:%s => %s\n",gettext("Поиск по дате записи"),poi->dat1z.ravno(),poi->dat2z.ravno());
  fprintf(ffsvod,"%s:%s => %s\n",gettext("Поиск по дате записи"),poi->dat1z.ravno(),poi->dat2z.ravno());
 }  

rapsap(ff1);

rapsap(ffden);

SPISOK SHET(0);
masiv_din_double DEB(0); //Массив содержимого кодов начислений
masiv_din_double KRE(0); //Массив содержимого кодов начислений

mesz=denz=0;
kol=0;
dbt=krt=0.;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kol);
  if(provprov(row,poi) != 0)
    continue;

  rsdat(&d,&m,&g,row[1],2);
  debet=atof(row[9]);
  kredit=atof(row[10]);

  sprintf(strsql,"%s|%s",row[2],row[3]);
  if((nomer=SHET.find(strsql)) != -1)
   {
    DEB.plus(debet,nomer);
    KRE.plus(kredit,nomer);
   }  
  else
   {
    SHET.plus(strsql);
    DEB.plus(debet,-1);
    KRE.plus(kredit,-1);
   }
  if(denz != d)
   {
    if(denz != 0)
      raspitog(dbtden,krtden,1,ffden);
    
    dbtden=krtden=0.;
    denz=d;
   }
  if(mesz != m)
   {
    if(mesz != 0)
      raspitog(dbtmes,krtmes,2,ffden);
    
    dbtmes=krtmes=0.;
    mesz=m;
   }
  naikon.new_plus("");

  /*получение наименования контрагента*/
  buhpnk(&naikon,row[4],iceb_t_mpods(row[5]),atoi(row[15]),row[14],row[6],atoi(row[7]));

  sozkom_prov(&koment,row[13],row[5],row[8]);

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %-*s%*s %-*s %10s",
  d,m,g,
  iceb_tu_kolbait(6,row[2]),row[2],
  iceb_tu_kolbait(6,row[3]),row[3],
  iceb_tu_kolbait(7,row[4]),row[4],
  iceb_tu_kolbait(3,row[5]),row[5],
  iceb_tu_kolbait(4,row[11]),row[11],
  iceb_tu_kolbait(10,row[6]),row[6],
  prnbr(debet));

  fprintf(ff1," %10s %*s %-*s %-*.*s %.*s\n",
  prnbr(kredit),
  iceb_tu_kolbait(4,row[16]),row[16],
  iceb_tu_kolbait(4,row[8]),row[8],
  iceb_tu_kolbait(32,koment.ravno()),
  iceb_tu_kolbait(32,koment.ravno()),
  koment.ravno(),
  iceb_tu_kolbait(40,naikon.ravno()),
  naikon.ravno());

  for(int nom=32; nom < iceb_tu_strlen(koment.ravno()); nom+=32)
    fprintf(ff1,"%82s %-*.*s\n",
    "",
    iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
    iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
    iceb_tu_adrsimv(nom,koment.ravno()));
    
  fprintf(ffden,"%02d.%02d.%4d %-*s %-*s %-*s %-*s%*s %-*s %10s",
  d,m,g,
  iceb_tu_kolbait(6,row[2]),row[2],
  iceb_tu_kolbait(6,row[3]),row[3],
  iceb_tu_kolbait(7,row[4]),row[4],
  iceb_tu_kolbait(3,row[5]),row[5],
  iceb_tu_kolbait(4,row[11]),row[11],
  iceb_tu_kolbait(10,row[6]),row[6],
  prnbr(debet));

  fprintf(ffden," %10s %*s %-*s %-*.*s %.*s\n",
  prnbr(kredit),
  iceb_tu_kolbait(4,row[16]),row[16],
  iceb_tu_kolbait(4,row[8]),row[8],
  iceb_tu_kolbait(32,koment.ravno()),
  iceb_tu_kolbait(32,koment.ravno()),
  koment.ravno(),
  iceb_tu_kolbait(40,naikon.ravno()),
  naikon.ravno());

  for(int nom=32; nom < iceb_tu_strlen(koment.ravno()); nom+=32)
    fprintf(ffden,"%82s %-*.*s\n",
    "",
    iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
    iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
    iceb_tu_adrsimv(nom,koment.ravno()));


  if(poi->dat1z.ravno()[0] != '\0')
   {
   
    tmm=atoi(row[12]);
    bf=localtime(&tmm);

    fprintf(ff1,"%s: %d.%d.%d%s %s: %d:%d:%d\n",
    gettext("Дата записи проводки"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min,bf->tm_sec);

    fprintf(ffden,"%s: %d.%d.%d%s %s: %d:%d:%d\n",
    gettext("Дата записи проводки"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min,bf->tm_sec);

   }

  dbt+=debet;
  krt+=kredit;
  dbtden+=debet;
  krtden+=kredit;
  dbtmes+=debet;
  krtmes+=kredit;
 }

raspitog(dbt,krt,0,ff1);
raspitog(dbtden,krtden,1,ffden);
raspitog(dbtmes,krtmes,2,ffden);
raspitog(dbt,krt,0,ffden);

podpis(ff1);
fclose(ff1);


podpis(ffden);
fclose(ffden);

/*******************************/
//Распечатка свода по счетам

fprintf(ffsvod,"\n");
fprintf(ffsvod,"\
----------------------------------------\n");
fprintf(ffsvod,gettext("\
  Счёт  |Счёт кор|  Дебет   |  Кредит  |\n"));
fprintf(ffsvod,"\
----------------------------------------\n");

int kolelem=SHET.kolih();

for(nomer=0; nomer < kolelem; nomer++)
 {
  strcpy(strsql,SHET.ravno(nomer));
  polen(strsql,&shet,1,'|');
  polen(strsql,&shetk,2,'|');
  
  fprintf(ffsvod,"%-*s %-*s %10.2f %10.2f\n",
  iceb_tu_kolbait(8,shet.ravno()),shet.ravno(),
  iceb_tu_kolbait(8,shetk.ravno()),shetk.ravno(),
  DEB.ravno(nomer),
  KRE.ravno(nomer));
  
 }
fprintf(ffsvod,"\
----------------------------------------\n");
fprintf(ffsvod,"%*s %10.2f %10.2f\n",
iceb_tu_kolbait(17,gettext("Итого")),gettext("Итого"),dbt,krt);

podpis(ffsvod);
fclose(ffsvod);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка проводок"));

oth.spis_imaf.plus(imafden);
oth.spis_naim.plus(gettext("Распечатка проводок с дневными итогами"));

oth.spis_imaf.plus(imafsvod);
oth.spis_naim.plus(gettext("Свод по счетам"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);

iceb_t_rabfil(&oth,"");

return(0);
}

/*******/
/*Шапка*/
/*******/
void	rapsap(FILE *ff1)
{

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
  Дата    | Счёт |Сч.к-т|Контра.| Кто   |Номер док.|   Дебет  |  Кредит  |КЭКЗ|К.о.|      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");

}
/************************************/
/*Формирование комментария           */
/************************************/

void sozkom_prov(class iceb_tu_str *koment,
char *komentprov, //Коментарий из проводки
char *kto, //Из какой подсистемы сделана проводка
char *kodop) //Код операции
{
char   strsql[512];
SQL_str row1;
SQLCURSOR cur1;
//memset(koment,'\0',razmk);
koment->new_plus(komentprov);
if(koment->getdlinna() > 1)
  return;
  
//if(koment[0] == '\0' && kto[0] == '\0' )
if(koment->getdlinna() <= 1 && kto[0] == '\0' )
  return;

memset(strsql,'\0',sizeof(strsql));
if(SRAV(kto,ICEB_MP_MATU,0) == 0)
 {
  sprintf(strsql,"select naik from Prihod where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    koment->new_plus(row1[0]);
  else
   {
    sprintf(strsql,"select naik from Rashod where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      koment->new_plus(row1[0]);
   }
 }

if(SRAV(kto,ICEB_MP_PPOR,0) == 0)
 {
  sprintf(strsql,"select naik from Opltp where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    koment->new_plus(row1[0]);
 }

if(SRAV(kto,ICEB_MP_KASA,0) == 0)
 {
  sprintf(strsql,"select naik from Кasop1 where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    koment->new_plus(row1[0]);
  else
   {
    sprintf(strsql,"select naik from Kasop2 where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      koment->new_plus(row1[0]);
   }
 }

if(SRAV(kto,ICEB_MP_USLUGI,0) == 0)
 {
  sprintf(strsql,"select naik from Usloper1 where kod='%s'",kodop);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      koment->new_plus(row1[0]);
  else
   {
    sprintf(strsql,"select naik from usloper2 where kod='%s'",kodop);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      koment->new_plus(row1[0]);
   }
 }
   

}
