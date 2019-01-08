/*$Id: uosinved.c,v 1.63 2013/09/26 09:43:42 sasa Exp $*/
/*15.11.2017	12.11.2001	Белых А.И.	uosinved.c
Рапечатка инвентаризационной ведомости в учете основных 
средств
*/
#include	<errno.h>
#include	"buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

class drag_met
 {
  public:
  class SPISOK dm_ei; /*Список драг металлов-единиц измерения*/
  class masiv_din_double ves_dm; /*Массив для итоговых весов по горизонтали*/
  class masiv_din_double ves_dmv; /*Массив для итоговых весов по вертикали*/
  class masiv_din_int sp_in; /*Список инвентарных номеров*/
  class masiv_din_double ves_hax; /*Шахматка основное средство-вес*/
 };

void	uosinsap(int*,int*,FILE*);
void  uosinvedsh(int *kolst,int *kollist,int *kolstr_ff_gos,int *kollist_ff_gos,FILE *ff,FILE *ff_gos);
void uosinved_sbu(short dost,short most,short gost,int *kollist,int *kolst,FILE *ff);
void uosinved_pl(short di,short mi,short gi,const char *ikod,int *kollist,int *kolstr,FILE *ffbu);
void  uosinvedsh_bu(short di,short mi,short gi,int *kolst,int *kollist,FILE *ff);
void uosinved_end_bu(FILE *ffbu);
void uosinved_sap(short mi,short gi,int podr,int kodotl,int *kollist,int *kolst,int *kollist_ff_gos,int *kolstr_ff_gos,FILE *ff,FILE *ff_gos);
void uosinved_ips(short di,short mi,short gi,const char *shetuz,int *kolst,int *kollist,int *kostr_ff_gos,int *kollist_ff_gos,int *kolst_bu,int *kollist_bu,int *shethiksh,double *balstsh,double *iznossh,FILE *ff,FILE *ffbu,FILE *ff_gos);
void uosinved_konec(short mi,short gi,const char *shetuz,int *shethik,double *itogo,double *itogo_iznos,int *shethiksh,double *balstsh,double *iznossh,FILE *ff,FILE *ffbu,FILE *ff_gos);
int uosinved_dm(int innom,class SPISOK *dm_ei,class masiv_din_double *ves);
int uosinved_rdm(short di,short mi,short gi,class drag_met *dragm,class iceb_tu_str *imaf);
void uosinsap_gos(int *kollist_ff_gos,int *kolstr_ff_gos,FILE *ff_gos);
void uosinved_h_572(int noml,int *kolstr,FILE *ff);
int uosinved_sh_572(int *kolstr,int *kolih_uos,double *itogo,const char *naim,int metka_fn,FILE *ff);
void uosinved_rps(class SPISOK *sp_shet,class masiv_din_double *kol_shet,class masiv_din_double *sum_shet,FILE *ff);
void usoinved_itog572(const char *nazv_itog,int kolih_uos_list,double *itogo_572,int metka_fn,FILE *ff);

extern short	ddd,mmm,ggg;
extern int kol_strok_na_liste;
extern int  kol_strok_na_liste_l; //Количесво строк на стандартном листе в ориентации ландшафт

