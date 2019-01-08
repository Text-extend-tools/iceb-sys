/*$Id: dirpodrusl.c,v 5.4 2013/05/17 14:55:58 sasa Exp $*/
/*13.07.2015	06.10.2011	Белых А.И.	dirpodrusl.c
Ввод и корректирвка списка подразделений в учёте услуг
*/
#include        <errno.h>
#include        "buhg.h"

class dirpodrusl_poi
 {
  public:
   int metka_sort;    //0-сортировка по возростанию номера склада 1-по наименованию
   class iceb_tu_str naimpoi;  
  dirpodrusl_poi()
   {
    metka_sort=0;    //0-сортировка по возростанию номера склада 1-по наименованию
    naimpoi.plus("");
   }
 };
 
void dirpodrusl_vvod(const char *kod_podr);
void dirpodrusl_sap(int,class dirpodrusl_poi*);
int  dirpodrusl_prov(const char*,int);
void dirpodrusl_ras(SQLCURSOR*);
void dirpodrusl_ud_neisp();
void dirpodrusl_korkod(const char *nkod,const char *skod);



int dirpodrusl(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_podr,class iceb_tu_str *naim_skl)
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
class dirpodrusl_poi poisk;

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


sprintf(strsql,"select * from Uslpodr");

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
  dirpodrusl_sap(metkar,&poisk);
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

  if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[1],poisk.naimpoi.ravno()) == 0)
   continue;
  
  sprintf(strsql,"%-3s|%-*.*s|%-*.*s|%.*s",
  row[0],
  iceb_tu_kolbait(20,row[1]),iceb_tu_kolbait(20,row[1]),row[1],
  iceb_tu_kolbait(15,row[5]),iceb_tu_kolbait(15,row[5]),row[5],
  iceb_tu_kolbait(15,row[4]),row[4]);
  

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

