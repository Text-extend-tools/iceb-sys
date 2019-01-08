/* $Id: dirpzpd.c,v 5.32 2013/05/17 14:55:58 sasa Exp $ */
/*13.07.2015    12.05.1998      Белых А.И.       dirpzpd.c
Просмотр ввод и корректировка подтверждающих записей к 
платежным документам
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

void            vdok5(VVOD*,const char*,int,short,short,short,short,short,short,double);
void            shdk5(const char*,double);
void		sozvdok5(VVOD*);

extern short    ddd,mmm,ggg;
extern class REC rec;

void dirpzpd(const char *tabl)
{
class spis_oth oth;
short           x=0,y=3;
struct  tm      *bf;
time_t          tmm;
int             K;
short           kom=0;
short           i;
char            imaf[64];
FILE            *ff;
class iceb_tu_str datpr("");
short		dp,mp,gp;
double		suma;
long		pozz,pozz1;
long		kolstr;
SQL_str         row;
char		strsql[1024];
double		sum;
int		prc;
class iceb_t_mstr spis_menu(0);

pozz=0;

//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);

naz1:;
clear();

short           kls=LINES-5;

sum=suma=0.;

move(0,0);

K=0;


if(SRAV(tabl,"Pltp",0) == 0)
 sprintf(strsql,"select datp,suma,ktoz from Pzpd where tp=0 and \
datd='%04d-%02d-%02d' and nomd='%s' order by datp asc",
 rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(SRAV(tabl,"Pltt",0) == 0)
 sprintf(strsql,"select datp,suma,ktoz from Pzpd where tp=1 and \
datd='%04d-%02d-%02d' and nomd='%s' order by datp asc",
 rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;
  shdk5(tabl,suma);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK5 || K == SFK2 || K == ENTER || K == FK3)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
  goto vper;
 }

naz:;
GDITE();

cur.poz_cursor(pozz);
pozz1=0;
suma=0.;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  rsdat(&dp,&mp,&gp,row[0],2);
  sum=atof(row[1]);
  suma+=sum;   
  sprintf(strsql,"%02d.%02d.%d|%-4s|%.2f",
  dp,mp,gp,row[2],sum);

  spis_menu.spis_str.plus(strsql);
  i++;
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

shdk5(tabl,suma);
prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i<1  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) != 0)
      break;
    i++;
   }
  kom=kls;
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
  printw("\n вперед pozz=%d\n",pozz);
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
    cur.read_cursor(&row);

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
  polen(spis_menu.spis_str.ravno(kom),&datpr,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&sum,3,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("dokum2_1_3.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case ENTER:
  case SFK2:   /*Корректировать запись*/
    VV.VVOD_delete();
    sozvdok5(&VV);
    rsdat(&dp,&mp,&gp,datpr.ravno(),1);
    VV.VVOD_SPISOK_zapis_data(0,datpr.ravno());
    sprintf(strsql,"%.2f",sum);    
    VV.VVOD_SPISOK_zapis_data(1,strsql);
    
    vdok5(&VV,tabl,atoi(rec.nomdk.ravno()),rec.dd,rec.md,rec.gd,dp,mp,gp,suma);

    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    VV.VVOD_delete();
    sozvdok5(&VV);
    dp=mp=gp=0;
    suma=rec.sumd-suma;
    if(suma == rec.sumd)
     {
       sprintf(strsql,"%d.%d.%d",rec.dd,rec.md,rec.gd);    
       VV.VVOD_SPISOK_zapis_data(0,strsql);
     }     
    sprintf(strsql,"%.2f",suma);    
    VV.VVOD_SPISOK_zapis_data(1,strsql);
    vdok5(&VV,tabl,atoi(rec.nomdk.ravno()),rec.dd,rec.md,rec.gd,dp,mp,gp,suma);

    goto naz1;
    break;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      clear();
      GDITE();
      rsdat(&dp,&mp,&gp,datpr.ravno(),1);
      if(SRAV(tabl,"Pltp",0) == 0)
       sprintf(strsql,"delete from Pzpd where tp=0 and \
datd='%04d-%02d-%02d' and datp='%04d-%02d-%02d' and nomd='%s' and suma=%.2f",
       rec.gd,rec.md,rec.dd,gp,mp,dp,rec.nomdk.ravno(),sum);

      if(SRAV(tabl,"Pltt",0) == 0)
       sprintf(strsql,"delete from Pzpd where tp=1 and \
datd='%04d-%02d-%02d' and datp='%04d-%02d-%02d' and nomd='%s' and suma=%.2f",
       rec.gd,rec.md,rec.dd,gp,mp,dp,rec.nomdk.ravno(),sum);

      sql_zapis(strsql,0,0);

      clear();
      goto naz1;
     }
    goto naz;
    
  case FK5:       /*Распечатать*/

    clear();
    refresh();

    sprintf(imaf,"pkpp%s.lst",rec.nomdk.ravno());
    if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      goto naz1;
     }

    time(&tmm);
    bf=localtime(&tmm);

    fprintf(ff,"%s\n\n%s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s %s: %d:%d\n",
    iceb_t_get_pnk("00",0),
    gettext("Распечатка подтверждающих записей к документу"),
    rec.nomdk.ravno(),
    gettext("от"),
    rec.dd,rec.md,rec.gd,
    gettext("г."),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    fprintf(ff,"\
-----------------------------------------------------\n");

    fprintf(ff,gettext("  Дата   | Kтo |    Сумма \n"));

    fprintf(ff,"\
-----------------------------------------------------\n");

    suma=0.;

    cur.poz_cursor(0);
    while(cur.read_cursor(&row) != 0)
     {
      rsdat(&dp,&mp,&gp,row[0],2);
      sum=atof(row[1]);
      suma+=sum;
      fprintf(ff,"%02d.%02d.%d|%-5s|%10.2f\n",
      dp,mp,gp,row[2],sum);

     }
    fprintf(ff,"\
-----------------------------------------------------\n\
%s - %.2f\n",gettext("Итого"),suma);
    fclose(ff);

    oth.clear();
    oth.spis_imaf.plus(imaf);
    oth.spis_naim.plus(gettext("Распечатка подтверждающих записей"));
    iceb_t_ustpeh(imaf,0);
    iceb_t_rabfil(&oth,"");
    clear();
    goto naz;

  default:
   if(kolstr > 0)
     goto naz1;
   else
     goto lll;

 }

}

