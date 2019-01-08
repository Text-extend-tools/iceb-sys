/*$Id: dirdov.c,v 5.17 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	30.03.2009	Белых А.И.	dirdov.c
Просмотр записей в доверенности
*/
#include "buhg.h"

int dirdov_vvodz(class iceb_tu_str *datdov,class iceb_tu_str *nomdov,int nomzap_k);
void dirdov_sap(const char *datadov,const char *nomdov,class iceb_tu_str *komu,class iceb_tu_str *post);
int xudrasdok(class iceb_tu_str *datdov,class iceb_tu_str *nomdov,class spis_oth *oth);


void dirdov(class iceb_tu_str *datadov,class iceb_tu_str *nomdov)
{
char		strsql[1024];
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=4;
int             X=0;
class SQLCURSOR cur;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str komu("");
class iceb_tu_str post("");

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

/*читаем шапку документа*/
sprintf(strsql,"select komu,post from Uddok where datd='%s' and nomd='%s'",
datadov->ravno_sqldata(),nomdov->ravno());
if(readkey(strsql,&row,&cur) == 1)
 {
  komu.new_plus(row[0]);
  post.new_plus(row[1]);
 }



naz1:;

short           kls=LINES-6;
int nomers_zap[kls];

sprintf(strsql,"select nz,zapis,ei,kol from Uddok1 where datd='%s' and nomd='%s'",
datadov->ravno_sqldata(),nomdov->ravno());



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirdov_sap(datadov->ravno(),nomdov->ravno(),&komu,&post);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK6)
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
  nomers_zap[i]=atoi(row[0]);
  pozz1++;

  sprintf(strsql,"%-*.*s|%-*s|%10.10g",
  iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
  iceb_tu_kolbait(6,row[2]),row[2],
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

dirdov_sap(datadov->ravno(),nomdov->ravno(),&komu,&post);

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
//    if(naimpoi[0] != '\0' && strstrm(row[5],naimpoi) == 0)
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
/********
    if(naimpoi[0] != '\0' && strstrm(row[5],naimpoi) == 0)
     continue;
*********/
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
//    if(naimpoi[0] != '\0' && strstrm(row[5],naimpoi) == 0)
//     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/


 }

vper:;

    class spis_oth oth;   
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("udov1_3.txt");
    goto naz;

  case SFK1:  /*Шапка документа*/
    udvhd(datadov,nomdov);
    goto naz1;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirdov_vvodz(datadov,nomdov,nomers_zap[kom]);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    if(iceb_t_pbpds(datadov->ravno()) != 0)
     goto naz;
    dirdov_vvodz(datadov,nomdov,0);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(datadov->ravno()) != 0)
     goto naz;
     
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"delete from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
    datadov->ravno_sqldata(),nomdov->ravno(),nomers_zap[kom]);

    sql_zapis(strsql,0,0);

    goto naz1;

  case SFK3:       /*Удалить документ*/
    if(iceb_t_pbpds(datadov->ravno()) != 0)
     goto naz;
     
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"delete from Uddok1 where datd='%s' and nomd='%s'",
    datadov->ravno_sqldata(),nomdov->ravno());

    sql_zapis(strsql,0,0);

    sprintf(strsql,"delete from Uddok where datd='%s' and nomd='%s'",
    datadov->ravno_sqldata(),nomdov->ravno());

    sql_zapis(strsql,0,0);

    return;

  case FK5:    //Распечатать
    if(xudrasdok(datadov,nomdov,&oth) == 0)
       iceb_t_rabfil(&oth,"");
    
    goto naz;


  default:
    goto naz;
 }

}
/*****************************/
/*Шапка*/
/**************************/
void dirdov_sap(const char *datadov,const char *nomdov,
class iceb_tu_str *komu,
class iceb_tu_str *post)
{
short		y=2;
short           x=COLS-20;
clear();
printw("%s:%s %s:%s\n",
gettext("Дата выписки"),datadov,
gettext("Номер документа"),nomdov);
printw("%s:%s\n",gettext("Кому"),komu->ravno());
printw("%s:%s\n",gettext("Постав."),post->ravno());

mvprintw(3,0,"                1                  2       3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("наименование"));
mvprintw(y++,x,"2-%s",gettext("ед.измер."));
mvprintw(y++,x,"3-%s",gettext("количество"));


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

}
/*********************************/
/*Запись строки*/
/*******************/
int dirdov_zapstr(class iceb_tu_str *datdov,
class iceb_tu_str *nomdov,
int nomzap_k,
class VVOD *VV)
{
char strsql[512];

if(VV->data_ravno(0)[0] == '\0' || \
VV->data_ravno(1)[0] == '\0' || \
VV->data_ravno(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты!"));
  return(1);
 } 

class lock_tables KKK("LOCK TABLES Uddok1 WRITE,icebuser READ");


if(nomzap_k == 0) /*Ввод новой записи*/
 {
 int nomzap=0;
  /*Узнаём свободный номер*/
  for(nomzap=1; nomzap < 10000; nomzap++)
   {
    sprintf(strsql,"select nz from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
    datdov->ravno_sqldata(),nomdov->ravno(),nomzap);
    if(readkey(strsql) == 0)
     break;
   }


  sprintf(strsql,"insert into Uddok1 values('%s','%s',%d,'%s','%s',%10.10g,%d,%ld)",
  datdov->ravno_sqldata(),nomdov->ravno(),nomzap,
  VV->data_ravno_filtr(0),  
  VV->data_ravno_filtr(1),  
  VV->data_ravno_atof(2),  
  iceb_t_getuid(),
  time(NULL));
 }
else /*Корректировка существующей*/
 {
  sprintf(strsql,"update Uddok1 set \
zapis='%s',\
ei='%s',\
kol=%10.10g,\
ktoz=%d,\
vrem=%ld \
where datd='%s' and nomd='%s' and nz=%d",
  VV->data_ravno_filtr(0),  
  VV->data_ravno_filtr(1),  
  VV->data_ravno_atof(2),
  iceb_t_getuid(),
  time(NULL),
  datdov->ravno_sqldata(),nomdov->ravno(),nomzap_k);

 }

if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
return(0);

}

/****************************************/
/*Ввод и корректировка записи в документ*/
/*****************************************/
int dirdov_vvodz(class iceb_tu_str *datdov,
class iceb_tu_str *nomdov,
int nomzap_k)
{
char strsql[512];
class VVOD VV(0);
int N=0;
int K=0;

VV.VVOD_SPISOK_add_data(200);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2


if(nomzap_k != 0) /*корректировка записи*/
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select zapis,ei,kol,ktoz,vrem from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
  datdov->ravno_sqldata(),nomdov->ravno(),nomzap_k);  
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s",gettext("Не нашли запись в документе"),datdov->ravno(),nomdov->ravno());
    iceb_t_soob(strsql);
    return(1);
   }
  VV.data_plus(0,row[0]);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]); 
  kzvz(row[3],row[4],&VV);
 }
if(nomzap_k == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

VV.VVOD_SPISOK_add_MD(gettext("Наименование........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));//2

naz:;


helstr(LINES-1,0,"F1",gettext("помощь"),
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
//    getch();
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

    if(N == 0) /*Количество*/
     {

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
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(dirdov_zapstr(datdov,nomdov,nomzap_k,&VV) != 0)
     goto naz;
    break;     
   }
  if(K == FK3) /*Единица измерения*/
   {
    class iceb_tu_str ei("");
    vibrek("Edizmer",&ei);
    VV.data_plus(1,ei.ravno());
    goto naz;    
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

return(0);
}