dirpodrusl_sap(metkar,&poisk);

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
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),kod_podr,1,'|');
  polen(spis_menu.spis_str.ravno(kom),naim_skl,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_podr->new_plus("");
     naim_skl->new_plus("");
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("usl3_5.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    dirpodrusl_vvod(kod_podr->ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirpodrusl_vvod("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(dirpodrusl_prov(kod_podr->ravno(),1) != 0)
        goto naz;
      sprintf(strsql,"delete from Uslpodr where kod=%s",kod_podr->ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3:       //Удалить неиспользуемые подарзделения
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить неиспользуемые записи ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    dirpodrusl_ud_neisp();
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
    dirpodrusl_ras(&cur);
    goto naz;

  case FK6:    //Переключение сортировки
    poisk.metka_sort++;
    if(poisk.metka_sort > 1)
     poisk.metka_sort=0;
    goto naz1;

  default:
    goto naz;
    break;
 }

return(1);


}
/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirpodrusl_zap(VVOD *VV,const char *kod_podr)
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


if(SRAV(kod_podr,VV->data_ravno(0),0) != 0)
 {
  //Проверяем может такое подразделение уже введено
  sprintf(strsql,"select kod from Uslpodr where kod=%s",VV->data_ravno(0));
  if(readkey(strsql) >= 1)
   {
    iceb_t_soob(gettext("Такой код подразделения уже ввели !"));
    return(1);
   }

  if(kod_podr[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }
 
time_t vrem;
time(&vrem);

if(kod_podr[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uslpodr values(%s,'%s',%d,%ld,'%s','%s','%s')",
  VV->VVOD_SPISOK_return_data(0),  
  VV->VVOD_SPISOK_return_data(1),  
  iceb_t_getuid(),vrem,
  VV->data_ravno(2),
  VV->data_ravno(3),
  VV->data_ravno(4));
else
  sprintf(strsql,"update Uslpodr set \
kod=%s,\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
np='%s',\
fo='%s',\
dol='%s' \
where kod=%s",  
  VV->data_ravno(0),
  VV->data_ravno(1),  
  iceb_t_getuid(),
  vrem,
  VV->data_ravno(2),
  VV->data_ravno(3),
  VV->data_ravno(4),
  kod_podr);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kod_podr[0] != '\0' && SRAV(kod_podr,VV->VVOD_SPISOK_return_data(0),0) != 0)
  dirpodrusl_korkod(VV->data_ravno(0),kod_podr);


return(0);


}

/**********************/
/*Ввод и корректировки*/
/**********************/

void dirpodrusl_vvod(const char *kod_podr)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
struct  tm      *bf;
int		N=1,K=0;

SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(60);//1
VV.VVOD_SPISOK_add_data(255);//2
VV.VVOD_SPISOK_add_data(128);//3
VV.VVOD_SPISOK_add_data(255);//4


if(kod_podr[0] != '\0')
 {

  sprintf(strsql,"select * from Uslpodr where kod=%s",kod_podr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  ktozap=atoi(row[2]);
  vremz=atol(row[3]);
  VV.data_plus(2,row[4]);   
  VV.data_plus(3,row[5]);   
  VV.data_plus(4,row[6]);   
     
 }
else
 {
  sprintf(strsql,"%d",oprsk("Uslpodr"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }



VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование подразделения.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Населённый пункт............"));//2
VV.VVOD_SPISOK_add_MD(gettext("Фамилия ответственного......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Должность ответственного...."));//4


naz:;

VV.VVOD_delete_ZAG();

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка подразделения"));

if(ktozap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(iceb_t_kszap(ktozap));
 }

if(vremz != 0)
 {
  bf=localtime(&vremz);
  sprintf(strsql,"%s: %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
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
    if(dirpodrusl_zap(&VV,kod_podr) != 0)
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



/**************************/
/*Шапка                   */
/**************************/

void	dirpodrusl_sap(int metkar,class dirpodrusl_poi *poisk)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка подразделений"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }
mvprintw(2,0,"   1           2                  3               4");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код подразделения"));
mvprintw(y++,x,"2-%s",gettext("наименование"));
mvprintw(y++,x,"3-%s",gettext("фамилия ответств."));
mvprintw(y++,x,"4-%s",gettext("населённый пункт"));

if(poisk->naimpoi.ravno()[0] != '\0')
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",poisk->naimpoi.ravno());
  attroff(A_BLINK);
  
 }

if(metkar < 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("сорт."),
//"F7",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/********************/
/*Распечатать       */
/********************/

void	dirpodrusl_ras(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"uslpodr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список подразделений"),0,0,0,0,0,0,ff);


fprintf(ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff,"\
Код | Наименование подразделения   |Населённый пункт                         |\n");

fprintf(ff,"\
------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
   
  fprintf(ff,"%-4s %-*.*s %s\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
  row[4]);
 }
fprintf(ff,"\
------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список подразделений"));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");

}
/************************/
/*Удаление неиспользуемых подразделений*/
/*********************************/
void dirpodrusl_ud_neisp()
{
int kolstr;
char strsql[1024];
SQL_str row;
SQLCURSOR cur;

clear();
GDITE();

sprintf(strsql,"select * from %s","Uslpodr");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%5s %-*s ",row[0],iceb_tu_kolbait(40,row[1]),row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirpodrusl_prov(row[0],0) != 0)
   {
    printw("\n");
    continue;
   }
  sprintf(strsql,"delete from %s where kod=%s","Uslpodr",row[0]);
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

VVOD		SOOB(1);
sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);

}
/***************************/
/*Корректрировка кода в таблицах*/
/*********************************/
void dirpodrusl_korkod(const char *nkod,const char *skod)
{
char strsql[512];
GDITE();

sprintf(strsql,"update Usldokum set podr=%s where podr=%s",nkod,skod);
sql_zapis(strsql,1,0);
sprintf(strsql,"update Usldokum1 set podr=%s where podr=%s",nkod,skod);
sql_zapis(strsql,1,0);
sprintf(strsql,"update Usldokum2 set podr=%s where podr=%s",nkod,skod);
sql_zapis(strsql,1,0);
sprintf(strsql,"update Usldokum3 set podr=%s where podr=%s",nkod,skod);
sql_zapis(strsql,1,0);


}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int dirpodrusl_prov(const char *kod,int metka_s)
{
char	strsql[512];



sprintf(strsql,"select podr from Usldokum where podr=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Usldokum",kod);
  return(1);
 }
sprintf(strsql,"select podr from Usldokum1 where podr=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Usldokum1",kod);
  return(1);
 }

sprintf(strsql,"select podr from Usldokum2 where podr=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Usldokum2",kod);
  return(1);
 }

sprintf(strsql,"select podr from Usldokum3 where podr=%d limit 1",atoi(kod));
if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
    iceb_t_ndelkod("Usldokum3",kod);
  return(1);
 }


return(0);


}
