/*$Id: rasrnn_v.c,v 5.83 2014/07/31 07:09:17 sasa Exp $*/
/*31.08.2016	29.11.2014	Белых А.И.	rasrnn_v8.c
Распечатка реестра выданных налоговых накладных 
*/
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include        "buhg.h"
#include "xrnn_poi.h"
#include "rnn_d5.h"


void rasrnn_v_xml_itog8(double *itog,double *itogo_u,FILE *ff_xml);
void rasrnn_v_str_xml8(int nomer_str,short d,short m,short g,const char *nomer_dok,const char *naim_kontr,const char *inn,double *suma,const char *viddok,FILE *ff_xml);
void rasrnn_v_rm8(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2);
void rasrnn_v_str8(const char *nomdok,const char *naim_kontr,int lnds,double sumd,double snds,double *stroka_xml,const char *viddok,FILE *ff);
void rasrnn_v_vvrozkor8(int nomer_str,short mnn,short gnn,const char *inn,double *stroka_xml,FILE *ff_xml_rozkor);

//void rasrnn_v_vvrozkor8(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,const char *nomer_dok,const char *naim_kontr,const char *inn,double *stroka_xml,const char *koment,int lnds,FILE *ff_xml_rozkor);
void rpvnn_vi8(char *vsego,double *itogo,double *itogo_u,FILE *ff);

extern int kol_strok_na_liste;

