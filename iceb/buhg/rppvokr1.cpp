/*$Id: rppvokr1.c,v 5.28 2013/09/26 09:43:40 sasa Exp $*/
/*15.07.2015	28.10.2002	Белых А.И.	rppvokr1.c
Реестр проводок по видам операций для учета командировочных расходов
*/
#include        <errno.h>
#include        "buhg.h"

void	saprppvo(short,short,short,short,short,short,int*,int*,FILE*);
void	shetrppvo(short,short,short,short,short,short,int*,int*,FILE*);
void	itdok(double,FILE*);
void	itrppvo(int,const char*,short,short,short,short,short,short,int*,int*,SPISOK*,masiv_din_double*,FILE*);

extern int kol_strok_na_liste;

int rppvokr1(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kontrp,
const char *vkomp,
const char *sheta,
class spis_oth *oth)
{
char		strsql[1024];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
FILE		*ff;
char		imaf[64];
class iceb_tu_str kontr("");
class iceb_tu_str vkom("");
time_t		tmmn;
int		i=0;
SQLCURSOR 	curr;
time(&tmmn);

printw("%s %d.%d.%d > %d.%d.%d\n",gettext("Даты поиска:"),dn,mn,gn,dk,mk,gk);
printw(gettext("Сортируем проводки\n"));

GDITE();


sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,oper from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_UKR);
SQLCURSOR cur;
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
const char *imatmptab={"rppvokr1"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  

  /*Счёт*/
  if(proverka(sheta,row[1],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select kont,vkom from Ukrdok where datd='%s' and \
nomd='%s'",row[6],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("Не найден документ N %s Дата=%s\n",
    row[3],row[6]);
    printw("%s %s %s %s\n",gettext("Проводка"),
    row[1],row[2],row[3]);
    OSTANOV();
    continue;
   }
  kontr.new_plus(row1[0]);
  vkom.new_plus(row1[1]);

  if(proverka(kontrp,kontr.ravno(),0,0) != 0)
    continue;

  if(proverka(vkomp,vkom.ravno(),0,0) != 0)
    continue;

  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,'%s','%s','%s','%s')",
  imatmptab,
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],kontr.ravno(),vkom.ravno_filtr());  

  sql_zapis(strsql,1,0);    
 }

sprintf(strsql,"select * from %s order by oper asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
short	dd,md,gd; //Дата документа
short	dp,mp,gp; //Дата проводки
class iceb_tu_str nomdok("");
class iceb_tu_str shet("");
class iceb_tu_str shetk("");
double	deb=0.;
class iceb_tu_str koment("");
int 	nlist=1;
int	kolst=0;
int	kom=0;


sprintf(imaf,"rpkr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


if(kontrp[0] != '\0')
 { 
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),kontrp);
  kolst++;
 }

if(vkomp[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Вид командировки"),vkomp);
  kolst++;
 }

if(sheta[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),sheta);
  kolst++;
 }

//saprppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);

SPISOK PARSH; //Список пар счетов
masiv_din_double oper; //Суммы по операциям
masiv_din_double kont; //Суммы по конрагентам
masiv_din_double itog; //Суммы по всему расчёту

