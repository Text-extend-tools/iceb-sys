/*$Id: opss.c,v 5.37 2013/09/26 09:43:36 sasa Exp $*/
/*15.07.2015	20.03.2005	Белых А.И.	opss.c
Отчёт по счетам списания материаллов
*/
#include        <errno.h>
#include "buhg.h"
class opss_svt
 {
  public:
   class masiv_din_int kod_mat;
   class SPISOK sheta_spis;
   class masiv_din_double sum_svt;
  
 };

class opss_kodm_ei
 {
  public:
   class SPISOK kodm_ei;
   class SPISOK sheta_spis;
   class masiv_din_double suma;
   class masiv_din_double kolih;
   class masiv_din_double gori_kolih;  
   class masiv_din_double gori_suma;  
 };
class opss_svt_shet
 {
  public:
   class SPISOK kod_gr_shet;
   class SPISOK sheta_spis;
   class masiv_din_double sum_svt;
  
 };
 

void opss_h(int *nomlist,int *kol_str,FILE *ff);
void opss_itogo(int metka,const char *kod,double itog,FILE *ff);
void opps_shet_str(int *nomlist,int *kol_str,FILE *ff);
void opss_rrp(int *kol_str,const char *sklad_p,const char *shets_p,const char *shetu_p,const char *grupa_p,const char *kodmat_p,const char *kodop_p,const char *kontr_p,const char *artikul,FILE *ff);
void opss_rsvt(int metka_oth,short dn,short mn,short gn,short dk,short mk,short gk,class opss_svt *svt,const char *sklad_p,const char *shets_p,const char *shetu_p,const char *grupa_p,const char *kodmat_p,const char *kodop_p,const char *kontr_p,const char *artikul,char *imaf);
void opss_rsvt_shet(short dn,short mn,short gn,short dk,short mk,short gk,class opss_svt_shet *svt,const char *sklad_p,const char *shets_p,const char *shetu_p,const char *grupa_p,const char *kodmat_p,const char *kodop_p,const char *kontr_p,const char *artikul,char *imaf);
void opss_ei_kolih(short dn,short mn,short gn,short dk,short mk,short gk,class opss_kodm_ei *svt,const char *sklad_p,const char *shets_p,const char *shetu_p,const char *grupa_p,const char *kodmat_p,const char *kodop_p,const char *kontr_p,const char *artikul,char *imaf);

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/
extern int kol_strok_na_liste;

int opss(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklad_p("");
static class iceb_tu_str shets_p(""); //Счёт списания 
static class iceb_tu_str shetu_p(""); //Счёт учёта
static class iceb_tu_str grupa_p("");
static class iceb_tu_str kodmat_p("");
static class iceb_tu_str kodop_p("");
static class iceb_tu_str kontr_p("");;
static class iceb_tu_str artikul("");;
class iceb_tu_str naim("");
int kodm;
int kgr;
char strsql[512];
int kom=0;
int kom1=0;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(shets_p.ravno(),112);
VV.VVOD_SPISOK_add_data(shetu_p.ravno(),112);
VV.VVOD_SPISOK_add_data(sklad_p.ravno(),112);
VV.VVOD_SPISOK_add_data(grupa_p.ravno(),112);
VV.VVOD_SPISOK_add_data(kodmat_p.ravno(),112);
VV.VVOD_SPISOK_add_data(kodop_p.ravno(),112);
VV.VVOD_SPISOK_add_data(kontr_p.ravno(),112);
VV.VVOD_SPISOK_add_data(artikul.ravno(),112);

VV.VVOD_SPISOK_add_ZAG(gettext("Движение товарно-материальных ценностей по счетам списания"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт списания.(,,)......"));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта..(,,)........"));//3
VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)............."));//4
VV.VVOD_SPISOK_add_MD(gettext("Группа материалов.(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код материалла...(,,)..."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)...."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)...."));//8
VV.VVOD_SPISOK_add_MD(gettext("Артикул.........(,,)...."));//9



naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
shets_p.new_plus(VV.VVOD_SPISOK_return_data(2));
shetu_p.new_plus(VV.VVOD_SPISOK_return_data(3));
sklad_p.new_plus(VV.VVOD_SPISOK_return_data(4));
grupa_p.new_plus(VV.VVOD_SPISOK_return_data(5));
kodmat_p.new_plus(VV.VVOD_SPISOK_return_data(6));
kodop_p.new_plus(VV.VVOD_SPISOK_return_data(7));
kontr_p.new_plus(VV.VVOD_SPISOK_return_data(8));
artikul.new_plus(VV.VVOD_SPISOK_return_data(9));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_19.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операцій расходов"));
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
         VV.data_z_plus(4,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kontr_p,&naim);
        VV.VVOD_SPISOK_zapis_data(8,kontr_p.ravno());
        goto naz;

      case 2 :
        vibrek("Grup",&grupa_p,&naim);
        VV.VVOD_SPISOK_zapis_data(5,grupa_p.ravno());
        goto naz;

      case 3 :
        if(dirrashod(1,&kodop_p,&naim) == 0)
          VV.data_z_plus(7,kodop_p.ravno());
        goto naz;
        
      case 4 :
        strsql[0]='\0';
        if(dirmatr(&kgr,&kodm,&naims,1,0,strsql,0,0) == 0)
         {
          VV.data_z_plus(6,kodm);
         }
        
        goto naz;

      case 5 :
        vibrek("Plansh",&naims,&naim);
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }
short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

sprintf(strsql,"select datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,shet from Dokummat1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and \
nomkar <> 0 and tipz=2 order by shet asc",
gn,mn,dn,gk,mk,dk);

int kolstr;
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"opssw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shsp char(24) not null,\
kod_grup int not null,\
shuh char(24) not null,\
datd DATE not null,\
sklad int not null,\
nomd char(24) not null,\
kodm int not null,\
kolih double(16,6) not null,\
cena double(16,6) not null,\
ei char(24) not null,\
kodop char(24) not null,\
kontr char(24) not null,\
naim char(112) not null) ENGINE = MYISAM",imatmptab);

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

class iceb_tu_str kodop("");
class iceb_tu_str kontr("");
class iceb_tu_str shet_uheta("");
class iceb_tu_str shet_sp("");
double kolih;
short dd,md,gd;
class iceb_tu_str kod_grup("");
int kolstr1=0;
class opss_svt svt; //по кодам материалов
class opss_svt svtg; //по группам материалов
class opss_kodm_ei svt_ei; /*по кодам материалов-единиц измерения */

class opss_svt_shet svtg_shets; //по группам материалов и счетам учёта