int rasrnn_v8(class xrnn_poi *data,
int period_type,
FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5 *dod5)
{
char strsql[1024];
int metka_sort=0;
class iceb_tu_str sort("");
if((metka_sort=rnn_vs()) < 0)
 return(1);
if(metka_sort == 0)
 sort.new_plus(" order by datd asc,nnni asc");
else
 sort.new_plus(" order by datd asc,dnz asc");
//printw("Реестр выданных накладных\n");
GDITE();
sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn");

if(data->dn != 0 && data->dk == 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn where \
datd >= '%04d-%02d-%02d'",data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

strcat(strsql,sort.ravno());


SQLCURSOR cur;
SQL_str row;
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  printw("%s\n",gettext("Не найдено ни одной записи !"));
  return(1);
 }
double proc_dot=data->proc_dot.ravno_atof();
class iceb_tu_str imafalx("rasrnn_v8.alx");


iceb_t_zagolov("Реєстро виданих та отриманих податкових накладних",data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,ff);
//Вставляем шапку 
iceb_t_insfil(imafalx.ravno(),ff);
if(ff_dot != NULL)
   iceb_t_insfil(imafalx.ravno(),ff_dot);

iceb_t_zagolov(gettext("Реестр выданных налоговых накладных"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,ff_prot);
if(data->kodgr1.ravno()[0] != '\0')
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr1.ravno());

rpvnn_ps_p(ff_prot);
/*****************************
fprintf(ff_lst_rozkor,"I Коригування податкових зобов'язань\n\
----------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                                 |Податкова накладна,  |Розрахунок коригуван.|Підстава  |       Обсяг         |       Сума          |Обсяг пос-|\n\
   |    Отримувач (покупець)         |за якою пров.коригув.|кількісних і вартісн.|для кориг.|      поставки       |        ПДВ          |тачання,  |\n\
   |                                 |                     |показників до под.нак|податкових|      (без ПДВ)      |       (+чи-)        |за яким не|\n\
   |-----------------------------------------------------------------------------|зобов'яза.|       (+чи-)        |                     |нарахову- |\n\
 N |   Найменування     |Індивідуал  |  Дата    |  Номер   | Номер    |Дата отрим|з податку |                     |                     |  вався   |\n\
   |(П.І.Б. для фізичних|  ьний      |          |          |          |мання отри|на додану |-------------------------------------------|податок на|\n\
   |      осіб)         |подаковий   |          |          |          |мувачем   |вартість  |   за     |   за     |   за     |    за    |  додану  |\n\
   |                    | номер      |          |          |          |(покупцем)|          |основною  |ставкою   |основною  | ставкою  | вартість |\n\
   |                    |            |          |          |          |розрахунку|          |ставкою   |   7%%     |  ставкою |    7%%    |          |\n\
------------------------------------------------------------------------------------------------------------------|--------------------------------|\n\
 1 |         2          |     3      |     4    |    5     |    6     |     7    |    8     |    9     |    10    |    11    |    12    |    13    |\n\
---------------------------------------------------------------------------------------------------------------------------------------------------|\n");
****************************/

fprintf(ff_lst_rozkor,"I Коригування податкових зобов'язань\n\
---------------------------------------------------------------------------------\n\
     |Платник    | Період   |Обсяг поста.|Сума податку на додану в.|Не нар.под. |\n\
  N  |           |          |            |-------------------------|            |\n\
     |           |          |            |Основ.ставк.|Ставка 7%%   |            |\n\
---------------------------------------------------------------------------------\n");
   
double sumd=0.;
double sumd1=0.;
double snds=0.;
double snds1=0.;

#define RAZMER_MAS 10
double itogo_mes[RAZMER_MAS];
memset(itogo_mes,'\0',sizeof(itogo_mes));

double itogo_o[RAZMER_MAS];
memset(itogo_o,'\0',sizeof(itogo_o));
double itogo_ou[RAZMER_MAS];
memset(itogo_ou,'\0',sizeof(itogo_ou));
int nomer_str=0;
int npz=0;
if(npz != 0)
 nomer_str=npz-1;

short mz=0;
short d,m,g;
short dd,md,gd;
int kolstr1=0;
int lnds=0;
int metka_pods;
class iceb_tu_str podsis("");
double sumd_pr=0.;
double sumnds_pr=0.;
class iceb_tu_str nomdok("");
class iceb_tu_str naim_kontr("");
int nomer_str_xml=0;
double stroka_xml[RAZMER_MAS];
double stroka_xml_dot[RAZMER_MAS];

#define RAZMER_MAS_KOR 4
double stroka_xml_kor[RAZMER_MAS_KOR];
double itogo_o1[RAZMER_MAS];
double itogo_o2[RAZMER_MAS];

int por_nom_dok=0;
int nomer_innn=0;
int korrekt=0;
int nomer_str_rozkor=0;
double itogo_rozkor[RAZMER_MAS_KOR];
memset(itogo_rozkor,'\0',sizeof(itogo_rozkor));
short dv,mv,gv;

#define RAZMER_MAS_INN 6
double itogo_inn[RAZMER_MAS_INN][RAZMER_MAS_KOR];

memset(itogo_inn,'\0',sizeof(itogo_inn));


while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(data->metka_ins == 1) //*только отмеченные
   if(data->nomzap_v.find(atoi(row[15])) < 0)
    continue;
    
  if(data->metka_ins == 2) //*только не отмеченные
    if(data->nomzap_v.find(atoi(row[15])) >= 0)
     continue;
  if(proverka(data->kodgr1.ravno(),row[10],0,0) != 0)
   continue;

  if(proverka(data->kodop.ravno(),row[12],0,0) != 0)
   continue;
  if(proverka(data->viddok.ravno(),row[13],0,0) != 0)
   continue;
  if(proverka(data->naim_kontr.ravno(),row[6],4,0) != 0)
   continue;
   
  if(data->podsistema.ravno()[0] != '\0')
   {
    class iceb_tu_str metka_pod("");

    if(row[7][0] == '1')
     metka_pod.new_plus(ICEB_MP_MATU);
    if(row[7][0] == '2')
     metka_pod.new_plus(ICEB_MP_USLUGI);
    if(row[7][0] == '3')
     metka_pod.new_plus(ICEB_MP_UOS);
    if(row[7][0] == '4')
     metka_pod.new_plus(ICEB_MP_UKR);
    if(row[7][0] == '5')
     metka_pod.new_plus(gettext("ГК"));
    if(proverka(data->podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
    continue;
   }
   
  if(proverka(data->nnn.ravno(),row[1],0,0) != 0)
    continue;

  if(proverka(data->nomdok.ravno(),row[9],0,0) != 0)
    continue;
  if(proverka(data->inn.ravno(),row[5],4,0) != 0)
   continue;
  if(proverka(data->koment.ravno(),row[11],4,0) != 0)
   continue;


  rsdat(&d,&m,&g,row[0],2);
  rsdat(&dd,&md,&gd,row[8],2);
  rsdat(&dv,&mv,&gv,row[14],2);

  korrekt=0;  
  if(strstrm(row[13],"У") == 1 || SRAV(row[13],"РКП",0) == 0 || SRAV(row[13],"РКЕ",0) == 0) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;

  if(mz != m)
   {
    if(mz != 0)
     {
      rasrnn_v_rm8(proc_dot,itogo_mes,itogo_o1,itogo_o2);
      rpvnn_vi8(gettext("Всего за месяц"),itogo_o1,itogo_ou,ff);
      if(ff_dot != NULL)
        rpvnn_vi8(gettext("Всего за месяц"),itogo_o2,itogo_ou,ff_dot);
     }

    for(int nom=0; nom < RAZMER_MAS; nom++)
      itogo_o[nom]+=itogo_mes[nom];
    
    memset(itogo_mes,'\0',sizeof(itogo_mes));
    mz=m;
   }

  sumd=atof(row[2]);
  sumd1=sumd=okrug(sumd,0.01);
  snds=atof(row[3]);
  snds1=snds=okrug(snds,0.01);

  if(proc_dot != 0.)
   {
    sumd1=sumd1*(100.0-proc_dot)/100.;
    sumd1=okrug(sumd1,0.01);
    snds1=snds1*(100.0-proc_dot)/100.;
    snds1=okrug(snds1,0.01);
   }

  lnds=atoi(row[4]);

  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd;
    if(lnds == 0)
     {
      itogo_ou[1]+=sumd-snds;
      itogo_ou[2]+=snds;
     }

    if(lnds == 4)
     {
      itogo_ou[3]+=sumd-snds;
      itogo_ou[4]+=snds;
     }

    if(lnds == 1)
     itogo_ou[5]+=sumd;

    if(lnds == 2)
     itogo_ou[6]+=sumd;

    if(lnds == 3)
     itogo_ou[8]+=sumd;

   }  
  sumd_pr+=sumd;
  sumnds_pr+=snds;
  
  metka_pods=atoi(row[7]);
  nomdok.new_plus(row[1]);

  naim_kontr.new_plus(row[6]);

  fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
  ++nomer_str,
  d,m,g,
  iceb_tu_kolbait(8,nomdok.ravno()),iceb_tu_kolbait(8,nomdok.ravno()),nomdok.ravno(),
  iceb_tu_kolbait(5,row[13]),iceb_tu_kolbait(5,row[13]),row[13],
  iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
  iceb_tu_kolbait(14,row[5]),row[5],
  sumd1);
  
  if(ff_dot != NULL)
    fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
    nomer_str,
    d,m,g,
    iceb_tu_kolbait(8,nomdok.ravno()),iceb_tu_kolbait(8,nomdok.ravno()),nomdok.ravno(),
    iceb_tu_kolbait(5,row[13]),iceb_tu_kolbait(5,row[13]),row[13],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    iceb_tu_kolbait(14,row[5]),row[5],
    sumd-sumd1);

  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));

  
  
  stroka_xml[0]=sumd1;
  stroka_xml_dot[0]=sumd-sumd1;

  if(lnds == 0) /*колонка 8,9*/
   {
    stroka_xml[1]=sumd-snds;
    stroka_xml[2]=snds;    
   }

  if(lnds == 4) /*колонка 10,11*/
   {
    stroka_xml[3]=sumd-snds;
    stroka_xml[4]=snds;    
   }

  if(lnds == 1) /*колонка 12*/
   {
    stroka_xml[5]=sumd;
   }

  if(lnds == 2) /*колонка 13*/
   {
    stroka_xml[6]=sumd;
   }


  if(lnds == 3) /*колонка 15*/
   {
    stroka_xml[8]=sumd;
   }

  for(int nom=0; nom < RAZMER_MAS; nom++)
   itogo_mes[nom]+=stroka_xml[nom];
  
  rasrnn_v_str8(nomdok.ravno(),naim_kontr.ravno(),lnds,sumd1,snds1,stroka_xml,row[13],ff);
  if(ff_dot != NULL)
    rasrnn_v_str8(nomdok.ravno(),naim_kontr.ravno(),lnds,sumd-sumd1,snds-snds1,stroka_xml_dot,row[13],ff_dot);

  podsis.new_plus("");

  if(metka_pods == 1)
   {
    class iceb_tu_str imafil_xml("");
    podsis.new_plus(ICEB_MP_MATU);
    //Запись накладной в xml формате
    xml_nn_mat(&por_nom_dok,2,dd,md,gd,row[9],row[1],data->mn,data->gn,data->mk,period_type,row[13],&imafil_xml);
   }

  if(metka_pods == 2)
   {
    class iceb_tu_str imafil_xml("");
    podsis.new_plus(gettext("УCЛ"));
    //Запись накладной в xml формате
    xml_nn_usl(&por_nom_dok,2,dd,md,gd,row[9],row[1],data->mn,data->gn,data->mk,period_type,row[13],&imafil_xml);
   }
  if(metka_pods == 3)
   {
    class iceb_tu_str imafil_xml("");
    podsis.new_plus(gettext("УОC"));
    //Запись накладной в xml формате
    xml_nn_uos(&por_nom_dok,2,dd,md,gd,row[9],row[1],data->mn,data->gn,data->mk,period_type,row[13],&imafil_xml);
   }
   

  fprintf(ff_prot,"%5d|%02d.%02d.%04d|%-*s|%11s|%9s|%d|%-5s|%-12s|%-*s|%02d.%02d.%04d|%-10s|%-*.*s|%s\n",
  nomer_str,d,m,g,
  iceb_tu_kolbait(10,row[1]),
  row[1],
  row[2],row[3],lnds,row[10],row[5],
  iceb_tu_kolbait(5,podsis.ravno()),
  podsis.ravno(),
  dd,md,gd,row[9],
  iceb_tu_kolbait(24,naim_kontr.ravno()),
  iceb_tu_kolbait(24,naim_kontr.ravno()),
  naim_kontr.ravno(),
  row[11]);


  if(korrekt == 1) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
   {
/*****************  
    class iceb_tu_str komentout(row[11]);
    if(komentout.getdlinna() <= 1)
      komentout.new_plus("Несвоєчасне отримання податкової накладної");
***************/
    if(dv == 0)
     {
      dv=d;
      mv=m;
      gv=g;
     }

    
    memset(stroka_xml_kor,'\0',sizeof(stroka_xml_kor));

    if(lnds == 0)
     {
      stroka_xml_kor[0]=sumd-snds;
      stroka_xml_kor[1]=snds;
     }
    if(lnds == 4)
     {
      stroka_xml_kor[0]=sumd-snds;
      stroka_xml_kor[2]=snds;
     }

    if(lnds == 1 || lnds == 2 || lnds == 3)
     stroka_xml_kor[3]=sumd;    

    for(int nom=0; nom < RAZMER_MAS_KOR; nom++)
      {
       itogo_rozkor[nom]+=stroka_xml_kor[nom];
      }         

    for(int nom=0; nom < RAZMER_MAS_INN; nom++)
     {
      sprintf(strsql,"%d00000000000",nom+1);
      if(SRAV(row[5],strsql,0) == 0)
       {
        for(int nom1=0; nom1 < RAZMER_MAS_KOR; nom1++)
         {
          itogo_inn[nom][nom1]+=stroka_xml_kor[nom1];
         }
       }
     }

   // rasrnn_v_vvrozkor8(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml,komentout.ravno(),lnds,ff_xml_rozkor);
    rasrnn_v_vvrozkor8(++nomer_str_rozkor,m,g,row[5],stroka_xml,ff_xml_rozkor);

/*********************
    fprintf(ff_lst_rozkor,"%3d %-*.*s %*s %02d.%02d.%04d %-*s %-10s %02d.%02d.%04d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    nomer_str_rozkor,
    iceb_tu_kolbait(20,naim_kontr.ravno()),
    iceb_tu_kolbait(20,naim_kontr.ravno()),
    naim_kontr.ravno(),
    iceb_tu_kolbait(10,row[5]),row[5],
    dv,mv,gv,
    iceb_tu_kolbait(10,nomdok.ravno()),nomdok.ravno(),
    "1",
    d,m,g,
    iceb_tu_kolbait(10,komentout.ravno()),
    iceb_tu_kolbait(10,komentout.ravno()),
    komentout.ravno(),
    stroka_xml_kor[0],
    stroka_xml_kor[1],
    stroka_xml_kor[2],
    stroka_xml_kor[3],
    stroka_xml_kor[4]);
************************/
    fprintf(ff_lst_rozkor,"%5d %-*s %02d.%02d.%04d %12.2f %12.2f %12.2f %12.2f\n",
    nomer_str_rozkor,
    iceb_tu_kolbait(10,row[5]),row[5],
    dv,mv,gv,
    stroka_xml_kor[0],
    stroka_xml_kor[1],
    stroka_xml_kor[2],
    stroka_xml_kor[3]);
    
   }

            
  rasrnn_v_str_xml8(++nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml,row[13],ff_xml);
  
   

  if(ff_xml_dot != NULL)
   {
    rasrnn_v_str_xml8(nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml_dot,row[13],ff_xml_dot);
   }  

  if(korrekt == 0)
   {
    if(dod5->innn2.find(row[5]) < 0)
     dod5->innn2.plus(row[5]);    

    if(mv != 0)   
     sprintf(strsql,"%s|%02d.%04d",row[5],mv,gv);
    else
     sprintf(strsql,"%s|%02d.%04d",row[5],md,gd);
     
    if((nomer_innn=dod5->innn_per2.find(strsql)) < 0)
     {
      dod5->innn_per2.plus(strsql);    
      dod5->naim_kontr2.plus(row[6]);
     }  

    dod5->suma_bez_nds2.plus(sumd-snds,nomer_innn);

    if(lnds == 4)
     {
      dod5->suma_nds2.plus(0.,nomer_innn);
      dod5->suma_nds2_7.plus(snds,nomer_innn);
     }
    else /*нужно учитывать что lnds может принимать разные значения а в массывы должно быть добавлено при любом значении lnds*/
     {
      dod5->suma_nds2.plus(snds,nomer_innn);
      dod5->suma_nds2_7.plus(0.,nomer_innn);
     }

    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }

   }
 }