/*******/
/*Ввод */
/*******/

void vdok5(VVOD *VV,const char *tabl,int npp,short d,short m,short g,
short dp,short mp,short gp,double sump)
{
int             par;
short		dv,mv,gv;
time_t		vrem;
int		i;
char		strsql[512];

VV->VVOD_SPISOK_add_MD(gettext("Дата...."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма..."));

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

par=VV->vvod(0,1,1,-1,-1);

if(par == FK2 || par == PLU)
 {
  if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введена дата !"));
    goto naz;
   }
  if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введена сумма !"));
    goto naz;
   }
  if(rsdat(&dv,&mv,&gv,VV->VVOD_SPISOK_return_data(0),1) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz;
   }


  time(&vrem);

  i=0;
  if(SRAV(tabl,"Pltp",0) == 0)
   i=0;  
  if(SRAV(tabl,"Pltt",0) == 0)
   i=1;  

  if(dp == 0)
   sprintf(strsql,"insert into Pzpd \
values (%d,'%04d-%02d-%02d','%04d-%02d-%02d','%d',%.2f,%d,%ld)",
   i,g,m,d,gv,mv,dv,npp,ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem);   
  else
    sprintf(strsql,"update Pzpd \
set \
datp='%04d-%02d-%02d',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where tp=%d and datd='%d-%02d-%02d' and datp='%d-%02d-%02d' and \
nomd='%d' and suma=%.2f",gv,mv,dv,ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem,
     i,g,m,d,gp,mp,dp,npp,sump);

  if(sql_zapis(strsql,0,0) != 0)
    goto naz;
   
 }
clear();
}

/*******************/
void shdk5(const char *tabl,double suma)
{
move(0,0);
if(SRAV(tabl,"Pltp",0) == 0)
  printw("%s %s %s %d.%d.%d%s",
  gettext("Ввод и корректировка сумм к платежному поручению"),
  rec.nomdk.ravno(),
  gettext("от"),
  rec.dd,rec.md,rec.gd,
  gettext("г."));

if(SRAV(tabl,"Pltt",0) == 0)
  printw("%s %s %s %d.%d.%d%s",
  gettext("Ввод и корректировка сумм к платежному требованию"),
  rec.nomdk.ravno(),
  gettext("от"),
  rec.dd,rec.md,rec.gd,
  gettext("г."));

move(1,0);
printw("%s %.2f %s %.2f",
gettext("Сумма платежа"),rec.sumd,
gettext("Сумма записей"),suma);
move(2,0);
printw(gettext("1-дата подтв. 2-кто записал 3-сумма"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
}
/********************************/
void	sozvdok5(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(11);
VV->VVOD_SPISOK_add_data(20);
}