while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  

  if(proverka(shets_p.ravno(),row[8],1,0) != 0)
    continue;  
  if(proverka(sklad_p.ravno(),row[1],0,0) != 0)
    continue;  

  if(proverka(kodmat_p.ravno(),row[4],0,0) != 0)
    continue;  

  //читаем материал
  naim.new_plus("");
  kod_grup.new_plus("");
  sprintf(strsql,"select kodgr,naimat,artikul from Material where kodm=%s",row[4]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[1]);
    if(proverka(grupa_p.ravno(),row1[0],0,0) != 0)
      continue;  
    if(proverka(artikul.ravno(),row1[2],0,0) != 0)
      continue;  
    kod_grup.new_plus(row1[0]);
   }
  kodop.new_plus("");
  kontr.new_plus("");
  
  //читаем шапку документа
  sprintf(strsql,"select kontr,kodop from Dokummat where datd='%s' and sklad=%s and \
nomd='%s'",row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(kodop_p.ravno(),row1[1],0,0) != 0)
      continue;  
    if(proverka(kontr_p.ravno(),row1[0],0,0) != 0)
      continue;  
    kodop.new_plus(row1[1]);
    kontr.new_plus(row1[0]);
   }

  rsdat(&dd,&md,&gd,row[0],2);
  kolih=readkolk(atoi(row[1]),atoi(row[3]),dd,md,gd,row[2]);  
  if(kolih == 0.)
   continue;
   
  shet_uheta.new_plus("");

  sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%s",row[1],row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
     if(proverka(shetu_p.ravno(),row1[0],1,0) != 0)
      continue;  
    shet_uheta.new_plus(row1[0]);    
   }
  else
   {
    iceb_t_soob(gettext("Не найденa карточка !"));
   }

  shet_sp.new_plus(row[8]);
  if(shet_sp.ravno()[0] == '\0')
   shet_sp.new_plus("???");

  sprintf(strsql,"insert into %s values ('%s',%s,'%s','%s',%s,'%s',%s,%.10g,%s,'%s','%s','%s','%s')",
  imatmptab,
  shet_sp.ravno(),kod_grup.ravno(),shet_uheta.ravno(),
  row[0],row[1],row[2],row[4],kolih,row[6],row[7],
  kodop.ravno(),kontr.ravno(),naim.ravno_filtr());

  sql_zapis(strsql,1,0);    

  if(svt.kod_mat.find(row[4]) < 0)
   svt.kod_mat.plus(row[4]);
  if(svt.sheta_spis.find(shet_sp.ravno()) < 0)
   svt.sheta_spis.plus(shet_sp.ravno());

  if(svtg.kod_mat.find(kod_grup.ravno_atoi()) < 0)
   svtg.kod_mat.plus(kod_grup.ravno_atoi());
  if(svtg.sheta_spis.find(shet_sp.ravno()) < 0)
   svtg.sheta_spis.plus(shet_sp.ravno());
   
  sprintf(strsql,"%s|%s",kod_grup.ravno(),shet_uheta.ravno());
  if(svtg_shets.kod_gr_shet.find(strsql) < 0)
   svtg_shets.kod_gr_shet.plus(strsql);
  if(svtg_shets.sheta_spis.find(shet_sp.ravno()) < 0)
   svtg_shets.sheta_spis.plus(shet_sp.ravno());

  sprintf(strsql,"%s|%s",row[4],row[7]);
  if(svt_ei.kodm_ei.find(strsql) < 0)
   svt_ei.kodm_ei.plus(strsql);   
  if(svt_ei.sheta_spis.find(shet_sp.ravno()) < 0)
   svt_ei.sheta_spis.plus(shet_sp.ravno());   
  
 }
sprintf(strsql,"select * from %s order by shsp,kod_grup,shuh,datd asc",imatmptab);
int kolstrtmp=0;
if((kolstrtmp=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
char imaf[64];
FILE *ff;

sprintf(imaf,"opss%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
int kol_str=0;

iceb_t_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),dn,mn,gn,dk,mk,gk,ff);
kol_str=5;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,sklad_p.ravno(),shets_p.ravno(),shetu_p.ravno(),grupa_p.ravno(),kodmat_p.ravno(),kodop_p.ravno(),kontr_p.ravno(),artikul.ravno(),ff);

int nomlist=0;

//Шапка 

opss_h(&nomlist,&kol_str,ff);


double suma;
double cena;
class iceb_tu_str shet_sp_z("");
class iceb_tu_str shet_uheta_z("");
double itogo_shetu=0.;
double itogo_shets=0.;
double itogo_kod_grup=0.;
class iceb_tu_str sklad("");
class iceb_tu_str nomd("");
class iceb_tu_str kodmat("");
char ei[64];
class iceb_tu_str kod_grup_z("");

svt.sum_svt.make_class(svt.kod_mat.kolih()*svt.sheta_spis.kolih());
svtg.sum_svt.make_class(svtg.kod_mat.kolih()*svtg.sheta_spis.kolih());
svtg_shets.sum_svt.make_class(svtg_shets.kod_gr_shet.kolih()*svtg_shets.sheta_spis.kolih());