for(int nom=0; nom < RAZMER_MAS_INN; nom++)
 {
  sprintf(strsql,"%d00000000000",nom+1);
  fprintf(ff_lst_rozkor,"%-28s %12.2f %12.2f %12.2f %12.2f\n",
  strsql,
  itogo_inn[nom][0],
  itogo_inn[nom][1],
  itogo_inn[nom][2],
  itogo_inn[nom][3]);
 }
fprintf(ff_prot,"%*s:%10.2f %8.2f\n",iceb_tu_kolbait(27,gettext("Итого")),gettext("Итого"),sumd_pr,sumnds_pr);


rasrnn_v_rm8(proc_dot,itogo_mes,itogo_o1,itogo_o2);

rpvnn_vi8(gettext("Всего за месяц"),itogo_o1,itogo_ou,ff);
if(ff_dot != NULL)
  rpvnn_vi8(gettext("Всего за месяц"),itogo_o2,itogo_ou,ff_dot);

for(int nom=0; nom < RAZMER_MAS; nom++)
 itogo_o[nom]+=itogo_mes[nom];

rasrnn_v_rm8(proc_dot,itogo_o,itogo_o1,itogo_o2);

if(data->mn != data->mk)
 {
  rpvnn_vi8(gettext("Общий итог"),itogo_o1,itogo_ou,ff);
  if(ff_dot != NULL)
    rpvnn_vi8(gettext("Общий итог"),itogo_o2,itogo_ou,ff_dot);
  
 }
