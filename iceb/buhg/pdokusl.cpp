/*$Id: pdokusl.c,v 5.6 2013/12/31 11:48:10 sasa Exp $*/
/*15.07.2015	11.01.2009	Белых А.И.	Белых А.И.
Просмотр документа в подсистеме "Учёт услуг
*/
#include <errno.h>
#include "buhg.h"

extern double	okrcn;  /*Округление цены*/

int pdokusl(short dd,short md,short gd,
int podr,
const char *nomdok,
int tipz)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_tu_str naim("");
FILE *ff;
char imaf[64];

sprintf(strsql,"select * from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gd,md,dd,podr,nomdok,tipz);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %d %s %d",gettext("Не найден документ!"),
  dd,md,gd,podr,nomdok,tipz);
  iceb_t_soob(strsql);
  return(1);
 
 }
int mnds=atoi(row[7]);

sprintf(imaf,"pdokusl%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",gettext("Учёт услуг"));
if(tipz == 1)
 fprintf(ff,"%s\n",gettext("Приход"));
if(tipz == 2)
 fprintf(ff,"%s\n",gettext("Расход"));
 
fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);



naim.new_plus("");
sprintf(strsql,"select naik from Uslpodr where kod=%d",podr);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),podr,naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[3],naim.ravno());

naim.new_plus("");
if(tipz == 1)
  sprintf(strsql,"select naik from Usloper1 where kod='%s'",row[6]);
if(tipz == 2)
  sprintf(strsql,"select naik from Usloper2 where kod='%s'",row[6]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[6],naim.ravno());

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_t_kszap(row[21]));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_tu_vremzap(row[22]));


if(atoi(row[27]) == 0)
 fprintf(ff,"%s\n",gettext("Документ неоплачен"));
if(atoi(row[27]) == 1)
 fprintf(ff,"%s\n",gettext("Документ оплачен"));

naim.new_plus("");       
sprintf(strsql,"select naik from Foroplat where kod='%s'",row[8]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);
fprintf(ff,"%s:%s %s\n",gettext("Форма оплаты"),row[8],naim.ravno());

if(row[9][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата оплаты"),iceb_tu_datzap(row[9]));


if(row[10][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Условие продажи"),row[10]);

double suma_kor=atof(row[11]);

if(row[16][0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Номер налоговой накладной"),row[16]);

if(row[24][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата получения налоговой накладной"),iceb_tu_datzap(row[24]));
if(row[17][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата выписки налоговой накладной"),iceb_tu_datzap(row[17]));

if(row[18][0] != '\0')
 {
  fprintf(ff,"%s:%s ",gettext("Доверенность"),row[18]);
  fprintf(ff,"%s:%s\n",gettext("Дата"),iceb_tu_datzap(row[19]));
 }
if(row[20][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Через кого"),row[20]);

if(row[23][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Основание"),row[23]);

double suma_nds=atof(row[25]);
float pnds=atof(row[26]);

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Код   |  Наименование услуги         |Счёт уч.|Eд.изм.|Количество|  Цена    | Сумма    |\n");
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,ktoi,vrem,dnaim from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and tp=%d and nomd='%s'",
gd,md,dd,podr,tipz,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
char naimu[112];
int metka=0;
double kolih=0.;
double cena=0.;
double suma=0.;
double itogo=0.;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=okrug(cena,okrcn);
  suma=kolih*cena;
  itogo+=suma;
    
  /*Читаем наименование услуги*/
  memset(naimu,'\0',sizeof(naimu));
  
  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",atoi(row[1]));

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    if(metka == 0)
     printw("%s %s !\n",gettext("Не найден код материалла"),row[1]);
    if(metka == 1)
     printw("%s %s !\n",gettext("Не найден код услуги"),row[1]);
    OSTANOV();
   }
  else
   {
    strncpy(naimu,row1[0],sizeof(naimu)-1);
    if(row[7][0] != '\0')
     {
      strcat(naimu," ");
      strcat(naimu,row[7]);
     }    
   }

  fprintf(ff,"%6s %-*.*s %-*.*s %-*.*s %10.10g %10.10g %10.2f %s %s\n",
  row[1],
  iceb_tu_kolbait(30,naimu),
  iceb_tu_kolbait(30,naimu),
  naimu,
  iceb_tu_kolbait(8,row[5]),
  iceb_tu_kolbait(8,row[5]),
  row[5],
  iceb_tu_kolbait(7,row[4]),
  iceb_tu_kolbait(7,row[4]),
  row[4],
  kolih,cena,suma,
  iceb_tu_vremzap(row[7]),
  iceb_t_kszap(row[6]));
  

  if(iceb_tu_strlen(naimu) > 30)
   fprintf(ff,"%6s %s\n","",iceb_tu_adrsimv(30,naimu));
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(76,gettext("Итого")),gettext("Итого"),itogo);

if(suma_kor < 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(76,gettext("Скидка")),gettext("Скидка"),suma_kor);
if(suma_kor > 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(76,gettext("Надбавка")),gettext("Надбавка"),suma_kor);

double oplata=itogo-suma_kor;

if(mnds == 0) 
 {
  double nds=0.;

  if(suma_nds != 0.)
   nds=suma_nds;
  else
    nds=oplata*pnds/100.;

  oplata=oplata+nds;
  fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(76,gettext("НДС")),gettext("НДС"),nds);
 }    
if(itogo != oplata)
 fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(76,gettext("Итого")),gettext("Итого"),oplata);
podpis(ff);
fclose(ff);

prosf(imaf);

unlink(imaf);

return(0);
}
