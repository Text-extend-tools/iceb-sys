/* $Id: provsh.c,v 5.25 2013-04-07 10:22:54 sasa Exp $ */
/*12.09.2013	20.07.1998	Белых А.И.	provshf.c
Проверка счета для автоматических проводок
Проверка на наличие в плане счетов такого счета и
если счет с развернутым сальдо то проверка на наличие
кода контрагента в списке счета
Если вернули 0 все впорядке
             1 - не найден счет в плане счетов
             2 - не определена организация
             3 - отказались искать организацию
             4 - счет имеет субсчета
             5 - счет не балансовий
*/

#include        <time.h>
#include        "bezcurses.h"


int provsh(class iceb_tu_str *kor,const char *shp4,
struct OPSHET *shetv,
int metka, /*************
           Передается:
           0 запрос на работу со списком контрагентов если контрагента нет в списке счёта
           1 автоматическая вставка кода контрагента в список счета для расчёта зарплаты
           2 автоматическая вставка из других подсистем
           ************/
int metkanbs) //0-не балансовый счет не возможен 1-возможен
{
class iceb_tu_str nai("");
short		i;
char		strsql[1024];
SQL_str         row;

shetv->saldo=0;
if((i=prsh(shp4,shetv)) == 0)
 {
  printf("%s-%s %s %s!\n",__FUNCTION__,gettext("Нет счета"),shp4,gettext("в плане счетов"));
  return(1);
 }
if(i == 2)
 {
  printf("%s-%s %s %s\n",__FUNCTION__,gettext("Счет"),shp4,gettext("имеет субсчёта. Введите нужный субсчёт."));
  return(4);
 }
if(metkanbs == 0  && shetv->stat == 1)
 {
  printf("%s-%s %s %s!\n",__FUNCTION__,gettext("Счет"),shp4,gettext("внебалансовый"));
  return(5);
 }

if(shetv->saldo == 3)
 {
  if(kor->getdlinna() > 1)
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shp4,kor->ravno());
    if(readkey(strsql) != 1)
     {
      if(metka != 0) /*Автоматическая вставка кода контрагента*/
       {
        int metka_prov_kont=0;
        //Проверяем есть ли в общем списке
        sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kor->ravno());
        if((metka_prov_kont=readkey(strsql)) != 1)
         {
          if(metka == 2) /*Добавлять в общий список нельзя*/
           {
            printf("%s-%s %s %s!\n",__FUNCTION__,gettext("Не найден код контрагента"),kor->ravno(),gettext("в общем списке"));
            return(3);
           }

         }
        
        sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",shp4,kor->ravno(),iceb_t_getuid(),time(NULL));
        sql_zapis(strsql,1,0);

        if(metka_prov_kont == 1) /*Есть в общем списке*/
         return(0);
                 
        if(metka == 1)
         {
          
          //Узнаем фамилию
          nai.new_plus("");

          class iceb_tu_str tabn;
          tabn.plus(iceb_tu_adrsimv(iceb_tu_strlen(ICEB_ZAR_PKTN),kor->ravno()));

          class iceb_tu_str adres("");

          class iceb_tu_str telef("");

          class iceb_tu_str inn("");
          
          SQLCURSOR curr;
          sprintf(strsql,"select fio,adres,telef,inn from Kartb where tabn=%s",tabn.ravno());
          if(readkey(strsql,&row,&curr) == 1)
           {
            nai.new_plus(row[0]);

            adres.new_plus(row[1]);

            telef.new_plus(row[2]);
         
            inn.new_plus(row[3]);
           }
          sprintf(strsql,"replace into Kontragent \
(kodkon,naikon,adres,innn,telef,ktoz,vrem) \
values ('%s','%s','%s','%s','%s',%d,%ld)",kor->ravno(),nai.ravno_filtr(),adres.ravno_filtr(),inn.ravno(),telef.ravno_filtr(),iceb_t_getuid(),time(NULL));
          if(sql_zap(&bd,strsql) != 0)
           {
            msql_error(&bd,__FUNCTION__,strsql);
           }
          return(0);
         }        
         
       }

      printf("%s-%s %s %s %s!\n",__FUNCTION__,gettext("Не найден код"),kor->ravno(),gettext("в списке счёта"),shp4);

      return(3);
     }
   }
  else
   {


      printf("%s-%s %s!\n",__FUNCTION__,gettext("Не задан код контрагента для счета"),shp4);

      return(3);
   }
 }
else
 kor->new_plus("");

return(0);
}
