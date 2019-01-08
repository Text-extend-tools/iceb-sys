/*$Id: cennikm.c,v 5.20 2013/11/05 10:50:48 sasa Exp $*/
/*13.07.2015	08.5.2003	Белых А.И.	cennikm.c
Распечатать ценники на отмеченные материалы
*/
#include        <errno.h>
#include	"buhg.h"

extern int kol_strok_na_liste;
extern double	okrg1;  /*Округление суммы*/

void cennikm(char *mas)
{
char 	strsql[512];
SQL_str	row1;
SQLCURSOR cur;
class iceb_tu_str naim("");
double cena=0.;
class iceb_tu_str strihkod("");
class iceb_tu_str kodm("");
time_t          tmm;
struct  tm      *bf;

if(mas == NULL)
 return;

GDITE();

time(&tmm);
bf=localtime(&tmm);

int kp=pole2(mas,','); //Количество полей в массиве
if(kp == 0)
 kp=1;



FILE *ff;
char imaf[64];

sprintf(imaf,"cenm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

for(int i=0; i< kp; i++)
 {
  if(i == 0 && polen(mas,&kodm,i+1,',') != 0)
     kodm.new_plus(mas);
  if(i != 0)
     polen(mas,&kodm,i+1,',');
  cena=0.;
  naim.new_plus("");
  strihkod.new_plus("");

  sprintf(strsql,"select naimat,strihkod,cenapr from Material where kodm=%s",kodm.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur) == 1)
   {
    naim.new_plus(row1[0]);
    strihkod.new_plus(row1[1]);

    cena=atof(row1[2]);
    cena=okrug(cena,okrg1);
   }  

  cennik(bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodm.ravno(),naim.ravno(),cena,strihkod.ravno(),iceb_t_get_pnk("00",0),ff);

 }
fclose(ff);

cenniks(imaf); //Преобразование в колонки

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Ценники на товары"));
iceb_t_ustpeh(imaf,1);

iceb_t_rabfil(&oth,"");
}
/********************************************/
/*Складывание распечатки в несколько колонок*/
/********************************************/

void	cenniks(char *imaf)
{
FILE	*ff,*fftmp;
char    imaftmp[64];
char	stroka[1024];
char    masiv[10][512];
short	kol=2; //Количество колонок в распечатке
short   kolstrlist=0;

sprintf(imaftmp,"cen%d.tmp",getpid());
//printw("\nimaftmp=%s\n",imaftmp);
//refresh();

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }

if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

short shethik=0;
short kolstr=0;
memset(&masiv,'\0',sizeof(masiv));
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  stroka[strlen(stroka)-1]='\0';

    
//  if(stroka[0] == '┌')
  if(strstrm(stroka,"┌") == 1)
    kolstr=0;

  if(shethik == 0)
   {
    strcpy(masiv[kolstr],stroka);
//    printw("*%d %s\n",kolstr,masiv[kolstr]);
   }    
  else
   {
    strcat(masiv[kolstr]," ");
    strcat(masiv[kolstr],stroka);
//    printw("**%d %s\n",kolstr,masiv[kolstr]);
   }

  kolstr++;

//  if(stroka[0] == '└')
  if(strstrm(stroka,"└") == 1)
    shethik++;

  if(shethik == kol)
   {
    if(kolstrlist+kolstr >= kol_strok_na_liste)
     {
      fprintf(fftmp,"\f");
      kolstrlist=kolstr;
     }
    else
     kolstrlist+=kolstr;
     
    for(int i=0; i < kolstr; i++)
      fprintf(fftmp,"%s\n",masiv[i]);
    memset(&masiv,'\0',sizeof(masiv));

    shethik=0;
   }
  
 }

if(masiv[0][0] != '\0')
  for(int i=0; i < kolstr; i++)
     fprintf(fftmp,"%s\n",masiv[i]);


fclose(ff);
fclose(fftmp);

rename(imaftmp,imaf);

}
