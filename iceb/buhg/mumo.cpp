/*$Id: mumo.c,v 5.8 2013/09/26 09:43:36 sasa Exp $*/
/*26.02.2017	01.12.2009	Белых А.И.	mumo.c
Материальный отчёт
*/

#include        <errno.h>
#include        "buhg.h"
class mumo_poi
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str sklad;
   class iceb_tu_str grup_mat;
   class iceb_tu_str kodmat;
   class iceb_tu_str shetz;
   
  mumo_poi()
   {
    clear();
   }
  void clear()
   {
    dat1.new_plus("");
    dat2.new_plus("");
    sklad.new_plus("");
    grup_mat.new_plus("");
    kodmat.new_plus("");
    shetz.new_plus("");
   }   
 };

void mumo_sap(short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *kontr_prih,class SPISOK *kontr_rash,class iceb_tu_str *liniq,FILE *ff);




int mumo(class spis_oth *oth)
{
char strsql[1024];
static mumo_poi rekp;
int kom=0,kom1=0;
int kodgr=0;
int kodmati=0;
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт материальгого отчёта"));

VV.VVOD_SPISOK_add_data(rekp.dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(rekp.dat2.ravno(),11);//1

VV.VVOD_SPISOK_add_data(rekp.sklad.ravno(),112);//2
VV.VVOD_SPISOK_add_data(rekp.grup_mat.ravno(),112);//3
VV.VVOD_SPISOK_add_data(rekp.kodmat.ravno(),112);//4
VV.VVOD_SPISOK_add_data(rekp.shetz.ravno(),112);//5

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад...........(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Группа мат-лла..(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код материалла..(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.....(,,).."));//5

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

rekp.dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
rekp.dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
rekp.sklad.new_plus(VV.VVOD_SPISOK_return_data(2));
rekp.grup_mat.new_plus(VV.VVOD_SPISOK_return_data(3));
rekp.kodmat.new_plus(VV.VVOD_SPISOK_return_data(4));
rekp.shetz.new_plus(VV.VVOD_SPISOK_return_data(5));

class iceb_tu_str kods("");
class iceb_tu_str naims("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_21.txt");
//    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&rekp.grup_mat);
        VV.VVOD_SPISOK_zapis_data(3,rekp.grup_mat.ravno());
        goto naz;


      case 2:
        if(dirmatr(&kodgr,&kodmati,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(4,kodmati);
         }
        
        goto naz;

      case 3 :
        vibrek("Plansh",&rekp.shetz);
        VV.VVOD_SPISOK_zapis_data(5,rekp.shetz.ravno());
        goto naz;


     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

short dn,mn,gn,dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;
GDITE();
class SQLCURSOR cur,cur1,cur2;
SQL_str row,row1,row2;

int kolstr=0;
int kolstr1=0;

sprintf(strsql,"select sklad,nomk,kodm,shetu,cena from Kart order by sklad asc,shetu asc");
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
class SPISOK skl_nk;
class SPISOK kontr_prih;
class SPISOK kontr_rash;
class SPISOK shetu;
class masiv_din_double cena_kart;

class masiv_din_double prihod_suma;
class masiv_din_double rashod_suma;
class masiv_din_double prihod_kolih;
class masiv_din_double rashod_kolih;

class masiv_din_double saldon_suma;
class masiv_din_double saldok_suma;
class masiv_din_double saldon_kolih;
class masiv_din_double saldok_kolih;

class masiv_din_double ikol_kolih_prih;
class masiv_din_double ikol_suma_prih;

class masiv_din_double ikol_kolih_rash;
class masiv_din_double ikol_suma_rash;



int nomer=0; 
class ostatokt ost;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(rekp.sklad.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(rekp.kodmat.ravno(),row[2],0,0) != 0)
    continue;
  if(proverka(rekp.shetz.ravno(),row[3],0,0) != 0)
    continue;

  if(rekp.grup_mat.getdlinna() > 1)
   {
    sprintf(strsql,"select kodgr from Material where kodm=%s",row[2]);
    if(readkey(strsql,&row1,&cur1) == 1)
     if(proverka(rekp.grup_mat.ravno(),row1[0],0,0) != 0)
      continue;
   }

  
  ostkar(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0.)
   continue;  

  sprintf(strsql,"%s|%s",row[0],row[1]);
  if((nomer=skl_nk.find(strsql)) < 0)
   {
    skl_nk.plus(strsql);
    shetu.plus(row[3]);
   }
  cena_kart.plus(atof(row[4]),nomer);
  /*смотрим приходы-расходы*/
  sprintf(strsql,"select nomd,datd,tipz from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' and sklad=%s and nomk=%s",
  gn,mn,dn,gk,mk,dk,row[0],row[1]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(SRAV("000",row1[0],0) == 0)
      continue;
      
    sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' and tip=%s",
    row1[1],row[0],row1[0],row1[2]);
    if(readkey(strsql,&row2,&cur2) != 1)
     {
      sprintf(strsql,"%s %s %s %s %s",gettext("Не найдена шапка документа"),
      row1[1],row[0],row1[0],row1[2]);
      iceb_t_soob(strsql);
      continue;
     }

    if(atoi(row1[2]) == 1) /*Приход*/
     if(kontr_prih.find_r(row2[0]) < 0)
      kontr_prih.plus(row2[0]);

    if(atoi(row1[2]) == 2) /*Расход*/
     if(kontr_rash.find_r(row2[0]) < 0)
      kontr_rash.plus(row2[0]);
   }
 }
GDITE();
int kolih_skl_nk=skl_nk.kolih();
int kolih_kontrp=kontr_prih.kolih();
int kolih_kontrr=kontr_rash.kolih();

prihod_suma.make_class(kolih_skl_nk*kolih_kontrp);
rashod_suma.make_class(kolih_skl_nk*kolih_kontrr);
prihod_kolih.make_class(kolih_skl_nk*kolih_kontrp);
rashod_kolih.make_class(kolih_skl_nk*kolih_kontrr);

saldon_suma.make_class(kolih_skl_nk);
saldok_suma.make_class(kolih_skl_nk);
saldon_kolih.make_class(kolih_skl_nk);
saldok_kolih.make_class(kolih_skl_nk);

ikol_kolih_prih.make_class(kolih_kontrp);
ikol_suma_prih.make_class(kolih_kontrp);

ikol_kolih_rash.make_class(kolih_kontrr);
ikol_suma_rash.make_class(kolih_kontrr);


class iceb_tu_str sklsp("");
class iceb_tu_str nomkart("");
int nomer_kontr=0;
double kolih=0.;
double suma=0.;
kolstr1=0;

for(int nom_skl_nk=0; nom_skl_nk < kolih_skl_nk; nom_skl_nk++)
 {
  strzag(LINES-1,0,kolih_skl_nk,++kolstr1);

  polen(skl_nk.ravno(nom_skl_nk),&sklsp,1,'|');
  polen(skl_nk.ravno(nom_skl_nk),&nomkart,2,'|');

  ostkar(dn,mn,gn,dk,mk,gk,sklsp.ravno_atoi(),nomkart.ravno_atoi(),&ost);

  saldon_kolih.plus(ost.ostm[0],nom_skl_nk);
  saldok_kolih.plus(ost.ostm[3],nom_skl_nk);

  saldon_suma.plus(ost.ostmc[0],nom_skl_nk);
  saldok_suma.plus(ost.ostmc[3],nom_skl_nk);
        
  /*смотрим приходы-расходы*/
  sprintf(strsql,"select nomd,datd,tipz,kolih from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' and sklad=%s and nomk=%s",
  gn,mn,dn,gk,mk,dk,sklsp.ravno(),nomkart.ravno());

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(SRAV("000",row1[0],0) == 0)
      continue;
    sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' and tip=%s",
    row1[1],sklsp.ravno(),row1[0],row1[2]);
    if(readkey(strsql,&row2,&cur2) != 1)
     {
      sprintf(strsql,"%s %s %s %s %s",gettext("Не найдена шапка документа"),
      row1[1],row[0],row1[0],row1[2]);
      iceb_t_soob(strsql);
      continue;
     }
    kolih=atof(row1[3]);
    suma=kolih*cena_kart.ravno(nom_skl_nk);
        
    if(atoi(row1[2]) == 1) /*Приход*/
     {
      if((nomer_kontr=kontr_prih.find_r(row2[0])) < 0)
       {
        sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),row2[0]);
        iceb_t_soob(strsql);
        continue;
       }
      prihod_suma.plus(suma,nom_skl_nk*kolih_kontrp+nomer_kontr);
      prihod_kolih.plus(kolih,nom_skl_nk*kolih_kontrp+nomer_kontr);

      ikol_kolih_prih.plus(kolih,nomer_kontr);
      ikol_suma_prih.plus(suma,nomer_kontr);
     }
    if(atoi(row1[2]) == 2) /*Расход*/
     {
      if((nomer_kontr=kontr_rash.find_r(row2[0])) < 0)
       {
        sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),row2[0]);
        iceb_t_soob(strsql);
        continue;
       }
      rashod_suma.plus(suma,nom_skl_nk*kolih_kontrr+nomer_kontr);
      rashod_kolih.plus(kolih,nom_skl_nk*kolih_kontrr+nomer_kontr);
      ikol_kolih_rash.plus(kolih,nomer_kontr);
      ikol_suma_rash.plus(suma,nomer_kontr);
     }
   }
  

 }

