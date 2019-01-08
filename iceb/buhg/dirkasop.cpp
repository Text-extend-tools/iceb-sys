/*$Id: dirkasop.c,v 5.1 2013/09/26 09:46:17 sasa Exp $*/
/*08.03.2018	06.09.2013	Белых А.И.	dirkasop.c
Ввод и корректировка списка операций для кассовых ордеров
*/
#include        <errno.h>
#include        "buhg.h"

void  dirkasop_kod(int tipop,const char *kods,const char *kodn);
void	dirkasop_ras(SQLCURSOR *cur,int tipop);
int dirkasop_pu(int tipop,const char *kodavt);
void dirkasop_sap(int tipop,int metkar,const char *naimpoi);
void dirkasop_vvod(const char *kodm,int tipop,const char *imatab);
int   dirkasop_zap(VVOD *VV,const char *kodm,int metka_prov,int tipop,const char *imatab);



int dirkasop(int tipop, /*1-приходы 2-расходы*/
int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kodm,class iceb_tu_str *naim)
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
class iceb_tu_str naimpoi("");
class iceb_tu_str kodvib("");
class iceb_tu_str naimvib("");
class iceb_tu_str prov_text("");
class iceb_tu_str imatab("Kasop1");
if(tipop < 1 || tipop > 2)
 {
  sprintf(strsql,"%s tipop!",gettext("Не правильно задан"));
  iceb_t_soob(strsql);
  return(1);
 }
if(tipop == 2)
 imatab.new_plus("Kasop2");
 
SQLCURSOR curr;
naimpoi.new_plus(naim->ravno());

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select * from %s",imatab.ravno());


class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirkasop_sap(tipop,metkar,naimpoi.ravno());
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

  if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
   continue;
  if(atoi(row[4]) == 0)
   prov_text.new_plus(gettext("Не делать"));
  else
   prov_text.new_plus(gettext("Делать"));

  if(metkar == 2)
    sprintf(strsql,"%-*s|%s",
    iceb_tu_kolbait(4,row[0]),
    row[0],
    row[1]);
  else  
    sprintf(strsql,"%-*s|%-*.*s|%-*s|%-*s|%2s|%s",
    iceb_tu_kolbait(4,row[0]),
    row[0],
    iceb_tu_kolbait(30,row[1]),
    iceb_tu_kolbait(30,row[1]),
    row[1],
    iceb_tu_kolbait(6,row[2]),
    row[2],
    iceb_tu_kolbait(6,row[3]),
    row[3],
    row[7],
    prov_text.ravno());
  
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

