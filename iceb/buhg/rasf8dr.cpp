/* $Id: rasf8dr.c,v 5.58 2013/08/13 05:49:53 sasa Exp $ */
/*31.12.2017	13.07.2000	Белых А.И.	rasf8dr.c
Вигрузка в *.dbf или *.lst файл формы 8ДР
*/
#include        <errno.h>
#include        "buhg.h"


#define         KSNL 63 /*Количество строк на листе*/
#define		SH1L 34 /*Шапка первого листа*/
#define		SHPL 6 /*Шапка последующих листов*/

extern int      kol_strok_na_liste;


int rasf8dr(const char *nomd,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
class spis_oth *oth)
{
int		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str inn("");
double		sumad,suman,isumad,isuman;
double		sumadn,sumann,isumadn,isumann;
class iceb_tu_str priz("");
short		lgota=0;
class iceb_tu_str lgotach("");
char		data1[32];
char		data2[32];
char		data11[32];
char		data21[32];
char		data12[32];
char		data22[32];
int		npp1;
char		imaf[64],imafdbf[64],imaffam[64];
FILE		*ff1=NULL,*ff4=NULL,*fffam=NULL;
FILE *ff_xml;
short		d,m,g;
int		kolr; /*Количество физических лиц*/
int 	klst=0; /*Количество строк*/
short		klls=1; /*Количество листов*/
short		god;
int		kvrt=0;
int		vidd;
class iceb_tu_str bros("");
class iceb_tu_str kod("");
short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
class iceb_tu_str priznak("");
static class iceb_tu_str kolih_v_htate("");
static class iceb_tu_str kolih_po_sovm("");

class SQLCURSOR curr;
char imaf_xml[512];
memset(imaf_xml,'\0',sizeof(imaf_xml));


/*Читаем шапку документа*/

sprintf(strsql,"select * from F8dr where nomd='%s'",nomd);
if(sql_readkey(&bd,strsql,&row,&curr) < 1)
 {
  iceb_t_soob(gettext("Не найден документ !"));
  return(1);
 } 
god=atoi(row[0]);
kvrt=atoi(row[1]);
vidd=atoi(row[3]);
kolih_v_htate.new_plus(row[10]);
kolih_po_sovm.new_plus(row[11]);

sprintf(imafdbf,"da0000%02d.%d",1,kvrt);
if(iceb_t_poldan("Имя DBF файла",&bros,"zarsdf1df.alx") == 0)
  sprintf(imafdbf,"%s%02d.%d",bros.ravno(),1,kvrt);

GDITE();

sprintf(strsql,"select inn,sumad,suman,priz,datap,datau,lgota,fio,sumadn,sumann,pr \
from F8dr1 where nomd='%s' order by inn asc,pr desc",nomd);

SQLCURSOR cur;
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

TYP=0;
class iceb_tu_str telef_kontr("");
class iceb_tu_str naim_kontr("");

sprintf(strsql,"select naikon,kod,telef from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  naim_kontr.new_plus(row[0]);
  kod.new_plus(row[1]);
  telef_kontr.new_plus(row[2]);
 }

if(iceb_t_poldan("Физическое лицо",&bros,"zarsdf1df.alx") == 0)
 if(SRAV(bros.ravno(),"Вкл",1) == 0)
  {
   TYP=1;
  }

kolrab(nomd,&kolr);


  VVOD VV(0);
  sprintf(strsql,"%s %d",gettext("Количество физических лиц"),kolr);
  VV.VVOD_SPISOK_add_ZAG(strsql);
  
  VV.VVOD_SPISOK_add_MD(gettext("Работало в штате.............."));
  VV.VVOD_SPISOK_add_MD(gettext("Работало по совместительству.."));

  VV.VVOD_SPISOK_add_data(kolih_v_htate.ravno(),20);
  VV.VVOD_SPISOK_add_data(kolih_po_sovm.ravno(),20); 

naz:;

   
  helstr(LINES-1,0,
//  "F1",gettext("помощь"),
  "F2/+",gettext("ввод"),
  "F4",gettext("очистить"),
  "F10",gettext("выход"),NULL);

  int kom=VV.vvod(0,1,1,-1,-1);

  move(LINES-1,0);

  kolih_v_htate.new_plus(VV.VVOD_SPISOK_return_data(0));
  kolih_po_sovm.new_plus(VV.VVOD_SPISOK_return_data(1));

  switch(kom)
   {
    case FK1:   /*Помощь*/
     GDITE();
//     prosf(strsql);
     goto naz;

    case ESC:
    case FK10:
     return(1);

    case FK2:
    case PLU:
       break;


    case FK4: /*Очистить меню*/
      VV.VVOD_clear_data();  
      goto naz;

    default:
       goto naz;
   }





sprintf(imaf,"dov%d_%d.lst",kvrt,getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaffam,"dovf%d_%d.lst",kvrt,getpid());
if((fffam = fopen(imaffam,"w")) == NULL)
 {
  error_op_nfil(imaffam,errno,"");
  return(1);
 }

//Заголовок распечаток

zagf1df(kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),kvrt,kod.ravno(),TYP,&klst,vidd,0,0,god,0,0,0,metka_oth,ff1,fffam);

spkvd(&klst,ff1);



creatheaddbf(imafdbf,kolstr+3);
if((ff4 = fopen(imafdbf,"a")) == NULL)
 {
  error_op_nfil(imafdbf,errno,"");
  return(1);
 }

