/*$Id: zaresvo.c,v 5.8 2013/09/26 09:43:45 sasa Exp $*/
/*28.02.2011	28.02.2011	zaresvo.c 	Белых А.И.	zaresvo.c
Отчёт на выплаченный единый социальный взнос
*/
#include <errno.h>
#include "buhg.h"

extern short *obud; /*Обязательные удержания*/

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/

extern float proc_esv; /*процент единого социального взноса с работника*/
extern float proc_esv_bol; /*процент единого социального взноса с работника*/
extern float proc_esv_inv; /*процент единого социального взноса с работника*/
extern float proc_esv_dog; /*процент единого социального взноса с работника*/

int zaresvo(class spis_oth *oth)
{
int tbn;
int kom=0;
class iceb_tu_str fiov("");
static class iceb_tu_str datr("");
static class iceb_tu_str tabnom("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт суммы единого социального взноса на выплаченную зарплату"));

VV.VVOD_SPISOK_add_data(datr.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),112);

VV.VVOD_SPISOK_add_MD(gettext("Дата....(м.г)........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,).."));//1

naz:;
clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datr.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("zarp4_7_1_1.txt");
   goto naz;

  case FK2:
  case PLU:
    break;
      
  case FK3: //Просмотр списка табельных номеров
    tbn=0;
    fiov.new_plus("");
    if(dirtab(&tbn,&fiov,0,1) == 0)
     {
      VV.data_z_plus(1,tbn);
     }    
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
    goto naz;
 }


short mr=0,gr=0;
if(rsdat1(&mr,&gr,datr.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }
char strsql[512];
int kolstr=0;
int kolstr1=0;
int kolstr2=0;
class SQLCURSOR cur,cur1;
SQL_str row,row1;
double suma_str[11]; /*начиленные суммы*/
double suma_str_v[11]; /*выплаченные суммы*/
double i_suma_str[11]; /*итоги начисленных сумм*/
double i_suma_str_v[11]; /*Итоги выплаченных сумм*/

memset(i_suma_str,'\0',sizeof(i_suma_str));
memset(i_suma_str_v,'\0',sizeof(i_suma_str_v));

sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного табельного номера !"));
  return(1);
 }
char imaf[64];
sprintf(imaf,"esvo%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
FILE *ff_prot;
char imaf_prot[64];
sprintf(imaf_prot,"pdp%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт суммы единого социального взноса на выплаченную зарплату"),0,0,0,0,mr,gr,ff);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |       Прізвище               |Нараховано|Обов'язков|До видачі |   ЄСВ               |  ЄСВ з лікарняних   |   ЄСВ з інвалідів   |  ЄСВ з договорів    |\n\
     |                              |          |і утриман.|          |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |\n");