int uosinved(class spis_oth *oth)
{
static class iceb_tu_str dat1(poltekdat());
static class iceb_tu_str podraz("");
static class iceb_tu_str matot("");
static class iceb_tu_str sheta("");
static class iceb_tu_str invent_nom("");
static class iceb_tu_str naimos("");
class iceb_tu_str naimshet("");
class iceb_tu_str zav_nomer("");
class iceb_tu_str nom_pasp("");

int		kom=0,kom1=0;
short		di,mi,gi; //Дата инвентаризации
char		imaf[64];
FILE		*ff;
char		strsql[1024];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
int		poddz=0,kodotl=0;
int		innom=0;
int		shethik=0,shethiksh=0;
class bsizw_data bal_st;
int		kolst=0,kollist=0;
int kolstr_ff_gos=0,kollist_ff_gos=0;
double		itogo=0,itogo_iznos=0.;
int		vbs=0; //1-балансовая стоимость налогового учёта 0 -бухгалтерского
double balst=0.;
double iznos=0.;
double balstsh=0.;
double iznossh=0.;
time_t		tmmn=0;
class iceb_tu_str god("");
SQLCURSOR cur1;
class iceb_tu_str shetuz("");
class iceb_tu_str ikod("");

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(podraz.ravno(),128);
VV.VVOD_SPISOK_add_data(matot.ravno(),128);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(invent_nom.ravno(),128);
VV.VVOD_SPISOK_add_data(naimos.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата инвентаризации....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код мат.от-нного..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Инвентарний номер.(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Наименование......(,,).."));//5

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка инвентаризационных описей"));

naz:;

clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("доп.инфор."),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
podraz.new_plus(VV.data_ravno(1));
matot.new_plus(VV.data_ravno(2));
sheta.new_plus(VV.data_ravno(3));
invent_nom.new_plus(VV.data_ravno(4));
naimos.new_plus(VV.data_ravno(5));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
//    prosf(bros);
//    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственных"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
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
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
          VV.data_z_plus(1,kod_v.ravno());
        goto naz;

      case 1 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
          VV.data_z_plus(2,kod_v.ravno());
        goto naz;

      case 2 :
        vibrek("Plansh",&kod_v,&naim_v);
        VV.VVOD_SPISOK_zapis_data(3,kod_v.ravno());
        goto naz;
     }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5: /*Ввод дополнительной информации в шапку инвентаризационной ведомости*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Настройка начала документа"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка конца документа Приложения 12"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка конца инвентаризационной описи"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка начала инвентаризационной описи бюджетных организаций "));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка концовки инвентаризационной описи бюджетных организаций "));
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
        iceb_redfil("inventuos.alx",0);
        goto naz;

      case 1 :
        iceb_redfil("in_d12_end.alx",0);
        goto naz;

      case 2 :
        iceb_redfil("inved_end.alx",0);
        goto naz;

      case 3 :
        iceb_redfil("invuos572z.alx",0);
        goto naz;
      case 4 :
        iceb_redfil("invuos572k.alx",0);
        goto naz;

     }
    goto naz;    

  default:
    goto naz;
 }

if(rsdat(&di,&mi,&gi,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата инвентаризации"));
  goto naz;
 }


 
MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите какая балансовая стоимости должна быть в документе"));

MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для бухгалтерского учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для налогового учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
vbs=0;
while(menu3w(stdscr,&MENU,&vbs,-1,-1,0) != 0);

if(vbs == -1 || vbs == 2)
  goto naz;


class VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Колонки \"Фактическое наличие\" заполнять?"));

int metka_fn=danet(&DANET,2,stdscr);



time(&tmmn);

GDITE();

SQLCURSOR cur;

class iceb_tu_str naim00("");

//читаем код
sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 {
  naim00.new_plus(row[0]);
  ikod.new_plus(row[1]);
 }
class drag_met dragm;


int kolih_dm=0;
/*Читаем список драг.металлов*/
sprintf(strsql,"select distinct kod,ei from Uosindm");
if((kolih_dm=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  dragm.dm_ei.plus(strsql);
 }
if(kolih_dm > 0)
 dragm.ves_dm.make_class(kolih_dm);

sprintf(strsql,"select innom,god,naim,model,pasp,zavnom,datvv from Uosin order by innom asc");
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
const char *imatmptab={"uosinved"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kmo int not null,\
shu char(32) not null,\
innom int not null,\
god smallint not null,\
naim char(112) not null,\
pod int not null,\
model char(112) not null,\
pasp char(112) not null,\
zavnom char(56) not null,\
datvv DATE not null,\
kla float(5,1) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
class iceb_tu_str pasp("");
class iceb_tu_str model("");
class iceb_tu_str zavnom("");
class iceb_tu_str naim("");
kolstr1=0;
class poiinpdw_data rekin;
float kol_let_amort=0; /*количество лет амортизации*/

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(invent_nom.ravno(),row[0],0,0) != 0)
   continue;
  innom=atol(row[0]);
  if(proverka(naimos.ravno(),row[2],4,0) != 0)
   continue;
   
  if(poiin(innom,di,mi,gi,&poddz,&kodotl) != 0) //Произошел расход или оно пришло позже даты
    continue;
    
  if(proverka(podraz.ravno(),poddz,0,0) != 0)
    continue;

  if(proverka(matot.ravno(),kodotl,0,0) != 0)
    continue;

  poiinpd(innom,mi,gi,&rekin);

  if(proverka(sheta.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  kol_let_amort=0;

  if(vbs == 1) /*налоговый учёт*/
   {
    /*Узнаём годовой процент амортизации*/
    sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(atof(row1[0])*rekin.popkf != 0.)
       kol_let_amort=100/atof(row1[0])*rekin.popkf;
     }               
   }
  else /*бухгалтерский учёт*/
   {
    /*Узнаём годовой процент амортизации*/
    sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(atof(row1[0])*rekin.popkfby != 0.)
       kol_let_amort=100/atof(row1[0])*rekin.popkfby;
      
     }               
   }
  dragm.sp_in.plus(row[0]);
  naim.new_plus(row[2]);
  model.new_plus(row[3]);
  pasp.new_plus(row[4]);
  zavnom.new_plus(row[5]);
  sprintf(strsql,"insert into %s values (%d,'%s',%s,%s,'%s',%d,'%s','%s','%s','%s',%.1f)",
  imatmptab,
  kodotl,rekin.shetu.ravno(),row[0],row[1],naim.ravno_filtr(),poddz,model.ravno_filtr(),pasp.ravno_filtr(),zavnom.ravno_filtr(),row[6],kol_let_amort);
  
  sql_zapis(strsql,1,0);

 }
if(kolih_dm > 0)
 {
  dragm.ves_hax.make_class(kolih_dm*dragm.sp_in.kolih());
  dragm.ves_dmv.make_class(dragm.sp_in.kolih());
 } 


sprintf(strsql,"select * from %s order by kmo asc,shu asc,innom asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }






char imafbu[64];

sprintf(imafbu,"invb%s_%d.lst",podraz.ravno(),getpid());
FILE *ffbu;
if((ffbu = fopen(imafbu,"w")) == NULL)
 {
  error_op_nfil(imafbu,errno,"");
  return(1);
 }
int kollist_bu=0;
int kolst_bu=0;


sprintf(imaf,"inv%s_%d.lst",podraz.ravno(),getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
char imaf_gos[64];
FILE *ff_gos;
sprintf(imaf_gos,"invg%s_%d.lst",podraz.ravno(),getpid());

if((ff_gos = fopen(imaf_gos,"w")) == NULL)
 {
  error_op_nfil(imaf_gos,errno,"");
  return(1);
 }

char imaf_ipsh[64];
FILE *ffipsh;
sprintf(imaf_ipsh,"dod%d.lst",getpid());
if((ffipsh = fopen(imaf_ipsh,"w")) == NULL)
 {
  error_op_nfil(imaf_ipsh,errno,"");
  return(1);
 }

char imaf_572[64];
sprintf(imaf_572,"io572_%d.lst",getpid());

class iceb_fopen fil_572;/*инвентаризационная опись наказ Міністерства фінансів України від 17.06.2015 N 572*/
if(fil_572.start(imaf_572,"w") != 0)
 return(1);

char imaf_572s[64];
sprintf(imaf_572s,"io572s_%d.lst",getpid());

class iceb_fopen fil_572s;/*инвентаризационная опись наказ Міністерства фінансів України від 17.06.2015 N 572*/
if(fil_572s.start(imaf_572s,"w") != 0)
 return(1);

in_d12_start(ffipsh);

class SPISOK sp_shet;
class masiv_din_double sum_shet;
class masiv_din_double kol_shet;

class SPISOK sp_shet_i;
class masiv_din_double sum_shet_i;
class masiv_din_double kol_shet_i;

shetuz.new_plus("");
short dvv,mvv,gvv;
int kod_otv_z=-1; //может равняться нулю
int nomer_sheta=0;
kolstr1=0;
double itogo_572[2];
memset(itogo_572,'\0',sizeof(itogo_572));
double itogo_572l[2];
memset(itogo_572l,'\0',sizeof(itogo_572l));
int nom_zap572=0;
class iceb_tu_str fio_mo("");
int kolstr_572s=0;
int kolih_uos_list=0;
class iceb_tu_str suma_prop("");
class iceb_tu_str dolg("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  kodotl=atoi(row[0]);
  rekin.shetu.new_plus(row[1]);
  innom=atoi(row[2]);
  god.new_plus(row[3]);
  naim.new_plus(row[4]);
  poddz=atoi(row[5]);
  model.new_plus(row[6]);
  nom_pasp.new_plus(row[7]);
  zav_nomer.new_plus(row[8]);
  rsdat(&dvv,&mvv,&gvv,row[9],2);

  if(kod_otv_z != kodotl)
   {
    if(kod_otv_z != -1)
     {
      uosinved_konec(mi,gi,shetuz.ravno(),&shethik,&itogo,&itogo_iznos,&shethiksh,&balstsh,&iznossh,ff,ffbu,ff_gos);

      usoinved_itog572(gettext("Итого"),kolih_uos_list,itogo_572l,metka_fn,fil_572s.ff);
      usoinved_itog572(gettext("Общий итог"),nom_zap572,itogo_572,metka_fn,fil_572s.ff);
      usoinved_itog572(gettext("Итого"),nom_zap572,itogo_572,metka_fn,fil_572.ff);

      fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(kolih_uos_list));
      fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(kolih_uos_list));
      kolih_uos_list=0;
      

      /*вставляем концовку ведомости*/

      fprintf(fil_572.ff,"ICEB_LST_END\n");

      uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
      uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);
      sp_shet.free_class();
      kol_shet.free_class();
      sum_shet.free_class();

      class iceb_vstavfil_rk rhd;
      rhd.zap(1,1,49,160,iceb_tu_nofwords(nom_zap572));
      rhd.zap(7,1,80,160,iceb_tu_nofwords(nom_zap572));
      preobr(itogo_572[0],&suma_prop,0);
      rhd.zap(9,1,62,160,suma_prop.ravno());
      rhd.zap(25,1,30,60,dolg.ravno());
      rhd.zap(25,1,97,160,fio_mo.ravno());

      iceb_t_vstavfil(&rhd,"invuos572k.alx",fil_572.ff);
      iceb_t_vstavfil(&rhd,"invuos572k.alx",fil_572s.ff);

      nom_zap572=0;
      memset(itogo_572,'\0',sizeof(itogo_572));
      memset(itogo_572l,'\0',sizeof(itogo_572l));
      
      fprintf(ff,"\f");
      fprintf(ffbu,"\f");
      fprintf(ff_gos,"\f");
      fprintf(fil_572.ff,"\fICEB_LST_START\n");
      fprintf(fil_572s.ff,"\f");
      
     }
    uosinved_pl(di,mi,gi,ikod.ravno(),&kollist_bu,&kolst_bu,ffbu);
    uosinved_sap(mi,gi,poddz,kodotl,&kollist,&kolst,&kollist_ff_gos,&kolstr_ff_gos,ff,ff_gos);

    sprintf(strsql,"select naik,dolg from Uosol where kod=%d",kodotl);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      fio_mo.new_plus(row1[0]);
      dolg.new_plus(row1[1]);
     }
    else
     {
      fio_mo.new_plus("");
      dolg.new_plus("");
     }     
    class iceb_vstavfil_rk rhd;
    rhd.zap(1,1,0,100,naim00.ravno());
    rhd.zap(4,2,32,50,ikod.ravno());
    rhd.zap(29,1,2,60,dolg.ravno());
    rhd.zap(29,1,107,140,fio_mo.ravno());

    /*вставляем заголовок файла*/
    iceb_t_vstavfil(&rhd,"invuos572z.alx",fil_572.ff);
    iceb_t_vstavfil(&rhd,"invuos572z.alx",fil_572s.ff);

    fprintf(fil_572.ff,"\fICEB_LST_START\n"); /*заголовок и шапка страницы не помещаются на одном листе*/
    uosinved_h_572(1,NULL,fil_572.ff);

    fprintf(fil_572s.ff,"\f"); /*заголовок и шапка страницы не помещаются на одном листе*/
    kolstr_572s=iceb_t_insfil("invuos572h.alx",fil_572s.ff);
            
    kod_otv_z=kodotl;
    
   }
  if(SRAV(rekin.shetu.ravno(),shetuz.ravno(),0) != 0)
   {
    if(shetuz.ravno()[0] != '\0')
     {
      if(balstsh != 0.)
        uosinved_ips(di,mi,gi,shetuz.ravno(),&kolst,&kollist,&kolstr_ff_gos,&kollist_ff_gos,&kolst_bu,&kollist_bu,&shethiksh,&balstsh,&iznossh,ff,ffbu,ff_gos);

     }
    sprintf(strsql,"select nais from Plansh where ns='%s'",rekin.shetu.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
      naimshet.new_plus(row1[0]);
    else
      naimshet.new_plus("");

    //Счетчик строк
    uosinvedsh(&kolst,&kollist,&kolstr_ff_gos,&kollist_ff_gos,ff,ff_gos);

    fprintf(ff,"%s:%s %s\n",gettext("Счёт"),rekin.shetu.ravno(),naimshet.ravno());
    fprintf(ff_gos,"%s:%s %s\n",gettext("Счёт"),rekin.shetu.ravno(),naimshet.ravno());

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%s:%s %s\n",gettext("Счёт"),rekin.shetu.ravno(),naimshet.ravno());
  
    shethiksh=0;
    balstsh=0.;
    iznossh=0.;
    shetuz.new_plus(rekin.shetu.ravno());
   }


  bsiz(innom,poddz,di,mi,gi,&bal_st,NULL);

  if(vbs == 1)
   {
    balst=bal_st.bs+bal_st.sbs;
    iznos=bal_st.siz+bal_st.iz+bal_st.iz1;
   }
  if(vbs == 0)
   {
    balst=bal_st.bsby+bal_st.sbsby;
    iznos=bal_st.sizby+bal_st.izby+bal_st.iz1by;
   }
  //Счетчик строк
  uosinvedsh(&kolst,&kollist,&kolstr_ff_gos,&kollist_ff_gos,ff,ff_gos);

  fprintf(ff,"%4d %-11d %-*.*s %4s %02d.%02d.%04d %-*.*s|%4s|%10.2f|    |          |    |          |\n",
  ++shethik,innom,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  god.ravno(),
  dvv,mvv,gvv,
  iceb_tu_kolbait(10,model.ravno()),
  iceb_tu_kolbait(10,model.ravno()),
  model.ravno(),
  "1",
  balst);

  fprintf(ff_gos,"%4d|%-11d|%-*.*s|%10.2f|%10.2f|%10.2f|%10s|%02d.%02d.%04d|     |          |          |          |          |\n",
  shethik,
  innom,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  balst,
  iznos,
  balst-iznos,
  "",
  dvv,mvv,gvv);

  
  uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);

  fprintf(ffbu,"%4d %-*.*s %4s %-11d %-*.*s %-*.*s|%4s|%10s|%4s|%10.2f|\n",
  shethik,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  god.ravno(),
  innom,
  iceb_tu_kolbait(9,zav_nomer.ravno()),
  iceb_tu_kolbait(9,zav_nomer.ravno()),
  zav_nomer.ravno(),
  iceb_tu_kolbait(8,nom_pasp.ravno()),
  iceb_tu_kolbait(8,nom_pasp.ravno()),
  nom_pasp.ravno(),
  "","","1",balst);

  if(iceb_tu_strlen(naim.ravno()) > 30)
   {
    //Счетчик строк
    uosinvedsh(&kolst,&kollist,&kolstr_ff_gos,&kollist_ff_gos,ff,ff_gos);
    fprintf(ff,"%4s %-11s %s\n"," "," ",iceb_tu_adrsimv(30,naim.ravno()));

    fprintf(ff_gos,"%4s|%-11s|%-*.*s|          |          |          |          |          |     |          |          |          |          |\n",
    " "," ",
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,naim.ravno())),
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,naim.ravno())),
    iceb_tu_adrsimv(30,naim.ravno()));

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%4s %s\n","",iceb_tu_adrsimv(30,naim.ravno()));

   }

  uosinved_sh_572(&kolstr_572s,&kolih_uos_list,itogo_572l,naim.ravno(),metka_fn,fil_572s.ff); /*считаем по 2 строки*/
  nom_zap572++;
  kolih_uos_list++;  

  if(metka_fn == 2)
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_tu_kolbait(5,gettext("Шт")),
   iceb_tu_kolbait(5,gettext("Шт")),
   "Шт",
   "",
   "",
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   
  else
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13.2f|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_tu_kolbait(5,gettext("Шт")),
   iceb_tu_kolbait(5,gettext("Шт")),
   "Шт",
   "1",
   balst,
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   

  for(int nom=30; nom < iceb_tu_strlen(naim.ravno()); nom+=30)
    {
     fprintf(fil_572s.ff,"%6s|%-*.*s|%10s|%14s|%10s|%8s|%5s|%4s|%13s|%6s|%4s|%13s|%12s|%12s|%6s|\n",
     "",
     iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
     iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
     iceb_tu_adrsimv(nom,naim.ravno()),
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "");
    }    

  if(metka_fn == 2)
   {
    for(int nom=0; nom < 173;nom++)
     fprintf(fil_572s.ff,".");
    fprintf(fil_572s.ff,"\n");
   }
      
  if(metka_fn == 2)
   fprintf(fil_572.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_tu_kolbait(5,gettext("Шт")),
   iceb_tu_kolbait(5,gettext("Шт")),
   "Шт",
   "",
   "",
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   
  else
   fprintf(fil_572.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13.2f|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   iceb_tu_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   iceb_tu_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_tu_kolbait(5,gettext("Шт")),
   iceb_tu_kolbait(5,gettext("Шт")),
   "Шт",
   "1",
   balst,
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   
  
  for(int nom=30; nom < iceb_tu_strlen(naim.ravno()); nom+=30)
   fprintf(fil_572.ff,"%6s|%-*.*s|%10s|%14s|%10s|%8s|%5s|%4s|%13s|%6s|%4s|%13s|%12s|%12s|%6s|\n",
   "",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim.ravno())),
   iceb_tu_adrsimv(nom,naim.ravno()),
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "");
  if(metka_fn == 2)
   {
    for(int nom=0; nom < 173;nom++)
      fprintf(fil_572.ff,".");
     fprintf(fil_572.ff,"\n");
   }   
  //Счетчик строк
  uosinvedsh(&kolst,&kollist,&kolstr_ff_gos,&kollist_ff_gos,ff,ff_gos);
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_gos,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  if(kolih_dm > 0)
   {
    class masiv_din_double ves;
    ves.make_class(kolih_dm);
    if(uosinved_dm(innom,&dragm.dm_ei,&ves) == 0)
     {
      int nomer_os=0;
      if((nomer_os=dragm.sp_in.find(innom)) < 0)
       {
        sprintf(strsql,"%s %d %s",gettext("Не найден инвентарный номер"),innom,gettext("в списке"));
        iceb_t_soob(strsql);
        continue;
       }

      for(int ii=0; ii < kolih_dm; ii++)
       {
        dragm.ves_dm.plus(ves.ravno(ii),ii);
        dragm.ves_hax.plus(ves.ravno(ii),kolih_dm*nomer_os+ii);
        dragm.ves_dmv.plus(ves.ravno(ii),nomer_os);
       }

     }    
   }

  if((nomer_sheta=sp_shet.find(rekin.shetu.ravno())) < 0)
   sp_shet.plus(rekin.shetu.ravno());
  kol_shet.plus(1,nomer_sheta);
  sum_shet.plus(balst,nomer_sheta);

  if((nomer_sheta=sp_shet_i.find(rekin.shetu.ravno())) < 0)
   sp_shet_i.plus(rekin.shetu.ravno());
  kol_shet_i.plus(1,nomer_sheta);
  sum_shet_i.plus(balst,nomer_sheta);
  
  itogo+=balst;
  itogo_iznos+=iznos;
  balstsh+=balst;
  iznossh+=iznos;
  shethiksh++;
  itogo_572[0]+=balst;
  itogo_572[1]+=iznos;
  itogo_572l[0]+=balst;
  itogo_572l[1]+=iznos;
 }