char imaf[56];
class iceb_fopen fmo;
 
sprintf(imaf,"matot%d.lst",getpid());

if(fmo.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Материальный отчёт"),dn,mn,gn,dk,mk,gk,fmo.ff);

fprintf(fmo.ff,"\
                                                           %s\n\n",gettext("Утверждаю"));
fprintf(fmo.ff,"\
                                                    _____________________\n\n\
                                                    _____________________\n");                                 


if(rekp.sklad.getdlinna() > 1)
 fprintf(fmo.ff,"%s:%s\n",gettext("Склад"),rekp.sklad.ravno());
if(rekp.grup_mat.getdlinna() > 1)
 fprintf(fmo.ff,"%s:%s\n",gettext("Группа"),rekp.grup_mat.ravno());
if(rekp.kodmat.getdlinna() > 1)
 fprintf(fmo.ff,"%s:%s\n",gettext("Код материалла"),rekp.kodmat.ravno());
if(rekp.shetz.getdlinna() > 1)
 fprintf(fmo.ff,"%s:%s\n",gettext("Счёт"),rekp.shetz.ravno());

class iceb_tu_str liniq;

int nomer_kart=0;
int skl1=0;
int skl2=0;
int kodmat=0;
double cena=0.;
class iceb_tu_str ei("");
class iceb_tu_str naim("");
kolstr1=0;
for(int nomer_skl_nk=0; nomer_skl_nk < kolih_skl_nk; nomer_skl_nk++)
 {
  strzag(LINES-1,0,kolih_skl_nk,++kolstr1);
  polen(skl_nk.ravno(nomer_skl_nk),&skl2,1,'|');
  polen(skl_nk.ravno(nomer_skl_nk),&nomer_kart,2,'|');
  
  if(skl1 != skl2)
   {
    if(skl1 != 0)
     fprintf(fmo.ff,"%s\n",liniq.ravno());

    naim.new_plus("");
    sprintf(strsql,"select naik from Sklad where kod=%d",skl2);

    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    fprintf(fmo.ff,"%s:%d %s\n",gettext("Склад"),skl2,naim.ravno());        
    mumo_sap(dn,mn,gn,dk,mk,gk,&kontr_prih,&kontr_rash,&liniq,fmo.ff); /*Печать шапки*/

    skl1=skl2;
   }  

  sprintf(strsql,"select kodm,ei,cena from Kart where sklad=%d and nomk=%d",skl2,nomer_kart);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не найдена карточка %d на складе %d!",nomer_kart,skl2);
    iceb_t_soob(strsql);
    continue;   
   }
  kodmat=atoi(row[0]);
  ei.new_plus(row[1]);
  cena=atof(row[2]);  
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);

  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);

  fprintf(fmo.ff,"%6d|%6d|%-*.*s|%-*.*s|%-*.*s|%10.10g|",
  nomer_kart,
  kodmat,
  iceb_tu_kolbait(20,naim.ravno()),
  iceb_tu_kolbait(20,naim.ravno()),
  naim.ravno(),
  iceb_tu_kolbait(6,shetu.ravno(nomer_skl_nk)),
  iceb_tu_kolbait(6,shetu.ravno(nomer_skl_nk)),
  shetu.ravno(nomer_skl_nk),
  iceb_tu_kolbait(5,ei.ravno()),
  iceb_tu_kolbait(5,ei.ravno()),
  ei.ravno(),
  cena);

  if(saldon_kolih.ravno(nomer_skl_nk) != 0. || saldon_suma.ravno(nomer_skl_nk) != 0.)
    fprintf(fmo.ff,"%10.2f|%10.2f|",saldon_kolih.ravno(nomer_skl_nk),saldon_suma.ravno(nomer_skl_nk));
  else
    fprintf(fmo.ff,"%10s|%10s|","","");
  

  for(nomer_kontr=0; nomer_kontr < kolih_kontrp; nomer_kontr++)
   {
    kolih=prihod_kolih.ravno(nomer_skl_nk*kolih_kontrp+nomer_kontr); 
    suma=prihod_suma.ravno(nomer_skl_nk*kolih_kontrp+nomer_kontr); 

    if(suma == 0. && kolih == 0.)
     fprintf(fmo.ff,"%10s|%10s|","","");
    else
     fprintf(fmo.ff,"%10.10g|%10.2f|",kolih,suma);
   }

  for(nomer_kontr=0; nomer_kontr < kolih_kontrr; nomer_kontr++)
   {
    kolih=rashod_kolih.ravno(nomer_skl_nk*kolih_kontrr+nomer_kontr); 
    suma=rashod_suma.ravno(nomer_skl_nk*kolih_kontrr+nomer_kontr); 

    if(suma == 0. && kolih == 0.)
     fprintf(fmo.ff,"%10s|%10s|","","");
    else
     fprintf(fmo.ff,"%10.10g|%10.2f|",kolih,suma);
   }

  fprintf(fmo.ff,"%10.10g|%10.2f|\n",
  saldok_kolih.ravno(nomer_skl_nk),
  saldok_suma.ravno(nomer_skl_nk));

 }