/*********
                                         0          1          2          3         4           5          6           7         8          9         10  
                                                               
12345 123456789012345678901234567890 1234567890 1234567890 1234567890
**********/

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
double sumanu=0.;
int knah=0;
zarrnesv();
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;

  fiov.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fiov.new_plus(row1[0]);

  fprintf(ff_prot,"\n\n%s %s\n",row[0],fiov.ravno());
  
  sprintf(strsql,"select prn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%d' and tabn=%s and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    fprintf(ff_prot,"Не найдено ни одного начисления или удержания!\n");
    continue;
   }
  memset(suma_str,'\0',sizeof(suma_str));
  memset(suma_str_v,'\0',sizeof(suma_str_v));

  while(cur1.read_cursor(&row1) != 0)
   {
    
    sumanu=atof(row1[2]);
    knah=atoi(row1[1]);
    
    if(atoi(row1[0]) == 1) /*начисления*/
     {
      suma_str[0]+=sumanu;
      
     }
    else
     {

      if(knah == kod_esv)
       suma_str[4]+=sumanu;
      if(knah == kod_esv_bol)
       suma_str[6]+=sumanu;
      if(knah == kod_esv_inv)
       suma_str[8]+=sumanu;
      if(knah == kod_esv_dog)
       suma_str[10]+=sumanu;

      if(provkod(obud,knah) >= 0)
       {
        suma_str[1]+=sumanu;
        continue;
       }

      if(atoi(row1[3]) == gr && atoi(row1[4]) == mr)      
        suma_str_v[2]+=sumanu;
     }

    

   }
  if(ff_prot != 0)
   fprintf(ff_prot,"Выплачено в расчётном месяце=%.2f\n",suma_str_v[2]);
   
  /*Смотрим начиления на фонд зарплаты*/
  sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%s",gr,mr,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  int kod_fonda=0;

  while(cur1.read_cursor(&row1) != 0)
   {
    kod_fonda=atoi(row1[0]);
    sumanu=atof(row1[1]);

    if(kod_fonda == kodf_esv)
     suma_str[3]+=sumanu;    
    if(kod_fonda == kodf_esv_bol)
     suma_str[5]+=sumanu;    
    if(kod_fonda == kodf_esv_inv)
     suma_str[7]+=sumanu;    
    if(kod_fonda == kodf_esv_dog)
     suma_str[9]+=sumanu;    

   }

  /*Определяем все выплаты в счёт этого месяца*/
  double suma_bv=0.;

  suma_str_v[2]+=prbper(mr,gr,mt,gt,atoi(row[0]),&suma_bv,0);
  suma_str_v[2]*=-1;  
  suma_str[2]=suma_str[0]+suma_str[1]; /*Прибавляем потому что отрицательное*/
  /*определяем процент выплаты*/
  double proc_vip=0.;
  if(suma_str[2] != 0.)
   proc_vip=suma_str_v[2]*100./suma_str[2];
  /*определяем сумму единого социального взноса на выплаченные суммы*/
  suma_str_v[3]=suma_str[3]*proc_vip/100.;
  suma_str_v[4]=suma_str[4]*proc_vip/100.;
  suma_str_v[5]=suma_str[5]*proc_vip/100.;
  suma_str_v[6]=suma_str[6]*proc_vip/100.;
  suma_str_v[7]=suma_str[7]*proc_vip/100.;
  suma_str_v[8]=suma_str[8]*proc_vip/100.;
  suma_str_v[9]=suma_str[9]*proc_vip/100.;
  suma_str_v[10]=suma_str[10]*proc_vip/100.;


  i_suma_str[0]+=suma_str[0];
  i_suma_str[1]+=suma_str[1];
  i_suma_str[2]+=suma_str[2];
  i_suma_str[3]+=suma_str[3];
  i_suma_str[4]+=suma_str[4];
  i_suma_str[5]+=suma_str[5];
  i_suma_str[6]+=suma_str[6];
  i_suma_str[7]+=suma_str[7];
  i_suma_str[8]+=suma_str[8];
  i_suma_str[9]+=suma_str[9];
  i_suma_str[10]+=suma_str[10];

  i_suma_str_v[0]+=suma_str_v[0];
  i_suma_str_v[1]+=suma_str_v[1];
  i_suma_str_v[2]+=suma_str_v[2];
  i_suma_str_v[3]+=suma_str_v[3];
  i_suma_str_v[4]+=suma_str_v[4];
  i_suma_str_v[5]+=suma_str_v[5];
  i_suma_str_v[6]+=suma_str_v[6];
  i_suma_str_v[7]+=suma_str_v[7];
  i_suma_str_v[8]+=suma_str_v[8];
  i_suma_str_v[9]+=suma_str_v[9];
  i_suma_str_v[10]+=suma_str_v[10];

  fprintf(ff,"%5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  row[0],
  iceb_tu_kolbait(30,fiov.ravno()),
  iceb_tu_kolbait(30,fiov.ravno()),
  fiov.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2],
  suma_str[3],
  suma_str[4],
  suma_str[5],
  suma_str[6],
  suma_str[7],
  suma_str[8],
  suma_str[9],
  suma_str[10]);

  fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(58,gettext("Выплачено")),
  gettext("Выплачено"),
  suma_str_v[2],
  suma_str_v[3],
  suma_str_v[4],
  suma_str_v[5],
  suma_str_v[6],
  suma_str_v[7],
  suma_str_v[8],
  suma_str_v[9],
  suma_str_v[10]);

  fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(58,gettext("Остаток")),
  gettext("Остаток"),
  suma_str[2]-suma_str_v[2],
  suma_str[3]-suma_str_v[3],
  suma_str[4]-suma_str_v[4],
  suma_str[5]-suma_str_v[5],
  suma_str[6]-suma_str_v[6],
  suma_str[7]-suma_str_v[7],
  suma_str[8]-suma_str_v[8],
  suma_str[9]-suma_str_v[9],
  suma_str[10]-suma_str_v[10]);

 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(36,gettext("Итого начислено")),
gettext("Итого начислено"),
i_suma_str[0],
i_suma_str[1],
i_suma_str[2],
i_suma_str[3],
i_suma_str[4],
i_suma_str[5],
i_suma_str[6],
i_suma_str[7],
i_suma_str[8],
i_suma_str[9],
i_suma_str[10]);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(58,gettext("Итого выплачено")),
gettext("Итого выплачено"),
i_suma_str_v[2],
i_suma_str_v[3],
i_suma_str_v[4],
i_suma_str_v[5],
i_suma_str_v[6],
i_suma_str_v[7],
i_suma_str_v[8],
i_suma_str_v[9],
i_suma_str_v[10]);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(58,gettext("Итого остаток")),
gettext("Итого остаток"),
i_suma_str[2]-i_suma_str_v[2],
i_suma_str[3]-i_suma_str_v[3],
i_suma_str[4]-i_suma_str_v[4],
i_suma_str[5]-i_suma_str_v[5],
i_suma_str[6]-i_suma_str_v[6],
i_suma_str[7]-i_suma_str_v[7],
i_suma_str[8]-i_suma_str_v[8],
i_suma_str[9]-i_suma_str_v[9],
i_suma_str[10]-i_suma_str_v[10]);

podpis(ff);
fclose(ff);
podpis(ff_prot);
fclose(ff_prot);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт сумм единого социального взноса на выплаченную зарплату"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);

}

