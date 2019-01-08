/*$Id: iceb_t_getuslp.c,v 5.6 2013/12/31 11:48:20 sasa Exp $*/
/*27.04.2018	16.02.2011	Белых А.И.	iceb_t_getuslp.c
Получить условие продажи 
*/
#include "buhl.h"

int iceb_t_getuslp(const char *kodkon,class iceb_tu_str *uslprod)
{
return(iceb_t_getuslp(kodkon,uslprod,0));
}
/*****************************************************************/
int iceb_t_getuslp(const char *kodkon,class iceb_tu_str *uslprod,int metka) /* 0-вид договора 1-основание*/
{
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd,vnn from Kontragent2 where kodkon='%s' order by pu desc,datd desc limit 1",kodkon);
if(readkey(strsql,&row,&cur) == 1)
 {
  if(metka == 0)
   {
    sprintf(strsql,"%s:%s:%s",row[2],iceb_tu_datzap(row[0]),row[1]);
    if(row[3][0] != '\0')
     {
      strcat(strsql,":");
      strcat(strsql,row[3]);
     }    
   }
  if(metka == 1)
    sprintf(strsql,"%s %s %s N%s",row[2],gettext("от"),iceb_tu_datzap(row[0]),row[1]);
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}
/*********************************/
int iceb_t_getuslp(int un_nom_zap,class iceb_tu_str *uslprod)
{
return(iceb_t_getuslp(un_nom_zap,uslprod,0));
}
/*****************************************/
int iceb_t_getuslp(int un_nom_zap,class iceb_tu_str *uslprod,int metka)
{
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd,vnn from Kontragent2 where nz=%d",un_nom_zap);
if(readkey(strsql,&row,&cur) == 1)
 {
   
  if(metka == 0)
   {
    sprintf(strsql,"%s:%s:%s",row[2],iceb_tu_datzap(row[0]),row[1]);
    if(row[3][0] != '\0')
     {
      strcat(strsql,":");
      strcat(strsql,row[3]);
     }
   }
  if(metka == 1)
   {
    /*после номера документа могут поставить код документа через двоеточие для реестра налоговых накладных*/
    class iceb_tu_str nomdok("");
    if(polen(row[1],&nomdok,1,':') != 0)
     nomdok.new_plus(row[1]);
    sprintf(strsql,"%s %s %s N%s",row[2],gettext("от"),iceb_tu_datzap(row[0]),nomdok.ravno());
   }
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}
