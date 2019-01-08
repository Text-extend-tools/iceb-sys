/*$Id: zaravpr_old.c,v 5.12 2014/03/31 06:06:16 sasa Exp $*/
/*30.03.2016	23.09.1998	Белых А.И.	zaravpr.c
Выполнение проводок для заработной платы
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

void zaravprf(int,short,short,class SQLCURSOR *cur,short*,class SPISOK*,class masiv_din_double*,FILE *ff_prot);

extern short    *kodbl;  /*Коды начисления больничного*/
extern class iceb_tu_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/
extern short    *knnf; //Коды начислений недежными формами 
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern double   okrg; /*Округление*/
extern char	*mprov; /*Массив проводок*/
extern double	*mdk;   /*Массив дебетов и кредитов к масиву проводок*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short	kodbezf;  /*Код фонда занятости*/
extern short	kodsoc;   /*Код фонда социального страхования*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/
extern class iceb_tu_str shrpzbt; /*Счет расчётов по зарплате бюджет*/
extern class SPISOK dop_zar_sheta; //Дополнительные зарплатные счёта
extern class SPISOK dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/
extern int kekv_at;  //Код экономической классификации для атестованых
extern int kekv_nat; //Код экономической классификации для не атестованых
extern int kodzv_nat;//Код звания неатестованный
extern int kekv_nfo; //Код экономической классификации для начислений на фонд оплаты
extern class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/

void zaravpr_old(int tbn,short mp,short gp,FILE *ff_prot)
{
class iceb_tu_str nn("");
short		kp;
short		d,m,g;
short		dd,md,gd;
class iceb_tu_str kom("");
double		deb,kre;
class iceb_tu_str kor21("");
class iceb_tu_str kontshzar("");
int		i,i1;
time_t		vrem;
double		sym;
short		prn;
class iceb_tu_str sh("");
class iceb_tu_str shet("");
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kontr1("");
short		kodnu; /*Код начисления/удержания*/
class iceb_tu_str koresp("");
class iceb_tu_str bros("");
int		metpr; /*1-приход 2 расход*/
short		knah;
class iceb_tu_str kodgni("");
int		polekor; //Номер поля с кореспондирующими счетами
class iceb_tu_str koment("");
struct OPSHET	shetv;
int		kolpr=2;
class iceb_tu_str shetzar("");
class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;
SQLCURSOR curr;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kekv=0;

if(shrpz.ravno()[0] == '\0')
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s-Не введён зарплатный счёт в файле настройки!\n",__FUNCTION__);
  return;
 }
if(iceb_t_pvglkni(mp,gp) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Главная книга заблокирована!\n",__FUNCTION__);
  return;
 }

gd=gp;
md=mp;

nn.new_plus(mp);
nn.plus("-");
nn.plus(tbn);

dpm(&dd,&md,&gd,5);


//Проводки удаляем сразу так, как можно обнулить все начисления и удержания
if(udprgr(ICEB_MP_ZARP,dd,md,gd,nn.ravno(),0,0) != 0)
 return;

sprintf(strsql,"select datz,suma,shet,prn,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and suma != 0.",
gp,mp,gp,mp,tbn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
if(kolstr <= 0)
  return;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-\nИспользуется табличная настрой проводок\nkekv_nat=%d kekv_at=%d kekv_nfo=%d kodzv_nat=%d\n",__FUNCTION__,kekv_nat,kekv_at,kekv_nfo,kodzv_nat);        

if(kodzv_nat != 0 && (kekv_at != 0 || kekv_nat != 0)) //Узнаём звание и определяем kekv
 {
  sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
  if(readkey(strsql,&row1,&curr) == 1)
   {
    if(atoi(row1[0]) == kodzv_nat)
     kekv=kekv_nat;
    else
     kekv=kekv_at;

   }   
  else
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Не нашли звание !!!\n",__FUNCTION__);
   }
 }

if(ff_prot != NULL)
    fprintf(ff_prot,"%s-kekv=%d\n",__FUNCTION__,kekv);

/*Открываем файл настроек*/

sprintf(strsql,"select str from Alx where fil='zarnpr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarnpr.alx\n");
  return;
 }
short avkvs=1;
class iceb_tu_str kontr_baz(ICEB_ZAR_PKTN);
kontr_baz.plus(tbn);
kontshzar.new_plus(kontr_baz.ravno());
if(provsh(&kontshzar,shrpz.ravno(),&shetv,avkvs,0) != 0)
   return;

