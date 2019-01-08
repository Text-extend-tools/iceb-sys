/*$Id: pehf.c,v 5.12 2013/05/17 14:56:31 sasa Exp $*/
/*14.02.2010	14.10.1997	Белых А.И.	pehf.c
Печать файла на системный или приэкранный принтер
*/
#include	"buhl.h"



void pehf(const char *imaf, //Имя файла
short kolih_kop,  //Количество копий
const char *print)  //Принтер
{
class iceb_tu_str bros("");
short kl;

if(kolih_kop == 0)
 {      
  helstr(LINES-1,0,"F10",gettext("выход"),NULL);
     
  VVOD  VVOD1(2);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество копий"));

  if((kl=vvod1(&bros,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
  if(kl == ESC)
   return;
  
  GDITE();   

  kolih_kop=bros.ravno_atoi();
  
 }

if(kolih_kop == 0)
 return;
 
move(LINES-5,0);
refresh();


char kluh_kol_kop[56];
memset(kluh_kol_kop,'\0',sizeof(kluh_kol_kop));
//imafn("nastsys.alx",bros);

iceb_t_poldan("Ключ для указания количества копий",kluh_kol_kop,"nastsys.alx");
if(kluh_kol_kop[0] == '\0')
 strcpy(kluh_kol_kop,"-#");

char buffer[512];
sprintf(buffer,"%s%s%s",g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);  
iceb_t_cp(imaf,buffer,0);

sprintf(buffer,"%s %s%d %s%s%s",print,kluh_kol_kop,kolih_kop,g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);
runsystem1(buffer,1);


}
