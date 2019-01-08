/*$Id:$*/
/*05.08.2015	05.08.2015	Белых А.И.	xukropp.c
Распечатка отчёта по контрагентам поставщикам услуг 
*/

#include "buhg.h"

void xukropp_shdok(int noml,int *kolstr,FILE *ff);
int xukropp_menu(class xukropp_poi *poi);
void xukropp_rp(class xukropp_poi *poi,FILE *ff);

class xukropp_poi
 {
  public:
  class iceb_tu_str datan;
  class iceb_tu_str datak;
  class iceb_tu_str kodkr;
  int metka_fop; /*0-все 1-только физицеские лица пердприниматели*/
  
  xukropp_poi()
   {
    clean();
   }

  void clean()
   {
    metka_fop=0;
    datan.new_plus("");
    datak.new_plus("");
    kodkr.new_plus("");
   }
 };
 

int xukropp(class spis_oth *oth)
{
time_t          tmmn;
static class xukropp_poi poi;


naz:;


if(xukropp_menu(&poi) != 0)
 return(1);


short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,poi.datan.ravno(),&dk,&mk,&gk,poi.datak.ravno()) != 0)
 goto naz;
clear();
time(&tmmn);

char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

sprintf(strsql,"select datd,nomd,kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd >= '%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d' and kdrnn <> ''",gn,mn,dn,gk,mk,dk);

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
char imaf_dok[64];
sprintf(imaf_dok,"xukropp%d.lst",getpid());

class iceb_fopen fil_dok;
if(fil_dok.start(imaf_dok,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Реестр командировочных расходов по поставщикам"),dn,mn,gn,dk,mk,gk,fil_dok.ff);

xukropp_rp(&poi,fil_dok.ff);

xukropp_shdok(1,NULL,fil_dok.ff);

class masiv_din_int sp_kodkr; /*список кодов командировочных расходов*/
class SPISOK sp_kontr; /*список кодов контрагентов*/

class SPISOK sp_kontr_kodkr; /*список кодов контрагентов|кодов командировочных расходов*/
class masiv_din_double sp_suma; /*суммы по кодам контрагентов|командировочных расходов*/
int nom_v_sp=0;
double suma=0.;
int kolstr1=0;
double itogo_kolih=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(poi.kodkr.ravno(),row[2],0,0) != 0)
   continue;

  if(poi.metka_fop == 1) /*только физические лица*/
   {
    sprintf(strsql,"select regnom from Kontragent where kodkon='%s'",row[7]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(row1[0][0] == '\0')
       continue;    
     }
   }

  itogo_kolih+=atof(row[3]);
  suma=atof(row[4])+atof(row[6]);

  if(sp_kodkr.find(row[2]) < 0)
   sp_kodkr.plus(row[2]);

  if(sp_kontr.find_r(row[7]) < 0)
   sp_kontr.plus(row[7]);

  sprintf(strsql,"%s|%s",row[7],row[2]);

  if((nom_v_sp=sp_kontr_kodkr.find_r(strsql)) < 0)
    sp_kontr_kodkr.plus(strsql);  
  sp_suma.plus(suma,nom_v_sp);
  

  printw("%s %-*s %3s %5.5g %10.2f %-*s %s %s\n",
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(10,row[1]),
  row[1],
  row[2],
  atof(row[3]),
  suma,
  iceb_tu_kolbait(6,row[5]),
  row[5],
  row[7],
  iceb_t_get_pnk(row[7],0));  

  fprintf(fil_dok.ff,"%s %-*s %8s %6.6g %10.2f %-*s %-*s %s\n",
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(6,row[1]),
  row[1],
  row[2],
  atof(row[3]),
  suma,
  iceb_tu_kolbait(6,row[5]),
  row[5],
  iceb_tu_kolbait(6,row[7]),
  row[7],
  iceb_t_get_pnk(row[7],0));  


 }
