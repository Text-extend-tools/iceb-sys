/* $Id: prosprus.c,v 5.36 2013/08/13 05:49:51 sasa Exp $ */
/*13.02.2014    22.02.1994      Белых А.И.      prosprus.c
Программа просмотра проводок и определения не сделанных
для учета услуг
*/
#include        <math.h>
#include        "bezcurses.h"

short provprus(class iceb_tu_str *sh1,short d,short m,short g,const char nn[],int tp,int podr);

extern double   okrg1; /*Округление*/
extern class masiv_din_int dp,mp,gp; /*Даты последнего подтверждения*/
extern class masiv_din_double snn; /*Суммы по накладных*/
class masiv_din_double sumpokart; /*Суммы по карточкам*/

extern class masiv_din_double sp; /*Суммы в проводках объявлен в матучёте*/
extern class masiv_din_double sbnds; /*Суммы без НДС*/

extern class SPISOK spsh; /*Список счетов учёта объявлен в матучёте*/
class SPISOK spis_uhet; //Список пар счетов списания,учёта
class masiv_din_double suma_spis_uhet; //массив с суммами по парам счетов


int             prosprus(short mr, //0-Проверит выполнение проводок
//		                     1-составить список проводок которые нужно сделать
//		                     2-распечатку на экран
//		                     3-1 и 0 одновременно 
int podr,
short d,short m,short g,
const char *nn,
int tp,   //1-приход 2-расход
int lnds, //Льготы по НДС
const char *kodop, //Код операции
float pnds)
{
int nom_sh=0;
short           d1=0,m1=0,g1=0;
short           mpi=0; /*0 - удалить отметку >0 записать отметку*/
class iceb_tu_str sh1("");
double          cnn,cnn1;
double          bb;
short           mop=0; /*0- нет проводок >0 -есть*/
int             i;
double          sumn,cn;
long		kolstr;
SQL_str         row,row2;
char		strsql[512];
short		dp1,mp1,gp1;
short		voz;
double		sumasnds;
double		snds=0.;
SQLCURSOR curr;
/*
printw("prosprus-%d %d %d.%d.%d %s %d %d\n",mr,podr,d,m,g,nn,tp,lnds);
refresh();
*/

if(mr == 2)
 {
  return(0);
 }

if(mr == 1 || mr == 3) //Составить список проводок
 {
  spis_uhet.free_class();
  suma_spis_uhet.free_class();
  
  sprintf(strsql,"select metka,kodzap,kolih,cena,shetu,nz,shsp from Usldokum1 \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and metka=1",
  g,m,d,podr,nn);  
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }
/*
  printw("\nprosprus-1 kolstr=%ld\n",kolstr);
  OSTANOV();  
*/
  if(kolstr == 0)
    return(0);

  spsh.free_class();
  sumpokart.free_class();
  sbnds.free_class();
  sp.free_class();
  snn.free_class();
  dp.free_class();
  mp.free_class();
  gp.free_class();


  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
    /*Читаем карточку материалла*/
    sumn=cn=atof(row[3]);
    if((nom_sh=spsh.find(row[4])) < 0)
     {
      spsh.plus(row[4]);    
      sp.plus(0.,-1);
      sumpokart.plus(0.,-1); /*должно быть синхронизировано с spsh*/
     } 
    sh1.new_plus(row[4]);      

    /*Читаем подтвержденное количество*/
 
    sprintf(strsql,"select datp,kolih from Usldokum2 where podr=%d and \
nomd='%s' and datd='%04d-%02d-%02d' and tp=%d and metka=%s and \
kodzap=%s and shetu='%s' and nz=%s",
    podr,nn,g,m,d,tp,row[0],row[1],row[4],row[5]);
    SQLCURSOR cur1;
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

//  if(kolstr == 0)
//    continue;

    cnn1=cnn=0.;
    d1=m1=g1=0;

    while(cur1.read_cursor(&row2) != 0)
     {
      bb=atof(row2[1]);
      bb=bb*cn;
      bb=okrug(bb,okrg1);
      cnn+=bb;

      bb=atof(row2[1])*sumn;
      bb=okrug(bb,okrg1);
      cnn1+=bb;
      rsdat(&dp1,&mp1,&gp1,row2[0],2);
//      if(d1 == 0 || (d1 != 0 && SRAV1(g1,m1,d1,gp1,mp1,dp1) > 0))
      if(sravmydat(d1,m1,g1,dp1,mp1,gp1) < 0)
       {
        d1=dp1; m1=mp1; g1=gp1;
       }
     }
    
    snn.plus(cnn1,nom_sh);
    if(lnds > 0)
      sbnds.plus(cnn1,nom_sh);
    else
      sbnds.plus(0.,nom_sh);
    sumpokart.plus(cnn,spsh.find(row[4]));
    dp.new_plus(d1,nom_sh); 
    mp.new_plus(m1,nom_sh);
    gp.new_plus(g1,nom_sh);


    if(row[6][0] != '\0')
     {
      sprintf(strsql,"%s,%s",row[6],row[4]);
      int nomer;
      if((nomer=spis_uhet.find(strsql)) < 0)
       spis_uhet.plus(strsql);
      suma_spis_uhet.plus(cnn1,nomer);
     }

   }

  provprus(&sh1,d,m,g,nn,tp,podr);
  if(mr == 1)
    return(0);
 
 }