dirkasop_sap(tipop,metkar,naimpoi.ravno());

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
    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
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
    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
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
    if(naimpoi.getdlinna() > 1 && strstrm(row[1],naimpoi.ravno()) == 0)
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

  polen(str_vib.ravno(),&kodvib,1,'|');
  polen(str_vib.ravno(),&naimvib,2,'|');

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
    if(tipop == 1)
      iceb_t_pdoc("kas3_2.txt");
    if(tipop == 2)
      iceb_t_pdoc("kas3_3.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
     {
      kodm->new_plus(kodvib.ravno());
      naim->new_plus(naimvib.ravno());
      return(0);
     }      

  case SFK2:   /*Корректировать*/
    dirkasop_vvod(kodvib.ravno(),tipop,imatab.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirkasop_vvod("",tipop,imatab.ravno());
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(dirkasop_pu(tipop,kodvib.ravno()) != 0)
        goto naz;
      sprintf(strsql,"delete from %s where kod='%s'",imatab.ravno(),kodvib.ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     naimpoi.new_plus("");
     
     vvod1(&naimpoi,40,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirkasop_ras(&cur,tipop);
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

void dirkasop_vvod(const char *kodm,int tipop,const char *imatab)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class SQLCURSOR curr;
int metka_prov=0;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(250);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(64);//5


if(kodm[0] != '\0')
 {

  sprintf(strsql,"select * from %s where kod='%s'",imatab,kodm);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,row[0]);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);

  metka_prov=atoi(row[4]);
  
  ktozap=atoi(row[5]);
  vremz=atol(row[6]);

  if(atoi(row[7]) > 0)
   VV.VVOD_SPISOK_zapis_data(4,row[7]);

 }

VV.VVOD_SPISOK_add_MD(gettext("Код операции............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование операции...."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт кассы..............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт корреспондент......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код целевого назначения.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Проводки................."));//5


naz:;

if(metka_prov == 0)
   VV.VVOD_SPISOK_zapis_data(5,gettext("Не делать"));
else
   VV.VVOD_SPISOK_zapis_data(5,gettext("Делать"));

VV.VVOD_delete_ZAG();
if(kodm[0] == '\0')
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));


kzvz(ktozap,vremz,&VV);

 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("счета"),
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
    if(N == 5)
     {
      int kkk=0;
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));
      MENU.VVOD_SPISOK_add_MD(gettext("Не делать"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Делать"));//1
      if(menu3w(stdscr,&MENU,&kkk,-1,-1,metka_prov) != 0)
       continue;

      if(kkk >= 0)
        metka_prov=kkk;

      goto naz;
     }
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
/*
    if(N == 1) 
     {

     }
*/
    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      if(SRAV(kodm,VV.data_ravno(N),0) != 0)
       {      
        sprintf(strsql,"select kod from %s where kod='%s'",imatab,VV.data_ravno(N));
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
    iceb_t_pdoc("kas3_2_1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirkasop_zap(&VV,kodm,metka_prov,tipop,imatab) != 0)
     goto naz;
    break;
   }

  if(K == FK3) //Просмотр единиц измерения
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    class iceb_tu_str shet("");
    class iceb_tu_str naim("");    
    if(dirshet(&shet,&naim,1) == 0)
     {
      if(N == 2)
       VV.VVOD_SPISOK_zapis_data(2,shet.ravno());
      if(N == 3)
       VV.VVOD_SPISOK_zapis_data(3,shet.ravno());
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

int   dirkasop_zap(VVOD *VV,const char *kodm,int metka_prov,int tipop,const char *imatab)
{
char	strsql[512];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       


class OPSHET rs;
if(VV->data_ravno(2)[0] != '\0')
 if(prsh1(VV->data_ravno(2),&rs) != 0)
  return(1);

if(VV->data_ravno(3)[0] != '\0')
 if(prsh1(VV->data_ravno(3),&rs) != 0)
  return(1);

if(VV->data_ravno_atoi(4) > 0)
 {
  sprintf(strsql,"select kod from Kascn where kod=%d",VV->data_ravno_atoi(4));
  if(readkey(strsql) <= 0)
   {
    sprintf(strsql,"%s %d",gettext("Не найден в справочнике код целевого назначения"),VV->data_ravno_atoi(4));
    iceb_t_soob(strsql);
    return(1);
   }  
 }

if(SRAV(kodm,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kod from %s where kod='%s'",imatab,VV->VVOD_SPISOK_return_data(0));
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


if(kodm[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into %s values('%s','%s','%s','%s',%d,%d,%ld,%d)",
  imatab,
  VV->data_ravno_filtr(0),
  VV->data_ravno_filtr(1),  
  VV->data_ravno_filtr(2),
  VV->data_ravno_filtr(3),
  metka_prov,
  iceb_t_getuid(),
  time(NULL),
  VV->data_ravno_atoi(4));
else
  sprintf(strsql,"update %s set \
kod='%s',\
naik='%s',\
shetkas='%s',\
shetkor='%s',\
metkapr=%d,\
ktoz=%d,\
vrem=%ld,\
kcn=%d \
  where kod='%s'",
  imatab,
  VV->data_ravno_filtr(0),  
  VV->data_ravno_filtr(1),  
  VV->data_ravno_filtr(2),
  VV->data_ravno_filtr(3),
  metka_prov,
  iceb_t_getuid(),
  time(NULL),
  VV->data_ravno_atoi(4),
  kodm);

if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kodm[0] != '\0' && SRAV(kodm,VV->VVOD_SPISOK_return_data(0),0) != 0)
 dirkasop_kod(tipop,kodm,VV->VVOD_SPISOK_return_data(0)); //Корректировка кода таблицах


return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void dirkasop_sap(int tipop,int metkar,const char *naimpoi)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  if(tipop == 1)
    printw(gettext("Ввод и корректировка приходных операций"));
  if(tipop == 2)
    printw(gettext("Ввод и корректировка расходных операций"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {
  mvprintw(2,0,"   1               2                    3      4     5   6");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код"));
  mvprintw(y++,x,"2-%s",gettext("наименование"));
  mvprintw(y++,x,"3-%s",gettext("счёт кассы"));
  mvprintw(y++,x,"4-%s",gettext("счёт кореспондент"));
  mvprintw(y++,x,"5-%s",gettext("код цел. наз-чения"));
  mvprintw(y++,x,"6-%s",gettext("проводки"));

 if(naimpoi[0] != '\0')
  {
   y++;
   attron(A_BLINK);
   mvprintw(y++,x,gettext("Поиск:"));
   mvprintw(y++,x,"%s",naimpoi);
   attroff(A_BLINK);
   
  }
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
 }
 
if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int dirkasop_pu(int tipop,const char *kodavt)
{
char	strsql[512];
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select kodop from Kasord where kodop='%s' and tp=%d limit 1",kodavt,tipop);

if(readkey(strsql,&row,&cur) > 0)
 {
  sprintf(strsql,"%s Kasord !",gettext("Код используется в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }


return(0);


}
/********************/
/*Распечатать       */
/********************/

void	dirkasop_ras(SQLCURSOR *cur,int tipop)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"dmet%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
class iceb_tu_str naim(gettext("Распечатка кодов приходных операций"));
if(tipop == 2)
 naim.new_plus(gettext("Распечатка кодов расходных операций"));

iceb_t_zagolov(naim.ravno(),ff);


fprintf(ff,"\
----------------------------------------------------------------------------\n");
fprintf(ff,"\
Код |       Наименование           |Счёт кассы|Счёт кор-нт|Код ц.н.|Проводки\n");
/*
1234 123456789012345678901234567890 1234567890 12345678901 12345678
*/

fprintf(ff,"\
----------------------------------------------------------------------------\n");

cur->poz_cursor(0);
class iceb_tu_str prov_text("");

while(cur->read_cursor(&row) != 0)
 {

  if(atoi(row[4]) == 0)
   prov_text.new_plus(gettext("Не делать"));
  else
   prov_text.new_plus(gettext("Делать"));
  
  fprintf(ff,"%-*s|%-*.*s|%-*s|%-*s|%8s|%s\n",
  iceb_tu_kolbait(4,row[0]),
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  iceb_tu_kolbait(10,row[2]),
  row[2],
  iceb_tu_kolbait(11,row[3]),
  row[3],
  row[7],
  prov_text.ravno());

 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(naim.ravno());

iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/**************************/
/*Корректировка кода      */
/**************************/
void  dirkasop_kod(int tipop,const char *kods,const char *kodn)
{
char		strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Kasord set kodop='%s' where kodop='%s' and tp=%d",kodn,kods,tipop);

if(sql_zapis(strsql,0,0) != 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=%d and kto='%s'",kodn,kods,tipop,ICEB_MP_KASA);
sql_zapis(strsql,1,0);

}
