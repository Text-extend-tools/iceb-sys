/* $Id: prospr.c,v 5.83 2014/02/28 05:13:47 sasa Exp $ */
/*12.12.2013    22.02.1994      Белых А.И.      prospr.c
Программа просмотра проводок и определения не сделанных
для материального учета
*/
#include        <math.h>
#include        "buhg.h"

short provpr(short d,short m,short g,const char *nn,int tp,int skk,class iceb_tu_str *matshet,int kolparshet,int kolshetvpd,int skl1);

extern double   okrg1; /*Округление*/
class masiv_din_int dp,mp,gp; /*Даты последнего подтверждения*/
class masiv_din_double snn; /*Суммы по документа*/
class masiv_din_double sn; /*Суммы подтвержденные в карточках*/
double          *snpar=NULL; /*Суммы подтвержденные в парном документе*/
class masiv_din_double sp; /*Суммы в проводках*/
double          *sppar=NULL; /*Суммы в проводках для парного документа*/
class masiv_din_double sbnds; /*Суммы без НДС*/
class SPISOK    spsh; /*Список счетов*/
class SPISOK    spshpar; //Список счетов в парном документе / счетов списания 
class SPISOK    vspsh; /*Список пар счетов для документов списания / внутреннего перемещения и изменения стоимости*/
double          *vsnn1=NULL; /*Суммы по приходному документа для пар счетов*/
double          *vsnn2=NULL; /*Суммы по расходному документа для пар счетов*/
double          *vsnn3=NULL; /*Суммы выполненых проводок для пар счетов*/
double		*kofusl=NULL; //Коэффициенты распределения услуг
double		sumusl=0.; //Сумма услуг по документу
extern double	okrcn;
extern short    vtara; /*Код группы возвратная тара*/
//extern char     *mtsh;  /*Перечень материальных счетов*/
extern class iceb_tu_str spmatshets; /*список материальных счетов*/
extern char	*kodopsp; //Коды операций сторнирования для приходов
extern char	*kodopsr; //Коды операций сторнирования для расходов

int             prospr(short mr, //0-Проверить выполнены проводки или нет
                                 //1-составить список проводок которые нужно сделать
                                 //2-распечатку на экран
				 //3-составить список и проверить
int skl, //Склад
const char *datdok, //Дата
const char *nn,  //Номер документа
int tp, //1-приход 2-расход*/
int lnds, //Метка льгот по НДС
int skl1, //Склад встречного документа
const char *nomdokv, //Номер встречного документа
const char *kodop) //Код операции
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,datdok,1);

