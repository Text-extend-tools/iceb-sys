/* $Id: zarsend.c,v 5.82 2013/09/26 09:43:45 sasa Exp $ */
/*16.06.2018	24.05.2000	Белых А.И.	zarsend.c
Автоматическая розноска зарплаты по карточкам
*/
#include        <errno.h>
#include        "buhg.h"

void hpzrs(int nom,int *kolstr,FILE *ff1);
void	strsen(int nomer,int tabn,const char *inn,const char *bankshet,const char *fio,double bbr,FILE *ff1);
double zarpv(short mr,short gr,int tabnom,double *sumab);


extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	mmm,ggg;
extern short	*obud; /*Обязательные удержания*/
short   	mfnn=1; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */

void		zarsend()
{
struct  tm      *bf;
char		imafprot[64];
int		kolz;
FILE		*ff1;
class iceb_tu_str bros("");
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str fio("");
short		prn;
int		knah;
static class iceb_tu_str tabel("");
static class iceb_tu_str kodn("");
static class iceb_tu_str podrz("");
static class iceb_tu_str dat1("");
static class iceb_tu_str kateg("");
static class iceb_tu_str maxsum("");
static class iceb_tu_str grupap("");
static class iceb_tu_str proc_vip("");
static class iceb_tu_str komentm("");
static class iceb_tu_str koment("***");
short		mr,gr;
int		kom,kom1;
double		itsal,ins,iu,inah,in1;
double          ius=0.,ius1=0.;   /*Итого удержано без учёта сальдо*/
double		nahshet=0.; //Итого начислено бюджет
double          iub=0.;   /*Итого удержано бюджета с учётом сальдо*/
double          iusb=0.;   /*Итого удержано без учёта сальдо бюджета*/
double		iusb1=0.;  /*Итого удержано без учёта сальдо бюджета */
class iceb_tu_str naim("");
int		tabb=0;
short		uhsl;
time_t          tmm;
int		podr=0;
double		sal;
double		itogo=0.,itogoo=0.;
int		im;
double		sym=0.,bbr=0.,bb=0.;
short		mz,gz;
short		dp,mp,gp; /*Дата перечисления зарплаты*/
class iceb_tu_str bankshet("");
double		saldb;
int		kodper=0;    //Код перечисления на карт-счёт
int             kod_v_kas=0; //Код выдачи из кассы
class iceb_tu_str shet_per(""),shet_per_b("");
class iceb_tu_str shet_v_kas(""),shet_v_kas_b("");
int		nomer=0;
class iceb_tu_str platpor("");
class iceb_tu_str dataper("");
short		uder;
class iceb_tu_str inn("");
double		dmaxsum=0.;
class ZARP     zp;
int		metkaras=0; //0-все 1-только хозрасчёт 2-только бюджет
class iceb_tu_str fiov("");


if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
if(proc_vip.ravno()[0] == '\0')
 proc_vip.new_plus("100");
 
kodper=0;
itsal=ins=ius=iu=inah=in1=0.;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Автоматическая рaзноска полученной зарплаты"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodn.ravno(),128);
VV.VVOD_SPISOK_add_data(tabel.ravno(),128);
VV.VVOD_SPISOK_add_data(kateg.ravno(),128);
VV.VVOD_SPISOK_add_data(dataper.ravno(),11);
VV.VVOD_SPISOK_add_data(platpor.ravno(),32);
VV.VVOD_SPISOK_add_data(maxsum.ravno(),20);
VV.VVOD_SPISOK_add_data(grupap.ravno(),128);
VV.VVOD_SPISOK_add_data(proc_vip.ravno(),32);
VV.VVOD_SPISOK_add_data(komentm.ravno(),36);

VV.VVOD_SPISOK_add_MD(gettext("Дата зарплаты..(м.г)......."));//0
VV.VVOD_SPISOK_add_MD(gettext("Подразделение...(,,)......."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код начисления..(,,)......."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,)......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код категории...(,,)......."));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата перечисления.(д.м.г).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Номер пла-ного документа..."));//6
VV.VVOD_SPISOK_add_MD(gettext("Максимальная сумма........."));//7
VV.VVOD_SPISOK_add_MD(gettext("Группа подразделения.(,,).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Процент выплати............"));//9
VV.VVOD_SPISOK_add_MD(gettext("Коментарий................."));//10

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
podrz.new_plus(VV.data_ravno(1));
kodn.new_plus(VV.data_ravno(2));
tabel.new_plus(VV.data_ravno(3));
kateg.new_plus(VV.data_ravno(4));
dataper.new_plus(VV.data_ravno(5));
platpor.new_plus(VV.data_ravno(6));
maxsum.new_plus(VV.data_ravno(7));
grupap.new_plus(VV.data_ravno(8));
proc_vip.new_plus(VV.data_ravno(9));
komentm.new_plus(VV.data_ravno(10));

switch (kom)
 {
  case ESC:
  case FK10:
     return;
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("zarp4_1_3.txt");
    goto naz;

  case PLU:
  case FK2:
     break;



  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка работников"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;


      case 0 :
        if(dirzarpodr(2,&podrz,&naim) == 0)
         VV.data_z_plus(1,podrz.ravno());
        goto naz;

      case 1 :
        if(dirnach(1,&kodn,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(2,kodn.ravno());
        goto naz;

      case 2 :

        clear();
        tabb=0;
        fiov.new_plus("");
        if(dirtab(&tabb,&fiov,0,1) == 0)
         {
          tabel.z_plus(tabb);
          VV.VVOD_SPISOK_zapis_data(3,tabel.ravno());

         }
        goto naz;

      case 3 :
        if(vibrek("Kateg",&kateg,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(4,kateg.ravno());

      case 4 :
        if(vibrek("Zargrupp",&grupap,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(8,grupap.ravno());
        goto naz;

     }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

dmaxsum=maxsum.ravno_atof();

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 { 
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 } 
double proc_vipl=proc_vip.ravno_atof();

SQLCURSOR curr;
dp=mp=gp=0;
if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
 {
  if(rsdat(&dp,&mp,&gp,VV.VVOD_SPISOK_return_data(5),1) != 0)
   { 
    iceb_t_soob(gettext("Не верно введена дата перечисления !"));
    goto naz;
   } 

  /*Читаем код перечисления*/
  if(iceb_t_poldan("Код перечисления зарплаты на счёт",&kodper,"zarnast.alx") != 0)
   {
    iceb_t_soob(gettext("Не найден код перечисления !"));
    goto naz;
   }

  if(iceb_t_poldan("Код получения зарплаты в кассе",&kod_v_kas,"zarnast.alx") != 0)
   {
    iceb_t_soob(gettext("Не найден код получения зарплаты в кассе !"));
    goto naz;
   }

  if(kod_v_kas == 0 && kodper == 0)
   {
    iceb_t_soob(gettext("Код получения зарплаты в кассе и код перечисления на счёт равны нолю !"));
    goto naz;
   }

   
/*
  printw("kodper=%d\n",kodper);
  OSTANOV();
*/
  
  //Читаем счёт перечисления на карт-счёт
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodper);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kodper);
    iceb_t_soob(strsql);
    goto naz;
   }
  shet_per.new_plus(row[0]);
  shet_per_b.new_plus(row[1]);

  //*Читаем счёт 
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_v_kas);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kod_v_kas);
    iceb_t_soob(strsql);
    goto naz;
   }
  shet_v_kas.new_plus(row[0]);
  shet_v_kas_b.new_plus(row[1]);

  if(shetbu != NULL)
   {
    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per.ravno());
    int i;
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {

      if(shet_per.getdlinna() > 1)
       sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet_per.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kodper,gettext("не введён хозрасчётный счёт в справочнике удержаний"));
      iceb_t_soob(strsql);       
      goto naz;
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per_b.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      if(shet_per_b.getdlinna() > 1)
       sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet_per_b.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kodper,gettext("не введён бюджетный счёт в справочнике удержаний"));
      iceb_t_soob(strsql);       
      goto naz;
     }

    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas.ravno());
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {
      if(shet_v_kas.getdlinna() > 1)
       sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet_v_kas.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kod_v_kas,gettext("не введён хозрасчётный счёт в справочнике удержаний"));
 
      iceb_t_soob(strsql);       
      goto naz;
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas_b.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      if(shet_v_kas_b.getdlinna() > 1)
       sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet_v_kas_b.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kod_v_kas,gettext("не введён бюджетный счёт в справочнике удержаний"));
      iceb_t_soob(strsql);       
      goto naz;
     }

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Хозрасчёт и бюджет"));
    MENU.VVOD_SPISOK_add_MD(gettext("Только хозрасчёт"));
    MENU.VVOD_SPISOK_add_MD(gettext("Только бюджет"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    metkaras=0;
    while(menu3w(stdscr,&MENU,&metkaras,-1,-1,0) != 0);
    if(metkaras == 3 || metkaras == -1)
      goto naz;    
   
   }
  
  if(kodper <= 0)
   {
    shet_per.new_plus(shet_v_kas.ravno());
    shet_per_b.new_plus(shet_v_kas_b.ravno());
    kodper=kod_v_kas;
   }

  if(kod_v_kas <= 0)
   {
    shet_v_kas.new_plus(shet_per.ravno());
    shet_v_kas_b.new_plus(shet_per_b.ravno());
    kod_v_kas=kodper;
   }
/*
  printw("shet=%s shet_per_b=%s\n",shet,shet_per_b);
  OSTANOV();
  return;
*/
 }

