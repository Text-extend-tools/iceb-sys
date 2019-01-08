/*$Id: mudmc.c,v 5.17 2013/09/26 09:43:35 sasa Exp $*/
/*27.02.2017	08.04.2009	Белых А.И.	mudmc.c
Движение материальных ценностей по кодам материалов
*/
#include <errno.h>
#include "buhg.h"

class mudmc_poi
 {
  public:
  class iceb_tu_str dat1;
  class iceb_tu_str dat2;
  class iceb_tu_str sklz;
  class iceb_tu_str shbz;
  class iceb_tu_str kodmz;
  class iceb_tu_str kgrmz;
  class iceb_tu_str naim_poi;

  mudmc_poi()
   {
    clear();
   }
  void clear()
   {
    dat1.new_plus("");
    dat2.new_plus("");
    sklz.new_plus("");
    shbz.new_plus("");
    kodmz.new_plus("");
    kgrmz.new_plus("");
    naim_poi.new_plus("");
    
   }
 };

class mudmc_oper
 {
  public:
   class SPISOK kodm_ei;
   class SPISOK oper;
   class masiv_din_double suma;
   class masiv_din_double kolih;
   class masiv_din_double kodm_ei_kol; /*Итог по горизонтали*/
   class masiv_din_double kodm_ei_sum; /*Итог по горизонтали*/
   class masiv_din_double oper_kol; /*Итог по вертикали*/
   class masiv_din_double oper_sum; /*Итог по вертикали*/
   
   int metka; /*1-приход 2-расход*/
   
 };

void mudmc_po(short dn,short mn,short gn,short dk,short mk,short gk,const char *imaf,class mudmc_oper *prr,class mudmc_poi *poi);
void mudmc_rkp(class mudmc_poi *poi,FILE *ff);

extern double	okrcn;
extern double   okrg1; /*Округление*/

int mudmc(class spis_oth *oth) //0-нормально 1-для кладовщика*/
{
class mudmc_oper prihod;
class mudmc_oper rashod;

int kodm=0;
int kgr=0;
class iceb_tu_str naim("");
char strsql[1024];
class iceb_tu_str naimk("");
int kom=0;
int kom1=0;
static class mudmc_poi poi;

class VVOD VV(0);
class VVOD MENU(3);


VV.VVOD_SPISOK_add_data(poi.dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(poi.dat2.ravno(),11);//1
VV.VVOD_SPISOK_add_data(poi.shbz.ravno(),112);//2
VV.VVOD_SPISOK_add_data(poi.sklz.ravno(),112);//3
VV.VVOD_SPISOK_add_data(poi.kgrmz.ravno(),112);//4
VV.VVOD_SPISOK_add_data(poi.kodmz.ravno(),112);//5
VV.VVOD_SPISOK_add_data(poi.naim_poi.ravno(),112);//6

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт движения по кодам материалов"));


VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт..(,,).............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Группа материалов.(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код материалла....(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Наименование мат. .(,,)."));//6

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);




kom=VV.vvod(0,1,1,-1,-1);

poi.dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
poi.dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
poi.shbz.new_plus(VV.VVOD_SPISOK_return_data(2));
poi.sklz.new_plus(VV.VVOD_SPISOK_return_data(3));
poi.kgrmz.new_plus(VV.VVOD_SPISOK_return_data(4));
poi.kodmz.new_plus(VV.VVOD_SPISOK_return_data(5));
poi.naim_poi.new_plus(VV.VVOD_SPISOK_return_data(6));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_23.txt");
    goto naz;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;


      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         {        
          VV.data_z_plus(3,kods.ravno());
          }
        goto naz;

      case 1 :
        vibrek("Grup",&poi.kgrmz);
        VV.VVOD_SPISOK_zapis_data(4,poi.kgrmz.ravno());
        goto naz;

      case 2 :

        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(5,kodm);
         }

        clear();
        goto naz;

      case 3 :
        vibrek("Plansh",&poi.shbz);
        VV.VVOD_SPISOK_zapis_data(2,poi.shbz.ravno());
        goto naz;
     }

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }
short dn,mn,gn,dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,poi.dat1.ravno(),&dk,&mk,&gk,poi.dat2.ravno()) != 0)
 goto naz;
SQL_str row,row1,row2;
class SQLCURSOR cur,cur1,cur2;