uosinved_konec(mi,gi,shetuz.ravno(),&shethik,&itogo,&itogo_iznos,&shethiksh,&balstsh,&iznossh,ff,ffbu,ff_gos);

usoinved_itog572(gettext("Итого"),kolih_uos_list,itogo_572l,metka_fn,fil_572s.ff);
usoinved_itog572(gettext("Общий итог"),nom_zap572,itogo_572,metka_fn,fil_572s.ff);
usoinved_itog572(gettext("Итого"),nom_zap572,itogo_572,metka_fn,fil_572.ff);

fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(kolih_uos_list));
fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(kolih_uos_list));

      
fprintf(fil_572.ff,"ICEB_LST_END\n");

uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);

class iceb_vstavfil_rk rhd;


rhd.zap(1,1,49,160,iceb_tu_nofwords(nom_zap572));
rhd.zap(7,1,80,160,iceb_tu_nofwords(nom_zap572));
preobr(itogo_572[0],&suma_prop,0);
rhd.zap(9,1,62,160,suma_prop.ravno());
rhd.zap(25,1,30,60,dolg.ravno());
rhd.zap(25,1,97,160,fio_mo.ravno());

iceb_t_vstavfil(&rhd,"invuos572k.alx",fil_572.ff);
iceb_t_vstavfil(&rhd,"invuos572k.alx",fil_572s.ff);

