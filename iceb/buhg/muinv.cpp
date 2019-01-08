/*$Id: muinv.c,v 5.46 2013/12/31 11:48:10 sasa Exp $*/
/*21.12.2017	10.08.2003	Белых А.И.	muinv.c
Распечатка инвентаризационных ведомостей для материального учета
*/

#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

void muinv_h_572(int noml,int *kolstr,FILE *ff);
int muinv_sh_572(int *kolstr,double *itogo,int *kol_na_liste,const char *naim,int metka_fn,FILE *ff);
void muinv_itogo572(char *naim_itog,double kolih,double suma,int metka_fn,FILE *ff);

extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/
extern int  kol_strok_na_liste_l; //Количесво строк на стандартном листе в ориентации ландшафт
void muinv_rps(class SPISOK *sp_shet,class masiv_din_double *kol_shet,class masiv_din_double *sum_shet,FILE *ff);

int muinv(class spis_oth *oth)
{
class ostatokt ost;
char		bros[512];
extern char	*maskmat; /*Массив кодов отмеченных материалов*/
struct  tm      *bf;
time_t          tmm;
static class iceb_tu_str dat1("");
static class iceb_tu_str skladi("");
static class iceb_tu_str grupi("");
static class iceb_tu_str sheta("");
static class iceb_tu_str kodm("");
short		dos,mos,gos;
int		kom=0,kom1=0;
int             metkainnom=0;
class iceb_tu_str naim("");
int		kgr=0,i=0;
char		strsql[1024];

time(&tmm);
bf=localtime(&tmm);


if(dat1.ravno()[0] == '\0')
 {
  sprintf(bros,"%d.%d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  dat1.new_plus(bros);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(skladi.ravno(),128);
VV.VVOD_SPISOK_add_data(grupi.ravno(),128);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(kodm.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка инвентаризационных ведомостей"));

VV.VVOD_SPISOK_add_MD(gettext("Склад.(,,)......."));
VV.VVOD_SPISOK_add_MD(gettext("Группа..(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт....(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Материал.(,,/*).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата остатка....."));
naz:;

clear();
//mvprintw(LINES-2,0,"%d",kol_strok_na_liste_l);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("доп.инфор."),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

skladi.new_plus(VV.data_ravno(0));
grupi.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
kodm.new_plus(VV.data_ravno(3));
dat1.new_plus(VV.data_ravno(4));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(kom)
 {
  case ESC:
  case FK10:
    return(1);
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu5_18.txt");
   clear();
   goto naz;

  case FK2:
  case PLU:
    break;

  case FK6: //Только карточки с инвентарными номеррами
    metkainnom=1;
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(0,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&kods,&naims);
        VV.VVOD_SPISOK_zapis_data(1,kods.ravno());
        goto naz;

      case 2 :
        if(dirmatr(&kgr,&i,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(3,i);
         }
        
        goto naz;

      case 3 :
        vibrek("Plansh",&kods,&naims);
        VV.VVOD_SPISOK_zapis_data(2,kods.ravno());
        goto naz;
     }

  case FK4: /*Очистка меню*/
    VV.VVOD_clear_data();
    goto naz;    

  case FK5: /*Ввод дополнительной информации в шапку инвентаризационной ведомости*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Настройка начала документа"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка конца Приложения 12"));
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
        iceb_redfil("inventm.alx",0);
        goto naz;

      case 1 :
        iceb_redfil("in_d12_end.alx",0);
        goto naz;

      case 2 :
        iceb_redfil("inved_end.alx",0);
        goto naz;

      case 3 :
        iceb_redfil("invmu572z.alx",0);
        goto naz;
      case 4 :
        iceb_redfil("invmu572k.alx",0);
        goto naz;


     }
    goto naz;    

  default:
    goto naz;    
 }


if(dat1.ravno()[0] == '\0' || rsdat(&dos,&mos,&gos,dat1.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

class VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Колонки \"Фактическое наличие\" заполнять?"));

int metka_fn=danet(&DANET,2,stdscr);


GDITE();

time(&tmm);
bf=localtime(&tmm);

int kolstr=0;

sprintf(strsql,"select sklad,nomk,kodm,ei,shetu,cena,innom,datv,rnd from Kart order by sklad,shetu asc");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной карточки !"));
  return(1);
 }
char imaf[64];
char imafipsh[64];
FILE *ff;
FILE *ffipsh;
class iceb_tu_str dopstr("");
class iceb_tu_str innom("");
class iceb_tu_str regnom("");
double cena=0.;
double summa=0.;
double ostatok=0.;
SQL_str row,row1;
int nk=0;
class iceb_tu_str ei("");
short  dv,mv,gv;
int kolstriv=0,kollistiv=0;
int nomerstr=0;
class iceb_tu_str sklad("");
class iceb_tu_str shetu("");
double  kolihsklad=0.;
double  sumasklad=0.;
double  kolihshet=0.;
double  sumashet=0.;
class iceb_tu_str matot("");
class iceb_tu_str mesqc("");
class iceb_tu_str naish("");
class iceb_tu_str artikul("");



mesc(mos,1,&mesqc);

class iceb_t_tmptab tabtmp;
const char *imatmptab={"muinv"};


char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

//Сортировка предпринята для того чтобы отсортировать и по наименованию

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
sh char(24) not null,\
naim char(112) not null,\
ei char(24) not null,\
cena double(15,6) not null,\
innom char(16) not null,\
datv DATE not null,\
rnd char(20) not null,\
articul char(32) not null,\
ost double(16,6) not null,\
INDEX(skl,sh,naim)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  

  if(proverka(skladi.ravno(),row[0],0,0) != 0)
    continue;

  if(kodm.ravno()[0] != '\0')
   {
    if(kodm.ravno()[0] == '*' && maskmat != NULL)
     {
      if(proverka(maskmat,row[2],0,0) != 0)
        continue;
     }
    else
      if(proverka(kodm.ravno(),row[2],0,0) != 0)
        continue;
   }

  if(proverka(sheta.ravno(),row[4],0,0) != 0)
    continue;

  if(metkainnom == 1)
   if(row[6][0] == '\0')
     continue;

  naim.new_plus("");
  artikul.new_plus("");

  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("%s %s!\n,",gettext("Не нашли наименование материалла по коду"),row[2]);
    OSTANOV();
    continue;
   }
  else
   {
    naim.new_plus(row1[2]);
    artikul.new_plus(row1[14]);
   }

  if(proverka(grupi.ravno(),row1[1],0,0) != 0)
   continue;

  nk=atoi(row[1]);

  ei.new_plus(row[3]);
  ostkar(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {

    VVOD SOOB(1);
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    SOOB.VVOD_SPISOK_add_MD(gettext("Отрицательный остаток"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

   }

  if(fabs(ost.ostg[3]) < 0.0000001 )
    continue;   

                                   //    0    1    2   3    4     5    6    7    8 
  sprintf(strsql,"insert into %s values (%d,'%s','%s','%s',%.10g,'%s','%s','%s','%s',%.10g)",
  imatmptab,
  atoi(row[0]),row[4],naim.ravno_filtr(),row[3],atof(row[5]),row[6],row[7],row[8],artikul.ravno(),ost.ostg[3]);

  sql_zapis(strsql,1,0);    

 }
GDITE();
class iceb_tu_str naim00("");
class iceb_tu_str edrpou("");

if(readkey("select naikon,kod from Kontragent where kodkon='00'",&row,&cur) == 1)
 {
  naim00.plus(row[0]);
  edrpou.plus(row[1]);  
 }
sprintf(strsql,"select * from %s order by skl asc,sh asc,naim asc",imatmptab);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf,"iv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imafipsh,"ivs%d.lst",getpid());

if((ffipsh = fopen(imafipsh,"w")) == NULL)
 {
  error_op_nfil(imafipsh,errno,"");
  return(1);
 }

in_d12_start(ffipsh);
char imaf_572[64];
sprintf(imaf_572,"invmu572_%d.lst",getpid());

class iceb_fopen fil_572;
if(fil_572.start(imaf_572,"w") != 0)
 return(1);

char imaf_572s[64];
sprintf(imaf_572s,"invmus572_%d.lst",getpid());

class iceb_fopen fil_572s;
if(fil_572s.start(imaf_572s,"w") != 0)
 return(1);


class SPISOK sp_shet;
class masiv_din_double sum_shet;
class masiv_din_double kol_shet;

class SPISOK sp_shet_i;
class masiv_din_double sum_shet_i;
class masiv_din_double kol_shet_i;
class iceb_tu_str naim_skl("");
class iceb_tu_str skladf("");
class iceb_tu_str shetuf("");
class iceb_tu_str cenaf("");
class iceb_tu_str datvf("");
int nomer_sheta;
class iceb_tu_str posada("");
int kolstr_572s=0;
double itogo_list[2];
memset(itogo_list,'\0',sizeof(itogo_list));
int kol_na_liste=0;
class iceb_tu_str suma_prop("");

while(cur.read_cursor(&row) != 0)
 {

  skladf.new_plus(row[0]);
  shetuf.new_plus(row[1]);
  naim.new_plus(row[2]);
  ei.new_plus(row[3]);
  cenaf.new_plus(row[4]);
  innom.new_plus(row[5]);
  datvf.new_plus(row[6]);
  regnom.new_plus(row[7]);
  artikul.new_plus(row[8]);
  ostatok=atof(row[9]);
  
  if(SRAV(sklad.ravno(),skladf.ravno(),0) != 0)
   {
    
    if(sklad.ravno()[0] != '\0')
     {

      sapkk1iv(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1iv(&kolstriv,&kollistiv,ff);

      fprintf(ff,"%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),kolihshet,sumashet);

      sapkk1iv(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1iv(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),kolihsklad,sumasklad);


      koncinv(mesqc.ravno(),gos,ff);

      fprintf(ff,"\f");

      muinv_itogo572(gettext("Итого"),itogo_list[0],itogo_list[1],metka_fn,fil_572s.ff);
      muinv_itogo572(gettext("Общий итог"),kolihsklad,sumasklad,metka_fn,fil_572s.ff);
      muinv_itogo572(gettext("Итого"),kolihsklad,sumasklad,metka_fn,fil_572.ff);

      fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(kol_na_liste));
      fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(itogo_list[0]));
      

            
      fprintf(fil_572.ff,"ICEB_LST_END\n");

      muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
      muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);

      sp_shet.free_class();
      kol_shet.free_class();
      sum_shet.free_class();

      /*записываем реквизиты концовки документа*/
      class iceb_vstavfil_rk rhd;

      rhd.zap(1,1,49,160,iceb_tu_nofwords(nomerstr));
      rhd.zap(7,1,80,160,iceb_tu_nofwords(kolihsklad));
      preobr(sumasklad,&suma_prop,0);
      rhd.zap(9,1,62,160,suma_prop.ravno());
      rhd.zap(25,1,23,63,posada.ravno());
      rhd.zap(25,1,98,160,matot.ravno());
      iceb_t_vstavfil(&rhd,"invmu572k.alx",fil_572.ff);
      iceb_t_vstavfil(&rhd,"invmu572k.alx",fil_572s.ff);

      fprintf(fil_572.ff,"\fICEB_LST_START\n");
      fprintf(fil_572s.ff,"\f");
     }


    naim_skl.new_plus("");
    matot.new_plus("");
    sprintf(strsql,"select naik,fmol,dolg from Sklad where kod=%s",skladf.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skladf.ravno_atoi());
      iceb_t_soob(strsql);
     }
    else
     {
      matot.new_plus(row1[1]);
      naim_skl.new_plus(row1[0]);
      posada.new_plus(row1[2]);
     }

    /*записываем реквизиты шапки документа*/
    class iceb_vstavfil_rk rhd;

    rhd.zap(1,1,0,100,naim00.ravno());
    rhd.zap(4,2,32,50,edrpou.ravno());
    rhd.zap(24,1,2,50,posada.ravno());

    rhd.zap(24,1,104,140,matot.ravno());
   

    /*вставляем заголовок файла*/
    iceb_t_vstavfil(&rhd,"invmu572z.alx",fil_572.ff);
    muinv_h_572(1,NULL,fil_572.ff);

    kolstr_572s=iceb_t_vstavfil(&rhd,"invmu572z.alx",fil_572s.ff);
    kolstr_572s+=iceb_t_insfil("invmu572h.alx",fil_572s.ff);
    kol_na_liste=0;

    sprintf(strsql,"%s:%s %s",gettext("Склад"),skladf.ravno(),naim_skl.ravno());     
    kolstriv=0;
    sapvin(matot.ravno(),mesqc.ravno(),gos,gos,&kolstriv,&kollistiv,strsql,ff);


    kolihsklad=sumasklad=0.;
    sklad.new_plus(skladf.ravno());
    shetu.new_plus("");
    nomerstr=0;
   }

  if(SRAV(shetu.ravno(),shetuf.ravno(),0) != 0)
   {
    if(shetu.ravno()[0] != '\0')
     {
      sapkk1iv(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1iv(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),kolihshet,sumashet);

     }
    naish.new_plus("");
    //Читаем наименование счета
    sprintf(strsql,"select nais from Plansh where ns='%s'",shetuf.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      naish.new_plus(row1[0]);
                  
    sapkk1iv(&kolstriv,&kollistiv,ff);
    fprintf(ff,"\n");
    sapkk1iv(&kolstriv,&kollistiv,ff);

    fprintf(ff,"%s: %s %s\n",gettext("Счёт"),shetuf.ravno(),naish.ravno());


    kolihshet=sumashet=0.;
    shetu.new_plus(shetuf.ravno());
   }

  cena=cenaf.ravno_atof();
  cena=okrug(cena,okrcn);
  
  summa=ostatok*cena;
  summa=okrug(summa,okrg1);
  
  kolihsklad+=ostatok;
  sumasklad+=summa;
  kolihshet+=ostatok;
  sumashet+=summa;
  
  if((nomer_sheta=sp_shet.find(shetu.ravno())) < 0)
   sp_shet.plus(shetu.ravno());
  sum_shet.plus(summa,nomer_sheta);      
  kol_shet.plus(ostatok,nomer_sheta);      

  if((nomer_sheta=sp_shet_i.find(shetu.ravno())) < 0)
   sp_shet_i.plus(shetu.ravno());
  sum_shet_i.plus(summa,nomer_sheta);      
  kol_shet_i.plus(ostatok,nomer_sheta);      
  
  
  rsdat(&dv,&mv,&gv,datvf.ravno(),2);

  dopstr.new_plus("");
  
  if(innom.ravno()[0] != '\0')
   {
    if( regnom.ravno()[0] == '\0')
     {
      dopstr.new_plus(gettext("Инвентарный номер"));
      dopstr.plus(":",innom.ravno());
      if(dv != 0)
       {
        sprintf(bros," %s:%d.%d.%d%s",gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(bros);
       }
     }
    else
     {
      dopstr.new_plus(gettext("Регистрационный номер"));
      dopstr.plus(":",regnom.ravno());

      sprintf(bros," %s:%s",gettext("Серия"),innom.ravno());
      dopstr.plus(bros);
      if(dv != 0)
       {
        sprintf(bros," %s:%d.%d.%d%s",gettext("Дата регистрации"),dv,mv,gv,gettext("г."));
        dopstr.plus(bros);
       }
     }
   }
  if(artikul.ravno()[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul.ravno());
    dopstr.plus(strsql);
   }

  sapkk1iv(&kolstriv,&kollistiv,ff);
  nomerstr++;
    
  fprintf(ff,"%4d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  nomerstr,
  iceb_tu_kolbait(39,naim.ravno()),iceb_tu_kolbait(39,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
  cena,ostatok,summa," "," "," "," ");

  if(dopstr.ravno()[0] != '\0')
   {
    sapkk1iv(&kolstriv,&kollistiv,ff);
    fprintf(ff,"%s\n",dopstr.ravno());
   }
  sapkk1iv(&kolstriv,&kollistiv,ff);

  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
 
  muinv_sh_572(&kolstr_572s,itogo_list,&kol_na_liste,naim.ravno(),metka_fn,fil_572s.ff); /*считаем по две строки*/

  kol_na_liste++;  
  itogo_list[0]+=ostatok; /*Именно здесь*/
  itogo_list[1]+=summa;
 
  if(metka_fn == 2) 
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11s|%12s|%12s|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_tu_kolbait(10,shetuf.ravno()),
   iceb_tu_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_tu_kolbait(20,innom.ravno()),
   iceb_tu_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_tu_kolbait(7,ei.ravno()),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   "",
   "",
   "",
   ostatok,
   cena,
   summa);
  else
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%12.12g|%12.2f|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_tu_kolbait(10,shetuf.ravno()),
   iceb_tu_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_tu_kolbait(20,innom.ravno()),
   iceb_tu_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_tu_kolbait(7,ei.ravno()),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   ostatok,
   cena,
   summa,
   ostatok,
   cena,
   summa);
  
  for(int nom=30; nom < iceb_tu_strlen(naim.ravno()); nom+=30)
   {
    kolstr_572s++;
    fprintf(fil_572s.ff,"%6s|%10s|%-*.*s|%20s|%7s|%11s|%12s|%12s|%11s|%12s|%12s|\n",
    "",
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
    "");
   }    

  if(metka_fn == 2)
   {
    for(int nom=0; nom < 160; nom++)
     fprintf(fil_572s.ff,".");
    fprintf(fil_572s.ff,"\n");  
   }
       
  if(metka_fn == 2) 
   fprintf(fil_572.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11s|%12s|%12s|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_tu_kolbait(10,shetuf.ravno()),
   iceb_tu_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_tu_kolbait(20,innom.ravno()),
   iceb_tu_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_tu_kolbait(7,ei.ravno()),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   "",
   "",
   "",
   ostatok,
   cena,
   summa);
  else
   fprintf(fil_572.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%12.12g|%12.2f|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_tu_kolbait(10,shetuf.ravno()),
   iceb_tu_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_tu_kolbait(30,naim.ravno()),
   iceb_tu_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_tu_kolbait(20,innom.ravno()),
   iceb_tu_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_tu_kolbait(7,ei.ravno()),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   ostatok,
   cena,
   summa,
   ostatok,
   cena,
   summa);
  
  for(int nom=30; nom < iceb_tu_strlen(naim.ravno()); nom+=30)
   fprintf(fil_572.ff,"%6s|%10s|%-*.*s|%20s|%7s|%11s|%12s|%12s|%11s|%12s|%12s|\n",
   "",
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
   "");
  if(metka_fn == 2)
   {
    for(int nom=0; nom < 160; nom++)
     fprintf(fil_572.ff,".");

    fprintf(fil_572.ff,"\n");  
   }
 }

fprintf(fil_572.ff,"ICEB_LST_END\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),kolihshet,sumashet);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%-*s %10.10g %10.2f\n",iceb_tu_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),kolihsklad,sumasklad);


/*Распечатка формы додаток 12*/
in_d12_it(&sp_shet_i,&kol_shet_i,&sum_shet_i,ffipsh);

koncinv(mesqc.ravno(),gos,ff);
podpis(ff);

fclose(ff);

iceb_t_insfil("in_d12_end.alx",ffipsh);
podpis(ffipsh);
fclose(ffipsh);


muinv_itogo572(gettext("Итого"),itogo_list[0],itogo_list[1],metka_fn,fil_572s.ff);
muinv_itogo572(gettext("Общий итог"),kolihsklad,sumasklad,metka_fn,fil_572s.ff);
muinv_itogo572(gettext("Итого"),kolihsklad,sumasklad,metka_fn,fil_572.ff);

fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(kol_na_liste));
fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(itogo_list[0]));

muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);






/*записываем реквизиты концовки документа*/
class iceb_vstavfil_rk rhd;

rhd.zap(1,1,49,160,iceb_tu_nofwords(nomerstr));
rhd.zap(7,1,80,160,iceb_tu_nofwords(kolihsklad));
preobr(sumasklad,&suma_prop,0);
rhd.zap(9,1,62,160,suma_prop.ravno());
rhd.zap(25,1,23,63,posada.ravno());
rhd.zap(25,1,98,160,matot.ravno());

iceb_t_vstavfil(&rhd,"invmu572k.alx",fil_572.ff);
iceb_t_vstavfil(&rhd,"invmu572k.alx",fil_572s.ff);

podpis(fil_572.ff);
fil_572.end();
podpis(fil_572s.ff);
fil_572s.end();

sprintf(strsql,"%s %s",imaf,imafipsh);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка инвентаризационных ведомостей"));
oth->spis_imaf.plus(imafipsh);
oth->spis_naim.plus(gettext("Дополнение 12"));
oth->spis_imaf.plus(imaf_572s);
oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций с итогами"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

oth->spis_imaf.plus(imaf_572);
oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций"));
iceb_t_ustpeh(imaf_572,3,&kolstr);
iceb_t_rnl(imaf_572,kolstr,"...",&muinv_h_572);

