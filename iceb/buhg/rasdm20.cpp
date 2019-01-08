/* $Id: rasdm20.c,v 5.25 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015	18.03.1999	Белых А.И.	rasdm20.c
Распечатка накладной на бланке строгой отчётности
Программа не локализована так, как это спецефический документ
украинского законодательства
*/

#define		KLST1 15
#define		KLST2 33
#include        <errno.h>
#include        "buhg.h"

void		vztr2(short,short,short,int,const char*,double *,FILE *,short,short,char[],int);
int             sortnak1(short,short,short,int,const char*);
int		shstr(int*,int*,int*,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern double   okrcn; /*Округление цены*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/

extern short	vtara; /*Код группы возвратная тара*/
extern short    obzap; /*0-не объединять записи 1-обединять*/

void rasdm20(short dd,short mm,short gg,int skl,const char *nomdok,int lnds)
{
char		bros[512];
char            str[512];
char            kop[112];
int		sag;
double		sag1;
FILE            *f1;
char            imaf[40];
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            pkod[112],pkod1[112]; /*Почтовый код*/
char            nmb[512],nmb1[512];  /*Наименование банка*/
char            mfo[32],mfo1[32];  /*МФО*/
char            nsh[32],nsh1[32];  /*Номер счета*/
char            inn[112],inn1[112];  /*Индивидуальный налоговый номер*/
char		grb[512],grb1[512];  /*Город банка*/
char		npnds[112],npnds1[112]; /*Номер плательщика НДС*/
char		tel[512],tel1[512];    /*Номер телефона*/
FILE		*ff;
char            bros1[512];
int             i,i1;
char            nomn[128];  /*Номенклатурный номер*/
double          itogo;
double          itogo1;
double          bb,bb1,bb2,bb3,bb4;
short		mnt; /*Метка наличия тары*/
double		sumt; /*Сумма по таре*/
short		vt; /*Код возвратной тары*/
char		kdtr[40]; /*Коды тары*/
short		mtpr; /*Метка первого и второго прохода*/
short		mtar; /*Метка наличия тары в накладной*/
short		mppu; /*Метка первого прохода услуг*/
char		imaf1[40];
int             kodm,kodmz,nk;
double		kol,sum;
double		kratn,kratz,cenan,cenaz,kolih,cena;
char		ein[56],eiz[56];
char		naim[512],naimz[512];
int		klst,klli;
double		sumkor;
short		kgrm;
double		nds;
char		shu[56];
int		kratd=216; /*Кратность дюйma*/
char		komanda[512];
char		naiei[512];
short		vots,lots;
int		kolzvn;
char		otpr[512]; /*Отпуск разрешил*/
char		glavb[512]; /*Главный бухгалтер*/
char		otpu[512]; /*Отпустил*/
char		kpos[56];
char		sod[512];
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
short		tp;
SQLCURSOR cur;

if((i=sortnak1(dd,mm,gg,skl,nomdok)) != 0)
 {
  VVOD SOOB(1);
  if(i == 1)  
   sprintf(strsql,"Кількість записів в документе більше ніж в бланке !");
  if(i == 2)  
   sprintf(strsql,gettext("Не найдено ни одной записи !"));

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  OSTANOV();
  return;
 }

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mm,dd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {

  sprintf(strsql,"%s-%s=%d %s=%s %s=%d %s!",
  __FUNCTION__,  
  gettext("Год"),
  gg,
  gettext("Документ"),
  nomdok,
  gettext("Склад"),
  skl,
  gettext("Не найден документ"));  
  
  iceb_t_soob(strsql);
  return;
 }
strncpy(kpos,row[3],sizeof(kpos)-1);
tp=atoi(row[0]);
vots=lots=0;
bros[0]='\0';
float pnds=atof(row[13]);
//sprintf(strsql,"%s",iceb_t_imafnsi("matnast.alx"));
if(iceb_t_poldan("Верхний отступ для накладной М-20",bros,"matnast.alx") == 0)
  vots=atoi(bros);

bros[0]='\0';
if(iceb_t_poldan("Левый отступ для накладной М-20",bros,"matnast.alx") == 0)
  lots=atoi(bros);

memset(otpr,'\0',sizeof(otpr));
iceb_t_poldan("Отпуск разрешил",otpr,"matnast.alx");

memset(glavb,'\0',sizeof(glavb));
iceb_t_poldan("Главбух",glavb,"matnast.alx");

memset(otpu,'\0',sizeof(otpu));
sprintf(bros,"Отпустил %d",skl);
iceb_t_poldan(bros,otpu,"matnast.alx");

vt=mnt=0;
memset(kdtr,'\0',sizeof(kdtr));
iceb_t_poldan("Код тары",kdtr,"matnast.alx");
   
memset(bros,'\0',sizeof(bros));


GDITE();
/*Читаем реквизиты организации свои */
sprintf(bros,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не знайдени реквізити по коду 00 !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;  
 }

strncpy(nmo,row[1],sizeof(nmo)-1);
strncpy(gor,row[3],sizeof(gor)-1);
strncpy(pkod,row[5],sizeof(pkod)-1);
strncpy(nmb,row[2],sizeof(nmb)-1);
strncpy(mfo,row[6],sizeof(mfo)-1);
strncpy(nsh,row[7],sizeof(nsh)-1);
strncpy(inn,row[8],sizeof(inn)-1);
strncpy(grb,row[4],sizeof(grb)-1);
strncpy(npnds,row[9],sizeof(npnds)-1);
strncpy(tel,row[10],sizeof(tel)-1);

memset(nmo1,'\0',sizeof(nmo1));
memset(gor1,'\0',sizeof(gor1));
memset(pkod1,'\0',sizeof(pkod1));
memset(nmb1,'\0',sizeof(nmb1));
memset(mfo1,'\0',sizeof(mfo1));
memset(nsh1,'\0',sizeof(nsh1));
memset(inn1,'\0',sizeof(inn1));
memset(grb1,'\0',sizeof(grb1));
memset(npnds1,'\0',sizeof(npnds1));
memset(tel1,'\0',sizeof(tel1));
/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(ATOFM(kpos) != 0. || kpos[0] != '0')
 {
  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos);
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"Не знайдени реквізити по коду %s !",kpos);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
  else
   {
    strncpy(nmo1,row[1],sizeof(nmo1)-1);
    strncpy(gor1,row[3],sizeof(gor1)-1);
    strncpy(pkod1,row[5],sizeof(pkod1)-1);
    strncpy(nmb1,row[2],sizeof(nmb1)-1);
    strncpy(mfo1,row[6],sizeof(mfo1)-1);
    strncpy(nsh1,row[7],sizeof(nsh1)-1);
    strncpy(inn1,row[8],sizeof(inn1)-1);
    strncpy(grb1,row[4],sizeof(grb1)-1);
    strncpy(npnds1,row[9],sizeof(npnds1)-1);
    strncpy(tel1,row[10],sizeof(tel1)-1);

   }

 }

sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

sprintf(imaf,"nak%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(f1);

fprintf(f1,"\x1B\x4D"); /*12-знаков*/
fprintf(f1,"\x0F"); /*Плотный режим печати*/


/*1/216 дюйма*/
if(kratd == 216)
 sprintf(komanda,"\x1B\x33");

/*1/72 Дюйма*/
if(kratd == 72)
 sprintf(komanda,"\x1B\x41");

sag1=kratd*(7+vots)/25.4;
sag=(int)okrug(sag1,1.);
fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/

fprintf(f1,"%*s%s",lots+5," ",nmo);

sag1=kratd*14/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
fprintf(f1,"%*s%s",lots+30," ",pkod);

memset(bros,'\0',sizeof(bros));
mesc(mm,1,bros);
i=gg-1990;
sag1=kratd*11/25.4;
sag=(int)okrug(sag1,1.);
fprintf(f1,"%s%c\n\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
fprintf(f1,"%*s%02d%8s%-25s%d",lots+95," ",dd," ",bros,i);

/*Свои реквизиты*/
sag1=kratd*9/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
sprintf(str,"%s %s Р.р.%s в %s м.%s МФО %s ",nmo,gor,nsh,nmb,grb,mfo);
fprintf(f1,"%*s %-120.120s %35s %s",lots+40," ",str," ",pkod);

/*Реквизиты заказчика*/
sag1=kratd*7.5/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
sprintf(str,"%s %s Р.р.%s в %s м.%s МФО %s ",nmo1,gor1,nsh1,nmb1,grb1,mfo1);
fprintf(f1,"%*s %-125.125s %35s %s",lots+35," ",str," ",pkod1);

/*Реквизиты получателя*/
sag1=kratd*7.5/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
sprintf(str,"%s %s Р.р.%s в %s м.%s МФО %s ",nmo1,gor1,nsh1,nmb1,grb1,mfo1);
fprintf(f1,"%*s %-135.135s %35s %s",lots+25," ",str," ",pkod1);

/*Основание*/
memset(str,'\0',sizeof(str));
sprintf(strsql,"select sodz from Dokummat2 where god=%d \
and sklad=%d and nomd='%s' and nomerz=3",gg,skl,nomdok);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  strncpy(str,row[0],sizeof(str)-1);
  
sag1=kratd*7.5/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
fprintf(f1,"%*s%s",lots+23," ",str);


/*Читаем доверенность*/

sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  POLE(row[0],bros,1,'#');
  POLE(row[0],str,2,'#');
 }
else
 {
  bros[0]='\0';
  str[0]='\0';
 }

/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

memset(sod,'\0',sizeof(sod));

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  strncpy(sod,row[0],sizeof(sod)-1);


sag1=kratd*7.5/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
fprintf(f1,"%*s%s %s %s",lots+25," ",sod,bros,str);


sag1=kratd*11/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c\n\n\n",komanda,sag); /*Устанавливаем межстрочный интервал*/

klst=0;
klli=1;

itogo=itogo1=0.;
mtar=mtpr=0;

naz:;
mnt=0;
kolzvn=0;
kol=sum=0.;
memset(eiz,'\0',sizeof(eiz));
memset(naimz,'\0',sizeof(naimz));
kratn=cenan=kratz=cenaz=kodmz=0;
sag1=kratd*5/25.4;
sag=(int)okrug(sag1,1.);
/*
printw("sag1=%f sag=%d\n",sag1,sag);
OSTANOV();
*/
while(fgets(str,sizeof(str),ff) != NULL)
 {
/*
  printw("%s",str);
  refresh();
*/
  POLE(str,bros,1,'|');
  kgrm=atoi(bros);
  POLE(str,naim,2,'|');
  POLE(str,bros,3,'|');
  kodm=(int)ATOFM(bros);
  POLE(str,bros,4,'|');
  nk=(int)ATOFM(bros);
  POLE(str,bros,5,'|');
  cenan=ATOFM(bros);
  POLE(str,bros,6,'|');
  kratn=ATOFM(bros);
  POLE(str,ein,7,'|');

  POLE(str,bros,8,'|');
  kolih=atof(bros);
  
  POLE(str,bros,9,'|');
  nds=atof(bros);

  POLE(str,shu,10,'|');

  if(nds == -1.)
   {
    mnt++;
    continue;
   } 

  if(kgrm == vt)
   {
    mnt++;
    continue;
   } 

  memset(bros,'\0',sizeof(bros));
  sprintf(bros,"%d",kgrm);
  if(kdtr[0] != '\0' && mtpr == 0 )
    if(pole1(kdtr,bros,',',0,&i1) == 0 || SRAV(kdtr,bros,0) == 0)
     {  
      mtar++;
      continue;
     }

  if(kdtr[0] != '\0' && mtpr == 1)
    if(pole1(kdtr,bros,',',0,&i1) != 0)
     if(SRAV(kdtr,bros,0) != 0)
       continue;

  /*Читаем наименование единицы измерения*/
  naiei[0]='\0';
  sprintf(strsql,"select naik from Edizmer where kod='%s'",ein);
  if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
   {
    beep();
    printw("Не знайшли найменування одиниці виміру %s\n",ein);
    OSTANOV();
   }
  else
    strncpy(naiei,row1[0],sizeof(naiei)-1);

  if(obzap == 1)
   {
    if((kodmz != 0 && kodmz != kodm) || (kratz != 0 && kratz != kratn) ||
    (cenaz != 0. && cenaz != cenan) || (eiz[0] != '\0' && SRAV(eiz,ein,0) != 0))
     {
      i++;
      fprintf(f1,"%*s %-*.*s %-*.*s %-*.*s %-*.*s %15.10g %18s %15s",
      lots+23,
      " ",
      iceb_tu_kolbait(77,naimz),iceb_tu_kolbait(77,naimz),naimz,
      iceb_tu_kolbait(15,nomn),iceb_tu_kolbait(15,nomn),nomn,
      iceb_tu_kolbait(14,eiz),iceb_tu_kolbait(14,eiz),eiz,
      iceb_tu_kolbait(10,naiei),iceb_tu_kolbait(10,naiei),naiei,
      kol," ",prcn(cenaz));
      
      fprintf(f1," %20s",prcn(sum));
      fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
      shstr(&klst,&klli,&kolzvn,f1);
     }
    if(kodmz != kodm || kratz != kratn || cenaz != cenan || 
     SRAV(eiz,ein,0) != 0 || SRAV(naimz,naim,0) != 0)
     {
      kol=sum=0.;
      kodmz=kodm;
      cenaz=cenan;
      kratz=kratn;
      strcpy(eiz,ein);
      strcpy(naimz,naim);
     }
   }
  
  sprintf(nomn,"%d.%s.%d.%d",skl,shu,kodm,nk);

  if(obzap == 1 && kol != 0.)
     sprintf(nomn,"%d.%s.%d.***",skl,shu,kodm);

  bb=cenan*kolih;
  bb=okrug(bb,okrg1);
  bb1=cenan+(cenan*pnds/100.);
  bb1=okrug(bb1,okrg1);

  if(obzap == 0)
   {
    strcpy(naimz,naim);
    i++;
    fprintf(f1,"%*s %-*.*s %-*.*s %-*.*s %-*.*s %12.10g %18s %15s",
    lots+23,
    " ",
    iceb_tu_kolbait(77,naimz),iceb_tu_kolbait(77,naimz),naimz,
    iceb_tu_kolbait(15,nomn),iceb_tu_kolbait(15,nomn),nomn,
    iceb_tu_kolbait(14,eiz),iceb_tu_kolbait(14,eiz),eiz,
    iceb_tu_kolbait(10,naiei),iceb_tu_kolbait(10,naiei),naiei,
    kolih," ",prcn(cenan));
    
    fprintf(f1," %23s",prcn(bb));
    fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
    shstr(&klst,&klli,&kolzvn,f1);
   }

  kol+=kolih;
  sum+=bb;

  itogo+=bb;

 }

if(obzap == 1)
 {
  i++;
  fprintf(f1,"%*s %-*.*s %-*.*s %-*.*s %-*.*s %12.10g %18s %15s",
  lots+23," ",
  iceb_tu_kolbait(77,naimz),iceb_tu_kolbait(77,naimz),naimz,
  iceb_tu_kolbait(15,nomn),iceb_tu_kolbait(15,nomn),nomn,
  iceb_tu_kolbait(14,eiz),iceb_tu_kolbait(14,eiz),eiz,
  iceb_tu_kolbait(10,naiei),iceb_tu_kolbait(10,naiei),naiei,
  kol," ",prcn(cenaz));
  fprintf(f1," %23s",prcn(sum));
  fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
  shstr(&klst,&klli,&kolzvn,f1);
 }

mtpr++;
if(mtpr == 1 && kdtr[0] != '\0' && mtar != 0) /*Распечатываем отдельно тару*/
 {
  /*Возврат для печати тары*/
  rewind(ff);
  goto naz;
 }

fclose(ff);
unlink(imaf1);

/*Распечатываем услуги*/
sprintf(strsql,"select naimu,kolih,cena,ei from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gg,mm,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


mppu=0;/*метка первого прохода услуг*/
SQLCURSOR cur1;
char naiusl[500];
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=okrug(cena,okrcn);

  memset(naiusl,'\0',sizeof(naiusl));
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     strncpy(naiusl,row1[0],sizeof(naiusl));
   }  
  if(naiusl[0] == '\0')
   strncpy(naiusl,row[7],sizeof(naiusl)-1);  
  else
   {
    strcat(naiusl," ");
    strcat(naiusl,row[7]);
   }


  /*Читаем наименование единицы измерения*/
  naiei[0]='\0';
  sprintf(strsql,"select naik from Edizmer where kod='%s'",row[3]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("Не знайшли найменування одиниці виміру %s\n",row[3]);
    OSTANOV();
   }
  else
    strncpy(naiei,row1[0],sizeof(naiei)-1);
    
  sprintf(nomn,"****");
  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;
  
  bb=okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=okrug(bb1,okrg1);


  fprintf(f1,"%*s %-*.*s %-*.*s %-*.*s %-*.*s %15.10g %18s %15s",
  lots+23," ",
  iceb_tu_kolbait(77,naiusl),iceb_tu_kolbait(77,naiusl),naiusl,
  iceb_tu_kolbait(15,nomn),iceb_tu_kolbait(15,nomn),nomn,
  iceb_tu_kolbait(14,row[3]),iceb_tu_kolbait(14,row[3]),row[3],
  iceb_tu_kolbait(10,naiei),iceb_tu_kolbait(10,naiei),naiei,
  kolih," ",
  prcn(cena));
  fprintf(f1," %20s",prcn(bb));
  fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/
  
  mppu++;
  
  itogo+=bb;

  shstr(&klst,&klli,&kolzvn,f1);

 }
