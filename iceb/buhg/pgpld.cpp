/* $Id: pgpld.c,v 5.34 2013-01-02 10:38:36 sasa Exp $ */
/*15.07.2015	11.05.1998	Белых А.И.	pgpld.c
Распечатка журнала платежных поручений
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

void pgpld_hap1(int nomstr,int *kolstr,FILE *ff);
void pgpld_hap2(int nomstr,int *kolstr,FILE *ff);

extern class REC rec;

int pgpld(const char *tabl,class spis_oth *oth)
{
short		kom;
short		d,m,g;
short		dn,mn,gn;
short		dk,mk,gk;
FILE		*ff,*ff1;
char		imaf[64],imaf1[64];
double		suma,suma1;
static class iceb_tu_str kor("");
long		kolstr=0,kolstr1=0,kolstr2=0;
SQL_str         row,row1;
char		strsql[512];
double		sum,usl;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kodop("");
int kom1=0;
class iceb_tu_str kod("");

VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента (,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)..."));

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка журнала платёжных документов"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kor.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kor.new_plus(VV.data_ravno(2));
kodop.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("dokum2_5.txt");
   goto naz;

  case FK10:
  case ESC:
    return(1);

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;        
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        clear();
        kod.new_plus(VV.data_ravno(2));
        if(vibrek("Kontragent",&kod) == 0)
         VV.data_plus(2,kod.ravno());
 
        break;

      case 1 :
        clear();
        kod.new_plus(VV.data_ravno(3));
        if(vibrek("Opltp",&kod) == 0)
         VV.data_plus(3,kod.ravno());
        break;

     }

    clear();
    goto naz;


  case FK2:
  case PLU:
    break;
  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;
  
move(LINES-1,0);
GDITE();


SQLCURSOR cur;
SQLCURSOR cur1;

sprintf(strsql,"select datd,nomd,suma,uslb,polu,oper from %s where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc",
tabl,gn,mn,dn,gk,mk,dk);
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
class iceb_tu_str edrpou("");
class iceb_tu_str rasshet("");
class iceb_tu_str mfo("");
class iceb_tu_str naimbank("");

sprintf(imaf,"gp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }
sprintf(imaf1,"gpa%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
   {
    error_op_nfil(imaf1,errno,"");
    return(1);
   }
if(SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"%s",gettext("Журнал регистрации платежных поручений"));

if(SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"%s",gettext("Журнал регистрации платежных требований"));

iceb_t_zagolov(strsql,1,mn,gn,dk,mk,gk,ff);
iceb_t_zagolov(strsql,1,mn,gn,dk,mk,gk,ff1);

if(kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Код операции"),kodop.ravno());
 }
if(kor.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kor.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Код контрагента"),kor.ravno());
 } 
int kol=0;
pgpld_hap1(1,&kol,ff);
pgpld_hap2(1,&kol,ff1);

suma1=suma=0.;

printw("%d.%d.%d > %d.%d.%d %s\n",dn,mn,gn,dk,mk,gk,kor.ravno());
refresh();

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);   
  
  if(proverka(kodop.ravno(),row[5],0,0) != 0)
    continue;

  polen(row[4],&rec.kodor1,1,'#');
  if(proverka(kor.ravno(),rec.kodor1.ravno(),0,0) != 0)
    continue;
      
  polen(row[4],&rec.naior1,2,'#');

  polen(row[4],&edrpou,4,'#');
  polen(row[4],&naimbank,5,'#');
  polen(row[4],&mfo,6,'#');
  polen(row[4],&rasshet,7,'#');


  rsdat(&d,&m,&g,row[0],2);
  sum=atof(row[2]);
  usl=atof(row[3]);
  suma+=sum;
  suma1+=usl;
  
  fprintf(ff,"%-*s %-*s %02d.%02d.%d %10.2f/%4.2f   %s\n",
  iceb_tu_kolbait(6,row[1]),row[1],
  iceb_tu_kolbait(4,row[5]),row[5],
  d,m,g,sum,usl,rec.naior1.ravno());

  fprintf(ff1,"%02d.%02d.%04d|%-*s|%10.2f|%-*s|%-*s|%-*s|%-*.*s|",
  d,m,g,
  iceb_tu_kolbait(6,row[1]),row[1],
  sum,
  iceb_tu_kolbait(8,mfo.ravno()),mfo.ravno(),
  iceb_tu_kolbait(20,rasshet.ravno()),rasshet.ravno(),
  iceb_tu_kolbait(10,edrpou.ravno()),edrpou.ravno(),
  iceb_tu_kolbait(30,rec.naior1.ravno()),iceb_tu_kolbait(30,rec.naior1.ravno()),rec.naior1.ravno());

  sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
  nomd='%s' and mz=0 order by nz asc",tabl,g,m,d,row[1]);
  /*printw("\n%s",strsql);*/
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 != 0)
   {
    int nomstr=0;
    while(cur1.read_cursor(&row1) != 0)
     {
  
      if(row1[0][0] != '\0')
       {
        fprintf(ff,"%37s %-*.*s\n"," ",iceb_tu_kolbait(45,row1[0]),iceb_tu_kolbait(45,row1[0]),row1[0]);
        if(nomstr == 0)
          fprintf(ff1,"%-*.*s\n",iceb_tu_kolbait(45,row1[0]),iceb_tu_kolbait(45,row1[0]),row1[0]);
        else
         {
          if(nomstr == 1)
           fprintf(ff1,"\
          |      |          |        |                    |          |%-*.*s|%-*.*s\n",
           iceb_tu_kolbait(30,iceb_tu_adrsimv(30,rec.naior1.ravno())),
           iceb_tu_kolbait(30,iceb_tu_adrsimv(30,rec.naior1.ravno())),
           iceb_tu_adrsimv(30,rec.naior1.ravno()),
           iceb_tu_kolbait(45,row1[0]),iceb_tu_kolbait(45,row1[0]),row1[0]);
         else
           fprintf(ff1,"\
          |      |          |        |                    |          |                              |%-*.*s\n",
           iceb_tu_kolbait(45,row1[0]),iceb_tu_kolbait(45,row1[0]),row1[0]);
         }
        nomstr++;
       }
          
      if(iceb_tu_strlen(row1[0]) > 45)
       {
        fprintf(ff,"%37s %-*.*s\n"," ",
        iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
        iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
        iceb_tu_adrsimv(45,row1[0]));

        if(nomstr == 1)
          fprintf(ff1,"\
          |      |          |        |                    |          |%-*.*s|%-*.*s\n",
          iceb_tu_kolbait(30,iceb_tu_adrsimv(30,rec.naior1.ravno())),
          iceb_tu_kolbait(30,iceb_tu_adrsimv(30,rec.naior1.ravno())),
          iceb_tu_adrsimv(30,rec.naior1.ravno()),
          iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
          iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
          iceb_tu_adrsimv(45,row1[0]));
        else
          fprintf(ff1,"\
          |      |          |        |                    |          |                              |%-*.*s\n",
          iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
          iceb_tu_kolbait(45,iceb_tu_adrsimv(45,row1[0])),
          iceb_tu_adrsimv(45,row1[0]));

        nomstr++;

       }      
     }
   }
  else
   fprintf(ff1,"\n");
 }

