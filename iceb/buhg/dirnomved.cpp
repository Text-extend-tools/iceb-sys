/*$Id: dirnomved.c,v 5.33 2013/05/17 14:55:58 sasa Exp $*/
/*09.03.2018	05.10.2004	Белых А.И.	dirnomved.c
Просмотр журнала номеров платёжных ведомостей
*/
#include        <errno.h>
#include	"buhg.h"
class nomved_poi
 {
  public:
  class iceb_tu_str nomdpoi;
  class iceb_tu_str dat1;
  class iceb_tu_str dat2;
  short metpoi;

  nomved_poi()
   {
    clear_data();
   }
  void clear_data()
   {
    nomdpoi.new_plus("");
    dat1.new_plus("");
    dat2.new_plus("");
    metpoi=0;
   }
 };


int vvodzapnv(short god,const char *nomved);
void nomved_sap(class nomved_poi *data);
int nomved_pol_nom(void );
int nomved_prov_poi(SQL_str row,class nomved_poi *data);
void nomved_rasp(SQLCURSOR *cur,int,class nomved_poi *data);


void dirnomved()
{
class nomved_poi data;
class iceb_t_mstr spis_menu(0);
char strsql[512];
short           dn=0,mn=0,gn=0,dk=0,mk=0,gk=0;
short  d,m,g;

int kolstr;
int K=0;
int pozz=0,pozz1=0;
short		mspz; /*метка соответствия первой записи*/
SQL_str row;
int kom=0;
int i=0;
int		prc;
short           x=0,y=2;
class iceb_tu_str nomord("");
int kl=0;
short den,mes,god;
class iceb_tu_str nomv_ved("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

naz1:;
clear();
GDITE();
if(gn == 0)
 gn=gt;

int kls=LINES-5;

sprintf(strsql,"select * from Kasnomved where datd >= '%04d-01-01' \
order by datd,nomv asc",gn);

if(dn != 0)
  sprintf(strsql,"select * from Kasnomved where datd >= '%04d-%02d-%02d' \
order by datd,nomv asc",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Kasnomved \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' \
order by datd,nomv asc",gn,mn,dn,gk,mk,dk);


SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {

lll:;
   nomved_sap(&data);

   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
   K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
  if(K == FK5 || K == SFK2 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
    goto naz1;  
  goto vper;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  if(nomved_prov_poi(row,&data) != 0)
   continue;

  rsdat(&d,&m,&g,row[0],2);
  
  sprintf(strsql,"%02d.%02d.%d|%-*s|%8s|%-*s|%.*s",
  d,m,g,
  iceb_tu_kolbait(4,row[2]),row[2],
  row[3],
  iceb_tu_kolbait(7,row[7]),row[7],
  iceb_tu_kolbait(40,row[4]),row[4]);

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

nomved_sap(&data);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размера экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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

    if(nomved_prov_poi(row,&data) != 0)
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
    if(nomved_prov_poi(row,&data) != 0)
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
/*
  printw("\npozz=%d kls=%d kolstr=%d i=%d\n",
  pozz,kls,kolstr,i);
  OSTANOV();
*/
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
/*  
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n",pozz,pozz1,i,kls);
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
    if(nomved_prov_poi(row,&data) != 0)
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
   goto naz;

  polen(str_vib.ravno(),strsql,sizeof(strsql),1,'|');
  rsdat(&den,&mes,&god,strsql,1);
  polen(str_vib.ravno(),&nomv_ved,2,'|');
  polen(str_vib.ravno(),&nomord,4,'|');
  
 }

vper:;
switch(K)
 {  
  
  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("kas5.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  //ввод новой записи

    vvodzapnv(0,"");

    goto naz1;

  case ENTER:
  case SFK2:  //Корректировка записей
    if(nomord.getdlinna() > 1)
     {
      iceb_t_soob(gettext("Корректировать можно только кассовый ордер!"));
      goto naz1;
     }

    vvodzapnv(god,nomv_ved.ravno());

    goto naz1;

  case FK3:  //Удалить запись
    if(nomord.getdlinna() > 1)
     {
      iceb_t_soob(gettext("Запись можно удалить только удалив кассовый ордер!"));
      goto naz1;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;  

    sprintf(strsql,"delete from Kasnomved where god=%d and nomv='%s'",god,nomv_ved.ravno());

    sql_zapis(strsql,0,0);

    goto naz1;



  case FK4:  /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа......."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска...."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска....."));
      
    VV.VVOD_SPISOK_add_data(data.nomdpoi.ravno(),32);
    VV.VVOD_SPISOK_add_data(data.dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(data.dat2.ravno(),11);
    
    
naz2:;
    data.metpoi=0;
    
    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

         
    kl=VV.vvod(0,1,1,-1,-1);

    data.nomdpoi.new_plus(VV.VVOD_SPISOK_return_data(0));
    data.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
    data.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));


    dn=mn=gn=dk=mk=gk=0;
    switch(kl)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
         data.metpoi=1;
        
        if(data.dat1.getdlinna() > 1)
         {
          if(rsdat(&d,&m,&g,data.dat1.ravno(),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto naz2;
           }
          dk=dn=d; mn=mk=m; gn=gk=g;
         }

        if(data.dat2.getdlinna() <= 1)
         {
          dpm(&dk,&mk,&gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,data.dat2.ravno(),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto naz2;
          }
         else
          {
           dk=d; mk=m; gk=g;
          }
        pozz=0;
        goto naz1;


      case FK4:
        data.clear_data();
        goto naz2;

                   
      default:
        goto naz2;
     }

    goto naz1;

   case FK5:
     nomved_rasp(&cur,kolstr,&data);
     goto naz1;

  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }


}

/***********************/
/*Вывод шапки на экран*/
/**********************/

void nomved_sap(class nomved_poi *data)
{
clear();

printw(gettext("Просмотр журнала платёжных ведомостей"));
move(1,0);
printw("  1            2      3       4        5");

int Y,X;

move(Y=2,X=COLS-21);
printw(gettext("Расшифровка полей:"));
move(++Y,X);
printw("1-%s",gettext("дата"));
move(++Y,X);
printw("2-%s",gettext("номер документа"));
move(++Y,X);
printw("3-%s",gettext("сумма"));
move(++Y,X);
printw("4-%s",gettext("номер ордера"));
move(++Y,X);
printw("5-%s",gettext("комментарий"));

if(data->metpoi == 1)
 {
  move(Y+=2,X);
  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(data->nomdpoi.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s %s",gettext("Н.д."),data->nomdpoi.ravno());
   }

  if(data->dat1.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s %s",gettext("Дата н."),data->dat1.ravno());
   }

  if(data->dat2.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s %s",gettext("Дата к."),data->dat2.ravno());
   }
  attroff(A_BLINK);
 }

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
//"F6",gettext("текущая дата"),
"F10",gettext("выход"),NULL);


}



/*************************************/
/*Ввод и корректировка записи*/
/*********************************/
//Если вернули 0-ввели 1-нет
int vvodzapnv(short god,const char *nomved)
{
int K=0,N=0;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
short d,m,g;

VVOD VV(0);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(99);

if(god == 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
  sprintf(strsql,"%d",nomved_pol_nom( ));
  VV.VVOD_SPISOK_zapis_data(1,strsql);
 }
if(god != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  sprintf(strsql,"select * from Kasnomved where god=%d and nomv=%s",god,nomved);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);   
   }  
  rsdat(&d,&m,&g,row[0],2);
  sprintf(strsql,"%d.%d.%d",d,m,g);
  VV.VVOD_SPISOK_zapis_data(0,strsql);

  VV.VVOD_SPISOK_zapis_data(1,row[2]);

  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  
  VV.VVOD_SPISOK_zapis_data(3,row[4]);
  
  kzvz(atoi(row[5]),atol(row[6]),&VV);


 }
VV.VVOD_SPISOK_add_MD(gettext("Дата документа..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.........."));
VV.VVOD_SPISOK_add_MD(gettext("Сумма по документу......."));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий..............."));



naz:;

//clear();


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F4",gettext("ном.док."),
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

    if(N == 0) /*Количество*/
     {

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
    N=-1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
   
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
   
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || \
     VV.VVOD_SPISOK_return_data(2)[0] == '\0'   || VV.VVOD_SPISOK_return_data(3)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }

    if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(0),1) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto naz;
     }
    
    time_t vrem;
    time(&vrem);
    if(SRAV(nomved,VV.VVOD_SPISOK_return_data(1),0) != 0 || god != VV.data_ravno_god(0))
     {
      sprintf(strsql,"select god from Kasnomved where god=%d and nomv='%s'",
      VV.data_ravno_god(0),VV.VVOD_SPISOK_return_data(1));
      if(readkey(strsql) > 0)
       {
        sprintf(strsql,gettext("Номер документа %s уже есть в базе!"),VV.data_ravno(1));
        iceb_t_soob(strsql);
        goto naz;
       
       }
     }

    if(god == 0)
       sprintf(strsql,"insert into Kasnomved \
values ('%d-%d-%d',%d,'%s',%.2f,'%s',%d,%ld,'')",
       g,m,d,
       g,
       VV.data_ravno_filtr(1),
       VV.data_ravno_atof(2),
       VV.VVOD_SPISOK_return_data(3),
       iceb_t_getuid(),
       vrem);
    else
       sprintf(strsql,"update Kasnomved \
set \
datd='%d-%d-%d',\
god=%d,\
nomv='%s',\
suma=%.2f,\
koment='%s',\
ktoi=%d,\
vrem=%ld \
where god=%d and nomv=%s",
       g,m,d,
       g,
       VV.data_ravno_filtr(1),
       VV.data_ravno_atof(2),
       VV.VVOD_SPISOK_return_data(3),
       iceb_t_getuid(),
       vrem,
       god,
       nomved);
//    printw("\nstrsql-%s\n",strsql);
//    OSTANOV();            
    if(sql_zapis(strsql,0,0) != 0)
     goto naz;
    return(0); 
   }
  if(K == FK4) //получить новый номер документа
   {
    sprintf(strsql,"%d",nomved_pol_nom( ));
    VV.VVOD_SPISOK_zapis_data(1,strsql);
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(1);
}
/**********************************/
/*Подпрограмма получения номера документа*/
/*****************************************/

