/* $Id: diramort.c,v 5.58 2014/08/31 06:18:17 sasa Exp $ */
/*10.07.2015    03.07.1996      Белых А.И.      diramort.c
Просмотр амортизации для налогового учёта
*/
#include        <errno.h>
#include        "buhg.h"
#include "bsizt.h"
class diramort_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str podr;
   class iceb_tu_str kodmo;
   short mn,gn;
   short mk,gk;
   
   int metka_poi;
  diramort_poi()
   {
    mk=gk=mn=gn=0;
    metka_poi=0;
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    kodmo.new_plus("");    
   }
 };

void vus7(VVOD *VV,short mt,int in,short m,short g,int podd,int kodmo,const char *hzt,const char *hna);
//void shus7(int,int,double,double,double,double,double,double,short,short,short);
void shus7(int in,int podd,double sumzap,double bs,double iz,double bs1,double iz1,double iz11,short dt,short mt,short gt,class diramort_poi *poi);
void uosrsd(int);
void diramort_vp(const char *imafprot,const char *imafprot_am);
void diramort_rasp(class SQLCURSOR *cur,int innom,class diramort_poi *poi);
int diramort_prov(SQL_str row,class diramort_poi *poi);
int diramort_poi(class diramort_poi *poi);

extern short	startgoduos; /*Стартовый год*/
extern short	metkabo; //Если 1 то организация бюджетная