svt_ei.kolih.make_class(svt_ei.kodm_ei.kolih()*svt_ei.sheta_spis.kolih());
svt_ei.suma.make_class(svt_ei.kodm_ei.kolih()*svt_ei.sheta_spis.kolih());
svt_ei.gori_kolih.make_class(svt_ei.sheta_spis.kolih());
svt_ei.gori_suma.make_class(svt_ei.sheta_spis.kolih());
int nomer_kod_mat=0;
int nomer_sheta_spis=0;
while(cur.read_cursor(&row) != 0)
 {
//  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    

  shet_sp.new_plus(row[0]);
  kod_grup.new_plus(row[1]);
  shet_uheta.new_plus(row[2]);
  rsdat(&dd,&md,&gd,row[3],2);
  sklad.new_plus(row[4]);
  nomd.new_plus(row[5]);
  kodmat.new_plus(row[6]);
  kolih=atof(row[7]);
  cena=atof(row[8]);
  strncpy(ei,row[9],sizeof(ei)-1);
  kodop.new_plus(row[10]);
  kontr.new_plus(row[11]);
  naim.new_plus(row[12]);

  if(SRAV(kod_grup_z.ravno(),kod_grup.ravno(),0) != 0)
   {
    if(kod_grup_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(3,kod_grup_z.ravno(),itogo_kod_grup,ff);
      itogo_kod_grup=0.;

     }    
    kod_grup_z.new_plus(kod_grup.ravno());
   }

  if(SRAV(shet_uheta_z.ravno(),shet_uheta.ravno(),0) != 0)
   {
    if(shet_uheta_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    shet_uheta_z.new_plus(shet_uheta.ravno());
   }

  if(SRAV(shet_sp_z.ravno(),shet_sp.ravno(),0) != 0)
   {
    if(shet_sp_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff);
      itogo_shetu=0.;

      
      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(3,kod_grup_z.ravno(),itogo_kod_grup,ff);
      itogo_kod_grup=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff);
      itogo_shets=0.;

      
      
      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    shet_sp_z.new_plus(shet_sp.ravno());
   }

  
  cena=okrug(cena,okrcn);
  
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  itogo_shetu+=suma;
  itogo_shets+=suma;  
  itogo_kod_grup+=suma;
  
  opps_shet_str(&nomlist,&kol_str,ff);

  fprintf(ff,"%-8s %-*.*s %-*s %-*s %10.2f %-6s %8.8g %10.10g %02d.%02d.%d %-*s %-5s %-*s %-*s\n",
  kodmat.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(10,shet_sp.ravno()),shet_sp.ravno(),
  iceb_tu_kolbait(10,shet_uheta.ravno()),shet_uheta.ravno(),
  suma,ei,kolih,cena,dd,md,gd,
  iceb_tu_kolbait(9,nomd.ravno()),nomd.ravno(),
  sklad.ravno(),
  iceb_tu_kolbait(6,kodop.ravno()),kodop.ravno(),
  iceb_tu_kolbait(7,kontr.ravno()),kontr.ravno());

  nomer_kod_mat=svt.kod_mat.find(kodmat.ravno_atoi());
  nomer_sheta_spis=svt.sheta_spis.find(shet_sp.ravno());
  svt.sum_svt.plus(suma,nomer_kod_mat*svt.sheta_spis.kolih()+nomer_sheta_spis);

  nomer_kod_mat=svtg.kod_mat.find(kod_grup.ravno_atoi());
  nomer_sheta_spis=svtg.sheta_spis.find(shet_sp.ravno());
  svtg.sum_svt.plus(suma,nomer_kod_mat*svtg.sheta_spis.kolih()+nomer_sheta_spis);
  
  sprintf(strsql,"%s|%s",kod_grup.ravno(),shet_uheta.ravno());
  nomer_kod_mat=svtg_shets.kod_gr_shet.find(strsql);
  nomer_sheta_spis=svtg_shets.sheta_spis.find(shet_sp.ravno());
  svtg_shets.sum_svt.plus(suma,nomer_kod_mat*svtg_shets.sheta_spis.kolih()+nomer_sheta_spis);

  sprintf(strsql,"%s|%s",kodmat.ravno(),ei);
  nomer_kod_mat=svt_ei.kodm_ei.find(strsql);
  nomer_sheta_spis=svt_ei.sheta_spis.find(shet_sp.ravno());
  svt_ei.suma.plus(suma,nomer_kod_mat*svt_ei.sheta_spis.kolih()+nomer_sheta_spis);
  svt_ei.kolih.plus(kolih,nomer_kod_mat*svt_ei.sheta_spis.kolih()+nomer_sheta_spis);
  svt_ei.gori_kolih.plus(kolih,nomer_sheta_spis);    
  svt_ei.gori_suma.plus(suma,nomer_sheta_spis);    
 }

