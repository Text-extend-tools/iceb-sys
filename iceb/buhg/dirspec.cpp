/* $Id: dirspec.c,v 5.51 2013/09/26 09:43:31 sasa Exp $ */
/*13.07.2015	12.01.2000	Белых А.И.	dirspec.c
Просмотр, ввод и корректировка спецификаций
Если вернули 0 выбрали
	     1-нет
*/
#include        <errno.h>
#include        <ctype.h>
#include        "buhg.h"

void		rasizdel(void);
void		shspec(int,const char*,int,int,const char*);
int		vniz(void);
int		vspec(int,int,const char*,int,class iceb_tu_str*,int);
void		sozvspec(VVOD*);
int specprov(char *kodiz,class iceb_tu_str *naim,const char *poisk,int metka_naim); //0-если поиск не заказан выход 1-если поиск не заказан читаем наименование и выходим
void            spiz(int);


int dirspec(int kodiz, //Код просмотриваемого изделия
int *kodizv, //Код выбранного изделия
const char *naimiz, //Наименование изделия
const char *naimpoi, //Наименование изделия для поиска
int *urov,   //Уровень глубины просмотра
int regim) //0-ввод и корректировка 1-выбор
{
class iceb_tu_str naimizv(""); //Наименование выбранного изделия
double		kolih;
short           x=0,y=2;
int             K;
short           kom,kom1;
int             i,prc;
long		pozz,pozz1;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str kodmv("");
class iceb_tu_str kodd("");
short		mspz;
class iceb_tu_str naim("");
int		kodizv1;
class iceb_tu_str izdel(""); /*Метка изделия*/
class iceb_tu_str poisk(naimpoi);
int             kod;
class iceb_t_mstr spis_menu(0);

*urov=*urov+1;

//Создаем класс меню
VVOD VVOD1(2);
VVOD DANET(1);
VVOD VV(0);

K=kom=kom1=pozz=pozz1=0;

naz1:;
GDITE();
clear();
short  kls=LINES-5;

K=0;
if(kodiz == 0)
 sprintf(strsql,"select distinct kodi from Specif order by kodi asc");
else
 sprintf(strsql,"select kodd,kz,kolih,koment,ei from Specif where kodi=%d",kodiz);

/*
printw("\n%s\n",strsql);
refresh();
*/
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  
  shspec(kodiz,naimiz,*urov,regim,poisk.ravno());

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);


  if(K == FK2 || K == PLU || K == FK1 || K == FK10 || K == ESC ||\
   K == FK4)
     goto vper;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto lll;
 }

if(kodmv.ravno()[0] != '\0' && kolstr > kls  && pozz < kolstr-kls)
 {
/*
  printw("pozz=%d kolstr=%d kls=%d\n",pozz,kolstr,kls);
  OSTANOV();
*/
  pozz=poivcur(kodmv.ravno(),0,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(specprov(row[0],&naim,poisk.ravno(),1) != 0)
   continue;
  memset(strsql,'\0',sizeof(strsql));   
  if(kodiz == 0)
    sprintf(strsql,"%-4s|%s",row[0],naim.ravno());
  else
   {
    izdel.new_plus("");
    if(row[1][0] == '0')
     izdel.new_plus("M");
    if(row[1][0] == '1')
     izdel.new_plus("U");
     
    /*Проверяем не является ли входящий материал изделием*/
    sprintf(strsql,"select kodi from Specif where kodi=%s limit 1",row[0]);
    kolstr1=sql_readkey(&bd,strsql,&row1,&cur1);

    if(kolstr1 != 0)
     izdel.plus("*");    
    kolih=atof(row[2]);

    if(kolih < 0.0001)
     sprintf(strsql,"%-4s|%-*.*s|%1s|%-*.*s|%.10f|%.5s",
     row[0],
     iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
     izdel.ravno(),
     iceb_tu_kolbait(4,row[4]),iceb_tu_kolbait(4,row[4]),row[4],
     kolih,row[3]);
    else
     sprintf(strsql,"%-4s|%-*.*s|%1s|%-*.*s|%10.10g|%.5s",
     row[0],
     iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
     izdel.ravno(),
     iceb_tu_kolbait(4,row[4]),iceb_tu_kolbait(4,row[4]),row[4],
     kolih,row[3]);
   }  
  spis_menu.spis_str.plus(strsql);
  i++;
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shspec(kodiz,naimiz,*urov,regim,poisk.ravno());

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
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(specprov(row[0],&naim,poisk.ravno(),0) != 0)
     continue;

//    if(provdirm(kgrm,row[1],nm,row[2],row[4],mpro,row[0],regim) != 0)
//      continue;
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

    if(specprov(row[0],&naim,poisk.ravno(),0) != 0)
     continue;

//    if(provdirm(kgrm,row[1],nm,row[2],row[4],mpro,row[0],regim) != 0)
//      continue;
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

    if(specprov(row[0],&naim,poisk.ravno(),0) != 0)
     continue;

//    if(provdirm(kgrm,row[1],nm,row[2],row[4],mpro,row[0],regim) != 0)
//      continue;
    i++;
   }
  goto naz;
 }
