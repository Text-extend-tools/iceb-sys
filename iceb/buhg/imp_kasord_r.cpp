/*$Id: imp_kasord_r.c,v 5.5 2014/07/31 07:09:16 sasa Exp $*/
/*22.02.2016	28.03.2007	Белых А.И.	imp_kasord_r.c
Импорт кассовых ордеров из файла
*/
#include        <sys/stat.h>
#include        "buhg.h"
int imp_kasord_r_vp(short dd,short md,short gd,const char *kassa,int tipzap,const char *nomdok,const char *kodop,const char *shetd);

int imp_kasord_r(const char *imafim,int metka,class SPISOK *kodkontr_new) //0-проверка на ошибки 1- загрузка
{
if(metka == 0)
 printw("%s\n",gettext("Проверка на наличие ошибок"));
if(metka == 1)
 printw("\n\n%s\n",gettext("Загружаем документы из файла"));
class iceb_tu_str stroka("");
class iceb_tu_str kod_str("");
class iceb_tu_str bros("");
int kol_oh=0; //количество ошибок
int tipzap=0;
class iceb_tu_str tip("");
class iceb_tu_str kassa("");
short dd,md,gd;
class iceb_tu_str nomdok("");
class iceb_tu_str shetd("");
class iceb_tu_str shetk("");
class iceb_tu_str kodop("");
class iceb_tu_str osnov("");
class iceb_tu_str dopol("");
class iceb_tu_str fio("");
class iceb_tu_str dokum("");
int razmer=0;
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
struct OPSHET rek_shet;
class iceb_tu_str kontr("");
double suma=0.;
class iceb_tu_str fam("");
class iceb_tu_str inn("");
class iceb_tu_str nomer_bl(""); /*Номер бланка кассового ордера*/  
int koldok_p=0; /*количество приходных документов*/
int koldok_r=0; /*количество расходных документов*/
double i_sumap=0.;
double i_sumar=0.;
int kodkontr=1;
struct stat work;

if(stat(imafim,&work) != 0)
 {
  sprintf(strsql,"%s %s!",gettext("Нет такого файла"),imafim);
  iceb_t_soob(strsql);
  return(1);
 }

class iceb_fopen ffimp;

if(ffimp.start(imafim,"r") != 0)
 return(1);
 
class iceb_tu_str shet_op("");
while(iceb_tu_fgets(&stroka,ffimp.ff) == 0)
 {
   
  razmer+=strlen(stroka.ravno());

  if(stroka.ravno()[0] == '#')
   continue;


  printw("%s",stroka.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  if(polen(stroka.ravno(),&kod_str,1,'|') != 0)
   continue;  

  if(SRAV(kod_str.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(stroka.ravno(),&kodkontr,NULL);
    continue;
   }
  
  if(SRAV("KDOK",kod_str.ravno(),0) == 0) //Строка с шапкой документа
   {
    if(nomdok.getdlinna() > 1) /*подтверждаем документ и делаем проводки*/
      imp_kasord_r_vp(dd,md,gd,kassa.ravno(),tipzap,nomdok.ravno(),kodop.ravno(),shetd.ravno());

    polen(stroka.ravno(),&tip,2,'|');
    
    if(tip.ravno()[0] == '+')
     tipzap=1;
    if(tip.ravno()[0] == '-')
     tipzap=2;
     
    polen(stroka.ravno(),&kassa,3,'|');
    polen(stroka.ravno(),&bros,4,'|');
    if(rsdat(&dd,&md,&gd,bros.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата !"));
      kol_oh++;
     }

    if(iceb_t_pbpds(md,gd) != 0) /*проверка блокировки*/
      kol_oh++;

    polen(stroka.ravno(),&nomdok,5,'|');
    polen(stroka.ravno(),&shetd,6,'|');
    polen(stroka.ravno(),&kodop,7,'|');
    polen(stroka.ravno(),&osnov,8,'|');
    polen(stroka.ravno(),&dopol,9,'|');
    polen(stroka.ravno(),&fio,10,'|');
    polen(stroka.ravno(),&dokum,11,'|');
    polen(stroka.ravno(),&nomer_bl,12,'|');

    if(metka == 0) //проверка на ошибки
     {
      //Проверяем код кассы
      sprintf(strsql,"select kod from Kas where kod=%s",kassa.ravno());
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код кассы"),kassa.ravno());
        iceb_t_soob(strsql);
        kol_oh++;
       }
       
      //проверяем код операции
      if(tip.ravno()[0] == '+')
        sprintf(strsql,"select shetkor from Kasop1 where kod='%s'",kodop.ravno());
      if(tip.ravno()[0] == '-')
        sprintf(strsql,"select shetkor from Kasop2 where kod='%s'",kodop.ravno());
        
      if(readkey(strsql,&row,&cur) != 1)
       {
        sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
        iceb_t_soob(strsql);
        kol_oh++;
       }

      if(shetd.getdlinna() <= 1)
          shetd.new_plus(row[0]);
      //проверяем счёт
          
      if(prsh1(shetd.ravno(),&rek_shet) != 0)
        kol_oh++;

      //проверяем номер документа
      if(nomdok.ravno()[0] != '\0')
       {
        
        sprintf(strsql,"select nomd from Kasord where nomd='%s' and kassa=%s and tp=%d and god=%d",
        nomdok.ravno(),kassa.ravno(),tipzap,gd);
        if(readkey(strsql) >= 1)
         {
          sprintf(strsql,gettext("Номер документа %s уже есть в базе !"),nomdok.ravno());
          iceb_t_soob(strsql);
          kol_oh++;
         }
           
       
       }
      /*Проверяем номер бланка кассового ордера*/
      if(nomer_bl.getdlinna() > 1)
       {
        sprintf(strsql,"select nomd from Kasord where nb='%s'",nomer_bl.ravno_filtr());
        if(readkey(strsql,&row,&cur) >= 1)
         {
          sprintf(strsql,gettext("Номер бланка %s уже имеет документ с номером %s!"),nomer_bl.ravno(),row[0]);
          iceb_t_soob(strsql);
          kol_oh++;
         }
       
       }    
              
     }
      
    if(metka == 1) //запись в базу шапки документа
     {
      class lock_tables kkkk("LOCK TABLE Kasord WRITE,icebuser READ");
      if(nomdok.ravno()[0] == '\0')
          iceb_t_nomnak(gd,kassa.ravno(),&nomdok,tipzap,2,1);

      if(zaphkor(0,kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),shetd.ravno(),kodop.ravno(),
      osnov.ravno_filtr(),dopol.ravno_filtr(),fio.ravno_filtr(),
      dokum.ravno_filtr(),"","",0,0,0,nomer_bl.ravno_filtr(),"") != 0)
       return(1);
      if(tipzap == 1)
        koldok_p++;
      else
        koldok_r++;
     }
   }
  
  if(SRAV("KZAP",kod_str.ravno(),0) == 0 || SRAV("\"KZAP",kod_str.ravno(),0) == 0) //Строка содержимым документа
   {
    polen(stroka.ravno(),&kontr,2,'|');
    polen(stroka.ravno(),&suma,3,'|');
    polen(stroka.ravno(),&inn,4,'|');
    polen(stroka.ravno(),&fam,5,'|');

    if(fam.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Не введено наименование контрагента"));
      kol_oh++;
     }

    if(iceb_t_get_kkfname(fam.ravno(),kontr.ravno(),&kontr,&kodkontr) < 0)
     {
      kol_oh++;
      continue;
     }

#if 0    
############################################33
    /*Проверяем индивидуальный налоговый номер*/

    class iceb_tu_str kod_kontr("");
    if(inn.getdlinna() > 1)
     {         
      sprintf(strsql,"select kodkon from Kontragent where kod='%s'",inn.ravno());
      if(readkey(strsql,&row,&cur) > 0)
        kod_kontr.new_plus(row[0]);
     }

    if(kod_kontr.getdlinna() <= 1)
     {
      /*Проверяем по фамилии*/
       
      sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",fam.ravno_filtr());
      if(readkey(strsql,&row,&cur) > 0)
        kod_kontr.new_plus(row[0]);
     }        

    if(kod_kontr.getdlinna() <= 1)
     {
      /*Записываем в базу данных нового контрагента*/
      class lock_tables kkkk("LOCK TABLE Kontragent WRITE,icebuser READ");
      if(kontr.getdlinna() > 1) /*проверяем свободен ли код*/
       {
        sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr.ravno_filtr());
        if(readkey(strsql) >= 1)
          kontr.new_plus(iceb_t_getnkontr(kodkontr));     
       }
      else
        kontr.new_plus(iceb_t_getnkontr(kodkontr));     

      sprintf(strsql,"insert into Kontragent (kodkon,naikon,kod,ktoz,vrem) values ('%s','%s','%s',%d,%ld)",
      kontr.ravno_filtr(),fam.ravno_filtr(),inn.ravno(),iceb_t_getuid(),time(NULL));
      
      if(sql_zapis(strsql,1,0) != 0)
       {
        kol_oh++;
        continue;
       }
      kodkontr_new->plus(kontr.ravno());
      kodkontr=kontr.ravno_atoi()+1;

     }
    else
     kontr.new_plus(kod_kontr.ravno());
###############################
#endif        

    if(metka == 1) //запись в базу строки документа
     {
      /*проверяем может контрагент грузится повторно*/
      sprintf(strsql,"select suma from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      kassa.ravno_atoi(),
      gd,
      tipzap,
      nomdok.ravno_filtr(),
      kontr.ravno_filtr());
      if(readkey(strsql,&row,&cur) == 1)
       {
        if(zapzkor(1,kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),kontr.ravno_filtr(),suma+atof(row[0]),kontr.ravno_filtr(),"") != 0)
         {
          iceb_t_soob(gettext("Ошибка записи в кассовый ордер"));
          kol_oh++;
          continue;
         }
       }
      else      
       if(zapzkor(0,kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),kontr.ravno_filtr(),suma,"","") != 0)
        {
         iceb_t_soob(gettext("Ошибка записи в кассовый ордер"));
         kol_oh++;
         continue;
        }

      if(tipzap == 1)
       i_sumap+=suma;
      else
       i_sumar+=suma;
#if 0
##########################################################33      
      /*подтверждаем  документ*/
      sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
      gd,md,dd,kassa.ravno(),gd,tipzap,nomdok.ravno());

      sql_zapis(strsql,0,0);
      

      //Читаем счёт кассы в коде операции если он там есть
      shetk.new_plus("");

      if(tipzap == 1)
       sprintf(strsql,"select shetkas,metkapr from Kasop1 where kod='%s'",kodop.ravno());
      if(tipzap == 2)
       sprintf(strsql,"select shetkas,metkapr from Kasop2 where kod='%s'",kodop.ravno());

      if(readkey(strsql,&row,&cur) == 1)
       {
        shetk.new_plus(row[0]);
        if(row[1][0] == '0')
         continue; /*проводок делать для этой операции не нужно*/
       }

      //Читаем счёт кассы
      if(shetk.ravno()[0] == '\0')
       {
        sprintf(strsql,"select shet from Kas where kod=%s",kassa.ravno());
        if(readkey(strsql,&row,&cur) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa.ravno());
          iceb_t_soob(strsql);
          continue;
         }
        else
         shetk.new_plus(row[0]);
        
       }
      if(shetk.ravno()[0] == '\0')
       continue;
      
      avtprkas(kassa.ravno(),tipzap,nomdok.ravno(),dd,md,gd,shetk.ravno(),shetd.ravno(),kodop.ravno()); /*выполняем проводки*/

      provpodko(kassa.ravno(),nomdok.ravno(),dd,md,gd,tipzap); /*проверяем подтверждение документа*/
      provprkas(kassa.ravno(),tipzap,nomdok.ravno(),dd,md,gd,kodop.ravno()); /*проверяем выполнение проводок*/