/*Распечатка формы додаток 12*/
in_d12_it(&sp_shet_i,&kol_shet_i,&sum_shet_i,ffipsh);

iceb_t_insfil("in_d12_end.alx",ffipsh);

podpis(ff);
podpis(ffbu);
podpis(ffipsh);
podpis(ff_gos);
podpis(fil_572.ff);
podpis(fil_572s.ff);

fclose(ff);
fclose(ffbu);
fclose(ffipsh);
fclose(ff_gos);
fil_572.end();
fil_572s.end();

class iceb_tu_str imaf_dragm;

if(kolih_dm > 0)
  uosinved_rdm(di,mi,gi,&dragm,&imaf_dragm);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Инвентаризационная опись"));

oth->spis_imaf.plus(imafbu);
oth->spis_naim.plus(gettext("Приложение 1"));

oth->spis_imaf.plus(imaf_ipsh);
oth->spis_naim.plus(gettext("Приложение 12"));

oth->spis_imaf.plus(imaf_gos);
oth->spis_naim.plus(gettext("Инвентаризационная опись форма 2"));

oth->spis_imaf.plus(imaf_572s);
oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций с итогами"));


if(kolih_dm > 0)
 {
  oth->spis_imaf.plus(imaf_dragm.ravno());
  oth->spis_naim.plus(gettext("Инвентаризационная опись драгоценных металлов"));
 } 

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imaf_572);
oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций"));
iceb_t_ustpeh(imaf_572,3,&kolstr);
iceb_t_rnl(imaf_572,kolstr,"....",&uosinved_h_572);


