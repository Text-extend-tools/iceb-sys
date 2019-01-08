/*$Id: poldan.c,v 5.19 2014/06/30 06:36:42 sasa Exp $*/
/*08.05.2014    12.03.1993      Белых А.И.      poldan.c
Подпрограмма получения нужных данных из файла настройки
Если вернули 0 образец найден
	     1- в файле нет строки
	     2- нет такого файла
*/
#include        <errno.h>
#include	"util.h"


int poldan(const char *obr, //Образец поиска
char *str, //Найденная строка
const char *imaf) //Имя файла с настройкой
{

 class iceb_tu_str stroka("");
 int voz=0;
 voz=poldan(obr,&stroka,imaf);
 if(voz == 0)
  strcpy(str,stroka.ravno());
 return(voz);
}

/*****************************************/
/*****************************************/

int poldan(const char *obr, //Образец поиска
class iceb_tu_str *str, //Найденная строка
const char *imaf) //Имя файла с настройкой
{
class iceb_tu_str stroka("");
class iceb_tu_str st1("");
FILE            *f;
short           voz;
class iceb_tu_str stroka1("");
class iceb_tu_str stroka2("");

voz=-1;
if((f = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
   return(errno);
 else
   {
    return(errno);
   }
 }
while(iceb_tu_fgets(&stroka,f) == 0)
 {
  polen(stroka.ravno(),&st1,1,'|');
  if(SRAV(st1.ravno(),obr,0) == 0)
   {
     polen(stroka.ravno(),&stroka1,2,'|');
     iceb_tu_fplus(stroka1.ravno(),&stroka2,f);
     str->new_plus(stroka2.ravno());     
     voz=0;
     break;
   }
 }

fclose(f);
return(voz);
}
#if 0
#####################################################33
/*****************************************/
/*****************************************/

int poldan(const char *obr, //Образец поиска
char *str, //Найденная строка
const char *imaf) //Имя файла с настройкой
{
//char            st[1024];
class iceb_tu_str stroka("");
class iceb_tu_str st1("");
FILE            *f;
short           voz;
class iceb_tu_str stroka1("");
class iceb_tu_str stroka2("");

voz=-1;
if((f = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
   return(errno);
 else
   {
    return(errno);
   }
 }
//while( fgets(st,sizeof(st),f) != NULL)
while(iceb_tu_fgets(&stroka,f) == 0)
 {
/***************
  polen(st,&st1,1,'|');
  if(SRAV(st1.ravno(),obr,0) == 0)
   {
     polen(st,&stroka1,2,'|');
     iceb_tu_fplus(stroka1.ravno(),&stroka2,f);
     strcpy(str,stroka2.ravno());     
     voz=0;
     break;
   }
*******************/
  polen(stroka.ravno(),&st1,1,'|');
  if(SRAV(st1.ravno(),obr,0) == 0)
   {
     polen(stroka.ravno(),&stroka1,2,'|');
     iceb_tu_fplus(stroka1.ravno(),&stroka2,f);
     strcpy(str,stroka2.ravno());     
     voz=0;
     break;
   }
/********************/
 }

fclose(f);
return(voz);
}
##############################################
#endif
