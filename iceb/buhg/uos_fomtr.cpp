/*$Id: uos_fomtr.c,v 5.31 2013/08/13 05:49:56 sasa Exp $*/
/*16.07.2015	20.10.2005	Белых А.И.	uos_fomtr.c
Форма обліку матеріально-технічних ресурсів. Додаток 3 до наказу МВС України
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
int uos_fomtr_read_alx(class masiv_din_int *kodgr,class SPISOK *sp_uos_gr);
void uos_fomtr_sapka(short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstr,FILE *ff);
int uos_fomtr_prov(const char *grup,class masiv_din_int *kodgr,class SPISOK *sp_uos_gr);
void uos_fomtr_shet(short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstr,FILE *ff);

extern int kol_strok_na_liste;
extern short	startgoduos; /*Стартовый год*/

int uos_fomtr(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str inv_nom("");
int kom=1;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт формы учёта материально-технических ресурсов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала....(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.....(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер (,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(inv_nom.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
inv_nom.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK5:
     iceb_redfil("uos_fomtr.alx",0);
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
GDITE(); 

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

class masiv_din_int kodgr;
class SPISOK sp_uos_gr;

if(uos_fomtr_read_alx(&kodgr,&sp_uos_gr) != 0)
 goto naz;

char strsql[512];
int kolstr;
SQL_str row;
SQL_str row1;
SQLCURSOR cur;

//printw("Период %d.%d.%d > %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
//printw("%s\n",gettext("Сортируем записи."));

sprintf(strsql,"select innom,god,naim,datvv from Uosin");
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
FILE *ff;
class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosfomtr"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kodos int not null,\
innom int not null,\
podr int not null,\
god int not null,\
naim char(255) not null,\
datvv DATE not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

int kolstr1=0;
int kolstr2=0;
int kodos=0; //Код основного средства
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  
  if(proverka(inv_nom.ravno(),row[0],0,0) != 0)
   continue;
   
  sprintf(strsql,"select podr from Uosdok1 where innom=%s \
and datd >= '%04d-01-01' and datd <= '%04d-%d-%d' and podt=1 \
order by datd desc,tipz asc",
  row[0],startgoduos,gk,mk,dk);
  SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    /*Возможно инвентарный номер пришел позже заказанного периода*/
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {

//    poiinpd(atol(row[0]),mk,gk,shetu,hzt,hau,hna,&popkf,&soso,nomz,&m1,&g1,hnaby,&popkfby);
    poiinpd(atol(row[0]),mk,gk,&rekin);

    kodos=uos_fomtr_prov(rekin.hna.ravno(),&kodgr,&sp_uos_gr);
/************
    fprintf(ff,"%d|%s|%s|%s|%s|%s|\n",kodos,row[0],row1[0],row[1],row[2],row[3]);
*************/
  sprintf(strsql,"insert into %s values (%d,%d,%d,%d,'%s','%s')",
  imatmptab,
  kodos,atoi(row[0]),atoi(row1[0]),atoi(row[1]),row[2],row[3]);
   }

 }
GDITE(); 
SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;
sprintf(strsql,"select * from %s order by kodos asc,innom asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
char imaf[64];
sprintf(imaf,"uos_fomtr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

kolstr=0;
int podr=0;
long innom=0;
class iceb_tu_str naim("");
short god_iz;
short dvv,mvv,gvv;
int nomzap=0;
int kolstrok=0;
int kollist=0;

fprintf(ff,"\
                                Форма обліку матеріально-технічних ресурсів\n\
                                %s\n",iceb_t_get_pnk("00",0));
kolstrok=2;

uos_fomtr_sapka(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok,ff);
                                

class bsiz_data ost_in;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,++kolstr);
  kodos=atoi(rowtmp[0]);
  innom=atoi(rowtmp[1]);
  podr=atoi(rowtmp[2]);
  god_iz=atoi(rowtmp[3]);
  naim.new_plus(rowtmp[4]);
  rsdat(&dvv,&mvv,&gvv,rowtmp[5],2);
    
//  ostuos(innom,podr,dn,mn,gn,dk,mk,gk,0,"",&ost_in);
  bsizp(innom,podr,0,dn,mn,gn,dk,mk,gk,&ost_in,NULL);

  //Если начальная балынсовая стоимость, за период и конечная равны нолю - значит списан
  if(ost_in.nu.bs_nah_per == 0. && ost_in.nu.bs_kon_per == 0. && \
  ost_in.nu.bs_prih_za_per == 0. && ost_in.nu.bs_rash_za_per == 0.)
   continue;

//Счётчик строк для разбивки по листам закомментирован по просьбе клиента   
//  uos_fomtr_shet(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok,ff);

  fprintf(ff,"%-5d %-6d %-*.*s %7s ",
  ++nomzap,
  kodos,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  "");

  double spis_iznos=0.;
  double spis_bs=0.;
  if(ost_in.nu.bs_kon_per == 0.)
//  if(ost_in.nu.bs_rash_za_per != 0.)
   {
    spis_iznos=ost_in.nu.iznos_rash_za_per*-1;
    spis_bs=ost_in.nu.bs_nah_per-spis_iznos;
   }
  double prih_iz=0.;
  double prih_bs=0.;
  if(ost_in.nu.bs_nah_per == 0.)
   {
    prih_bs=ost_in.nu.bs_prih_za_per;
    prih_iz=ost_in.nu.iznos_prih_za_per;
   }
  double iznos_kon=ost_in.nu.izmen_iznosa_kon_per+ost_in.nu.amort_kon_per;
  if(ost_in.nu.bs_kon_per == 0.)
    iznos_kon=0.;
    
  if(ost_in.nu.bs_nah_per != 0. || ost_in.nu.izmen_iznosa_nah_per+ost_in.nu.amort_nah_per != 0.)
    fprintf(ff,"\
%-10d %02d.%02d.%04d %10.2f %10.2f \
%10s %10s %10.f %10.2f \
%10.2f %10.2f %10s %10s \
%10.2f %10.2f\n",
    god_iz,dvv,mvv,gvv,ost_in.nu.bs_nah_per,ost_in.nu.izmen_iznosa_nah_per+ost_in.nu.amort_nah_per,
    "","", prih_bs,prih_iz,
    spis_bs, spis_iznos,"","",
    ost_in.nu.bs_kon_per,iznos_kon);
  else
    fprintf(ff,"\
%-10s %10s %10s %10s \
%-10d %02d.%02d.%04d %10.f %10.2f \
%10.2f %10.2f %10s %10s \
%10.2f %10.2f\n",
    "","","","",
    god_iz,dvv,mvv,gvv, prih_bs,prih_iz,
    spis_bs, spis_iznos,"","",
    ost_in.nu.bs_kon_per,iznos_kon);

 }

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт формы учёта материально-технических ресурсов"));
iceb_t_ustpeh(imaf,3);
return(0);
}


/************************************/
/*Чтение настроечного файла*/
/****************************/
int uos_fomtr_read_alx(class masiv_din_int *kodgr,class SPISOK *sp_uos_gr)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='uos_fomtr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uos_fomtr.alx");
  iceb_t_soob(strsql);
  return(1);
 }



class iceb_tu_str stroka_kodgr("");
class iceb_tu_str kod_gr("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&kod_gr,1,'|') != 0)
   continue;
  if(kod_gr.ravno_atoi() == 0)
   continue;
   
  if(polen(row_alx[0],&stroka_kodgr,3,'|') != 0)
   continue;
   
  kodgr->plus(atoi(kod_gr.ravno()),-1);
  sp_uos_gr->plus(stroka_kodgr.ravno());
 }
 
return(0);
}


/***********************************/
/*Проверка принадлежности*/
/***********************************/

int uos_fomtr_prov(const char *grup,
class masiv_din_int *kodgr,class SPISOK *sp_uos_gr)
{
//printw("\ngrup=%s\n",grup);

for(int i=0; i < kodgr->kolih(); i++)
 {
//  printw("%d/%d=%s\n",i,kodgr->ravno(i),sp_uos_gr->ravno(i));
  if(proverka(sp_uos_gr->ravno(i),grup,0,1) == 0)
   return(kodgr->ravno(i));

 }

return(0);
}
/***********************/
/*Счётчик строк */
/*********************/
void uos_fomtr_shet(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,
int *kolstr,
FILE *ff)
{
*kolstr+=1;
if(*kolstr <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kolstr=1;
uos_fomtr_sapka(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);


}
/***************************************/
/*Шапка документа*/
/***************************************/

void uos_fomtr_sapka(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,
int *kolstr,
FILE *ff)
{

*kollist+=1;
*kolstr+=10;

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
     | Код  |           Повне              |Норма- |                                           |                                           |                  Вибуло                   |                     |        |\n\
     |основ-|        найменування          |тивний |         Залишок на %02d.%02d.%04d             |            Надходження                    |-------------------------------------------|Залишок на %02d.%02d.%04d|        |\n\
     |ного  |         тип,марка            |термін |                                           |                                           |       Списано       |      Передано       |                     |        |\n\
     |засобу|                              |експлу-|---------------------------------------------------------------------------------------------------------------------------------------------------------|        |\n\
 Н/п |      |                              |атації |Дата виго-|Дата вве- |Первісна  |  Сума    |Дата виго-|Дата вве- |Первісна  |  Сума    |Балансова |  Сума    |Первісна  |  Сума    |Первісна  |  Сума    |Примітка|\n\
     |      |                              |(років)| товлення |дення в   |вартість  | зносу    | товлення |дення в   |вартість  | зносу    |вартість  |  зносу   |вартість  | зносу    |вартість  | зносу    |        |\n\
     |      |                              |       |          |експлуата-|  грн.    |  грн.    |          |експлуата-|  грн.    |  грн.    |   грн.   |   грн.   |  грн.    |  грн.    |  грн.    |  грн.    |        |\n\
     |      |                              |       |          |  цію     |          |          |          |  цію     |          |          |          |          |          |          |          |          |        |\n\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
dn,mn,gn,dk,mk,gk);

/*
12345 123456 123456789012345678901234567890 1234567 1234567890 1234567890 
*/

}
