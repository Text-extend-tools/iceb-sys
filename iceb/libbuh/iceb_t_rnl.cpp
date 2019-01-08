/*$Id:$*/
/*24.04.2018	22.10.2014	Белых А.И.	iceb_t_rnl.c
Разбивка рапечатки на листы со вставкой шапок и нумерацией страниц
Передаётся адрес подпрограммы которая печатает шапку документа
и устанавливает количество строк в шапке документа
*/
#include "buhl.h"

extern int  kol_strok_na_liste; //Количесво строк на стандартном листе
extern int  kol_strok_na_liste_l; //Количесво строк на стандартном листе в ориентации ландшафт

int iceb_t_rnl(const char *imaf,
class iceb_rnl_c *rh, /*реквизиты необходимые для печати шапки*/
void (*hapka1)(class iceb_rnl_c *rh,int *nom_str,FILE *ff)) /*подпрограмма которая печатает шапку документа*/
{
class iceb_tu_str stroka("");
char imaf2[64];
int kolstrok=0;
int metka_end=0;
int nom_str=0;

if(rh->orient <= 0)
 kolstrok=kol_strok_na_liste;

if(rh->orient == 1)
 kolstrok=kol_strok_na_liste_l;

if(kolstrok == 0)
 kolstrok=rh->orient;
 
class iceb_fopen fil1;

if(fil1.start(imaf,"r") != 0)
 return(1);


sprintf(imaf2,"dv%d.tmp",getpid());

class iceb_fopen fil2;
if(fil2.start(imaf2,"w") != 0)
 return(1);

//rh->ff=fil2.ff;

while(iceb_tu_fgets(&stroka,fil1.ff) == 0)
 {
  if(strstrm(stroka.ravno(),"ICEB_LST_END") == 1) /*Дальше шапки не вставлять*/
   {
    metka_end=1;
    continue;
   }
  if(strstrm(stroka.ravno(),"ICEB_LST_START") == 1) /*Начало нового расчёта страниц*/
   {
//    nom_list=1;   
    rh->nom_list=1;
    metka_end=0;
    nom_str=0;
    if(strstrm(stroka.ravno(),"\f") == 1)
      fprintf(fil2.ff,"\f");
    continue;
   }

  if(metka_end == 0)
   nom_str++;

  if(nom_str > kolstrok)
   {
    if(rh->obr.ravno()[0] != '\0')
    if(SRAV(stroka.ravno(),rh->obr.ravno(),1) == 0) /*строка которую надо игнорировать например строка подчёркивания в этом случае не выводить*/
     {
      nom_str--;
      continue;
     }

    fprintf(fil2.ff,"\f");

//    printw("\n%s\n",__FUNCTION__);
//    OSTANOV();

    rh->nom_list++; /*увеличивать нужно именно здесь*/
    nom_str=0;  
    hapka1(rh,&nom_str,fil2.ff);/*Внутри обнуляется счётчик строк*/
  
    nom_str++; /*учитываем строку перед которой напечатали шапку*/

   }

  fprintf(fil2.ff,"%s",stroka.ravno());

 }

fil1.end();
fil2.end();

unlink(imaf);

rename(imaf2,imaf);
return(0);
}
/************************************************************/
/***********************************************************/

int iceb_t_rnl(const char *imaf,int orient, /*0-портрет, 1-ландшафт, больше 1 это количество строк на листе*/
const char *obr,/*образец начала cтроки которую надо игнорировать*/
void (*hapka)(int nom,int *kostr,FILE *FF)) /*подпрограмма которая печатает шапку документа*/
{
int nom_str=0;
int nom_list=1; /*на первом листе шапка должна быть*/
class iceb_tu_str stroka("");
char imaf2[64];
int kolstrok=0;
int metka_end=0;

if(orient <= 0)
 kolstrok=kol_strok_na_liste;

if(orient == 1)
 kolstrok=kol_strok_na_liste_l;

if(kolstrok == 0)
 kolstrok=orient;
 
class iceb_fopen fil1;

if(fil1.start(imaf,"r") != 0)
 return(1);


sprintf(imaf2,"dv%d.tmp",getpid());

class iceb_fopen fil2;
if(fil2.start(imaf2,"w") != 0)
 return(1);

while(iceb_tu_fgets(&stroka,fil1.ff) == 0)
 {
  if(strstrm(stroka.ravno(),"ICEB_LST_END") == 1) /*Дальше шапки не вставлять*/
   {
    metka_end=1;
    continue;
   }
  if(strstrm(stroka.ravno(),"ICEB_LST_START") == 1) /*Начало нового расчёта страниц*/
   {
    nom_list=1;   
    metka_end=0;
    nom_str=0;
    if(strstrm(stroka.ravno(),"\f") == 1)
      fprintf(fil2.ff,"\f");
    continue;
   }

  if(metka_end == 0)
   nom_str++;

  if(nom_str > kolstrok)
   {
    if(obr != NULL)
    if(SRAV(stroka.ravno(),obr,1) == 0) /*строка которую надо игнорировать например строка подчёркивания в этом случае не выводить*/
     {
      nom_str--;
      continue;
     }
    fprintf(fil2.ff,"\f");
    nom_str=0;
    hapka(++nom_list,&nom_str,fil2.ff);
    nom_str+=1;       

   }

  fprintf(fil2.ff,"%s",stroka.ravno());

 }

fil1.end();
fil2.end();

unlink(imaf);

rename(imaf2,imaf);
return(0);
}
