/* $Id: vzrkbz.c,v 5.16 2013-04-07 10:22:44 sasa Exp $ */
/*26.11.2012	22.07.1997	Белых А.И.	vzrkbz.c
Взять реквизиты организаций из базы для заполнения платежных 
поручений
*/
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

void		vzrkbz()
{
class iceb_tu_str kodo("");
class iceb_tu_str naim("");
SQL_str         row;
char		strsql[512];


if(dirkontr(&kodo,&naim,1) == 0)
 {
  SQLCURSOR cur;

  char kontr00[64];
  memset(kontr00,'\0',sizeof(kontr00));
    
  iceb_t_poldan("Код контрагента для платёжного поручения",kontr00,"nastdok.alx");
  if(kontr00[0] == '\0')
    strcpy(kontr00,"00");
  
  sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim from \
Kontragent where kodkon='%s'",kontr00);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    char strok[1024];
    sprintf(strok,"%s-%s %s !",__FUNCTION__,gettext("Не найден код контрагента"),kontr00);
    iceb_t_soob(strok);
   }
  else
   {
    rec.kodor.new_plus("00");


    if(row[6][0] == '\0')          
     {

      if(polen(row[0],&rec.naior,1,'(') != 0)    
        rec.naior.new_plus(row[0]);
     }
    else
      rec.naior.new_plus(row[6]);


    rec.kod.new_plus(row[1]);
    rec.naiban.new_plus(row[2]);
    rec.mfo.new_plus(row[3]);
    rec.nsh.new_plus(row[4]);
    rec.gorod.new_plus(row[5]);
   }

  sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim from \
Kontragent where kodkon='%s'",kodo.ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найден код контрагента которому платят"));
   }
  else
   {
    rec.kodor1.new_plus(kodo.ravno());

    if(row[6][0] == '\0')          
     {
      if(polen(row[0],&rec.naior1,1,'(') != 0)    
       rec.naior1.new_plus(row[0]);
     }
    else
      rec.naior1.new_plus(row[6]);
    rec.kod1.new_plus(row[1]);
    rec.naiban1.new_plus(row[2]);
    rec.mfo1.new_plus(row[3]);
    rec.nsh1.new_plus(row[4]);
    rec.gorod1.new_plus(row[5]);
   }
  }
}
