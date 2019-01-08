/*$Id: buhgsp.c,v 5.24 2014/07/31 07:09:15 sasa Exp $*/
/*13.07.2015	10.08.2006	Белых А.И.	buhgsp.c
Расчёт журнала для счётов с развёрнутым сальдо по заданным
спискам групп контрагентов
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

void buhgsp_ss(short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstrok_ras,FILE *ff);

void rpsrs(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *sp_kontr,class SPISOK *sp_grup,
class masiv_din_double *deb_saldon,class masiv_din_double *kre_saldon,class masiv_din_double *deb_oborot,
class masiv_din_double *kre_oborot,class masiv_din_double *deb_saldok,class masiv_din_double *kre_saldok);


extern short	startgodb; /*Стартовый год*/
extern int kol_strok_na_liste;

int buhgsp(class spis_oth *oth)
{
int kolstr;
char strsql[512];
static class iceb_tu_str shet("");
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str spisok("");
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт по спискам групп контрагентов"));

VV.VVOD_SPISOK_add_MD(gettext("Счёт....................."));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код списка групп..(,,)..."));

VV.VVOD_SPISOK_add_data(shet.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(spisok.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

shet.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
dat2.new_plus(VV.data_ravno(2));
spisok.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_11.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK5:
     iceb_redfil("buhgsp.alx",0);
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

GDITE();

//проверяем - счёт должен быть с развёрнутым сальдо
OPSHET rek_sh;
if(prsh1(shet.ravno(),&rek_sh) != 0)
 goto naz;
if(rek_sh.saldo != 3)
 {
  iceb_t_soob(gettext("Счёт должен быть с развёрнутым сальдо !"));
  goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;
class SQLCURSOR cur;
sprintf(strsql,"select str from Alx where fil='buhgsp.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки buhgsp.alx");
  return(1);
 }

class SPISOK naim_sp;
class SPISOK kod_sp;
class SPISOK kod_gr;

class iceb_tu_str bros("");
SQL_str row;

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],&bros,1,'|') != 0)
   continue;  
  if(SRAV(bros.ravno(),"Наименование списка групп",0) == 0)
   {

    polen(row[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
     continue;

    naim_sp.plus(bros.ravno());

    cur.read_cursor(&row);
    polen(row[0],&kod_sp,2,'|');

    cur.read_cursor(&row);
    polen(row[0],&bros,2,'|');
    iceb_t_fplus(1,bros.ravno(),&kod_gr,&cur);
       
   }
 }


int kol_spiskov=naim_sp.kolih();

if(kol_spiskov == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного списка групп контрагентов !"));
  return(1); 
 }

class SPISOK sp_kontr;
class SPISOK sp_grup;
class masiv_din_double deb_saldon;
class masiv_din_double kre_saldon;
class masiv_din_double deb_oborot;
class masiv_din_double kre_oborot;
class masiv_din_double deb_saldok;
class masiv_din_double kre_saldok;

//расчёт
rpsrs(shet.ravno(),dn,mn,gn,dk,mk,gk,&sp_kontr,&sp_grup,&deb_saldon,&kre_saldon,&deb_oborot,&kre_oborot,&deb_saldok,&kre_saldok);

FILE *ff;
char imaf[56];
sprintf(imaf,"gsp%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL) 
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт по спискам групп контрагентов"),dn,mn,gn,dk,mk,gk,ff);

int kolstrok_ras=5;
int kollist=0;
int kolgrup=0;
int kol_kontr=sp_kontr.kolih();
class iceb_tu_str naim_grup("");
class iceb_tu_str naim_kontr("");
double itogo_gr[6];
memset(itogo_gr,'\0',sizeof(itogo_gr));
double itogo_ob[6];
memset(itogo_ob,'\0',sizeof(itogo_ob));
double itogo_sp[6];
memset(itogo_sp,'\0',sizeof(itogo_sp));
short metka_gr=0;
class iceb_tu_str grupa("");

for(int ii=0; ii < kol_spiskov; ii++)
 {
  if(proverka(spisok.ravno(),kod_sp.ravno(ii),0,0) != 0)
   continue;
  memset(itogo_sp,'\0',sizeof(itogo_sp));
  
  if(ii != 0)
   {
    fprintf(ff,"\f");
    kolstrok_ras=0;
    kollist=0;
   }

  fprintf(ff,"%s\n",naim_sp.ravno(ii));
  kolstrok_ras+=1;

  metka_gr=0;      
  kolgrup=pole2(kod_gr.ravno(ii),',');

  for(int kk=0; kk < kolgrup; kk++)
   {

    polen(kod_gr.ravno(ii),&grupa,kk+1,',');
    if(grupa.ravno()[0] == '\0')
     continue;
    memset(itogo_gr,'\0',sizeof(itogo_gr));

    metka_gr++;
    if(metka_gr > 1)
     {
      fprintf(ff,"\f");
      kolstrok_ras=0;
     }

    sprintf(strsql,"select naik from Gkont where kod=%s",grupa.ravno());
    if(readkey(strsql,&row,&cur) == 1)
     naim_grup.new_plus(row[0]);
    else
     naim_grup.new_plus("");
     
    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%s:%s %s\n",gettext("Группа"),grupa.ravno(),naim_grup.ravno());
    
    sapgors(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);

    int kolih_v_gr=0;
    for(int spk=0; spk < kol_kontr; spk++)
     {
      if(SRAV(grupa.ravno(),sp_grup.ravno(spk),0) != 0)
       continue;

      if(fabs(deb_saldon.ravno(spk)) < 0.009 && \
         fabs(kre_saldon.ravno(spk)) < 0.009 && \
         fabs(deb_oborot.ravno(spk)) < 0.009 && \
         fabs(kre_oborot.ravno(spk)) < 0.009 )
            continue;
      //Узнаём наименование контрагента
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",sp_kontr.ravno(spk));
      if(readkey(strsql,&row,&cur) == 1)       
       naim_kontr.new_plus(row[0]);
      else
       naim_kontr.new_plus("");
      
      buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
            
      if(deb_saldon.ravno(spk) > kre_saldon.ravno(spk))
        fprintf(ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
        iceb_tu_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
        deb_saldon.ravno(spk),"",deb_oborot.ravno(spk),kre_oborot.ravno(spk));
      else
        fprintf(ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
        iceb_tu_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
        "",kre_saldon.ravno(spk),deb_oborot.ravno(spk),kre_oborot.ravno(spk));

      if(deb_saldok.ravno(spk) > kre_saldok.ravno(spk))
        fprintf(ff,"%14.2f\n",deb_saldok.ravno(spk));
      else
        fprintf(ff,"%14s %14.2f\n","",kre_saldok.ravno(spk));
       
      if(iceb_tu_strlen(naim_kontr.ravno()) > 20)
       {
        buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
        fprintf(ff,"%-10s %s\n"," ",
        iceb_tu_adrsimv(20,naim_kontr.ravno()));
       }

      itogo_gr[0]+=deb_saldon.ravno(spk);
      itogo_gr[1]+=kre_saldon.ravno(spk);

      itogo_gr[2]+=deb_oborot.ravno(spk);
      itogo_gr[3]+=kre_oborot.ravno(spk);

      itogo_gr[4]+=deb_saldok.ravno(spk);
      itogo_gr[5]+=kre_saldok.ravno(spk);
      kolih_v_gr++;  
     }

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");

    
    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
    iceb_tu_kolbait(31,gettext("Итого по группе")),gettext("Итого по группе"),
    itogo_gr[0],itogo_gr[1],itogo_gr[2],itogo_gr[3],itogo_gr[4],itogo_gr[5]);

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%*s %d\n",iceb_tu_kolbait(31,gettext("Количество контрагентов в группе")),gettext("Количество контрагентов в группе"),kolih_v_gr); 
    
    itogo_sp[0]+=itogo_gr[0];
    itogo_sp[1]+=itogo_gr[1];
    itogo_sp[2]+=itogo_gr[2];
    itogo_sp[3]+=itogo_gr[3];
    itogo_sp[4]+=itogo_gr[4];
    itogo_sp[5]+=itogo_gr[5];
    
   }

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
  iceb_tu_kolbait(31,gettext("Итого по списку")),gettext("Итого по списку"),
  itogo_sp[0],itogo_sp[1],itogo_sp[2],itogo_sp[3],itogo_sp[4],itogo_sp[5]);

  itogo_ob[0]+=itogo_sp[0];
  itogo_ob[1]+=itogo_sp[1];
  itogo_ob[2]+=itogo_sp[2];
  itogo_ob[3]+=itogo_sp[3];
  itogo_ob[4]+=itogo_sp[4];
  itogo_ob[5]+=itogo_sp[5];

 }

