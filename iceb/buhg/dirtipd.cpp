/*$Id: dirtipd.c,v 5.1 2013-01-02 10:38:36 sasa Exp $*/
/*19.06.2015	24.03.2003	Белых А.И.	dirtipd.c
Ввод и корректирвка списка типовых документов
*/
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

void dirtipd_vvod(const char *tablica,const char*);
void            dirtipd_sap(const char*);
void            dirtipd_ras(SQLCURSOR*);
void dirtipd_std(const char *tablica,const char *kodd);
int dirtipd_snd(const char *tablica,const char *kodd);

extern class REC rec;
extern short    kp;    /*Количество повторов*/

void dirtipd(const char *tablica)
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
class iceb_tu_str kodd("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str naimpoi("");
class iceb_tu_str knst("");
SQLCURSOR curr;


//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select * from %s order by kod asc",tablica);


class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  dirtipd_sap(naimpoi.ravno());
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }


naz:;

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;



cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(naimpoi.getdlinna() > 1 && strstrm(row[5],naimpoi.ravno()) == 0)
   continue;
  
  sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(5,row[0]),row[0],row[1]);
  
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

dirtipd_sap(naimpoi.ravno());

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
    if(naimpoi.getdlinna() > 1 && strstrm(row[5],naimpoi.ravno()) == 0)
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
//    if(naimpoi[0] != '\0' && strstrm(row[5],naimpoi) == 0)
    if(naimpoi.getdlinna() > 1 && strstrm(row[5],naimpoi.ravno()) == 0)
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
//    if(naimpoi[0] != '\0' && strstrm(row[5],naimpoi) == 0)
    if(naimpoi.getdlinna() > 1 && strstrm(row[5],naimpoi.ravno()) == 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str strvib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),strvib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(strvib.ravno(),&kodd,1,'|');

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
    iceb_t_pdoc("dokum2_2.txt");
    goto naz;

  case SFK2:   /*Корректировать*/
    dirtipd_vvod(tablica,kodd.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirtipd_vvod(tablica,"");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from %s where kod='%s'",tablica,kodd.ravno());
      if(sql_zapis(strsql,0,0) != 0)
        goto naz;

      class iceb_tu_str tablica_tipd("");

      if(SRAV(tablica,"Ktnp",0) == 0)
       tablica_tipd.new_plus("Tpltp");
      if(SRAV(tablica,"Ktnt",0) == 0)
       tablica_tipd.new_plus("Tpltt");
      
      /*Удаляем типовой документа*/
      sprintf(strsql,"delete from %s where nomd='%s'",tablica_tipd.ravno(),kodd.ravno());
      sql_zapis(strsql,0,0);
      
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
    dirtipd_ras(&cur);
    goto naz;

  case FK6:    //Ввод типового документа
    dirtipd_std(tablica,kodd.ravno());
    goto naz;

  case ENTER:
  case FK7: /*создание нового документа на базе типового*/
   dirtipd_snd(tablica,kodd.ravno());
   return;
   
  default:
    goto naz;
    break;
 }



}
/*************************************/
/*Корректировка кода документа*/
/**************************************/
void dirtipd_kor(const char *tablica,const char *skod,const char *nkod)
{

char strsql[512];
class iceb_tu_str tablica_tipd("");

if(SRAV(tablica,"Ktnp",0) == 0)
 tablica_tipd.new_plus("Tpltp");
if(SRAV(tablica,"Ktnt",0) == 0)
 tablica_tipd.new_plus("Tpltt");


sprintf(strsql,"update %s set nomd='%s' where nomd='%s'",tablica_tipd.ravno(),nkod,skod);
sql_zapis(strsql,1,0);



}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   dirtipd_zap(const char *tablica,VVOD *VV,const char *kodd,int vid)
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


 
if(SRAV(kodd,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kod from %s where kod='%s'",tablica,VV->VVOD_SPISOK_return_data(0));
  if(readkey(strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }

  if(kodd[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }

time_t vrem;
time(&vrem);

if(kodd[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into %s values ('%s','%s',%d,%ld)",
  tablica,
  VV->data_ravno(0),  
  VV->data_ravno_filtr(1),  
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"update %s \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s'",
  tablica,
  VV->data_ravno(0),  
  VV->data_ravno_filtr(1),  
  iceb_t_getuid(),vrem,
  kodd);


if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kodd[0] != '\0' && SRAV(kodd,VV->VVOD_SPISOK_return_data(0),0) != 0)
 dirtipd_kor(tablica,kodd,VV->VVOD_SPISOK_return_data(0)); //Корректировка кода таблицах



return(0);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirtipd_vvod(const char *tablica,const char *kodd)
{
char		strsql[1024];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
int		vid=0;
SQLCURSOR curr;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(60);//1


if(kodd[0] != '\0')
 {

  sprintf(strsql,"select * from %s where kod='%s'",tablica,kodd);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;   
   }
  VV.VVOD_SPISOK_zapis_data(0,kodd);
  VV.VVOD_SPISOK_zapis_data(1,row[1]);

  ktozap=atoi(row[2]);
  vremz=atol(row[3]);

 }


VV.VVOD_SPISOK_add_MD(gettext("Код документа..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование документа.."));//1


naz:;

VV.VVOD_delete_ZAG();

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка типовых документов"));


kzvz(ktozap,vremz,&VV);


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
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

    if(N == 1)
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
//    iceb_t_pdoc("upl3_4_1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirtipd_zap(tablica,&VV,kodd,vid) != 0)
     goto naz;
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
}



/**************************/
/*Шапка                   */
/**************************/

void	dirtipd_sap(const char *naimpoi)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Ввод и корректировка типовых документов"));

mvprintw(2,0,"   1       2");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код"));
mvprintw(y++,x,"2-%s",gettext("наименование"));

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
"F6",gettext("тип.док."),
"F7",gettext("док."),
"F10",gettext("выход"),NULL);


}

/********************/
/*Распечатать       */
/********************/

void	dirtipd_ras(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;

sprintf(imaf,"tipd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка списка типовых документов"),ff);


fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |Наименование\n"));
/*
1234 1234567890 1234567890 1234567890 1234567 1234567 123
*/

fprintf(ff,"\
----------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  fprintf(ff,"%-*s %s\n",
  iceb_tu_kolbait(5,row[0]),
  row[0],
  row[1]);
 }
