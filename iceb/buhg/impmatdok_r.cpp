/*$Id: impmatdok_r.c,v 5.4 2014/07/31 07:09:16 sasa Exp $*/
/*15.02.2016	06.07.2001	Белых А.И.	impmatdok_r.c
Импорт документов в материальный учет из файла
0-нет ошибок 1-есть
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"

extern double	okrcn;  /*Округление цены*/

int impmatdok_r(const char *imafim,int metkaproh) /*0-проверка 1-загрузка*/
{
struct stat work;
char		strsql[2048];
class iceb_tu_str strok("");
SQL_str		row;
SQLCURSOR       cur;


if(stat(imafim,&work) != 0)
 {
  
  sprintf(strsql,"%s-%s !",__FUNCTION__,gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  return(1);
 }



FILE *ff;

if((ff = fopen(imafim,"r")) == NULL)
 {
  sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Ошибка окрытия файла"),errno);
  iceb_t_soob(strsql);
  return(1);
 }

class iceb_tu_str kodiopuc("");

int kodopuc=0; //Метка операции списания по учетной цене
if(metkaproh == 1)
  iceb_t_poldan("Коды операций расхода по учетным ценам",&kodiopuc,"matnast.alx");

if(metkaproh == 0)
 printw("%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 printw("\n\n%s\n\n",gettext("Загружаем документы из файла"));

int metka_nnn=iceb_t_poldan_vkl("Нумерация налоговых накладных обновляется каждый месяц","matnast.alx");

int lnds=0;
int razmer=0;
int koldok=0;
int kod_mat_pred=0;
int kodusl=0;
class iceb_tu_str nomdok("");
short dd=0,md=0,gd=0;
class iceb_tu_str sklad("");
int tipz=0;
int koloh=0;
class iceb_tu_str kontr("");
class iceb_tu_str nomnalnak("");
class iceb_tu_str kodop("");
class iceb_tu_str nomvstdok("");
short dpnn=0,mpnn=0,gpnn=0;
int kodmat=0;
class iceb_tu_str naimat("");
double kolih=0.;
double cena=0.;
class iceb_tu_str eiz("");
int vtara=0;
double nds=0.;
int ndsvkl=0.;
class iceb_tu_str shetu("");
struct OPSHET	shetv;
class iceb_tu_str nomzak("");
class iceb_tu_str naim("");
int nomz=0;
class iceb_tu_str naim_kontr("");
int nom_kod_kontr=1;
int metka_zapisi_kk=0; /*Метка записи кода контрагента в справочник контрагентов*/
class iceb_tu_str rek1("");
while(iceb_tu_fgets(&strok,ff) == 0)
 {
  
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    continue;
   }

  if(SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(strok.ravno(),&nom_kod_kontr,NULL);
    continue;
   }

  if(SRAV(strok.ravno(),"DOC|",1) == 0)
   {
    if(koldok > 0 && metkaproh > 0 && nomdok.ravno()[0] != '\0')
     {
      podtdok1(dd,md,gd,sklad.ravno_atoi(),nomdok.ravno(),tipz,dd,md,gd,1);
      podvdok(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi());

      prospr(1,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno()); /*составляем список проводок которые должны быть сделаны*/
      avtpromu1(tipz,dd,md,gd,nomdok.ravno(),NULL); /*делаем проводки*/
      prospr(0,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno()); /*проверяем сделаны проводки все или нет*/
     }     
    metka_zapisi_kk=0;    
    lnds=0;
    koldok++;
    nomdok.new_plus("");    
    dd=md=gd=0;
    tipz=0;

    polen(strok.ravno(),strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      iceb_t_soob(gettext("Не определен вид документа (приход/расход) !"));
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    if(rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата!"));
      koloh++;
      continue;
     }
    if(iceb_t_pbpds(md,gd) != 0)
     {
      koloh++;
      continue;
     }
    polen(strok.ravno(),&sklad,4,'|');
    sprintf(strsql,"select kod from Sklad where kod=%d",sklad.ravno_atoi());
    if(readkey(strsql) != 1)
     {
      sprintf(strsql,gettext("Не найден склад %d в списке складов !"),sklad.ravno_atoi());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&kontr,5,'|');

    if(polen(strok.ravno(),&naim_kontr,11,'|') != 0)
     {
      iceb_t_soob(gettext("Не найдено поле наименования контрагента"));
      koloh++;
      continue;
     }
        
    if(naim_kontr.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Не введено поле наименования контрагента"));
      koloh++;
      continue;
     }

    /*определяем код контрагента в общем списке контрагентов*/
    if((metka_zapisi_kk=iceb_t_get_kkfname(naim_kontr.ravno(),kontr.ravno(),&kontr,&nom_kod_kontr)) < 0)
     {
      koloh++;
      continue;
     }

    polen(strok.ravno(),&nomnalnak,7,'|');

    if(tipz == 2 && nomnalnak.ravno()[0] != '\0')
     {
      if(metka_nnn == 0) /*месячная нумерация*/
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
        tipz,gd,md,gd,md,nomnalnak.ravno());
      else /*годовая*/
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
        tipz,gd,gd,nomnalnak.ravno());

            
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row[0],row[1]);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        koloh++;
        continue;
       }

      if(metka_nnn == 0)
       sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
       tipz,gd,md,gd,md,nomnalnak.ravno());
      else
       sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
       tipz,gd,gd,nomnalnak.ravno());

      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"%s %s %s",gettext("В учете услуг"),row[0],row[1]);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        koloh++;
        continue;
       }
     }

    polen(strok.ravno(),&kodop,8,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Prihod where kod='%s'",
      kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Rashod where kod='%s'",
      kodop.ravno());

    if(readkey(strsql) != 1)
     {
      sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&nomvstdok,9,'|');

    dpnn=mpnn=gpnn=0;
    if(nomnalnak.ravno()[0] != '\0')
     {
      polen(strok.ravno(),strsql,sizeof(strsql),10,'|');
      
      if(strsql[0] != '\0')
       {
        if(rsdat(&dpnn,&mpnn,&gpnn,strsql,1) != 0)
         {
          if(tipz == 1)
            sprintf(strsql,gettext("Не верно введена дата получения налоговой накладной !"));
          if(tipz == 2)
            sprintf(strsql,gettext("Не верно введена дата выдачи налоговой накладной !"));
          iceb_t_soob(strsql);
          koloh++;
          continue;
         }
       }
      else
       {
        dpnn=dd; mpnn=md; gpnn=gd;
       }
     }
    
    
    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    if(polen(strok.ravno(),&nomdok,6,'|') != 0)
     {
      sprintf(strsql,"%s!",gettext("Не найдено поле с номером документа"));
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }
//  printw("***nomdok=%s %d\n",nomdok,metkaproh);
    if(nomdok.getdlinna() > 1)
     { 
      sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s'",
      tipz,gd,gd,sklad.ravno_atoi(),nomdok.ravno());
      if(readkey(strsql) >= 1)
       {
        sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
        iceb_t_soob(strsql);
        koloh++;
        continue;
       }
     }

    //Запись в базу
    if(metkaproh != 0)
     {
      class lock_tables lk("LOCK TABLES Dokummat WRITE,Nalog READ,icebuser READ");
      /*если поле пустое то сами назначаем номер документа*/
      if(nomdok.getdlinna() <= 1)
        iceb_t_nomnak(gd,sklad.ravno(),&nomdok,tipz,0,0);
      float pnds=iceb_t_pnds(dd,md,gd);
      if(lnds == 4)
       pnds=7.;
      zap_s_mudok(tipz,dd,md,gd,sklad.ravno_atoi(),kontr.ravno(),nomdok.ravno(),nomnalnak.ravno(),kodop.ravno(),0,0,nomvstdok.ravno(),dpnn,mpnn,gpnn,0,"00",pnds);
     }        

   }



  //Строка записи в документе
  if(SRAV(rek1.ravno(),"ZP1",0) == 0)
   {
   
    polen(strok.ravno(),&kodmat,2,'|');
    polen(strok.ravno(),&naimat,11,'|');

    if(naimat.getdlinna() <= 1) /*наименования материалла нет*/
     {
      sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodmat);
        iceb_t_soob(strsql);
        koloh++;
        continue;
       }
     }
    else
     {
      /*проверяем есть ли такое наименование в списке материаллов*/
      sprintf(strsql,"select kodm from Material where naimat='%s'",naimat.ravno_filtr());
      if(readkey(strsql,&row,&cur) == 1)
       {
        kodmat=atoi(row[0]);
       }
      else
       {
        int metka_nomkm=0;
        class lock_tables lk("LOCK TABLES Material WRITE,Uslugi READ,icebuser READ");
        if(kodmat != 0)
         {
          /*проверяем нет ли уже другого материала или услуги с этим кодом*/
          sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
          if(readkey(strsql) == 1)
           {
            kodmat=nomkm(kod_mat_pred);
            metka_nomkm=1;
           }
          else
           {
            sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodmat);
            if(readkey(strsql) == 1)
             {
              kodmat=nomkm(kod_mat_pred);
              metka_nomkm=1;
             }
           }
         }
        else
         {
          kodmat=nomkm(kod_mat_pred);
          metka_nomkm=1;
         }
        sprintf(strsql,"insert into Material (kodm,naimat,ktoz,vrem) values(%d,'%s',%d,%ld)",kodmat,naimat.ravno_filtr(),iceb_t_getuid(),time(NULL));

        if(sql_zapis(strsql,1,0) == 0)
         {
          if(metka_nomkm == 1)        
           kod_mat_pred=kodmat;
         }
        else
         {
          koloh++;
         }
       }             
     }
    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      sprintf(strsql,"%s !",gettext("В записи нулевое количество материалла"));
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=okrug(cena,okrcn);

    polen(strok.ravno(),&eiz,5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(readkey(strsql) != 1)
     {
      sprintf(strsql,gettext("Не найдено единицу измерения %s !"),eiz.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),6,'|');
    vtara=atoi(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),7,'|');
    nds=atof(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),8,'|');
    ndsvkl=atoi(strsql);

    polen(strok.ravno(),&shetu,9,'|'); //Для расходных документов не обязательный реквизит
    if(tipz == 1 || (tipz == 2  && shetu.ravno()[0] != '\0'))
      if(prsh1(shetu.ravno(),&shetv) != 0)
       {
        koloh++;
        continue;
       }

    polen(strok.ravno(),&nomzak,10,'|');
   
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
//      printw("%d.%d.%d %d %d %s %.10g %.10g %s %f %d %d %d\n",
//      dd,md,gd,sklad,kodmat,nomdok.ravno(),kolih,cena,eiz.ravno(),nds,ndsvkl,vtara,tipz);      

      //Проверяем может такой код материалла уже есть в накладной
      if(tipz == 2)
       {
        kodopuc=0;
        if(proverka(kodiopuc.ravno(),kodop.ravno(),0,1) == 0)
           kodopuc=1;  //Списание по учетной цене

        sprintf(strsql,"select kodm from Dokummat1 where tipz=%d and \
  datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s' \
  and kodm=%d",tipz,gd,gd,sklad.ravno_atoi(),nomdok.ravno(),kodmat);
        if(sql_readkey(&bd,strsql,&row,&cur) > 0)
         {
          sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
          if(sql_readkey(&bd,strsql,&row,&cur) > 0)
            naimat.new_plus(row[0]);
          else
            naimat.new_plus("");

          VVOD SOOB(1);
          sprintf(strsql,"%s %d %s",
          gettext("Код материалла"),kodmat,naimat.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Такая запись в документе уже есть !"));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,"%s N%s",gettext("Документ"),nomdok.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          continue;
         }
       }

      if(tipz == 1)      
       {
        zap_prihod(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),kodmat,kolih,cena,eiz.ravno(),shetu.ravno(),nds,vtara,ndsvkl,nomzak.ravno(),"");

       }

      if(tipz == 2)
       {
        priv_k_kart(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),kodmat,kolih,cena,shetu.ravno(),eiz.ravno(),nds,ndsvkl,vtara,nomzak.ravno(),kodopuc,"");
       }
     }
   }

  //Строка записи услуг
  if(SRAV(rek1.ravno(),"ZP2",0) == 0)
   {
    polen(strok.ravno(),&eiz,2,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,gettext("Не найдено единицу измерения %s !"),eiz.ravno());
      iceb_t_soob(strsql);
      koloh++;
      continue;
     }
    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);

    polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=okrug(cena,okrcn);
    polen(strok.ravno(),strsql,sizeof(strsql),5,'|');
    nds=atof(strsql);

    polen(strok.ravno(),&naim,6,'|');

    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat3 (datd,sklad,nomd,ei,kolih,cena,nds,naimu,ktoi,vrem,ku)\