if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  
  if(pozz+kom >= kolstr)
   goto naz;

  polen(spis_menu.spis_str.ravno(kom),kodizv,1,'|');
//  *kodizv=atoi(bros);
  polen(spis_menu.spis_str.ravno(kom),&naimizv,2,'|');
 }

vper:;

switch (K)
 {
  case FK1:   /*Помощь*/
   GDITE();
   if(kodiz == 0)
     sprintf(strsql,"%s","spec1.txt");
   else
     sprintf(strsql,"%s","spec2.txt");
   
   iceb_t_pdoc(strsql);
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
    *urov=*urov-1;
    return(1);

  case ENTER:
   if(regim == 1)
    {
     *urov=*urov-1;
     return(0);
    }   

   kodizv1=0;
   naim.new_plus("");
   dirspec(*kodizv,&kodizv1,naimizv.ravno(),naim.ravno(),&(*urov),0);
   goto naz1;
   
  case FK2: //Ввести запись материалла
  case PLU: //Ввести запись материалла
    if(kodiz == 0)
      vniz();
    else
     {
      kodmv.new_plus("");
      vspec(kodiz,0,naimiz,0,&kodmv,0);
     }

    goto naz1;

  case SFK2:     /*Корректировать запись*/
    kod=*kodizv;
    kodd.new_plus("");       
    vspec(kodiz,kod,naimiz,1,&kodd,0); //Услуга или материал определим внутри подпрограммы
    goto naz1;


  case FK3:  /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;



    if(kodiz == 0)
     {
      //Проверяем входимость в другие изделия      
      sprintf(strsql,"select kodd from Specif where kodd=%d",*kodizv);
      if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
       {
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Удаление невозможно !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("Изделие входит в другие изделия как узел"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }
      sprintf(strsql,"delete from Specif where kodi=%d",*kodizv);
     }
    else
     sprintf(strsql,"delete from Specif where kodi=%d and kodd=%d",
     kodiz,*kodizv);
    
   /*      printw("\nstrsql=%s\n",strsql);*/
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
    goto naz1;   

  case SFK3:  /*Удалить все записи*/

    if(kodiz == 0)
      goto naz;
    sprintf(strsql,gettext("Удалить все записи в изделии ? Вы уверены ?"));

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(strsql);

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Specif where kodi=%d",kodiz);
    
   /*      printw("\nstrsql=%s\n",strsql);*/
    if(sql_zap(&bd,strsql) != 0)
      msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
    goto naz1;   

  case FK4:       /*Отобрать по образцу*/

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    poisk.new_plus("");
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

    vvod1(&poisk,32,&VVOD1,NULL,stdscr,-1,-1);

    GDITE();

    clear();
    kom=pozz=0;
    goto naz;

  case FK5:       /*Распечатать*/
    GDITE();
    if(kodiz == 0)
     rasizdel();
    else
     rasspec(kodiz);
    goto naz;

  case FK7: //Ввести запись услуги
        
    if(kodiz == 0)
     goto naz1;
//      vniz();
    else
     {
      kodmv.new_plus("");
      vspec(kodiz,0,naimiz,0,&kodmv,1);
     }
    goto naz1;

  case FK8:       //Списать
    if(kodiz == 0)
     spiz(*kodizv);    
    goto naz;

  case FK9:       /*Расчёт*/
    if(kodiz == 0)
     specras(*kodizv);    
    goto naz;
    
  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }

}

/*********************/
/*Ввод нового изделия*/
/*********************/
/*Если вернули 0 ввели новое изделие
               1 не ввели новое изделие
*/

