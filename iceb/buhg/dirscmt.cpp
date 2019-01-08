/*$Id: dirscmt.c,v 5.26 2013/05/17 14:55:59 sasa Exp $*/
/*13.07.2015	26.05.2005	Белых А.И.	dirscmt.c
Работа со справочником средних цен на автомобильное топливо
*/
#include        <errno.h>
#include        "buhg.h"

class dirscmt_poi
 {
  public:
   short dn,mn,gn;
   short dk,mk,gk;

   class iceb_tu_str kodmt;

   short metka_poi;

  dirscmt_poi()
   {
    mn=gn=dn=0;
    mk=gk=dk=0;
    kodmt.new_plus("");
    metka_poi=0;
   }
 };

void dirscmt_sap(class dirscmt_poi *poi);
int dirscmt_vvod(short dv,short mz,short gz,const char *kodmtz);
int dirscmt_zap(short dv,short mv,short gv,const char *kodmt,class VVOD *VV);
int dirscmt_prov(SQL_str row,class dirscmt_poi *poi);
int dirscmt_p(class dirscmt_poi *poi);
void dirscmt_ras(SQLCURSOR *cur,class dirscmt_poi *poi);
 

extern char *imabaz;

void dirscmt()
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
short d,m,g;
short dv,mv,gv; //Дата в выбранной строке
char kodmtv[64]; //Выбранная марка топлива
class dirscmt_poi poi;
class iceb_t_mstr spis_menu(0);

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

if(poi.metka_poi == 0)
 sprintf(strsql,"select * from Uplmtsc order by datsc desc");
if(poi.metka_poi == 1)
 {
  if(poi.dn != 0)
   {
    sprintf(strsql,"select * from Uplmtsc where datsc >= '%04d-%02d-%02d' order by datsc desc",
    poi.gn,poi.mn,poi.dn);
  
    if(poi.dk != 0)
     sprintf(strsql,"select * from Uplmtsc where datsc >= '%04d-%02d-%02d' and \
datsc <= '%04d-%02d-%02d' order by datsc desc",
     poi.gn,poi.mn,poi.dn,  
     poi.gk,poi.mk,poi.dk);
    
   }
  
 }

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
lll:;

  dirscmt_sap(&poi);

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

  if(dirscmt_prov(row,&poi) != 0)
   continue;

  rsdat(&d,&m,&g,row[0],2);       

  sprintf(strsql,"%02d.%02d.%04d|%-*s|%6s|%s",
  d,m,g,
  iceb_tu_kolbait(6,row[1]),row[1],
  row[2],row[5]);

  spis_menu.spis_str.plus(strsql);
  i++;
    
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


dirscmt_sap(&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));

kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
    if(dirscmt_prov(row,&poi) != 0)
     continue;
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
    if(dirscmt_prov(row,&poi) != 0)
     continue;
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
    if(dirscmt_prov(row,&poi) != 0)
     continue;
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

  POLE(spis_menu.spis_str.ravno(kom),strsql,1,'|');
  rsdat(&dv,&mv,&gv,strsql,1);
  POLE(spis_menu.spis_str.ravno(kom),kodmtv,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("upl3_8.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirscmt_vvod(dv,mv,gv,kodmtv);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirscmt_vvod(0,0,0,"");
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(mv,gv) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Uplmtsc where datsc='%04d-%02d-%02d' and kodt='%s'",
      gv,mv,dv,kodmtv);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK4:       /*Поиск*/

     dirscmt_p(&poi);
     GDITE();

     clear();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    dirscmt_ras(&cur,&poi);
    goto naz1;

  default:
    goto naz;
    break;
 }

return;


}
/************************/
/*Шапка*/
/*************************/
void dirscmt_sap(class dirscmt_poi *poi)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка средних цен на топливо"));

mvprintw(2,0,"      1        2       3    4");

mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("код топлива"));
mvprintw(y++,x,"3-%s",gettext("цена"));
mvprintw(y++,x,"4-%s",gettext("коэф.п."));
if(poi->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  if(poi->mn != 0)
    mvprintw(y++,x,"%s:%d.%d.%d",gettext("Дата н."),poi->dn,poi->mn,poi->gn);
  if(poi->mk != 0)
    mvprintw(y++,x,"%s:%d.%d.%d",gettext("Дата к."),poi->dk,poi->mk,poi->gk);
  if(poi->kodmt.getdlinna() > 1)
    mvprintw(y++,x,"%s:%d.%d",gettext("Топливо"),poi->kodmt.ravno());
   
  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

}
/**********************/
/*Ввод и корректировка записи*/
/*****************************/

int dirscmt_vvod(short dz,short mz,short gz,const char *kodmtz)
{
time_t vremz;
int ktozap;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int voz=0;
int N=0,K=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(11);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3


if(kodmtz[0] != '\0')
 {

  sprintf(strsql,"select * from Uplmtsc where datsc='%04d-%02d-%02d' and kodt='%s'",
  gz,mz,dz,kodmtz);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,1,1);
    return(1);
   }
  short d,m,g;
  rsdat(&d,&m,&g,row[0],2);
  sprintf(strsql,"%d.%d.%d",d,m,g);
  

  VV.data_plus(0,strsql);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]);
  VV.data_plus(3,row[5]);

  ktozap=atoi(row[3]);
  vremz=atol(row[4]);

  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

  kzvz(ktozap,vremz,&VV);
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));