rasrnn_v_xml_itog8(itogo_o1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_v_xml_itog8(itogo_o2,itogo_ou,ff_xml_dot);

if(nomer_str_rozkor > 0)
 {

  for(int nom=0; nom < RAZMER_MAS_INN; nom++)
   {
    sprintf(strsql,"%d00000000000",nom+1);
    fprintf(ff_lst_rozkor,"%-30s %12.2f %12.2f %12.2f %12.2f\n",
    strsql,
    itogo_inn[nom][0],
    itogo_inn[nom][1],
    itogo_inn[nom][2],
    itogo_inn[nom][3]);

    if(nom == 0)  
     {
      fprintf(ff_xml_rozkor,"   <R0011G4>%.2f</R0011G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0011G5>%.2f</R0011G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0011G6>%.2f</R0011G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0011G7>%.2f</R0011G7>\n",itogo_inn[nom][3]);
     }  
    if(nom == 1)  
     {
      fprintf(ff_xml_rozkor,"   <R0012G4>%.2f</R0012G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0012G5>%.2f</R0012G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0012G6>%.2f</R0012G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0012G7>%.2f</R0012G7>\n",itogo_inn[nom][3]);
     }  
    if(nom == 2)  
     {
      fprintf(ff_xml_rozkor,"   <R0013G4>%.2f</R0013G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0013G5>%.2f</R0013G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0013G6>%.2f</R0013G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0013G7>%.2f</R0013G7>\n",itogo_inn[nom][3]);
     }  
    if(nom == 3)  
     {
      fprintf(ff_xml_rozkor,"   <R0014G4>%.2f</R0014G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0014G5>%.2f</R0014G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0014G6>%.2f</R0014G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0014G7>%.2f</R0014G7>\n",itogo_inn[nom][3]);
     }  
    if(nom == 4)  
     {
      fprintf(ff_xml_rozkor,"   <R0015G4>%.2f</R0015G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0015G5>%.2f</R0015G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0015G6>%.2f</R0015G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0015G7>%.2f</R0015G7>\n",itogo_inn[nom][3]);
     }  
    if(nom == 5)  
     {
      fprintf(ff_xml_rozkor,"   <R0016G4>%.2f</R0016G4>\n",itogo_inn[nom][0]);
      fprintf(ff_xml_rozkor,"   <R0016G5>%.2f</R0016G5>\n",itogo_inn[nom][1]);
      fprintf(ff_xml_rozkor,"   <R0016G6>%.2f</R0016G6>\n",itogo_inn[nom][2]);
      fprintf(ff_xml_rozkor,"   <R0016G7>%.2f</R0016G7>\n",itogo_inn[nom][3]);
     }  
   }

  fprintf(ff_xml_rozkor,"   <R010G4>%.2f</R010G4>\n",itogo_rozkor[0]);//9
  fprintf(ff_xml_rozkor,"   <R010G5>%.2f</R010G5>\n",itogo_rozkor[1]);//10
  fprintf(ff_xml_rozkor,"   <R010G6>%.2f</R010G6>\n",itogo_rozkor[2]);//11
  fprintf(ff_xml_rozkor,"   <R010G7>%.2f</R010G7>\n",itogo_rozkor[3]);//12


  fprintf(ff_lst_rozkor,"%*s:%12.2f %12.2f %12.2f %12.2f\n",
  iceb_tu_kolbait(28,gettext("Итого")),gettext("Итого"),
  itogo_rozkor[0],
  itogo_rozkor[1],
  itogo_rozkor[2],
  itogo_rozkor[3]);
 }

GDITE();
return(0);
}
/**********************************/
/*Запись строки в xml файл*/
/**************************/

