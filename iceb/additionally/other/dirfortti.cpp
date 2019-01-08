/*$Id:$*/
/*01.03.2017	01.03.2017	Белых А.И.	dirgortti.c
Ввод и корректирвка трудоёмкости изготовления деталей по цехам
*/
#include        <errno.h>
#include        <buhg.h>
#include "fort.h"

class dirgortti_poi
 {
  public:
   class iceb_tu_str kod_ceh;
   class iceb_tu_str kod_det;
   class iceb_tu_str naim_det;
   class iceb_tu_str naimpoi;  
   int metka_poi;
  dirgortti_poi()
   {
    naimpoi.plus("");
    kod_ceh.plus("");
    kod_det.plus("");
    naim_det.plus("");
    metka_poi=0;
   }
 };
 
void dirgortti_vvod(const char *kod_skl,const char *kod_det);
void dirgortti_sap(class dirgortti_poi*);
int dirgortti_zap(VVOD *VV,const char *kod_ceh,const char *kod_det);
void	dirgortti_ras(SQLCURSOR *cur,class dirgortti_poi *poisk);
int dirfortti_prov(SQL_str row,class dirgortti_poi *poisk);
int dirfortti_poi(class dirgortti_poi *poisk);



int dirgortti(const char *kod_ceha)
{
class iceb_tu_str kod_ceh("");
class iceb_tu_str kod_det("");
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class dirgortti_poi poisk;
class iceb_tu_str naim_det("");

poisk.kod_ceh.new_plus(kod_ceha);
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


sprintf(strsql,"select * from Fortti");

  
class SQLCURSOR cur,cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirgortti_sap(&poisk);
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

  if(dirfortti_prov(row,&poisk) != 0)
   continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");
  
  sprintf(strsql,"%-*s|%-6s|%-*.*s|%10.2f|%10.2f",
  iceb_tu_kolbait(6,row[0]),
  row[0],
  row[1],
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(), 
  atof(row[2]),
  atof(row[3]));  

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

dirgortti_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


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
    if(dirfortti_prov(row,&poisk) != 0)
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
    if(dirfortti_prov(row,&poisk) != 0)
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
    if(dirfortti_prov(row,&poisk) != 0)
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

  polen(str_vib.ravno(),&kod_ceh,1,'|');
  polen(str_vib.ravno(),&kod_det,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
      
  case SFK2:   /*Корректировать*/
    dirgortti_vvod(kod_ceh.ravno(),kod_det.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirgortti_vvod("","");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Fortti where kod='%s' and kd=%d",kod_ceh.ravno(),kod_det.ravno_atoi());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

    

  case FK4:       /*Поиск*/
     dirfortti_poi(&poisk);
     GDITE();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    dirgortti_ras(&cur,&poisk);
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

void dirgortti_vvod(const char *kod_ceh,const char *kod_det)
{
char		strsql[2048];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class iceb_tu_str naim_ceh("");
class iceb_tu_str naim_det("");
class iceb_tu_str kod("");
int kol=0;
SQLCURSOR cur;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(10);//3



if(kod_ceh[0] != '\0')
 {

  sprintf(strsql,"select * from Fortti where ceh='%s' and kd=%d",kod_ceh,atoi(kod_det));
  if((kol=readkey(strsql,&row,&cur)) != 1)
   {
    printw("\n%s-%s kol=%d\n",__FUNCTION__,strsql,kol);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);

  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);
  ktozap=atoi(row[4]);
  vremz=atol(row[5]);

     
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 }



VV.VVOD_SPISOK_add_MD(gettext("Код цеха.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код детали............"));//1
VV.VVOD_SPISOK_add_MD(gettext("Время в минутах......."));//2
VV.VVOD_SPISOK_add_MD(gettext("Стоимость в копейках.."));//3


naz:;


VV.VVOD_delete_ZAG();

if(kod_ceh[0] != '\0')
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktozap,vremz,&VV);
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 }

naim_ceh.new_plus("");
naim_det.new_plus("");
if(VV.data_ravno(0)[0] != '\0')
 {
  sprintf(strsql,"select naik from Fortsc where kod='%s'",VV.data_ravno(0));
  if(readkey(strsql,&row,&cur) == 1)
   naim_ceh.new_plus(row[0]);
  
  sprintf(strsql,"%s:%s",gettext("Цех"),naim_ceh.ravno());
  
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }
if(VV.data_ravno(1)[0] != '\0')
 {
  sprintf(strsql,"select naimat from Material where kodm=%d",VV.data_ravno_atoi(1));
  if(readkey(strsql,&row,&cur) == 1)
   naim_det.new_plus(row[0]);

  sprintf(strsql,"%s:%s",gettext("Деталь"),naim_det.ravno());
  
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

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
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 4) //Состояние склада
     {
            
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      N++;
      goto naz;
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
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
    if(dirgortti_zap(&VV,kod_ceh,kod_det) != 0)
     goto naz;
    break;
   }

  if(K == FK3) /*реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    class VVOD MENU(3);

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка цехов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка деталей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;
      
      case 0:
       naim_ceh.new_plus("");
       if(dirceh(1,&kod,&naim_ceh) == 0)
        {
         VV.data_plus(0,kod.ravno());
        }
       clear();
       goto naz;
       
      case 1 :
        naim_det.new_plus("");
        if(dirmatr2(0,&kod,&naim_det) == 0)
         {
          VV.data_plus(1,kod.ravno());
         }
        clear();
        goto naz;
     }
    goto naz;

    clear();
    goto naz;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int dirgortti_zap(VVOD *VV,const char *kod_ceh,const char *kod_det)
{
char	strsql[2048];
SQL_str row;
class SQLCURSOR cur;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

/*проверяем код цеха*/
sprintf(strsql,"select kod from Fortsc where kod=%d",VV->data_ravno_atoi(0));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код склада"),VV->data_ravno_atoi(0));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select kodm from Material where kodm=%d",VV->data_ravno_atoi(1));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код детали"),VV->data_ravno_atoi(1));
  iceb_t_soob(strsql);
  return(1);
 }
 