fprintf(f1,"%*s%*s %10s\n",
lots+5," ",
iceb_tu_kolbait(199,"Всього:"),"Всього:",
prcn(itogo));

shstr(&klst,&klli,&kolzvn,f1);
kolzvn--;

itogo1=itogo;

  /*Читаем сумму корректировки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,13);

sumkor=0.;

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  sumkor=atof(row[0]);
  sumkor=okrug(sumkor,0.01);
 }

bb=bb1=bb2=bb3=bb4=0.;

if(sumkor != 0)
 {
  if(sumkor > 0.)
    bb=sumkor*100./itogo;
  if(sumkor < 0.)
    bb=100*sumkor/(itogo-sumkor*-1);
  bb=okrug(bb,0.01);
  if(sumkor < 0)
    sprintf(kop,"Знижка %.6g%% :",bb*(-1));
  if(sumkor > 0)
    sprintf(kop,"Надбавка %.6g%% :",bb);

  fprintf(f1,"%*s%199s %10.2f\n",lots+5," ",kop,sumkor);
  shstr(&klst,&klli,&kolzvn,f1);
  kolzvn--;

  if(sumkor < 0)
    sprintf(kop,"Всього із знижкою :");
  if(sumkor > 0)
    sprintf(kop,"Всього з надбавкою :");

  fprintf(f1,"%*s%199s %10.2f\n",lots+5," ",kop,sumkor+itogo);
  shstr(&klst,&klli,&kolzvn,f1);
  kolzvn--;
 }

bb=0.;
if(lnds == 0)
 {
  sprintf(kop,"ПДВ %.2f%% :",pnds);
  bb=(bb1+bb3+itogo+sumkor)*pnds/100.;
  bb=okrug(bb,0.01);
  fprintf(f1,"%*s%199s %10.2f\n",lots+5," ",kop,bb);

  shstr(&klst,&klli,&kolzvn,f1);
  kolzvn--;
 }
 
sumt=0.;
if(mnt != 0)
   vztr2(dd,mm,gg,skl,nomdok,&sumt,f1,vt,lots,komanda,sag);
      
itogo1=(itogo+sumkor)+bb1+bb3+bb+sumt;
strcpy(bros,"Всього до оплаты:");
if(tp == 1)
  strcpy(bros,"Всього:");

fprintf(f1,"%*s%*s %10.2f\n",
lots+5," ",
iceb_tu_kolbait(199,bros),bros,itogo1);
shstr(&klst,&klli,&kolzvn,f1);
kolzvn--;

if(klli == 1)
 {
  fprintf(f1,"\f");
  for(i=0; i < KLST2 ; i++)
    fprintf(f1,"\n");
 }
else
 {
/*
  printw("\nKLST2=%d - klst=%d = %d\n",KLST2,klst,KLST2-klst);
  OSTANOV();
*/
  for(i=0; i < KLST2-klst ; i++)
    fprintf(f1,"\n");

 } 