values ('%04d-%02d-%02d',%d,'%s','%s',%.10g,%.10g,%.10g,'%s',%d,%ld,%d)",
      gd,md,dd,sklad.ravno_atoi(),nomdok.ravno(),eiz.ravno(),kolih,cena,nds,naim.ravno(),
      iceb_t_getuid(),time(NULL),kodusl);

      sql_zapis(strsql,1,0);
     }
   }

  //Строка записи дополнительной информации к документа
  if(SRAV(rek1.ravno(),"ZP3",0) == 0)
   {
    polen(strok.ravno(),&nomz,2,'|');

    if(nomz == 0)
      continue;    
    polen(strok.ravno(),&naim,3,'|');

    if(nomz == 8)
     {
      sprintf(strsql,"select kod from Foroplat where kod='%s'",naim.ravno());
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,gettext("Не найдена форма оплаты %s !"),naim.ravno());
        iceb_t_soob(strsql);
        koloh++;
        continue;
       }

     }

    if(nomz == 11)
      polen(strok.ravno(),&lnds,3,'|');

    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%s')",
      gd,sklad.ravno_atoi(),nomdok.ravno(),nomz,naim.ravno());
//      printw("%s\n",strsql);

      sql_zapis(strsql,1,0);
     }    
   }
 }
strzag(LINES-1,0,work.st_size,work.st_size);

fclose(ff);
if(koldok > 0 && metkaproh > 0 && nomdok.ravno()[0] != '\0')
 {
  podtdok1(dd,md,gd,sklad.ravno_atoi(),nomdok.ravno(),tipz,dd,md,gd,1);
  podvdok(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi());
  prospr(1,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno()); /*составляем список проводок которые должны быть сделаны*/
  avtpromu1(tipz,dd,md,gd,nomdok.ravno(),NULL); /*делаем проводки*/
  prospr(0,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno()); /*проверяем сделаны проводки все или нет и выставляем метку выполнения проводок*/
 }

if(koloh != 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s:%d",gettext("Количество ошибок"),koloh);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,gettext("Импорт документов осуществляется если нет ни одной ошибки"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

if(koldok == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа!"));
  return(1);
 }

if(metkaproh == 1)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s",gettext("Загрузка завершена"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s:%d",gettext("Количество документов"),koldok);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
return(0);
}
