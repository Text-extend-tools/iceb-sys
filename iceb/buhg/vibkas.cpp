/* $Id: vibkas.c,v 5.12 2013/05/17 14:56:15 sasa Exp $ */
/*14.09.2008	10.03.2000	Белых А.И. 	vibkas.c
Выбор кассового аппарата
Возвращаем номер кассового аппарата или -1 если не выбрали
*/
#include        <ctype.h>
#ifdef HAVE_TORGDEV
#include	<libconfig.h>
#include	<libdev.h>
#endif
#include	"buhg.h"

#ifndef CONFIG_INI
#define CONFIG_INI "iceb.ini"
#endif

#ifdef HAVE_TORGDEV
extern cassa_pars *cp[MAX_CASS];
#endif

int		vibkas(char imqkas[],//Наименование кассы
int *termlin, //Номер терминальной линии
int *nomernl, //Номер на линии
int *tipkas)  //Тип кассы
{
int		nomer=-1;
#ifdef HAVE_TORGDEV
SPISOK	masivmenu;
long		dlinm;
int		i,i1,i2,i3;
char		bros[512];
char		imqkas1[512];



dlinm=0;
for (i=0; i< MAX_CASS; i++) 
 {
  if (cp[i] != NULL)
   {
    memset(imqkas1,'\0',sizeof(imqkas1));
    if(cp[i]->type == T_DATECS)
      strncpy(imqkas1,devmsg(28,"DATECS или SAMSUNG"),sizeof(imqkas1)-1);
    if(cp[i]->type == T_ERA)
      strncpy(imqkas1,devmsg(27,"ЕРА-802"),sizeof(imqkas1)-1);

    memset(bros,'\0',sizeof(bros));

    i1=strlen(cp[i]->port);
    i2=0;
    for(i3=0; i3 < i1; i3++)
     if(isdigit(cp[i]->port[i3]) != 0) 
       bros[i2++]=cp[i]->port[i3];

    i2=atoi(bros);

    memset(bros,'\0',sizeof(bros));
    sprintf(bros,"%-20s|%d-%d",imqkas1,cp[i]->num,i2);
    masivmenu.plus(bros);
   }
  else 
    break;
 }

if(masivmenu.kolih() == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Нет списка кассовых апаратов !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(-1);
 }



helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);
int kod_kl=0;
nomer=dirmasiv(&masivmenu, -1, -1,0,"Виберіть потрібний касовий апарат.",1,&kod_kl);
if(nomer < 0 || kod_kl != ENTER )
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Увага ! Не вибрано касовий апарат !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  imqkas[0]='\0';
  return(-1);
 }
nomer-=1;
*nomernl=cp[nomer]->num;
*tipkas=cp[nomer]->type;

i1=strlen(cp[nomer]->port);
memset(bros,'\0',sizeof(bros));

i2=0;
for(i=0; i < i1; i++)
 if(isdigit(cp[nomer]->port[i]) != 0) 
   bros[i2++]=cp[nomer]->port[i];

*termlin=atoi(bros);

if(cp[nomer]->type == T_DATECS)
  strcpy(imqkas,devmsg(28,"DATECS или SAMSUNG"));
if(cp[nomer]->type == T_ERA)
  strcpy(imqkas,devmsg(27,"ЕРА-802"));
VVOD DANET(1);
      
DANET.VVOD_SPISOK_add_MD("Переведіть касовий апарат в стан зв'язку з комп'ютером.");
if(cp[nomer]->type == T_DATECS)
 {
  DANET.VVOD_SPISOK_add_MD("(Установіть ключ в положення \"ВИКЛ\" і натисніть клавішу 9.");
  DANET.VVOD_SPISOK_add_MD("       На індикаторі має з'явитися OFF-ln)");
 }
DANET.VVOD_SPISOK_add_MD("                        Зроблено ?");
clearstr(LINES-1,0);

if(danet(&DANET,2,stdscr) == 1)
 {
  GDITE();
 

  if((i=Cassa_connect(cp[nomer])) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD("Зв'зок з кассою не встановлено !");
    sprintf(bros,"Помилка: %s",CA_error(i));
    SOOB.VVOD_SPISOK_add_MD(bros);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(-1);
   }
  else
   {
    clear();
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD("Зв'зок з кассою встановлено !");
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }            
#endif
return(nomer);
}