return(0);

}

/******************/
/*Шапка распечатки*/
/******************/

void uosinsap(int *kollist,int *kolst,FILE *ff)
{

*kollist+=1;
*kolst+=5;
fprintf(ff,"%65s %s%d\n"," ",gettext("Лист N"),*kollist);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Nп-п|Инвентарный|        Наименование          |Год |Дата ввода|  Модель  |   Бухучет     | Факт. наличие |    Разница    |\n"));
fprintf(ff,gettext("\
    |  номер    |                              |вып.|в эксплуа.|          |Кол.|Стоимость |Кол.|Стоимость |Кол.|Стоимость |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
}
/***********************************/
/*шапка для отчёта для госстандарта*/
/***********************************/
void uosinsap_gos(int *kollist_ff_gos,int *kolstr_ff_gos,FILE *ff_gos)
{
*kollist_ff_gos+=1;
*kolstr_ff_gos+=6;

fprintf(ff_gos,"%65s %s%d\n"," ",gettext("Лист N"),*kollist_ff_gos);
fprintf(ff_gos,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_gos,"\
Nп-п|Інвентарний|                              |Балансова |  Знос    |Залишкова |Ліквідацій|Дата ввода|Група|Термін кор|Мінімальни|          |          |\n\
    |  номер    |      Найменування            |вартість  |          |вартість  |на вартіст|в експлуат|     |исного вик|й термін  | Примітка | Примітка |\n\
    |           |                              |          |          |          |ь         |ацію      |     |ористання |використ. |          |          |\n");
/*******
1234 12345678901 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 12345
***********/
       
fprintf(ff_gos,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*Счетчик строк*/
/****************/
void  uosinvedsh(int *kolst,int *kollist,int *kolstr_ff_gos,int *kollist_ff_gos,FILE *ff,FILE *ff_gos)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=1;
  uosinsap(kollist,kolst,ff);
 }

*kolstr_ff_gos+=1;
if(*kolstr_ff_gos >= kol_strok_na_liste)
 {
  fprintf(ff_gos,"\f");
  *kolstr_ff_gos=1;
  uosinsap_gos(kollist_ff_gos,kolstr_ff_gos,ff_gos);
 }
}
/***************/
/*Счетчик строк для бюджетной распечатки*/
/****************/
void  uosinvedsh_bu(short di,short mi,short gi,
int *kolst,int *kollist,FILE *ff)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=1;
  uosinved_sbu(di,mi,gi,kollist,kolst,ff);
 }
}
/**************************************/
/*Шапка для бюджетных организиций*/
/*********************************/