#################################################3
#endif
     }
   }




 }
strzag(LINES-1,0,work.st_size,razmer);/*при чтении убираются символы управления*/
if(nomdok.getdlinna() > 1) /*подтверждаем документ и делаем проводки*/
  imp_kasord_r_vp(dd,md,gd,kassa.ravno(),tipzap,nomdok.ravno(),kodop.ravno(),shetd.ravno());

if(metka == 1)
 {

  if(kodkontr_new->kolih() > 0)
   {
    printw("%s:\n",gettext("Список добавленных контрагентов в справочник"));
    for(int nom=0; nom < kodkontr_new->kolih(); nom++)
     printw("%*s %s\n",iceb_tu_kolbait(6,kodkontr_new->ravno(nom)),kodkontr_new->ravno(nom),iceb_t_get_pnk(kodkontr_new->ravno(nom),0));
   }

  VVOD SOOB(1);
  
  sprintf(strsql,"%s %d %s %.2f",gettext("Приходных документов"),koldok_p,gettext("на сумму"),i_sumap);

  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s %d %s %.2f",gettext("Расходных документов"),
  koldok_r,
  gettext("на сумму"),
  i_sumar);

  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"Количество ошибок:%d",kol_oh);  
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);

 }

ffimp.end();

return(kol_oh);
}
/**********************************************/
/*подтверждение документа и выполнение проводок*/
/*************************************************/
int imp_kasord_r_vp(short dd,short md,short gd,
const char *kassa,
int tipzap,
const char *nomdok,
const char *kodop,
const char *shetd)
{
char strsql[512];
class SQLCURSOR cur;
SQL_str row;
/*подтверждаем  документ*/
sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
gd,md,dd,kassa,gd,tipzap,nomdok);

