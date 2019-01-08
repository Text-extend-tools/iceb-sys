/*$Id: rnn_rkontr.c,v 5.3 2013/05/17 14:56:10 sasa Exp $*/
/*15.07.2015	24.10.2011	Белых А.И.	rnn_rkontr.c
Распечтака всех записей по контрагенту
*/
#include <errno.h>
#include "buhg.h"


class rnn_rkontr_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str inn;
   class iceb_tu_str pri_ras;
  rnn_rkontr_poi()
   {
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    inn.new_plus("");
    pri_ras.new_plus("");
   }   
 };

int rnn_rkontr_plus(class rnn_rkontr_poi *poi,FILE *ff,FILE *ff_ro);
int rnn_rkontr_minus(class rnn_rkontr_poi *poi,FILE *ff);
void rnn_rkontr_rsd_ro(FILE *ff);



int rnn_rkontr(class spis_oth *oth)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

static class rnn_rkontr_poi poi;
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка записей по контрагенту"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.............(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный налоговый номер.."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход..............+/-.."));

VV.VVOD_SPISOK_add_data(poi.datan.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.datak.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.inn.ravno(),32);
VV.VVOD_SPISOK_add_data(poi.pri_ras.ravno(),2);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("контрагенты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi.datan.new_plus(VV.data_ravno(0));
poi.datak.new_plus(VV.data_ravno(1));
poi.inn.new_plus(VV.data_ravno(2));
poi.pri_ras.new_plus(VV.data_ravno(3));

class iceb_tu_str kod("");
class iceb_tu_str naim("");
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
     if(vibrek("Kontragent",&kod,&naim) == 0)
      {
       sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno());
       if(readkey(strsql,&row,&cur) == 1)
        {
         VV.data_plus(2,row[0]);
        }
      }
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,poi.datan.ravno(),&dk,&mk,&gk,poi.datak.ravno()) != 0)
 goto naz;

if(poi.inn.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён индивидуальный налоговый номер!"));
  goto naz;
  
 }

sprintf(strsql,"select kodkon,naikon,kod from Kontragent where innn='%s'",poi.inn.ravno());
if(readkey(strsql,&row,&cur) < 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден контрагент с ИНН"),poi.inn.ravno());
  iceb_t_soob(strsql);
  goto naz;
 } 


FILE *ff=NULL;
char imaf[64];
sprintf(imaf,"pkontr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

FILE *ff_ro=NULL;
char imaf_ro[64];
sprintf(imaf_ro,"pkontrr%d.lst",getpid());


zagolov(gettext("Рапечатка записей по контрагенту"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff);
fprintf(ff,"\n%s:%s\n",gettext("Индивидуальный налоговый номер"),poi.inn.ravno());
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[0],row[1]);
fprintf(ff,"%s:%s\n",gettext("Код"),row[2]);

if(poi.pri_ras.getdlinna() <= 1 || poi.pri_ras.ravno_pr() == 1)
 {
  if((ff_ro = fopen(imaf_ro,"w")) == NULL)
   {
    error_op_nfil(imaf_ro,errno,"");
    return(1);
   }
  zagolov(gettext("Рапечатка записей по контрагенту"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff_ro);
  fprintf(ff_ro,"\n%s:%s\n",gettext("Индивидуальный налоговый номер"),poi.inn.ravno());
  fprintf(ff_ro,"%s:%s %s\n",gettext("Контрагент"),row[0],row[1]);
  fprintf(ff_ro,"%s:%s\n",gettext("Код"),row[2]);
  rnn_rkontr_plus(&poi,ff,ff_ro);

  podpis(ff_ro);
  fclose(ff_ro);
 }
if(poi.pri_ras.getdlinna() <= 1 || poi.pri_ras.ravno_pr() == 2)
  rnn_rkontr_minus(&poi,ff);


podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка записей по контрагенту"));
if(ff_ro != NULL)
 {
  oth->spis_imaf.plus(imaf_ro);
  oth->spis_naim.plus(gettext("Распечатка записей по контрагенту (развёрнутый отчёт)"));
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);

}
/**********************************/
/*распечатка шапки документа*/
/*******************************/
void rnn_rkontr_rsd(FILE *ff)
{

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
       Реквизиты документа                   |         Реквизиты налоговой накладной       |\n\
  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |Сумма по док|Сумма НДС |\n"));

/***********
1234567890|1234567890|123456789012|1234567890|
*************/
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
}
/**********************************/
/*распечатка шапки документа развёрнутого отчёта для приходных документов*/
/*******************************/
void rnn_rkontr_rsd_ro(FILE *ff)
{

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
       Реквизиты документа                   |         Реквизиты налоговой накладной       |Внутренние документы |\n\
  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |\n"));

/***********
1234567890|1234567890|123456789012|1234567890|
*************/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
}


/*****************************************/
/*приходные документы*/
/**********************************/
int rnn_rkontr_plus(class rnn_rkontr_poi *poi,FILE *ff,FILE *ff_ro)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,poi->datan.ravno(),&dk,&mk,&gk,poi->datak.ravno()) != 0)
 return(1);

sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,nkontr,mi,datdi,nomdi,dvd \
from Reenn1 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and inn='%s'",gn,mn,dn,gk,mk,dk,poi->inn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного полученного документа!"));
  return(1);
 }

fprintf(ff,"\n%s\n",gettext("Полученные документы"));

rnn_rkontr_rsd(ff);
rnn_rkontr_rsd_ro(ff_ro);

int metka_ps=0;
int kolstr1=0;
double sumadok=0.;
double isumadok[2];
double sumands=0.;
double isumands[2];
class iceb_tu_str nd_pos("");

