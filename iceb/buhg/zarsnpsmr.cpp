/*$Id:$*/
/*28.03.2018	05.02.2015	Белых А.И.	zarsnpsmr.c
Свод начислений по счетам, месяцам, работникам
*/
#include "buhg.h"

class zarsnpsmr_rp
 {
  public:
   class iceb_tu_str datn;
   class iceb_tu_str datk;
   class iceb_tu_str shet;
   class iceb_tu_str tabnom;
   class iceb_tu_str podr;   
   zarsnpsmr_rp()
    {
     clear_rp();
    }
   void clear_rp()
    {
     datn.new_plus("");
     datk.new_plus("");
     shet.new_plus("");
     tabnom.new_plus("");
     podr.new_plus("");
    }
 };

extern short mmm,ggg;

int zarsnpsmr(class spis_oth *oth)
{
static class zarsnpsmr_rp rp;
int kom1=0;
char strsql[1024];
int tabn=0;
class iceb_tu_str tabnom("");

if(rp.datn.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  rp.datn.new_plus(strsql);
 }

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(rp.datn.ravno(),8); 
VV.VVOD_SPISOK_add_data(rp.datk.ravno(),8); 
VV.VVOD_SPISOK_add_data(rp.shet.ravno(),128); 
VV.VVOD_SPISOK_add_data(rp.tabnom.ravno(),128); 
VV.VVOD_SPISOK_add_data(rp.podr.ravno(),128); 

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и по счетам, месяцам, работникам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала периода..(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца периода...(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт.................(,,)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер......(,,)..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения....(,,)..."));//4

naz:;

clear();



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int rkka=VV.vvod(0,1,1,-1,-1);

rp.datn.new_plus(VV.data_ravno(0));
rp.datk.new_plus(VV.data_ravno(1));
rp.shet.new_plus(VV.data_ravno(2));
rp.tabnom.new_plus(VV.data_ravno(3));
rp.podr.new_plus(VV.data_ravno(4));

class iceb_tu_str kodv("");
class iceb_tu_str naim("");
switch(rkka)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("zarp4_3_6.txt");
   clear();
   goto naz;
   
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр плана счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
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
      
        if(dirtab(&tabn,&naim,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
         }
        goto naz;

      case 1 :
        vibrek("Plansh",&naim);
        VV.VVOD_SPISOK_zapis_data(2,naim.ravno());
        goto naz;

      case 2 :
        if(dirzarpodr(2,&kodv,&naim) == 0)
         VV.data_z_plus(4,kodv.ravno());
        goto naz;
     }  

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }
short mn=0,gn=0;
short mk=0,gk=0;
if(rsdat1(&mn,&gn,rp.datn.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(rp.datk.ravno()[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,rp.datk.ravno()) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }

int kolih_mes=period(1,mn,gn,1,mk,gk,1);

SQL_str row,row1;
class SQLCURSOR cur,cur1;

int kolstr=0;
sprintf(strsql,"select datz,tabn,suma,shet,podr from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and prn='1' and suma <> 0. order by datz asc",gn,mn,gk,mk);


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

class iceb_t_tmptab tabtmp;
const char *imatmptab={"zarsnpsmr"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shet char(32) not null default '',\
data DATE NOT NULL DEFAULT '0000-00-00',\
tn INT(10) NOT NULL DEFAULT 0,\
sum double(12,2) not null DEFAULT 0.,\
unique (shet,data,tn)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

int kolstr1=0;
short d=0,m=0,g=0;
//clear();
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(rp.shet.ravno(),row[3],0,0) != 0)
   continue;

  if(proverka(rp.tabnom.ravno(),row[1],0,0) != 0)
   continue;

  if(proverka(rp.podr.ravno(),row[4],0,0) != 0)
   continue;

  rsdat(&d,&m,&g,row[0],2);
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
//  refresh();  
  sprintf(strsql,"select sum from %s where shet='%s' and data='%04d-%02d-01' and tn=%s",imatmptab,row[3],g,m,row[1]);
  
  if(readkey(strsql,&row1,&cur1) == 0)
    sprintf(strsql,"insert into %s values('%s','%04d-%02d-01',%s,%s)",imatmptab,row[3],g,m,row[1],row[2]);
  else
   {
    
    sprintf(strsql,"update %s set sum=%.2f where shet='%s' and data='%04d-%02d-01' and tn=%s",imatmptab,atof(row[2])+atof(row1[0]),row[3],g,m,row[1]);
   }
//  printw("%s\n",strsql);    
  sql_zapis(strsql,1,0);  

 }
//OSTANOV();

sprintf(strsql,"select distinct shet,tn from %s order by shet asc,tn asc",imatmptab);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи во временной таблице!"));
  return(1);
 }
class iceb_fopen fil;
char imaf[64];
sprintf(imaf,"zarsn%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод начислений"),1,mn,gn,31,mk,gk,fil.ff);
 
if(rp.shet.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Счёт"),rp.shet.ravno());
if(rp.tabnom.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Табельный номер"),rp.tabnom.ravno());

if(rp.podr.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Табельный номер"),rp.podr.ravno());
 
double str_suma[kolih_mes];
double itogo_shet[kolih_mes];
double itogo[kolih_mes];

kolstr1=0;
class iceb_tu_str shetz("");
class iceb_tu_str fio("");

class iceb_tu_str liniq("--------------------------------------");

for(int nom=0; nom < kolih_mes+1; nom++)
 liniq.plus("------------");

fprintf(fil.ff,"%s\n",liniq.ravno());
fprintf(fil.ff,"%s",gettext(" Т/н  |Фамилия, Имя, Отчество        |"));
d=1;
m=mn;
g=gn;

for(int nom=0; nom < kolih_mes; nom++)
 {
  sprintf(strsql,"%02d.%04d",m,g);
  fprintf(fil.ff,"%11s|",strsql);
  dpm(&d,&m,&g,3);
 }
fprintf(fil.ff,"%-*s|\n",iceb_tu_kolbait(11,gettext("Итого")),gettext("Итого"));
fprintf(fil.ff,"%s\n",liniq.ravno());
memset(itogo_shet,'\0',sizeof(itogo_shet));
memset(itogo,'\0',sizeof(itogo));
double itogo_str=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(SRAV(shetz.ravno(),row[0],0) != 0)
   {
    if(shetz.getdlinna() > 1)
     {
      itogo_str=0.;
      fprintf(fil.ff,"%s\n%*s|",liniq.ravno(),iceb_tu_kolbait(37,gettext("Итого по счёту")),gettext("Итого по счёту"));
      for(int nom=0; nom < kolih_mes; nom++)
       {
        fprintf(fil.ff,"%11.2f|",itogo_shet[nom]);
        itogo_str+=itogo_shet[nom];
       }
      fprintf(fil.ff,"%11.2f|\n",itogo_str);
     }

    sprintf(strsql,"select nais from Plansh where ns='%s'",row[0]);
    if(readkey(strsql,&row1,&cur1) >= 1)
     fio.new_plus(row1[0]);
    else
     fio.new_plus("");
         
    fprintf(fil.ff,"%s:%s %s\n",gettext("Счёт"),row[0],fio.ravno());

    shetz.new_plus(row[0]);
    memset(itogo_shet,'\0',sizeof(itogo_shet));
   }
  memset(str_suma,'\0',sizeof(str_suma));

  d=1;
  m=mn;
  g=gn;

  for(int nom=0; nom < kolih_mes; nom++)
   {
    sprintf(strsql,"select sum from %s where data='%04d-%02d-01' and shet='%s' and tn=%s",imatmptab,g,m,row[0],row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
      str_suma[nom]+=atof(row1[0]);         
    dpm(&d,&m,&g,3);
   }
  for(int nom=0; nom < kolih_mes; nom++)
   {
    itogo_shet[nom]+=str_suma[nom];
    itogo[nom]+=str_suma[nom];
   }
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  else
   fio.new_plus("");
  
  fprintf(fil.ff,"%6s|%-*.*s|",row[1],iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno());
  itogo_str=0.;
  for(int nom=0; nom < kolih_mes; nom++)
   {
    fprintf(fil.ff,"%11.2f|",str_suma[nom]);
    itogo_str+=str_suma[nom];
   }
  fprintf(fil.ff,"%11.2f|\n",itogo_str);  
 }
fprintf(fil.ff,"%s\n%*s|",liniq.ravno(),iceb_tu_kolbait(37,gettext("Итого по счёту")),gettext("Итого по счёту"));
itogo_str=0.;
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(fil.ff,"%11.2f|",itogo_shet[nom]);
  itogo_str+=itogo_shet[nom];
 }
fprintf(fil.ff,"%11.2f|\n",itogo_str);

fprintf(fil.ff,"%s\n%*s|",liniq.ravno(),iceb_tu_kolbait(37,gettext("Итого")),gettext("Итого"));
itogo_str=0.;
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(fil.ff,"%11.2f|",itogo[nom]);
  itogo_str+=itogo[nom];
 }
fprintf(fil.ff,"%11.2f|\n",itogo_str);


fprintf(fil.ff,"%s\n",liniq.ravno());
podpis(fil.ff);

fil.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод начислений по счетам, месяцам, работникам"));

iceb_t_ustpeh(imaf,3);

return(0);

}