void uosinved_sbu(short dost,short most,short gost,int *kollist,int *kolst,FILE *ff)
{
*kollist+=1;
*kolst+=6;

fprintf(ff,"%80s %s N%d\n"," ",gettext("Лист"),*kollist);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |        Найменування          |Рік |            Номер             |    Станом   на %02d.%02d.%04dг.   |Відмітка о |\n\
з/п |                              |    |Інвентарний|заводсь- |паспорта|Факт. наявність|   Бухоблік    |вибутії    |\n\
    |                              |    |           |  кий    |        |Кіл.|Вартість  |Кіл.|Вартість  |           |\n",
dost,most,gost);
//   123456789012345678901234567890 1234 12345678901 123456789 12345678 1234 1234567890 1234 1234567890
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

}
/****************************************/
/*Первый лист бюджетного документа*/
/***********************************/

void uosinved_pl(short di,short mi,short gi,
const char *ikod,
int *kollist,
int *kolstr,
FILE *ffbu)
{
*kollist=*kolstr=0;

fprintf(ffbu,"\
                                                            Додаток 1\n\
                                                            до інструкції з інвентаризації матеріальних цінностей,\n\
                                                            розрахунків та інших статей балансу бюджених установ.\n");
fprintf(ffbu,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ffbu,"Індентифікаційний код за ЄДРПОУ %s\n",ikod);
                                                                          
fprintf(ffbu,"                                        ІНВЕТАРИЗАЦІЙНИЙ ОПИС ОСНОВНИХ ЗАСОБІВ\n");
fprintf(ffbu,"                                                  %02d.%02d.%04d\n",di,mi,gi);
fprintf(ffbu,"Основні засоби_______________________________________________________________________________\n\n");
fprintf(ffbu,"Місце знаходження____________________________________________________________________________\n");

fprintf(ffbu,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ffbu,"\
______________________________   _____________________________   _______________________________\n\
   (посада, підпис)                   (посада, підпис)               (посада, підпис)\n\n\
______________________________   _____________________________   _______________________________\n\
(призвище, ім'я, по батькові)    (призвище, ім'я, по батькові)   (призвище, ім'я, по батькові)\n\n");

fprintf(ffbu,"На підставі (розпорядження) від \"____\"_____________________р.\n\n");
fprintf(ffbu,"Виконано знімання фактичних залишків__________________________________________\n\n");
fprintf(ffbu,"що значаться на обліку станом на \"____\"____________________р.\n\n");
fprintf(ffbu,"Інвентаризація: розпочата \"___\"____________________р.\n\n");
fprintf(ffbu,"                закінчена \"___\"____________________p.\n");

fprintf(ffbu,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));
*kolstr+=32;
uosinved_sbu(di,mi,gi,kollist,kolstr,ffbu);

}
/*******************************/
/*Концовка отчёта для бюджета*/
/*******************************/
void uosinved_end_bu(FILE *ffbu)
{
fprintf(ffbu,"\
Разом за описом : а) кількість порядкових номерів____________________________\n\
                                                          (прописом)\n\
                  б) загальна кількість лдиниць, фактично____________________\n\
                                                          (прописом)\n\
                  в) на суму, грн., фактично_________________________________\n\
                                                          (прописом)\n\n");
fprintf(ffbu,"\
Голова комісії ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
Члени комісії: ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
               ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
              ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (призвище, ім'я, по батькові)\n\n\
 Усі цінності, пойменовані в цьому інвентаризаційному описі з N__________ до N___________,\n\
перевірені комісією в натурі за моєї (нашої) присутності та внесені в опис, у зв'язку з чим\n\
претензій до інвентаризаційної комісії не маю (не маємо). Цінності, перелічені в описі,\n\
знаходяться на моєму (нашому) відповідальному зберіганні.\n\n\
Особа(и), відповідальна(і) за збереження основних засобів:\n\n\
\"____\"_____________________р.\n\n\
____________________________________________________________________________________________\n\n\
 Вказані у данному описі дані превірив ____________________  ______________________\n\
                                             (посада)            (підпис)\n\n\
\"____\"_____________________р.\n");

}
/******************************************/
/*                                         */
/******************************************/
void uosived_sap1(short mi,short gi,int podr,int kodotl,int *kolst,FILE *ff)
{
char strsql[512];
class iceb_tu_str naipod("");
class iceb_tu_str fiootv("");
SQL_str row;
class SQLCURSOR cur1;
if(kodotl != 0)
 {
  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),kodotl);
    iceb_t_soob(strsql);
   }
  else 
   fiootv.new_plus(row[0]);
 }

if(podr != 0)
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
    iceb_t_soob(strsql);
   }
  else 
    naipod.new_plus(row[0]);
 }

fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(ff,gettext("ИНВЕНТАРИЗАЦИОННАЯ ОПИСЬ N_____ ОСНОВНЫХ СРЕДСТВ.\n"));
fprintf(ff,"%s %s\n",gettext("Подразделение"),naipod.ravno());
fprintf(ff,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ff,"\n\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
fiootv.ravno());

*kolst=12;
*kolst+=iceb_t_insfil("inventuos.alx",ff);

char mesqc[64];
memset(mesqc,'\0',sizeof(mesqc));
mesc(mi,1,mesqc);

fprintf(ff,"\n\
%*s:%*s \"______\" %s %d%s\n\
%15s %*s \"______\" %s %d%s\n",
iceb_tu_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
iceb_tu_kolbait(10,gettext("начата")),gettext("начата"),
mesqc,gi,
gettext("г."),
" ",
iceb_tu_kolbait(10,gettext("окончена")),gettext("окончена"),
mesqc,gi,
gettext("г."));

