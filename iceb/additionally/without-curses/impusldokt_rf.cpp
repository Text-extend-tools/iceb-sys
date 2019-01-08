/*$Id:$*/
/*16.02.2016	23.09.2013	Белых А.И.
Импорт услуг
*/
#include <errno.h>
#include        <sys/stat.h>
#include        "bezcurses.h"

extern double	okrcn;  /*Округление цены*/

int impusldokt_r(int metkaproh,const char *imaf)
{
int koldok=0;
FILE *ff;
char strsql[2048];
struct stat work;
time_t tmm;
SQL_str row;
SQLCURSOR cur;
stat(imaf,&work);

if((ff = fopen(imaf,"r")) == NULL)
 {
  printf("%s-%s %d !",__FUNCTION__,gettext("Ошибка окрытия файла"),errno);
  return(1);
 }


if(metkaproh == 0)
 printf("%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 printf("\n\n%s\n\n",gettext("Загружаем документы из файла"));

int metka_nnn=iceb_t_poldan_vkl("Нумерация налоговых накладных обновляется каждый месяц","matnast.alx");

int razmer=0;
class iceb_tu_str nomdok("");
class iceb_tu_str nomnalnak("");
class iceb_tu_str strok("");
int i=0;
int koloh=0;
int tipz=0;
short dd,md,gd;
int kodpod=0;
class iceb_tu_str kontragent("");
class iceb_tu_str kodop("");
int lnds=0;
int kodus=0;
double kolih,cena;
class iceb_tu_str shet("");
class iceb_tu_str eiz("");
OPSHET rekshet;
class iceb_tu_str dop_naim("");
time(&tmm);
class iceb_tu_str naim_kontr("");
class iceb_tu_str naim_uslug("");
int nom_kod_kontr=1;
int metka_zapisi_kk=0;
float pnds=0.;
class iceb_tu_str osnov("");
class iceb_tu_str uslprod("");
class iceb_tu_str herez("");
class iceb_tu_str dover("");
class iceb_tu_str datadover("");
int kod_usl_pred=1;
class iceb_tu_str rek1("");
while(iceb_tu_fgets(&strok,ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  i=strlen(strok.ravno());
  razmer+=i;

  printf("%s",strok.ravno());
  if(polen(strok.ravno(),&rek1,1,'|') != 0)
    continue;    

  if(SRAV(strok.ravno(),"DOCU|",1) == 0)
   {
    if(nomdok.getdlinna() > 1 && metkaproh > 0)
     {
      uslpoddok(dd,md,gd,dd,md,gd,nomdok.ravno(),kodpod,tipz,1); /*подтверждаем документа*/
      podudok(dd,md,gd,nomdok.ravno(),kodpod,tipz); /*проверяем подтверждение и ставим метку подтверждённого документа*/

      prosprus(1,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds); /*составляем список проводок которые должны быть сделаны*/
      avtprousl1(tipz,dd,md,gd,nomdok.ravno(),NULL); /*делаем проводки*/
      prosprus(0,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds); /*проверяем сделаны ли проводки и ставим метку выполненных проводок*/
     }    
    metka_zapisi_kk=0;
    kodpod=lnds=dd=md=gd=0;
    tipz=0;
        
    polen(strok.ravno(),strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      printf("%s\n",gettext("Не определен вид документа (приход/расход) !"));
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    if(rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      printf("%s\n",gettext("Не верно введена дата!"));
      koloh++;
      continue;
     }
    if(iceb_t_pbpds(md,gd) != 0)
     {
      koloh++;
      continue;
     }
     
    pnds=iceb_t_pnds(dd,md,gd); /*получаем действующее значение НДС*/
    polen(strok.ravno(),&kodpod,4,'|');
    sprintf(strsql,"select kod from Uslpodr where kod=%d",kodpod);
    if(sql_readkey(&bd,strsql) != 1)
     {
      printf("%s %d !\n",gettext("Не найден код подразделения"),kodpod);
      koloh++;
      continue;
     }

    polen(strok.ravno(),&kontragent,5,'|');

    if(polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      printf("%s\n",gettext("Не найдено поле наименования контрагента"));
      koloh++;
      continue;
     }

    if(naim_kontr.getdlinna() <= 1)
     {
      printf("%s\n",gettext("Не введено поле наименования контрагента"));
      koloh++;
      continue;
     
     }
    /*определяем код контрагента в общем списке контрагентов*/
    if((metka_zapisi_kk=iceb_t_get_kkfname(naim_kontr.ravno(),kontragent.ravno(),&kontragent,&nom_kod_kontr)) < 0)
     {
      koloh++;
      continue;
     }

    polen(strok.ravno(),&nomnalnak,7,'|');

    if(tipz == 2 && nomnalnak.ravno()[0] != '\0')
     {
      if(metka_nnn == 0)
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
        tipz,gd,md,gd,md,nomnalnak.ravno());
      else
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
        tipz,gd,gd,nomnalnak.ravno());

      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        printf("%s %s %s!\n",gettext("С номером"),nomnalnak.ravno(),gettext("налоговая накладная уже есть"));
        printf("%s %s %s\n",gettext("Материальный учет"),row[0],row[1]);
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
        printf("%s %s %s!\n",gettext("С номером"),nomnalnak.ravno(),gettext("налоговая накладная уже есть"));
        printf("%s %s %s\n",gettext("В учете услуг"),row[0],row[1]);
        koloh++;
        continue;
       }
     }

    polen(strok.ravno(),&kodop,8,'|');
    
    if(tipz == 1)
      sprintf(strsql,"select kod from Usloper1 where kod='%s'",kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Usloper2 where kod='%s'",kodop.ravno());
    
    if(sql_readkey(&bd,strsql) != 1)
     {
      printf("%s %s!\n",gettext("Не найден код операции"),kodop.ravno());
      koloh++;
      continue;
     }
    polen(strok.ravno(),&lnds,9,'|');

    polen(strok.ravno(),&osnov,11,'|');
    polen(strok.ravno(),&uslprod,12,'|');
    polen(strok.ravno(),&herez,13,'|');
    polen(strok.ravno(),&dover,14,'|');
    polen(strok.ravno(),&datadover,15,'|');
    
    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    polen(strok.ravno(),&nomdok,6,'|');
 
    sprintf(strsql,"select nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%d and nomd='%s'",
    tipz,gd,gd,kodpod,nomdok.ravno());
    if(sql_readkey(&bd,strsql) >= 1)
     {
      printf("%s %s %s!\n",gettext("С номером"),nomdok.ravno(),gettext("документ уже есть"));
      koloh++;
      continue;
     }
    //Запись в базу
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      
      sprintf(strsql,"insert into Usldokum \
(tp,datd,podr,kontr,nomd,nomnn,kodop,nds,ktoi,vrem,pn,osnov,uslpr,sherez,dover,datdov) \
values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%d,%d,%ld,%.2f,'%s','%s','%s','%s','%s')",
      tipz,gd,md,dd,kodpod,kontragent.ravno(),nomdok.ravno(),nomnalnak.ravno(),kodop.ravno(),lnds,iceb_t_getuid(),tmm,pnds,
      osnov.ravno_filtr(),
      uslprod.ravno_filtr(),
      herez.ravno_filtr(),
      dover.ravno_filtr(),
      datadover.ravno_sqldata());

      if(sql_zapis(strsql,1,0) == 0)
       koldok++;
      else
       koloh++;
      continue;
     }        
    
   }

  if(SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(strok.ravno(),&nom_kod_kontr,NULL);
    continue;
   }

  if(SRAV(rek1.ravno(),"ZPU",0) == 0)
   {

    polen(strok.ravno(),&kodus,2,'|');
    polen(strok.ravno(),&naim_uslug,8,'|');
    if(naim_uslug.getdlinna() > 1)
     {
      /*проверяем есть ли такое наименование в списке материаллов*/
      sprintf(strsql,"select kodus from Uslugi where naius='%s'",naim_uslug.ravno_filtr());
      if(readkey(strsql,&row,&cur) == 1)
       {
        kodus=atoi(row[0]);
       }
      else
       {
        int metka_nomkm=0;
        class lock_tables lk("LOCK TABLES Material READ,Uslugi WRITE,icebuser READ");
        if(kodus != 0)
         {
          /*проверяем нет ли уже другого материала с этим кодом*/
          sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodus);
          if(readkey(strsql) == 1)
           {
            kodus=nomkm(kod_usl_pred);
            metka_nomkm=1;
           }
          else
           {
            sprintf(strsql,"select kodm from Material where kodm=%d",kodus);
            if(readkey(strsql) == 1)
             {
              kodus=nomkm(kod_usl_pred);
              metka_nomkm=1;
             }
           }
         }
        else
         {
          kodus=nomkm(kod_usl_pred);
          metka_nomkm=1;
         }

        sprintf(strsql,"insert into Uslugi (kodus,naius,ktoz,vrem) values(%d,'%s',%d,%ld)",kodus,naim_uslug.ravno_filtr(),iceb_t_getuid(),time(NULL));

        if(sql_zapis(strsql,1,0) == 0)
         {
          if(metka_nomkm == 1)        
           kod_usl_pred=kodus;
         }
        else
         koloh++;
       }             
     }    
    else
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodus);
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        printf("%s %d !\n",gettext("Не найден код услуги"),kodus);
        koloh++;
        continue;
       }
     }

    polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      printf("%s !\n",gettext("В записи нулевое количество услуг"));
      koloh++;
      continue;
     }

    polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=okrug(cena,okrcn);

    polen(strok.ravno(),&eiz,5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      printf("%s %s!\n",gettext("Не найдено единицу измерения"),eiz.ravno());
      koloh++;
      continue;
     }

    polen(strok.ravno(),&shet,6,'|'); 
    if(prsh1(shet.ravno(),&rekshet) != 0)
     {
      koloh++;
      continue;
     }

    polen(strok.ravno(),&dop_naim,7,'|');
    

    //Запись в базу
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
      if(zapuvdok(tipz,dd,md,gd,nomdok.ravno(),1,kodus,kolih,cena,eiz.ravno(),shet.ravno(),kodpod,"",dop_naim.ravno()) != 0)
       koloh++;

   }
   

 }

if(nomdok.getdlinna() > 1 && metkaproh > 0)
 {
  
  uslpoddok(dd,md,gd,dd,md,gd,nomdok.ravno(),kodpod,tipz,1); /*подтверждаем документа*/
  podudok(dd,md,gd,nomdok.ravno(),kodpod,tipz); /*проверяем подтверждение и ставим метку подтверждённого документа*/
  
  pnds=iceb_t_pnds(dd,md,gd); /*получаем действующее значение НДС*/

  prosprus(1,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds); /*составляем список проводок которые должны быть сделаны*/
  avtprousl1(tipz,dd,md,gd,nomdok.ravno(),NULL); /*делаем проводки*/
  prosprus(0,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds); /*проверяем сделаны ли проводки и ставим метку выполненных проводок*/
 }    

fclose(ff);
//OSTANOV();
if(koldok > 0)
 printf("%s:%d\n",gettext("Количество загруженных документов"),koldok);
return(koloh);

}