GDITE();

opss_itogo(3,kod_grup_z.ravno(),itogo_kod_grup,ff);
opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff);
opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff);

podpis(ff);

fclose(ff);

char imaf_svt[64];
opss_rsvt(0,dn,mn,gn,dk,mk,gk,&svt,sklad_p.ravno(),shets_p.ravno(),shetu_p.ravno(),grupa_p.ravno(),kodmat_p.ravno(),kodop_p.ravno(),kontr_p.ravno(),artikul.ravno(),imaf_svt);


char imaf_svtg[64];
opss_rsvt(1,dn,mn,gn,dk,mk,gk,&svtg,sklad_p.ravno(),shets_p.ravno(),shetu_p.ravno(),grupa_p.ravno(),kodmat_p.ravno(),kodop_p.ravno(),kontr_p.ravno(),artikul.ravno(),imaf_svtg);

char imaf_svtg_shet[64];
opss_rsvt_shet(dn,mn,gn,dk,mk,gk,&svtg_shets,sklad_p.ravno(),shets_p.ravno(),shetu_p.ravno(),grupa_p.ravno(),kodmat_p.ravno(),kodop_p.ravno(),kontr_p.ravno(),artikul.ravno(),imaf_svtg_shet);

char imaf_ei[64];
opss_ei_kolih(dn,mn,gn,dk,mk,gk,&svt_ei,sklad_p.ravno(),shets_p.ravno(),shetu_p.ravno(),grupa_p.ravno(),kodmat_p.ravno(),kodop_p.ravno(),kontr_p.ravno(),artikul.ravno(),imaf_ei);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение товарно-материальных ценностей по счетам списания"));
oth->spis_imaf.plus(imaf_svt);
oth->spis_naim.plus(gettext("Общий итог"));