fprintf(ff,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));
*kolst+=5;

}


/*****************************/
/*Шапка */
/******************************/
void uosinved_sap(short mi,short gi,int podr,int kodotl,int *kollist,int *kolst,int *kollist_ff_gos,int *kolstr_ff_gos,FILE *ff,FILE *ff_gos)
{

uosived_sap1(mi,gi,podr,kodotl,kolst,ff);
uosived_sap1(mi,gi,podr,kodotl,kolstr_ff_gos,ff_gos);

*kollist=0;
*kollist_ff_gos=0;
uosinsap(kollist,kolst,ff);
uosinsap_gos(kollist_ff_gos,kolstr_ff_gos,ff_gos);

}
/**********************************/
/*Итого по счёту*/
/*******************************/

void uosinved_ips(short di,short mi,short gi,
const char *shetuz,
int *kolst,int *kollist,
int *kolstr_ff_gos,int *kollist_ff_gos,
int *kolst_bu,int *kollist_bu,
int *shethiksh,
double *balstsh,
double *iznossh,
FILE *ff,FILE *ffbu,FILE *ff_gos)
{
char bros[512];

sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);

uosinvedsh(kolst,kollist,kolstr_ff_gos,kollist_ff_gos,ff,ff_gos);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_tu_kolbait(74,bros),bros,*shethiksh,*balstsh);
fprintf(ff_gos,"%*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(47,bros),bros,*balstsh,*iznossh,*balstsh-*iznossh);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_tu_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);
*balstsh=0.;
*iznossh=0.;
*shethiksh=0;
}

/*******************************/
/*концовка распечатки*/
/*****************************/
void uosinved_konec(short mi,short gi,const char *shetuz,int *shethik,double *itogo,double *itogo_iznos,int *shethiksh,double *balstsh,double *iznossh,FILE *ff,FILE *ffbu,FILE *ff_gos)
{
char bros[512];


sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_tu_kolbait(74,bros),bros,*shethiksh,*balstsh);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%4d %10.2f\n",iceb_tu_kolbait(74,gettext("Итого")),gettext("Итого"),*shethik,*itogo);

fprintf(ff_gos,"%*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(47,bros),bros,*balstsh,*iznossh,*balstsh-*iznossh);

fprintf(ff_gos,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_gos,"%*s|%10.2f|%10.2f|%10.2f|\n",iceb_tu_kolbait(47,gettext("Итого")),gettext("Итого"),*itogo,*itogo_iznos,*itogo-*itogo_iznos);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_tu_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_tu_kolbait(71,gettext("Итого")),gettext("Итого"),"","",*shethik,*itogo);

char mesqc[64];
memset(mesqc,'\0',sizeof(mesqc));
mesc(mi,1,mesqc);

koncinv(mesqc,gi,ff);

koncinv(mesqc,gi,ff_gos);

uosinved_end_bu(ffbu);
*balstsh=0.;
*shethik=0;
*shethiksh=0;
*itogo=0.;
*itogo_iznos=0.;
*iznossh=0.;
}

/*************************/
/*Чтение записей по драг.металлам*/
/*********************************/
/*Если вернули 0-драг.металлы есть 1-нет*/
int uosinved_dm(int innom,class SPISOK *dm_ei,class masiv_din_double *ves)
{
int kolih=0;
class SQLCURSOR cur;
SQL_str row;
char strsql[512];
sprintf(strsql,"select kod,ei,ves from Uosindm where innom=%d",innom);

if((kolih=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolih == 0)
 return(1);
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  if((nomer=dm_ei->find(strsql)) < 0)
   {
    iceb_t_soob("Не найдена нужная запись в списке!!!");
    continue;
   }
//  ves[nomer]+=atof(row[2]);
  ves->plus(atof(row[2]),nomer);
 }
return(0);
}

/****************************************/
/*Распечатка по драгоценным металлам*/
/****************************************/
int uosinved_rdm(short di,short mi,short gi,
class drag_met *dragm,
class iceb_tu_str *imaf)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff_dm;
char imaf_dm[56];
sprintf(imaf_dm,"indm%d.lst",getpid());
imaf->new_plus(imaf_dm);
if((ff_dm = fopen(imaf_dm,"w")) == NULL)
 {
  error_op_nfil(imaf_dm,errno,"");
  return(1);
 }


fprintf(ff_dm,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(ff_dm,gettext("ИНВЕНТАРИЗАЦИОННАЯ ОПИСЬ N_____ ДРАГОЦЕННЫХ МЕТАЛЛОВ В ОСНОВНЫХ СРЕДСТВАХ\n"));
fprintf(ff_dm,"%s %s\n",gettext("Подразделение"),"");
fprintf(ff_dm,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ff_dm,"\n\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
"");

iceb_t_insfil("inventuos.alx",ff_dm);

class iceb_tu_str mesqc("");
mesc(mi,1,&mesqc);

fprintf(ff_dm,"\n\
%*s:%*s \"______\" %s %d%s\n\
%15s %*s \"______\" %s %d%s\n",
iceb_tu_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
iceb_tu_kolbait(10,gettext("начата")),gettext("начата"),
mesqc.ravno(),gi,
gettext("г."),
" ",
iceb_tu_kolbait(10,gettext("окончена")),gettext("окончена"),
mesqc.ravno(),gi,
gettext("г."));

fprintf(ff_dm,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));


class iceb_tu_str liniq;

int kolih_dm=dragm->dm_ei.kolih();

liniq.plus("-----------------------------------------------------------------------");
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  liniq.plus("-----------");
 }
fprintf(ff_dm,"%s\n",liniq.ravno());
  