if(kol_spiskov > 1)
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_tu_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),
itogo_ob[0],itogo_ob[1],itogo_ob[2],itogo_ob[3],itogo_ob[4],itogo_ob[5]);



podpis(ff);
fclose(ff);

VVOD SOOB(1);

sprintf(strsql,"%-*s:%10.2f %10.2f",iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),itogo_ob[0],itogo_ob[1]);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%-*s:%10.2f %10.2f",iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),itogo_ob[2],itogo_ob[3]);
SOOB.VVOD_SPISOK_add_MD(strsql);

sprintf(strsql,"%-*s:%10.2f %10.2f",iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),itogo_ob[4],itogo_ob[5]);
SOOB.VVOD_SPISOK_add_MD(strsql);

soobshw(&SOOB,stdscr,-1,-1,0,1);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт по спискам групп контрагентов"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);

}

/***************************/
/*счётчик строк*/
/*********************/

void buhgsp_ss(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstrok_ras,FILE *ff)
{

*kolstrok_ras+=1;
if(*kolstrok_ras <= kol_strok_na_liste)
 return;

*kolstrok_ras=0;

fprintf(ff,"\f");
sapgors(dn,mn,gn,dk,mk,gk,kollist,kolstrok_ras,ff);
*kolstrok_ras+=1;

}
