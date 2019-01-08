/*$Id: dirkv.c,v 5.1 2013/09/26 09:43:30 sasa Exp $*/
/*10.01.2018	03.09.2013	Белых А.И.	dirkv.c
Ввод и корректировка курса валют
*/
#include        <errno.h>
#include        "buhg.h"

void	dirkv_ras(SQLCURSOR *cur,const char *naim_val);
void dirkv_sap(const char *kodval,const char *naim_val);
void dirkv_vvod(const char *kod_val,const char *datvib);
int   dirkv_zap(VVOD *VV,const char *kod_val,const char *datvib);



int dirkv(const char *kod_val)
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
class iceb_tu_str koment("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str datvib("");
class SQLCURSOR curr;
class SQLCURSOR cur;
class iceb_tu_str naim_val("");

sprintf(strsql,"select naik from Glksval where kod='%s'",kod_val);
if(readkey(strsql,&row,&cur) == 1)
 naim_val.new_plus(row[0]);

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select datk,kurs,kom from Glkkv where kod='%s' order by datk desc",kod_val);



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirkv_sap(kod_val,naim_val.ravno());
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

//  if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
//   continue;
  
  sprintf(strsql,"%-*s|%10s|%s",
  iceb_tu_kolbait(4,row[0]),
  iceb_tu_datzap(row[0]),
  row[1],
  row[2]);
  
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

dirkv_sap(kod_val,naim_val.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));

kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-21);


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
//    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
//     continue;
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
//    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
//     continue;
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
//    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
//     continue;
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

  polen(str_vib.ravno(),&datvib,1,'|');

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
//    iceb_t_pdoc("uos5_10.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirkv_vvod(kod_val,datvib.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirkv_vvod(kod_val,"");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Glkkv where kod='%s' and datk='%s'",kod_val,datvib.ravno_sqldata());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  case FK5:    //Распечатать
    dirkv_ras(&cur,naim_val.ravno());
    goto naz;

  case FK6:    //Ввод норм расхода топлива
//    dirnrt(kodm);
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

void dirkv_vvod(const char *kodval,const char *datvib)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
SQLCURSOR curr;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(128);//2


if(datvib[0] != '\0')
 {

  sprintf(strsql,"select * from Glkkv where kod='%s' and datk='%s'",kodval,iceb_tu_tosqldat(datvib));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }

  VV.VVOD_SPISOK_zapis_data(0,iceb_tu_datzap(row[1]));
  VV.VVOD_SPISOK_zapis_data(1,row[2]);
  VV.VVOD_SPISOK_zapis_data(2,row[3]);

  ktozap=atoi(row[4]);
  vremz=atol(row[5]);

 }


VV.VVOD_SPISOK_add_MD(gettext("Дата........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Курс........"));//1
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.."));//2


naz:;

VV.VVOD_delete_ZAG();
if(kodval[0] == '\0')
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));


kzvz(ktozap,vremz,&VV);

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
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
    goto naz;


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
/*
    if(N == 1) 
     {

     }
*/
    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      if(SRAV(kodval,VV.data_ravno(N),0) != 0)
       {      
        sprintf(strsql,"select kod from Glksval where kod='%s'",VV.VVOD_SPISOK_return_data(N));
        if(readkey(strsql) > 0)
         {
          iceb_t_soob(gettext("Такая запись уже есть!"));
         }
       }
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
    if(dirkv_zap(&VV,kodval,datvib) != 0)
     goto naz;
    break;
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

int   dirkv_zap(VVOD *VV,const char *kodval,const char *datvib)
{
char	strsql[1024];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

if(VV->prov_dat(0,1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  return(1);
 }

if(SRAV(kodval,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kod from Glkkv where kod='%s' and datk='%s'",VV->data_ravno_filtr(0),iceb_tu_tosqldat(datvib));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }

 }


if(datvib[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Glkkv values('%s','%s',%.10g,'%s',%d,%ld)",
  kodval,  
  VV->ravno_sqldat(0),  
  VV->data_ravno_atof(1),
  VV->data_ravno_filtr(2),
  iceb_t_getuid(),time(NULL));
else
 {
  class iceb_tu_str datas(iceb_tu_tosqldat(datvib));
  
  sprintf(strsql,"update Glkkv set \
datk='%s',\
kurs=%.10g,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s' and datk='%s'",
  VV->ravno_sqldat(0),
  VV->data_ravno_atof(1),
  VV->data_ravno_filtr(2),
  iceb_t_getuid(),
  time(NULL),
  kodval,datas.ravno());
 }
 

if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void dirkv_sap(const char *kodval,const char *naim_val)
{
short		y=2;
short           x=COLS-20;


clear();
printw(gettext("Ввод и корректировка курсов валюты"));
printw("\n%s:%s %s",gettext("Валюта"),kodval,naim_val);

mvprintw(2,0,"      1          2      3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("курс"));
mvprintw(y++,x,"3-%s",gettext("коментарий"));


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}

/********************/
/*Распечатать       */
/********************/

void	dirkv_ras(SQLCURSOR *cur,const char *naim_val)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"kv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

//iceb_t_zagolov(gettext("Распечатка списка курсов валют"),0,0,0,0,0,0,ff);
iceb_t_zagolov(gettext("Распечатка списка курсов валют"),ff);
fprintf(ff,"%s:%s\n",gettext("Валюта"),naim_val);


fprintf(ff,"\
--------------------------------------------------\n");
fprintf(ff,"\
 Дата     |  Курс    |Коментарий\n");
fprintf(ff,"\
--------------------------------------------------\n");


cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  fprintf(ff,"%s %-10s %s\n",
  iceb_tu_datzap(row[0]),
  row[1],
  row[2]);
 }
fprintf(ff,"\
--------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка курсов валют"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
