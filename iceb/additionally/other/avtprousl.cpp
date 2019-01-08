/* $Id: avtprousl.c,v 5.43 2011-01-13 13:43:21 sasa1 Exp $ */
/*10.12.2010    21.11.2000      Белых А.И.      avtprousl.c
Автоматическое проведение проводок для учета услуг
*/
#include        <errno.h>
#include        <time.h>
#include        <math.h>
#include        "buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif


extern masiv_din_int dp,mp,gp; /*Даты последнего подтверждения*/
extern masiv_din_double snn; /*Суммы по накладных*/
extern class masiv_din_double sumpokart; /*Суммы по карточкам*/
extern masiv_din_double sp; /*Суммы в проводках*/
extern SPISOK   spsh; /*Список счетов*/
extern masiv_din_double sbnds; /*Суммы без НДС*/
extern short    ggg;

void avtprousl(int tipz, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char kop[], //Код операции
const char nn[], //Номер документа
int podr,  //код подразделения
const char kor[], //Код контрагента
double sumkor, //Сумма корректировки суммы документа
const char nn1[],  //Номер встречного документа
float pnds, //Процент НДС на момент создания документа
FILE *ff_prot)
{
char		bros[1024];
double          bb=0.,deb=0.,kre=0.;
char            shp1[32],shp2[32],shp21[32],shp3[32],shp4[32],shp5[32],shp6[32];
char            bros1[40];
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");
class iceb_tu_str kontr21("");
class iceb_tu_str kontr3("");
class iceb_tu_str kontr4("");
class iceb_tu_str kontr5("");
class iceb_tu_str kontr6("");
int             i1;
char		kto[32];
short           tp1,tp2,tp21,tp3,tp4,tp5;
short		mvd;
class iceb_tu_str kvd("");
char		mopr;
char		kopz[32];
short		mon,mvz; /*Метка оплаты накладной / метка выдачи запроса*/
class iceb_tu_str nai("");
time_t		vrem;
char		korr[32];
short		mkp;
short		kp;
double		prockor=0.;
char		strsql[512];
SQL_str         row;
struct OPSHET	shetv;
int		kolpr=2;
double		sumandspr=0.;
int		metkands=0;
class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;
SQLCURSOR curr;
int kekv=0;
class iceb_tu_str imaf_np;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"avtpromu-tipz=%d %d.%d.%d kop=%s nn=%s\n",
 tipz,dd,md,gd,kop,nn);

if(dp.suma() == 0)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Документ не подтверждён!"),dd,md,gd,nn);
  iceb_t_soob(strsql);
  return;
 }

prockor=0.;
if(sumkor != 0)
 {
  prockor=sumkor*100./snn.suma();
/*
  printw("\nПроцент корректировки %.6g Сумма=%.2f/%.2f\n",
  prockor,bb,sumkor);
  OSTANOV();
*/
 }

kp=mvz=mon=mopr=mvd=0;
memset(korr,'\0',sizeof(korr));
strcpy(korr,kor);

strcpy(kto,gettext("УСЛ"));
strcpy(kopz,kop);

/*Если возврат с реализации то ищем накладную на реализацию
определяем какая там операция и делаем проводки по этой операции
только красным*/

if(tipz == 1)
  imaf_np.new_plus("avtprouslp.alx");
if(tipz == 2)
  imaf_np.new_plus("avtprouslr.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_np.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_np.ravno());
  return;
 }


