/*$Id: udvhd.c,v 5.17 2014/08/31 06:18:18 sasa Exp $*/
/*15.03.2018	23.03.2009	Белых А.И.	udvhd.c
Ввод шапки документа для учёта доверенностей
*/
#include <ctype.h>
#include "buhg.h"

int udvhd_zap(class iceb_tu_str *datdov,class iceb_tu_str *ndov,class VVOD *VV);


int udvhd(class iceb_tu_str *datdov,class iceb_tu_str *ndov)
{
int N=0,K=0;
char strsql[512];
SQL_str row;
class SQLCURSOR	cur;

class VVOD VV(0);


VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(11);//2
VV.VVOD_SPISOK_add_data(80);//3
VV.VVOD_SPISOK_add_data(255);//4
VV.VVOD_SPISOK_add_data(11);//5
VV.VVOD_SPISOK_add_data(100);//6
VV.VVOD_SPISOK_add_data(100);//7
VV.VVOD_SPISOK_add_data(50);//8
VV.VVOD_SPISOK_add_data(20);//9
VV.VVOD_SPISOK_add_data(20);//10
VV.VVOD_SPISOK_add_data(11);//11
VV.VVOD_SPISOK_add_data(200);//12
VV.VVOD_SPISOK_add_data(50);//13


if(datdov->getdlinna() > 1) /*Корректировка шапки документа*/
 {
  sprintf(strsql,"select * from Uddok where datd='%s' and nomd='%s'",
  datdov->ravno_sqldata(),ndov->ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s",gettext("Не найден документ!"),datdov->ravno(),ndov->ravno());
    iceb_t_soob(strsql);
    return(1);
   }

  VV.data_plus(0,iceb_tu_datzap(row[0]));
  VV.data_plus(1,row[1]);
  VV.data_plus(2,iceb_tu_datzap(row[2]));
  VV.data_plus(3,row[3]);
  VV.data_plus(4,row[4]);
  VV.data_plus(8,row[5]);
  VV.data_plus(5,iceb_tu_datzap(row[7]));
  VV.data_plus(6,row[8]);
  VV.data_plus(7,row[6]);
  VV.data_plus(9,row[9]);
  VV.data_plus(10,row[10]);
  VV.data_plus(11,iceb_tu_datzap(row[11]));
  VV.data_plus(12,row[12]);
  VV.data_plus(13,row[13]);

  kzvz(row[14],row[15],&VV);

 }





VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи......(д.м.г)......."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер доверенности............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Действительна до..(д.м.г)......"));//2
VV.VVOD_SPISOK_add_MD(gettext("Кому видана доверенность......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Поставщик......................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Дата наряда......(д.м.г)......."));//5-6
VV.VVOD_SPISOK_add_MD(gettext("Номер наряда..................."));//6-7
VV.VVOD_SPISOK_add_MD(gettext("Отметка использования.........."));//7-8
VV.VVOD_SPISOK_add_MD(gettext("По документу..................."));//8-5
VV.VVOD_SPISOK_add_MD(gettext("Серия документа................"));//9
VV.VVOD_SPISOK_add_MD(gettext("Номер документа................"));//10
VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи документа (д.м.г).."));//11
VV.VVOD_SPISOK_add_MD(gettext("Кем выдан документ............."));//12
VV.VVOD_SPISOK_add_MD(gettext("Должность......................"));//13

naz:;

clear();