int nomved_pol_nom()
{
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int nomer_ved=0;
int nm=0;
int kolstr=0;
sprintf(strsql,"select nomv from Kasnomved where god=%d",gt);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
  nm=atoi(row[0]);
  if(nm > nomer_ved)
   nomer_ved=nm;
 }
return(++nomer_ved); 

}
/****************************/
/*Праверка на условия поиска*/
/****************************/

int nomved_prov_poi(SQL_str row,class nomved_poi *data)
{
if(data->metpoi == 0)
 return(0);
if(proverka(data->nomdpoi.ravno(),row[2],0,0) != 0)
 return(1);
return(0); 
}
/************************************/
/*Распечатка */
/***********************************/

void nomved_rasp(SQLCURSOR *cur,int kolstr,class nomved_poi *data)
{
SQL_str row;
FILE *ff;
char imaf[64];

sprintf(imaf,"nomved%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

short dn,mn,gn; 
short dk,mk,gk; 
rsdat(&dn,&mn,&gn,data->dat1.ravno(),1);
rsdat(&dk,&mk,&gk,data->dat2.ravno(),1);

iceb_t_zagolov(gettext("Журнал платёжных ведомостей"),dn,mn,gn,dk,mk,gk,ff);
fprintf(ff,"\
-------------------------------------------------------------\n");

fprintf(ff,"\
  Дата    |Намер вед.|Сумма   | Коментарий\n");
  
fprintf(ff,"\
-------------------------------------------------------------\n");
cur->poz_cursor(0);
int kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(nomved_prov_poi(row,data) != 0)
   continue;
  rsdat(&dn,&mn,&gn,row[0],2);
  fprintf(ff,"%02d.%02d.%d %-*s %-8s %s\n",
  dn,mn,gn,
  iceb_tu_kolbait(10,row[2]),row[2],
  row[3],row[4]);
  
 }
fprintf(ff,"\
-------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Журнал платёжных ведомостей"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