/******************************************/
  /*Ищем проводки*/
if( mr == 0 || mr == 3 )
 {
//  move(14,0);
 //Читаем сумму корректировки если она есть
  double sumkor=0.;
  sprintf(strsql,"select sumkor from Usldokum where datd='%d-%d-%d' \
and podr=%d and nomd='%s' and tp=%d",g,m,d,podr,nn,tp);
  if(readkey(strsql,&row,&curr) == 1)
       sumkor=atof(row[0]);       
  double prockor=0.;
  if(sumkor != 0)
   {
    int i1=spsh.kolih();

    bb=0.;
    for(i=0; i < i1 ; i++)
     bb+=snn.ravno(i);

    prockor=sumkor*100./bb;
  /*
    printw("\nПроцент корректировки %.6g Сумма=%.2f/%.2f\n",
    prockor,bb,sumkor);
    OSTANOV();
  */
   }
  

  mop=mpi=0;
  mop=provprus(&sh1,d,m,g,nn,tp,podr);


  if( mop > 0 )
   {
    //Узнаем тип операции
    int vido;    
    vido=0;
    if(tp == 1)
     sprintf(strsql,"select vido from Usloper1 where kod='%s'",kodop);
    if(tp == 2)
     sprintf(strsql,"select vido from Usloper2 where kod='%s'",kodop);

    if(readkey(strsql,&row,&curr) > 0)
      vido=atoi(row[0]);
    for(i=0; i < spsh.kolih() ; i++)
     {
      sh1.new_plus(spsh.ravno(i));

      if(tp == 1 || vido == 1)
       {
        if(fabs(sp.ravno(i) - sumpokart.ravno(i)) > 0.009)
         {
          printf("%s %s %s %.14g != %.14g %d/%d!\n",gettext("По счету"),
          sh1.ravno(),
          gettext("не сделаны все проводки"),
          sp.ravno(i),sumpokart.ravno(i),i,spsh.kolih());
          mpi++;
         }
       }
      else
       {

        snds=0.;

        double suma=sumpokart.ravno(i);
        suma=suma+suma*prockor/100.;
        if(lnds == 0 && vido == 0)
         snds=suma*pnds/100.;
        
        sumasnds=suma+snds;

        sumasnds=okrug(sumasnds,okrg1);
        if(fabs(sp.ravno(i) - sumasnds) > 0.009)
         {
          printf("%s %s %s! %.14g != %.14g %d/%d %f\n",gettext("По счету"),
          sh1.ravno(),
          gettext("не сделаны все проводки"),
          sp.ravno(i),sumasnds,i,spsh.kolih(),sumkor);

          mpi++;
         }

       }
     }
   
   }
  if(mpi > 0)
   {
    printf("%s %s %s %d.%d.%d\n",gettext("Документ"),nn,gettext("дата"),d,m,g);
   }
 }

if(mop == 0)
  mpi++;


/*
printw("\n***mpi-%d mop-%d\n",mpi,mop);
OSTANOV();
*/
voz=0;
if(mpi == 0 )
 {
  /*Проводки выполнены*/
  sprintf(strsql,"update Usldokum \
set pro=1 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  g,m,d,podr,nn,tp);
  voz=1;
 }
else
 {
  /*Проводки не выполнены*/
  voz=0;
  sprintf(strsql,"update Usldokum \
set pro=0 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  g,m,d,podr,nn,tp);
 }

//Если документ заблокирован то никаких меток не ставим
if(iceb_t_pbpds(m,g,1) == 0) //Дата не заблокирована
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   msql_error(&bd,__FUNCTION__,strsql);
  }

return(voz);
}

/****************************/
/*Поиск выполненных проводок*/
/****************************/
short provprus(class iceb_tu_str *sh1,short d,short m,short g,
const char nn[],int tp,int podr)
{
short           mop;
long		kolstr;
SQL_str         row;
char		strsql[512];

if(sp.kolih() == 0)
  return(0);

mop=0;

sp.clear_class();

sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%04d-%02d-%02d'",
ICEB_MP_USLUGI,podr,nn,g,m,d);


//printw("\n%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }
//printw("kolstr=%d\n",kolstr);
//refresh();
if(kolstr == 0)
  return(0);
int nom_sh=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  mop++;
  if(tp == 1 )
    sh1->new_plus(row[0]);

  if(tp == 2 )
    sh1->new_plus(row[1]);

  if((nom_sh=spsh.find(sh1->ravno())) >= 0)  
   sp.plus(atof(row[2]),nom_sh);
  
 }

//OSTANOV();

return(mop);
}