if(SRAV(kod_ceh,VV->data_ravno(0),0) != 0 || atoi(kod_det) != VV->data_ravno_atoi(1))
 {
  //Проверяем может такая запись уже есть
  sprintf(strsql,"select ceh from Fortti where ceh='%s' and kd=%d",VV->data_ravno(0),VV->data_ravno_atoi(1));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    return(1);
   }

 }
 

if(kod_ceh[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Fortti values('%s',%d,%.2f,%.2f,%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  VV->data_ravno_atoi(1),  
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(3),
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Fortti set \
ceh='%s',\
kd=%d,\
min=%.2f,\
kop=%.2f,\
ktoz=%d,\
vrem=%ld \
where ceh='%s' and kd=%d",  
  VV->VVOD_SPISOK_return_data(0),  
  VV->data_ravno_atoi(1),  
  VV->data_ravno_atof(2),
  VV->data_ravno_atof(3),
  iceb_t_getuid(),
  time(NULL),
  kod_ceh,atoi(kod_det));


if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirgortti_sap(class dirgortti_poi *poisk)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка трудоёмкости изготовления"));

mvprintw(2,0,"     1      2                 3                    4         5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код цеха"));
mvprintw(y++,x,"2-%s",gettext("код детали"));
mvprintw(y++,x,"3-%s",gettext("наим.дет."));
mvprintw(y++,x,"4-%s",gettext("минуты"));
mvprintw(y++,x,"5-%s",gettext("копейки"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));

  if(poisk->kod_ceh.getdlinna() > 1)
    mvprintw(y++,x,"%s-%s",gettext("Код цех."),poisk->kod_ceh.ravno());
  if(poisk->kod_det.getdlinna() > 1)
    mvprintw(y++,x,"%s-%s",gettext("Код дет."),poisk->kod_det.ravno());
  if(poisk->naim_det.getdlinna() > 1)
    mvprintw(y++,x,"%s-%s",gettext("Наим. дет."),poisk->naim_det.ravno());


  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);


}

/********************/
/*Распечатать       */
/********************/

void	dirgortti_ras(SQLCURSOR *cur,class dirgortti_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
class iceb_tu_str naim_det("");
char strsql[1024];

sprintf(imaf,"ti%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Справочник трудоёмкости изготовления"),ff);
if(poisk->kod_ceh.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код цеха"),poisk->kod_ceh.ravno());
if(poisk->kod_det.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код детали"),poisk->kod_det.ravno());
if(poisk->naim_det.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Наименование детали"),poisk->naim_det.ravno());


fprintf(ff,"\
----------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Цех  |Код д.|   Наименование детали        |Минуты| Копейки  |\n"));
fprintf(ff,"\
----------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(dirfortti_prov(row,poisk) != 0)
     continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");
   
  fprintf(ff,"%*s %-6s %-*.*s %6.2f %10.2f\n",
  iceb_tu_kolbait(7,row[0]),
  row[0],
  row[1],
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  atof(row[2]),
  atof(row[3]));  
 }
fprintf(ff,"\
----------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник трудоёмкости изготовления"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}



/*********************************/
/*ввод реквизитов поиска*/
/******************************/
int dirfortti_poi(class dirgortti_poi *poisk)
{
class VVOD MENU(3);
class VVOD VV(0);
int kom1=0;
int kl=0;
class iceb_tu_str naim("");
class iceb_tu_str kod("");
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Код цеха.............(,,).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код детали...........(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Наименование детали..(,,).."));//2

VV.VVOD_SPISOK_add_data(poisk->kod_ceh.ravno(),60);
VV.VVOD_SPISOK_add_data(poisk->kod_det.ravno(),60);
VV.VVOD_SPISOK_add_data(poisk->naim_det.ravno(),128);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kl=VV.vvod(0,1,1,-1,-1);

poisk->kod_ceh.new_plus(VV.data_ravno(0));
poisk->kod_det.new_plus(VV.data_ravno(1));
poisk->naim_det.new_plus(VV.data_ravno(2));

switch(kl)
 {
  case FK10:
  case ESC:
    poisk->metka_poi=0; 
    break;

  case FK2:
    poisk->metka_poi=1; 
    break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка цехов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка деталей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;
      
      case 0:
       if(dirceh(1,&kod,&naim) == 0)
        {
         VV.data_z_plus(0,kod.ravno());
        }
       clear();
       goto naz;
       
      case 1 :
        naim.new_plus("");
        if(dirmatr2(0,&kod,&naim) == 0)
         {
          VV.data_z_plus(1,kod.ravno());
         }
        clear();
        goto naz;
     }
    goto naz;
    
  case FK4:
    VV.VVOD_clear_data();
    goto naz;
               
  default:
    goto naz;
 }

return(poisk->metka_poi);
}

/******************************/
/*проверка реквизитов поиска*/
/******************************/
int dirfortti_prov(SQL_str row, //Строка для проверки 
class dirgortti_poi *poisk)
{
if(poisk->metka_poi == 0)
 return(0);

if(proverka(poisk->kod_ceh.ravno(),row[0],0,0) != 0)
 return(1);

if(proverka(poisk->kod_det.ravno(),row[1],0,0) != 0)
 return(1);

if(poisk->naim_det.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  
  char strsql[512];
  class iceb_tu_str naim_det("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  if(proverka(poisk->naim_det.ravno(),naim_det.ravno(),4,0) != 0)
    return(1);

 } 
return(0);
}
