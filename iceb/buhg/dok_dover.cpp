/*$Id: dok_dover.c,v 5.43 2014/08/31 06:18:18 sasa Exp $*/
/*16.04.2017	05.06.2005	Белых А.И.	dok_dover.c
Выписка доверенности на бланке
*/
#include <errno.h>
#include <ctype.h>
#include "buhg.h"

void dok_dover_rl(const char *nomer_dov,short dv,short mv,short gv,short dd,short md,short gd,
short dvd,short mvd,short gvd,
const char *fio,const char *seriq,const char *nomer,
const char *vid,const char *naim,const char *naimbank,const char *kod,
const char *adres,const char *rasshet,const char *mfo,const char *post,
const char *inn,const char *nsv,const char *telefon,const char *ndok,const char *imaf);


extern class iceb_tu_str nameprinter;

void dok_dover()
{
char strsql[1024];
SQL_str row,row1;
SQLCURSOR cur,cur1;
class iceb_tu_str naim("");
class iceb_tu_str adres("");
class iceb_tu_str kod("");
class iceb_tu_str inn("");
class iceb_tu_str nsv("");
class iceb_tu_str mfo("");
class iceb_tu_str rasshet("");
class iceb_tu_str naimbank("");
class iceb_tu_str adresban("");
class iceb_tu_str telefon("");
class iceb_tu_str vid("");
class iceb_tu_str dvd("");
int  kom1=0;
class iceb_tu_str dat1("");
class iceb_tu_str dat2("");
short d1,m1,g1;
short d2,m2,g2;
short d3,m3,g3;
class iceb_tu_str tabnom("");
class iceb_tu_str post("");
class iceb_tu_str ndoc("");
int tabn;
class iceb_tu_str fio("");
class iceb_tu_str naim_kontr("");
class iceb_tu_str kontr("");
class iceb_tu_str nomer_dov("");

int N=0,K=0;

VVOD VV(0);
VVOD MENU(3);


sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  if(row[16][0] == '\0')
    naim.new_plus(row[1]);
  else
    naim.new_plus(row[16]);  
  adres.new_plus(row[3]);
  kod.new_plus(row[5]);
  inn.new_plus(row[8]);  
  nsv.new_plus(row[9]);  
  mfo.new_plus(row[6]);
  rasshet.new_plus(row[7]);
  naimbank.new_plus(row[2]);
  adresban.new_plus(row[4]);
  
  if(polen(row[10],&telefon,1,' ') != 0)
   telefon.new_plus(row[10]);
 }





VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи.....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дейсвительна до (д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Поставщик................"));//3
VV.VVOD_SPISOK_add_MD(gettext("По документу............."));//4
VV.VVOD_SPISOK_add_MD(gettext("Номер доверенности......."));//5

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(80);//2
VV.VVOD_SPISOK_add_data(255);//3
VV.VVOD_SPISOK_add_data(80);//4
VV.VVOD_SPISOK_add_data(32);//5

short d,m,g;
poltekdat(&d,&m,&g);
sprintf(strsql,"%d.%d.%d",d,m,g);
VV.VVOD_SPISOK_zapis_data(0,strsql);


naz:;
clear();

VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Выписка доверенности"));
if(fio.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(fio.ravno());
 

helstr(LINES-1,0,"F2/+",gettext("печать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);



VV.vramka(-1,-1,0);


for(;;)
 {


  attron(VV.VVOD_return_cs(iceb_CFS)); 


  VV.vprintw(N,stdscr);
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 2 && VV.data_ravno(N)[0] != '\0' ) //Табельный номер
     {
      sprintf(strsql,"select fio from Kartb where tabn=%s",VV.data_ravno(N));
      if(readkey(strsql,&row,&cur) == 1)
       {
        fio.new_plus(row[0]);
        N++;
        goto naz;
       }
       
     }
    if(N == 3 && VV.data_ravno(N)[0] != '\0' ) //Поставщик
     {
      if(isdigit(VV.data_ravno(N)[0]) != 0) //Значит символ число
       {
        sprintf(strsql,"select naikon from Kontragent where kodkon='%d'",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(3,row[0]);
          N++;
          goto naz;
         }

       }
       
     }

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    return;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    break;
   }
  if(K == FK3)
   {
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;


      case 0 :

        fio.new_plus("");
        tabn=0;
        if(dirtab1(&tabn,&fio,0) == 0)
         {
          VV.data_plus(2,tabn);
         }        

        break;

      case 1 :
        naim_kontr.new_plus("");
        kontr.new_plus("");
        if(vibrek("Kontragent",&kontr,&naim_kontr) == 0)
         {
          sprintf(strsql,"select pnaim from Kontragent where kodkon='%s'",kontr.ravno_filtr());
          if(readkey(strsql,&row1,&cur1) == 1)
           if(row1[0][0] != '\0')
            naim_kontr.new_plus(row1[0]);
          VV.data_plus(3,naim_kontr.ravno());
         }
        break;
     }
    goto naz;
   }

  if(K == FK5)
   {
    iceb_redfil("dok_dover.alx",0);
    goto naz;
   }

 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(2));
post.new_plus(VV.VVOD_SPISOK_return_data(3));
ndoc.new_plus(VV.VVOD_SPISOK_return_data(4));
nomer_dov.new_plus(VV.VVOD_SPISOK_return_data(5));

if(rsdat(&d1,&m1,&g1,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата выдачи доверенности !"));
  goto naz;
 }
if(rsdat(&d2,&m2,&g2,dat2.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата действительно до !"));
  goto naz;
 }

int nast_vert=25;

class iceb_tu_str bros("");

sprintf(strsql,"Настройка для принтера %s",nameprinter.ravno());
if(iceb_t_poldan(strsql,&bros,"dok_dover.alx") == 0)
 nast_vert=bros.ravno_atoi();

char mesqc[64];
char mesd[64];
mesc(m1,1,mesqc);
mesc(m2,1,mesd);

fio.new_plus("");
char seriq[16];
char nomer[32];
sprintf(strsql,"select * from Kartb where tabn=%s",tabnom.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найден табельный номер !"));
  goto naz;
 }
 else
 {
  fio.new_plus(row[1]);
  
  polen(row[12],seriq,sizeof(seriq),1,' ');
  polen(row[12],nomer,sizeof(nomer),2,' ');

  vid.new_plus(row[13]);
  dvd.new_plus(row[19]);  
 }
char imaf[64];

sprintf(imaf,"dov%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }	
startfil(ff);

char str[1024];  
//char str1[500];  

fprintf(ff,"\x1b\x6C%c",8); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм
//fprintf(ff,"\x1B\x45"); /*Включение режима выделенной печати*/

fprintf(ff,"                        IПH - %s  NCB - %s тел.%.*s",inn.ravno(),nsv.ravno(),iceb_tu_kolbait(15,telefon.ravno()),telefon.ravno());  

fprintf(ff,"\x1B\x33%c\n",nast_vert);   
fprintf(ff,"\n");

fprintf(ff,"  %s",naim.ravno());
fprintf(ff,"\x1B\x33%c\n",33);   

fprintf(ff,"\n");

if(iceb_tu_strlen(adres.ravno()) > 50)
 {
  fprintf(ff,"\x0F");  /*Ужатый режим*/
  fprintf(ff,"  %s",adres.ravno());  
  fprintf(ff,"\x12\n");  /*Нормальный режим печати*/
 }
else
 fprintf(ff,"  %s\n",adres.ravno());  

fprintf(ff,"%s%s\n","                       ",kod.ravno());  
fprintf(ff,"  %s\n",naim.ravno());
fprintf(ff,"\n");  

if(iceb_tu_strlen(adres.ravno()) > 50)
 {
  fprintf(ff,"\x0F");  //Ужатый режим
  fprintf(ff,"  %s",adres.ravno());  
  fprintf(ff,"\x12\n");  //Нормальный режим печати
 }
else
 fprintf(ff,"  %s",adres.ravno());    

fprintf(ff,"\x1B\x33%c\n",34); 
fprintf(ff,"         %s         %s\n",rasshet.ravno(),mfo.ravno());  

sprintf(str,"%s %s",naimbank.ravno(),adresban.ravno());    
fprintf(ff,"  %-*s %02d %-*s  %02d\n",iceb_tu_kolbait(57,str),str,d2,iceb_tu_kolbait(12,mesd),mesd,g2-2000);

fprintf(ff,"\x1B\x33%c\n",44);   
fprintf(ff,"\n");    
fprintf(ff,"\n");    
fprintf(ff,"\n");    
sprintf(str,"                                    %02d %-*s %02d",d1,iceb_tu_kolbait(13,mesqc),mesqc,g1-2000);
fprintf(ff,"%s\n",str);    
fprintf(ff,"\n");    
sprintf(str,"            %s",fio.ravno());
fprintf(ff,"%s",str);    
fprintf(ff,"\x1B\x33%c\n",40);
fprintf(ff,"\n");
sprintf(str,"                               Паcпорт");  
fprintf(ff,"%s",str);
fprintf(ff,"\x1B\x33%c\n",40);  
fprintf(ff,"\n");

rsdat(&d3,&m3,&g3,dvd.ravno(),2);

char naz_m[64];
memset(naz_m,'\0',sizeof(naz_m));

mesc(m3,1,naz_m);

fprintf(ff,"           %s           %s          %02d %s %04d\n",seriq,nomer,d3,naz_m,g3);

fprintf(ff,"\n");  
sprintf(str,"            %s",vid.ravno());
fprintf(ff,"%s\n",str); 
fprintf(ff,"\x1B\x33%c\n",30);    
sprintf(str,"                    %s",post.ravno());
fprintf(ff,"%s\n",str);   
fprintf(ff,"\n"); 
//  fprintf(ff,"\x1B\x33%c\n",28);    
sprintf(str,"                 %s",ndoc.ravno());
fprintf(ff,"%s\n",str);     

fprintf(ff,"\x1B\x48"); /*Выключение режима двойного удара*/

fclose(ff);

char imaf_l[64];

sprintf(imaf_l,"dovl%d.lst",getpid());
dok_dover_rl(nomer_dov.ravno(),d1,m1,g1,d2,m2,g2,d3,m3,g3,fio.ravno(),seriq,nomer,vid.ravno(),naim.ravno(),naimbank.ravno(),kod.ravno(),adres.ravno(),rasshet.ravno(),mfo.ravno(),post.ravno(),inn.ravno(),nsv.ravno(),telefon.ravno(),ndoc.ravno(),imaf_l);

iceb_t_ustpeh(imaf_l,1);

class spis_oth oth;
oth.spis_imaf.plus(imaf_l);
oth.spis_naim.plus(gettext("Доверенность"));
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Доверенность на бланке строгой отчётности"));
iceb_t_rabfil(&oth,"");



}
/************************/
/*Печать доверенности на листе бумаги*/
/***********************************/
void dok_dover_rl(const char *nomer_dov,
short dv,short mv,short gv,
short dd,short md,short gd,
short dvd,short mvd,short gvd,
const char *fio,
const char *seriq,
const char *nomer,
const char *vid,
const char *naim,
const char *naimbank,
const char *kod,
const char *adres,
const char *rasshet,
const char *mfo,
const char *post,
const char *inn,const char *nsv,const char *telefon,
const char *ndok,
const char *imaf)
{
char mesqc[32];
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='dok_dover_r.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки dok_dover_r.alx");
  iceb_t_soob(strsql);
  return;
 }



FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }	

//startfil(ff);
//fprintf(ff,"\x1b\x6C%c",6); /*Установка левого поля*/
//fprintf(ff,"\x1B\x4D"); /*12-знаков*/



fprintf(ff,"                        IПH - %s  NCB - %s тел.%.*s\n",inn,nsv,iceb_tu_kolbait(15,telefon),telefon);  

char bros[512];
class iceb_tu_str stroka("");
int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  stroka.new_plus(row_alx[0]);
  nomer_str++;
  switch(nomer_str)
   {
    case 1:
     iceb_tu_vstav(&stroka,naim,0,53,1);
     break;

    case 3:
     iceb_tu_vstav(&stroka,adres,0,46,1);

     if(strlen(adres) > 47)
      {
       fprintf(ff,"%s",stroka.ravno());
       stroka.new_plus("");
       for(int kk=47; kk < iceb_tu_strlen(adres); kk+=47)
        {
         fprintf(ff,"%s\n",iceb_tu_adrsimv(kk,adres));
        }     
       continue;
      }
     break;

    case 4:
     iceb_tu_vstav(&stroka,kod,28,53,1);
     break;

    case 5:
     iceb_tu_vstav(&stroka,naim,0,53,1);
     break;

    case 7:
     iceb_tu_vstav(&stroka,adres,0,46,1);
     if(strlen(adres) > 47)
      {
       fprintf(ff,"%s",stroka.ravno());
       stroka.new_plus("");
       for(int kk=47; kk < iceb_tu_strlen(adres); kk+=47)
        {
         fprintf(ff,"%s\n",iceb_tu_adrsimv(kk,adres));
        }     
       continue;
      }
     break;

    case 8:
     iceb_tu_vstav(&stroka,rasshet,8,28,1);
     iceb_tu_vstav(&stroka,mfo,33,41,1);
     break;

    case 9:
     iceb_tu_vstav(&stroka,naimbank,0,53,1);
     memset(mesqc,'\0',sizeof(mesqc));
     mesc(md,1,mesqc);
     sprintf(bros,"%02d %s %d %s",dd,mesqc,gd,gettext("г."));
     iceb_tu_vstav(&stroka,bros,67,90,1);
        
     break;

    case 13:
     iceb_tu_vstav(&stroka,nomer_dov,43,63,1);
     break;

    case 14:
     memset(mesqc,'\0',sizeof(mesqc));
     mesc(mv,1,mesqc);
     sprintf(bros,"%02d %s %d %s",dv,mesqc,gv,gettext("г."));
     iceb_tu_vstav(&stroka,bros,34,69,1);
        
     break;

    case 16:
     iceb_tu_vstav(&stroka,fio,7,91,1);
     break;

    case 20:
     iceb_tu_vstav(&stroka,seriq,6,16,1);
     iceb_tu_vstav(&stroka,nomer,19,36,1);
     sprintf(bros,"%02d.%02d.%d %s",dvd,mvd,gvd,gettext("г."));
     iceb_tu_vstav(&stroka,bros,41,91,1);

     break;


    case 22:
     iceb_tu_vstav(&stroka,vid,8,91,1);
     break;

    case 24:
     iceb_tu_vstav(&stroka,post,17,91,1);
     break;

    case 26:
     iceb_tu_vstav(&stroka,ndok,13,91,1);
     break;
   }
   
  fprintf(ff,"%s",stroka.ravno());
  
 }


fclose(ff);


}