int	vniz()
{
class iceb_tu_str kodiz("");
int		i,skl;
class iceb_tu_str naim("");
int		kodm,kgrm;
class iceb_tu_str bros("");
char		strsql[512];
SQL_str         row;
SQLCURSOR       cur1;
class iceb_tu_str kod("");


helstr(LINES-1,0,"Enter",gettext("материалы"),
"F10",gettext("выход"),"  ",
gettext(" + перед числом - поиск по числу в наименовании"),NULL);

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование изделия"));

if((i=vvod1(&kodiz,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return(1);
if(i == ESC)
  return(1);

GDITE();
kgrm=0;

kodm=kodiz.ravno_atoi();

if(kodiz.ravno()[0] == '\0' || isdigit(kodiz.ravno()[0]) == 0)
 {
  clear();

  bros.new_plus("");
  if(polen(kodiz.ravno(),&bros,2,'+') != 0)
    bros.new_plus(kodiz.ravno());
  kodm=0;
  if(isdigit(bros.ravno()[0]) != 0)
     kodm=bros.ravno_atoi();
  naim.new_plus(kodiz.ravno());
  if(kodm != 0)
    naim.new_plus(kodm);
  skl=0;
  if(dirmatr(&kgrm,&kodm,&naim,1,skl,"",0,0) != 0)
   {
    return(1);
   }

 }

//Проверяем может изделие уже введено
sprintf(strsql,"select kodi from Specif where kodi=%d limit 1",kodm);
if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
 {
  sprintf(strsql,gettext("Это изделие уже введено !"));
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(strsql,"select * from Material \
where kodm=%d",kodm);

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код изделия"),kodm);
  iceb_t_soob(strsql);

  return(1);
 }

kodm=atoi(row[0]);
naim.new_plus(row[2]);
kgrm=atoi(row[1]);
clear();
return(vspec(kodm,0,naim.ravno(),0,&kod,0));

}

/**********************************************/
/*Ввод и корректировка записи в списке изделия*/
/**********************************************/
/*Если вернули 0- запись была сделана
               1- запись не сделана
*/
               
int vspec(int kodiz, //Код изделия
int kodizv, //Код входящей детали
const char *naimiz,  //Наименование изделия
int regim,  //0-ввод новой записи 1-корректировка
class iceb_tu_str *kodmv, //Код введенного материалла
int metka_zap) //0-материал 1- услуга
{
int		K,N;
char		strsql[512];
SQL_str         row;
SQLCURSOR       cur;
double		kolih;
time_t		vrem;
int		koddz;
class iceb_tu_str naimat("");
int		kodm,kgrm;
int		skl=0;
int             kto=0;

if(regim == 1 && kodiz == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка изделия невозможна !"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Можно корректировать только записи внутри изделия !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }


VVOD VV(0);

sozvspec(&VV); //Создаем до чтения, чтобы сохранились данные



if(regim == 1)
 {

  sprintf(strsql,"select * from Specif where kodi=%d and kodd=%d",
  kodiz,kodizv);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки !"));
    return(1);
   }

  kto=atoi(row[4]);
  vrem=atol(row[5]);    

  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  kolih=atof(row[2]);    
  if(kolih < 0.0001)
   {
    sprintf(strsql,"%.10f",kolih);
    VV.VVOD_SPISOK_zapis_data(1,strsql);
   }
  else
   {
    sprintf(strsql,"%.10g",kolih);
    VV.VVOD_SPISOK_zapis_data(1,strsql);
   }    
  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  VV.VVOD_SPISOK_zapis_data(3,row[7]);
  metka_zap=atoi(row[6]);  
 }




K=N=0;


koddz=atoi(VV.VVOD_SPISOK_return_data(0));
naimat.new_plus("");
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
 {
  if(metka_zap == 0)
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",VV.VVOD_SPISOK_return_data(0));
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),VV.VVOD_SPISOK_return_data(0));
      iceb_t_soob(strsql);
     }  
    else
     naimat.new_plus(row[0]);
   }
  if(metka_zap == 1)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",VV.VVOD_SPISOK_return_data(0));
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),VV.VVOD_SPISOK_return_data(0));
      iceb_t_soob(strsql);
     }  
    else
     naimat.new_plus(row[0]);
   }
 }

if(metka_zap == 0)
 VV.VVOD_SPISOK_add_MD(gettext("Код детали........."));