if(datdov->getdlinna() <= 1)
 printw("%s\n",gettext("Ввод нового документа"));

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("ном.док."),
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

    if(N == 0) /*Дата выдачи*/
     {
      if(VV.prov_dat(N,1) != 0)
       {
        iceb_t_soob(gettext("Не правильно введена дата!"));
        goto naz;
       }
      if(VV.data_ravno(1)[0] == '\0')
       {
        VV.data_plus(1,iceb_t_nomdok(VV.ravno_god(0),"Uddok"));
        N++;
        goto naz;        
       }      
    
     }

    if(N == 2) /*Действительна до*/
     {
      if(VV.prov_dat(N,1) != 0)
       {
        iceb_t_soob(gettext("Не правильно введена дата!"));
        goto naz;
       }
     }

    if(N == 3 && VV.data_ravno(N)[0] != '\0' ) /*Кому выдан документ*/
     {
      if(isdigit(VV.data_ravno(N)[0]) != 0) //Значит символ число
       {
        sprintf(strsql,"select fio,nomp,vidan,datvd,dolg from Kartb where tabn=%d",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          VV.data_plus(3,row[0]);

          polen(row[1],strsql,sizeof(strsql),1,' ');
          VV.data_plus(9,strsql);
          polen(row[1],strsql,sizeof(strsql),2,' ');
          VV.data_plus(10,strsql);
          VV.data_plus(12,row[2]);
          VV.data_plus(11,iceb_tu_datzap(row[3]));
          VV.data_plus(8,gettext("Паспорт"));
          VV.data_plus(13,row[4]);
          
          N++;
          goto naz;
         }

       }
       
     }

    if(N == 4 && VV.data_ravno(N)[0] != '\0' ) //Поставщик
     {
      if(isdigit(VV.data_ravno(N)[0]) != 0) //Значит символ число
       {
        sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%d'",VV.data_ravno_atoi(N));
        if(readkey(strsql,&row,&cur) == 1)
         {
          if(row[1][0] == '\0')
           VV.data_plus(4,row[0]);
          else
           VV.data_plus(4,row[1]);
          
          N++;
          goto naz;
         }

       }
       
     }

    if(N == 5) /*Дата наряда*/
     {
      if(VV.prov_dat(N,1) != 0)
       {
        iceb_t_soob(gettext("Не правильно введена дата!"));
        goto naz;
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
    return(1);
   }
  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    GDITE();
    iceb_t_pdoc("udov1_1.txt");
    goto naz;    
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(udvhd_zap(datdov,ndov,&VV) == 0)
     return(0);

   }
  if(K == FK3) /*РЕКВИЗИТЫ*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VVOD MENU(3);

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    class iceb_tu_str kod("");
    class iceb_tu_str naim("");
    int tabn=0;
    int podr=0;
    class iceb_tu_str fio("");
    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 0 :
        if(dirtab1(&tabn,&fio,podr) == 0)
         {
          sprintf(strsql,"select fio,nomp,vidan,datvd,dolg from Kartb where tabn=%d",tabn);
          if(readkey(strsql,&row,&cur) == 1)
           {
            VV.data_plus(3,row[0]);

            polen(row[1],strsql,sizeof(strsql),1,' ');
            VV.data_plus(9,strsql);
            polen(row[1],strsql,sizeof(strsql),2,' ');
            VV.data_plus(10,strsql);
            VV.data_plus(12,row[2]);
            VV.data_plus(11,iceb_tu_datzap(row[3]));
            VV.data_plus(8,gettext("Паспорт"));
            VV.data_plus(13,row[4]);
           }            
          VV.data_plus(3,fio.ravno());
         }
        break;

      case 1 :
        if(vibrek("Kontragent",&kod,&naim) == 0)
         {
          VV.data_plus(4,iceb_t_get_pnk(kod.ravno(),1));
                    
         }
        break;
     }
    goto naz;
   }
  if(K == FK4) /*Получить номер документа*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    GDITE();
    VV.data_plus(1,iceb_t_nomdok(VV.ravno_god(0),"Uddok"));
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
   
return(0);
}
/***********************/
/*Запись шапки документа*/
/***************************/
int udvhd_zap(class iceb_tu_str *datdov,class iceb_tu_str *ndov,class VVOD *VV)
{

if(iceb_t_pbpds(VV->data_ravno(0)) != 0)
 return(1);
if(iceb_t_pbpds(datdov->ravno()) != 0)
 return(1);


if(VV->prov_dat(0,0) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата выдачи доверенности!"));
  return(1);
 }
if(VV->prov_dat(2,0) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата \"Действительна до\"!"));
  return(1);
 }

