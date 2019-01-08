/*$Id: diruplds.c,v 5.33 2013/12/31 11:48:10 sasa Exp $*/
/*15.05.2018	17.04.2003	Белых А.И.	diruplds.c
Посмотр списка документов "Учет путевых листов"
*/
#include	"buhg.h"
class poisk
 {
  public:
  short	dn,mn,gn; //дата начала
  short	dk,mk,gk; //дата конца
  class iceb_tu_str nomd;   //номер документа
  class iceb_tu_str podr;
  class iceb_tu_str kodavt; //код автомобиля
  class iceb_tu_str kodvod; //код водителя
  short metpoi; //0-нет поиска 1-есть
  short sptop; //0-все 1-только не со списанным топливом

  poisk()
   {
    dn=mn=gn=dk=mk=gk=0;
    nomd.new_plus("");
    metpoi=sptop=0;
    podr.new_plus("");
    kodavt.new_plus("");
    kodvod.new_plus("");
   }
 };
 
void             duplds(class poisk*);
int              poiupl(class poisk*);
int              provpoi(class poisk*,SQL_str);

extern short	startgodupl; //Стартовый год для "Учета путевых листов"

void	diruplds(short dn,short mn,short gn) //дата начала поиска
{
long		kolstr;
long		pozz=0; /*Позиция первой строчки меню*/
long            pozz1=0; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
short		dk=0,mk=0,gk=0;
int		K=0;
short		mspz; /*метка соответствия первой записи*/
int		i=0;
int		kom=0;
int		prc=0;
short           X=0,Y=2;
short		dd,md,gd;
class iceb_tu_str nomd("");
class iceb_tu_str podr("");
class iceb_tu_str naim("");
class iceb_tu_str kod("");
class iceb_t_mstr spis_menu(0);
class iceb_tu_str konc("");
class iceb_tu_str fio_vod("");
short metka_sort_dat=0; /*0-в порядке возростания дат 1-в порядке убывания*/
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR curr;
VVOD SOOB(1);
poisk poi;

if(gn == 0)
 gn=startgodupl;

if(gn == 0)
  gn=gt;

naz1:;

GDITE();

short           kls=LINES-5;
int		ktoz[kls];
time_t          vrem[kls];

const char *pol={"datd,kp,nomd,ka,kv,mt,ktoz,vrem"};

sprintf(strsql,"select %s from Upldok where datd >= '%04d-01-01'",pol,gn);

if(dn != 0)
  sprintf(strsql,"select %s from Upldok where datd >= '%04d-%02d-%02d'",pol,gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select %s from Upldok where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",pol,gn,mn,dn,gk,mk,dk);

//Если задан поиск
if(poi.dn != 0)
  sprintf(strsql,"select %s from Upldok where datd >= '%04d-%02d-%02d'",pol,poi.gn,poi.mn,poi.dn);

if(poi.dn != 0 && poi.dk != 0)
  sprintf(strsql,"select %s from Upldok where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",pol,poi.gn,poi.mn,poi.dn,poi.gk,poi.mk,poi.dk);

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc,nomd asc");
else
 strcat(strsql," order by datd desc,nomd asc");

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;


  duplds(&poi);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
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

  if(provpoi(&poi,row) != 0)
    continue;
  
  sprintf(strsql,"select naik from Uplouot where kod=%s",row[4]);
  if(readkey(strsql,&row1,&curr) == 1)
   fio_vod.new_plus(row1[0]);
  else
   fio_vod.new_plus("");
   
  rsdat(&dd,&md,&gd,row[0],2);

  ktoz[i]=atoi(row[6]);  
  vrem[i]=atol(row[7]);  
 
  if(row[5][0] == '0')
   konc.new_plus("?");
  else
   konc.new_plus("");
  
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-2s|%-3s|%-3s|%1s|%s",
  dd,md,gd,
  iceb_tu_kolbait(6,row[2]),row[2],
  row[1],row[3],row[4],konc.ravno(),fio_vod.ravno());

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

duplds(&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-25);


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
/*
    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps,kprrz,skl11,tipzz,nomdokv,mpn,
    dover,metpoi,nomnalnak) != 0)
      continue;
*/
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
/*
    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps,kprrz,skl11,tipzz,nomdokv,mpn,
    dover,metpoi,nomnalnak) != 0)
      continue;
*/
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
  GDITE();
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
/*
    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps,kprrz,skl11,tipzz,nomdokv,mpn,
     dover,metpoi,nomnalnak) != 0)
      continue;
*/
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
   goto naz;
  
  polen(strvib.ravno(),strsql,sizeof(strsql),1,'|');
  rsdat(&dd,&md,&gd,strsql,1);

  polen(strvib.ravno(),&nomd,2,'|');

  polen(strvib.ravno(),&podr,3,'|');

  
 }