fprintf(ff1,"ICEB_LST_END\n\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f\n",iceb_tu_kolbait(17,gettext("Итого")),gettext("Итого"),suma);

fprintf(ff,"ICEB_LST_END\n\
---------------------------------------------------------------------------\n\
%*s: %10.2f/%4.2f\n",iceb_tu_kolbait(21,gettext("Итого")),gettext("Итого"),suma,suma1);

podpis(ff);

fclose(ff);

podpis(ff1);

fclose(ff1);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка журнала платежных документов"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Распечатка журнала платежных документов"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 {
  kol=iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
  if(nom == 0)
    iceb_t_rnl(oth->spis_imaf.ravno(nom),kol,NULL,&pgpld_hap1);
  if(nom == 1)
    iceb_t_rnl(oth->spis_imaf.ravno(nom),kol,NULL,&pgpld_hap2);
 } 
return(0);
}
/*************************************************/
void pgpld_hap1(int nomstr,int *kolstr,FILE *ff)
{
*kolstr=4;
fprintf(ff,"%60s %s N%d\n","",gettext("Страница"),nomstr);
fprintf(ff,"\
--------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Н.док.|К.о.|   Дата    |      Сумма     |  Наименование контрагента / Коментарий\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------\n");
}
/*********************************************/
void pgpld_hap2(int nomstr,int *kolstr,FILE *ff)
{
*kolstr=4;
fprintf(ff,"%120s %s N%d\n","",gettext("Страница"),nomstr);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
   Дата   |Ном.д.|  Cумма   |  МФО   |Расчётный счёт      |EГРПОУ    |        Контрагент            |   Назначение платежа\n");
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
/********
1234567890 123456 1234567890 12345678 12345678901234567890 1234567890 123456789012345678901234567890
*******/
}