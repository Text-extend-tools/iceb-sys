/* $Id: vos.c,v 5.63 2014/02/28 05:13:48 sasa Exp $ */
/*16.05.2017    20.05.1996      Белых А.И.      vos.c
Ввод основных средств
Если вернули 0 значит сделали запись
	     1 - нет
*/
#include        "buhg.h"
#include        "uos.h"
#include "poiinpd.h"
#include "bsizt.h"

#define		NOMM 3

void uoskar(int in,double bs,double iz);
int uosoz6(int innom);
int poiskin(VVOD *VV,long in,int podd,short d,short m,short g,short *m1,short *g1,class bsizw_data *bal_st);
int             vos_prshet(class iceb_tu_str *shh,short);
void		rasnas(long);
int dirindm(int);
void diruosls(int innom);
int vos_ras(int innom,short d,short m,short g);
void uosavve(int invnom);
int uosik818(int inv_nom);

void rasdopuos(short d,short m,short g,int);
int uosdi(char mr,int md,int innom,const char *imaf);

extern char	*imabaz;
extern short	startgoduos;
extern class UOS uos;

int vos(VVOD *VV,
long innom, //Инвентарный номер
short metka, //1-ввод нового 2-корректировка
short d,short m,short g, //Дата
int podr, //Подразделение
int kodotl) //Код ответственного лица
{
char            bros[512];
short           m1,g1;   /*Дата*/
short		mz,gz;
int             K;
int             N;
class bsizw_data bal_st;
short           mst; /*Метка следующей строки*/
int             podd=0; /*Код подразеделения*/
int             podd1=0;
long            inn;
int             i,i1;
class iceb_tu_str kod(""); /*Код справочной информации*/
char            zkr;    /*запрет корректировки*/
SQL_str         row;
char		strsql[2048];
int		kom1;
class iceb_tu_str vstr("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR curr;
VVOD VVOD1(2);
VVOD MENU(3);

//printw("\nvos- in=%ld metka=%d %d.%d.%d podr=%d\n",innom,metka,d,m,g,podr);
//refresh();

VV->VVOD_SPISOK_add_data(80);//0
VV->VVOD_SPISOK_add_data(80);//1
VV->VVOD_SPISOK_add_data(60);//2
VV->VVOD_SPISOK_add_data(60);//3
VV->VVOD_SPISOK_add_data(5); //4
VV->VVOD_SPISOK_add_data(80);//5
VV->VVOD_SPISOK_add_data(11);//6
VV->VVOD_SPISOK_add_data(80);//7
VV->VVOD_SPISOK_add_data(80);//8
VV->VVOD_SPISOK_add_data(80);//9
VV->VVOD_SPISOK_add_data(80);//10
VV->VVOD_SPISOK_add_data(20);//11
VV->VVOD_SPISOK_add_data(80);//12
VV->VVOD_SPISOK_add_data(20);//13
VV->VVOD_SPISOK_add_data(50);//14
VV->VVOD_SPISOK_add_data(20);//15
VV->VVOD_SPISOK_add_data(20);//16
VV->VVOD_SPISOK_add_data(20);//17
VV->VVOD_SPISOK_add_data(20);//18


VV->VVOD_SPISOK_add_MD(gettext("Наименование................."));//0
VV->VVOD_SPISOK_add_MD(gettext("Завод изготовитель..........."));//1
VV->VVOD_SPISOK_add_MD(gettext("Паспорт/чертеж N............."));//2
VV->VVOD_SPISOK_add_MD(gettext("Модель/тип/марка............."));//3
VV->VVOD_SPISOK_add_MD(gettext("Год выпуска.................."));//4
VV->VVOD_SPISOK_add_MD(gettext("Заводской номер.............."));//5
VV->VVOD_SPISOK_add_MD(gettext("Дата ввода в эксплуатацию...."));//6
VV->VVOD_SPISOK_add_MD(gettext("Счёт учета..................."));//7
VV->VVOD_SPISOK_add_MD(gettext("Шифр затрат амортотчислений.."));//8
VV->VVOD_SPISOK_add_MD(gettext("Шифр аналитического учета...."));//9
VV->VVOD_SPISOK_add_MD(gettext("Шифр н-мы амор-ний (нал.уч).."));//10
VV->VVOD_SPISOK_add_MD(gettext("Поправочный коэффициент(н/б)."));//11
VV->VVOD_SPISOK_add_MD(gettext("Состояние объекта............"));//12
VV->VVOD_SPISOK_add_MD(gettext("Номерной знак................"));//13
VV->VVOD_SPISOK_add_MD(gettext("Шифр н-мы амор-ний (бух.уч).."));//14
if(metka == 1)
 {
  VV->VVOD_SPISOK_add_MD(gettext("Балансовая стоимость,нал.уч.."));//15
  VV->VVOD_SPISOK_add_MD(gettext("Износ,налоговый учет........."));//16
  VV->VVOD_SPISOK_add_MD(gettext("Балансовая стоимость,бух.уч.."));//17
  VV->VVOD_SPISOK_add_MD(gettext("Износ, бухгалерский учет....."));//18
 }

naz1:;

podd=podr;
m1=m; g1=g;
zkr=N=0;
//bs=iz=0.;
VV->VVOD_clear_data();
if((i=poiskin(VV,innom,podd,d,m,g,&m1,&g1,&bal_st)) != 0)
 {
  if(i == 3)
   return(1);
  if(i == 1)
   {
    zkr=1;
    goto naz;
   }

  if(i == 2)
   {
    if(metka == 1)
     {
      uos.popkf=1.;
      VV->VVOD_SPISOK_zapis_data(11,"1/1");
      VV->VVOD_SPISOK_zapis_data(12,gettext("Бух.учёт + Налоговый учёт +"));
     }
    if(metka == 2)
     {
      iceb_t_soob(gettext("Такого инвентарного номера нет !"));
      return(1);
     }
   }
 }


naz:;

clear();
if(metka == 2)
 {
  move(18,0);
  printw("%30s %*s  %s","",iceb_tu_kolbait(22,gettext("Налоговый учёт")),gettext("Налоговый учёт"),gettext("Бухгалтерский учёт"));

  move(19,0);

  sprintf(bros,"%s 1.%d",gettext("Балансовая стоимость на"),startgoduos);
  printw("%*s:%10.2f %10.2f %10.2f %10.2f\n",iceb_tu_kolbait(31,bros),bros,bal_st.sbs,bal_st.siz,bal_st.sbsby,bal_st.sizby);
    
  printw("%*s:%10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(31,gettext("Изменение за период")),gettext("Изменение за период"),bal_st.bs,bal_st.iz,bal_st.bsby,bal_st.izby);

  printw("%*s:%10s %10.2f %10s %10.2f\n",
  iceb_tu_kolbait(31,gettext("Амортизация")),gettext("Амортизация")," ",bal_st.iz1," ",bal_st.iz1by);

  sprintf(bros,"%s %02d.%d",gettext("Балансовая стоимость на"),m,g);

  printw("%*s:%10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(31,bros),bros,bal_st.sbs+bal_st.bs,bal_st.siz+bal_st.iz+bal_st.iz1,bal_st.sbsby+bal_st.bsby,bal_st.sizby+bal_st.izby+bal_st.iz1by);

  printw("%*s %10.2f %10s %10.2f",
  iceb_tu_kolbait(42,gettext("Остаток")),gettext("Остаток"),
  (bal_st.sbs+bal_st.bs)-(bal_st.siz+bal_st.iz+bal_st.iz1),
  " ",
  (bal_st.sbsby+bal_st.bsby)-(bal_st.sizby+bal_st.izby+bal_st.iz1by));

 }


move(0,0);
printw("%s N%ld %s:%d %s:%d %s:",
gettext("Инв-ный"),innom,
gettext("Подр."),podd,
gettext("К.о.л."),kodotl,
gettext("Дата зап."));
if(m1 != mt  || g1 != gt)
 {
  attron(A_BLINK);  /*Включить мерцание*/
 }

printw("%d.%d%s",m1,g1,gettext("г."));
attroff(A_BLINK);  /*Выключить мерцание*/

if(metka == 1)   /*Вод нового ин. номера*/
 helstr(LINES-1,0,"F1",gettext("помощь"),
 "F2/+",gettext("запись"),
 "F3",gettext("копировать"),
 "F4",gettext("просмотр"),
 "F10",gettext("выход"),NULL);
if(metka == 2)   /*Работа со старым */
 helstr(LINES-1,0,"F1",gettext("помощь"),
 "F2/+",gettext("запись"),
 "F4",gettext("просмотр"),
 "F5",gettext("печать"),
 "F6",gettext("драг.мет."),
 "F7",gettext("ликв.стоим."),
 "F10",gettext("выход"),NULL);



VV->vramka(1,0,0);


for(;;)
 {
  attron(VV->VVOD_return_cs(iceb_CFS)); 

  VV->vprintw(N,stdscr);
  
  K=getch();
  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  VV->vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   goto naz;
   
  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV->PY,VV->PX,VV->MDLS,VV->KLST,stdscr);

  if(N == 10 && K == TAB) //Обнуляем стороку если нужно
   {
    VV->VVOD_SPISOK_zapis_data(N,"");
    goto naz;
   }
  if(N == 14 && K == TAB)
   {
    VV->VVOD_SPISOK_zapis_data(N,"");
    goto naz;
   }
     
  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));
    attron(VV->VVOD_return_cs(iceb_CFV));
    VV->vprintw_clear(N,stdscr);

    mst=0;

    if(N == 8)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naik("");
      if((i1=vvodkod("Uoshz",&kod,&naik,-1,-1,0)) == 0)
        goto naz;
      
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV->VVOD_SPISOK_zapis_data(N,strsql);
      N++;
      goto naz;
     }

    if(N == 9)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naik("");
      if((i1=vvodkod("Uoshau",&kod,&naik,-1,-1,0)) == 0)
        goto naz;
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV->VVOD_SPISOK_zapis_data(N,strsql);
      N++;
      goto naz;
     }

    if(N == 10)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naik("");
      if((i1=vvodkod("Uosgrup",&kod,&naik,-1,-1,0)) == 0)
        goto naz;
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV->VVOD_SPISOK_zapis_data(N,strsql);
      N++;
      goto naz;
     }

    if(N == 12)   /*Состояние объекта*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));

      VVOD MENU2(3);

      MENU2.VVOD_SPISOK_add_MD(gettext("Амортизация считается для бух.учета и налогового учёта"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Амортизация не считается для бух.учета и налогового учёта"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Амортизация считается для бух.учета и не считается для налогового учёта"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Амортизация не считается для бух.учета и считается для налогового учёта"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      kom1=0;     
      while(menu3w(stdscr,&MENU2,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case -1:
        case 4:
          break;

        case 0:
          VV->VVOD_SPISOK_zapis_data(N,gettext("Бух.учёт + Налоговый учёт +"));
          uos.soso=0;
          break;                      

        case 1:
          VV->VVOD_SPISOK_zapis_data(N,gettext("Бух.учёт - Налоговый учёт -"));
          uos.soso=1;
          break;                      

        case 2:
          VV->VVOD_SPISOK_zapis_data(N,gettext("Бух.учёт + Налоговый учёт -"));
          uos.soso=2;
          break;                      

        case 3:
          VV->VVOD_SPISOK_zapis_data(N,gettext("Бух.учёт - Налоговый учёт +"));
          uos.soso=3;
          break;                      
       }

      goto vp;
     }

    if(N == 14)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naik("");
      if((i1=vvodkod("Uosgrup1",&kod,&naik,-1,-1,0)) == 0)
       {
        goto naz;
       }
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV->VVOD_SPISOK_zapis_data(N,strsql);
      if(metka == 1)
        N++;
      goto naz;
     }

    VV->vgetstr(N,stdscr);
    
    if(N == 4)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if((uos.godv=(short)ATOFM(VV->VVOD_SPISOK_return_data(4))) <= 1900)
       {
        iceb_t_soob(gettext("Не верно введён год выпуска !"));
	goto naz;
       }
     }

    if(N == 6)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(rsdat(&uos.dv,&uos.mv,&uos.gv,VV->VVOD_SPISOK_return_data(6),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала эксплуатации !"));
	goto naz;
       }

     }
    if(N == 7)
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      class iceb_tu_str provshet(VV->data_ravno(7));
      
      if(vos_prshet(&provshet,1) != 0)
       {
	VV->data_plus(N,"");
	goto naz;
       }
      else
       VV->data_plus(7,provshet.ravno());
     }
vp:;
    attroff(VV->VVOD_return_cs(iceb_CFV));
    attron(VV->VVOD_return_cs(iceb_CFM));
    VV->vprintw(N,stdscr);

    if(mst == 0)
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
  if(N > VV->KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV->KLST-1;
   }

  switch (K)
   {
    case ESC : /*Выход*/
    case FK10 : /*Выход*/
      attroff(VV->VVOD_return_cs(iceb_CFS));
      attron(VV->VVOD_return_cs(iceb_CFM));
      return(1);
      break;

    case FK1:   /*Помощь*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     GDITE();
     iceb_t_pdoc("uos3.txt");
     clear();
     goto naz;

   case FK2: /*Запись*/
   case PLU: /*Запись*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     if(iceb_t_pbpds(m,g) != 0)
       goto naz;

     if(zkr == 1)
      {
       iceb_t_soob(gettext("Коректировка не возможна !"));
       goto naz;
      }

     if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введено наименование !"));
       goto naz;
      }

     uos.naim.new_plus(VV->VVOD_SPISOK_return_data(0));
     
     uos.zaviz.new_plus(VV->VVOD_SPISOK_return_data(1));

     uos.pasp.new_plus(VV->VVOD_SPISOK_return_data(2));

     uos.model.new_plus(VV->VVOD_SPISOK_return_data(3));

     if((uos.godv=(short)ATOFM(VV->VVOD_SPISOK_return_data(4))) <= 1900)
      {
       iceb_t_soob(gettext("Не верно введён год выпуска !"));
       goto naz;
      }


     uos.zavnom.new_plus(VV->VVOD_SPISOK_return_data(5));

     if(rsdat(&uos.dv,&uos.mv,&uos.gv,VV->VVOD_SPISOK_return_data(6),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата начала эксплуатации !"));
       goto naz;
      }
     polen(VV->VVOD_SPISOK_return_data(7),&uos.shetu,1,'|');

     polen(VV->VVOD_SPISOK_return_data(8),&uos.hzt,1,'|');

     polen(VV->VVOD_SPISOK_return_data(9),&uos.hau,1,'|');

     polen(VV->VVOD_SPISOK_return_data(10),&uos.hna,1,'|');
     polen(VV->VVOD_SPISOK_return_data(14),&uos.hnaby,1,'|');

     uos.popkf=(float)ATOFM(VV->VVOD_SPISOK_return_data(11));
     polen(VV->VVOD_SPISOK_return_data(11),bros,sizeof(bros),2,'/');
     uos.popkfby=(float)ATOFM(bros);
     
     if(uos.popkf == (float)0. || uos.popkfby == (float)0. )
      {
       iceb_t_soob(gettext("Поправочный коэфициент не может быть равным нолю !"));
       goto naz;
      }

     uos.nomz.new_plus(VV->VVOD_SPISOK_return_data(13));

     if(metka == 1) /*Ввод новой записи*/
      {
       sprintf(strsql,"select innom from Uosin where innom=%ld",innom);
       if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
        {
         iceb_t_soob(gettext("Такой инвентарный номер уже есть !"));
         goto naz;
        }       
       
       
       sprintf(strsql,"insert into Uosin \
values (%ld,%d,'%s','%s','%s','%s','%s','%d-%02d-%02d',%d,%ld)",
       innom,uos.godv,uos.naim.ravno_filtr(),uos.zaviz.ravno_filtr(),uos.pasp.ravno_filtr(),uos.model.ravno_filtr(),uos.zavnom.ravno_filtr(),uos.gv,uos.mv,uos.dv,iceb_t_getuid(),time(NULL));
       if(sql_zap(&bd,strsql) != 0)
        {
          msql_error(&bd,"1-Ошибка ввода записи !",strsql);
          goto naz;
        }

       sprintf(strsql,"insert into Uosinp \
values (%ld,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
       innom,m,g,uos.shetu.ravno(),uos.hzt.ravno(),uos.hau.ravno(),uos.hna.ravno(),uos.popkf,uos.soso,
       uos.nomz.ravno(),iceb_t_getuid(),time(NULL),uos.hnaby.ravno(),uos.popkfby);

       if(sql_zap(&bd,strsql) != 0)
        {
         msql_error(&bd,"2-Ошибка ввода записи !",strsql);
         goto naz;
        }

      }

     if(metka == 2) /*Корректировка записи*/
      {

       sprintf(strsql,"select innom from Uosin where innom=%ld",innom);
       if(sql_readkey(&bd,strsql,&row,&curr) != 1)
        {
         iceb_t_soob(gettext("Не найден инвентарный номер !"));
         goto naz;
        }
        memset(bros,'\0',sizeof(bros));
        sprintf(bros,"%d.%d",mt,gt);
        vstr.new_plus(bros); 
datz:;
        helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
        
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату записи (м.г)"));

        if((kom1=vvod1(&vstr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
          goto naz;
        if(kom1 == ESC)
          goto naz;
        if(rsdat1(&mz,&gz,vstr.ravno()) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата !"));
          goto datz;
         }       
        GDITE();
        
       sprintf(strsql,"update Uosin \
set \
god=%d,\
naim='%s',\
zaviz='%s',\
pasp='%s',\
model='%s',\
zavnom='%s',\
datvv='%d-%02d-%02d',\
ktoz=%d,\
vrem=%ld \
where innom=%ld",uos.godv,uos.naim.ravno_filtr(),uos.zaviz.ravno_filtr(),uos.pasp.ravno_filtr(),uos.model.ravno_filtr(),uos.zavnom.ravno_filtr(),uos.gv,uos.mv,uos.dv,
       iceb_t_getuid(),time(NULL),innom);

       if(sql_zap(&bd,strsql) != 0)
        {
         msql_error(&bd,__FUNCTION__,strsql);
         goto naz;
        }

       /*Проверяем есть ли запись точно с такой датой
       если есть удаляем*/

       sprintf(strsql,"delete from Uosinp where innom=%ld and god=%d \
and mes=%d",innom,gz,mz);
       if(sql_zap(&bd,strsql) != 0)
        {
         msql_error(&bd,"Ошибка удаления записи !",strsql);
        }

       sprintf(strsql,"insert into Uosinp \
values (%ld,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
       innom,mz,gz,uos.shetu.ravno(),uos.hzt.ravno(),uos.hau.ravno(),uos.hna.ravno(),uos.popkf,uos.soso,
       uos.nomz.ravno(),iceb_t_getuid(),time(NULL),uos.hnaby.ravno(),uos.popkfby);

       if(sql_zap(&bd,strsql) != 0)
        {
         msql_error(&bd,__FUNCTION__,strsql);
         goto naz;
        }

       sprintf(strsql,"%s !",gettext("Информация записана"));
       iceb_t_soob(strsql);

       goto naz;
      }

     m1=mz;  g1=gz;

     if(metka == 2)
       goto naz;
     if(metka == 1)
       return(0);

   case FK4: /*Просмотры*/

     attroff(VV->VVOD_return_cs(iceb_CFS)); 

     MENU.VVOD_delete();
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр амортизации налогового учёта"));//0
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр амортизации бухгалтерского учёта"));//1
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка документов"));//2
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка изменений"));//3
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр дополнительной информации"));//4
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр НСИ"));//5
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр оборотной стороны формы NoОЗ-6"));//6
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр оборотной стороны карточки (приказ 818)"));//7
     MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//8

     clearstr(LINES-1,0);
     kom1=0;     
     while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

     switch (kom1)
      {
       case -1:
       case 8:
          goto naz;

       case 0:
          diramort(innom,podd,kodotl,uos.hzt.ravno(),uos.hna.ravno());

          bsiz(innom,podd,1,m,g,&bal_st,NULL);
          goto naz;

       case 1:
          diramortby(innom,podd,kodotl,uos.hzt.ravno(),uos.hnaby.ravno());

          bsiz(innom,podd,1,m,g,&bal_st,NULL);
          goto naz;

       case 2:
         uosrsd1(innom);
         goto naz;

       case 3:
         clear(); /*Очистить экран и закрасить фоновым цветом*/
         rasnas(innom);
         goto naz;

       case 4:              
         GDITE();

         if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
          {
           iceb_t_soob(gettext("Не введено наименование !"));
           goto naz;
          }
         
         //Установить флаг работы с карточкой
         sprintf(strsql,"u%s%ld",imabaz,innom);
         if(sql_flag(&bd,strsql,0,0) != 0)
          {
           iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
           goto naz;
          }

         sprintf(strsql,"kontr%d.tmp",getpid());
         uosdi('-',0,innom,strsql);
         vizred(strsql);
         GDITE();
         uosdi('+',0,innom,strsql);
         unlink(strsql);

         /*Снять флаг работы с карточкой*/
         sprintf(strsql,"u%s%ld",imabaz,innom);
         sql_flag(&bd,strsql,0,1);
         goto naz;


       case 5:

         nsiuos();
         goto naz;

       case 6:              
         GDITE();

         if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
          {
           iceb_t_soob(gettext("Не введено наименование !"));
           goto naz;
          }
         
         //Установить флаг работы с карточкой
         sprintf(strsql,"uu%s%ld",imabaz,innom);
         if(sql_flag(&bd,strsql,0,0) != 0)
          {
           iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
           goto naz;
          }

         sprintf(strsql,"uoskart%d.tmp",getpid());
         uosdi('-',1,innom,strsql);
         vizred(strsql);
         GDITE();
         uosdi('+',1,innom,strsql);
         unlink(strsql);

         /*Снять флаг работы с карточкой*/
         sprintf(strsql,"uu%s%ld",imabaz,innom);
         sql_flag(&bd,strsql,0,1);
         goto naz;

       case 7:              
         GDITE();

         if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
          {
           iceb_t_soob(gettext("Не введено наименование !"));
           goto naz;
          }
         
         //Установить флаг работы с карточкой
         sprintf(strsql,"ik%s%ld",imabaz,innom);
         if(sql_flag(&bd,strsql,0,0) != 0)
          {
           iceb_t_soob(gettext("С карточкой уже работает другой оператор !"));
           goto naz;
          }

         sprintf(strsql,"uoskart%d.tmp",getpid());
         uosdi('-',2,innom,strsql);
         vizred(strsql);
         GDITE();
         uosdi('+',2,innom,strsql);
         unlink(strsql);

         /*Снять флаг работы с карточкой*/
         sprintf(strsql,"ik%s%ld",imabaz,innom);
         sql_flag(&bd,strsql,0,1);
         goto naz;

       
      }
     break;
     

   case FK3: /*Взять настройку другого инвентарного номера*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     if(metka == 1)
      {
       helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
       
       bros[0]='\0';
       
       VVOD1.VVOD_delete();
       VVOD1.VVOD_SPISOK_add_MD(gettext("Введите инвентарный номер"));
       vstr.new_plus("");
       if((kom1=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
         goto naz;

       if(kom1 == ESC)
         goto naz;

       inn=vstr.ravno_atoi();
       /*strcpy(podd1,podd);*/
       podd1=0;
       if(poiskin(VV,inn,podd1,d,m,g,&m1,&g1,&bal_st) == 2)
	{
         sprintf(strsql,"%s !",gettext("Не найден инвентарный номер"));
         iceb_t_soob(strsql);
	 goto naz;
	}
       goto naz;
      }


     break;


   case FK5: /*Распечатать*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
 
     if(metka == 2)
      vos_ras(innom,d,m,g);

     goto naz;
     
   case FK6: /*Ввод драг.металлов */
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     if(metka == 2)
      {
       dirindm(innom);
       goto naz;
      }
     break;

   case FK7: /*Ввод драг.металлов */
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     if(metka == 2)
      {
       diruosls(innom);
       goto naz;
      }
     break;
     
   case PD: /*Листание даты на уменьшение*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     dpm(&d,&m,&g,4);
     goto naz1;

   case PU: /*Листание даты на увеличение*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     dpm(&d,&m,&g,3);
     goto naz1;


   default:
    break;
  }
 }
attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

}

/**************/
/*Проверка НСИ*/
/**************/
int prtl1(const char *tabl,const char *kodk,
class iceb_tu_str *naim,
short met) //0-без названия 1-с названием
{
char		strsql[1024];
SQL_str         row;
class iceb_tu_str kod("");
SQLCURSOR curr;
naim->new_plus("");

if(kodk[0] == '\0')
  return(0);
  
if(polen(kodk,&kod,1,'|') != 0)
  kod.new_plus(kodk);
  
if(SRAV(tabl,"Uosol",0) == 0)
  sprintf(strsql,"select naik from %s where kod=%s",tabl,kod.ravno());
else
  sprintf(strsql,"select naik from %s where kod='%s'",tabl,kod.ravno());

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {    
  VVOD SOOB(1);
  if(SRAV(tabl,"Uospod",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),kod.ravno());
  if(SRAV(tabl,"Uosgrup",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден шифр нормы отчислений"),kod.ravno());
  if(SRAV(tabl,"Uosgrup1",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден шифр нормы отчислений"),kod.ravno());
  if(SRAV(tabl,"Uoshau",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден шифра аналитического учета"),kod.ravno());
  if(SRAV(tabl,"Uoshz",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден шифр производственных затрат"),kod.ravno());
  if(SRAV(tabl,"Uosol",0) == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден код мат.-ответственного"),kod.ravno());

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

if(met == 1)
 {
//  sprintf(strsql,"%s|%.30s",kod,row[0]);
//  strncpy(kodk,strsql,dlinna);
  naim->new_plus(row[0]);
 }
return(0);
}

/****************/
/*Проверка счета*/
/****************/
int vos_prshet(class iceb_tu_str *shh,short met) //0-без названия 1-с названием
{
struct OPSHET shetv;
char		strsql[1024];
SQL_str         row;
class iceb_tu_str sh("");
SQLCURSOR curr;

if(shh->ravno()[0] == '\0')
 return(0);
 
if(polen(shh->ravno(),&sh,1,'|') != 0)
  sh.new_plus(shh->ravno());

if(prsh1(sh.ravno(),&shetv) != 0)
  return(1);


if(met == 1)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {    
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),sh.ravno());
    iceb_t_soob(strsql);
   }
  else
   {
//    sprintf(shh,"%s|%.*s",sh.ravno(),iceb_tu_kolbait(30,row[0]),row[0]);
    shh->new_plus(sh.ravno());
//    sprintf(strsql,"%.*s",iceb_tu_kolbait(60,row[0]),row[0]);
    shh->plus("|",row[0]);
   }
 }
return(0);
}

/***************************/
/*Поиск инвентарного номера*/
/***************************/
int poiskin(VVOD *VV,long in,int podd,short d,short m,short g,
short *m1,short *g1,
class bsizw_data *bal_st)
{
short           mm;
int		poddz;
char		strsql[1024];
SQL_str         row;
char		bros[512];
int		kodotl;
SQLCURSOR curr;
/*
printw("\npoiskin-%ld %d %d.%d.%d\n",in,podd,d,m,g);
OSTANOV();
*/

sprintf(strsql,"select * from Uosin where innom=%ld",in);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
  return(2);

uos.godv=atoi(row[1]);
uos.naim.new_plus(row[2]);
uos.zaviz.new_plus(row[3]);
uos.pasp.new_plus(row[4]);
uos.model.new_plus(row[5]);
uos.zavnom.new_plus(row[6]);
rsdat(&uos.dv,&uos.mv,&uos.gv,row[7],2);


/*Если подразделение не определено при поиске то сначала определяем его*/ 
poddz=0;
if(podd == 0)
 {
  poiin(in,d,m,g,&poddz,&kodotl);
  podd=poddz;
/*
  printw("poddz=%d\n",poddz);
  OSTANOV();
*/
 }
class poiinpdw_data rekin;
//if((mm=poiinpd(in,m,g,uos.shetu.ravno(),uos.hzt.ravno(),uos.hau.ravno(),uos.hna.ravno(),&uos.popkf,&uos.soso,uos.nomz.ravno(),m1,g1,uos.hnaby.ravno(),&uos.popkfby)) != 0)
if((mm=poiinpd(in,m,g,&rekin)) != 0)
 {
  if(mm == 3)
   {
    iceb_t_soob(gettext("Не найдено ни одной записи в таблице Uosinp !"));
    return(3);
   }


   sprintf(bros,"%s %d",gettext("Этот инвентарный номер в текущем месяце не числится !"),mm);
   if(mm == 1)
    {
     memset(bros,'\0',sizeof(bros));
     sprintf(bros,"%s %d",gettext("Числится позже текущей даты !"),mm);
    }
   if(mm == 2)
    {
     memset(bros,'\0',sizeof(bros));
     sprintf(bros,"%s-%s! %d",__FUNCTION__,gettext("Произошел расход"),mm);
    }

   iceb_t_soob(bros);
 }
else
 {
  uos.shetu.new_plus(rekin.shetu.ravno());
  uos.hzt.new_plus(rekin.hzt.ravno());
  uos.hau.new_plus(rekin.hau.ravno());
  uos.hna.new_plus(rekin.hna.ravno());
  uos.popkf=rekin.popkf;
  uos.soso=rekin.soso;
  uos.nomz.new_plus(rekin.nomz.ravno());
  uos.hnaby.new_plus(rekin.hnaby.ravno());
  uos.popkfby=rekin.popkfby;
 }
VV->VVOD_SPISOK_zapis_data(0,uos.naim.ravno());
VV->VVOD_SPISOK_zapis_data(1,uos.zaviz.ravno());
VV->VVOD_SPISOK_zapis_data(2,uos.pasp.ravno());
VV->VVOD_SPISOK_zapis_data(3,uos.model.ravno());
sprintf(strsql,"%d",uos.godv);
VV->VVOD_SPISOK_zapis_data(4,strsql);
VV->VVOD_SPISOK_zapis_data(5,uos.zavnom.ravno());
sprintf(strsql,"%d.%d.%d",uos.dv,uos.mv,uos.gv);
VV->VVOD_SPISOK_zapis_data(6,strsql);

sprintf(strsql,"select nais from Plansh where ns='%s'",uos.shetu.ravno());
memset(bros,'\0',sizeof(bros));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,gettext("Нет счета %s в плане счетов !"),uos.shetu.ravno());
  iceb_t_soob(strsql);
 }
else
  strncpy(bros,row[0],sizeof(bros));  

sprintf(strsql,"%s|%.*s",uos.shetu.ravno(),iceb_tu_kolbait(20,bros),bros);
VV->VVOD_SPISOK_zapis_data(7,strsql);

 /*printw("* us3.in-%ld us3.hzt-%s us3.hau-%s\n",us3.in,us3.hzt,us3.hau);*/
VV->VVOD_SPISOK_zapis_data(8,uos.hzt.ravno());
class iceb_tu_str naimr("");
if(prtl1("Uoshz",VV->VVOD_SPISOK_return_data(8),&naimr,1) == 0)
 {
  VV->data_strcat(8,"|");
  VV->data_strcat(8,naimr.ravno());
 }
VV->data_plus(9,uos.hau.ravno());
if(prtl1("Uoshau",VV->VVOD_SPISOK_return_data(9),&naimr,1) == 0)
 {
  VV->data_strcat(9,"|");
  VV->data_strcat(9,naimr.ravno());
 }

VV->data_plus(10,uos.hna.ravno());
if(prtl1("Uosgrup",VV->VVOD_SPISOK_return_data(10),&naimr,1) == 0)
 {
  VV->data_strcat(10,"|");
  VV->data_strcat(10,naimr.ravno());
 }
 

sprintf(strsql,"%.6g/%.6g",uos.popkf,uos.popkfby);
VV->VVOD_SPISOK_zapis_data(11,strsql);


if(uos.soso == 0)
  VV->VVOD_SPISOK_zapis_data(12,gettext("Бух.учёт + Налоговый учёт +"));

if(uos.soso == 1)
  VV->VVOD_SPISOK_zapis_data(12,gettext("Бух.учёт - Налоговый учёт -"));

if(uos.soso == 2)
  VV->VVOD_SPISOK_zapis_data(12,gettext("Бух.учёт + Налоговый учёт -"));

if(uos.soso == 3)
  VV->VVOD_SPISOK_zapis_data(12,gettext("Бух.учёт - Налоговый учёт +"));

VV->VVOD_SPISOK_zapis_data(13,uos.nomz.ravno());

VV->VVOD_SPISOK_zapis_data(14,uos.hnaby.ravno());
if(prtl1("Uosgrup1",VV->VVOD_SPISOK_return_data(14),&naimr,1) == 0)
 {
  VV->data_strcat(14,"|");
  VV->data_strcat(14,naimr.ravno());
 }

bsiz(in,podd,d,m,g,bal_st,NULL);

if( mm != 0)
   return(1);

return(0);
}
/*************/
/*распечатки*/
/************/

int vos_ras(int innom,short d,short m,short g)
{

int		kom;

VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Раcпечатки"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка типовой формы N03-6"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка карточки"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка акта ввода в эксплуатацию"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка инвентарной карточки (приказ 818)"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

naz:;

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 4 :
  case -1 :
    return(-1);

  case 0 :
  case 1:
  case 2:
  case 3:
    break;

  default:
    kom=0;
    goto naz;
 }

if(kom == 0)
 uosoz6(innom);
if(kom == 1)
 rasdopuos(d,m,g,innom);
if(kom == 2)
  uosavve(innom);

if(kom == 3)
  uosik818(innom);

return(0);
}