sag1=kratd*7/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c",komanda,sag); /*Устанавливаем межстрочный интервал*/

memset(str,'\0',sizeof(str));
bb=kolzvn;
preobr(bb,str,2);

POLE(str,bros,1,'0');
fprintf(f1,"%*s %s\n",lots+30," ",bros);

memset(str,'\0',sizeof(str));
preobr(itogo1,str,0);

/*Берем гривны*/
POLE(str,bros,1,'.');

/*Затираем грн.*/
i=strlen(bros);
bros[i-1]='\0';
bros[i-2]='\0';
bros[i-3]='\0';
bros[i-4]='\0';

/*Берем копейки*/
POLE(str,bros1,2,'.');
/*Затираем коп.*/
i=strlen(bros1);
bros1[i-1]='\0';
bros1[i-2]='\0';
bros1[i-3]='\0';
bros1[i-4]='\0';

fprintf(f1,"%*s %-100s %s\n",lots+20," ",bros,bros1);

fprintf(f1,"%*s %-140s %s\n",lots+30," ",otpr,glavb);

sag1=kratd*9/25.4;
sag=(int)okrug(sag1,1.);

fprintf(f1,"%s%c",komanda,sag); /*Устанавливаем межстрочный интервал*/
fprintf(f1,"%*s %s",lots+30," ",otpu);

