/* $Id: vvodztab.c,v 5.34 2013/08/13 05:50:00 sasa Exp $ */
/*30.11.2016    25.05.2001      Белых А.И.      vvodztab.c
Ввод и корректировка записи табеля
*/

#include        "buhg.h"


int vvodztab(int metka, //0-ввод новой записи 1-корректировка старой
short mp,short gp,int tabnom,int kod_tab,int nom_zap) //Дата
{
char		strsql[1024];
int		K,N;
float		dnei,hasov;
float		has1;
double		bb;
class iceb_tu_str bros("");
short		maxkkd=0; /*Максимальное количество календарных дней*/
short		d,m,g;
SQL_str         row;
class iceb_tu_str naim("");
class iceb_tu_str fio("");
class iceb_tu_str stabn(""); /*Старый табельный номер*/
class iceb_tu_str skod("");  /*Старий код*/
short		dn,mn,gn,dk,mk,gk;
int		poz,komv;
class iceb_tu_str kod("");
float		hassm;
int		tabn=0;
int		kom1;
long		kolstr;
float		kof=1.;
double		okl=0.;
int metka_sp=0;
class iceb_tu_str snomz("");
short		dnp=0,mnp=0,gnp=0;
short		dkp=0,mkp=0,gkp=0;
class iceb_tu_str shet_do("");

SQLCURSOR cur;
has1=0.;

class VVOD MENU(3);
class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(6);
VV.VVOD_SPISOK_add_data(7);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(3);

m=mp; g=gp;
maxkkd=0;
dpm(&maxkkd,&m,&g,5);
redkal(mp,gp,&dnei,&hasov);
snomz.new_plus(VV.VVOD_SPISOK_return_data(10));

hassm=8;
if(metka == 0)
 {
  if(dnei != 0.)
   VV.data_plus(2,dnei);
  if(hasov != 0.)
   VV.data_plus(3,hasov);
  if(maxkkd != 0)
   VV.data_plus(4,maxkkd);
  sprintf(strsql,"select naik from Tabel where kod=1");
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    VV.data_plus(1,"1");
   }
 }

if(metka == 1)
 {

  okl=oklad(VV.data_ravno_atoi(0),&metka_sp);
  
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabnom);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fio.new_plus(row[0]);

  sprintf(strsql,"select naik from Tabel where kod=%d",kod_tab);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.new_plus(row[0]);

  sprintf(strsql,"select dnei,has,kdnei,datn,datk,ktoz,vrem,kolrd,kolrh,kom from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
  tabnom,gp,mp,kod_tab,nom_zap);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }

  VV.data_plus(0,tabnom);
  VV.data_plus(1,kod_tab);
  
  if(atof(row[0]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[0]));
    VV.data_plus(2,strsql);
   }
  if(atof(row[1]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[1]));
    VV.data_plus(3,strsql);
   }

  if(atof(row[2]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[2]));
    VV.data_plus(4,strsql);
   }

  if(atof(row[7]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[7]));
    VV.data_plus(5,strsql);
   }
  if(atof(row[8]) > 0.)
   {
    sprintf(strsql,"%.6g",atof(row[8]));
    VV.data_plus(6,strsql);
   }

  if(row[3][0] != '0')
    VV.data_plus(7,iceb_tu_datzap(row[3]));

  if(row[4][0] != '0')
    VV.data_plus(8,iceb_tu_datzap(row[4]));

  VV.data_plus(9,row[9]);
  VV.data_plus(10,nom_zap);
  kzvz(row[5],row[6],&VV);


  stabn.new_plus(tabnom);
  skod.new_plus(kod_tab);
  rsdat(&dnp,&mnp,&gnp,VV.VVOD_SPISOK_return_data(7),1);
  rsdat(&dkp,&mkp,&gkp,VV.VVOD_SPISOK_return_data(8),1);
 

 }

K=N=0;

sprintf(strsql,"%s:%d.%d%s %s:%.f %s:%.f %s:%d",
gettext("Дата"),mp,gp,gettext("г."),
gettext("Дней"),dnei,
gettext("Часов"),hasov,
gettext("Кал-рных дней"),maxkkd);

VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код табеля..................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Количество дней.............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Количество часов (ч:м)......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Количество календарных дней.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Колич. раб. дней в месяце...."));//5
VV.VVOD_SPISOK_add_MD(gettext("Колич. часов в рабочем дне..."));//6
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.................."));//7
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..................."));//8
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..................."));//9
VV.VVOD_SPISOK_add_MD(gettext("Номер записи................."));//10


naz:;
clear();

if(fio.ravno()[0] != '\0')
 mvprintw(1,0,"%s",fio.ravno());
if(naim.getdlinna() > 1)
 mvprintw(2,0,"%s",naim.ravno());
move(3,0);
if(okl != 0.)
  printw("%s:%.2f",gettext("Оклад"),okl);
/*****************
if(has1 != 0.)
  printw(" %s:%.2f",gettext("Часы"),has1);
if(kof != 1.)
  printw(" %s:%.2f",gettext("Ставка"),kof);
******************/
//Вывод списка табелей на экран

sprintf(strsql,"select kod,naik from Tabel order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int X=COLS-30;
int Y=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  mvprintw(Y++,X,"%2s %.27s",row[0],row[1]);
  if(Y >= LINES)
   break;
 }  



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(5,0,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

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

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Табельный номер*/
     {
      okl=oklad(VV.data_ravno_atoi(N),&metka_sp);
      if(SRAV(stabn.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        sprintf(strsql,"select fio from Kartb where tabn=%s",VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) == 1)
          fio.new_plus(row[0]);
        else
         {
          sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          goto naz;
         }
        /*Если не введено ни одной записи*/
        sprintf(strsql,"select tabn from Ztab where god=%d and mes=%d \
and tabn=%s",gp,mp,VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) > 0)
         {
          VV.data_plus(1,"");
          VV.data_plus(2,"");
          VV.data_plus(3,"");
          VV.data_plus(4,"");
          VV.data_plus(5,"");
          VV.data_plus(6,"");
         }
        N++;
        goto naz;
       }      
     }
   if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Код табеля*/
     {
      if(SRAV(skod.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        sprintf(strsql,"select naik from Tabel where kod=%s",VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) == 1)
          naim.new_plus(row[0]);
        else
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код табеля"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          goto naz;
         }
        N++;
        goto naz;
       }      
     }
    
    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Количество отработанных дней*/
     {
        if(has1 != 0.)
          bb=ATOFM(VV.VVOD_SPISOK_return_data(N))*has1*kof;
        else
         {
          if(ATOFM(VV.VVOD_SPISOK_return_data(N)) == dnei || hasov > 0.)
            bb=hasov;
          else   
            bb=ATOFM(VV.VVOD_SPISOK_return_data(N))*8;
         }
        bb=okrug(bb,0.01);
        VV.data_plus(3,bb);

      if(ATOFM(VV.VVOD_SPISOK_return_data(N)) != dnei && ATOFM(VV.VVOD_SPISOK_return_data(4)) == maxkkd)
         VV.data_plus(4,"");
                
      N++;
      goto naz;
     }
 
    if(N == 3) /*Количество отработанных часов*/
     {

      hasov=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(polen(VV.VVOD_SPISOK_return_data(N),&bros,2,':') == 0)
       {
        bb=(hasov*60+bros.ravno_atof())/60;
       }
      if(hasov != 0.)
       VV.data_plus(N,hasov);
      N++;
      goto naz;
     }

    if(N == 4) /*Количество отработанных календарных дней*/
     {
      d=(short)ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(d > maxkkd)
       {
        sprintf(strsql,gettext("В этом месяце календарных дней не может быть больше %d !"),maxkkd);
        iceb_t_soob(strsql);
        goto naz;
       }
     }
 
    if(N == 6 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Количество часов в смене*/
     {
      hassm=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(hassm != 0. && ATOFM(VV.VVOD_SPISOK_return_data(2)) != 0.)
       {
        hasov=hassm*ATOFM(VV.VVOD_SPISOK_return_data(2));
        VV.data_plus(3,hasov);
       }
      goto naz;
     }

    if(N == 7 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата начала периода
     {
      if(rsdat(&dnp,&mnp,&gnp,VV.VVOD_SPISOK_return_data(N),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала !"));
        goto naz;
       }
     }
    if(N == 8 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата конца периода
     {
      if(rsdat(&dkp,&mkp,&gkp,VV.VVOD_SPISOK_return_data(N),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата конца !"));
        goto naz;
       }
      if(dnp != 0 && dkp != 0)
       {
        sprintf(strsql,"%d",period(dnp,mnp,gnp,dkp,mkp,gkp,0));
        VV.VVOD_SPISOK_zapis_data(4,strsql);
        sprintf(strsql,"%d",krdvp(dnp,mnp,gnp,dkp,mkp,gkp));
        VV.VVOD_SPISOK_zapis_data(2,strsql);
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
    return(1);
   }

  if(K == FK1) /*Просмотр подсказки*/
   {  
    GDITE();
    iceb_t_pdoc("zarp2_6_1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(iceb_t_pbpds(mp,gp) != 0)
     {
       class VVOD DANET(1);
       DANET.VVOD_SPISOK_add_MD(gettext("Игнорировать блокировку?"));
       if(danet(&DANET,2,stdscr) == 2)
         goto naz;
     }

    //Проверяем блокировку карточки
    sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,atoi(VV.VVOD_SPISOK_return_data(0)));
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     if(atoi(row[0]) != 0)
      {
       sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
       iceb_t_soob(strsql);
       goto naz;
      }
    
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введены все обязательные реквизиты"));
      goto naz;
     }
    dn=mn=gn=dk=mk=gk=0;
    if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
     if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(7),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата начала !"));
       goto naz;
      }

    if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
     if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(8),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата конца !"));
       goto naz;
      }
 
    if(SRAV(stabn.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0 || SRAV(skod.ravno(),VV.VVOD_SPISOK_return_data(1),0) != 0)
     {
      /*Проверяем может такая запись уже введена*/
      sprintf(strsql,"select tabn from Ztab where god=%d and mes=%d \
and tabn=%s and kodt=%s and nomz=%d",
      gp,mp,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),atoi(VV.VVOD_SPISOK_return_data(10)));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        iceb_t_soob(gettext("Такая запись уже есть !"));
        goto naz;
       }         
     }
    if(zaptabel(metka,mp,gp,atol(VV.VVOD_SPISOK_return_data(0)),atoi(VV.VVOD_SPISOK_return_data(1)),atof(VV.VVOD_SPISOK_return_data(2)),\
    atof(VV.VVOD_SPISOK_return_data(3)),atof(VV.VVOD_SPISOK_return_data(4)),dn,mn,gn,dk,mk,gk,atof(VV.VVOD_SPISOK_return_data(5)),\
    atof(VV.VVOD_SPISOK_return_data(6)),VV.data_ravno_filtr(9),atoi(VV.VVOD_SPISOK_return_data(10)),snomz.ravno_atoi()) != 0)
       goto naz;
    return(0);
   }

  if(K == FK3) /*Просмотр списка видов табелей*/
   {  
    attroff(VV.VVOD_return_cs(iceb_CFS));
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов табеля"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
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

        kod.new_plus("");
        naim.new_plus("");
        poz=komv=0;
        if(dirmat1("Tabel",&kod,&naim,15,40,4,2,&poz,&komv,"","\0") == 0)
         {
          VV.data_plus(1,kod.ravno());
         }
        goto naz;

      case 1 :
        naim.new_plus("");
       
        if(dirtab(&tabn,&naim,0,1) == 0)
         {
          VV.data_plus(0,tabn);
         }
        goto naz;
     }
   }
  if(K == FK1) /*Просмотр подсказки*/
   {  
   }
 }
return(0);
}