fprintf(fil_dok.ff,"\
-----------------------------------------------------------------------------------\n");
fprintf(fil_dok.ff,"%*s:%6.6g %10.2f\n",
iceb_tu_kolbait(26,gettext("Итого")),
gettext("Итого"),
itogo_kolih,sp_suma.suma());

fprintf(fil_dok.ff,"ICEB_LST_END\n");
podpis(fil_dok.ff);
fil_dok.end();


/*распечатываем свод*/
char imaf_svod[64];
sprintf(imaf_svod,"xukropps%d.lst",getpid());
if(fil_dok.start(imaf_svod,"w") != 0)
 return(1);


iceb_t_zagolov(gettext("Свод командировочных расходов по поставщикам"),dn,mn,gn,dk,mk,gk,fil_dok.ff);

xukropp_rp(&poi,fil_dok.ff);

class iceb_tu_str kod("");
class iceb_tu_str naim("");

class iceb_tu_str liniq("----------------------------------------------------");
for(int nom=0; nom <= sp_kodkr.kolih(); nom++)
 liniq.plus("-----------");

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

fprintf(fil_dok.ff,"%-*.*s|%-*.*s|%-*.*s|",
iceb_tu_kolbait(27,gettext("Контрагент поставщик услуг")),
iceb_tu_kolbait(27,gettext("Контрагент поставщик услуг")),
gettext("Контрагент поставщик услуг"),
iceb_tu_kolbait(12,gettext("Инд.нал.ном.")),
iceb_tu_kolbait(12,gettext("Инд.нал.ном.")),
gettext("Инд.нал.ном."),
iceb_tu_kolbait(10,gettext("Код ЕГРПОУ")),
iceb_tu_kolbait(10,gettext("Код ЕГРПОУ")),
gettext("Код ЕГРПОУ"));

for(int nom=0; nom < sp_kodkr.kolih(); nom++)
 {
  sprintf(strsql,"select naim from Ukrkras where kod=%d",sp_kodkr.ravno(nom));
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%d %s",sp_kodkr.ravno(nom),naim.ravno());
  
  fprintf(fil_dok.ff,"%-*.*s|",
  iceb_tu_kolbait(10,strsql),
  iceb_tu_kolbait(10,strsql),
  strsql);
  
 }
fprintf(fil_dok.ff,"%-*.*s|\n",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

double itogo_str=0.;
double itogo_kol[sp_kodkr.kolih()];
class iceb_tu_str inn("");
class iceb_tu_str egrpou("");
for(int nom_kontr=0; nom_kontr < sp_kontr.kolih(); nom_kontr++)
 {
  kod.new_plus(sp_kontr.ravno(nom_kontr));
  sprintf(strsql,"select naikon,kod,innn from Kontragent where kodkon='%s'",kod.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    egrpou.new_plus(row[1]);
    inn.new_plus(row[2]);
   }
  else
   {
    naim.new_plus("");
    egrpou.new_plus("");
    inn.new_plus("");
   }
    
  fprintf(fil_dok.ff,"%-*s %-*.*s|%-*s|%-*s|",
  iceb_tu_kolbait(6,kod.ravno()),
  kod.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),
  iceb_tu_kolbait(20,naim.ravno()),
  naim.ravno(),
  iceb_tu_kolbait(12,inn.ravno()),
  inn.ravno(),
  iceb_tu_kolbait(10,egrpou.ravno()),
  egrpou.ravno());

  itogo_str=0.;
  for(int nom_kodkr=0; nom_kodkr < sp_kodkr.kolih(); nom_kodkr++)
   {
    sprintf(strsql,"%s|%d",kod.ravno(),sp_kodkr.ravno(nom_kodkr));
    if((nom_v_sp=sp_kontr_kodkr.find_r(strsql)) < 0)
     {
      fprintf(fil_dok.ff,"%10s|","");
     }
    else
     {
      fprintf(fil_dok.ff,"%10.2f|",sp_suma.ravno(nom_v_sp));
      itogo_str+=sp_suma.ravno(nom_v_sp);
      itogo_kol[nom_kodkr]+=sp_suma.ravno(nom_v_sp);
     }    

   }
  fprintf(fil_dok.ff,"%10.2f|\n",itogo_str);
 }

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

