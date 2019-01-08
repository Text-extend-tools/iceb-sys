/*$Id: dirindm.c,v 5.14 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	10.02.2009	Белых А.И.	dirindm.c
Ввод и корректировка списка драгоценных металлов в карточку инвентаргого номера
*/
#include        <errno.h>
#include        "buhg.h"

void  dirindm_kod(int,const char *kods);
void	dirindm_ras(int,SQLCURSOR *cur);
void dirindm_sap(int);
void dirindm_vvod(int,const char *kodm);
int   dirindm_zap(int,VVOD *VV,const char *kodm);





int dirindm(int inv_nom)
{
char		strsql[1024];
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str kodvib("");
class iceb_tu_str naim_dm;
class SQLCURSOR cur1;
SQL_str row1;

SQLCURSOR curr;

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select * from Uosindm where innom=%d",inv_nom);


class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirindm_sap(inv_nom);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;



cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  naim_dm.new_plus("");
  sprintf(strsql,"select naik from Uosdm where kod=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_dm.new_plus(row1[0]);

  
  sprintf(strsql,"%-3s|%-*.*s|%-*s|%10.10g",row[1],
  iceb_tu_kolbait(30,naim_dm.ravno()),iceb_tu_kolbait(30,naim_dm.ravno()),naim_dm.ravno(),
  iceb_tu_kolbait(5,row[2]),row[2],
  atof(row[3]));

  i++;

  spis_menu.spis_str.plus(strsql);
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirindm_sap(inv_nom);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-21);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
/*  
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&kodvib,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("uos3_1.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirindm_vvod(inv_nom,kodvib.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirindm_vvod(inv_nom,"");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Uosindm where innom=%d and kod=%s",inv_nom,kodvib.ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK5:    //Распечатать
    dirindm_ras(inv_nom,&cur);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirindm_vvod(int innom,const char *kodm)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class iceb_tu_str naim_met("");

SQLCURSOR cur;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2


if(kodm[0] != '\0')
 {

  sprintf(strsql,"select * from Uosindm where innom=%d and kod=%d",innom,atoi(kodm));
  if(readkey(strsql,&row,&cur) != 1)
   {
    mvprintw(LINES-2,0,"%s",strsql);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
        
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[3]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);

  ktozap=atoi(row[4]);
  vremz=atol(row[5]);
  sprintf(strsql,"select naik from Uosdm where kod=%d",atoi(row[1]));
  if(readkey(strsql,&row,&cur) == 1)
   naim_met.new_plus(row[0]);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код драг-металла........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Вес....................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Единицa измерения......."));//2


naz:;

VV.VVOD_delete_ZAG();
if(kodm[0] == '\0')
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
if(naim_met.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naim_met.ravno());
 

kzvz(ktozap,vremz,&VV);

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
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
    goto naz;
   }


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      sprintf(strsql,"select kod,naik,ei from Uosdm where kod='%s'",VV.data_ravno(N));
      if(readkey(strsql,&row,&cur) <= 0)
       {
        sprintf(strsql,"%s %s!",gettext("Не найден код материалла"),VV.data_ravno(N));
        iceb_t_soob(strsql);
       }
      else
       {
        if(VV.data_ravno(2)[0] == '\0')
         VV.data_plus(2,row[2]);
        naim_met.new_plus(row[1]);
       }
      N++;
      goto naz;
     }

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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirindm_zap(innom,&VV,kodm) != 0)
     goto naz;
    break;
   }

  if(K == FK3) //Просмотр единиц измерения
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    VVOD MENU(3);

//    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка единиц измерения"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    class iceb_tu_str kod("");
    class iceb_tu_str naim("");
    switch (kom1)
     {
      case  2 :
      case -1 :
       goto naz;

      case  0 :
        if(dirdm(1,&kod,&naim) == 0)
         {
          VV.data_plus(0,kod.ravno());
         }
        break;

      case  1 :
        if(vibrek("Edizmer",&kod) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(2,kod.ravno());
         } 
        break;


     }

    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirindm_zap(int inv_nom,VVOD *VV,const char *kodm)
{
char	strsql[512];
//SQL_str row;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0'  ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       
/*проверяем код материалла*/
sprintf(strsql,"select kod from Uosdm where kod=%d",VV->data_ravno_atoi(0));
if(readkey(strsql) <= 0)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код материалла"),VV->data_ravno_atoi(0));
  iceb_t_soob(strsql);
  return(1);
 }