return(prospr(mr,skl,d,m,g,nn,tp,lnds,skl1,nomdokv,kodop));
}
/*******************************************************************/
int             prospr(short mr, //0-Проверить выполнены проводки или нет
                                 //1-составить список проводок которые нужно сделать
                                 //2-распечатку на экран
				 //3-составить список и проверить
int skl, //Склад
short d,short m,short g, //Дата
const char *nn,  //Номер документа
int tp, //1-приход 2-расход*/
int lnds, //Метка льгот по НДС
int skl1, //Склад встречного документа
const char *nomdokv, //Номер встречного документа
const char *kodop) //Код операции
{
class iceb_tu_str shetu("");
int nom_sh=0;
char		bros[512];
short           d1=0,m1=0,g1=0;
short           dpd=0,mpd=0,gpd=0; //Дата парного документа
short           mpi=0; /*0 - удалить отметку >0 записать отметку*/
class iceb_tu_str sh1("");
double          cnn=0.,cnn1=0.,vcnn1=0.;
short           xv,yv;
double          bb;
short           mop=0; /*0- нет проводок >0 -есть*/
int             i,i1;
double          sumn=0.,cn=0.,vcn=0.;
long		kolstr;
SQL_str         row,row1,row2;
char		strsql[1024];
int		nk,nomkarpar;
int		kgrm;
double		nds;
short		dp1,mp1,gp1;
double		kolih,cena;
double		kolihpod=0.;
short		voz=0;
class iceb_tu_str matshet;
int		X=COLS-21,Y=1;
class iceb_tu_str vstrshet(""); //Встречный счет
static int	kolparshet; //Количество пар счетов
static int	kolshetvpd; //Количество счетов в парном документе
int		storno=0;
double cena_real=0.;
double cena_uh=0.;
static double suma_nacenki=0.;

static double	sumad=0.; //Общая сумма по документа

/*****************
printw("\nprospr-mr=%d skl=%d %d.%d.%d nn=%s tp=%d lnds=%d skl1=%d nomdokv=%s\n",
mr,skl,d,m,g,nn,tp,lnds,skl1,nomdokv);
OSTANOV();
**********************/
if(mr == 2) //Распечатку на экран
 {
  provpr(d,m,g,nn,tp,skl,&matshet,kolparshet,kolshetvpd,skl1);

  xv=X; yv=Y;
  bb=0.;
  
  for(i=0; i < spsh.kolih(); i++)
   {
    sh1.new_plus(spsh.ravno(i));
    
    if(fabs(sn.ravno(i)) < 0.01)
      attron(A_BLINK); //Включить мерцание
    move(yv++,xv);
    printw("%-*s %12s",iceb_tu_kolbait(8,sh1.ravno()),sh1.ravno(),prnbr(sn.ravno(i)));
    attroff(A_BLINK); //Выключить мерцание
    bb+=sn.ravno(i);
   }
  
  if(fabs(sumusl) > 0.009)
   {
    move(yv++,xv);
    printw("%-*s %12s",iceb_tu_kolbait(8,gettext("Услуги")),gettext("Услуги"),prnbr(sumusl));
    bb+=sumusl;
   }
  move(yv++,xv);
  printw("%-*s %12s",iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"),prnbr(bb));
  refresh();
  bb=0.;

  if(vsnn1 != NULL)
   {
    move(yv++,xv);
    
    if(skl1 != 0)
     {
      printw("%s:",gettext("Парний документ"));
      for(i=0; i < spshpar.kolih(); i++)
       {
        if(fabs(snpar[i]) < 0.01)
          attron(A_BLINK); //Включить мерцание
        move(yv++,xv);
        printw("%-*s %10.2f",iceb_tu_kolbait(10,spshpar.ravno(i)),spshpar.ravno(i),snpar[i]);
        attroff(A_BLINK); //Выключить мерцание
        bb+=snpar[i];
       }
     }
    else
     {
      printw("%s:",gettext("Счета списания"));

      for(i=0; i < vspsh.kolih() ; i++)
       {
        if(fabs(vsnn2[i]) < 0.01)
          attron(A_BLINK); //Включить мерцание
        move(yv++,xv);
        printw("%-*s %10.2f",iceb_tu_kolbait(10,vspsh.ravno(i)),vspsh.ravno(i),vsnn2[i]);
        attroff(A_BLINK); //Выключить мерцание
        bb+=vsnn2[i];
       }
     }
    move(yv++,xv);
    /*Cумма итого может быть больще по количеству знаков поэтому 8 а не 10*/
    printw("%-*s %12s",iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"),prnbr(bb));
   }

  if(suma_nacenki > 0.)
   {
    move(yv++,xv);
    /*Cумма итого может быть больще по количеству знаков поэтому 8 а не 10*/
    printw("%-*s %12s",iceb_tu_kolbait(8,gettext("Наценка")),gettext("Наценка"),prnbr(suma_nacenki));
   }   
  refresh();
  return(0);
 }

if(mr == 1 || mr == 3) //Составить список проводок которые надо сделать
 {
  suma_nacenki=0.;
  SQLCURSOR cur;
  SQLCURSOR cur1;
  storno=0;
  if(tp == 1)
   if(proverka(kodopsp,kodop,0,1) == 0)
     storno=1;
  if(tp == 2)
   if(proverka(kodopsr,kodop,0,1) == 0)
     storno=1;
  if(storno == 1) //Ишем дату документа по которому произошла реализация
   {
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=14",g,nn,skl);
    if(readkey(strsql,&row,&cur) > 0)
      rsdat(&dpd,&mpd,&gpd,row[0],1);

   }         

  kolparshet=0;
  kolshetvpd=0;
  sumad=0.;  
  sprintf(strsql,"select nomkar,kolih,cena,nds,kodm,nomkarp,shet from \
Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",g,m,d,skl,nn);  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }

  if(kolstr == 0)
    return(0);

  xv=X; yv=Y;
  move(14,0);
  spsh.free_class();
  spshpar.free_class();
  vspsh.free_class();
  sp.free_class();
  snn.free_class();
  sn.free_class();  
  sbnds.free_class();
  dp.free_class();
  mp.free_class();
  gp.free_class();
  

  while(cur.read_cursor(&row) != 0)
   {
    /*Читаем карточку материалла*/
    nk=atoi(row[0]);
    nds=atof(row[3]);
    vstrshet.new_plus(row[6]);
    sumad+=atof(row[1])*atof(row[2]);
    if(tp == 1 && storno == 1 && nomdokv[0] != '\0' && dpd != 0)
     {
      //Читаем в документе какая там была отпускная цена
      sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d'\
and nomd='%s' and kodm=%s and tipz=2",gpd,mpd,dpd,nomdokv,row[4]);
      if(readkey(strsql,&row1,&cur1) <= 0)
       {
        sprintf(strsql,"%s N %s %s %s !",gettext("В документе"),nomdokv,gettext("нет кода материалла"),row[4]);
        iceb_t_soob(strsql);
        continue;
       }
      sumn=cn=atof(row1[0]);
      
     }
    else
      sumn=cn=atof(row[2]);
    cena_uh=cena_real=0.;    
    vcn=0.;
    nomkarpar=atoi(row[5]);
    //Определяем стоимость во встречном документе
    if(tp == 2 && skl1 != 0 && nomdokv[0] != '\0' && nk != 0)
     {
      vstrshet.new_plus("");
      if(nomkarpar != 0)
        sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d' \
and sklad=%d and nomd='%s' and kodm=%s and tipz=1 and nomkar=%d",
        g,m,d,skl1,nomdokv,row[4],nomkarpar);
      else
        sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d' \
and sklad=%d and nomd='%s' and kodm=%s and tipz=1",
        g,m,d,skl1,nomdokv,row[4]);
      if(readkey(strsql,&row1,&cur1) > 0)
       {
        vcn=atof(row1[0]);
        vcn=okrug(vcn,okrcn);
        //Определяем счёт в карточке
        sprintf(strsql,"select shetu,cena,cenap from Kart where sklad=%d and nomk=%d",skl1,nomkarpar);

        if(readkey(strsql,&row1,&cur1) > 0)
         {
          vstrshet.new_plus(row1[0]);
          cena_uh=atof(row1[1]);
          cena_real=atof(row1[2]);
         }
       }
     }

    if(nk != 0)
     {
      sprintf(strsql,"select shetu,krat,cena from Kart where sklad=%d and nomk=%d",skl,nk);
      if(readkey(strsql,&row1,&cur1) != 1)
       {
        sprintf(bros,"%s-%s %d %s %d\n",__FUNCTION__,gettext("Не нашли карточки"),nk,gettext("склад"),skl);
        iceb_t_soob(bros);
       }
     }
    else
      continue;

    shetu.new_plus(row1[0]);
    if((nom_sh=spsh.find(shetu.ravno())) < 0)
     {
      spsh.plus(row1[0]);
      sp.plus(0.,-1);
      sn.plus(0.,-1); /*должны быть синхронны*/
     }

    sh1.new_plus(row1[0]);      
    cn=atof(row1[2]);

    /*Определяем группу материалла*/
    sprintf(strsql,"select kodgr from Material where kodm=%s",row[4]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      sprintf(bros,"%s-%s %s",__FUNCTION__,gettext("Не найден код материалла"),row[4]);
      iceb_t_soob(bros);
     }
    
    kgrm=atoi(row1[0]);


    /*Читаем количество в карточке*/
    
    kolihpod=0.;
     
    sprintf(strsql,"select datdp,kolih from Zkart where sklad=%d and \
nomk=%d and nomd='%s' and datd='%d-%02d-%02d' order by datdp asc",
    skl,nk,nn,g,m,d);
    SQLCURSOR cur1;
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,__FUNCTION__,strsql);
     }
    vcnn1=cnn1=cnn=0.;
    d1=m1=g1=0;
    while(cur1.read_cursor(&row2) != 0)
     {
      kolih=atof(row2[1]);
      kolihpod+=kolih;
      bb=kolih*cn;
      bb=okrug(bb,okrg1);
      cnn+=bb;
      
      bb=kolih*sumn;
      bb=okrug(bb,okrg1);
      cnn1+=bb;

      bb=kolih*vcn;
      bb=okrug(bb,okrg1);
      vcnn1+=bb;
      if(cena_real > 0.)      
        suma_nacenki+=(cena_real-cena_uh)*kolih;
      rsdat(&dp1,&mp1,&gp1,row2[0],2);
      if(sravmydat(d1,m1,g1,dp1,mp1,gp1) < 0)
       {
        d1=dp1; m1=mp1; g1=gp1;
       }

     }
    snn.plus(cnn1,nom_sh);
    if(nds < 0. || (kgrm == vtara && vtara != 0) || lnds > 0)
      sbnds.plus(cnn1,nom_sh);
    else
      sbnds.plus(0.,nom_sh);
    
    sn.plus(cnn,spsh.find(shetu.ravno()));

    if(nom_sh < 0 || (nom_sh >=0 && d1 != 0)) /*чтобы уже существующая дата не забилась нулевой датой если не все записи в документе подтверждены*/
     {    
      dp.new_plus(d1,nom_sh); 
      mp.new_plus(m1,nom_sh);
      gp.new_plus(g1,nom_sh);
     }
    /********Работа со счетами парного документа **********/

    if(vstrshet.ravno()[0] != '\0')
     {
      if( skl1 != 0 && nomdokv[0] != '\0' && nk != 0 ) //Внутреннее перемещение
       {
        //Смотрим количество подтвержденное во встречном документе
        bb=readkolk(skl1,nomkarpar,d,m,g,nomdokv);  

        //Если в расходном документе несколько записей ссылаются на одну в приходном
        if(kolihpod < bb)
          bb=kolihpod;

        bb=bb*vcn;
       }       
      else
       bb=kolihpod*cnn1;
       
      bb=okrug(bb,okrg1);
      if((i1=spshpar.find(vstrshet.ravno())) >= 0)
       {
        if( skl1 != 0 && nomdokv[0] != '\0' && nk != 0 ) //Внутреннее перемещение
          snpar[i1]+=bb;
       }       
      else
       {
        if(spshpar.find_r(vstrshet.ravno()) < 0)
         {
          spshpar.plus(vstrshet.ravno());
          kolshetvpd++;
         }
        if((snpar=(double *)realloc(snpar,kolshetvpd*sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s snpar !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(0);
         }
        if((sppar=(double *)realloc(sppar,kolshetvpd*sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s sppar !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(0);
         }
        snpar[kolshetvpd-1]=bb;
       }
      sprintf(bros,"%s,%s",sh1.ravno(),vstrshet.ravno());
      if((i1=vspsh.find(bros)) >= 0)
       {
        vsnn1[i1]+=vcnn1;
        vsnn2[i1]+=cnn1;
       }
      else
       {
        sprintf(bros,"%s,%s",sh1.ravno(),vstrshet.ravno());

        if(vspsh.find_r(bros) < 0)
         {
          kolparshet++;
          vspsh.plus(bros);
         }
        if((vsnn1=(double *)realloc(vsnn1,kolparshet*sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s vsnn1 !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(0);
         }
        if((vsnn2=(double *)realloc(vsnn2,kolparshet*sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s vsnn2 !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(0);
         }
        if((vsnn3=(double *)realloc(vsnn3,kolparshet*sizeof(double))) == NULL)
         {
          sprintf(strsql,"%s-%s vsnn3 !",__FUNCTION__,gettext("Не могу выделить память для"));
          iceb_t_soob(strsql);
          return(0);
         }
        vsnn1[kolparshet-1]=vcnn1;
        vsnn2[kolparshet-1]=cnn1;
        vsnn3[kolparshet-1]=0.;
       }
     }    
   }


  /*Смотрим сумму услуг, если они есть*/ 

  sumusl=0;

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"select kolih,cena from Dokummat3 where datd='%d-%02d-%02d' and \
  sklad=%d and nomd='%s'",g,m,d,skl,nn);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
   }

  while(cur.read_cursor(&row) != 0)
   {
    kolih=atof(row[0]);
    cena=atof(row[1]);
    cena=okrug(cena,okrcn);
    
    if(kolih > 0)
      bb=kolih*cena;
    else
      bb=cena;

    bb=okrug(bb,okrg1);
    sumusl+=okrug(bb,okrg1);

   }

  if(sumusl != 0.)
    sumusl=okrug(sumusl,0.01);

  kofusl=NULL;  
  if(sn.kolih() > 0)
    kofusl=new double[sn.kolih()];
  bb=sn.suma();
  for(i=0; i < sn.kolih() ; i++)
   {
    kofusl[i]=sn.ravno(i)/bb;
   }


  if(mr == 1)
    return(0);
 
 }
/******************************************/
  /*Ищем проводки*/
if( mr == 0 || mr == 3) //Проверить выполнены все проводки или нет
 {
  GDITE();
  mop=mpi=0;
      
  mop=provpr(d,m,g,nn,tp,skl,&matshet,kolparshet,kolshetvpd,skl1);
  VVOD SOOB(1);

  if( mop > 0 )
   {
    if(matshet.getdlinna() > 1)
     {
      sprintf(strsql,"%s %s!",gettext("В документе нет материалла со счетом"),matshet.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
      mpi++;
     }

    if(skl1 != 0 && sppar != NULL)
     {
      for(i=0; i < spsh.kolih() ; i++)
       {
        sh1.new_plus(spsh.ravno(i));

        if(fabs(sp.ravno(i) - sn.ravno(i)) > 0.009)
         {
          sprintf(strsql,"%s %s %s! %.14g != %.14g %d/%d",
          gettext("По счёту"),
          sh1.ravno(),
          gettext("не сделаны все проводки"),
          sp.ravno(i),
          sn.ravno(i),i,spsh.kolih());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          mpi++;
         }
        }
      for(i=0; i < kolshetvpd ; i++)
       {
        sh1.new_plus(spshpar.ravno(i));
        if(fabs(sppar[i] - snpar[i]) > 0.009)
         {
          sprintf(strsql,"%s %s %s! %.14g != %.14g %d/%d",
          gettext("По счёту"),
          sh1.ravno(),
          gettext("не сделаны все проводки"),
          sppar[i],snpar[i],i,kolshetvpd);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          mpi++;
         }
        }
     }
    else
     for(i=0; i < spsh.kolih() ; i++)
      {
       sh1.new_plus(spsh.ravno(i));
       if(fabs(sp.ravno(i) - sn.ravno(i)) > 0.009)
        {
         beep();
         sprintf(strsql,"%s %s %s! %.14g != %.14g %d/%d",
         gettext("По счёту"),
         sh1.ravno(),
         gettext("не сделаны все проводки"),
         sp.ravno(i),sn.ravno(i),i,spsh.kolih());
         SOOB.VVOD_SPISOK_add_MD(strsql);
         mpi++;
        }
      }
   
   }
  if(mpi > 0)
   {
    sprintf(strsql,"%s %s %s %d.%d.%d %s",
    gettext("Документ"),
    nn,
    gettext("дата"),
    d,m,g,
    gettext("г."));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }

if(mop == 0 && sumad != 0.)
  mpi++;

sumusl=0.;

if(sppar != NULL)
 {
  free(sppar);
  sppar=NULL;
 }
if(snpar != NULL)
 {
  free(snpar);
  snpar=NULL;
 }

if(kofusl != NULL)
 {
  delete(kofusl);
  kofusl=NULL;
 }
if(vsnn1 != NULL)
 {
  free(vsnn1);
  vsnn1=NULL;
 }
if(vsnn2 != NULL)
 {
  free(vsnn2);
  vsnn2=NULL;
 }
if(vsnn3 != NULL)
 {
  free(vsnn3);
  vsnn3=NULL;
 }

/*
printw("\n***mpi-%d mop-%d\n",mpi,mop);
OSTANOV();
  */

voz=0;
if(mpi == 0 )
 {
  /*Проводки выполнены*/
  sprintf(strsql,"update Dokummat \
set pro=1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,nn);
  voz=1;
 }
else
 {
  /*Проводки не выполнены*/
  voz=0;
  sprintf(strsql,"update Dokummat \
set pro=0 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,nn);
 }

/*Если документ заблокирован то никаких меток не ставим*/
if(iceb_t_pbpds(m,g,1) == 0) //Дата не заблокирована
 {
  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
 }
return(voz);
}

/****************************/
/*Поиск выполненных проводок*/
/****************************/
short provpr(short d,short m,short g,
const char *nn,int tp,int skk,class iceb_tu_str *matshet,int kolparshet,int kolshetvpd,int skl1)
{
class iceb_tu_str sh1("");
int             i,i1;
short           mop;
long		kolstr;
SQL_str         row;
char		strsql[512];
double		deb=0.,kre=0.;

matshet->new_plus("");

if(sp.kolih() == 0)
 return(0);
 
mop=0;

sp.clear_class();

for(i=0; i < kolshetvpd; i++)
  sppar[i]=0.;
for(i=0; i < kolparshet; i++)
  vsnn3[i]=0.;

sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
ICEB_MP_MATU,skk,nn,g,m,d);

SQLCURSOR cur;
  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
  return(0);
int nomer_sh=0;
while(cur.read_cursor(&row) != 0)
 {
  mop++;
  deb=atof(row[2]);
  kre=atof(row[3]);
  if(skl1 == 0)
   {
    if(tp == 1 )
      sh1.new_plus(row[0]);

    if(tp == 2 )
      sh1.new_plus(row[1]);

    if((nomer_sh=spsh.find(sh1.ravno())) >= 0)
     {        
      if(SRAV(row[0],row[1],0) == 0 )  /*Для внутреннего перемещения*/
        sp.plus(atof(row[2]),nomer_sh);
      else         
        sp.plus(atof(row[2])+atof(row[3])*(-1),nomer_sh);
     }
    else
/*************     
     if(mtsh != NULL)
     if(pole1(mtsh,sh1.ravno(),',',0,&i1) == 0 || SRAV(mtsh,sh1.ravno(),0) == 0)
************/
     if(proverka(spmatshets.ravno(),sh1.ravno(),0,1) == 0)
      {
       matshet->plus(sh1.ravno());
       matshet->plus(" ");
      }     
   }
  else
   {
    /*Смотрим для расходного документа*/
    if((i1=spsh.find(row[0])) >= 0) /*есть такой счет*/
      if(fabs(kre) > 0.009)
        sp.plus(kre,i1);
    /*Смотрим для приходного документа*/
    if((i1=spshpar.find(row[0])) >= 0) /*есть такой счет*/
     if(fabs(deb) > 0.009)
        sppar[i1]+=deb;
   }


  if(vsnn3 != NULL)
   {
    class iceb_tu_str bros;
    bros.plus(row[0]);
    bros.plus(",",row[1]);
    if((i1=vspsh.find(bros.ravno())) >= 0) /*есть такой счет*/
     {
      vsnn3[i1]+=atof(row[3]);
     }
   }

 }

return(mop);
}
/******************************/
/*Распечатка массивов на экран*/
/*******************************/
void 	prosprprint(int metka)
{
double    bb3=0.,bb2=0.,bb1=0.,bb=0.,bb4=0.;
class iceb_tu_str sh("");
char	strsql[512];


if(metka == 0 )
 {
  
  mvprintw(LINES/2,0,"\
-----------------------------------------------------------------------\n");

  printw(gettext("Счёт |Сумма по док. |Без НДС |Подтверджено|Проводки|Вст.док | Дата     |\n"));

  printw("\
-----------------------------------------------------------------------\n");
  int kol=spsh.kolih();
  for(int i=0; i < kol ;i++)
   {
    sh.new_plus(spsh.ravno(i));
    if(sh.ravno()[0] == '\0')
      break;

    printw("%-5s %13.2f %8.2f %12.2f %8.2f %8.2f %02d.%02d.%d\n",
    sh.ravno(),snn.ravno(i),sbnds.ravno(i),sn.ravno(i),sp.ravno(i),0.,dp.ravno(i),mp.ravno(i),gp.ravno(i));
    bb+=snn.ravno(i);
    bb1+=sbnds.ravno(i);
    bb2+=sn.ravno(i);
    bb3+=sp.ravno(i);
   }

  printw("\
-----------------------------------------------------------------------\n");
  printw("%-5s %13.2f %8.2f %12.2f %8.2f %8.2f\n",
  " ",bb,bb1,bb2,bb3,bb4);
  OSTANOV();

 }
if(metka == 1)
 {
  move(LINES/2,0);
  if(vsnn1 == NULL)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s !",gettext("Нет счетов для парного документа"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Возможно не подтвержден документ."));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }     
  else
   {
    printw("%s\n",gettext("Счета          Приход    Расход   Разница   Проводки"));
    for(int i=0; i < vspsh.kolih()   ; i++)
     {
      sh.new_plus(vspsh.ravno(i));
      printw("%-10s %10.2f %10.2f %8.2f %10.2f\n",
      sh.ravno(),vsnn1[i],vsnn2[i],vsnn1[i]-vsnn2[i],vsnn3[i]);
      bb+=vsnn2[i];
      bb1+=vsnn1[i];
      bb2+=vsnn3[i];
     }  
    printw("%-10s %10.2f %10.2f %8.2f %10.2f\n",
    "Итого",bb1,bb,bb1-bb,bb2);

    OSTANOV();
  
  }
 }

}
