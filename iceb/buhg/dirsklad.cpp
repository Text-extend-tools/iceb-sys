/*$Id: dirsklad.c,v 5.27 2013/08/13 05:49:47 sasa Exp $*/
/*23.07.2018	24.03.2003	Белых А.И.	dirsklad.c
Ввод и корректирвка списка складов
*/
#include        <errno.h>
#include        "buhg.h"

class dirsklad_poi
 {
  public:
   int metka_sort;    //0-сортировка по возростанию номера склада 1-по наименованию
   int metka_isp_skl; /*0-все склады 1-только используемые 2-только неиспользуемые*/
   class iceb_tu_str naimpoi;  
  dirsklad_poi()
   {
    metka_sort=0;    //0-сортировка по возростанию номера склада 1-по наименованию
    metka_isp_skl=0; //0-все склады 1-только используемые 2-только неиспользуемые
    naimpoi.plus("");
   }
 };
 
void dirsklad_vvod(const char *kod_skl);
void dirsklad_sap(int,class dirsklad_poi*);
int  dirsklad_zap(VVOD*,const char*,int);
int  dirsklad_prov(const char*,int);
void	dirsklad_ras(SQLCURSOR *cur,class dirsklad_poi *poisk);
void dirsklad_ud_neisp();
void dirsklad_korkod(const char *nkod,const char *skod);



int dirsklad(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_skl,class iceb_tu_str *naim_skl)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
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
class dirsklad_poi poisk;

//memset(koment,'\0',sizeof(koment));
poisk.naimpoi.new_plus(naim_skl->ravno());

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


sprintf(strsql,"select * from Sklad");

if(poisk.metka_isp_skl == 1)
 strcat(strsql," where mi=1");
if(poisk.metka_isp_skl == 2)
 strcat(strsql," where mi=0");

if(poisk.metka_sort == 0)
 strcat(strsql," order by kod asc");
if(poisk.metka_sort == 1)
 strcat(strsql," order by naik asc");
  
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirsklad_sap(metkar,&poisk);
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
char metka_isp[5];

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[1],poisk.naimpoi.ravno()) == 0)
   continue;
  memset(metka_isp,'\0',sizeof(metka_isp));
  if(row[6][0] == '1')
   strcpy(metka_isp,"*");
  
  sprintf(strsql,"%-3s|%-*.*s|%-*.*s|%-*.*s|%s",
  row[0],
  iceb_tu_kolbait(20,row[1]),iceb_tu_kolbait(20,row[1]),row[1],
  iceb_tu_kolbait(10,row[2]),iceb_tu_kolbait(10,row[2]),row[2],
  iceb_tu_kolbait(10,row[3]),iceb_tu_kolbait(10,row[3]),row[3],
  metka_isp);
  

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

