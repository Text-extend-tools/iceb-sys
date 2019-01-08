/* $Id: raszkar.c,v 5.24 2014/02/28 05:13:48 sasa Exp $ */
/*15.07.2015    28.01.1994      Белых А.И.      raszkar.c
Распечатка карточки материалов в файл
*/
#include <math.h>
#include        <errno.h>
#include	"buhg.h"

void raszkar(SQLCURSOR *cur,int skl,int nk,short dn,
short mn,short gn,short dk,short mk,short gk,const char *nomdp,const char *kontrp,
const char *kodop,int tipzz)
{
class ostatokt ost;
short		dp,mp,gp;
short		d,m,g;
double		kolih,cena,cenar;
short		tp;
class iceb_tu_str bros("");
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
time_t          tmm;
FILE            *f1;
char            imaf[64];
double          it;
class iceb_tu_str br1("");
class iceb_tu_str naiskl("");
class iceb_tu_str naim("");
class iceb_tu_str naimg("");
int		kodm,kgrm,kodt;
double		nds,fas,krt;
class iceb_tu_str shu("");
class iceb_tu_str ei("");
class iceb_tu_str kop(""),kor("");
class iceb_tu_str nomz(""); //Номер заказа
class iceb_tu_str innom(""); //Инвентарный номер / Серия для мед-препаратов
short		dv=0,mv=0,gv=0; //Дата ввода в эксплуатацию малоценки/дата регистрации мед препарата
class iceb_tu_str rnd(""); //регистрационный номер для мед-препарата
SQLCURSOR       cur1;
clear();
GDITE();


/*Узнаем наименование склада материалла*/
sprintf(strsql,"select naik from Sklad where kod=%d",
skl);
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s-%s %d!",__FUNCTION__,gettext("Не найден склад"),skl);
  iceb_t_soob(strsql);
  return;
 }
naiskl.new_plus(row[0]);


/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
skl,nk);
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s-%s %d %s %d!",__FUNCTION__,gettext("Не найдена карточка материалла"),nk,gettext("на складе"),skl);
  iceb_t_soob(strsql);
  return;
 }

ei.new_plus(row[4]);
fas=atof(row[10]);
krt=atof(row[8]);
kodm=atoi(row[2]);
kodt=atoi(row[11]);
cena=atof(row[6]);
cenar=atof(row[7]);
shu.new_plus(row[5]);
nds=atof(row[9]);
rsdat(&dv,&mv,&gv,row[14],2);
innom.new_plus(row[15]);
rnd.new_plus(row[16]);
nomz.new_plus(row[17]);

/*Узнаем наименование материалла*/
sprintf(strsql,"select kodgr,naimat  from Material where kodm=%d",
kodm);
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s-%s!",__FUNCTION__,gettext("Не найден код материалла в списке материаллов"));
  iceb_t_soob(strsql);
  return;
 }

kgrm=atoi(row[0]);
naim.new_plus(row[1]);


/*Узнаем наименование группы материалла*/
sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
if(readkey(strsql,&row,&cur1) == 1)
  naimg.new_plus(row[0]);

memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"k%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

time(&tmm);
bf=localtime(&tmm);

fprintf(f1,"%s\n",iceb_t_get_pnk("00",0));
fprintf(f1,gettext("Распечатка карточки %d склада %s\n"),
nk,naiskl.ravno());

