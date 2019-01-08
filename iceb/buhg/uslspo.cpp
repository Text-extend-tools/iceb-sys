/*$Id:$*/
/*21.06.2015	20.06.2015	Белых А.И.	uslspo.c
Расчёт свода услуг по оплаченным документам
 */
#include "buhg.h"
class uslspo_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str kodusl;
   class iceb_tu_str prr;
  uslspo_poi()
   {
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    kodusl.new_plus("");
    prr.new_plus("");
   }
 };

double uslgetsd(const char *datdok,const char *podr,const char *nomdok);
void uslspo_rp(class uslspo_poi *poi,FILE *ff);

int uslspo(class spis_oth *oth)
{
class iceb_tu_str naim("");
time_t          tmmn;
class uslspo_poi poi;
int kom=0;
int kodgr=0,kodusl=0;

class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода услуг по оплаченным документам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата оплаты начальная.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата оплаты конечная..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код услуги............(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.........(+/-)...."));



VV.VVOD_SPISOK_add_data(poi.datan.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.datak.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.kodusl.ravno(),255);
VV.VVOD_SPISOK_add_data(poi.prr.ravno(),2);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi.datan.new_plus(VV.VVOD_SPISOK_return_data(0));//0
poi.datak.new_plus(VV.VVOD_SPISOK_return_data(1));//1
poi.kodusl.new_plus(VV.VVOD_SPISOK_return_data(2));
poi.prr.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
   naim.new_plus("");
   if(dirusl(&kodgr,&kodusl,&naim,1,"") == 0)
    {
     poi.kodusl.z_plus(kodusl);
     VV.VVOD_SPISOK_zapis_data(2,poi.kodusl.ravno());
    }
   goto naz;   
   
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

time(&tmmn);

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,poi.datan.ravno(),&dk,&mk,&gk,poi.datak.ravno()) != 0)
 goto naz;

char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

sprintf(strsql,"select * from Opldok where ps=2 and dato >= '%04d-%02d-%02d' and dato <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной оплаты!"));
  goto naz;
 }
clear();
class masiv_din_int spkodusl;
class masiv_din_int spmetka;
class masiv_din_double spkolih;
class masiv_din_double spsuma_bez_nds;
class masiv_din_double spsuma_s_nds;

char imaf[64];
sprintf(imaf,"uslspo%d.lst",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод оплаченных документов"),dn,mn,gn,dk,mk,gk,fil.ff);

uslspo_rp(&poi,fil.ff);

class iceb_tu_str datadok("");
class iceb_tu_str dataopl("");
int kolstr1=0;
int nom_vsp=0;

fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,gettext("\
Дата опла.|Дата докум|Под.|Ном.док.| Счёт |Сумма опла.|КЭКЗ|Н. платёж.|Коментарий\n"));

fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------\n");
double itogo[2];
memset(itogo,'\0',sizeof(itogo));
double sumsnds=0.;
double suma_po_dok=0.;
int lnds=0;
double proc_nds=0.;
double suma=0.;
double koef=0.;
class SPISOK dat_nom; /*список дата-номер документа*/
class masiv_din_double sumaop; /*сумма оплаты по документу*/
class SPISOK kkez; /*список кодов класификации экономических затрат*/
class masiv_din_double suma_kkez; /*суммы по кекам*/

int nomvs=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  suma=atof(row[6]);
  
  sprintf(strsql,"%s|%s",row[1],row[3]);
  if((nomvs=dat_nom.find(strsql)) < 0)
    dat_nom.plus(strsql);
  sumaop.plus(suma,nomvs);  

  if((nomvs=kkez.find(row[7])) < 0)
   kkez.plus(row[7]);
  suma_kkez.plus(suma,nomvs);
  
  datadok.new_plus(iceb_tu_datzap(row[1]));
  dataopl.new_plus(iceb_tu_datzap(row[4]));

  
  


  printw("%s %s %2s %-*s %-*s %10.2f %4s %-*s %s\n",
  dataopl.ravno(),
  datadok.ravno(),
  row[2],
  iceb_tu_kolbait(8,row[3]),
  row[3],
  iceb_tu_kolbait(6,row[5]),
  row[5],
  suma,
  row[7],
  iceb_tu_kolbait(6,row[8]),
  row[8],
  row[9]);
  refresh();  
  
  fprintf(fil.ff,"%s %s %4s %-*s %-*s %11.2f %4s %-*s %s\n",
  dataopl.ravno(),
  datadok.ravno(),
  row[2],
  iceb_tu_kolbait(8,row[3]),
  row[3],
  iceb_tu_kolbait(6,row[5]),
  row[5],
  suma,
  row[7],
  iceb_tu_kolbait(10,row[8]),
  row[8],
  row[9]);

  itogo[0]+=atof(row[6]);

 }

fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s:%11.2f\n",
iceb_tu_kolbait(42,gettext("Итого")),
gettext("Итого"),
itogo[0]);