f1df_xml_zag(kvrt,god,kod.ravno(),TYP,metka_oth,kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),naim_kontr.ravno(),imaf_xml,&ff_xml);


isumad=isuman=0.;
isumadn=isumann=0.;
npp1=0;
int kolstr1=0;
int nomer_xml_str=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
//  refresh();
  strzag(LINES-1,0,kolstr,++kolstr1);  
  inn.new_plus(row[0]);

  sumad=atof(row[1]);
  sumad=okrug(sumad,0.01);

  suman=atof(row[2]);
  suman=okrug(suman,0.01);

  sumadn=atof(row[8]);
  sumadn=okrug(sumadn,0.01);

  sumann=atof(row[9]);
  sumann=okrug(sumann,0.01);

  isumad+=sumad;
  isuman+=suman;

  isumadn+=sumadn;
  isumann+=sumann;

  priz.new_plus(row[3]);

  d=m=g=0;
  rsdat(&d,&m,&g,row[4],2);
  memset(data1,'\0',sizeof(data1));
  memset(data11,'\0',sizeof(data11));
  memset(data12,'\0',sizeof(data12));
  if(d != 0)
   {
    sprintf(data1,"%04d%02d%02d",g,m,d);
    sprintf(data11,"%02d.%02d.%4d",d,m,g);
    sprintf(data12,"%02d%02d%4d",d,m,g);
   }
  d=m=g=0;
  rsdat(&d,&m,&g,row[5],2);
  memset(data2,'\0',sizeof(data2));
  memset(data21,'\0',sizeof(data21));
  memset(data22,'\0',sizeof(data22));
  if(d != 0)
   {
    sprintf(data2,"%04d%02d%02d",g,m,d);
    sprintf(data21,"%02d.%02d.%04d",d,m,g);
    sprintf(data22,"%02d%02d%04d",d,m,g);
   }

  lgota=atoi(row[6]);

  if(lgota != 0)
   sprintf(strsql,"%02d",lgota);  
  else
   memset(strsql,'\0',sizeof(strsql));

  lgotach.new_plus(strsql);

  priznak.new_plus("");
  if(vidd != 0)
    priznak.new_plus(row[10]);

  ssf8dr(&klst,&klls,ff1);
  npp1++;    

    
  fprintf(ff1,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|\n",
  npp1,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data11,data21,lgotach.ravno(),priznak.ravno());

    //Узнаем инвентарный номер
    class iceb_tu_str fam("");    
    class iceb_tu_str tabn("");
    if(row[7][0] == '\0')
     {
      sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",row[0]);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
       {
        tabn.new_plus(row1[0]);
        fam.new_plus(row1[1]);
       }
     }
    else
     {
      fam.new_plus(row[7]);      
     }

  fprintf(fffam,"%-5d %-10s %10.2f %10.2f %10.2f %10.2f %-6s %10s %10s %-10s %6s %-5s %s\n",
  npp1,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data11,data21,lgotach.ravno(),priznak.ravno(),tabn.ravno(),fam.ravno());   
  zapf8dr(npp1,kvrt,god,kod.ravno(),TYP,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data1,data2,lgota,atoi(row[10]),ff4);

  f1df_xml_str(&nomer_xml_str,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data12,data22,lgotach.ravno(),row[10],ff_xml);
 }


/*узнаём военный сбор*/
double nah_dohod_vs=0.;
double vip_dohod_vs=0.;
double nah_vs=0.;
double vip_vs=0.;

sprintf(strsql,"select sfr,svs,sfrv,svsv from F1dfvs where nomd='%s'",nomd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  nah_dohod_vs+=atof(row[0]);
  nah_vs+=atof(row[1]);
  vip_dohod_vs+=atof(row[2]);
  vip_vs+=atof(row[3]);
    
 }

koncf8dr(kvrt,god,kod.ravno(),TYP,isumad,isumadn,isuman,isumann,isumad,isumadn,isuman,isumann,kolr,klls,&npp1,2,nah_dohod_vs,vip_dohod_vs,nah_vs,vip_vs,ff1,fffam,ff4);

fclose(ff1);
fclose(fffam);

fputc(26, ff4);
fclose(ff4);






f1df_xml_kon(isumadn,isumad,isumann,isuman,nomer_xml_str,kolr,klls,naim_kontr.ravno(),telef_kontr.ravno(),TYP,nah_dohod_vs,vip_dohod_vs,nah_vs,vip_vs,ff_xml);


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт формы 1ДФ"));

sprintf(strsql,"%s+%s",
gettext("Расчёт формы 1ДФ."),
gettext("Фамилия"));
oth->spis_naim.plus(strsql);
oth->spis_imaf.plus(imaffam);

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

oth->spis_naim.plus(gettext("Документ в формате dbf"));
oth->spis_imaf.plus(imafdbf);

oth->spis_imaf.plus(imaf_xml);
oth->spis_naim.plus(gettext("Документ в формате xml"));

return(0);
}

/***********************/
/*Счетчик строк        */
/***********************/

void	ssf8dr(int *klst,short *kollist,FILE *ff)
{
*klst+=1;
if(*klst < kol_strok_na_liste)
  return; 
fprintf(ff,"\f");
*klst=0;
spkvd(klst,ff);
*kollist+=1;
}