if(metka_zap == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Код услуги........."));
VV.VVOD_SPISOK_add_MD(gettext("Количество........."));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........."));
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения.."));

naz:;

//clear();
VV.VVOD_delete_ZAG();
if(regim == 0)
  VV.VVOD_SPISOK_add_ZAG("Ввод новой записи");
if(regim == 1)
  VV.VVOD_SPISOK_add_ZAG("Корректировка записи");

sprintf(strsql,"Изделие:%d %s",kodiz,naimiz);
VV.VVOD_SPISOK_add_ZAG(strsql);

if(naimat.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s","Деталь",naimat.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(regim == 1)
  kzvz(kto,vrem,&VV);

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("материалы"),
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

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(N == 0 && K == TAB) /*Код материалла*/
   {
    VVOD VVOD1(2);
    VVOD1.VVOD_SPISOK_add_MD("Введите наименование материалла");
    int kom1=0;    
    class iceb_tu_str naimmat("");
    if((kom1=vvod1(&naimmat,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;

    if(dirmatr(&kgrm,&kodm,&naimmat,1,skl,"",0,0) == 0)
     {
      VV.data_plus(0,kodm);
      naimat.new_plus(naimmat.ravno());

      sprintf(strsql,"select ei from Material where kodm=%d",kodm);
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
         VV.VVOD_SPISOK_zapis_data(3,row[0]);

      N++;
     }
    goto naz;
   }

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

     
/************Чтение строки*************/
//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Код материалла*/
     {
      if(kodiz == atoi(VV.VVOD_SPISOK_return_data(0)))
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
        iceb_t_soob("Материал не может входить сам в себя !");
        goto naz;
       }

      if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
       {
        if(metka_zap == 0)
          sprintf(strsql,"select naimat,ei from Material where kodm=%s",VV.VVOD_SPISOK_return_data(0));
        if(metka_zap == 1)
          sprintf(strsql,"select naius,ei from Uslugi where kodus=%s",VV.VVOD_SPISOK_return_data(0));
        if(sql_readkey(&bd,strsql,&row,&cur) != 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));
          attron(VV.VVOD_return_cs(iceb_CB));

          if(metka_zap == 0)
            sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),VV.VVOD_SPISOK_return_data(0));
          if(metka_zap == 1)
            sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),VV.VVOD_SPISOK_return_data(0));

          iceb_t_soob(strsql);

          goto naz;
         }  
        else
         {
          naimat.new_plus(row[0]);
          VV.VVOD_SPISOK_zapis_data(3,row[1]);
          N++;
          goto naz;
         }
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
   if(K == FK1) //Помощь
    {
     GDITE();
   
     iceb_t_pdoc("spec3.txt");
     goto naz;
    }

   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

     if(kodiz == atoi(VV.VVOD_SPISOK_return_data(0)))
      {
       iceb_t_soob(gettext("Материал не может входить сам в себя !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
      {
       VVOD SOOB(1);
       if(metka_zap == 0)
         SOOB.VVOD_SPISOK_add_MD(gettext("Не введён код материалла !"));
       if(metka_zap == 1)
         SOOB.VVOD_SPISOK_add_MD(gettext("Не введён код услуги !"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введено количество !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(3)[0] != '\0')
      {

       sprintf(strsql,"select kod from Edizmer where kod='%s'",VV.VVOD_SPISOK_return_data(3));
       if(sql_readkey(&bd,strsql,&row,&cur) < 1)
        {
         sprintf(strsql,"%s %s !",gettext("Не найдена единица измерения"),VV.VVOD_SPISOK_return_data(3));
         iceb_t_soob(strsql);
         goto naz;
       }
      }

     kodm=(int)ATOFM(VV.VVOD_SPISOK_return_data(0));
     kolih=ATOFM(VV.VVOD_SPISOK_return_data(1));

     kolih=okrug(kolih,0.00000000001);

     time(&vrem);

     if(regim == 0)
      {
       //Проверяем может этот материал уже введен
       sprintf(strsql,"select kodd from Specif where kodi=%d and kodd=%d",
       kodiz,kodm);

       if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
        {
         VVOD SOOB(1);
         if(metka_zap == 0)
           SOOB.VVOD_SPISOK_add_MD(gettext("Этот материал уже введён в изделие !"));
         if(metka_zap == 1)
           SOOB.VVOD_SPISOK_add_MD(gettext("Эта услуга уже введена в изделие !"));
         soobshw(&SOOB,stdscr,-1,-1,0,1);
         goto naz;
        }

       sprintf(strsql,"insert into Specif \
values (%d,%d,%.10g,'%s',%d,%ld,%d,'%s')",
       kodiz,kodm,kolih,VV.VVOD_SPISOK_return_data(2),iceb_t_getuid(),vrem,
       metka_zap,VV.VVOD_SPISOK_return_data(3));
      }
      
     if(regim == 1)     
      sprintf(strsql,"update Specif \
set \
kodd=%d,\
kolih=%.10g,\
koment='%s',\
ei='%s',\
ktoz=%d,\
vrem=%ld \
where kodi=%d and kodd=%d",kodm,kolih,VV.VVOD_SPISOK_return_data(2),\
      VV.VVOD_SPISOK_return_data(3),\
      iceb_t_getuid(),vrem,\
      kodiz,koddz);

//     printw("%s\n",strsql);
     GDITE();
     if(sql_zap(&bd,strsql) != 0)
      {
       msql_error(&bd,gettext("Ошибка записи !"),strsql);
       goto naz;
      }
     kodmv->new_plus(kodm);
     return(0);

    }

   if(K == FK3) /*Просмотр списка материалов*/
    {
     skl=0;
     
     if(metka_zap == 0)
      {
       class iceb_tu_str naim("");
       if(dirmatr(&kgrm,&kodm,&naim,1,skl,"",0,0) == 0)
        {
         VV.data_plus(0,kodm);
         naimat.new_plus(naim.ravno());

         sprintf(strsql,"select ei from Material where kodm=%d",kodm);
         if(sql_readkey(&bd,strsql,&row,&cur) == 1)
            VV.VVOD_SPISOK_zapis_data(3,row[0]);

        }
      }
     if(metka_zap == 1)
      {
       class iceb_tu_str naiusl("");
       if(dirusl(&kgrm,&kodm,&naiusl,1,"") == 0)
        {
         VV.data_plus(0,kodm);
         naimat.new_plus(naiusl.ravno());

         sprintf(strsql,"select ei from Uslugi where kodus=%d",kodm);
         if(sql_readkey(&bd,strsql,&row,&cur) == 1)
            VV.VVOD_SPISOK_zapis_data(3,row[0]);
        }
      }
     goto naz;
    }
 }

}

/********/
/*Шапка */
/********/
void shspec(int kodiz,const char *naim,int urov,int regim,const char *poisk)
{
int		X,Y;
int		smesh=18;
move(0,0);
if(kodiz == 0)
 printw("Список изделий. Уровень:%d",urov);

if(kodiz != 0)
 printw("Изделие: %d %s Уровень:%d",
 kodiz,naim,urov); 

if(regim == 1)
 printw(" Выберите нужное");

move(Y=1,X=COLS-smesh);
printw("Расшифровка полей:");
if(kodiz == 0)
 {
  move(++Y,X);
  printw("1-%s",gettext("код изделия"));

  move(++Y,X);
  printw("2-%s",gettext("наименование"));

  move(1,0);
  printw("    1               2");
 }
else
 {
  move(1,0);
  printw("    1                2                3  4       5       6");
  move(++Y,X);
  printw("1-%s",gettext("код детали"));
  move(++Y,X);
  printw("2-%s",gettext("наименование"));
  move(++Y,X);
  printw("3-%s",gettext("метка изделия"));
  move(++Y,X);

  printw("4-%s",gettext("единица изме-ния"));
  move(++Y,X);

  printw("5-%s",gettext("количество"));
  move(++Y,X);
  printw("6-%s",gettext("комментарий"));
  
  Y++;
  move(++Y,X);
  printw("* %s",gettext("узел"));

  move(++Y,X);
  printw("M %s",gettext("материал"));

  move(++Y,X);
  printw("U %s",gettext("услуга"));
            
 }
if(poisk[0] != '\0')
 {
  attron(A_BLINK);
  move(Y+=2,X);
  printw(gettext("Поиск:"));
  move(++Y,X);
  printw("%.*s",smesh-1,poisk);
  attroff(A_BLINK);
 }
if(kodiz == 0)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
//"F7",gettext("запись"),
"F8",gettext("спис."),
"F9",gettext("расчёт"),
"F10",gettext("выход"),NULL);
else
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F7",gettext("запись"),
"F10",gettext("выход"),NULL);

}