dirsklad_sap(metkar,&poisk);

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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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

  polen(str_vib.ravno(),kod_skl,1,'|');
  polen(str_vib.ravno(),naim_skl,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_skl->new_plus("");
     naim_skl->new_plus("");
     return(1);

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    dirsklad_vvod(kod_skl->ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirsklad_vvod("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(dirsklad_prov(kod_skl->ravno(),1) != 0)
        goto naz;
      sprintf(strsql,"delete from Sklad where kod=%s",kod_skl->ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3:       //Удалить неиспользуемые склады
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить неиспользуемые записи ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    dirsklad_ud_neisp();
    goto naz1;
    

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     poisk.naimpoi.new_plus("");
     
     if(vvod1(&poisk.naimpoi,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      goto naz;

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirsklad_ras(&cur,&poisk);
    goto naz;

  case FK6:    //Переключение сортировки
    poisk.metka_sort++;
    if(poisk.metka_sort > 1)
     poisk.metka_sort=0;
    goto naz1;

  case FK7:    //Переключение просмотра используемых/неиспользуемых складов
    poisk.metka_isp_skl++;
    if(poisk.metka_isp_skl > 2)
     poisk.metka_isp_skl=0;
    goto naz1;

  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirsklad_vvod(const char *kod_skl)
{
char		strsql[2048];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=1,K=0;
int metka_isp_skl=0; //0-используется 1-нет

SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(124);//1
VV.VVOD_SPISOK_add_data(255);//2
VV.VVOD_SPISOK_add_data(124);//3
VV.VVOD_SPISOK_add_data(30);//4
VV.VVOD_SPISOK_add_data(255);//5



if(kod_skl[0] != '\0')
 {

  sprintf(strsql,"select * from Sklad where kod=%s",kod_skl);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);
  ktozap=atoi(row[4]);
  vremz=atol(row[5]);
  metka_isp_skl=atoi(row[6]);
  VV.data_plus(5,row[7]);   

  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка склада"));
  kzvz(ktozap,vremz,&VV);
     
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод нового склада"));
  sprintf(strsql,"%d",oprsk("Sklad"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }



VV.VVOD_SPISOK_add_MD(gettext("Код склада.............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование склада....."));//1
VV.VVOD_SPISOK_add_MD(gettext("Фамилия ответственного.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Должность..............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Состояние..............."));//4
VV.VVOD_SPISOK_add_MD(gettext("Населённый пункт........"));//5


naz:;

if(metka_isp_skl == 0)
  VV.VVOD_SPISOK_zapis_data(4,gettext("Используется"));
if(metka_isp_skl == 1)
  VV.VVOD_SPISOK_zapis_data(4,gettext("Неиспользуется"));


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
//"F3",gettext("реквизиты"),
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

    if(N == 4) //Состояние склада
     {
      VVOD MENUW(3);
      MENUW.VVOD_SPISOK_add_MD(gettext("Используеться"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Неиспользуется"));
     
      MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите состояние склада"));
      menu3w(stdscr,&MENUW,&metka_isp_skl,-1,-1,0);
      goto naz;
            
     }


//    my_getstr(VV.VVOD_SPISOK_return_data(N),dlinastr,VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
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
    if(dirsklad_zap(&VV,kod_skl,metka_isp_skl) != 0)
     goto naz;
    break;
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

int   dirsklad_zap(VVOD *VV,const char *kod_skl,int metka_isp_skl)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       


if(SRAV(kod_skl,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такой склад уже введён
  sprintf(strsql,"select kod from Sklad where kod=%s",VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код склада уже ввели !"));
    return(1);
   }

  if(kod_skl[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kod_skl[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Sklad values(%s,'%s','%s','%s',%d,%ld,%d,'%s')",
  VV->VVOD_SPISOK_return_data(0),  
  VV->data_ravno_filtr(1),  
  VV->data_ravno_filtr(2),
  VV->data_ravno_filtr(3),
  iceb_t_getuid(),vrem,
  metka_isp_skl,
  VV->data_ravno_filtr(5));
else
  sprintf(strsql,"update Sklad set \
kod=%s,\
naik='%s',\
fmol='%s',\
dolg='%s',\
ktoz=%d,\
vrem=%ld,\
mi=%d,\
np='%s' \
where kod=%s",  
  VV->data_ravno(0),
  VV->data_ravno_filtr(1),  
  VV->data_ravno_filtr(2),
  VV->data_ravno_filtr(3),
  iceb_t_getuid(),
  vrem,
  metka_isp_skl,
  VV->data_ravno_filtr(5),
  kod_skl);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kod_skl[0] != '\0' && SRAV(kod_skl,VV->VVOD_SPISOK_return_data(0),0) != 0)
  dirsklad_korkod(VV->data_ravno(0),kod_skl);


return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirsklad_sap(int metkar,class dirsklad_poi *poisk)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка складов"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {
  if(poisk->metka_isp_skl == 2)
    mvprintw(1,0,gettext("Только используемые склады"));
  if(poisk->metka_isp_skl == 1)
    mvprintw(1,0,gettext("Только неиспользуемые склады"));
                         
  mvprintw(2,0,"   1           2                3          4      5");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код склада"));
  mvprintw(y++,x,"2-%s",gettext("наименование"));
  mvprintw(y++,x,"3-%s",gettext("фамилия"));
  mvprintw(y++,x,"4-%s",gettext("должность"));
  mvprintw(y++,x,"5-%s",gettext("метка"));

  if(poisk->naimpoi.ravno()[0] != '\0')
   {
    y++;
    attron(A_BLINK);
    mvprintw(y++,x,gettext("Поиск:"));
    mvprintw(y++,x,"%s",poisk->naimpoi.ravno());
    attroff(A_BLINK);
    
   }

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("удалить"),
  "F4",gettext("поиск"),
  "F5",gettext("печать"),
  "F6",gettext("сорт."),
  "F7",gettext("просмотр"),
  "F10",gettext("выход"),NULL);
 } 

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/********************/
/*Распечатать       */
/********************/

void	dirsklad_ras(SQLCURSOR *cur,class dirsklad_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"komt%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список складов"),ff);


fprintf(ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код | Наименование склада          |Фамилия  мат.ответс.|  Должность         |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------\n");

cur->poz_cursor(0);
char metka_isp_skl[5];

while(cur->read_cursor(&row) != 0)
 {
  if(poisk->naimpoi.ravno()[0] != '\0' && strstrm(row[1],poisk->naimpoi.ravno()) == 0)
   continue;


  memset(metka_isp_skl,'\0',sizeof(metka_isp_skl));
  if(row[6][0] == '1')
   strcpy(metka_isp_skl,"*");
   
  fprintf(ff,"%-4s %-*.*s %-*.*s %-*.*s %s\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
  iceb_tu_kolbait(20,row[2]),iceb_tu_kolbait(20,row[2]),row[2],
  iceb_tu_kolbait(20,row[3]),iceb_tu_kolbait(20,row[3]),row[3],
  metka_isp_skl);
 }
fprintf(ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff,"%s\n",gettext("\"*\" - отмемечены неиспользуемые склады"));

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список складов"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/************************/
/*Удаление неиспользуемых складов*/
/*********************************/
void dirsklad_ud_neisp()
{
int kolstr;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

clear();
GDITE();

sprintf(strsql,"select * from %s","Sklad");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%6s %-*s ",row[0],iceb_tu_kolbait(40,row[1]),row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirsklad_prov(row[0],0) != 0)
   {
    printw("\n");
    continue;
   }
  sprintf(strsql,"delete from %s where kod=%s","Sklad",row[0]);
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
iceb_t_soob(strsql);

}
/***************************/
/*Корректрировка кода в таблицах*/
/*********************************/
void dirsklad_korkod(const char *nkod,const char *skod)
{
char strsql[512];
GDITE();
sprintf(strsql,"update Roznica set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat1 set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat2 set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat3 set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Kart set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Zkart set sklad=%s where sklad=%s",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Uplouot set skl=%s where skl=%s",nkod,skod);
sql_zapis(strsql,1,0);

}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int dirsklad_prov(const char *kod,int metka_s)
{
char	strsql[512];


sprintf(strsql,"select sklad from Roznica where sklad=%s limit 1",kod);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Roznica",kod);
  return(1);
 }




sprintf(strsql,"select sklad from Kart where sklad=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Kart",kod);
  return(1);
 }

sprintf(strsql,"select sklad from Dokummat where sklad=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Dokummat",kod);
  return(1);
 }

sprintf(strsql,"select skl from Uplouot where skl=%s limit 1",kod);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Uplout",kod);
  return(1);
 }



return(0);


}