sql_zapis(strsql,0,0);


//Читаем счёт кассы в коде операции если он там есть
class iceb_tu_str shetk("");

if(tipzap == 1)
 sprintf(strsql,"select shetkas,metkapr from Kasop1 where kod='%s'",kodop);
if(tipzap == 2)
 sprintf(strsql,"select shetkas,metkapr from Kasop2 where kod='%s'",kodop);

if(readkey(strsql,&row,&cur) == 1)
 {
  shetk.new_plus(row[0]);
  if(row[1][0] == '0')
   return(1); /*проводок делать для этой операции не нужно*/
 }

//Читаем счёт кассы
if(shetk.ravno()[0] == '\0')
 {
  sprintf(strsql,"select shet from Kas where kod=%s",kassa);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa);
    iceb_t_soob(strsql);
    return(1);
   }
  else
   shetk.new_plus(row[0]);
  
 }
if(shetk.ravno()[0] == '\0')
 return(1);

avtprkas(kassa,tipzap,nomdok,dd,md,gd,shetk.ravno(),shetd,kodop); /*выполняем проводки*/

provpodko(kassa,nomdok,dd,md,gd,tipzap); /*проверяем подтверждение документа*/
provprkas(kassa,tipzap,nomdok,dd,md,gd,kodop); /*проверяем выполнение проводок*/
return(0);
}