uhsl=0; /*Учет сальдо включен*/
if(iceb_t_poldan("Учет сальдо в распечатке",&bros,"zarnast.alx") == 0)
 if(SRAV(bros.ravno(),"Вкл",1) != 0)
   uhsl=1;  /*Учет сальдо выключен*/

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("С учетом сальдо ?"));

if(uhsl == 1)
 {
 if(danet(&DANET,2,stdscr) == 1)
  uhsl=0; /*Учет сальдо включен*/
 }
else
 if(danet(&DANET,2,stdscr) == 2)
  uhsl=1;/*Учет сальдо выключен*/

uder=2; /*Удержания не выполнять*/
if(dp != 0)
 {
  
  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD(gettext("Выполнить удержания ?"));
  uder=danet(&DANET,2,stdscr);

  if(iceb_t_pbpds(mp,gp) != 0)
   goto naz;

 }
 
clear();
if(komentm.getdlinna() > 1)
 koment.plus(" ",komentm.ravno());
 
time(&tmm);
bf=localtime(&tmm);
printw("%s:%.2f\n",gettext("Максимальная сумма"),dmaxsum);  
printw("%s\n",gettext("Сортируем записи."));
refresh();

sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d order by tabn asc",gr,mr);

SQLCURSOR cur;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr2 == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }


sprintf(imafprot,"send%d.lst",getpid());


if((ff1 = fopen(imafprot,"w")) == NULL)
 {
  error_op_nfil(imafprot,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Протокол разноски заработной платы"),0,0,0,0,mr,gr,ff1);

//Вывод шапки
hpzrs(1,NULL,ff1);
//hpzrs(mr,gr,ff1);

kolstr1=0;
podr=0;
sal=itogo=itogoo=0.;
nomer=kolz=kolstr1=0;
double viplata=0.;
double vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
double vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
int kod_zap=0;
class iceb_tu_str shet_zap("");
class iceb_tu_str shet_zap_b("");
class SQLCURSOR cur1;
double itogo_shet=0.;
double itogo_b_shet=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr2,++kolstr1);

  if(proverka(tabel.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(podrz.ravno(),row[1],0,0) != 0)
    continue;

  if(proverka(kateg.ravno(),row[2],0,0) != 0)
    continue;

  if(grupap.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Podr where kod=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     if(proverka(grupap.ravno(),row1[0],0,0) != 0)
       continue;
   }

  tabb=atol(row[0]);
  podr=atoi(row[1]);



  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%d",tabb);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabb);
    OSTANOV();
    continue;
   }
  fio.new_plus(row1[0]);
  inn.new_plus(row1[2]);
  bankshet.new_plus(row1[1]);
  if(bankshet.ravno()[0] == '\0')
   {
    shet_zap.new_plus(shet_v_kas.ravno());
    shet_zap_b.new_plus(shet_v_kas_b.ravno()); //это пока не сделано
    kod_zap=kod_v_kas;
   }
  else
   {
    shet_zap.new_plus(shet_per.ravno());
    shet_zap_b.new_plus(shet_per_b.ravno());
    kod_zap=kodper;
   }   
  sal=0.;
  /*Читаем сальдо*/

  sal=zarsald(1,mr,gr,tabb,&saldb);
   
  itsal+=sal;
  if(sal < 0. && uhsl == 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Внимание ! Отрицательное сальдо !"));
    sprintf(strsql,"%s:%d %s",gettext("Табельный номер"),tabb,fio.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }


  sprintf(strsql,"select datz,prn,knah,suma,godn,mesn,shet \
from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and suma != 0. order by prn,knah asc",
  gr,mr,gr,mr,tabb);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  
  if(kolstr == 0)
   {
    if(uhsl == 0 && sal > 0.)
     {
      sprintf(strsql,"%d %s %.2f\n%s",tabb,fio.ravno(),sal,gettext("Нет ни начислений ни удержаний! Пропускаем!"));
      iceb_t_soob(strsql);
     }  
  
    continue;
   }
  in1=inah=nahshet=iu=ins=ius=ius1=iub=iusb=0.;
  
  vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
  vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
  im=0;
  while(cur1.read_cursor(&row1) != 0)
   {

/*
    printw("%s %s %s %s %s %s %s %s\n",
    row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/
    
    prn=atoi(row1[1]);

    if(prn == 1)
     {
      if(proverka(kodn.ravno(),row1[2],0,1) == 0)
        continue;
     }

    knah=atoi(row1[2]);
    sym=atof(row1[3]);
    gz=atoi(row1[4]);
    mz=atoi(row1[5]);

    if(prn == 1)
     {
      in1+=sym;
      inah+=sym;
      ins+=sym;
      if(proverka(shetb,row1[6],0,1) == 0)
         nahshet+=sym;
     }

    if(prn == 2)
     {
      iu+=sym;
      if(proverka(shetbu,row1[6],0,1) == 0)
         iub+=sym;
      
      if(gz == gr && mz == mr)
       {
	ius+=sym;
        if(proverka(shetbu,row1[6],0,1) == 0)
          iusb+=sym;
       }
      else
       {
        if(provkod(obud,knah) >= 0)
         {
          ius+=sym;
          if(proverka(shetbu,row1[6],0,1) == 0)
            iusb+=sym;
         }
       }


     }


    im++;

   }

  if(im > 0 || sal != 0.)
   {



    if(uhsl == 0) /*Без учёта сальдо*/
     {
      //При расчёте ведомости этой проверки на выданную зарплату нет так как
      //нам нужно получить ведомость без учёта уже выданной зарплаты
      vid_zar=zarpv(mr,gr,(int)tabb,&iusb1);
      bbr=in1+iu+sal+vid_zar;

     }

    if(uhsl == 1) /*C yчётом сальдо*/
     {
      ius1=prbper(mr,gr,bf->tm_mon+1,bf->tm_year+1900,tabb,&iusb1,0);

      bbr=in1+ius+ius1;
     }

    if(bbr > 0.009)
     {


      viplata=bbr;
      if(proc_vipl != 100.)
       {
        viplata=bbr*proc_vipl/100.;
        viplata=okrug(viplata,okrg);
        }
      if(dmaxsum > 0.009) 
       if(viplata > dmaxsum)
        viplata=dmaxsum;

      kolz++;
      printw("%4d %-5ld %8.2f %-20s %s\n",podr,tabb,viplata,bankshet.ravno(),fio.ravno());
      refresh();

      itogo+=viplata;
      if(bankshet.getdlinna() <= 1)
       itogo_b_shet+=viplata;
      else
       itogo_shet+=viplata;                   
      strsen(++nomer,tabb,inn.ravno(),bankshet.ravno(),fio.ravno(),viplata,ff1);

      if(dp != 0 && uder == 1)
       {
        /*Проверяем записан ли стандартный набор*/
        provzzag(mp,gp,tabb,2,podr);
        zp.dz=dp;
        zp.mesn=mr; zp.godn=gr;
        zp.nomz=0;
        zp.podr=podr;
       }

      bb=0.;     

      if(shetbu != NULL && dmaxsum < 0.01 && shet_zap_b.ravno()[0] != '\0')
       {
       
        if(uhsl == 0 ) /*С учетом сальдо*/
         {
          bb=bbr-(nahshet+iub+saldb+vid_zar_b);

         }               
       
        if(uhsl == 1 ) /*Без учета сальдо*/
         {
          bb=in1-nahshet+(ius-iusb)+(ius1-iusb1);
         }

        bb=bbr-bb;
        if(bb != 0.)
         {
          fprintf(ff1,"%4s %-5s %8.2f/%.2f %s/%s\n",
          " "," ",bb,bbr-bb,shet_zap_b.ravno(),shet_zap.ravno());
         }       

        if(metkaras == 0 || metkaras == 2)
        if(dp != 0 && uder == 1)
        if(bb > 0.)
         {
          strcpy(zp.shet,shet_zap_b.ravno());
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=okrug(viplata,okrg);
            }

          if(dmaxsum > 0.009) 
           if(viplata > dmaxsum)
            viplata=dmaxsum;
         
          zapzarp(dp,mp,gp,tabb,2,kod_zap,viplata*-1,shet_zap_b.ravno(),mr,gr,0,0,koment.ravno(),podr,"",zp); 
         }
       }
      if(metkaras == 0 || metkaras == 1)
      if(dp != 0 && uder == 1)
       {
        bb=bbr-bb;
     
        /*Записываем код перечисления*/
        if(bb > 0.)
         {
//          printw("%d.%d.%d %d %d.%d %f\n",dp,mp,gp,tabb,mr,gr,bb);
          strcpy(zp.shet,shet_zap.ravno());
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=okrug(viplata,okrg);
           }

          if(dmaxsum > 0.009) 
           if(viplata > dmaxsum)
            viplata=dmaxsum;

          if(zapzarp(dp,mp,gp,tabb,2,kod_zap,viplata*-1,shet_zap.ravno(),mr,gr,0,0,koment.ravno(),podr,"",zp) != 0)
            printw("Ошибка ввода записи !!!\n");

         }
       }
      //Делаем проводки
      zaravpr(tabb,mp,gp,NULL);

     }

   }


 }