fprintf(fil_dok.ff,"%*.*s|",
iceb_tu_kolbait(51,gettext("Итого")),
iceb_tu_kolbait(51,gettext("Итого")),
gettext("Итого"));

for(int nom=0; nom < sp_kodkr.kolih(); nom++)
 fprintf(fil_dok.ff,"%10.2f|",itogo_kol[nom]);


fprintf(fil_dok.ff,"%10.2f|\n",sp_suma.suma());

podpis(fil_dok.ff);

fil_dok.end();


oth->spis_imaf.plus(imaf_dok);
oth->spis_naim.plus(gettext("Реестр командировочных расходов по поставщикам"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),3,&kolstr);
iceb_t_rnl(oth->spis_imaf.ravno(0),kolstr,"",&xukropp_shdok);

oth->spis_imaf.plus(imaf_svod);
oth->spis_naim.plus(gettext("Свод комадировочных расходов по поставщикам"));
iceb_t_ustpeh(oth->spis_imaf.ravno(1),3);

printw_vr(tmmn);

OSTANOV();

return(0);

}
/************************************************/
/*шапка*/
/**************************************************/
void xukropp_shdok(int noml,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr=*kolstr+4;
 
fprintf(ff,"%60s %s:%d\n","",gettext("Страница"),noml);

fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Дата доку.|Ном.д.|Код рас.|Колич.|  Сумма   |Ед.из.|Контрагент поставщик\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

}
/**************************************/
int xukropp_menu(class xukropp_poi *poi)
{
int N=0,K=0;
class VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка по поставщикам услуг"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код расхода....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Вид контрагента......"));//3

VV.VVOD_SPISOK_add_data(poi->datan.ravno(),11);
VV.VVOD_SPISOK_add_data(poi->datak.ravno(),11);
VV.VVOD_SPISOK_add_data(poi->kodkr.ravno(),128);
VV.VVOD_SPISOK_add_data(40);



naz:;
if(poi->metka_fop == 0)
 VV.data_plus(3,gettext("Все"));
else
 VV.data_plus(3,gettext("Только физические лица"));


clear(); /*Очистить экран и закрасить фоновым цветом*/


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("код расхода"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw(N,stdscr);

    if(N == 3) /*вид контрагента*/
     {
      class VVOD MENU(3);

      MENU.VVOD_SPISOK_add_MD(gettext("Все"));
      MENU.VVOD_SPISOK_add_MD(gettext("Только физические лица"));

      while(menu3w(stdscr,&MENU,&poi->metka_fop,-1,-1,0) != 0);



      goto naz;

     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Количество*/
     {
     }


    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

  if(K == ESC || K == FK10)
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     return(1);
     break;
   }
  if(K == FK1)
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     GDITE();
//      iceb_t_pdoc("usl2_1_1_2.txt");
     clear();
     goto naz;
   }
  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
     
    poi->datan.new_plus(VV.VVOD_SPISOK_return_data(0));
    poi->datak.new_plus(VV.VVOD_SPISOK_return_data(1));
    poi->kodkr.new_plus(VV.VVOD_SPISOK_return_data(2));
        
    return(0);     
   
   }
  if(K == FK3) /*Код расхода*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    class iceb_tu_str kod("");
    class iceb_tu_str naim("");
    if(dirkkr(&kod,&naim,1) == 0)
     {
      VV.data_z_plus(2,kod.ravno());
     }

   
    goto naz;
   }

  if(K == FK4) /*Код расхода*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.VVOD_clear_data();  
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();


return(0);
}
/**********************************/
void xukropp_rp(class xukropp_poi *poi,FILE *ff)
{
if(poi->metka_fop == 1)
 fprintf(ff,"%s\n",gettext("Только физические лица"));
if(poi->kodkr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код командировочных расходов"),poi->kodkr.ravno()); 
}
