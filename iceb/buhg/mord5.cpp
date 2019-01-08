/*$Id: mord5.c,v 5.19 2014/07/31 07:09:17 sasa Exp $*/
/*15.07.2015	10.08.2008	Белых А.И.	mord5.c
Мемориальный ордер 5
*/
#include <errno.h>
#include "buhg.h"

void mord5_rasp(const char *variant,const char *ikod,short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *naim_oper,class SPISOK *sp_deb,class SPISOK *sp_kre,class masiv_din_double *sum_op,FILE *ff);


int mord5(class spis_oth *oth)
{

char strsql[1024];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str variants("");
int kom=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N5"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Вариант расчёта..(,,).."));//2

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(variants.ravno(),512);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);      


kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
variants.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(bros);
   goto naz;
 
  case FK2:
  case PLU:
    break;

  case FK4:
    VV.VVOD_clear_data();         
    goto naz;

  case FK5:
    iceb_redfil("mord5.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);
     
  default:
   kom=0;
   goto naz;
 }

GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='mord5.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"mord5.alx");
  iceb_t_soob(strsql);
  return(1);
 }

class SPISOK naim_oper; //наименование операций
class SPISOK sp_deb;
class SPISOK sp_kre;

class SPISOK variant; /*Наименование вариантов настроек*/
class iceb_tu_str var;
class iceb_tu_str nm_op;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;  
  
  if(polen(row_alx[0],&nm_op,1,'|') != 0)
   continue;

  if(SRAV(nm_op.ravno(),"Вариант",0) == 0)
   {
    polen(row_alx[0],&var,2,'|');
    variant.plus(var.ravno());
    continue;
   }   

  if(variant.kolih() == 0)
   {
    variant.plus("0");
    var.new_plus("0");
   }

  sprintf(strsql,"%s|%s",var.ravno(),nm_op.ravno());
  naim_oper.plus(strsql);

  polen(row_alx[0],strsql,sizeof(strsql),2,'|');
  sp_deb.plus(strsql);

  polen(row_alx[0],strsql,sizeof(strsql),3,'|');
  sp_kre.plus(strsql);
      
 }

int kol_nast=naim_oper.kolih();
class masiv_din_double sum_op;
sum_op.make_class(kol_nast);


sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and deb <> 0. order by datp,nomd asc",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur ;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  goto naz;
 }
SQL_str row;

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  for(int ii=0; ii < kol_nast; ii++)
   {
    if(proverka(sp_deb.ravno(ii),row[0],1,0) != 0)
     continue;   
    if(proverka(sp_kre.ravno(ii),row[1],1,0) != 0)
     continue;
    sum_op.plus(atof(row[2]),ii);
   }

 }
FILE *ff;
char imaf[64];
sprintf(imaf,"mord5_%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
class iceb_tu_str ikod("");

sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 ikod.new_plus(row[0]);
for(int ii=0; ii < variant.kolih(); ii++)
 {
  if(proverka(variants.ravno(),variant.ravno(ii),0,0) != 0)
   continue;
  if(ii > 0)
    fprintf(ff,"\f");
  mord5_rasp(variant.ravno(ii),ikod.ravno(),dn,mn,gn,dk,mk,gk,&naim_oper,&sp_deb,&sp_kre,&sum_op,ff);
 }
fclose(ff);
 
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus("Меморіальний ордер N5");
iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);
return(0);
}


/***********************************/
/*Распечатка мемориального ордера*/
/*********************************/
void mord5_rasp(const char *variant,const char *ikod,
short dn,short mn,short gn,
short dk,short mk,short gk,
class SPISOK *naim_oper,
class SPISOK *sp_deb,
class SPISOK *sp_kre,
class masiv_din_double *sum_op,
FILE *ff)
{
int kol_nast=naim_oper->kolih();

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff,"\
                                                       Типова форма N405(бюджет)\n\
                                                       Затверджено\n\
                                                       Наках Державного казначейства України\n\
                                                       27.07.2000 N68\n\
\n\
Ідентифікаційний код %-*s                  Код за ДКУД ______________\n\
за ЄДРПОУ\n\
\n\
                          МЕМОРІАЛЬНИЙ ОРДЕР N5\n",iceb_tu_kolbait(16,ikod),ikod);

fprintf(ff,"\
                          за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n",dn,mn,gn,dk,mk,gk);
                          

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N |    Зміст операції            | Дебет субрахунку   | Кредит субрахунку  |  Сумма    |\n");
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

int nomer_str=0;
class iceb_tu_str var;
class iceb_tu_str nm_op;
for(int ii=0; ii < kol_nast; ii++)
 {
  polen(naim_oper->ravno(ii),&var,1,'|');
  if(SRAV(variant,var.ravno(),0) != 0)
   continue;
  polen(naim_oper->ravno(ii),&nm_op,2,'|');
  
  fprintf(ff,"%3d %-*.*s %-20s %-20s %10.2f\n",
  ++nomer_str,
  iceb_tu_kolbait(30,nm_op.ravno()),iceb_tu_kolbait(30,nm_op.ravno()),nm_op.ravno(),
  sp_deb->ravno(ii),
  sp_kre->ravno(ii),
  sum_op->ravno(ii));

  if(iceb_tu_strlen(naim_oper->ravno(ii)) > 30)
   fprintf(ff,"%3s %s\n","",iceb_tu_adrsimv(30,naim_oper->ravno(ii))); 
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,"\
                                                                     Усього :%10.2f\n",sum_op->suma());
fprintf(ff,"\
                                       Сумма оборотів за меморіальним оредром____________\n");
class iceb_t_fioruk_rk gbuh;
iceb_t_fioruk(2,&gbuh);

                                       
fprintf(ff,"\
Виконавець         __________________ _______________ %s\n\
                        (посада)         (підпис)     (ініціали і призвище)\n\n\
Перевірив          __________________ _______________ %s %s\n\
                        (посада)         (підпис)     (ініціали і призвище)\n\n\
Головний бухгалтер _______________ %s %s\n\
                      (підпис)     (ініціали і призвище)\n\n\
   \"____\"___________20___р.                        Додаток на _____ аркушах\n",
iceb_t_getfioop(),
gbuh.inic,gbuh.famil.ravno(),
gbuh.inic,gbuh.famil.ravno());
                        
podpis(ff);


}