oth->spis_imaf.plus(imaf_svtg);
oth->spis_naim.plus(gettext("Общий итог по группам материалла"));
oth->spis_imaf.plus(imaf_svtg_shet);
oth->spis_naim.plus(gettext("Общий итог по счетам учёта-счетам списания"));
oth->spis_imaf.plus(imaf_ei);
oth->spis_naim.plus(gettext("Общий итог по счетам с количеством"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
 
}

/********************************/
/*шапка*/
/**********************************/

void opss_h(int *nomlist,int *kol_str,FILE *ff)
{
*nomlist+=1;
fprintf(ff,"%110s%s N%d\n","",gettext("Лист"),*nomlist);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код   |    Наименование    |  Счёт    |   Счёт   |   Сумма  |Един. |Количec-|   Цена   |   Дата   |Номер    |Склад|Опера-|Контра-|\n"));
fprintf(ff,gettext("\
материа.|     материалла     |списания  |  учёта   |          |измер.|  тво   |          | документа|документа|     |ция   |гент   |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*kol_str+=5;


}

/*****************************/
/*Выдача итога*/
/*****************************/

void opss_itogo(int metka, //1-счёт учёта 2-счёт списания 3-по коду группы
const char *kod,double itog,FILE *ff)
{
if(itog == 0.)
 return;
 
char bros[1024];
if(metka == 1)
  sprintf(bros,"%s %s",gettext("Итого по счёту учёта"),kod);
if(metka == 2)
  sprintf(bros,"%s %s",gettext("Итого по счёту списания"),kod);

if(metka == 3) //По коду группы
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];
  class iceb_tu_str naim("");
  sprintf(strsql,"select naik from Grup where kod=%s",kod);
  if(readkey(strsql,&row,&cur) == 1)
    naim.new_plus(row[0]);
  else
    naim.new_plus("");
  sprintf(bros,"%s %s %s",gettext("Итого по группе"),kod,naim.ravno());

 }

fprintf(ff,"%*.*s:%10.2f\n",iceb_tu_kolbait(51,bros),iceb_tu_kolbait(51,bros),bros,itog);

}
/************************/
/*Счётчик строк*/
/*************************/

void opps_shet_str(int *nomlist,int *kol_str,FILE *ff)
{
*kol_str+=1;
if(*kol_str <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kol_str=0;
opss_h(nomlist,kol_str,ff);
*kol_str+=1;
}
/********************************/
/*Распечатка сводной таблицы*/
/******************************/
void opss_rsvt(int metka_oth, //0-по кодам материалов 1-по кодам групп материалов
short dn,short mn,short gn,
short dk,short mk,short gk,
class opss_svt *svt,
const char *sklad_p,
const char *shets_p,
const char *shetu_p,
const char *grupa_p,
const char *kodmat_p,
const char *kodop_p,
const char *kontr_p,
const char *artikul,
char *imaf)
{
FILE *ff;
if(metka_oth == 0)
  sprintf(imaf,"opss_sv%d.lst",getpid());
if(metka_oth == 1)
 sprintf(imaf,"opss_svg%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),dn,mn,gn,dk,mk,gk,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,sklad_p,shets_p,shetu_p,grupa_p,kodmat_p,kodop_p,kontr_p,artikul,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_kod_mat=svt->kod_mat.kolih();

class iceb_tu_str liniq;
liniq.plus("-------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("-----------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

if(metka_oth == 0)
 fprintf(ff,"%-*s|%-*.*s|",
 iceb_tu_kolbait(5,gettext("Код")),gettext("Код"),
 iceb_tu_kolbait(30,gettext("Наименование материалла")),
 iceb_tu_kolbait(30,gettext("Наименование материалла")),
 gettext("Наименование материалла"));

if(metka_oth == 1)
 fprintf(ff,"%-*s|%-*.*s|",
 iceb_tu_kolbait(5,gettext("Код")),gettext("Код"),
 iceb_tu_kolbait(30,gettext("Наименование группы материалла")),
 iceb_tu_kolbait(30,gettext("Наименование группы материалла")),
 gettext("Наименование группы материалла"));

for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_tu_kolbait(10,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

class iceb_tu_str naim_kod_mat("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor=0.;

for(int ii=0; ii < kolih_kod_mat; ii++)
 {
  if(metka_oth == 0)  
   sprintf(strsql,"select naimat from Material where kodm=%d",svt->kod_mat.ravno(ii));

  if(metka_oth == 1)  
   sprintf(strsql,"select naik from Grup where kod=%d",svt->kod_mat.ravno(ii));
  
  if(readkey(strsql,&row,&cur) == 1)
   naim_kod_mat.new_plus(row[0]);  
  else
   naim_kod_mat.new_plus("");  
  fprintf(ff,"%5d %-*.*s",svt->kod_mat.ravno(ii),
  iceb_tu_kolbait(30,naim_kod_mat.ravno()),iceb_tu_kolbait(30,naim_kod_mat.ravno()),naim_kod_mat.ravno());
  itogo_gor=0.;
    
  for(int kk=0; kk < kolih_shet; kk++)
   {
    fprintf(ff," %10.2f",svt->sum_svt.ravno(ii*kolih_shet+kk)); 
    itogo_gor+=svt->sum_svt.ravno(ii*kolih_shet+kk);
   }
  fprintf(ff," %10.2f\n",itogo_gor);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"));
itogo_gor=0;
double itogo_kol=0;
for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_kol=0.;  
  //считаем колонку по вертикали
  for(int kk=0; kk < kolih_kod_mat; kk++)
    itogo_kol+=svt->sum_svt.ravno(kk*kolih_shet+ii);    
  fprintf(ff," %10.2f",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff," %10.2f\n",itogo_gor);


podpis(ff);
fclose(ff);






}
/***********************/
/*Распечатка реквизитов поиска*/
/*****************************/
void opss_rrp(int *kol_str,
const char *sklad_p,
const char *shets_p,
const char *shetu_p,
const char *grupa_p,
const char *kodmat_p,
const char *kodop_p,
const char *kontr_p,
const char *artikul,
FILE *ff)
{

if(sklad_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklad_p);
  *kol_str+=1;
 }
if(shets_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт списания"),shets_p);
  *kol_str+=1;
 }
if(shetu_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetu_p);
  *kol_str+=1;
 }
if(grupa_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),grupa_p);
  *kol_str+=1;
 }
if(kodmat_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat_p);
  *kol_str+=1;
 }
if(kodop_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop_p);
  *kol_str+=1;
 }
if(kontr_p[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),kontr_p);
  *kol_str+=1;
 }
if(artikul[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Артикул"),kontr_p);
  *kol_str+=1;
 }
}

/********************************/
/*Распечатка сводной таблицы по группам и счетам учёта*/
/******************************/
void opss_rsvt_shet(short dn,short mn,short gn,
short dk,short mk,short gk,
class opss_svt_shet *svt,
const char *sklad_p,
const char *shets_p,
const char *shetu_p,
const char *grupa_p,
const char *kodmat_p,
const char *kodop_p,
const char *kontr_p,
const char *artikul,
char *imaf)
{
FILE *ff;
sprintf(imaf,"opss_svg_sh%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),dn,mn,gn,dk,mk,gk,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,sklad_p,shets_p,shetu_p,grupa_p,kodmat_p,kodop_p,kontr_p,artikul,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_kod_mat=svt->kod_gr_shet.kolih();

class iceb_tu_str liniq;
liniq.plus("----------------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("-----------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*s|%-*.*s|%-*.*s|",
iceb_tu_kolbait(5,gettext("Код")),gettext("Код"),
iceb_tu_kolbait(30,gettext("Наименование группы материалла")),
iceb_tu_kolbait(30,gettext("Наименование группы материалла")),
gettext("Наименование группы материалла"),
iceb_tu_kolbait(8,gettext("Счёт")),
iceb_tu_kolbait(8,gettext("Счёт")),
gettext("Счёт"));

for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_tu_kolbait(10,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

class iceb_tu_str naim_kod_mat("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor=0.;
char kod_gr[64];
char shet_uheta[64];

for(int ii=0; ii < kolih_kod_mat; ii++)
 {
  polen(svt->kod_gr_shet.ravno(ii),kod_gr,sizeof(kod_gr),1,'|');
  polen(svt->kod_gr_shet.ravno(ii),shet_uheta,sizeof(shet_uheta),2,'|');
    

  sprintf(strsql,"select naik from Grup where kod=%s",kod_gr);
  
  if(readkey(strsql,&row,&cur) == 1)
   naim_kod_mat.new_plus(row[0]);  
  else
   naim_kod_mat.new_plus("");  
  fprintf(ff,"%5s %-*.*s %-*.*s",
  kod_gr,
  iceb_tu_kolbait(30,naim_kod_mat.ravno()),iceb_tu_kolbait(30,naim_kod_mat.ravno()),naim_kod_mat.ravno(),
  iceb_tu_kolbait(8,shet_uheta),iceb_tu_kolbait(8,shet_uheta),shet_uheta);
  
  itogo_gor=0.;
    
  for(int kk=0; kk < kolih_shet; kk++)
   {
    fprintf(ff," %10.2f",svt->sum_svt.ravno(ii*kolih_shet+kk)); 
    itogo_gor+=svt->sum_svt.ravno(ii*kolih_shet+kk);
   }
  fprintf(ff," %10.2f\n",itogo_gor);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s",iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"));
itogo_gor=0;
double itogo_kol=0;
for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_kol=0.;  
  //считаем колонку по вертикали
  for(int kk=0; kk < kolih_kod_mat; kk++)
    itogo_kol+=svt->sum_svt.ravno(kk*kolih_shet+ii);    
  fprintf(ff," %10.2f",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff," %10.2f\n",itogo_gor);


podpis(ff);
fclose(ff);

}
/*********************************/
/*Распечатка сводной таблицы по кодам материалов-единицам измерения с количеством и суммами*/
/**********************************/
void opss_ei_kolih(short dn,short mn,short gn,
short dk,short mk,short gk,
class opss_kodm_ei *svt,
const char *sklad_p,
const char *shets_p,
const char *shetu_p,
const char *grupa_p,
const char *kodmat_p,
const char *kodop_p,
const char *kontr_p,
const char *artikul,
char *imaf)
{
FILE *ff;
sprintf(imaf,"opss_eik%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),dn,mn,gn,dk,mk,gk,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,sklad_p,shets_p,shetu_p,grupa_p,kodmat_p,kodop_p,kontr_p,artikul,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_km_ei=svt->kodm_ei.kolih();

class iceb_tu_str liniq;
liniq.plus("--------------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("----------------------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*s|%-*.*s|%-*.*s|",
iceb_tu_kolbait(5,gettext("Код")),gettext("Код"),
iceb_tu_kolbait(30,gettext("Наименование материалла")),
iceb_tu_kolbait(30,gettext("Наименование материалла")),
gettext("Наименование материалла"),
iceb_tu_kolbait(6,gettext("Единица")),
iceb_tu_kolbait(6,gettext("Единица")),
gettext("Единица"));


for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_tu_kolbait(21,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%-*s|\n",iceb_tu_kolbait(21,gettext("Итого")),gettext("Итого"));



fprintf(ff,"%5s|%30s|%*.*s|","","",
iceb_tu_kolbait(6,gettext("измерения")),
iceb_tu_kolbait(6,gettext("измерения")),
gettext("измерения"));

for(int ii=0; ii < kolih_shet+1; ii++)
 fprintf(ff,"%-*.*s|%-*.*s|",
 iceb_tu_kolbait(10,gettext("Количество")),
 iceb_tu_kolbait(10,gettext("Количество")),
 gettext("Количество"),
 iceb_tu_kolbait(10,gettext("Сумма")),
 iceb_tu_kolbait(10,gettext("Сумма")),
 gettext("Сумма"));

fprintf(ff,"\n");


fprintf(ff,"%s\n",liniq.ravno());
class iceb_tu_str ei("");
int kodmat=0;
class iceb_tu_str naim_kod_mat("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor_kolih=0.;
double itogo_gor_suma=0.;
double kolih=0.;
double suma=0.;
for(int nom_km_ei=0; nom_km_ei < kolih_km_ei; nom_km_ei++)
 {
  polen(svt->kodm_ei.ravno(nom_km_ei),&kodmat,1,'|');
  polen(svt->kodm_ei.ravno(nom_km_ei),&ei,2,'|');
    

  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
  
  if(readkey(strsql,&row,&cur) == 1)
   naim_kod_mat.new_plus(row[0]);  
  else
   naim_kod_mat.new_plus("");  
  fprintf(ff,"%5d|%-*.*s|%-*.*s|",
  kodmat,
  iceb_tu_kolbait(30,naim_kod_mat.ravno()),iceb_tu_kolbait(30,naim_kod_mat.ravno()),naim_kod_mat.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),iceb_tu_kolbait(6,ei.ravno()),ei.ravno());
  
  itogo_gor_kolih=itogo_gor_suma=0.;
    
  for(int nom_shet=0; nom_shet < kolih_shet; nom_shet++)
   {
    suma=svt->suma.ravno(nom_km_ei*kolih_shet+nom_shet);
    kolih=svt->kolih.ravno(nom_km_ei*kolih_shet+nom_shet);
    if(kolih != 0. || suma != 0.)
     {    
      fprintf(ff,"%10.2f|%10.2f|",kolih,suma);
      itogo_gor_kolih+=kolih;
      itogo_gor_suma+=suma;
     }
    else
     fprintf(ff,"%10s|%10s|","","");
   }
  fprintf(ff,"%10.2f|%10.2f|\n",itogo_gor_kolih,itogo_gor_suma);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_tu_kolbait(43,gettext("Итого")),gettext("Итого"));

for(int nom_shet=0; nom_shet < kolih_shet; nom_shet++)
 fprintf(ff,"%10.2f|%10.2f|",svt->gori_kolih.ravno(nom_shet),svt->gori_suma.ravno(nom_shet));
 
fprintf(ff,"%10.2f|%10.2f|\n",svt->gori_kolih.suma(),svt->gori_suma.suma());

podpis(ff);
fclose(ff);

}