VV.VVOD_SPISOK_add_MD(gettext("Дата (д.м.г)............................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код топлива.............................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Средняя цена............................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коэффициент перевода литр => киллограмм.."));//3


naz:;



 helstr(LINES-1,0,\
// "F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

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

//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    voz=1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirscmt_zap(dz,mz,gz,kodmtz,&VV) != 0)
      goto naz;
    voz=0;
    break;
   }

  if(K == FK3) //Просмотр списка водителей
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    class iceb_tu_str kod("");
    if(dirmt(&kod,1) == 0)
      VV.data_plus(1,kod.ravno());
    clear();
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
return(voz);



}
/**************************/
/*Запись данных в базу*/
/*************************/

int dirscmt_zap(short dv,short mv,short gv,const char *kodmt,class VVOD *VV)
{
char strsql[512];
short d,m,g;

if(rsdat(&d,&m,&g,VV->data_ravno(0),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата !"));
  return(1);
 }

if(iceb_t_pbpds(m,g) != 0)
  return(1);
if(iceb_t_pbpds(mv,gv) != 0)
  return(1);

sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV->data_ravno(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),VV->data_ravno(1));
  iceb_t_soob(strsql);
  return(1);
 }
time_t vrem;
time(&vrem);
if(kodmt[0] == '\0') //Ввод новой записи
 {
  sprintf(strsql,"insert into Uplmtsc values('%04d-%02d-%02d','%s',%.2f,%d,%ld,%.3f)",
  g,m,d,VV->data_ravno(1),VV->data_ravno_atof(2),iceb_t_getuid(),vrem,VV->data_ravno_atof(3));
  
 }
else //Корректировка 
 {
  sprintf(strsql,"update Uplmtsc set \
datsc='%04d-%02d-%02d',\
kodt='%s',\
srcn=%.2f,\
ktoz=%d,\
vrem=%ld,\
kp=%.3f \
where datsc='%04d-%02d-%02d' and kodt='%s'",
  g,m,d,
  VV->data_ravno(1),
  VV->data_ravno_atof(2),
  iceb_t_getuid(),
  vrem,
  VV->data_ravno_atof(3),
  gv,mv,dv,kodmt);
 }
if(sql_zapis(strsql,0,0) != 0)
 return(1);

return(0);

}
/**********************/
/*Меню задания реквизитов поиска*/
/********************************/

int dirscmt_p(class dirscmt_poi *poi)
{
short dn,mn,gn;
short dk,mk,gk;
class iceb_tu_str kod("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код топлива.........."));//2

char strsql[100];

memset(strsql,'\0',sizeof(strsql));
if(poi->mn != 0)
 sprintf(strsql,"%d.%d.%d",poi->dn,poi->mn,poi->gn);
VV.data_sp_plus(strsql,11);

memset(strsql,'\0',sizeof(strsql));
if(poi->mk != 0)
 sprintf(strsql,"%d.%d.%d",poi->dk,poi->mk,poi->gk);

VV.data_sp_plus(strsql,11);
VV.data_sp_plus(poi->kodmt.ravno(),32);

naz:;

poi->metka_poi=0;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kl=VV.vvod(0,1,1,-1,-1);


switch(kl)
 {
  case FK10:
  case ESC:
    return(1);
    break;
    
  case FK2:
  case PLU:
    poi->mn=0;
    poi->gn=0;
    poi->dn=0;

    poi->mk=0;
    poi->gk=0;
    poi->dk=0;
    
    if(VV.data_ravno(0)[0] != '\0')
     if(rsdatp(&dn,&mn,&gn,VV.data_ravno(0),&dk,&mk,&gk,VV.data_ravno(1)) != 0)
      goto naz;

    poi->mn=mn;
    poi->gn=gn;
    poi->dn=dn;

    poi->mk=mn;
    poi->gk=gn;
    poi->dk=dk;

    poi->kodmt.new_plus(VV.data_ravno(1));
    poi->metka_poi=1;
    break;

  case FK3:
    kod.new_plus("");
    if(dirmt(&kod,1) == 0)
      VV.data_plus(2,kod.ravno());
    clear();
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

 }
return(0);
}
/************************/
/*Проверка на условия поиска*/
/****************************/
int dirscmt_prov(SQL_str row,class dirscmt_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);


if(proverka(poi->kodmt.ravno(),row[1],0,0) != 0)
 return(1);

return(0);

}
/********************************/
/*Распечатка*/
/*********************************/
void dirscmt_ras(SQLCURSOR *cur,class dirscmt_poi *poi)
{

char imaf[64];
sprintf(imaf,"scmt%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"%s\n\
%s\n",iceb_t_get_pnk("00",0),
gettext("Распечатка справочника средних цен на топливо"));

fprintf(ff,"\
-----------------------------\n");
fprintf(ff,"\
  Дата    |Код топ.| Цена   |\n");
fprintf(ff,"\
-----------------------------\n");

SQL_str row;
cur->poz_cursor(0);
short d,m,g;
while(cur->read_cursor(&row) != 0)
 {
  if(dirscmt_prov(row,poi) != 0)
     continue;

  rsdat(&d,&m,&g,row[0],2);
  fprintf(ff,"%02d.%02d.%04d %-*s %8s\n",
  d,m,g,
  iceb_tu_kolbait(8,row[1]),row[1],
  row[2]);
  
 }
fprintf(ff,"\
-----------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка справочника средних цен на топливо"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);

iceb_t_rabfil(&oth,"");


}
