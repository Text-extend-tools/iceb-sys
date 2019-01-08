/*$Id: iceb_tu_fplus.c,v 5.8 2013/08/13 05:50:32 sasa Exp $*/
/*22.07.2013	28.11.2007	Белых А.И.	iceb_tu_fplus.c
Подпрограмма чтения продолжения строки из файла
*/
#include "util.h"



void iceb_tu_fplus(const char *stroka,class iceb_tu_str *stroka1,FILE *ff)
{

if(polen(stroka,stroka1,1,'\\') != 0)
 {
  stroka1->new_plus(stroka);
  return;
 }
char strokk[2048];
for(;;)
 {

  memset(strokk,'\0',sizeof(strokk));
  if(fgets(strokk,sizeof(strokk),ff) == NULL)
   return;
   
   
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

void iceb_tu_fplus(int metka_sp, /*0-разделённые запятыми в список 1-просто прибавить строку*/
const char *stroka,class SPISOK *nastr,FILE *ff)
{
class iceb_tu_str s_nastr;
iceb_tu_fplus(stroka,&s_nastr,ff);
if(metka_sp == 1)
 {
  nastr->plus(s_nastr.ravno());
  return;
 }

char sodp[2048];
/*printw("iceb_tu_fplus-%s %s\n",stroka,s_nastr.ravno());*/
int kolihp=pole2(s_nastr.ravno(),',');
if(kolihp == 0)
 {
  nastr->plus(stroka);
  return;  
 }
for(int ii=0; ii <= kolihp; ii++)
 {
  memset(sodp,'\0',sizeof(sodp));
  polen(s_nastr.ravno(),sodp,sizeof(sodp),ii+1,',');
  if(sodp[0] == '\0')
   continue;  
  nastr->plus(sodp);
 }
}