fprintf(f1,"\x1B\x50"); /*10-знаков*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/

fclose(f1);
clearstr(LINES-1,0);


//rabfil("Накладная для бланка.",imaf);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка накладной форма М20"));
iceb_t_rabfil(&oth,"");


}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr2(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,short vt,short lots,char komanda[],int sag)
{
double		itogo,bb;
char		nomn[32];
char		naiei[512];
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
int		nk,kgrm;
double		kolih,cena;
char		ei[32];
char		naim[512];
int		vtr;
char		shu[32];
int		i;

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 

itogo=0.;

i=0;
while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование материалла*/
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     beep();
     printw("vztr-Не нашли наименование материалла по коду %s\n",row[4]);
     OSTANOV();
    }
  else
    strncpy(naim,row1[0],sizeof(naim)-1);

  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);

  /*Читаем наименование единицы измерения*/

  naiei[0]='\0';
  sprintf(strsql,"select naik from Edizmer where kod='%s'",row[3]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("Не знайшли найменування одиниці виміру %s\n",row[3]);
    OSTANOV();
   }
  else
    strncpy(naiei,row1[0],sizeof(naiei)-1);

  strcpy(shu,"**");
  
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%d",
    row[1],nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      strncpy(shu,row1[0],sizeof(shu)-1);
     }
   }

  sprintf(nomn,"%s.%s.%s.%d",row[1],shu,row[4],nk);

  bb=cena*kolih;
  bb=okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"Зворотъня тара:\n");

  fprintf(f1,"%*s %-*.*s %-*.*s %-*.*s %-*.*s %15.10g %18s %15s",
  lots+23," ",
  iceb_tu_kolbait(77,naim),iceb_tu_kolbait(77,naim),naim,
  iceb_tu_kolbait(15,nomn),iceb_tu_kolbait(15,nomn),nomn,
  iceb_tu_kolbait(14,ei),iceb_tu_kolbait(14,ei),ei,
  iceb_tu_kolbait(10,naiei),iceb_tu_kolbait(10,naiei),naiei,
  kolih," ",prcn(cena));
  
  fprintf(f1," %20s",prcn(bb));
  fprintf(f1,"%s%c\n",komanda,sag); /*Устанавливаем межстрочный интервал*/

  itogo+=bb;

 }