/************************************************/
/*Получение наименования и проверка на совпадение*/
/************************************************/
/*Если вернули 0 подходит
	       1 не подходит
*/
int specprov(char *kodiz,class iceb_tu_str *naim,const char *poisk,int metka_naim) //0-если поиск не заказан выход 1-если поиск не заказан читаем наименование и выходим
{
SQL_str         row1;
SQLCURSOR       cur1;
char		strsql[512];

if(poisk[0] == '\0'  && metka_naim == 0)
 return(0);
 
naim->new_plus("");
for(int metka_zap=0 ; metka_zap < 2; metka_zap++)
 {
  if(metka_zap == 0)
    sprintf(strsql,"select naimat from Material where kodm=%s",kodiz);
  if(metka_zap == 1)
    sprintf(strsql,"select naius from Uslugi where kodus=%s",kodiz);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(metka_zap == 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код записи"),kodiz);
      iceb_t_soob(strsql);
     }
   }  
  else
   {
    naim->new_plus(row1[0]);
    break;
   }
 }
if(poisk[0] == '\0')
 return(0);

if(strstrm(naim->ravno(),poisk) == 0)
  return(1);

return(0);

}

/***************************************************/
void	sozvspec(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(40);
VV->VVOD_SPISOK_add_data(40);
}