prihod.metka=1;
rashod.metka=2;
int kolstr1=0;

sprintf(strsql,"select distinct kodm,ei from Kart order by kodm asc");
int kolstr=0;
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
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"%s|%s",row[0],row[1]);
  prihod.kodm_ei.plus(strsql);
  rashod.kodm_ei.plus(strsql);
 }

sprintf(strsql,"select kod from Prihod");
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
 prihod.oper.plus(row[0]);

sprintf(strsql,"select kod from Rashod");
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
 rashod.oper.plus(row[0]);
 
prihod.suma.make_class(prihod.kodm_ei.kolih()*prihod.oper.kolih());
rashod.suma.make_class(rashod.kodm_ei.kolih()*rashod.oper.kolih());

prihod.kolih.make_class(prihod.kodm_ei.kolih()*prihod.oper.kolih());
rashod.kolih.make_class(rashod.kodm_ei.kolih()*rashod.oper.kolih());

prihod.kodm_ei_kol.make_class(prihod.kodm_ei.kolih());
prihod.kodm_ei_sum.make_class(prihod.kodm_ei.kolih());
prihod.oper_kol.make_class(prihod.oper.kolih());
prihod.oper_sum.make_class(prihod.oper.kolih());

rashod.kodm_ei_kol.make_class(rashod.kodm_ei.kolih());
rashod.kodm_ei_sum.make_class(rashod.kodm_ei.kolih());
rashod.oper_kol.make_class(rashod.oper.kolih());
rashod.oper_sum.make_class(rashod.oper.kolih());


class SPISOK kodm_ei; /*Список кодов материалла-единиц измерения*/

class masiv_din_double kolih1; /*количество на начало периода*/
class masiv_din_double kolih2; /*Количество приход за период*/
class masiv_din_double kolih3; /*Количество расход за период*/
class masiv_din_double kolih4; /*Количество остаток на конец периода*/

class masiv_din_double suma1; /*CУмма на начало периода*/
class masiv_din_double suma2; /*CУмма приход за период*/
class masiv_din_double suma3; /*CУмма расход за период*/
class masiv_din_double suma4; /*CУмма остаток на конец периода*/

sprintf(strsql,"select sklad,nomk,kodm,ei,shetu,cena from Kart order by kodm asc");
kolstr=0;
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
class ostatokt ost;
kolstr1=0;
int nomer=0;
int kolstr2=0;
int nomer_op=0;
double kolih=0.;
double suma=0.;
short d,m,g;
int tipz=0;
double cena_po_kart=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(poi.shbz.ravno(),row[4],0,0) != 0)
    continue;
  if(proverka(poi.kodmz.ravno(),row[2],0,0) != 0)
    continue;
  if(proverka(poi.sklz.ravno(),row[0],0,0) != 0)
    continue;

  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(poi.kgrmz.ravno(),row1[0],0,0) != 0)
      continue;
    if(proverka(poi.naim_poi.ravno(),row1[1],4,0) != 0)
      continue;
   }

  ostkar(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0.)
   continue;  

  cena_po_kart=atof(row[5]);
  cena_po_kart=okrug(cena_po_kart,okrcn);
  
  sprintf(strsql,"%s|%s",row[2],row[3]);
  if((nomer=kodm_ei.find_r(strsql)) < 0)
    kodm_ei.plus(strsql);

  kolih1.plus(ost.ostm[0],nomer);
  kolih2.plus(ost.ostm[1],nomer);
  kolih3.plus(ost.ostm[2],nomer);
  kolih4.plus(ost.ostm[3],nomer);

  suma1.plus(ost.ostmc[0],nomer);
  suma2.plus(ost.ostmc[1],nomer);
  suma3.plus(ost.ostmc[2],nomer);
  suma4.plus(ost.ostmc[3],nomer);


  sprintf(strsql,"select nomd,datd,tipz,kolih,cena,datdp from Zkart where sklad=%s and nomk=%s and \
datdp >= '%04d-%02d-%02d' and datdp <= '%04d-%02d-%02d'",row[0],row[1],gn,mn,dn,gk,mk,dk);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    tipz=atoi(row1[2]);
    rsdat(&d,&m,&g,row1[5],2);

    if(SRAV(row1[0],"000",0) == 0)
        continue;

    kolih=atof(row1[3]);
    suma=kolih*cena_po_kart;
    suma=okrug(suma,okrg1);
    /*Узнаём код операции*/
    sprintf(strsql,"select kodop from Dokummat where datd='%s' and sklad=%s and nomd='%s'",

    row1[1],row[0],row1[0]);
    if(readkey(strsql,&row2,&cur2) != 1)
     {
      sprintf(strsql,"%s %s %s %s",gettext("Не нашли документ!"),row1[1],row[0],row1[0]);
      iceb_t_soob(strsql);
      continue;
     }
  
    sprintf(strsql,"%s|%s",row[2],row[3]);
        
    if(tipz == 1)
     {
      nomer_op=prihod.oper.find(row2[0]);
      nomer=prihod.kodm_ei.find(strsql);     
      prihod.suma.plus(suma,nomer*prihod.oper.kolih()+nomer_op);
      prihod.kolih.plus(kolih,nomer*prihod.oper.kolih()+nomer_op);

      prihod.kodm_ei_kol.plus(kolih,nomer);
      prihod.kodm_ei_sum.plus(suma,nomer);

      prihod.oper_kol.plus(kolih,nomer_op);
      prihod.oper_sum.plus(suma,nomer_op);

     }
    else
     {
  
      nomer_op=rashod.oper.find(row2[0]);
      nomer=rashod.kodm_ei.find(strsql);     
      rashod.suma.plus(suma,nomer*rashod.oper.kolih()+nomer_op);
      rashod.kolih.plus(kolih,nomer*rashod.oper.kolih()+nomer_op);
      
      rashod.kodm_ei_kol.plus(kolih,nomer);
      rashod.kodm_ei_sum.plus(suma,nomer);


      rashod.oper_kol.plus(kolih,nomer_op);
      rashod.oper_sum.plus(suma,nomer_op);
     }
   }
 }