fprintf(ff,"\
----------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка типовых документов"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/*******************************/
/*создание типового документа*/
/****************************/
void dirtipd_std(const char *tablica,const char *kodd)
{
class iceb_tu_str tablica_tipd("");

if(SRAV(tablica,"Ktnp",0) == 0)
 tablica_tipd.new_plus("Tpltp");
if(SRAV(tablica,"Ktnt",0) == 0)
 tablica_tipd.new_plus("Tpltt");

if(tablica_tipd.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не определено наименование таблицы!"));
  return;
 }

if(readpdok(tablica_tipd.ravno(),0,kodd) != 0)
 {
  /*Создаем новую платежку*/
  vzrkbz();
  char imaf[64];
 
  sozkom_pd(imaf,0.,0.);
  rec.nds=0.;
  rec.sumd=0.;
  rec.nomdk.new_plus(kodd);
 }
rec.dd=rec.md=rec.gd=0;

pld(1,tablica_tipd.ravno());
clear();      

}
/********************************************/
/*Создание нового документа с искользованием типового документа*/
/*0-создали документа 1-нет*/
/*****************************************************************/
int dirtipd_snd(const char *tablica,const char *kodd)
{
class iceb_tu_str tablica_tipd("");
class iceb_tu_str tablica_dok("");
int tipz=0;

if(SRAV(tablica,"Ktnp",0) == 0)
 {
  tablica_tipd.new_plus("Tpltp");
  tipz=2;
 }
if(SRAV(tablica,"Ktnt",0) == 0)
 {
  tablica_tipd.new_plus("Tpltt");
  tipz=1;
 }
if(tablica_tipd.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не определено наименование типовой таблицы!"));
  return(1);
 }

if(SRAV(tablica,"Ktnp",0) == 0)
 tablica_dok.new_plus("Pltp");
if(SRAV(tablica,"Ktnt",0) == 0)
 tablica_dok.new_plus("Pltt");

if(tablica_dok.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не определено наименование таблицы документа !"));
  return(1);
 }



if(readpdok(tablica_tipd.ravno(),0,kodd) != 0)
 {
  iceb_t_soob(gettext("Не найдено типового документа !"));
  return(1);
 }
char strsql[512];

static short dtp=0,mtp=0,gtp=0;

if(dtp == 0)
  poltekdat(&dtp,&mtp,&gtp);

VVOD VVOD1(2);
class iceb_tu_str vstr("");
sprintf(strsql,"%d.%d.%d",dtp,mtp,gtp);
vstr.new_plus(strsql);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Введите дату"));
int kom1=0;     

naz_dat:;
if((kom1=vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
 {
  return(1);
 }

if(kom1 == ESC)
 {
  return(1);
 }

if(rsdat(&rec.dd,&rec.md,&rec.gd,vstr.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz_dat;
 }



dtp=rec.dd; mtp=rec.md; gtp=rec.gd;

rec.tipz=tipz;

rec.nomdk.new_plus(iceb_t_nomdok(gtp,tablica_dok.ravno()));

pld(kp,tablica_dok.ravno());

clear();      
return(0);
}