void rasrnn_v_str_xml8(int nomer_str,
short d,short m,short g,
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *suma,
const char *viddok,
FILE *ff_xml)
{

fprintf(ff_xml,"   <T1RXXXXG1 ROWNUM=\"%d\">%d</T1RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T1RXXXXG2D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG2D>\n",nomer_str,d,m,g); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG3S ROWNUM=\"%d\">%s</T1RXXXXG3S>\n",nomer_str,iceb_tu_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG3S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG3S>\n",nomer_str); //3


if(viddok[0] != '\0')
 {
  class iceb_tu_str r1("");
  class iceb_tu_str r2("");
  class iceb_tu_str r3("");

  if(polen(viddok,&r1,1,',') != 0)
   r1.new_plus(viddok);
  else
   {
    polen(viddok,&r2,2,',');
    polen(viddok,&r3,3,',');
   }   

  if(iceb_tu_strlen(r1.ravno()) > 3)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 3 %s\n%d.%d.%d %s\n%.*s",
    gettext("Первое поле вида документа"),
    r1.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_tu_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(iceb_tu_strlen(r2.ravno()) > 2)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 2 %s\n%d.%d.%d %s\n%.*s",
    gettext("Второе поле вида документа"),
    r2.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_tu_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(iceb_tu_strlen(r3.ravno()) > 1)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 1 %s\n%d.%d.%d %s\n%.*s",
    gettext("Третье поле вида документа"),
    r2.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_tu_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(r1.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG41S ROWNUM=\"%d\">%s</T1RXXXXG41S>\n",nomer_str,r1.ravno()); //4
  if(r2.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG42S ROWNUM=\"%d\">%s</T1RXXXXG42S>\n",nomer_str,r2.ravno()); //4
  if(r3.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG43S ROWNUM=\"%d\">%s</T1RXXXXG43S>\n",nomer_str,r3.ravno()); //4
 }

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",nomer_str,iceb_tu_filtr_xml(naim_kontr)); //5
else
 fprintf(ff_xml,"   <T1RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG5S>\n",nomer_str); //5

if(inn[0] == '\0' || isdigit(inn[0]) == 0) /*Значит символ не число*/
  fprintf(ff_xml,"   <T1RXXXXG6 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str); //6
else
  fprintf(ff_xml,"   <T1RXXXXG6 ROWNUM=\"%d\">%s</T1RXXXXG6>\n",nomer_str,inn); //6

fprintf(ff_xml,"   <T1RXXXXG7 ROWNUM=\"%d\">%.2f</T1RXXXXG7>\n",nomer_str,suma[0]); //7

fprintf(ff_xml,"   <T1RXXXXG8 ROWNUM=\"%d\">%.2f</T1RXXXXG8>\n",nomer_str,suma[1]); //8
fprintf(ff_xml,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",nomer_str,suma[2]); //9

fprintf(ff_xml,"   <T1RXXXXG110 ROWNUM=\"%d\">%.2f</T1RXXXXG110>\n",nomer_str,suma[3]); //10
fprintf(ff_xml,"   <T1RXXXXG111 ROWNUM=\"%d\">%.2f</T1RXXXXG111>\n",nomer_str,suma[4]); //11

fprintf(ff_xml,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",nomer_str,suma[5]); //12
fprintf(ff_xml,"   <T1RXXXXG113 ROWNUM=\"%d\">%.2f</T1RXXXXG113>\n",nomer_str,suma[6]); //13
fprintf(ff_xml,"   <T1RXXXXG114 ROWNUM=\"%d\">%.2f</T1RXXXXG114>\n",nomer_str,suma[7]); //14
fprintf(ff_xml,"   <T1RXXXXG12 ROWNUM=\"%d\">%.2f</T1RXXXXG12>\n",nomer_str,suma[8]); //15
fprintf(ff_xml,"   <T1RXXXXG13 ROWNUM=\"%d\">%.2f</T1RXXXXG13>\n",nomer_str,suma[9]); //16



}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_v_vvrozkor8(int nomer_str,
short mnn,short gnn, //Дата выписки налоговой накладной
const char *inn,
double *stroka_xml,
FILE *ff_xml_rozkor)
{
/*************** 
fprintf(ff_xml_rozkor,"   <T11RXXXXG2S ROWNUM=\"%d\">%s</T11RXXXXG2S>\n",*nomer_str,iceb_tu_filtr_xml(naim_kontr));//2
fprintf(ff_xml_rozkor,"   <T11RXXXXG3 ROWNUM=\"%d\">%s</T11RXXXXG3>\n",*nomer_str,inn);//3
fprintf(ff_xml_rozkor,"   <T11RXXXXG4D ROWNUM=\"%d\">%02d%02d%04d</T11RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);//4
fprintf(ff_xml_rozkor,"   <T11RXXXXG5S ROWNUM=\"%d\">%s</T11RXXXXG5S>\n",*nomer_str,iceb_tu_filtr_xml(nomer_dok));//5
fprintf(ff_xml_rozkor,"   <T11RXXXXG6S ROWNUM=\"%d\">1</T11RXXXXG6S>\n",*nomer_str); //6 номер расчёта корректировки
fprintf(ff_xml_rozkor,"   <T11RXXXXG7D ROWNUM=\"%d\">%02d%02d%04d</T11RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);//7
fprintf(ff_xml_rozkor,"   <T11RXXXXG8S ROWNUM=\"%d\">%s</T11RXXXXG8S>\n",*nomer_str,iceb_tu_filtr_xml(koment));//8

fprintf(ff_xml_rozkor,"   <T11RXXXXG9 ROWNUM=\"%d\">%.2f</T11RXXXXG9>\n",*nomer_str,stroka_xml[0]);//9
fprintf(ff_xml_rozkor,"   <T11RXXXXG10 ROWNUM=\"%d\">%.2f</T11RXXXXG10>\n",*nomer_str,stroka_xml[1]);//10
fprintf(ff_xml_rozkor,"   <T11RXXXXG11 ROWNUM=\"%d\">%.2f</T11RXXXXG11>\n",*nomer_str,stroka_xml[2]);//11
fprintf(ff_xml_rozkor,"   <T11RXXXXG12 ROWNUM=\"%d\">%.2f</T11RXXXXG12>\n",*nomer_str,stroka_xml[3]);//12
fprintf(ff_xml_rozkor,"   <T11RXXXXG13 ROWNUM=\"%d\">%.2f</T11RXXXXG13>\n",*nomer_str,stroka_xml[4]);//13
********************/

fprintf(ff_xml_rozkor,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,inn);//3
fprintf(ff_xml_rozkor,"   <T1RXXXXG3A ROWNUM=\"%d\">%02dd</T1RXXXXG3A>\n",nomer_str,mnn);//4
fprintf(ff_xml_rozkor,"   <T1RXXXXG3B ROWNUM=\"%d\">%04d</T1RXXXXG3B>\n",nomer_str,gnn);//4

fprintf(ff_xml_rozkor,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,stroka_xml[0]);//9
fprintf(ff_xml_rozkor,"   <T1RXXXXG5 ROWNUM=\"%d\">%.2f</T1RXXXXG5>\n",nomer_str,stroka_xml[1]);//10
fprintf(ff_xml_rozkor,"   <T1RXXXXG6 ROWNUM=\"%d\">%.2f</T1RXXXXG6>\n",nomer_str,stroka_xml[2]);//11
fprintf(ff_xml_rozkor,"   <T1RXXXXG7 ROWNUM=\"%d\">%.2f</T1RXXXXG7>\n",nomer_str,stroka_xml[3]);//12

}
/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_v_xml_itog8(double *itog,double *itog_u,FILE *ff_xml)
{
fprintf(ff_xml,"   <R011G7>%.2f</R011G7>\n",itog[0]);
fprintf(ff_xml,"   <R011G8>%.2f</R011G8>\n",itog[1]);
fprintf(ff_xml,"   <R011G9>%.2f</R011G9>\n",itog[2]);
fprintf(ff_xml,"   <R011G110>%.2f</R011G110>\n",itog[3]);
fprintf(ff_xml,"   <R011G111>%.2f</R011G111>\n",itog[4]);
fprintf(ff_xml,"   <R011G10>%.2f</R011G10>\n",itog[5]);
fprintf(ff_xml,"   <R011G113>%.2f</R011G113>\n",itog[6]);
fprintf(ff_xml,"   <R011G114>%.2f</R011G114>\n",itog[7]);
fprintf(ff_xml,"   <R011G12>%.2f</R011G12>\n",itog[8]);
fprintf(ff_xml,"   <R011G13>%.2f</R011G13>\n",itog[9]);

fprintf(ff_xml,"   <R012G7>%.2f</R012G7>\n",itog_u[0]);
fprintf(ff_xml,"   <R012G8>%.2f</R012G8>\n",itog_u[1]);
fprintf(ff_xml,"   <R012G9>%.2f</R012G9>\n",itog_u[2]);
fprintf(ff_xml,"   <R012G110>%.2f</R012G110>\n",itog_u[3]);
fprintf(ff_xml,"   <R012G111>%.2f</R012G111>\n",itog_u[4]);
fprintf(ff_xml,"   <R012G10>%.2f</R012G10>\n",itog_u[5]);
fprintf(ff_xml,"   <R012G113>%.2f</R012G113>\n",itog_u[6]);
fprintf(ff_xml,"   <R012G114>%.2f</R012G114>\n",itog_u[7]);
fprintf(ff_xml,"   <R012G12>%.2f</R012G12>\n",itog_u[8]);
fprintf(ff_xml,"   <R012G13>%.2f</R012G13>\n",itog_u[9]);
}
/************************/
/*разбивка массива по проценту на два массива*/
/*******************************/
void rasrnn_v_rm8(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2)
{
memset(itogo_o1,'\0',sizeof(double)*RAZMER_MAS);
memset(itogo_o2,'\0',sizeof(double)*RAZMER_MAS);

for(int nom=0; nom < RAZMER_MAS; nom++)
 itogo_o1[nom]=itogo_o[nom];

if(proc_dot != 0.)
 for(int nom=0; nom < RAZMER_MAS; nom++)
  {   
   itogo_o1[nom]=itogo_o[nom]*(100.-proc_dot)/100.;
   itogo_o1[nom]=okrug(itogo_o1[nom],0.01);
   itogo_o2[nom]=itogo_o[nom]-itogo_o1[nom];
  }

}
/***********************/
/*вывод строки продолжения*/
/**************************/
void rasrnn_v_str8(const char *nomdok,const char *naim_kontr,int lnds,double sumd,double snds,
double *stroka_xml,
const char *viddok,
FILE *ff)
{

for(int nom=1; nom < 4; nom+=2)
 {
  if(fabs(stroka_xml[nom]) > 0.009)
   fprintf(ff,"%9.2f|",stroka_xml[nom]);
  else
   fprintf(ff,"%9s|","");

  if(fabs(stroka_xml[nom+1]) > 0.009) 
    fprintf(ff,"%8.2f|",stroka_xml[nom+1]);
  else
   {
    if(fabs(stroka_xml[nom]) > 0.009)
      fprintf(ff,"XXXXXXXX|");
    else
     fprintf(ff,"%8s|","");
   }
 }

for(int nom=5; nom < RAZMER_MAS-1; nom++)
 {
  if(fabs(stroka_xml[nom]) > 0.009)
   fprintf(ff,"%9.2f|",stroka_xml[nom]);
  else  
   fprintf(ff,"%9s|","");
 }

if(fabs(stroka_xml[9]) > 0.009)
 fprintf(ff,"%10.2f|",stroka_xml[9]);
else  
 fprintf(ff,"%10s|","");
 

fprintf(ff,"\n");
if(iceb_tu_strlen(naim_kontr) > 20 || iceb_tu_strlen(nomdok) > 8 || iceb_tu_strlen(viddok) > 5)
 {
  fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%9s|%9s|%9s|%10s|\n",
  "","",
  iceb_tu_kolbait(8,iceb_tu_adrsimv(8,nomdok)),iceb_tu_kolbait(8,iceb_tu_adrsimv(8,nomdok)),iceb_tu_adrsimv(8,nomdok),
  iceb_tu_kolbait(5,iceb_tu_adrsimv(5,viddok)),iceb_tu_kolbait(5,iceb_tu_adrsimv(5,viddok)),iceb_tu_adrsimv(5,viddok),
  iceb_tu_kolbait(20,iceb_tu_adrsimv(20,naim_kontr)),iceb_tu_kolbait(20,iceb_tu_adrsimv(20,naim_kontr)),iceb_tu_adrsimv(20,naim_kontr),
  "","","","","","","","","","","");
 }
for(int nom=40; nom < iceb_tu_strlen(naim_kontr); nom+=20)
 {
  fprintf(ff,"|%5s|%10s|%8s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%9s|%9s|%9s|%10s|\n",
  "","","","",
  iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim_kontr)),iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim_kontr)),iceb_tu_adrsimv(nom,naim_kontr),
  "","","","","","","","","","","");
 }

}
/**************************/
void rpvnn_vi8s(char *vsego,double *itogo,FILE *ff)
{

if(itogo[1] != 0.)//8
 fprintf(ff,"%9.2f|",itogo[1]);
else
 fprintf(ff,"%9s|","");

if(itogo[2] != 0.)//9
 fprintf(ff,"%8.2f|",itogo[2]);
else
 fprintf(ff,"%8s|","");


if(itogo[3] != 0.)//10
 fprintf(ff,"%9.2f|",itogo[3]);
else
 fprintf(ff,"%9s|","");

if(itogo[4] != 0.)//11
 fprintf(ff,"%8.2f|",itogo[4]);
else
 fprintf(ff,"%8s|","");


if(itogo[5] != 0.)//12
 fprintf(ff,"%9.2f|",itogo[5]);
else
 fprintf(ff,"%9s|","");

if(itogo[6] != 0.)//13
 fprintf(ff,"%9.2f|",itogo[6]);
else
 fprintf(ff,"%9s|","");

if(itogo[7] != 0.)//14
 fprintf(ff,"%9.2f|",itogo[7]);
else
 fprintf(ff,"%9s|","");

if(itogo[8] != 0.)//15
 fprintf(ff,"%9.2f|",itogo[8]);
else
 fprintf(ff,"%9s|","");

if(itogo[9] != 0.)//16
 fprintf(ff,"%10.2f|",itogo[9]);
else
 fprintf(ff,"%10s|","");

fprintf(ff,"\n");
}
/***********************************************/

void rpvnn_vi8(char *vsego,double *itogo,double *itogo_u,FILE *ff)
{
class iceb_tu_str str_pod;


str_pod.plus("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

fprintf(ff,"%s\n",str_pod.ravno());

fprintf(ff,"|%*s|%10.2f|",iceb_tu_kolbait(67,vsego),vsego,itogo[0]);//7

rpvnn_vi8s(vsego,itogo,ff);

fprintf(ff,"%s\n",str_pod.ravno());


const char *repl={"З них включено до уточнюючих розрахунків за звітний період"};

fprintf(ff,"|%*s|%10.2f|",iceb_tu_kolbait(67,repl),repl,itogo_u[0]);//7

rpvnn_vi8s(vsego,itogo_u,ff);

fprintf(ff,"%s\n",str_pod.ravno());

}