return(0);
}
/**********************************/
/*печать шапки страницы инвентаризационной ведомости для бюдженых организаций Наказ 572 от 17.06.2015р.*/
/******************************/
void muinv_h_572(int noml,int *kolstr,FILE *ff)
{
 
fprintf(ff,"%135s %s:%d\n","",gettext("Страница"),noml);

int kz=iceb_t_insfil("invmu572h.alx",ff);

if(kolstr != NULL)
 *kolstr=*kolstr+kz+2;

}
/*********************************************************************/
/*Счётчик инвентаризационной ведомости с итогами на каждой странице*/
/********************************************************************/
int muinv_sh_572(int *kolstr,double *itogo,int *kol_na_liste,const char *naim,int metka_fn,FILE *ff)
{
int kol=2;
if(metka_fn == 1)
 kol=1;

*kolstr+=kol; /*+строка подчёркивания*/

int kds=0; /*количество дополнительных строк*/
for(int nom=30; nom < iceb_tu_strlen(naim);nom+=30)
  kds++;
  

*kolstr+=kds;


if(*kolstr <= kol_strok_na_liste_l-3)
 return(0);

muinv_itogo572(gettext("Итого"),itogo[0],itogo[1],metka_fn,ff);

double kk=*kol_na_liste;

fprintf(ff,"Кількість порядкових номерів:%s\n",iceb_tu_nofwords(kk));
fprintf(ff,"Кількість одиниць:%s\n",iceb_tu_nofwords(itogo[0]));

fprintf(ff,"\f");

*kolstr=iceb_t_insfil("invmu572h.alx",ff)+kol+kds;

itogo[0]=itogo[1]=0.;
*kol_na_liste=0;

return(1);
}
/******************/
/*строка с итогами*/
/******************/
void muinv_itogo572(char *naim_itog,double kolih,double suma,int metka_fn,FILE *ff)
{
if(metka_fn == 2)
 fprintf(ff,"%*s|%11s|%12s|%12s|%11.11g|%12s|%12.2f|\n",
 iceb_tu_kolbait(77,naim_itog),
 naim_itog,
 "",
 "",
 "",
 kolih,
 "",
 suma);
else
 fprintf(ff,"%*s|%11.11g|%12s|%12.2f|%11.11g|%12s|%12.2f|\n",
 iceb_tu_kolbait(77,naim_itog),
 naim_itog,
 kolih,
 "",
 suma,
 kolih,
 "",
 suma);

}
/****************************************/
/*Распечатка по счетам*/
/*******************************/
void muinv_rps(class SPISOK *sp_shet,class masiv_din_double *kol_shet,class masiv_din_double *sum_shet,FILE *ff)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str naim_shet("");
char strsql[1024];

for(int nom=0; nom < sp_shet->kolih(); nom++)
 {
  /*узнаём наименование счёта*/
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(nom));
  if(readkey(strsql,&row,&cur) == 1)
   {
    naim_shet.new_plus(row[0]);
   }
  else
   naim_shet.new_plus("");



  fprintf(ff,"%-*.*s %-*.*s %10.10g %12.2f\n",
  iceb_tu_kolbait(10,sp_shet->ravno(nom)),
  iceb_tu_kolbait(10,sp_shet->ravno(nom)),
  sp_shet->ravno(nom),
  iceb_tu_kolbait(30,naim_shet.ravno()),
  iceb_tu_kolbait(30,naim_shet.ravno()),
  naim_shet.ravno(),
  kol_shet->ravno(nom),
  sum_shet->ravno(nom));  
 }
}