class iceb_tu_str vkom1("");
class iceb_tu_str kontr1("");
class iceb_tu_str shett("");
class iceb_tu_str naikont("");
class iceb_tu_str nomdok1("");
class iceb_tu_str naivkom("");
double itogd=0.;

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rsdat(&dp,&mp,&gp,row[0],2);

  shet.new_plus(row[1]);
  shetk.new_plus(row[2]);
  nomdok.new_plus(row[3]);
  deb=atof(row[4]);
  rsdat(&dd,&md,&gd,row[5],2);
  koment.new_plus(row[6]);

  kontr.new_plus(row[7]);

  vkom.new_plus(row[8]);

  if(SRAV(nomdok.ravno(),nomdok1.ravno(),0) != 0)
   {
    if(nomdok1.ravno()[0] != '\0')
     {
      shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
      itdok(itogd,ff);
     }
    nomdok1.new_plus(nomdok.ravno());
    itogd=0.;
   }

  if(SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
   {
    if(kontr1.ravno()[0] != '\0')
      itrppvo(0,kontr1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naikont.new_plus(row[0]);    
    else
      naikont.new_plus("");
    kom=1;
    kontr1.new_plus(kontr.ravno());
   }

  if(SRAV(vkom.ravno(),vkom1.ravno(),0) != 0)
   {
    if(vkom1.ravno()[0] != '\0')
     {
      itrppvo(1,vkom1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
      fprintf(ff,"\f");
      kolst=0;
     }    

    iceb_t_zagolov(gettext("Реестр проводок по виду командировок."),dn,mn,gn,dk,mk,gk,ff);
    kolst+=5;

    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     naivkom.new_plus(row[0]);   
    else
     naivkom.new_plus("");

    fprintf(ff,"%s:%s %s\n",gettext("Вид командировки"),
    vkom.ravno(),naivkom.ravno());
    kolst++;

    saprppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
    kom=1;
    vkom1.new_plus(vkom.ravno());
   }

//  sprintf(shett,"%s,%s",shet.ravno(),shetk.ravno());
  shett.new_plus(shet.ravno());
  shett.plus(",",shetk.ravno());
  if((i=PARSH.find(shett.ravno())) == -1)
    PARSH.plus(shett.ravno());

  oper.plus(deb,i);
  kont.plus(deb,i);
  itog.plus(deb,i);
  itogd+=deb;

  printw("%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
  dp,mp,gp,dd,md,gd,
  iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
  iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
  deb);

  shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);

  if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
     iceb_tu_kolbait(7,kontr.ravno()),
     kontr.ravno(),
     iceb_tu_kolbait(40,naikont.ravno()),
     iceb_tu_kolbait(40,naikont.ravno()),
     naikont.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     "",
     iceb_tu_kolbait(10,shett.ravno()),shett.ravno(),
     prnbr(deb),koment.ravno());

  if(kom == 0)
     fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
    "",
     iceb_tu_kolbait(10,shett.ravno()),shett.ravno(),
    prnbr(deb),koment.ravno());

  kom=0;
   
 }

itdok(itogd,ff);
itrppvo(0,kontr1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);
itrppvo(1,vkom1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
itrppvo(2,"",dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&itog,ff);

podpis(ff);

fclose(ff);

printw_vr(tmmn);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр проводок по видам командировок"));
iceb_t_ustpeh(imaf,1);
return(0);
}

/***************************************/
/*Счетчик строк                        */
/***************************************/
void	shetrppvo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,FILE *ff)
{
*kolst+=1;
if(*kolst < kol_strok_na_liste)
  return; 

fprintf(ff,"\f");
*kolst=0;
saprppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);

}

/****************************************/
/*  Шапка документа                     */
/****************************************/
void	saprppvo(
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
FILE *ff)
{
fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %*s%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
iceb_tu_kolbait(30,gettext("Лист N")),gettext("Лист N"),
*nlist);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
	Контрагент                              |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма         |Коментарий\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*nlist+=1;
*kolst+=4;
}
/**************************************/
/*Распечатка итога                    */
/**************************************/

void	itrppvo(int metka, //0-по контрагенту 1-по виду командировки 2-общий итог
const char *kod,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
SPISOK *PARSH,masiv_din_double *kontr,FILE *ff)
{
SQLCURSOR curr;
double itog=0.;
int	kolih=PARSH->kolih();
class iceb_tu_str naim("");
char	strsql[512];
SQL_str row;
double	suma=0;
class iceb_tu_str naimitog("");


if(kolih > 0)
 {
  naimitog.new_plus("");
  if(metka == 0)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);    
    naimitog.new_plus(gettext("Итого по контрагенту"));
   }     
  if(metka == 1)
   {
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);    
    naimitog.new_plus(gettext("Итого по виду командировки"));
   }
  if(metka == 2)
    naimitog.new_plus(gettext("Общий итог"));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%s %s:\n",naimitog.ravno(),naim.ravno());
 }
/*printw("kso- %d\n",kso);*/

for(int i=0; i< kolih ;i++)
 {
  suma=kontr->ravno(i);
  if(suma == 0.)
   continue;

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",
  iceb_tu_kolbait(10,PARSH->ravno(i)),PARSH->ravno(i),
  prnbr(suma));
  itog+=suma;
 }
if(itog != 0.)
 {
  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",
  iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(itog));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }
kontr->clear_class();
}
/****************************************/
/*Итог по документа                     */
/****************************************/
void	itdok(double itogd,FILE *ff)
{
fprintf(ff,"%*s %20s\n",iceb_tu_kolbait(100,gettext("Итого по документа:")),gettext("Итого по документа:"),prnbr(itogd));
}
