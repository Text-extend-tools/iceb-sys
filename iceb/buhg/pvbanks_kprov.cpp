/*$Id: pvbanks_kprov.c,v 5.12 2013/09/26 09:43:38 sasa Exp $*/
/*18.10.2014	20.02.2008	Белых А.И.	pvbanks_kprov.c
проверка контрагента
*/
#include "buhg.h"


int pvbanks_kprov(class SPISOK *KONT,int snom,const char *pris,int metkadk,double *sum,class iceb_tu_str *tabnom,
double *deb,double *kre,int kolih_simv,class iceb_tu_str *fio,class iceb_tu_str *bankshet,class iceb_tu_str *inn,const char *grupk,int kod_bank)
{
char strsql[1024];
class iceb_tu_str kontrag("");

fio->new_plus("");
bankshet->new_plus("");
tabnom->new_plus("");
inn->new_plus("");

kontrag.new_plus(KONT->ravno(snom));

if(grupk[0] != '\0')
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kontrag.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   {
     if(proverka(grupk,row[0],0,0) != 0)
       return(1);
   }
 }


if(strstrm(kontrag.ravno(),pris) == 0)
 return(1);
 
if(metkadk == 0)
  *sum=deb[snom]-kre[snom];
if(metkadk == 1)
  *sum=kre[snom]-deb[snom];

if(*sum <= 0.009)
 return(1);

tabnom->new_plus(iceb_tu_adrsimv(kolih_simv,kontrag.ravno()));
SQL_str row1;
SQLCURSOR cur1;
  
//Читем в карточке фамилию и картсчет   
sprintf(strsql,"select fio,bankshet,inn,kb from Kartb where tabn=%s",tabnom->ravno());
if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
 {
  fio->new_plus(row1[0]);
  bankshet->new_plus(row1[1]);
  inn->new_plus(row1[2]);
 }


if(kod_bank != 0)
 {
  sprintf(strsql,"select nks from Zarkh where tn=%s and kb=%d",tabnom->ravno(),kod_bank);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Для работника %s %s нет картсчёта для кода банка %d!",tabnom->ravno(),fio->ravno(),kod_bank);
    iceb_t_soob(strsql);
    return(1);
   }
  bankshet->new_plus(row1[0]);
 
 }  

if(bankshet->ravno()[0] == '\0')
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
  sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),tabnom->ravno(),fio->ravno());
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
return(0);
}