GDITE();
FILE *ff;
char imaf[64];
sprintf(imaf,"mudmc%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Расчёт движения по кодам материалов"),dn,mn,gn,dk,mk,gk,ff);


/*печать реквизитов поиска*/
mudmc_rkp(&poi,ff);


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  |        Наименование          |Едини.|Остаток на %02d.%02d.%04d|      Приход         |       Расход        |Остаток на %02d.%02d.%04d|\n\
м-алла|         материалла           |измер.|Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |Количество|  Сумма   |\n",
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");
class iceb_tu_str kodmat;
class iceb_tu_str ei;

for(int ii=0; ii < kodm_ei.kolih(); ii++)
 {
  polen(kodm_ei.ravno(ii),&kodmat,1,'|');
  polen(kodm_ei.ravno(ii),&ei,2,'|');
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat.ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  fprintf(ff,"\
%6d %-*.*s %-*.*s %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
  kodmat.ravno_atoi(),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
  kolih1.ravno(ii),suma1.ravno(ii),
  kolih2.ravno(ii),suma2.ravno(ii),
  kolih3.ravno(ii),suma3.ravno(ii),
  kolih4.ravno(ii),suma4.ravno(ii));
  
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f %10.10g %10.2f\n",
iceb_tu_kolbait(44,gettext("Итого")),gettext("Итого"),
kolih1.suma(), 
suma1.suma(), 
kolih2.suma(), 
suma2.suma(), 
kolih3.suma(), 
suma3.suma(), 
kolih4.suma(), 
suma4.suma()); 

podpis(ff);
fclose(ff);
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт движения по кодам материалов"));

sprintf(imaf,"opp%d.lst",getpid());

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расшифровка приходов по операциям"));

mudmc_po(dn,mn,gn,dk,mk,gk,imaf,&prihod,&poi);

sprintf(imaf,"opr%d.lst",getpid());

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расшифровка расходов по операциям"));

mudmc_po(dn,mn,gn,dk,mk,gk,imaf,&rashod,&poi);

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}