fprintf(ff_dm,gettext("\
Номер|Инвентарный|        Наименование          | Модель   |Заводской |"));
class iceb_tu_str drm("");
class iceb_tu_str naim_dm("");
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  polen(dragm->dm_ei.ravno(ii),&drm,1,'|');
  sprintf(strsql,"select naik from Uosdm where kod=%d",drm.ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   naim_dm.new_plus(row[0]);    
  else
   naim_dm.new_plus("");
  sprintf(strsql,"%s %s",drm.ravno(),naim_dm.ravno());
  fprintf(ff_dm,"%-*.*s|",iceb_tu_kolbait(10,strsql),iceb_tu_kolbait(10,strsql),strsql);
 }
fprintf(ff_dm,"\n");

fprintf(ff_dm,gettext("\
 п-п | номер     |                              |          |  номер   |"));
/*
12345 12345678901 123456789012345678901234567890 1234567890 1234567890
*/
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  polen(dragm->dm_ei.ravno(ii),&drm,2,'|');
  fprintf(ff_dm,"%-*.*s|",iceb_tu_kolbait(10,drm.ravno()),iceb_tu_kolbait(10,drm.ravno()),drm.ravno());
 }

fprintf(ff_dm,"\n%s\n",liniq.ravno());
class iceb_tu_str naim("");
class iceb_tu_str model("");
class iceb_tu_str zav_nomer("");
int nomer_pp_dm=0;
for(int nom_in=0; nom_in < dragm->sp_in.kolih(); nom_in++)
 {
  if(dragm->ves_dmv.ravno(nom_in) == 0.)
   continue;
 
  sprintf(strsql,"select naim,model,zavnom from Uosin where innom=%d",dragm->sp_in.ravno(nom_in));
  if(readkey(strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    model.new_plus(row[1]);
    zav_nomer.new_plus(row[2]);
   }
  else
   {
    zav_nomer.new_plus("");
    naim.new_plus("");
    model.new_plus("");
   }
  fprintf(ff_dm,"%5d|%-11d|%-*.*s|%-*.*s|%-*.*s|",
  ++nomer_pp_dm,
  dragm->sp_in.ravno(nom_in),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(10,model.ravno()),iceb_tu_kolbait(10,model.ravno()),model.ravno(),
  iceb_tu_kolbait(10,zav_nomer.ravno()),iceb_tu_kolbait(10,zav_nomer.ravno()),zav_nomer.ravno());
  for(int nom_dm=0; nom_dm < kolih_dm; nom_dm++)
   {
    if(dragm->ves_dm.ravno(nom_dm) == 0.)
     continue;
    fprintf(ff_dm,"%10.10g|",dragm->ves_hax.ravno(kolih_dm*nom_in+nom_dm));
   }
  fprintf(ff_dm,"\n");
  
 }

fprintf(ff_dm,"%s\n",liniq.ravno());
fprintf(ff_dm,"%*s|",iceb_tu_kolbait(70,gettext("Итого")),gettext("Итого"));
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
     continue;
  fprintf(ff_dm,"%10.10g|",dragm->ves_dm.ravno(ii));
 }
fprintf(ff_dm,"\n%s\n",liniq.ravno());

koncinv(mesqc.ravno(),gi,ff_dm);

podpis(ff_dm);
fclose(ff_dm);

return(0);

}
/**********************************/
/*печать шапки страницы инвентаризационной ведомости для бюдженых организаций Наказ 572 от 17.06.2015р.*/
/******************************/
void uosinved_h_572(int noml,int *kolstr,FILE *ff)
{
 
fprintf(ff,"%145s %s:%d\n","",gettext("Страница"),noml);

int kz=iceb_t_insfil("invuos572h.alx",ff);

if(kolstr != NULL)
 *kolstr=*kolstr+kz+1;

}
/*********************************************************************/
/*Счётчик инвентаризационной ведомости с итогами на каждой странице*/
/********************************************************************/
int uosinved_sh_572(int *kolstr,int *kolih_uos,double *itogo,const char *naim,int metka_fn,FILE *ff)
{
int kol=2;
if(metka_fn == 1)
 kol=1;
 
*kolstr+=kol;

int kds=0; /*количество дополнительных строк*/
for(int nom=30; nom < iceb_tu_strlen(naim);nom+=30)
  kds++;
  

*kolstr+=kds;


if(*kolstr <= kol_strok_na_liste_l-3) /*отнимаем 3 итоговые строки*/
 return(0);

usoinved_itog572(gettext("Итого"),*kolih_uos,itogo,metka_fn,ff);
fprintf(ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(*kolih_uos));
fprintf(ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(*kolih_uos));

*kolih_uos=0;
itogo[0]=itogo[1]=0.;

fprintf(ff,"\f");

*kolstr=iceb_t_insfil("invuos572h.alx",ff)+kol+kds;

return(1);
}
/****************************************/
/*Распечатка по счетам*/
/*******************************/
void uosinved_rps(class SPISOK *sp_shet,class masiv_din_double *kol_shet,class masiv_din_double *sum_shet,FILE *ff)
{

for(int nom=0; nom < sp_shet->kolih(); nom++)
 {
  fprintf(ff,"%-*.*s %10.10g %12.2f\n",
  iceb_tu_kolbait(10,sp_shet->ravno(nom)),
  iceb_tu_kolbait(10,sp_shet->ravno(nom)),
  sp_shet->ravno(nom),
  kol_shet->ravno(nom),
  sum_shet->ravno(nom));  
 }
}
/*************************************/
/*итоги*/
/**************************************/
void usoinved_itog572(const char *nazv_itog,int kolih_uos_list,double *itogo_572,int metka_fn,FILE *ff)
{

if(metka_fn == 2)
 fprintf(ff,"%*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
 iceb_tu_kolbait(89,nazv_itog),
 nazv_itog,
 "",
 "",
 "",
 kolih_uos_list,
 itogo_572[0],
 itogo_572[1],
 itogo_572[0]-itogo_572[1]);
else
 fprintf(ff,"%*s|%4d|%13.2f|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
 iceb_tu_kolbait(89,nazv_itog),
 nazv_itog,
 kolih_uos_list,
 itogo_572[0],
 "",
 kolih_uos_list,
 itogo_572[0],
 itogo_572[1],
 itogo_572[0]-itogo_572[1]);

}
