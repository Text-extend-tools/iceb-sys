/*$Id: zarsbka.c,v 5.4 2013/05/17 14:56:18 sasa Exp $*/
/*30.09.2014	10.12.2009	Белых А.И.	zarsbka.c
Автоматическая смена банка и картсчёта во всех карточках
*/
#include "buhg.h"


void zarsbka()
{

char strsql[512];
int K=0,N=0;

class VVOD VV(0);
VV.VVOD_SPISOK_add_data(64);//0
VV.VVOD_SPISOK_add_data(64);//1
VV.VVOD_SPISOK_add_data(112);//1

VV.VVOD_SPISOK_add_ZAG(gettext("Автоматическая смена кода банка в карточках"));

VV.VVOD_SPISOK_add_MD(gettext("Код банка который нужно сменить....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код банка на который нужно сменить.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.....(,,).........."));//2


naz:;

clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2",gettext("сменить"),
"F3",gettext("подразделения"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);



class iceb_tu_str kod("");
class iceb_tu_str naim("");


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  K=getch();
  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   goto naz;
  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    if(N == 0)      /*Код банка*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      class iceb_tu_str kod_b("");
      class iceb_tu_str naimb("");
      
      if(dirsb(2,&kod_b,&naimb) == 0)
       {
        sprintf(strsql,"%s %s",kod_b.ravno(),naimb.ravno());
        VV.data_plus(N,strsql);
       }
      goto naz;
     }  

    if(N == 1)      /*Код банка*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      class iceb_tu_str kod_b("");
      class iceb_tu_str naimb("");
      
      if(dirsb(2,&kod_b,&naimb) == 0)
       {
        sprintf(strsql,"%s %s",kod_b.ravno(),naimb.ravno());
        VV.data_plus(N,strsql);
       }
      goto naz;
     }  

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));


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

  switch (K)
   {
    case ESC :
#ifndef M_UNIX
      K=getch();
#endif
      K=ESC;

    case FK10:
    case TAB:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      return;

    case FK2:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      goto vpered;

    case FK3:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

       if(dirzarpodr(2,&kod,&naim) == 0)
        VV.data_z_plus(2,kod.ravno());

      goto naz;

   } 
 }

vpered:;

int kodbanka_s=0;
int kodbanka_n=0;
int kolstr=0;
int kolstr1=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

polen(VV.data_ravno(0),&kodbanka_s,1,' ');

polen(VV.data_ravno(1),&kodbanka_n,1,' ');
sprintf(strsql,"select tabn,podr from Kartb where kb=%d",kodbanka_s);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(VV.data_ravno(2),row[1],0,0) != 0)
   continue;

  /*Проверяем есть ли код нового банка у работника в списке его счетов*/
  sprintf(strsql,"select nks from Zarkh where tn=%s and kb=%d",row[0],kodbanka_n);
  if(readkey(strsql,&row1,&cur1) <= 0)
   continue;  
  sprintf(strsql,"update Kartb set \
bankshet='%s',\
kb=%d \
where tabn=%s",
  row1[0],kodbanka_n,row[0]);

  sql_zapis(strsql,1,0);  

 }

}
