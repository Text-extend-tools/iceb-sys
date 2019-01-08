/*$Id:$*/
/*02.03.2017	02.03.2017	Белых А.И.	dirfortnp.c
Ввод и корректирвка списка деталей для цеха
*/
#include        <errno.h>
#include        <buhg.h>
#include "fort.h"

class dirfortnp_poi
 {
  public:
   class iceb_tu_str kod_ceh;
   class iceb_tu_str kod_det;
   class iceb_tu_str naim_det;
   short metka_poi;

   class iceb_tu_str kod_ceha;
  dirfortnp_poi()
   {
    metka_poi=0;
    kod_ceh.plus("");
    kod_det.plus("");
    naim_det.plus("");
   }
 };
 
void dirfortnp_vvod(unsigned int nom_zap);
void dirfortnp_sap(class dirfortnp_poi*);
int dirfortnp_zap(VVOD *VV,unsigned int nom_zap);
int  dirfortnp_prov_vud(unsigned int nom_zap,int);
void	dirfortnp_ras(SQLCURSOR *cur,class dirfortnp_poi *poisk);
void dirfortnp_ud_neisp(unsigned int nom_zap);
int dirfortnp_prov(SQL_str row,class dirfortnp_poi *poisk);
int dirfortti_poi(class dirfortnp_poi *poisk);
int dirfortnpz(unsigned int nom_zap_h);



int dirfortnp(const char *kod_ceha)
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
class dirfortnp_poi poisk;
class iceb_tu_str naim_det("");
class VVOD DANET(1);
class VVOD VVOD1(2);

poisk.kod_ceha.plus(kod_ceha);
if(poisk.kod_ceh.getdlinna() <= 1 && kod_ceha[0] != '\0')
 poisk.kod_ceh.new_plus(kod_ceha);


naz1:;

short           kls=LINES-6;
unsigned int nom_zap[kls];

if(kod_ceha[0] == '\0' || poisk.metka_poi == 1)
 sprintf(strsql,"select * from Fortnp");
else
 sprintf(strsql,"select * from Fortnp where ceh='%s'",kod_ceha);

  
class SQLCURSOR cur,cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirfortnp_sap(&poisk);
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

  if(dirfortnp_prov(row,&poisk) != 0)
   continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");


  nom_zap[i]=atoi(row[0]);  

  sprintf(strsql,"%-*s|%-6s|%-*.*s|%-*s|%s",
  iceb_tu_kolbait(6,row[1]),
  row[1],
  row[2],
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  iceb_tu_kolbait(4,row[3]),
  row[3],
  row[4]);
  
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

dirfortnp_sap(&poisk);

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
    if(dirfortnp_prov(row,&poisk) != 0)
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
    if(dirfortnp_prov(row,&poisk) != 0)
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
    if(dirfortnp_prov(row,&poisk) != 0)
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
    dirfortnpz(nom_zap[kom]);
    goto naz1;
          
  case SFK2:   /*Корректировать*/
    dirfortnp_vvod(nom_zap[kom]);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirfortnp_vvod(0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(dirfortnp_prov_vud(nom_zap[kom],1) != 0)
        goto naz;
      sprintf(strsql,"delete from Fortnp where nz=%d",nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case SFK3:       //Удалить неиспользуемые записи
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить неиспользуемые записи ? Вы уверены ?"));
    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    dirfortnp_ud_neisp(nom_zap[kom]);
    goto naz1;
    

  case FK4:       /*Поиск*/
     
     dirfortti_poi(&poisk);

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    dirfortnp_ras(&cur,&poisk);
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

void dirfortnp_vvod(unsigned int nom_zap)
{
char		strsql[2048];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
class iceb_tu_str kod("");
class iceb_tu_str naim_ceh("");
class iceb_tu_str naim_det("");
class iceb_tu_str ei("");
class iceb_tu_str naim_ei;
SQLCURSOR cur;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(60);//3



if(nom_zap != 0)
 {

  sprintf(strsql,"select * from Fortnp where nz=%d",nom_zap);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[2]);
  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  VV.VVOD_SPISOK_zapis_data(3,row[4]);
  ktozap=atoi(row[5]);
  vremz=atol(row[6]);

     
 }



VV.VVOD_SPISOK_add_MD(gettext("Код цеха..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код детали........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........."));//3


naz:;

VV.VVOD_delete_ZAG();

if(nom_zap != 0)
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
  sprintf(strsql,"select naimat,ei from Material where kodm=%d",VV.data_ravno_atoi(1));
  if(readkey(strsql,&row,&cur) == 1)
   naim_det.new_plus(row[0]);

  sprintf(strsql,"%s:%s",gettext("Деталь"),naim_det.ravno());
  
  VV.VVOD_SPISOK_add_ZAG(strsql);
  if(VV.data_ravno(2)[0] == '\0')
   VV.data_plus(2,row[1]);
 }

if(VV.data_ravno(2)[0] != '\0')
 {
  sprintf(strsql,"select naik from Edizmer where kod='%s'",VV.data_ravno_filtr(2));
  if(readkey(strsql,&row,&cur) == 1)
   naim_ei.new_plus(row[0]);

  sprintf(strsql,"%s:%s",gettext("Единица измерения"),naim_ei.ravno());
  
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


helstr(LINES-1,0,"F1",gettext("помощь"),
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
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 2)
     {
      naim_ei.new_plus("");
      if(iceb_tl_ei(2,&ei,&naim_ei) == 0)
       VV.data_plus(N,ei.ravno());
      N++;
      goto naz;
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
    if(dirfortnp_zap(&VV,nom_zap) != 0)
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

int dirfortnp_zap(VVOD *VV,unsigned int nom_zap)
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

sprintf(strsql,"select kod from Fortsc where kod='%s'",VV->data_ravno(0));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s %s",gettext("Не найден код цеха"),VV->data_ravno(0),gettext("в справочнике цехов"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select kodm from Material where kodm=%d",VV->data_ravno_atoi(1));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d %s",gettext("Не найден код детали"),VV->data_ravno_atoi(1),gettext("в справочнике деталей"));
  iceb_t_soob(strsql);
  return(1);
 }
sprintf(strsql,"select kod from Edizmer where kod='%s'",VV->data_ravno_filtr(2));
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s %s",gettext("Не найдена единица измерения"),VV->data_ravno(2),gettext("в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }

if(nom_zap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Fortnp (ceh,kd,ei,kom,ktoz,vrem) values('%s',%d,'%s','%s',%d,%ld)",
  VV->data_ravno_filtr(0),  
  VV->data_ravno_atoi(1),  
  VV->data_ravno_filtr(2),  
  VV->data_ravno_filtr(3),  
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Fortnp set \
ceh='%s',\
kd=%d,\
ei='%s',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",  
  VV->data_ravno_filtr(0),  
  VV->data_ravno_atoi(1),  
  VV->data_ravno_filtr(2),  
  VV->data_ravno_filtr(3),  
  iceb_t_getuid(),
  time(NULL),
  nom_zap);

if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirfortnp_sap(class dirfortnp_poi *poisk)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка списка деталей"));
if(poisk->kod_ceha.getdlinna() > 1)
 mvprintw(1,0,"%s %s",gettext("Просмотр по цеху"),poisk->kod_ceha.ravno());
                         
mvprintw(2,0,"    1       2                 3                 4    5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код цеха"));
mvprintw(y++,x,"2-%s",gettext("код детали"));
mvprintw(y++,x,"3-%s",gettext("наименование дет."));
mvprintw(y++,x,"4-%s",gettext("един.измерения"));
mvprintw(y++,x,"5-%s",gettext("коментарий"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,"%s:",gettext("Поиск"));
 
  if(poisk->kod_ceh.getdlinna() > 1)  
   mvprintw(y++,x,"%s:%s",gettext("Код цех."),poisk->kod_ceh.ravno());
  if(poisk->kod_det.getdlinna() > 1)  
   mvprintw(y++,x,"%s:%s",gettext("Код дет."),poisk->kod_det.ravno());
  if(poisk->naim_det.getdlinna() > 1)  
   mvprintw(y++,x,"%s:%s",gettext("Наим.дет."),poisk->naim_det.ravno());
 
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

void	dirfortnp_ras(SQLCURSOR *cur,class dirfortnp_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
class iceb_tu_str naim_det("");
char strsql[1024];


sprintf(imaf,"np%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список деталей"),ff);

if(poisk->kod_ceh.getdlinna() > 1)  
 fprintf(ff,"%s:%s\n",gettext("Код цех."),poisk->kod_ceh.ravno());
if(poisk->kod_det.getdlinna() > 1)  
 fprintf(ff,"%s:%s\n",gettext("Код дет."),poisk->kod_det.ravno());
if(poisk->naim_det.getdlinna() > 1)  
 fprintf(ff,"%s:%s\n",gettext("Наим.дет."),poisk->naim_det.ravno());


fprintf(ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код цеха|Код дет.| Наименование детали          |Ед.изм.| Коментарий\n"));
/*
12345678 12345678 123456789012345678901234567890 1234567
*/
fprintf(ff,"\
------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {

  if(dirfortnp_prov(row,poisk) != 0)
   continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  fprintf(ff,"%-*s|%-8s|%-*.*s|%-*s|%s\n",
  iceb_tu_kolbait(8,row[1]),
  row[1],
  row[2],
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  iceb_tu_kolbait(7,row[3]),
  row[3],
  row[4]);
 }
fprintf(ff,"\
------------------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список деталей"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}
/************************/
/*Удаление неиспользуемых складов*/
/*********************************/
void dirfortnp_ud_neisp(unsigned int nom_zap)
{
int kolstr;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

clear();
GDITE();

sprintf(strsql,"select * from %s","Fortnp");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%*s %s ",
  iceb_tu_kolbait(10,row[1]),
  row[1],
  row[2]);

  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirfortnp_prov_vud(nom_zap,0) != 0)
   {
    printw("\n");
    continue;
   }
  sprintf(strsql,"delete from %s where nz=%d","Fortnp",nom_zap);
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
iceb_t_soob(strsql);

}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int dirfortnp_prov_vud(unsigned int nom_zap,int metka_s)
{
char	strsql[512];


sprintf(strsql,"select nz from Fortnpz where nzh=%d limit 1",nom_zap);

if(readkey(strsql) > 0)
 {
  if(metka_s == 1)
   {
    iceb_t_soob(gettext("Удалить невозможно! Есть записи к этой детали!"));
   }
  return(1);
 }


return(0);


}
/*********************************/
/*ввод реквизитов поиска*/
/******************************/
int dirfortti_poi(class dirfortnp_poi *poisk)
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
    poisk->kod_ceha.new_plus("");
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
int dirfortnp_prov(SQL_str row, //Строка для проверки 
class dirfortnp_poi *poisk)
{
if(poisk->metka_poi == 0)
 return(0);

if(proverka(poisk->kod_ceh.ravno(),row[1],0,0) != 0)
 return(1);

if(proverka(poisk->kod_det.ravno(),row[2],0,0) != 0)
 return(1);

if(poisk->naim_det.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  
  char strsql[512];
  class iceb_tu_str naim_det("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  if(proverka(poisk->naim_det.ravno(),naim_det.ravno(),4,0) != 0)
    return(1);

 } 
return(0);
}