if(VV->prov_dat(5,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата наряда!"));
  return(1);
 }
if(VV->prov_dat(11,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата выдачи документа!"));
  return(1);
 }

if(VV->data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели номер доверенности!"));
  return(1);
 }
if(VV->data_ravno(3)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели \"Кому выдана\"!"));
  return(1);
 }
if(VV->data_ravno(4)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели \"Поставщик\"!"));
  return(1);
 }
if(VV->data_ravno(8)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели \"По документа\"!"));
  return(1);
 }
char strsql[512];

class lock_tables KKK("LOCK TABLES Uddok WRITE,Uddok1 WRITE,icebuser READ");
short dd=0,md=0,gd=0;
rsdat(&dd,&md,&gd,VV->data_ravno(0),1);
if(SRAV(ndov->ravno(),VV->data_ravno(1),0) != 0 || gd != datdov->ravno_god())
 {
  /*Проверяем есть ли такой номер документа*/
  sprintf(strsql,"select nomd from Uddok where datd >= '%04d-01-01' and datd <= '%04d-12-31' \
  and nomd = '%s'",gd,gd,VV->data_ravno(1));
  if(readkey(strsql) > 0)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть!"),VV->data_ravno(1));    
    iceb_t_soob(strsql);
    return(1);
   }
 }

//5-6
//6-7
//7-8
//8-5

if(datdov->getdlinna() <= 1) /*Новый документ*/
 {
  sprintf(strsql,"insert into Uddok values('%s','%s','%s','%s','%s','%s','%s','%s','%s',\
'%s','%s','%s','%s','%s',%d,%ld)",
  VV->ravno_sqldat(0),
  VV->data_ravno_filtr(1),
  VV->ravno_sqldat(2),
  VV->data_ravno_filtr(3),
  VV->data_ravno_filtr(4),
  VV->data_ravno_filtr(8),
  VV->data_ravno_filtr(7),
  VV->ravno_sqldat(5),
  VV->data_ravno_filtr(6),
  VV->data_ravno_filtr(9),
  VV->data_ravno_filtr(10),
  VV->ravno_sqldat(11),
  VV->data_ravno_filtr(12),
  VV->data_ravno_filtr(13),
  iceb_t_getuid(),
  time(NULL));
        
 }
else /*Корректировка уже введённого документа*/
 {
  sprintf(strsql,"update Uddok set \
datd='%s',\
nomd='%s',\
datds='%s',\
komu='%s',\
post='%s',\
podok='%s',\
otis='%s',\
datn='%s',\
nomn='%s',\
seriq='%s',\
nomerd='%s',\
datavd='%s',\
vidan='%s',\
dolg='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%s' and nomd='%s'",
  VV->ravno_sqldat(0),
  VV->data_ravno_filtr(1),
  VV->ravno_sqldat(2),
  VV->data_ravno_filtr(3),
  VV->data_ravno_filtr(4),
  VV->data_ravno_filtr(8),
  VV->data_ravno_filtr(7),
  VV->ravno_sqldat(5),
  VV->data_ravno_filtr(6),
  VV->data_ravno_filtr(9),
  VV->data_ravno_filtr(10),
  VV->ravno_sqldat(11),
  VV->data_ravno_filtr(12),
  VV->data_ravno_filtr(13),
  iceb_t_getuid(),
  time(NULL),
  datdov->ravno_sqldata(),
  ndov->ravno_filtr());  
 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);

/*Корректируем записи в документах*/
if(sravmydat(datdov->ravno(),VV->data_ravno(0)) != 0 || SRAV(ndov->ravno(),VV->data_ravno(1),0) != 0)
 {
  sprintf(strsql,"update Uddok1 set datd='%s', nomd='%s' where datd='%s' and nomd='%s'",
  VV->ravno_sqldat(0),
  VV->data_ravno(1),
  datdov->ravno_sqldata(),
  ndov->ravno());

  sql_zapis(strsql,0,0); 
 }

datdov->new_plus(VV->data_ravno(0));
ndov->new_plus(VV->data_ravno(1)); 
return(0);
}