kontshzar.new_plus(kontr_baz.ravno());

if(shrpzbt.ravno()[0] != '\0')
 if(provsh(&kontshzar,shrpzbt.ravno(),&shetv,avkvs,0) != 0)
   return;


kp=0;
while(cur.read_cursor(&row) != 0)
 {
   
  if(ff_prot != NULL) 
   fprintf(ff_prot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  //Проверяем нужно ли делать проводки для этого начисления/удержания
  prn=atoi(row[3]);  
  
  if(row[2][0] == '\0')
   {
    if(prn == 1)
      sprintf(strsql,"%s %d %s %s!",gettext("Табельный номер"),tbn,gettext("Не введён счёт в начислении с кодом"),row[4]);
    if(prn == 2)
      sprintf(strsql,"%s %d %s %s!",gettext("Табельный номер"),tbn,gettext("Не введён счёт в удержании с кодом"),row[4]);
    iceb_t_soob(strsql);
    continue;
   }
  
  if(prn == 2) //Удержания
    sprintf(strsql,"select prov from Uder where kod=%s",row[4]);
  if(prn == 1) //Начисления
    sprintf(strsql,"select prov from Nash where kod=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    if(row1[0][0] == '1')
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Для кода %s проводки выключены!\n",row[4]);
      continue;
     }

  sym=atof(row[1]);
  if(fabs(sym) < 0.01)
    continue;

  sh.new_plus(row[2]);
  

//  printw("\nsh=%s\n",sh);
  
  shetzar.new_plus("");
  if(prn == 1)
   {
    if(proverka(shetb,sh.ravno(),0,1) == 0)
     {
      if(shrpzbt.ravno()[0] != '\0')
       shetzar.new_plus(shrpzbt.ravno());   
      else
       shetzar.new_plus(shrpz.ravno());   
     }
    else
      shetzar.new_plus(shrpz.ravno());   
   }
  else
   {
    if(proverka(shetbu,sh.ravno(),0,1) == 0)
     {
      if(shrpzbt.ravno()[0] != '\0')
       shetzar.new_plus(shrpzbt.ravno());   
      else
       shetzar.new_plus(shrpz.ravno());   
     }
    else
     shetzar.new_plus(shrpz.ravno());   
   }
  
  knah=atoi(row[4]);
  rsdat(&d,&m,&g,row[0],2);  

  deb=sym;
  if(prn == 2)
    deb*=(-1);
  kre=0.;
  kom.new_plus(row[5]);

  kontr1.new_plus("");
  kor21.new_plus("");

  class iceb_tu_str shet_zar_tek(""); //Счёт зарплаты текущий
  class iceb_tu_str kontr_shet_zar_tek(""); //Код контрагента для счёта зарплаты
    
  //Обязательно тут это копирование должно быть так как настроек в файле может и не быть
  shet_zar_tek.new_plus(shetzar.ravno());
  kontr_shet_zar_tek.new_plus(kontshzar.ravno());

  //Проверяем может это начисление для которого нужно делать проводки по дополнительному зарплатному счёту
  if(prn == 1)
   for(int ii=0; ii < dop_zar_sheta_spn.kolih(); ii++)
   if(proverka(dop_zar_sheta_spn.ravno(ii),knah,0,1) == 0)
    {
     shet_zar_tek.new_plus(dop_zar_sheta.ravno(ii));
     kontr_shet_zar_tek.new_plus(kontr_baz.ravno());
     if(ff_prot != NULL)
         fprintf(ff_prot,"1проверяем контрагент %s счёт %s\n",kontr_shet_zar_tek.ravno(),shet_zar_tek.ravno());
     if(provsh(&kontr_shet_zar_tek,shet_zar_tek.ravno(),&shetv,avkvs,0) != 0)
       continue;

    }  
  /*Читаем файл настройки*/
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {

    /*Счёт который будет браться для выполнения проводок- потому что в настройках проводок счёт может быть изменён*/ 
    if(row_alx[0][0] == '#')
      continue;
    if(row_alx[0][0] == 'f')
      continue;
   
    if(prn == 1 && row_alx[0][0] == '-')
     continue;
    if(prn == 2 && row_alx[0][0] == '+')
     continue;
     
    if(polen(row_alx[0],&bros,2,'|') != 0)
     continue;

    kodnu=bros.ravno_atoi();

    if(kodnu == 0 || kodnu != knah)
     continue;

    int nast_tn;
    if(polen(bros.ravno(),&nast_tn,2,':') == 0)
     {
        
      if(nast_tn != 0 && nast_tn != tbn) /*Если табельный номер в настройке не совпадает с табельным номером-пропускаем*/
        continue;
     }  
     
    if(polen(bros.ravno(),strsql,sizeof(strsql),3,':') == 0) //Введён счёт вместо зарплатного счёта (661)
     {
      
      if(strsql[0] != '\0')
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Введён счёт вместо зарплатного счёта=%s\n",strsql);
        shet_zar_tek.new_plus(strsql);
        if(provsh(&kontr_shet_zar_tek,shet_zar_tek.ravno(),&shetv,avkvs,0) != 0)
         continue;
       }
     }

     
    polekor=4;
    // Смотрим настройку на комментарий
    if(polen(bros.ravno(),strsql,sizeof(strsql),2,'/') == 0)
     {
      if(polen(strsql,&koment,1,':') != 0) 
       koment.new_plus(strsql);
      if(koment.ravno()[0] != '\0')
       {
        if(strstrm(row[5],koment.ravno()) != 0)
          polekor++;
       }
     }
    polen(row_alx[0],&bros,1,'|');
    i1=strlen(bros.ravno());
    metpr=0;
    for(i=0; i < i1; i++)
     {
      if(bros.ravno()[i] == '+')
       {
        metpr=1;
        break;
       }
      if(bros.ravno()[i] == '-')
       {
        metpr=2;
        break;
       }
     }

    if(metpr == 0 || metpr != prn)
     continue;


    /*Берем счет*/
    polen(row_alx[0],&bros,3,'|');

    if(polen(bros.ravno(),&shet,1,':') != 0)
     shet.new_plus(bros.ravno());
    else
     polen(bros.ravno(),&kontr1,2,':');

    if(shet.ravno()[0] == '\0' || strstrm(shet.ravno(),"*") == 1) /*если счёт не введён - значит настройка нам подходит и она для всех счетов этого начисления-удержания*/
     shet.new_plus(sh.ravno());    

    if(SRAV(shet.ravno(),sh.ravno(),1) != 0)
      continue;

    if(kontr1.ravno()[0] != '\0')
     kontr_shet_zar_tek.new_plus(kontr1.ravno());
    else
     kontr_shet_zar_tek.new_plus(kontr_baz.ravno());
        
    if(ff_prot != NULL)
     fprintf(ff_prot,"2проверяем контрагент %s счёт %s\n",kontshzar.ravno(),shet.ravno());
    if(provsh(&kontr_shet_zar_tek,shet.ravno(),&shetv,avkvs,0) != 0)
       continue;

    /*Берем кореспонденцию*/
    polen(row_alx[0],&koresp,polekor,'|');
    if(koresp.getdlinna() <= 1)
     continue;


    i1=pole2(koresp.ravno(),',');
    for(i=0; i <= i1; i++)
     {
      shet.new_plus("");

      if(i1 > 0)
       polen(koresp.ravno(),&bros,i+1,',');
      else
       bros.new_plus(koresp.ravno());      

      if(i1 > 0 && i == 0)
       i1--;
       
      /*Проверяем есть ли код контрагента*/
      if(polen(bros.ravno(),&shet,1,':') != 0)
       shet.new_plus(bros.ravno());
      else
       polen(bros.ravno(),&kor21,2,':');

      if(shet.ravno()[0] == '\0')
       continue;

      if(kor21.getdlinna() <= 1)
        kor21.new_plus(kontr_baz.ravno());

      if(ff_prot != NULL)
       fprintf(ff_prot,"3проверяем контрагент %s счёт %s\n",kor21.ravno(),shet.ravno());
      if(provsh(&kor21,shet.ravno(),&shetv,avkvs,1) != 0)
         continue;
   /****************
       1я проводка 
         делаем одиночную проводку и переходим на просмотр следующего 
         начисления/удержания
       Nя проводка
         делаем одиночную проводку и следующий в цепочке счёт должен 
         кореспонди
       ********************/
      kolpr=2;
      if(shetv.stat == 1)
        kolpr=1;
      
      if(prn == 1)
       {
        if(kolpr == 2)
          zapmpr(g,m,d,shet.ravno(),shet_zar_tek.ravno(),kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
        if(kolpr == 1)
          zapmpr(g,m,d,shet.ravno(),"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
        
       }
      if(prn == 2)
       {
        if(kolpr == 2)
         zapmpr(g,m,d,shet_zar_tek.ravno(),shet.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
        if(kolpr == 1)
         zapmpr(g,m,d,"",shet.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      if(kolpr == 2)
       {
        shet_zar_tek.new_plus(shet.ravno());
        kontr_shet_zar_tek.new_plus(kor21.ravno());
       }
      kor21.new_plus("");

     }
  // Настройка может быть только одна
    break;
   }
  
  if(kontr1.ravno()[0] != '\0')
    kor21.new_plus(kontr1.ravno());
#if 0
################################################33
  if(SRAV(sh.ravno(),shpn,0) == 0)
   {
    /*Читаем код города налоговой инспекции*/
    sprintf(strsql,"select kodg from Kartb where tabn=%d",tbn);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tbn);
      iceb_t_soob(strsql);
      continue;      
     }    

    /*Читаем код контрагента*/
    kodgni.new_plus("");
    if(atoi(row1[0]) != 0)
     {
      kodgni.new_plus(row1[0]);
      
      sprintf(strsql,"select kontr from Gnali where kod=%s",kodgni.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s %s !",
        gettext("Не найден код города налоговой инспекции"),kodgni.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"%s:%d !",
        gettext("Табельный номер"),tbn);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        continue;      
       }
      else
       kor21.new_plus(row1[0]);
     }
   }
#######################################
#endif
  if(kor21.getdlinna() <= 1)
    kor21.new_plus(kontr_baz.ravno());

  if(ff_prot != NULL)
    fprintf(ff_prot,"4проверяем контрагент %s счёт %s\n",kor21.ravno(),sh.ravno());

  if(provsh(&kor21,sh.ravno(),&shetv,avkvs,1) != 0)
     continue;
  kolpr=2;
  if(shetv.stat == 1)
    kolpr=1;

  if(prn == 1)
   {
    if(kolpr == 2)
      zapmpr(g,m,d,sh.ravno(),shet_zar_tek.ravno(),kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      zapmpr(g,m,d,sh.ravno(),"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
  if(prn == 2)
   {
    if(kolpr == 2)
      zapmpr(g,m,d,shet_zar_tek.ravno(),sh.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      zapmpr(g,m,d,"",sh.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
 }



/*Выполнение проводок по соцстаху*/
if(sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Расчёт проводок по социальным фондам до 1.1.2011 не выполняется !!!\n");
 }
else
 zaravprf(tbn,mp,gp,&cur_alx,&kp,&sp_prov,&sum_prov_dk,ff_prot);


dpm(&dd,&md,&gd,5);

/*Запись проводок из памяти в базу*/




kor21.new_plus(ICEB_MP_ZARP);
time(&vrem);

sprintf(strsql,"LOCK TABLES Prov WRITE,Alx READ,Blok READ,icebuser READ,Plansh READ");
class lock_tables lt(strsql);

zapmpr1(nn.ravno(),"",0,vrem,kor21.ravno(),dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot);

}

/**************************************/
/*Расчёт проводок по социальным фондам начиная с 1.1.2011*/
/**************************************/

void zaravprf(int tbn,short mp,short gp,class SQLCURSOR *cur_alx,
short *kolprov,
class SPISOK *sp_prov,
class masiv_din_double *sum_prov_dk,
FILE *ff_prot)
{
long		kolstr;
SQL_str         row,row2;
char		strsql[1024];
class iceb_tu_str kor11("");
class iceb_tu_str kor21("");;
class iceb_tu_str kontr1("");
class iceb_tu_str kontsocsh("");
class iceb_tu_str kontsocsh1("");
class iceb_tu_str kontsocsh2("");
short		kodz;
double		sum,sumi,sumas,sumap;
class iceb_tu_str socshet(""); /*Соц. счёт если он один*/
class iceb_tu_str socshet1(""); /*Соц. счёт не бюджетный*/
class iceb_tu_str socshet2(""); /*Соц. счёт бюджетный*/
class iceb_tu_str nahshet("");
short		d,m,g;
class iceb_tu_str koment("");
class iceb_tu_str koment1("");
double		deb,kre;
class iceb_tu_str shet("");
class iceb_tu_str shet1("");
class iceb_tu_str bros("");
class iceb_tu_str koresp("");
int		i,i1;
class iceb_tu_str kodnv("");
int		polekor; //Номер поля с кореспондирующими счетами
struct OPSHET   shetv;
int		kolpr=2;
double suma_for_esv=0.; /*Сумма начислений с которой берётся единый социальный взнос*/
double suma_for_esv_bol=0.; /*Cумма начислений больничного для единого социального взноса*/
SQLCURSOR curr;
class iceb_tu_str shet_zamena("");
int kekv=kekv_nfo;
SQL_str row_alx;
int metka_bol=0;
short avkvs=1;
if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Выполнение проводок на фонд зарплаты.\nkekv=%d\n",__FUNCTION__,kekv);
 
masiv_din_double SUMA;
class SPISOK NAHIS;
if(zaravpt_os(tbn,mp,gp,&NAHIS,&SUMA) != 0)
 return;
int kodf_esv_bol=0;
if(iceb_t_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx") != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден Код фонда единого социального взноса для больничных!\n");
 }
int kodf_esv_bol_inv=0;
if(iceb_t_poldan("Код фонда единого социального взноса с больничных для инвалидов",&kodf_esv_bol_inv,"zarnast.alx") != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден Код фонда единого социального взноса с больничных для инвалидов!\n");
 }

/*Смотрим записи социальных отчислений*/
sprintf(strsql,"select kodz,shet,sumas,sumap,sumapb from Zarsocz where datz='%04d-%d-01' and tabn=%d",gp,mp,tbn);
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr <= 0)
  return;

int kolzap=NAHIS.kolih();
/*В соответцтвии с законом Украины минимальная сумма с которой может начислятся ЕСВ это минимальная зарплата
Если сумма начисления меньше чем та с которой начислено ЕСВ то в расчёт нужно брать её
Определяем сумму болничных и сумму всех остальных начислений
*/
 
for(int nom=0; nom < kolzap; nom++)
 {
  polen(NAHIS.ravno(nom),&kodz,3,'|');  
  if(proverka(knvr_esv_r.ravno(),kodz,0,1) == 0)
   continue;

//  if(ff_prot != NULL)
//   fprintf(ff_prot,"%s-%s %.2f\n",__FUNCTION__,NAHIS.ravno(nom),SUMA.ravno(nom));
  sum=SUMA.ravno(nom);

  if(provkod(kodbl,kodz) >= 0)
   {   
    suma_for_esv_bol+=sum;
//    if(ff_prot != NULL)
//     fprintf(ff_prot,"%s-1 больничный %d %.2f\n",__FUNCTION__,kodz,sum);
    continue;
   }
  if(proverka(kod_dop_nah_bol.ravno(),kodz,0,1) == 0)
   {
    suma_for_esv_bol+=sum;
//    if(ff_prot != NULL)
//     fprintf(ff_prot,"%s-2 больничный %d %.2f-%s\n",__FUNCTION__,kodz,sum,kod_dop_nah_bol.ravno());
    continue;
   }   
  suma_for_esv+=sum;
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Сумма больничных %.2f Сумма остальных начислений %.2f Количество начислений=%d\n",__FUNCTION__,suma_for_esv_bol,suma_for_esv,kolzap);

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"\n%s-Запись фонда-%s %s %s %s %s\n\
-----------------------------------------------------------\n",__FUNCTION__,
    row[0],row[1],row[2],row[3],row[4]);

  kodz=atoi(row[0]);
  metka_bol=0;
  if(kodz == kodf_esv_bol || kodz == kodf_esv_bol_inv) /*фонд для больничного нужно смотреть только больничные*/
   metka_bol=1;

  socshet1.new_plus("");
  socshet2.new_plus("");
  socshet.new_plus(row[1]);
  if(polen(socshet.ravno(),&socshet1,1,',') == 0)
     polen(socshet.ravno(),&socshet2,2,',');

  sumas=atof(row[2]);
  sumap=atof(row[3]);


  /*Берем коды не входящие в расчёт*/
  kodnv.new_plus("");
  sprintf(strsql,"select kodn from Zarsoc where kod=%d\n",kodz);
  if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код соц. страхования"),kodz);
    iceb_t_soob(strsql);
    continue;
   }  
  else
   kodnv.new_plus(row2[0]);
   
  sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
  kontsocsh.new_plus(strsql);
  if(socshet1.getdlinna() <= 1 && socshet2.getdlinna() <= 1)
   if(provsh(&kontsocsh,socshet.ravno(),&shetv,avkvs,0) != 0)
     return;

  if(socshet1.getdlinna() > 1)
   {
    sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
    kontsocsh1.new_plus(strsql);
    if(provsh(&kontsocsh1,socshet1.ravno(),&shetv,avkvs,0) != 0)
     return;
   }
  if(socshet2.getdlinna() > 1)
   {
    sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
    kontsocsh2.new_plus(strsql);
    if(provsh(&kontsocsh2,socshet2.ravno(),&shetv,avkvs,0) != 0)
     return;
   }
  /*Повторно*/
  kor11.new_plus("");
  kor21.new_plus("");
  sumi=0.;

  for(int nomz=0; nomz < kolzap; nomz++)
   {

    if(fabs(sumi) > fabs(sumas))
      break;

     if(sumas < 0. && sumi < sumas)
      break;

    char dataz[16];
    class iceb_tu_str kodnah("");
    
    polen(NAHIS.ravno(nomz),&kodnah,3,'|');

    if(metka_bol == 0) /*если фонд не больничный то больничные пропускаем*/
     {
      if(provkod(kodbl,kodnah.ravno_atoi()) >= 0 || proverka(kod_dop_nah_bol.ravno(),kodnah.ravno(),0,1) == 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Код начисления %s Фонд не больничный - пропускаем\n",kodnah.ravno());
        continue;
       }

      if(suma_for_esv < sumas)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Сумма всех начислений меньше чем сумма с которой начислялся ЕСВ %.2f < %.2f\nБерём в расчёт меньшую сумму\n",__FUNCTION__,suma_for_esv,sumas);
        sumas=suma_for_esv; 
       }
     }

    if(metka_bol == 1) /*если фонд больничный то не больничные пропускаем*/
     {
      if(provkod(kodbl,kodnah.ravno_atoi()) < 0 && proverka(kod_dop_nah_bol.ravno(),kodnah.ravno(),0,1) != 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Код начисления %s Фонд больничный - пропускаем %s\n",kodnah.ravno(),kod_dop_nah_bol.ravno());
        continue;
       }
      if(suma_for_esv_bol < sumas)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Сумма всех начислений меньше чем сумма с которой начислялся ЕСВ %.2f < %.2f\nБерём в расчёт меньшую сумму\n",__FUNCTION__,suma_for_esv_bol,sumas);
        sumas=suma_for_esv_bol; 
       }
     }
    sum=SUMA.ravno(nomz);

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s = %.2f\n",NAHIS.ravno(nomz),sum);

    polen(NAHIS.ravno(nomz),dataz,sizeof(dataz),1,'|');

    /*Проверяем на коды не входящие в расчёт */
    if(kodnv.getdlinna() > 1)
     if(pole1(kodnv.ravno(),kodnah.ravno(),',',0,&i1) == 0 || SRAV(kodnv.ravno(),kodnah.ravno(),0) == 0)
       continue;

    rsdat(&d,&m,&g,dataz,2);


    polen(NAHIS.ravno(nomz),&nahshet,2,'|');
    polen(NAHIS.ravno(nomz),&koment,4,'|');
    kre=0.;

    if(fabs(sumi+sum) <= fabs(sumas))
     deb=sum;
    else
     {
      deb=sumas-sumi;
     }
    sumi+=sum;
    
    if(ff_prot != NULL)
     fprintf(ff_prot,"Сумма начисления %.2f\n\
Вычисляем сумму взноса которая приходится на эту сумму\n\
%.2f*%.2f/%.2f=%.2f\n",
      deb, 
      deb,sumap,sumas,
      deb*sumap/sumas);
      
    deb=deb*sumap/sumas;     //не округляем округлять будем при выполнении проводок

    kontr1.new_plus("");
    kor21.new_plus("");

    shet1.new_plus(socshet.ravno());
    
    kor11.new_plus(kontsocsh.ravno());
    if(socshet1.getdlinna() > 1)
     {
      shet1.new_plus(socshet1.ravno());
      kor11.new_plus(kontsocsh1.ravno());
     }   
    if(socshet2.getdlinna() > 1 && shetb != NULL)
      if(pole1(shetb,nahshet.ravno(),',',0,&i1) == 0 || SRAV(shetb,nahshet.ravno(),0) == 0)
       {
        shet1.new_plus(socshet2.ravno());
        kor11.new_plus(kontsocsh2.ravno());
       }
    if(fabs(deb) < 0.01)
     continue;

    shet_zamena.new_plus(""); //только здесь

    /*Читаем файл настройки*/
    cur_alx->poz_cursor(0);
    while(cur_alx->read_cursor(&row_alx) != 0)
     {
      
      if(row_alx[0][0] == '#')
        continue;        
      if(row_alx[0][0] == '+')
        continue;        
      if(row_alx[0][0] == '-')
        continue;        
      
      if(polen(row_alx[0],&bros,1,'|') != 0)
       continue;
 
      if(strstrm(bros.ravno(),"f") == 0)
       continue;

      /*Берем код*/
      polen(row_alx[0],&bros,2,'|');
      if(bros.ravno_atoi() != kodz)
        continue;         


      polekor=4;
      // Смотрим настройку на комментарий
      if(polen(bros.ravno(),strsql,sizeof(strsql),2,'/') == 0)
       {
        if(polen(strsql,&koment1,1,':') != 0)
         koment1.new_plus(strsql);
        if(koment1.getdlinna() > 1)
         {
          if(strstrm(koment.ravno(),koment1.ravno()) != 0)
            polekor++;
         }
       }     
      /*Смотрим табельный номер*/
      if(polen(bros.ravno(),strsql,sizeof(strsql),2,':') == 0)
       {
        if(proverka(strsql,tbn,0,0) != 0)
          continue;
       }
      /*Смотрим счёт в замену соц. счёта*/
      if(polen(bros.ravno(),strsql,sizeof(strsql),3,':') == 0)
       {
        if(strsql[0] != '\0')
         {
          shet1.new_plus(strsql);
          if(ff_prot != NULL)
           fprintf(ff_prot,"Заменили соц. счёт на %s\n",strsql);
          if(provsh(&kor11,shet1.ravno(),&shetv,avkvs,0) != 0)
              continue;
         }
       }
      /*Берем счет*/
      polen(row_alx[0],&bros,3,'|');
              
      if(polen(bros.ravno(),&shet,1,':') != 0)
       {
        shet.new_plus(bros.ravno());
       }
      else
       {
        polen(bros.ravno(),&kontr1,2,':');
        //Проверяем для какого кода начисления настройка
        class iceb_tu_str kod_nah_nast("");
        if(polen(bros.ravno(),&kod_nah_nast,4,':') == 0)
         if(proverka(kod_nah_nast.ravno(),kodnah.ravno(),0,0) != 0)
          continue;
       }        
      if(shet.getdlinna() <= 1 || strstrm(shet.ravno(),"*") == 1) /*Если счёт не введён то подходит*/
       shet.new_plus(nahshet.ravno());   
      
      if(SRAV(shet.ravno(),nahshet.ravno(),1) != 0)
       {
//        if(ff_prot != NULL)
//         fprintf(ff_prot,"%s\nНе равны счета %s != %s\n",row_alx[0],shet.ravno(),nahshet.ravno());
        kontr1.new_plus("");
        continue;
       }

      if(ff_prot != NULL)
       {
        fprintf(ff_prot,"%s",row_alx[0]);
        fprintf(ff_prot,"Обнаружили настройкy на счёт-%s код контрагента=%s\n",shet.ravno(),kontr1.ravno());
       }         
     //настройка на замену одного счета другим
     //делаем это только после сравнения счетов
     polen(bros.ravno(),&shet_zamena,3,':');//Только здесь 
     if(shet_zamena.ravno()[0] == '*')
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"1 Меняем на счёт в карточке\n");
       SQLCURSOR cursh;
       SQL_str   rowsh;
       //взять счёт из карточки
       sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
       if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
        {
         VVOD SOOB(1);
         sprintf(strsql,"%s-Не найдена настройка по дате %d.%d для определения счета в карточке !",__FUNCTION__,mp,gp);
         SOOB.VVOD_SPISOK_add_MD(strsql);
         sprintf(strsql,"Табельный номер %d",tbn);
         SOOB.VVOD_SPISOK_add_MD(strsql);
         soobshw(&SOOB,stdscr,-1,-1,0,1);
        }
       else
        shet_zamena.new_plus(rowsh[0]);
      }
     if(ff_prot != NULL)
      fprintf(ff_prot,"shet_zamena=%s\n",shet_zamena.ravno());
             
      /*Берем кореспонденцию*/
     polen(row_alx[0],&koresp,polekor,'|');
     if(koresp.getdlinna() <= 1)
       continue;
     if(ff_prot != NULL)
        fprintf(ff_prot,"Строка кореспондирующих счетов = %s\n",koresp.ravno());

     i1=pole2(koresp.ravno(),',');
     for(i=0; i <= i1; i++)
       {
        shet.new_plus("");         
        kor21.new_plus("");

        if(i1 > 0)
         polen(koresp.ravno(),&bros,i+1,',');
        else
         bros.new_plus(koresp.ravno());      

        if(ff_prot != NULL)
          fprintf(ff_prot,"Взяли счёт-%s\n",bros.ravno());

        if(i1 > 0 && i == 0)
         i1--;
         
        /*Проверяем есть ли код контрагента*/
        if(polen(bros.ravno(),&shet,1,':') != 0)
         shet.new_plus(bros.ravno());
        else
         polen(bros.ravno(),&kor21,2,':');

        if(ff_prot != NULL)
          fprintf(ff_prot,"Счёт-%s код контрагента-%s\n",shet.ravno(),kor21.ravno());

        if(shet.getdlinna() <= 1)
         continue;

        if(kor21.ravno()[0] == '\0')
         { 
          sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
          kor21.new_plus(strsql);
         }
        if(provsh(&kor21,shet.ravno(),&shetv,avkvs,1) != 0)
           continue;
        //Если счёт не балансовый то никаких проводок для соц-фондов быть не может
        kolpr=2;
        if(shetv.stat == 1)
           kolpr=1; 

        if(ff_prot != NULL)
          fprintf(ff_prot,"\n-shet=%s shet1=%s kor21=%s kor11=%s deb=%.2f kre=%.2f\nshet_zamena=%s\n",shet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena.ravno());

        if(kolpr == 2)
          zapmpr(g,m,d,shet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
        if(kolpr == 1)
          zapmpr(g,m,d,shet.ravno(),"",kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);

        if(kolpr == 2)
         {
          shet1.new_plus(shet.ravno());
          kor11.new_plus(kor21.ravno());
         }
        kor21.new_plus("");
       }
      break; //если сюда дошли значит проводку сделали и дальше смотеть не имеет смысла
     }

    
    if(kontr1.getdlinna() > 1)
     {
      kor21.new_plus(kontr1.ravno());
     }

    if(kor21.getdlinna() <= 1)
     {
      sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
      kor21.new_plus(strsql);
     }

    if(provsh(&kor21,nahshet.ravno(),&shetv,avkvs,1) != 0)
       continue;

    kolpr=2;
    if(shetv.stat == 1)
      kolpr=1;

    /*Если небыло цепочки проводок то он не заменён*/
    if(shet_zamena.ravno()[0] == '*')
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"2 Меняем на счёт в карточке shet_zamena=%s\n",shet_zamena.ravno());
      SQLCURSOR cursh;
      SQL_str   rowsh;
      //взять счёт из карточки
      sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
      if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s-Не найдена настройка по дате %d.%d для определения счета в карточке !",__FUNCTION__,mp,gp);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"Табельный номер %d",tbn);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
       }
      else
       shet_zamena.new_plus(rowsh[0]);
     }
    
    if(shet_zamena.ravno()[0] != '\0')
      nahshet.new_plus(shet_zamena.ravno());

    if(ff_prot != NULL)
      fprintf(ff_prot,"\n*nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f kre=%f\n\
shet_zamena=%s\n",nahshet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena.ravno());

    if(kolpr == 2)
      zapmpr(g,m,d,nahshet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
    if(kolpr == 1)
      zapmpr(g,m,d,nahshet.ravno(),"",kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
   }
 }
}