fprintf(ff1,"ICEB_LST_END\n");

GDITE();

itogoo+=itogo;

//Вывод концовки файла

fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f\n",iceb_tu_kolbait(74,gettext("Итого")),gettext("Итого"),itogoo);

fprintf(ff1,"%*s:%11.2f\n",
iceb_tu_kolbait(20,gettext("На карточки")),
gettext("На карточки"),itogo_shet);

fprintf(ff1,"%*s:%11.2f\n",
iceb_tu_kolbait(20,gettext("В кассу")),
gettext("В кассу"),itogo_b_shet);


podpis(ff1);


fclose(ff1);

printw("%*s:%11.2f\n",
iceb_tu_kolbait(20,gettext("На карточки")),
gettext("На карточки"),itogo_shet);

printw("%*s:%11.2f\n",
iceb_tu_kolbait(20,gettext("В кассу")),
gettext("В кассу"),itogo_b_shet);

printw("%*s:%11.2f\n",
iceb_tu_kolbait(20,gettext("Общий итог")),
gettext("Общий итог"),itogoo);

printw_vr(tmm);

int orient=0;
iceb_t_ustpeh(imafprot,3,&orient);
iceb_t_rnl(imafprot,orient,NULL,&hpzrs);


OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imafprot);
oth.spis_naim.plus(gettext("Протокол разноски заработной платы"));

iceb_t_ustpeh(imafprot,3);

iceb_t_rabfil(&oth,"");

}


/**********************************/
/*Шапка                           */
/**********************************/

void hpzrs(int nom,int *kolstr,FILE *ff1)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
Ном.|Табел.|Идентефик.|     Номер          |   Фамилия Имя Отчество       |  Сумма   |\n\
п/п |номер |  номер   | карточного счёта   |                              |          |\n"));
//   123456            12345678901234567890 123456789012345678901234567890 

fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");


}

/*************/
/*Вывод строки*/
/**************/

void strsen(int nomer,int tabn,const char *inn,const char *bankshet,const char *fio,double bbr,FILE *ff1)
{

fprintf(ff1,"%4d %-6d %-10s %-20s %-*.*s %10.2f\n",
nomer,
tabn,
inn,
bankshet,
iceb_tu_kolbait(30,fio),
iceb_tu_kolbait(30,fio),
fio,
bbr);

}