void diramort(int in,int podd,int kodmo,const char *hzt,
const char *hna)
{
class diramort_poi poi;
class spis_oth oth;
char            bros[512];
char            bros1[512];
short           i;
short           x=0,y=4;
FILE            *ff=NULL;
int             K;
short           kom=0,kom1=0;
double          sao=0.;
char            imaf[64];
short           d1,m1,g1,kvrt;
double		sumzap;
class bsizw_data bal_st;
class iceb_tu_str vvstr("");
long		pozz,pozz1;
long		kolstr;
short		mspz=0; /*метка соответствия первой записи*/
SQL_str         row;
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
int		prc;
char		imafprot[64];
char		imafprot_am[64];
short metka_ras_amort=0;
int podraz=0,kodol=0;
short dt,mt,gt;

poltekdat(&dt,&mt,&gt);
dt=1;

VVOD VV(0);
VVOD  VVOD1(2);
VVOD		DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

 
pozz=0;
sumzap=0.;

sprintf(imafprot,"bsiz%d.lst",getpid());
sprintf(imafprot_am,"bsiza%d.lst",getpid());

unlink(imafprot);
unlink(imafprot_am);

naz1:;

clear();
GDITE();
short           kls=LINES-8;
time_t vremz[kls];
int ktozap[kls];

ff=NULL; //обязательно

if(metka_ras_amort == 0)
 {
  if((ff = fopen(imafprot,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
  startfil(ff);

 }

if(in < 0)
 { 
  bal_st.bs=bal_st.iz=0.;
  sprintf(bros1,"Стартовая балансовая стоимость для группы %d",in*-1);
  memset(bros,'\0',sizeof(bros));
  if(iceb_t_poldan(bros1,bros,"uosnast.alx") != 0)
   {
    sprintf(strsql,"Не найдена %s !!",bros1);
    iceb_t_soob(strsql);
    
   }
  else
    bal_st.bs=atof(bros);

  memset(bros,'\0',sizeof(bros));
  sprintf(bros1,"Стартовый износ для группы %d",in*-1);
  if(iceb_t_poldan(bros1,bros,"uosnast.alx") != 0)
   {
    sprintf(strsql,"Не найден %s !!",bros1);
    iceb_t_soob(strsql);
    
   }
  else
    bal_st.iz=atof(bros);

   
  bsiz23(hna,dt,mt,gt,&bal_st.bs,&bal_st.iz,&bal_st.iz1,ff);
 }
else
 {
  bsiz(in,podd,dt,mt,gt,&bal_st,ff);
 }

if(metka_ras_amort == 0)
  fclose(ff);
metka_ras_amort=0;

K=0;

//sprintf(strsql,"select * from Uosamor where innom=%d and god >= %d order by god desc,mes desc",in,startgoduos);
sprintf(strsql,"select * from Uosamor where innom=%d and da >= '%04d-01-01' order by da desc",in,startgoduos);
if(poi.metka_poi == 1)
 {
  if(poi.gn != 0 && poi.gk != 0)
  sprintf(strsql,"select * from Uosamor1 where innom=%d and da >= '%04d-%02d-01' and da <= '%04d-%02d-31' \
 order by da desc",in,poi.gn,poi.mn,poi.gk,poi.mk);

  if(poi.gn != 0 && poi.gk == 0)
  sprintf(strsql,"select * from Uosamor1 where innom=%d and da >= '%04d-%02d-01' \
 order by da desc",in,poi.gn,poi.mn);

  if(poi.gn == 0 && poi.gk != 0)
  sprintf(strsql,"select * from Uosamor1 where innom=%d and da <= '%04d-%02d-01' \
 order by da desc",in,poi.gk,poi.mk);

 }
/*
printw("\n%s\n",strsql);
refresh();
*/
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {  
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

  shus7(in,podd,sumzap,bal_st.sbs,bal_st.siz,bal_st.bs,bal_st.iz,bal_st.iz1,dt,mt,gt,&poi);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == FK3 || K == FK5)
      goto lll;
  goto vper;
 }
else
 {
  sumzap=0.;
  while(cur.read_cursor(&row) != 0)
   sumzap+=atof(row[6]);
 }
naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
pozz1=0;
mspz=2;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1; )
 {
  if(cur.read_cursor(&row) == 0)
    break;
  if(diramort_prov(row,&poi) != 0)
   continue;

  pozz1++;
  
  ktozap[i]=atoi(row[6]);
  vremz[i]=atol(row[7]);
    
  sprintf(strsql,"%s|%10.2f|%-3s|%-*s|%-4s|%s",
  iceb_tu_datzap(row[1]),
  atof(row[5]),
  row[2],
  iceb_tu_kolbait(4,row[3]),
  row[3],row[4],row[8]);

  spis_menu.spis_str.plus(strsql);
  i++;
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }


shus7(in,podd,sumzap,bal_st.sbs,bal_st.siz,bal_st.bs,bal_st.iz,bal_st.iz1,dt,mt,gt,&poi);

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
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(diramort_prov(row,&poi) != 0)
     continue;
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

    if(diramort_prov(row,&poi) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  if(pozz + kls >= kolstr)
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
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  GDITE();
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
    if(diramort_prov(row,&poi) != 0)
     continue;

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
  polen(str_vib.ravno(),bros,sizeof(bros),1,'|');
  rsdat(&d1,&m1,&g1,bros,1);
  polen(str_vib.ravno(),&sao,2,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC: /*Выход*/
     unlink(imafprot);
     unlink(imafprot_am);
     return;

  case FK1: /*помощь*/
    GDITE();
    iceb_t_pdoc("uos8.txt");
    clear();
    goto naz1;


  case FK2:    /*Ввести запись*/
  case PLU:    /*Ввести запись*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_data(8);
    VV.VVOD_SPISOK_add_data(20);
 
    vus7(&VV,0,in,0,0,podd,kodmo,hzt,hna);
    goto naz1;

  case ENTER: /*Корректировать запись*/
  case SFK2: /*Корректировать запись*/

    VV.VVOD_delete();
    kzvz(ktozap[kom],vremz[kom],&VV);
    
    sprintf(strsql,"%d.%d",m1,g1);
    VV.VVOD_SPISOK_add_data(strsql,8);

    sprintf(strsql,"%.2f",sao);
    VV.VVOD_SPISOK_add_data(strsql,20);

    vus7(&VV,1,in,m1,g1,podd,kodmo,hzt,hna);
    goto naz1;

  case FK3:   /*Удалить запись*/
//    sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
    if(iceb_t_pbpds(m1,g1) != 0)
      goto naz;

    if(danet(&DANET,2,stdscr) == 1)
     {
//      sprintf(strsql,"delete from Uosamor where innom=%d and god=%d and mes=%d",in,g1,m1);
      sprintf(strsql,"delete from Uosamor where innom=%d and da >= '%04d-%02d-01' and da <= '%04d-%02d-31'",in,g1,m1,g1,m1);
/*    printw("\nstrsql=%s\n",strsql);*/
      sql_zapis(strsql,0,0);
      goto naz1;
     }

    goto naz;

  case SFK4:   /*поиск*/
    if(diramort_poi(&poi) == 0)
       pozz=0;
    goto naz1;

  case FK4:   /*Расчёт амортизации*/

    memset(bros,'\0',sizeof(bros));
    kvrt=m1=g1=0;

nazz:;
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);
    if(uosprovar(hna,0) == 1)    
     {
      if(metkabo == 0)
       {
        helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
        
        
        VVOD1.VVOD_delete();
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите квартал (1,2,3,4)"));
        vvstr.new_plus("");
        if((kom1=vvod1(&vvstr,2,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
         {
          if(kolstr > 0)
           goto naz;
          else
           goto lll;
         }

        if(kom1 == ESC)
         {
          if(kolstr > 0)
           goto naz;
          else
           goto lll;
         }

        kvrt=vvstr.ravno_atoi();
        if(kvrt < 1 || kvrt > 4)
         {
          iceb_t_soob(gettext("Не верно введён квартал !"));
          goto nazz;
         } 
       }
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
      
      sprintf(bros,"%d",gt);
      vvstr.new_plus(bros);      
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));

      if((kom1=vvod1(&vvstr,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       {
        if(kolstr > 0)
         goto naz;
        else
         goto lll;
       }
      if(kom1 == ESC)
       {
        if(kolstr > 0)
         goto naz;
        else
         goto lll;
       }

      g1=vvstr.ravno_atoi();
      if(g1 < 1997 || g1 > 3000)
       {
        iceb_t_soob(gettext("Не верно введён год !"));
        goto nazz;
       } 
    }
   else
    {
nazz1:;
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
      
      sprintf(bros,"%d.%d",mt,gt);
      vvstr.new_plus(bros);
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)"));

      if((kom1=vvod1(&vvstr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       {
        if(kolstr > 0)
         goto naz;
        else
         goto lll;
       }
      if(kom1 == ESC)
       {
        if(kolstr > 0)
         goto naz;
        else
         goto lll;
       }


      if(rsdat1(&m1,&g1,vvstr.ravno()) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
        goto nazz1;
       }
    }

    if(kvrt == 1)
      m1=1;
    if(kvrt == 2)
      m1=4;
    if(kvrt == 3)
      m1=7;
    if(kvrt == 4)
     m1=10;
    if(metkabo == 1)
      m1=1;
      
//    sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));

    for(i=m1 ; i < m1+3; i++)
    if(iceb_t_pbpds(i,g1) != 0)
     {
      if(kolstr > 0)
       goto naz;
      else
       goto lll;
     }
    
    metka_ras_amort=1;
    
    if(in > 0)
     {
      if((ff = fopen(imafprot_am,"w")) == NULL)
       {
        error_op_nfil(imaf,errno,"");
        return;
       }
      startfil(ff);

      if(amort(in,m1,g1,1,kvrt,hna,ff) != 0)
	OSTANOV();
      fclose(ff);
     }
    else
     {
      if((ff = fopen(imafprot_am,"w")) == NULL)
       {
        error_op_nfil(imaf,errno,"");
        return;
       }
      startfil(ff);
      
      amort23(in,podd,kodmo,kvrt,g1,hzt,ff);
      fclose(ff);
     } 
    goto naz1;

  case FK5:   /*Распечатать*/

    diramort_rasp(&cur,in,&poi);
    goto naz;


  case FK6:   /*Распечатка документов*/

    uosrsd(in);

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK7:   /*Просмотр протокола расчёта амортизации*/
    diramort_vp(imafprot,imafprot_am);

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK8:   //Смена даты получения остатка

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату, на которую рассчитать остаток (д.м.г)"));
    vvstr.new_plus("");
    if((kom1=vvod1(&vvstr,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
      if(kolstr > 0)
       goto naz;
      else
       goto lll;
     }
    if(kom1 == ESC)
     {
      if(kolstr > 0)
       goto naz;
      else
       goto lll;
     }
     

    if(rsdat(&d1,&m1,&g1,vvstr.ravno(),1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
     }
    else
     {
      dt=d1; mt=m1; gt=g1;
     }     
    /*Узанём где числится на эту дату*/
    if(poiin(in,dt,mt,gt,&podraz,&kodol) == 0)
     podd=podraz;
     
    goto naz1;

  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }
}

/*******/
/*Ввод */
/*******/

void vus7(VVOD *VV,short mt, //0-ввести новое 1-корректировать
int in,short m,short g,
int podd,
int kodmo,
const char *hzt,const char *hna)
{
int             par;
short           m1,g1;
char		strsql[512];
time_t		vrem;

VV->VVOD_SPISOK_add_MD(gettext("Дата....(м.г).........."));
VV->VVOD_SPISOK_add_MD(gettext("Сумма амортотчислений.."));

naz:;


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

par=VV->vvod(0,1,1,-1,-1);

switch(par)
 {
  
  case FK2:
  case PLU:
    break;
  case FK10:
  case ESC:
    return;
  default:
   goto naz;
 }

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён месяц !"));
  goto naz;
 }


if(rsdat1(&m1,&g1,VV->VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Неверно введена дата !"));
  goto naz;
 }
//sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
if(iceb_t_pbpds(m1,g1) != 0)
  goto naz;

if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введена сумма амортотчислений !"));
  goto naz;
 }

time(&vrem);

if(mt == 0)
 {
  sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",in,g1,m1,podd,hzt,hna,ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem,kodmo);
 }


if(mt == 1)
 {
//  sprintf(strsql,"%s",iceb_t_imafnsi("bluos.alx"));
  if(iceb_t_pbpds(m,g) != 0)
    goto naz;
/*****************
  sprintf(strsql,"update Uosamor \
set \
god=%d,\
mes=%d,\
podr=%d,\
kmo=%d,\
hzt='%s',\
hna='%s',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where innom=%d and god=%d and mes=%d",
  g1,m1,podd,kodmo,hzt,hna,ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem,in,g,m);
***********************/
  sprintf(strsql,"update Uosamor \
set \
da='%04d-%02d-01',\
podr=%d,\
kmo=%d,\
hzt='%s',\
hna='%s',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where innom=%d and da='%04d-%02d-01'",
  g1,m1,podd,kodmo,hzt,hna,ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem,in,g,m);
 }

if(sql_zapis(strsql,0,0) != 0)
 {
  goto naz;
 }

}

/*******/
/*Шапка*/
/*******/
void shus7(int in,int podd,double sumzap,
double bs,double iz,double bs1,double iz1,double iz11,
short dt,short mt,short gt,class diramort_poi *poi)
{
int		X,Y;

move(0,0);

printw("%s\n",
gettext("Ввод и корректировка амортотчислений для налогового учёта"));
if(in > 0)
  printw("%s %d %s %d",
  gettext("Инвентарный номер"),
  in,
  gettext("Подразделение"),
  podd);
else
 {
  printw("%s %d",
  gettext("Группа"),
  in*(-1));
 }
printw(" %s:%.2f\n",gettext("Сумма всех записей"),sumzap);

move(Y=3,X=COLS-30);
printw(gettext("Расшифровка полей:"));
move(++Y,X);
printw("1-%s",gettext("дата"));
move(++Y,X);
printw("2-%s",gettext("сумма"));
move(++Y,X);
printw("3-%s",gettext("подразделение"));
move(++Y,X);
printw("4-%s",gettext("шифр затрат"));
move(++Y,X);
printw("5-%s",gettext("шифр н. аморт-отч."));
move(++Y,X);
printw("6-%s",gettext("код.мат.от."));

move(Y+=2,X);
printw(gettext("Бал.стоимость   Износ"));
move(++Y,X);
printw("%10.2f %10.2f",bs,iz);
move(++Y,X);
printw("%10.2f %10.2f",bs1,iz1);
move(++Y,X);
printw("%s:%9.2f",gettext("Амортизация"),iz11);
move(++Y,X);
printw("%10.2f %10.2f",bs+bs1,iz+iz1+iz11);

move(++Y,X);
printw("%*s:%9.2f",iceb_tu_kolbait(11,gettext("Остаток")),gettext("Остаток"),(bs+bs1)-(iz+iz1+iz11));
move(++Y,X-2);
printw("%*s:%02d.%02d.%d",iceb_tu_kolbait(11,gettext("Дата остатка")),gettext("Дата остатка"),dt,mt,gt);

if(poi->metka_poi != 0)
 {
  Y++;
  mvprintw(++Y,X,"%s:",gettext("Поиск"));
  if(poi->datan.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s\n",gettext("Дата начала"),poi->datan.ravno());
  if(poi->datak.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s\n",gettext("Дата конца"),poi->datak.ravno());
  if(poi->podr.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s\n",gettext("Подразделение"),poi->podr.ravno());
  if(poi->kodmo.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s\n",gettext("Мат.отв."),poi->kodmo.ravno());
 }

move(3,0);
printw("      1          2       3    4   5    6");

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("расчёт"),
"F5",gettext("печать"),
"F6",gettext("док."),
"F7",gettext("прот."),
"F8",gettext("см.даты"),
"F10",gettext("выход"),NULL);
}
/*********************************/
/*выбор просмотра*/
/********************************/
void diramort_vp(const char *imafprot,const char *imafprot_am)
{

int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Просмотр протокола расчёта балансовой стоимости и износа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр протокола расчёта амортизации"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13
for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;

  switch (kom)
   {
    case 2:
    case -1 :
      return;
    case 0:
      vizred(imafprot);
      return;
    case 1:
      vizred(imafprot_am);
      return;
   }
 }


}
/************************************/
/*Распечатка*/
/***********************************/
void diramort_rasp(class SQLCURSOR *cur,int innom,class diramort_poi *poi)
{
char strsql[512];
class iceb_tu_str naim("");
class iceb_fopen fil;
char imaf[64];
SQL_str row;
class SQLCURSOR cur1;
sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(readkey(strsql,&row,&cur1) == 1)
 naim.new_plus(row[0]);


sprintf(imaf,"amortnu%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Амортизация налогового учёта"),fil.ff);
fprintf(fil.ff,"%s:%d %s\n",gettext("Инвентарный номер"),innom,naim.ravno());

if(poi->metka_poi != 0)
 {
  if(poi->datan.getdlinna() > 1)
    fprintf(fil.ff,"%s:%s\n",gettext("Дата начала"),poi->datan.ravno());
  if(poi->datak.getdlinna() > 1)
    fprintf(fil.ff,"%s:%s\n",gettext("Дата конца"),poi->datak.ravno());
  if(poi->podr.getdlinna() > 1)
    fprintf(fil.ff,"%s:%s\n",gettext("Подразделение"),poi->podr.ravno());
  if(poi->kodmo.getdlinna() > 1)
    fprintf(fil.ff,"%s:%s\n",gettext("Мат.отв."),poi->kodmo.ravno());
 }

fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"\
   Дата   |  Сумма   |Подр.|Мат.от.|Шифр затрат|Группа|Дата и время запис.| Кто записал\n");
  
fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------\n");



int kolstr=0;
double itogo=0.;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(diramort_prov(row,poi) != 0)
   continue;
  fprintf(fil.ff,"%s %10.2f %-5s %-7s %-*s %-*s %s %s\n",
  iceb_tu_datzap(row[1]),
  atof(row[5]),
  row[2],
  row[8],
  iceb_tu_kolbait(11,row[3]),
  row[3],
  iceb_tu_kolbait(6,row[4]),
  row[4],
  iceb_tu_vremzap(row[7]),iceb_t_kszap(row[6]));
  
  itogo+=atof(row[5]);
  kolstr++;
 }
fprintf(fil.ff,"%*s %10.2f\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),itogo);

fprintf(fil.ff,"%s: %d\n",gettext("Количество записей"),kolstr);
podpis(fil.ff);
fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка амортотчислений"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
/*******************************/
/*Поиск*/
/**********************************/
int diramort_poi(class diramort_poi *poi)
{
int kom=0;

class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.......(м.г)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца........(м.г)."));//1
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный.(,,).."));//3

VV.VVOD_SPISOK_add_data(poi->datan.ravno(),8);
VV.VVOD_SPISOK_add_data(poi->datak.ravno(),8);
VV.VVOD_SPISOK_add_data(poi->podr.ravno(),256);
VV.VVOD_SPISOK_add_data(poi->kodmo.ravno(),256);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi->datan.new_plus(VV.VVOD_SPISOK_return_data(0));
poi->datak.new_plus(VV.VVOD_SPISOK_return_data(1));
poi->podr.new_plus(VV.VVOD_SPISOK_return_data(2));
poi->kodmo.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   poi->metka_poi=0;
   return(1);

  case FK2:
  case PLU:
     poi->gn=poi->mn=poi->gk=poi->mk=0;

     if(poi->datan.getdlinna() > 1)
      if(rsdat1(&poi->mn,&poi->gn,poi->datan.ravno()) != 0)
       {
        iceb_t_soob(gettext("Не правильно введена дата начала!"));
        goto naz;
       }
     if(poi->datak.getdlinna() > 1)
      if(rsdat1(&poi->mk,&poi->gk,poi->datak.ravno()) != 0)
       {
        iceb_t_soob(gettext("Не правильно введена дата конца!"));
        goto naz;
       }
     poi->metka_poi=1;
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


return(0);
}
/*****************************************/
/*проверка*/
/*****************************************/
int diramort_prov(SQL_str row,class diramort_poi *poi)
{
if(poi->metka_poi == 0)
 return(0);
 
if(proverka(poi->podr.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(poi->kodmo.ravno(),row[8],0,0) != 0)
 return(1);

return(0);
}
