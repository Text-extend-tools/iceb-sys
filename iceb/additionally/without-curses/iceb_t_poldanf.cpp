/*$Id: iceb_t_poldanf.c,v 5.11 2013/08/13 05:50:23 sasa Exp $*/
/*05.12.2013	27.09.2009	Белых А.И. 	iceb_t_poldanf.c
Поиск настройки в файле и получение значения принадлежащее этой настройке
*/

#include "bezcurses.h"


int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf,
int metka_ps) /*0-без перевода строки если несколько строк 1- с переводом строки*/
{
char strsql[strlen(strpoi)+1024];
SQL_str row;
class iceb_tu_str prom("");
class SQLCURSOR cur;
int kol=0;
find_dat->new_plus("");
sprintf(strsql,"select ns,str from Alx where fil='%s' and str like '%s|%%'",imaf,strpoi);
if((kol=readkey(strsql,&row,&cur)) <= 0)
 {
//  printw("%s-%s\n",__FUNCTION__,strsql);
/*********
  sprintf(strsql,"%s\n%s\n%s",gettext("Ненайдена настройка!"),strpoi,imaf);
  iceb_t_soob(strsql);
***********/
  return(1);  
 }

if(kol > 1)
  printf("%s-%s %s\n%s\n%s %d",__FUNCTION__,gettext("Имя файла"),imaf,strpoi,gettext("Количество настроек"),kol);

if(polen(row[1],&prom,2,'|') == 0)
 {
  if(polen(prom.ravno(),find_dat,1,'\\') != 0)
   {
    find_dat->new_plus(prom.ravno());
    return(0);
   }
  int ns=atoi(row[0]);
  int otn=1;
  while(prom.ravno()[strlen(prom.ravno())-otn] == '\\')
   {
    sprintf(strsql,"select ns,str from Alx where fil='%s' and ns=%d",imaf,++ns);
    if(readkey(strsql,&row,&cur) != 1)
      return(0);    

    if(row[1][0] == '#')
     continue;

    prom.new_plus(row[1]);

    class iceb_tu_str prom1("");

    if(polen(prom.ravno(),&prom1,1,'\\') != 0)
     {
      int dlin=strlen(prom.ravno())-1;
      if(prom.ravno()[dlin] == '\n')
        prom.udsimv("\n");
//       prom[dlin]='\0';
      find_dat->plus(prom.ravno());
      return(0);
     }
    otn=2;
    find_dat->plus(prom1.ravno());
    if(metka_ps == 1)
     find_dat->plus("\n");
   }
  return(0);
 }


return(1);

}
/********************************/
int iceb_t_poldan(const char *strpoi,class iceb_tu_str *find_dat,const char *imaf)
 {
  return(iceb_t_poldan(strpoi,find_dat,imaf,0));
 }
/********************************/

int iceb_t_poldan(const char *strpoi,char *find_dat,const char *imaf)
{
class iceb_tu_str str("");
find_dat[0]='\0';

if(iceb_t_poldan(strpoi,&str,imaf,0) != 0)
 return(1);

strcpy(find_dat,str.ravno());
return(0);
}
/****************************/
int iceb_t_poldan(const char *strpoi,int *kod,const char *imaf)
{
*kod=0;
class iceb_tu_str strok("");
if(iceb_t_poldan(strpoi,&strok,imaf) == 0)
 {
  *kod=strok.ravno_atoi();
  return(0);
 }
return(1);
}
/****************************/
int iceb_t_poldan(const char *strpoi,short *kod,const char *imaf)
{
*kod=0;
class iceb_tu_str strok("");
if(iceb_t_poldan(strpoi,&strok,imaf) == 0)
 {
  *kod=strok.ravno_atoi();
  return(0);
 }
return(1);
}
/****************************/
int iceb_t_poldan(const char *strpoi,double *kod,const char *imaf)
{
*kod=0.;
class iceb_tu_str strok("");
if(iceb_t_poldan(strpoi,&strok,imaf) == 0)
 {
  *kod=strok.ravno_atof();
  return(0);
 }
return(1);
}