if(liniq.getdlinna() > 1)
 {
  fprintf(fmo.ff,"%s\n",liniq.ravno());

  fprintf(fmo.ff,"%*s|%10.10g|%10.2f|",
  iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),
  saldon_kolih.suma(),
  saldon_suma.suma());


  for(nomer_kontr=0; nomer_kontr < kolih_kontrp; nomer_kontr++)
   fprintf(fmo.ff,"%10.10g|%10.2f|",
   ikol_kolih_prih.ravno(nomer_kontr),
   ikol_suma_prih.ravno(nomer_kontr));

  for(nomer_kontr=0; nomer_kontr < kolih_kontrr; nomer_kontr++)
   fprintf(fmo.ff,"%10.10g|%10.2f|",
   ikol_kolih_rash.ravno(nomer_kontr),
   ikol_suma_rash.ravno(nomer_kontr));

  fprintf(fmo.ff,"%10.10g|%10.2f|\n",
  saldok_kolih.suma(),
  saldok_suma.suma());

  fprintf(fmo.ff,"%s\n",liniq.ravno());
 }

podpis(fmo.ff);
//fclose(ff);
fmo.end();
clear();
move(LINES/2-2,0);
sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dn,mn,gn);

printw("%-*s:%12.12g %12.2f\n",
iceb_tu_kolbait(25,strsql),
strsql,
saldon_kolih.suma(),
saldon_suma.suma());