memset(isumadok,'\0',sizeof(isumadok));
memset(isumands,'\0',sizeof(isumands));

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  metka_ps=atoi(row[11]);

  sumadok=atof(row[2])+atof(row[4])+atof(row[6])+atof(row[8]);
  sumands=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);

  isumadok[1]+=sumadok;
  isumands[1]+=sumands;

  if(metka_ps == 0 || metka_ps == 5) /*запись сделана в ручную или из главной книги*/
   {
    fprintf(ff,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|\n",
    "","","","",
    iceb_tu_kolbait(10,row[1]),
    row[1],
    iceb_tu_datzap(row[0]),sumadok,sumands);

    class iceb_tu_str datdok(iceb_tu_datzap(row[12]));

    fprintf(ff_ro,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|%-10s|%10s|\n",
    "","","","",
    iceb_tu_kolbait(10,row[1]),
    row[1],
    iceb_tu_datzap(row[0]),sumadok,sumands,row[13],datdok.ravno());
    continue;
   }

  nd_pos.new_plus("");
  if(metka_ps == 1) /*материальный учёт*/
   {
    sprintf(strsql,"select sklad,nomon from Dokummat where datd='%s' and nomd='%s' and tip=1",row[12],row[13]);    
    if(readkey(strsql,&row1,&cur1) == 1)
     {
    
      nd_pos.new_plus(row1[1]);
/*************
      rsdat(&d,&m,&g,row[12],2);

      sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and nomd='%s' and nomerz=14",g,row1[0],row[13]);
      if(readkey(strsql,&row1,&cur1) == 1)
       datavnn.new_plus(row1[0]);
***************/             
     }
   }

  if(metka_ps == 2) /*учёт услуг*/
   {
    sprintf(strsql,"select nomdp,datdp from Usldokum where datd='%s' and nomd='%s' and tp=1",row[12],row[13]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      nd_pos.new_plus(row1[0]);
      
     }
   }

  if(metka_ps == 3) /*учёт основных средств*/
   {
    sprintf(strsql,"select nomdv from Uosdok where datd='%s' and nomd='%s' and tipz=1",row[12],row[13]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      nd_pos.new_plus(row1[0]);
     }
   }
  

  if(metka_ps == 4) /*учёт командировочных расходов*/
   {
/**********
    sprintf(strsql,"select dvnn from Ukrdok where datd='%s' and nomd='%s' and ndrnn='%s'",row[12],row[13],row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
     datavnn.new_plus(row1[0]);
**************/
    nd_pos.new_plus(row[1]);
    
    
   }

  if(metka_ps >0 && metka_ps < 5) /*запись импортирована*/
   {
    isumadok[0]+=sumadok;
    isumands[0]+=sumands;

    class iceb_tu_str datnak(iceb_tu_datzap(row[0]));    
    class iceb_tu_str datnn(iceb_tu_datzap(row[14]));

    fprintf(ff,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|\n",
    iceb_tu_kolbait(10,nd_pos.ravno()),
    nd_pos.ravno(),
    datnak.ravno(),
    sumadok,sumands,
    iceb_tu_kolbait(10,row[1]),
    row[1],datnn.ravno(),sumadok,sumands);

    fprintf(ff_ro,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|%-10s|%10s|\n",
    iceb_tu_kolbait(10,nd_pos.ravno()),
    nd_pos.ravno(),
    datnak.ravno(),
    sumadok,sumands,
    iceb_tu_kolbait(10,row[1]),
    row[1],datnn.ravno(),sumadok,sumands,row[13],iceb_tu_datzap(row[12]));
   }
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff_ro,"\
------------------------------------------------------------------------------------------------------------------\n");
 
fprintf(ff,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_tu_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);

fprintf(ff_ro,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_tu_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);




return(0);
}
/*****************************************/
/*расходные документы*/
/**********************************/
int rnn_rkontr_minus(class rnn_rkontr_poi *poi,FILE *ff)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,poi->datan.ravno(),&dk,&mk,&gk,poi->datak.ravno()) != 0)
 return(1);

sprintf(strsql,"select datd,nnn,sumd,snds,nkontr,mi,datdi,nomdi \
from Reenn where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and inn='%s'",gn,mn,dn,gk,mk,dk,poi->inn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного выданного документа !"));
  return(1);
 }

fprintf(ff,"\n%s\n",gettext("Выданные документы"));

rnn_rkontr_rsd(ff);

int metka_ps=0;
int kolstr1=0;
double sumadok=0.;
double isumadok[2];
double sumands=0.;
double isumands[2];
class iceb_tu_str nd_pos("");

memset(isumadok,'\0',sizeof(isumadok));
memset(isumands,'\0',sizeof(isumands));

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  metka_ps=atoi(row[5]);

  sumadok=atof(row[2]);
  sumands=atof(row[3]);

  isumadok[1]+=sumadok;
  isumands[1]+=sumands;

  if(metka_ps == 0 || metka_ps == 5) /*запись сделана в ручную или импортирована из главной книги*/
   {
    fprintf(ff,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|\n",
    "","","","",
    iceb_tu_kolbait(10,row[1]),
    row[1],
    iceb_tu_datzap(row[0]),sumadok,sumands);
    continue;
   }

  if(metka_ps > 0 && metka_ps < 5) /*запись импортирована*/
   {
    isumadok[0]+=sumadok;
    isumands[0]+=sumands;

    class iceb_tu_str datnak(iceb_tu_datzap(row[6]));    
    
    fprintf(ff,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|\n",
    iceb_tu_kolbait(10,row[7]),
    row[7],
    datnak.ravno(),
    sumadok,sumands,
    iceb_tu_kolbait(10,row[1]),
    row[1],iceb_tu_datzap(row[0]),sumadok,sumands);
   }
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
 
fprintf(ff,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_tu_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);




return(0);
}