int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_t_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"uslnast.alx") == 0)
 {
   if(SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }
if(ff_prot != NULL)
 {
  if(metka_vkontr == 0)
   fprintf(ff_prot,"Ручная вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
  if(metka_vkontr == 2)
   fprintf(ff_prot,"Автоматическая вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
 }

if(tipz == 1)
 {
  sprintf(strsql,"select sumnds from Usldokum \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nn,tipz);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    sumandspr=atof(row[0]);
    metkands=1;
   }
 }

mkp=0;
GDITE();
time(&vrem);
int nomer_sheta=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
  if(polen(row_alx[0],bros,sizeof(bros),2,'|') != 0)
   continue;

  if(SRAV(kopz,bros,0) != 0)
    continue;
  kontr1.new_plus("");
  kontr2.new_plus("");
  kontr21.new_plus("");
  kontr3.new_plus("");
  kontr4.new_plus("");
  kontr5.new_plus("");
  kontr6.new_plus("");
  
  mopr=1;
  
  shp1[0]=shp2[0]=shp21[0]=shp3[0]=shp4[0]=shp5[0]=shp6[0]='\0';
  tp1=tp2=tp21=tp3=tp4=tp5=0;

  polen(row_alx[0],shp1,sizeof(shp1),3,'|');
  if((nomer_sheta=spsh.find(shp1)) < 0)
    continue;

  
  if(iceb_t_pvglkni(mp.ravno(nomer_sheta),gp.ravno(nomer_sheta)) != 0)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s",row_alx[0]);

  polen(row_alx[0],bros,sizeof(bros),4,'|');

  if(polen(bros,shp2,sizeof(shp2),1,',') != 0)
    strncpy(shp2,bros,sizeof(shp2));
  else
    polen(bros,shp21,sizeof(shp21),2,',');     

  polen(row_alx[0],shp3,sizeof(shp4),5,'|');
  polen(row_alx[0],shp4,sizeof(shp5),6,'|');
  polen(row_alx[0],shp5,sizeof(shp5),7,'|');
  polen(row_alx[0],shp6,sizeof(shp6),8,'|');

  move(20,0);
  if(mvd == 0 && shp4[0] != '\0')
   {
    mvd++;
    VVOD  VVOD1(2);

    helstr(LINES-1,0,"F10",gettext("выход"),
    "Enter",gettext("просмотр кодов"),NULL);

    if(tipz == 2)
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код валового дохода"));
    if(tipz == 1)
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код валового расхода"));

    kvd.new_plus("");    
    if((i1=vvod1(&kvd,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
      return;
     }
    if(i1 == ESC)
     {
      return;
     }

    if(i1 == ENTER && kvd.getdlinna() <= 1)
     {
      kvd.new_plus("");
      nai.new_plus("");
      if(dirskont(shp4,&kvd,&nai,1) != 0)
       {
        return;
       }
      kontr4.new_plus(kvd.ravno());
     }

    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shp4,kvd.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,gettext("Не найден код %s в списке счета %s !"),kvd.ravno(),shp4);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      return;
     }
    GDITE();
   }
  
  if(shp1[0] == '\0')
   continue;

  kontr1.new_plus(korr);   
  if(shp1[0] != '\0')
    if(provsh(&kontr1,shp1,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }
  tp1=shetv.saldo;

  kontr2.new_plus(korr);   
  if(shp2[0] != '\0')
    if(provsh(&kontr2,shp2,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }
  tp2=shetv.saldo;

  if(shp21[0] != '\0')
   {
    if(mvz == 0)
     {

      VVOD MENU(4);
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужный счет для НДС"));

      MENU.VVOD_SPISOK_add_MD(shp2);
      MENU.VVOD_SPISOK_add_MD(shp21);
      int kom2=0;
      while(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0);
      if(kom2 == 1)
       {
        kontr21.new_plus(korr);     
        if(provsh(&kontr21,shp21,&shetv,metka_vkontr,0) != 0)
         {
          return;
         }
        tp21=shetv.saldo;
        if(kontr21.getdlinna() > 1)
         strncpy(korr,kontr21.ravno(),sizeof(korr)-1);
        kontr21.new_plus(korr);        
        mon=1;
       }

     }
   if(mvz == 1 && mon == 1)
    {
     kontr21.new_plus(korr);     
     if(provsh(&kontr21,shp21,&shetv,metka_vkontr,0) != 0)
       {
        return;
       }
     tp21=shetv.saldo;
     if(kontr21.getdlinna() > 1)
      strncpy(korr,kontr21.ravno(),sizeof(korr)-1);
     kontr21.new_plus(korr);        
    }
    mvz=1;

   }
  if(shp3[0] != '\0') /*Счёт списания*/
   {
    kontr3.new_plus(korr);     
    if(provsh(&kontr3,shp3,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }
    tp3=shetv.saldo;
    if(kontr3.getdlinna() > 1)
     strncpy(korr,kontr3.ravno(),sizeof(korr)-1);
   }
   
  if(shp4[0] != '\0')
   {
    kontr4.new_plus(kvd.ravno());     
    if(provsh(&kontr4,shp4,&shetv,0,0) != 0)
     {
      return;
     }
    tp4=shetv.saldo;
    if(kontr4.getdlinna() > 1)
     kvd.new_plus(kontr4.ravno());
   }

  if(shp5[0] != '\0')
   {
    kontr5.new_plus(korr);     
    if(provsh(&kontr5,shp5,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }
    tp5=shetv.saldo;
    if(kontr5.getdlinna() > 1)
     strncpy(korr,kontr5.ravno(),sizeof(korr)-1);
   }


  bb=sumpokart.ravno(nomer_sheta);
  bb=okrug(bb,0.01);
  if(ff_prot != NULL)
   fprintf(ff_prot,"bb=%f\n",bb);
   
  memset(bros1,'\0',sizeof(bros1));
  if(ff_prot != NULL)
    fprintf(ff_prot,"\n1=%s 2=%s 3=%s 4=%s 5=%s 6=%s\n",
    shp1,shp2,shp3,shp4,shp5,shp6);
  /*** ПРИХОДЫ *****/
  if(tipz == 1)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Приход услуг\n");

     }     
    
    //Если счет валового расхода отсутствует делаем проводку на сумму без НДС
    if(shp4[0] == '\0')
     {
      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        //cумма без НДС
        deb=bb; kre=0.;
        memset(bros1,'\0',sizeof(bros1));
        if(nn1[0] != '\0')
          sprintf(bros1,"N%s",nn1);
        if(shp5[0] != '\0' && shp3[0] != '\0')
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp5,shp3,kontr5.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

        if(shp5[0] != '\0' && shp1[0] != '\0')
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

       }
      else
        if(shp1[0] != '\0' && shp4[0] == '\0' && shp3[0] != '\0')
         { 
          deb=bb; kre=0.;
          memset(bros1,'\0',sizeof(bros1));
          if(nn1[0] != '\0')
            sprintf(bros1,"N%s",nn1);
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp3,kontr1.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
     }
    /*НДС оплачено*/
    if(shp2[0] != '\0' && shp3[0] != '\0' && mon == 0 && metkands < 2)
     {
      if(sumandspr != 0.)
       {
        bb=sumandspr;
        metkands++;
       }
      else
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
       }
      bb=okrug(bb,0.01);
      memset(bros1,'\0',sizeof(bros1));
      if(nn1[0] != '\0')
       sprintf(bros1,"N%s",nn1);

      deb=bb; kre=0.;
      if(fabs(bb) > 0.009)      
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp2,shp3,kontr1.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }       

    /*НДС не оплачено*/
    if(shp21[0] != '\0' && shp3[0] != '\0' && mon == 1 && metkands < 2)
     {
      if(sumandspr != 0.)
       {
        bb=sumandspr;
        metkands++;
       }
      else
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
       }
      bb=okrug(bb,0.01);
      if(nn1[0] != '\0')
        sprintf(bros1,"N%s",nn1);
      deb=bb; kre=0.;

      if(fabs(bb) > 0.009)      
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp21,shp3,kontr21.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    if(shp4[0] != '\0') //счёт валового расхода введён
     {
//      bb=sn[nomer_sheta];
      bb=sumpokart.ravno(nomer_sheta);
      bb=okrug(bb,0.01);
      if(fabs(bb) > 0.009  && shp3[0] != '\0')      
       {
        deb=bb; kre=0.;
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp3,kontr4.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      if(atoi(shp6) == 0) //Нулевой вариант расчета
       {            
        if(fabs(bb) > 0.009)
         {
          strcat(bros1," ");
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp4,kontr1.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        if(fabs(bb) > 0.009 )
         {
          memset(bros1,'\0',sizeof(bros1));
          strcat(bros1," ");
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          if(shp5[0] != '\0')
           {
            zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp5,shp4,kontr5.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
           }
          if(shp1[0] != '\0' && shp5[0] != '\0')
           {
            zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
           }
         }
       }
     }
  }

  /******** РАСХОДЫ **********/
  if(tipz == 2)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Расход услуг\n");
     }     
    
    bb=bb+bb*prockor/100.;
    if(shp5[0] != '\0' && shp1[0] != '\0')
     {
      deb=bb; kre=0.;

      /*Проводка на сумму Учета*/  
      zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    /*НДС оплачено*/
    if(shp2[0] != '\0' && shp1[0] != '\0' && mon == 0)
     {
      bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;

      bb=okrug(bb,0.01);
         
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;

        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp2,kontr1.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    /*НДС не оплачено*/
    if(shp1[0] != '\0' && shp21[0] != '\0' && mon == 1)
     {
      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;

      bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
      bb=okrug(bb,0.01);
         
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp21,kontr1.ravno(),kontr21.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    //Если счет валового дохода не введен

    if(shp3[0] != '\0' && shp4[0] == '\0' && shp1[0] != '\0')
     {
      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
      bb=(bb-sbnds.ravno(nomer_sheta))+(bb-sbnds.ravno(nomer_sheta))*pnds/100.+sbnds.ravno(nomer_sheta);
      bb=okrug(bb,0.01);
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp1,kontr3.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }

    if(shp4[0] != '\0')
     {

      if(atoi(shp6) == 0) //Нулевой вариант расчета
       {

        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))+(bb-sbnds.ravno(nomer_sheta))*pnds/100.+sbnds.ravno(nomer_sheta);

        strcat(bros1," ");
        strcat(bros1,shp3);
        strcat(bros1,"-код");
        strcat(bros1,korr);

        bb=okrug(bb,0.01);

        if(fabs(bb) > 0.009 && shp1[0] != '\0')      
         {
          deb=bb; kre=0.;
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp1,kontr4.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
//        bb=bb*pnds/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
        bb=okrug(bb,0.01);

        if(fabs(bb) > 0.009 && shp3[0] != '\0')      
         {
          memset(bros1,'\0',sizeof(bros1));

          strcat(bros1,shp1);
          strcat(bros1,"-ПДВ");
          deb=bb; kre=0.;
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp4,kontr3.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=okrug(bb,0.01);


        if(fabs(bb) > 0.009  && shp1[0] != '\0')      
         {
          memset(bros1,'\0',sizeof(bros1));
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          deb=bb; kre=0.;
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp1,kontr4.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
        bb=okrug(bb,0.01);
        if(fabs(bb) > 0.009  && shp1[0] != '\0' && shp3[0] != '\0')      
         {
          deb=bb; kre=0.;
          memset(bros1,'\0',sizeof(bros1));
          zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp1,kontr3.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
      bb=okrug(bb,0.01);


      if(fabs(bb) > 0.009  && shp3[0] != '\0')      
       {
        memset(bros1,'\0',sizeof(bros1));
        strcpy(bros1,shp1);
        deb=bb; kre=0.;
        zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp4,kontr3.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }


   }
 }
/*Запись проводок из памяти в базу*/

zapmpr1(nn,kop,podr,vrem,kto,dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot);


if(mopr == 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,gettext("Для операции %s не найдена настройка проводок !"),kopz);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }   



}