printw("%-*s:%12.12g %12.2f\n",
iceb_tu_kolbait(25,gettext("Приход")),
gettext("Приход"),
ikol_kolih_prih.suma(),
ikol_suma_prih.suma());

printw("%-*s:%12.12g %12.2f\n",
iceb_tu_kolbait(25,gettext("Расход")),
gettext("Расход"),
ikol_kolih_rash.suma(),
ikol_suma_rash.suma());

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dk,mk,gk);
printw("%-*s:%12.12g %12.2f\n",
iceb_tu_kolbait(25,strsql),
strsql,
saldok_kolih.suma(),
saldok_suma.suma());

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Материальный отчёт"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),0);


return(0);
}

/**********************************/
/*Шапка*/
/**********************************/
void mumo_sap(short dn,short mn,short gn,
short dk,short mk,short gk,
class SPISOK *kontr_prih,class SPISOK *kontr_rash,class iceb_tu_str *liniq,FILE *ff)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kod_skl=0;
class iceb_tu_str naim("");
int kolih_kontrp=kontr_prih->kolih();
int kolih_kontrr=kontr_rash->kolih();

liniq->new_plus("---------------------------------------------------------------------------------");

for(int nom=0; nom < kolih_kontrp+kolih_kontrr+1; nom++)
 liniq->plus("----------------------");