*sumt=itogo;

}

/***************/
/*Счетчик строк*/
/***************/
/*Если вернули 0 можно дальше печатать
               1 конец листа
*/
int shstr(int *klst,int *klli,int *kolzvn,FILE *f1)
{
*klst=*klst+1;
*kolzvn=*kolzvn+1;

if(*klli == 2 && *klst > KLST2)
  return(1);

if(*klli == 1 && *klst >= KLST1 )
 {
  fprintf(f1,"\x1B\x38"); /*Отключение контроля конца бумаги*/
  fprintf(f1,"\f");
  fprintf(f1,"\x1B\x39"); /*Включение контроля конца бумаги*/
  *klst=0;
  *klli=*klli+1;
 }
return(0);

}

/**********************/
/*Сортировка накладной*/
/**********************/
int sortnak1(short dd,short mm,short gg,int skl,const char *nomdok)
{
FILE		*ff;
char		imaf[32];
double		krat;
char		shu[32];
short		kols;
int		kodm,nk,kgrm;
double		kolih,cena,nds=0.;
char		ei[32];
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
char		naimat[512];

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mm,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(2);
 }

if(kolstr == 0)
 { 
  return(2);
 } 

sprintf(imaf,"sn%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(3);
 }

kols=0;
while(cur.read_cursor(&row) != 0)
 {

  krat=0.;

  /*Читаем наименование материалла*/
  memset(naimat,'\0',sizeof(naimat));
  sprintf(strsql,"select naimat,krat,kodgr from Material where kodm=%s",
  row[4]);
  if(readkey(strsql,&row1,&cur1) == 1)
   strncpy(naimat,row1[0],sizeof(naimat)-1);


  krat=atof(row1[1]);
  kgrm=atoi(row1[2]);

  kodm=atoi(row[4]);
  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  strncpy(ei,row[7],sizeof(ei)-1);

  strcpy(shu,"**");  
  if(nk != 0)
   {
    sprintf(strsql,"select krat,shetu from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      krat=atof(row1[0]);
      strncpy(shu,row1[1],sizeof(shu)-1);
     }
   }

  fprintf(ff,"%d|%s|%d|%d|%.6g|%.5g|%s|%.10g|%.10g|%s|\n",
  kgrm,naimat,kodm,nk,cena,krat,ei,kolih,
  nds,shu);

  kols++;

 }

fclose(ff);
if(kols+3 > KLST1 + KLST2)
 {
/*
  printw("kols=%d+3 > %d\n",kols,KLST1 + KLST2);
  OSTANOV();
*/
    return(1);
 }  

if(srtnk == 1)
 {
   /*Сортировка файла*/
   memset(strsql,'\0',sizeof(strsql));
//   sprintf(strsql,"sort -o %s -t\\| +0n +1b %s",imaf,imaf);
   sprintf(strsql,"sort -o %s -t\\| -k1,2n -k2,3b %s",imaf,imaf);
   runsystem(strsql);
 }

return(0);

}