/*проверяем единицу измерения*/
if(VV->data_ravno(2)[0] != '\0')
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",VV->data_ravno(2));
  if(readkey(strsql) <= 0)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена единица измерения"),VV->data_ravno(2));
    iceb_t_soob(strsql);
    return(1);
   }
 }

if(SRAV(kodm,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kod from Uosindm where innom=%d and kod=%s",inv_nom,VV->VVOD_SPISOK_return_data(0));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }

  if(kodm[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }

time_t vrem;
time(&vrem);

if(kodm[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uosindm values(%d,%d,'%s',%10.10g,%d,%ld)",
  inv_nom,
  VV->data_ravno_atoi(0),  
  VV->data_ravno(2),  
  VV->data_ravno_atof(1),
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"replace into Uosindm values(%d,%d,'%s',%10.10g,%d,%ld)",
  inv_nom,
  VV->data_ravno_atoi(0),  
  VV->data_ravno(2),  
  VV->data_ravno_atof(1),
  iceb_t_getuid(),vrem);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kodm[0] != '\0' && SRAV(kodm,VV->VVOD_SPISOK_return_data(0),0) != 0)
 dirindm_kod(inv_nom,kodm);


return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void dirindm_sap(int innom)
{
short		y=2;
short           x=COLS-20;
class SQLCURSOR cur1;
SQL_str row1;
char strsql[512];
class iceb_tu_str naim_in("");
sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(readkey(strsql,&row1,&cur1) == 1)
 naim_in.new_plus(row1[0]);



clear();
printw(gettext("Ввод и корректировка драгоценных металлов"));
mvprintw(1,0,"%s:%d %s",gettext("Инвентарный номер"),innom,naim_in.ravno());
mvprintw(2,0,"   1                 2                 3       4");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("ед.измерения"));
mvprintw(y++,x,"4-%s",gettext("вес"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}

/********************/
/*Распечатать       */
/********************/

void	dirindm_ras(int inv_nom,SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;
class SQLCURSOR cur1;
SQL_str row1;

sprintf(imaf,"dmet%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка списка драгоценных металлов"),0,0,0,0,0,0,ff);

char strsql[512];
class iceb_tu_str naim_in("");
sprintf(strsql,"select naim from Uosin where innom=%d",inv_nom);
if(readkey(strsql,&row1,&cur1) == 1)
 naim_in.new_plus(row1[0]);
 
fprintf(ff,"%s:%d %s\n",gettext("Инвентарный номер"),inv_nom,naim_in.ravno());
fprintf(ff,"\
------------------------------------------------------------\n");
fprintf(ff,"\
Код |       Наименование           |Единица изм.|   Вес    |\n");
/*
1234 123456789012345678901234567890 123456789012 1234567890
*/

fprintf(ff,"\
------------------------------------------------------------\n");

cur->poz_cursor(0);
class iceb_tu_str naim_dm; 
while(cur->read_cursor(&row) != 0)
 {
  naim_dm.new_plus("");
  sprintf(strsql,"select naik from Uosdm where kod=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_dm.new_plus(row1[0]);
  fprintf(ff,"%-4s %-*.*s %-*s %10.10g\n",row[0],
  iceb_tu_kolbait(30,naim_dm.ravno()),iceb_tu_kolbait(30,naim_dm.ravno()),naim_dm.ravno(),
  iceb_tu_kolbait(12,row[2]),row[2],
  atof(row[3]));
 }
fprintf(ff,"\
------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка драгоценных металлов"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/**************************/
/*Корректировка кода      */
/**************************/
void  dirindm_kod(int inv_nom,const char *kods)
{
char		strsql[512];


//Удаляем запись которую откорректировали
sprintf(strsql,"delete from Uosindm where innom=%d and kod=%s",inv_nom,kods);
sql_zapis(strsql,1,0);

}
