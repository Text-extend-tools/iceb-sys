/*$Id: iceb_t_fplus.c,v 5.4 2013-04-07 10:22:54 sasa Exp $*/
/*09.11.2008	28.11.2007	Белых А.И.	iceb_t_fplus.c
Подпрограмма чтения продолжения строки из файла
*/
#include "buhl.h"


void iceb_t_fplus(const char *stroka,class iceb_tu_str *stroka1,class SQLCURSOR *cur)
{
SQL_str row_alx;

if(polen(stroka,stroka1,1,'\\') != 0)
 {
  stroka1->new_plus(stroka);
  return;
 }
char strokk[2048];
for(;;)
 {

  if(cur->read_cursor(&row_alx) == 0)
   return;
  memset(strokk,'\0',sizeof(strokk));
  strncpy(strokk,row_alx[0],sizeof(strokk)-1);
  
//  if(fgets(strokk,sizeof(strokk),ff) == NULL)
//   return;
   
   
  if(strokk[0] == '#')
   continue;

  class iceb_tu_str strokk1("");
  if(polen(strokk,&strokk1,1,'\\') != 0)
   {
    /*удаляем символ перевода строки*/
    int dlina=strlen(strokk);
    if(strokk[dlina-1] == '\n')
     strokk[dlina-1]='\0';
    
    stroka1->plus(strokk);
    return;
   }
  stroka1->plus(strokk1.ravno());
 }
}

/*************************************************/
/************************************************/

void iceb_t_fplus(int metka_sp, /*0-разделённые запятыми в список 1-просто прибавить строку*/
const char *stroka,class SPISOK *nastr,class SQLCURSOR *cur)
{
class iceb_tu_str s_nastr;

iceb_t_fplus(stroka,&s_nastr,cur);

if(metka_sp == 1)
 {
  nastr->plus(s_nastr.ravno());
  return;
 }

class iceb_tu_str sodp("");
int kolihp=pole2(s_nastr.ravno(),',');
if(kolihp == 0)
 {
  nastr->plus(stroka);
  return;  
 }
for(int ii=0; ii <= kolihp; ii++)
 {
  polen(s_nastr.ravno(),&sodp,ii+1,',');
  if(sodp.ravno()[0] == '\0')
   continue;  
  nastr->plus(sodp.ravno());
 }
}