vper:;

switch(K)
 {  
  case ENTER:  //Просмотр выбранного документа
//     dirupld(dd,md,gd,&nomd,&podr);
     dirupldok3(dd,md,gd,nomd.ravno(),podr.ravno_atoi());
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: //Помощь
    GDITE();
    iceb_t_pdoc("upl2_4.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2: //Переключение режимов просмотра
    poi.sptop++;
    if(poi.sptop == 2)
     poi.sptop=0;
    pozz=0;
    goto naz;

  case FK4: //Поиск
    poiupl(&poi);
    pozz=0;
    goto naz1;
         
  case FK5: //Расшифровка
    SOOB.VVOD_delete();

    sprintf(strsql,"%-20s:%d.%d.%d",gettext("Дата документа"),dd,md,gd);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%-20s:%s",gettext("Номер документа"),nomd.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    //Читаем наименование автомобиля
    polen(spis_menu.spis_str.ravno(kom),&kod,4,'|');
    naim.new_plus("");
    sprintf(strsql,"select naik from Uplavt where kod=%s",kod.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);

    sprintf(strsql,"%-20s:%s %.40s",gettext("Автомобиль"),kod.ravno(),naim.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    //Читаем водителя
    polen(spis_menu.spis_str.ravno(kom),&kod,5,'|');
    naim.new_plus("");
    sprintf(strsql,"select naik from Uplouot where kod=%s",kod.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);

    sprintf(strsql,"%-20s:%s %.40s",gettext("Водитель"),kod.ravno(),naim.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    if(ktoz[kom] != 0)
     {
      sprintf(strsql,"%-*s:%s",iceb_tu_kolbait(20,gettext("Записал")),gettext("Записал"),iceb_t_kszap(ktoz[kom]));
      SOOB.VVOD_SPISOK_add_MD(strsql);             
     }
    if(vrem[kom] != 0)
     {
      bf=localtime(&vrem[kom]);
      sprintf(strsql,"%-20s:%d.%d.%d%s %s: %d:%d:%d",
      gettext("Дата записи"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;

  case FK6: /*Просмотр от текущего дня*/
    poi.metpoi=1;
    poi.dn=dt;
    poi.mn=mt;
    poi.gn=gt;        
    poi.dk=poi.mk=poi.gk=0;
    goto naz1;

  case FK7: //День назад
    poi.metpoi=1;
    if(poi.dn == 0)
     {
      poi.dn=dn;
      poi.mn=mn;
      poi.gn=gn;
     }

    if(poi.dn == 0)
     {
      poi.dn=dt;
      poi.mn=mt;
      poi.gn=gt;
     }
    dpm(&poi.dn,&poi.mn,&poi.gn,2);
    poi.dk=poi.dn; poi.mk=poi.mn; poi.gk=poi.gn;
    goto naz1;

  case SFK7: //День вперед
    poi.metpoi=1;
    if(poi.dn == 0)
     {
      poi.dn=dn;
      poi.mn=mn;
      poi.gn=gn;
     }

    if(poi.dn == 0)
     {
      poi.dn=dt;
      poi.mn=mt;
      poi.gn=gt;
     }
    dpm(&poi.dn,&poi.mn,&poi.gn,1);
    poi.dk=poi.dn; poi.mk=poi.mn; poi.gk=poi.gn;
    goto naz1;
    
  case TAB:
   metka_sort_dat++;
   if(metka_sort_dat >= 2)
    metka_sort_dat=0;
   goto naz1;
   
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }


}
/*******************************/
/*Шапка                        */
/*******************************/

void  duplds(class poisk *poi)
{
int Y=0,X=0;

clear();
printw("%s",gettext("Просмотр списка документов"));
if(poi->sptop == 1)
  printw(" %s",gettext("Только документы с не списанным топливом"));
  
mvprintw(1,0,"      1         2    3  4   5  6   7");

move(Y=1,X=COLS-21);
mvprintw(Y++,X,gettext("Расшифровка полей:"));
mvprintw(Y++,X,"1 %s",gettext("дата документа"));
mvprintw(Y++,X,"2 %s",gettext("номер документа"));
mvprintw(Y++,X,"3 %s",gettext("код подразделения"));
mvprintw(Y++,X,"4 %s",gettext("код автомобиля"));
mvprintw(Y++,X,"5 %s",gettext("код водителя"));
mvprintw(Y++,X,"6 %s",gettext("метка не списано топливо"));
mvprintw(Y++,X,"7 %s",gettext("водитель"));

if(poi->metpoi == 1)
 {
  attron(A_BLINK);
  mvprintw(Y+=2,X,"%s:",gettext("Поиск"));
  if(poi->nomd.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Н.д."),poi->nomd.ravno());

  if(poi->dn != 0)
    mvprintw(++Y,X,"%s:%d.%d.%d",gettext("Дата н."),poi->dn,poi->mn,poi->gn);

  if(poi->dk != 0)
    mvprintw(++Y,X,"%s:%d.%d.%d",gettext("Дата к."),poi->dk,poi->mk,poi->gk);

  if(poi->podr.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Подр"),poi->podr.ravno());

  if(poi->kodavt.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Авт."),poi->kodavt.ravno());

  if(poi->kodvod.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Вод."),poi->kodvod.ravno());
  attroff(A_BLINK);

 }

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2"," ? ",
//"F3"," * ",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F6",gettext("текущая дата"),
"F7",gettext("пред.день"),
"F10",gettext("выход"),NULL);

}
/***********************************/
/*Ввод реквизитов поиска документов*/
/***********************************/
//Если вернули 0-поиск выполнять 1-нет
int   poiupl(class poisk *poi)
{
int   kom1=0;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");

poi->metpoi=0;
poi->dn=poi->mn=poi->gn=poi->dk=poi->mk=poi->gk=0;

VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

VV.VVOD_SPISOK_add_MD(gettext("Номер документа..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения........."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля............"));//4
VV.VVOD_SPISOK_add_MD(gettext("Код водителя.............."));//5

VV.VVOD_SPISOK_add_data(poi->nomd.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(poi->podr.ravno(),32);
VV.VVOD_SPISOK_add_data(poi->kodavt.ravno(),32);
VV.VVOD_SPISOK_add_data(poi->kodvod.ravno(),32);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kl=VV.vvod(0,1,1,-1,-1);

poi->nomd.new_plus(VV.VVOD_SPISOK_return_data(0));
dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
poi->podr.new_plus(VV.VVOD_SPISOK_return_data(3));
poi->kodavt.new_plus(VV.VVOD_SPISOK_return_data(4));
poi->kodvod.new_plus(VV.VVOD_SPISOK_return_data(5));

switch(kl)
 {
  case FK10:
  case ESC:
    return(1);    

  case FK2:
  case PLU:
    GDITE();
    if(dat1.ravno()[0] != '\0')
     {
      if(rsdat(&poi->dn,&poi->mn,&poi->gn,dat1.ravno(),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата начала !"));
        goto naz;
       }
      poi->dk=poi->dn; poi->mk=poi->mn; poi->gk=poi->gn;
     }

    if(dat2.ravno()[0] == '\0')
     {
      dpm(&poi->dk,&poi->mk,&poi->gk,5); 
     }
    else
     if(rsdat(&poi->dk,&poi->mk,&poi->gk,dat2.ravno(),0) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата конца !"));
       goto naz;
      }
     
    poi->metpoi=1;    
    break;

  case FK3:
   
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;        
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       break;

      case 0 :
        clear();
        kodn.new_plus("");
        if(vibrek("Uplpodr",&kodn) == 0)
          VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
        break;

      case 1 :
        clear();
        naimn.new_plus("");
        if(diravt0(&kodn,&naimn,1) == 0)
          VV.VVOD_SPISOK_zapis_data(4,kodn.ravno());
        clear();
        break;

      case 2 :
        clear();
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,1) == 0)
          VV.VVOD_SPISOK_zapis_data(5,kodn.ravno());
        clear();
        break;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
 }
return(0);
}
/****************************/
/*Проверка на условия поиска*/
/****************************/
//Если вернули 0-подходит 1-нет
int   provpoi(class poisk *poi,SQL_str row)
{

if(poi->sptop == 1 && row[16][0] != '0')
 return(1);

if(poi->metpoi == 0)
 return(0);
 
if(proverka(poi->nomd.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(poi->podr.ravno(),row[1],0,0) != 0)
 return(1);

if(proverka(poi->kodavt.ravno(),row[5],0,0) != 0)
 return(1);

if(proverka(poi->kodvod.ravno(),row[6],0,0) != 0)
 return(1);
 
return(0);

}