fprintf(ff,"%s\n",liniq->ravno());

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dn,mn,gn);

fprintf(ff,gettext("\
N кар-|Код м-|    Наименование    | Счёт |Един.|  Цена    |"));

fprintf(ff,"%-*s|",iceb_tu_kolbait(21,strsql),strsql);
            
if(kolih_kontrp > 0)
 fprintf(ff,"%-*s|",iceb_tu_kolbait(kolih_kontrp*22-1,gettext("Приход")),gettext("Приход"));

if(kolih_kontrr > 0)
 fprintf(ff,"%-*s|",iceb_tu_kolbait(kolih_kontrr*22-1,gettext("Расход")),gettext("Расход"));

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dk,mk,gk);
fprintf(ff,"%-*s|\n",iceb_tu_kolbait(21,strsql),strsql);

fprintf(ff,gettext("\
точки |атери-|     материалла     |учёта |изме-|          |---------------------|"));

for(int nom=0; nom < kolih_kontrp; nom++)
 {
  naim.new_plus(kontr_prih->ravno(nom));
  if((kod_skl=iceb_t_00_skl(kontr_prih->ravno(nom))) > 0)
   {
    
    sprintf(strsql,"select naik from Sklad where kod=%d",kod_skl);
    if(readkey(strsql,&row,&cur) == 1)
     naim.plus(" ",row[0]);     
   }
  else
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr_prih->ravno(nom));
    if(readkey(strsql,&row,&cur) == 1)
     naim.plus(" ",row[0]);     

   }

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(21,naim.ravno()),iceb_tu_kolbait(21,naim.ravno()),naim.ravno());
 }

for(int nom=0; nom < kolih_kontrr; nom++)
 {
  naim.new_plus(kontr_rash->ravno(nom));
  if((kod_skl=iceb_t_00_skl(kontr_rash->ravno(nom))) > 0)
   {
    
    sprintf(strsql,"select naik from Sklad where kod=%d",kod_skl);
    if(readkey(strsql,&row,&cur) == 1)
     naim.plus(" ",row[0]);     
   }
  else
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr_rash->ravno(nom));
    if(readkey(strsql,&row,&cur) == 1)
     naim.plus(" ",row[0]);     

   }

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(21,naim.ravno()),iceb_tu_kolbait(21,naim.ravno()),naim.ravno());
 }
fprintf(ff,"---------------------|\n");

fprintf(ff,gettext("\
      |ала   |                    |      |рения|          |Количество|  Сумма   |"));
/*                                                  
123456 123456 12345678901234567890 12345 1234567890 1234567890 1234567890
*/

for(int nom=0; nom < kolih_kontrp+kolih_kontrr+1; nom++)
 fprintf(ff,"%-*.*s|%-*.*s|",
 iceb_tu_kolbait(10,gettext("Количество")),
 iceb_tu_kolbait(10,gettext("Количество")),
 gettext("Количество"),
 iceb_tu_kolbait(10,gettext("Сумма")),
 iceb_tu_kolbait(10,gettext("Сумма")),
 gettext("Сумма"));

fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq->ravno());
}