fprintf(f1,"\
%s %d.%d.%d%s %s - %d:%d\n\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(f1,"%s - %d %s",gettext("Группа"),kgrm,naimg.ravno());
fprintf(f1,"\n");
fprintf(f1,"%s - %d %s\n",gettext("Материал"),kodm,naim.ravno());
fprintf(f1,"%s - %.6g %s - %.6g\n",gettext("Цена учётная"),cena,gettext("Цена реализации"),cenar);
fprintf(f1,"%s - %s %s - %.10g %s - %d\n",gettext("Счет"),shu.ravno(),gettext("НДС"),nds,gettext("Код тары"),kodt);
fprintf(f1,"%s - %s %s - %f %s - %f\n",gettext("Единица измерения"),ei.ravno(),gettext("Фасовка"),fas,gettext("Кратность"),krt);

fprintf(f1,"%s:%s\n",gettext("Номер заказа"),nomz.ravno());
if(rnd.ravno()[0] == '\0')
 {
  fprintf(f1,"%s:%s\n",gettext("Инвентарный номер"),innom.ravno());
  fprintf(f1,"%s:%d.%d.%d\n",gettext("Дата ввода в эксплуатацию"),dv,mv,gv);
 }
else
 {
  fprintf(f1,"%s:%s\n",gettext("Регистрационный номер"),rnd.ravno());
  fprintf(f1,"%s:%s\n",gettext("Серия"),innom.ravno());
  fprintf(f1,"%s:%d.%d.%d\n",gettext("Дата регистрации"),dv,mv,gv);
 }

ostkar(dn,mn,gn,dk,mk,gk,skl,nk,&ost);

fprintf(f1,"\n%-*s %10.10g %15.2f %10.10g %15.2f\n",iceb_tu_kolbait(8,gettext("Остаток")),gettext("Остаток"),ost.ostg[0],ost.ostgc[0],ost.ostm[0],ost.ostmc[0]);
fprintf(f1,"%-*s %10.10g %15.2f %10.10g %15.2f\n",iceb_tu_kolbait(8,gettext("Приход")),gettext("Приход"),ost.ostg[1],ost.ostgc[1],ost.ostm[1],ost.ostmc[1]);
fprintf(f1,"%-*s %10.10g %15.2f %10.10g %15.2f\n",iceb_tu_kolbait(8,gettext("Расход")),gettext("Расход"),ost.ostg[2],ost.ostgc[2],ost.ostm[2],ost.ostmc[2]);
fprintf(f1,"%-*s %10.10g %15.2f %10.10g %15.2f\n",iceb_tu_kolbait(8,gettext("Остаток")),gettext("Остаток"),ost.ostg[3],ost.ostgc[3],ost.ostm[3],ost.ostmc[3]);

fprintf(f1,"%s %d.%d.%d => %d.%d.%d\n",gettext("Движение за период"),dn,mn,gn,dk,mk,gk);
if(nomdp[0] != '\0')
 fprintf(f1,gettext("Номер документа: %s\n"),nomdp);
if(kontrp[0] != '\0')
 fprintf(f1,gettext("Котнтрагент: %s\n"),kontrp);
if(kodop[0] != '\0')
 fprintf(f1,"%s: %s\n",gettext("Код операции"),kodop);
if(tipzz == 1)
 fprintf(f1,gettext("Только приходы\n"));
if(tipzz == 2)
 fprintf(f1,gettext("Только расходы\n"));

fprintf(f1,"\n\
--------------------------------------------------------------------------------------------------------\n");
fprintf(f1,gettext("Дата подт.|Дата док. | Ном. доку. |Операц.  |Код конт.|Наименование контра.|  Количество |     Цена    |\n"));
fprintf(f1,"\
--------------------------------------------------------------------------------------------------------\n");


it=0.;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {

  /*Читаем код организации, код операции*/
  sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
  row[4],skl,row[2]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
      clearstr(22,0);
      printw("Не знайден документ ! \
Дата-%s Склад-%d Кар.-%d Док.-%s",
      row[4],skl,nk,row[2]);
    OSTANOV();
    continue;
   }
  tp=atoi(row1[0]);
  kop.new_plus(row1[1]);
  kor.new_plus(row1[2]);    

  if(poizkart(nomdp,row[2],kontrp,kor.ravno(),kodop,kop.ravno(),tipzz,tp) != 0)
   continue;

  bros.new_plus("");
  if(ATOFM(kor.ravno()) == 0. && polen(kor.ravno(),&bros,2,'-') == 0 && kor.ravno()[0] == '0')
    bros.new_plus("00");
  else
    bros.new_plus(kor.ravno());

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
     beep();
     printw("Не найден контрагент - %s\n",kor.ravno());
     OSTANOV();
    }
  bros.new_plus(row1[0]);

  br1.new_plus("+");
  if(tp == 2)
   {  
    br1.new_plus("-");
    kolih*=(-1);
   }
  rsdat(&dp,&mp,&gp,row[3],2);
  rsdat(&d,&m,&g,row[4],2);

  //Пропускаем промежуточные стартовые остатки
  if(SRAV(row[2],"000",0) == 0)
   if(sravmydat(d,m,g,dn,mn,gn) != 0)
     continue;

  kolih=atof(row[6]);  
  cena=atof(row[7]);  
  if(row[5][0] == '2')
    kolih*=-1;

  it+=kolih;
  
  fprintf(f1,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s %s|%-*s|%-*.*s|\
%13.13g|%13.10g\n",
  dp,mp,gp,d,m,g,
  iceb_tu_kolbait(12,row[2]),row[2],
  iceb_tu_kolbait(7,kop.ravno()),kop.ravno(),
  br1.ravno(),
  iceb_tu_kolbait(9,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(20,bros.ravno()),iceb_tu_kolbait(20,bros.ravno()),bros.ravno(),
  kolih,cena);
 }


if(fabs(it) < 0.0000000001)
  it=0.;
fprintf(f1,"\
--------------------------------------------------------------------------------------------------------\n\
%*s: %13.10g\n",iceb_tu_kolbait(74,gettext("Итого")),gettext("Итого"),it);

podpis(f1);
fclose(f1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка карточки."));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
iceb_t_rabfil(&oth,"");
}