/**************************************/
/*Распечатка масивов по кодам операций*/
/**************************************/
void mudmc_po(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,class mudmc_oper *prr,
class mudmc_poi *poi)
{
char strsql[512];
class iceb_tu_str naim("");
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str liniq;
double kol=0.;
double sum=0.;
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

if(prr->metka == 1)
  iceb_t_zagolov(gettext("Расшифровка приходов по операциям"),dn,mn,gn,dk,mk,gk,ff);
if(prr->metka == 2)
  iceb_t_zagolov(gettext("Расшифровка расходов по операциям"),dn,mn,gn,dk,mk,gk,ff);

/*печать реквизитов поиска*/
mudmc_rkp(poi,ff);

liniq.plus("-----------------------------------");
//          12345678901234567890123456789012345
for(int ii=0; ii < prr->oper.kolih(); ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  liniq.plus("----------------------");
 }
liniq.plus("----------------------"); /*полка над итого*/
//            1234567890123456789012
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"\
 Код  |    Наименование    |Едини.|");
/*
12345678901234567890123456789012345
*/
for(int ii=0; ii < prr->oper.kolih(); ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  naim.new_plus(prr->oper.ravno(ii));
  naim.plus(" ");
  if(prr->metka == 1)
    sprintf(strsql,"select naik from Prihod where kod='%s'",prr->oper.ravno(ii));
  if(prr->metka == 2)
    sprintf(strsql,"select naik from Rashod where kod='%s'",prr->oper.ravno(ii));
  if(readkey(strsql,&row,&cur) == 1)
    naim.plus(row[0]);   
  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(21,naim.ravno()),iceb_tu_kolbait(21,naim.ravno()),naim.ravno());
 }
fprintf(ff,"       Итого         |\n");

fprintf(ff,"\
матер.|    материала       |измер.|");
for(int ii=0; ii < prr->oper.kolih(); ii++)
 {
  if(prr->oper_kol.ravno(ii) == 0.)
   continue;
  fprintf(ff,"Количеств.|  Сумма   |");
 }
fprintf(ff,"Количеств.|  Сумма   |");/*для итого*/

fprintf(ff,"\n");
fprintf(ff,"%s\n",liniq.ravno());
class iceb_tu_str kodm("");
class iceb_tu_str ei("");
int koloper=prr->oper.kolih();
for(int nomer_mat=0; nomer_mat < prr->kodm_ei.kolih(); nomer_mat++)
 {
  /*Строку с нолями пропускаем*/
  if(prr->kodm_ei_kol.ravno(nomer_mat) == 0.)
   continue;

  polen(prr->kodm_ei.ravno(nomer_mat),&kodm,1,'|');
  polen(prr->kodm_ei.ravno(nomer_mat),&ei,2,'|');
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%s",kodm.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  
  fprintf(ff,"%6s %-*.*s|%-*.*s|",
  kodm.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),iceb_tu_kolbait(6,ei.ravno()),ei.ravno());
  
  for(int nomer_op=0; nomer_op < koloper; nomer_op++)
   {
    if(prr->oper_kol.ravno(nomer_op) == 0.)
     continue;

    kol=prr->kolih.ravno(nomer_mat*koloper+nomer_op);
    sum=prr->suma.ravno(nomer_mat*koloper+nomer_op);
    fprintf(ff,"%10.10g|%10.2f|",kol,sum);
    
   }
  fprintf(ff,"%10.10g|%10.2f|\n",prr->kodm_ei_kol.ravno(nomer_mat),prr->kodm_ei_sum.ravno(nomer_mat));


 }

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"\
         Итого                    |");
for(int nomer_op=0; nomer_op < koloper; nomer_op++)
 {
  if(prr->oper_kol.ravno(nomer_op) == 0.)
   continue;

  fprintf(ff,"%10.10g|%10.2f|",prr->oper_kol.ravno(nomer_op),prr->oper_sum.ravno(nomer_op));
 }
fprintf(ff,"%10.10g|%10.2f|\n",prr->oper_kol.suma(),prr->oper_sum.suma());

fprintf(ff,"%s\n",liniq.ravno());

podpis(ff);
fclose(ff);


}

/********************************/
/*Реквизиты поиска*/
/**********************/
void mudmc_rkp(class mudmc_poi *poi,FILE *ff)
{
if(poi->sklz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),poi->sklz.ravno());
if(poi->shbz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),poi->shbz.ravno());
if(poi->kodmz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),poi->kodmz.ravno());
if(poi->kgrmz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код группы материалла"),poi->kgrmz.ravno());
if(poi->naim_poi.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Наименование материалла"),poi->naim_poi.ravno());
}
