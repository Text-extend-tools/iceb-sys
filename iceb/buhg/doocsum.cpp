/* $Id: doocsum.c,v 5.32 2013/09/26 09:43:32 sasa Exp $ */
/*01.12.2016	30.5.2001	Белых А.И.	doocsum.c
Получение суммы дооценки выполненой в автоматическом режиме 
по документам
*/
#include        <errno.h>
#include        "buhg.h"

int doocsumr(short,short,short,short,short,short,const char*,const char *kodop,class spis_oth*);


void	doocsum(void)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str sklad("");
static class iceb_tu_str kodop("");
short		dn,mn,gn,dk,mk,gk;
int		kom=0;
int kom1=0;
class VVOD VV(0);
class VVOD MENU(3);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklad.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции дооценки.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Склад......(,,)............."));

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт суммы выполненых дооценок"));


naz1:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

kodop.new_plus(VV.data_ravno(2));
sklad.new_plus(VV.data_ravno(3));

class iceb_tu_str kod("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return;
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_16.txt");
    clear();
    goto naz1;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz1;


      case 0 :
//        if(vibrek("Prihod",&kod) == 0)
        if(dirprihod(1,&kod,&naims) == 0)
          VV.data_z_plus(2,kod.ravno());
        goto naz1;

      case 1 :
        kod.new_plus("");
        naims.new_plus("");
        if(dirsklad(1,&kod,&naims) == 0)
           VV.data_z_plus(3,kod.ravno());

        goto naz1;

     }

    goto naz1;


  default:
    goto naz1;
    break;
 }
if(kodop.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён код операции!"));
  goto naz1;
  
 }
if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz1;
class spis_oth oth;
if(doocsumr(dn,mn,gn,dk,mk,gk,sklad.ravno(),kodop.ravno(),&oth) == 0)
 iceb_t_rabfil(&oth,"");

}
/*****************************/
/*  Расчёт дооценки           */
/*******************************/

int doocsumr(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *sklad,
const char *kodopdooc,
class spis_oth *oth)
{
SQL_str	row,row1;
char	strsql[1024];
int	kolstr=0,kolstr1=0;
short	d,m,g;
int vt; //возвратная тара
double	sum=0.,sum1=0.;
double  sumkor=0.;
double  suma=0.;
double  sumabn=0.;
int	mnds=0;
FILE	*ff;
char	imaf[64];
struct  tm      *bf;
time_t	   	tmm;
double	sumi[3];
class iceb_tu_str osnov("");
double  nds=0.;

clear();
printw("%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
GDITE();


sprintf(strsql,"select datd,sklad,nomd,nomon,pn,kodop from Dokummat where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and tip=1",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }


vt=0;
iceb_t_poldan("Код группы возвратная тара",&vt,"matnast.alx");

sprintf(imaf,"dooc%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
startfil(ff);
time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"%s\n\n\
%s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Расчёт суммы выполненых дооценок"),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"%s:%s\n",gettext("Код операции"),kodopdooc);
if(sklad[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Склад|   Дата   |Номер док.|Номер док.|        Сумма        |  Сумма   |Основание\n"));
fprintf(ff,gettext("\
     |          | приход   |  расход  |  приход  |  расход  | дооценки |\n"));
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
memset(&sumi,'\0',sizeof(sumi));
float pnds=0.;
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  printw("%s %s %s\n",row[0],row[1],row[2]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kodopdooc,row[5],0,0) != 0)
    continue;
  if(proverka(sklad,row[1],0,0) != 0)
    continue;
    
  rsdat(&d,&m,&g,row[0],2);
  pnds=atof(row[4]);
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[2],row[1]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    mnds=atoi(row1[0]);
   }  
  //Узнаем основание документа
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s \
and nomd='%s' and nomerz=3",g,row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    osnov.new_plus(row1[0]);
  else
    osnov.new_plus("");    
  sumzap1(d,m,g,row[2],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum=suma+sumkor+nds+sumabn;
  
  sumzap1(d,m,g,row[3],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,2,&kolih);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum1=suma+sumkor+nds+sumabn;
  sumi[0]+=sum;
  sumi[1]+=sum1;
  sumi[2]+=sum-sum1;
  fprintf(ff,"%-5s %02d.%02d.%d %-10s %-10s %10.2f %10.2f %10.2f %s\n",
  row[1],d,m,g,row[2],row[3],sum,sum1,sum-sum1,osnov.ravno());
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(38,gettext("Итого")),
gettext("Итого"),
sumi[0],sumi[1],sumi[2]);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт суммы выполненых дооценок"));
return(0);
}