fprintf(fil.ff,"\
---------------------\n");
fprintf(fil.ff,gettext("\
 ККЭЗ |Сумма\n"));
fprintf(fil.ff,"\
---------------------\n");
for(int nom=0; nom < kkez.kolih();nom++)
 {
  fprintf(fil.ff,"%6s|%10.2f\n",kkez.ravno(nom),suma_kkez.ravno(nom));
 }
fprintf(fil.ff,"\
---------------------\n");






podpis(fil.ff);

fil.end();
class iceb_tu_str nomdok("");

for(int nom=0; nom < dat_nom.kolih(); nom++)
 {
  polen(dat_nom.ravno(nom),&datadok,1,'|');
  polen(dat_nom.ravno(nom),&nomdok,2,'|');
    
  suma=sumaop.ravno(nom);
  
  suma_po_dok=uslgetsd(iceb_tu_datzap(datadok.ravno()),"",nomdok.ravno());
  koef=suma_po_dok/suma;

  
  /*читаем метку ндс*/
  sprintf(strsql,"select tp,nds,sumkor,sumnds,pn from Usldokum where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(poi.prr.ravno_pr() == 1 && atoi(row[0]) != 1)
     continue;
    if(poi.prr.ravno_pr() == 2 && atoi(row[0]) != 2)
     continue;
    lnds=atoi(row1[1]);
    proc_nds=atof(row1[4]);
   }




  /*читаем содержимое документа*/
  sprintf(strsql,"select metka,kodzap,kolih,cena from Usldokum1 where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  int metka_usl=0;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(poi.kodusl.ravno(),row1[1],0,0) != 0)
     continue;
//    printw("%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    metka_usl++;
    if((nom_vsp=spkodusl.find(atoi(row1[1]))) < 0)
     {
      spkodusl.plus(atoi(row1[1]),nom_vsp);
      spmetka.plus(atoi(row1[0]),nom_vsp);
     }
    spkolih.plus(atof(row1[2]),nom_vsp);
    suma=atof(row1[2])*atof(row1[3])/koef;
    spsuma_bez_nds.plus(suma,nom_vsp);


    if(lnds == 0)
     sumsnds=suma+suma*proc_nds/100.;
    else
     sumsnds=suma;
    spsuma_s_nds.plus(sumsnds,nom_vsp);
   }  

  if(metka_usl == 0)
   continue;

 }


char imaf_sv[64];
sprintf(imaf_sv,"svod%d.lst",getpid());

if(fil.start(imaf_sv,"w") != 0)
 return(1);

double itogi_sv[3];
memset(itogi_sv,'\0',sizeof(itogi_sv));

iceb_t_zagolov(gettext("Свод оплаченных услуг"),dn,mn,gn,dk,mk,gk,fil.ff);
uslspo_rp(&poi,fil.ff);

fprintf(fil.ff,"\
-------------------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
 Код |  Наименование услуги         |Количество |Сум.без НДС|Сумма с НДС|\n"));
fprintf(fil.ff,"\
-------------------------------------------------------------------------\n");
for(int nom=0; nom < spkodusl.kolih(); nom++)
 {

  if(spmetka.ravno(nom) == 0)
    sprintf(strsql,"select naimat from Material where kodm=%d",spkodusl.ravno(nom));
  else  
    sprintf(strsql,"select naius from Uslugi where kodus=%d",spkodusl.ravno(nom));

  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

   
  fprintf(fil.ff,"%5d|%-*.*s|%11.2f|%11.2f|%11.2f|\n",
  spkodusl.ravno(nom),
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  spkolih.ravno(nom),
  spsuma_bez_nds.ravno(nom),
  spsuma_s_nds.ravno(nom));
  itogi_sv[0]+=spkolih.ravno(nom);
  itogi_sv[1]+=spsuma_bez_nds.ravno(nom);
  itogi_sv[2]+=spsuma_s_nds.ravno(nom);
  
 }
fprintf(fil.ff,"\
-------------------------------------------------------------------------\n");
fprintf(fil.ff,"%*s|%11.2f|%11.2f|%11.2f|\n",
iceb_tu_kolbait(36,gettext("Итого")),
gettext("Итого"),
itogi_sv[0],
itogi_sv[1],
itogi_sv[2]);

podpis(fil.ff);
fil.end();


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод оплаченных документов"));
oth->spis_imaf.plus(imaf_sv);
oth->spis_naim.plus(gettext("Свод услуг по оплаченным документам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

printw_vr(tmmn);

OSTANOV();
return(0);
}
/********************/
/*реквизиты поиска*/
/********************/
void uslspo_rp(class uslspo_poi *poi,FILE *ff)
{

if(poi->kodusl.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код услуги"),poi->kodusl.ravno());
if(poi->prr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Приход/расход"),poi->prr.ravno());
 
}
