/*$Id: dirdokskr.c,v 5.30 2013/12/31 11:48:10 sasa Exp $*/
/*15.03.2018	26.10.2002	Белых А.И.	dirdokskr.c
Просмотр списка документов командировочных расходов
*/
#include	"buhg.h"

class poisk
 {
  public:
  short dn,mn,gn,dk,mk,gk; //Даты поиска
  short	metkapros; //0-все документы 1-не подтвержденные 2-без проводок
  short dp,mp,gp;    //Дата приказа
  short dao,mao,gao;  //Дата авансового отчёта
  short metkapoiska;  //0-без поиска 1-с поиском
  class iceb_tu_str kontr;
  class iceb_tu_str vkom;
  class iceb_tu_str nomdok;
  class iceb_tu_str nomerpr; //Номер приказа
  class iceb_tu_str nomaoth; //Номер авансового отчёта
  class iceb_tu_str punktnaz; /*Пунк назначения*/
  class iceb_tu_str organ;   /*Организация*/

  poisk()
   {
    clear_data();
   }
  void clear_data()
   {
    dn=mn=gn=dk=mk=gk=0;
    metkapros=0;
    dp=mp=gp=0;
    dao=mao=gao=0;
    metkapoiska=0;
    kontr.new_plus("");
    vkom.new_plus("");
    nomdok.new_plus("");
    nomerpr.new_plus("");
    nomaoth.new_plus("");
    punktnaz.new_plus("");
    organ.new_plus("");
   }   
 };

void  dirdokskrsp(struct poisk*);
int   provdirdokkr(SQL_str,struct poisk*);

extern short    startgod;
 
void	dirdokskr(int metkapoi) //0-все за стартовый год 1-за текущюю дату
{
long		kolstr;
long		pozz=0; /*Позиция первой строчки меню*/
long            pozz1=0; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
struct  tm      *bf;
short		dd,md,gd; //Дата документа
short		dp,mp,gp; //дата приказа
short		d,m,g;
int             K;
short           x=0,y=2;
int             i,kom=0,kom1;
int		prc;
time_t		vrem;
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str nomdok("");
short metka_sort_dat=0; /*0-в порядке возростания дат 1-в порядке убывания дат*/
char		konec[32];
class iceb_tu_str kontr;
class iceb_tu_str naimo;
class iceb_tu_str vkom;
class iceb_tu_str nomerpr;
class iceb_tu_str nomaoth; //Номер авансового отчёта
SQLCURSOR curr;
VVOD VV(0);

VVOD SOOB(1);

poisk POI;
POI.metkapros=0; //0-все документы 1-не подтвержденные 2-без проводок

time(&vrem);
bf=localtime(&vrem);

if(metkapoi == 0)
 {
  POI.dn=1;
  POI.mn=1;
  POI.gn=startgod;
  if(POI.gn == 0)
    POI.gn=bf->tm_year+1900;
 }

if(metkapoi == 1)
 { 
  POI.dn=bf->tm_mday;
  POI.mn=bf->tm_mon+1;
  POI.gn=bf->tm_year+1900;
 }

POI.dk=POI.mk=POI.gk=0;

naz1:;

GDITE();

short           kls=LINES-5;

if(POI.metkapoiska == 0)
  sprintf(strsql,"select nomd,datd,kont,nompr,datp,vkom,mprov,mpodt,nomao,datao,pnaz,organ from \
Ukrdok where datd >= '%04d-%d-%d'",POI.gn,POI.mn,POI.dn);
else
  sprintf(strsql,"select nomd,datd,kont,nompr,datp,vkom,mprov,mpodt,nomao,datao,pnaz,organ from \
Ukrdok where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d'",POI.gn,POI.mn,POI.dn,POI.gk,POI.mk,POI.dk);

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc");
else
 strcat(strsql," order by datd desc");

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;
  dirdokskrsp(&POI);

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
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;
  if(provdirdokkr(row,&POI) != 0)
    continue;

  konec[0]='\0';
  if(row[6][0] == '0')
   strcpy(konec,"*");
  if(row[7][0] == '0')
   strcat(konec,"?");
   
  rsdat(&dd,&md,&gd,row[1],2);
  rsdat(&dp,&mp,&gp,row[9],2); //дата авансового отчёта

  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-*s|%02d.%02d.%04d|%-*s|%s",
  dd,md,gd,
  iceb_tu_kolbait(4,row[0]),row[0],
  iceb_tu_kolbait(7,row[2]),row[2],
  iceb_tu_kolbait(3,row[5]),row[5],
  iceb_tu_kolbait(5,row[3]),row[3],
  dp,mp,gp,
  iceb_tu_kolbait(4,row[8]),row[8],
  konec);

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

dirdokskrsp(&POI);

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

    if(provdirdokkr(row,&POI) != 0)
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
    if(provdirdokkr(row,&POI) != 0)
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
    if(provdirdokkr(row,&POI) != 0)
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

  POLE(str_vib.ravno(),strsql,1,'|');
  rsdat(&dd,&md,&gd,strsql,1);
  polen(str_vib.ravno(),&nomdok,2,'|');
  polen(str_vib.ravno(),&kontr,3,'|');
  polen(str_vib.ravno(),&vkom,4,'|');
  polen(str_vib.ravno(),&nomerpr,5,'|');
  POLE(str_vib.ravno(),strsql,6,'|');
  rsdat(&dp,&mp,&gp,strsql,1);
  polen(str_vib.ravno(),&nomaoth,7,'|');

 }

vper:;

switch(K)
 {  
  case ENTER:
     dirdokkr(dd,md,gd,&nomdok);
     goto naz1;

  case FK1: //Просмотр документации
    GDITE();
    iceb_t_pdoc("ukr2_3.txt");
    clear();

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  //Не подтвержденные документы
    if(POI.metkapros == 1)
      POI.metkapros=0;
    else
      POI.metkapros=1;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK3:  //Документы без проводок
    if(POI.metkapros == 2)
      POI.metkapros=0;
    else
      POI.metkapros=2;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK4:  //Поиск

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа........"));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска....."));//1
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска......"));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,).."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Вид командировки.(,,).."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Номер приказа....(,,).."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Дата приказа..........."));//6
    VV.VVOD_SPISOK_add_MD(gettext("Номер аван.отчёта.(,,)."));//7
    VV.VVOD_SPISOK_add_MD(gettext("Дата аван.отчёта......."));//8
    VV.VVOD_SPISOK_add_MD(gettext("Пункт назначения..(,,)."));//9
    VV.VVOD_SPISOK_add_MD(gettext("Организация.......(,,)."));//10

    VV.VVOD_SPISOK_add_data(POI.nomdok.ravno(),64);
    if(POI.dn != 0)
     sprintf(strsql,"%d.%d.%d",POI.dn,POI.mn,POI.gn);
    VV.VVOD_SPISOK_add_data(strsql,11);

    strsql[0]='\0';
    if(POI.dk != 0)
     sprintf(strsql,"%d.%d.%d",POI.dk,POI.mk,POI.gk);
    VV.VVOD_SPISOK_add_data(strsql,11);

    VV.VVOD_SPISOK_add_data(POI.kontr.ravno(),128);
    VV.VVOD_SPISOK_add_data(POI.vkom.ravno(),128);
    VV.VVOD_SPISOK_add_data(POI.nomerpr.ravno(),128);

    strsql[0]='\0';
    if(POI.dp != 0)
      sprintf(strsql,"%d.%d.%d",POI.dp,POI.mp,POI.gp);
    VV.VVOD_SPISOK_add_data(strsql,11);
    VV.VVOD_SPISOK_add_data(POI.nomaoth.ravno(),128);

    strsql[0]='\0';
    if(POI.dao != 0)
      sprintf(strsql,"%d.%d.%d",POI.dao,POI.mao,POI.gao);
    VV.VVOD_SPISOK_add_data(strsql,11);

    VV.VVOD_SPISOK_add_data(POI.punktnaz.ravno(),255);

    VV.VVOD_SPISOK_add_data(POI.organ.ravno(),255);

fk4:;

    POI.metkapoiska=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);

    POI.dk=POI.mk=POI.gk=0;

    switch(kom1)
     {
      case FK10:
      case ESC:
        break;

               
      case FK2:
      case PLU:
        GDITE();
        POI.nomdok.new_plus(VV.VVOD_SPISOK_return_data(0));

        if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
         {
          if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto fk4;
           }
          POI.dk=POI.dn=d; POI.mn=POI.mk=m; POI.gn=POI.gk=g;
         }
        if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
         {
          dpm(&POI.dk,&POI.mk,&POI.gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(2),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto fk4;
          }
         else
          {
           POI.dk=d; POI.mk=m; POI.gk=g;
          }
         
        POI.kontr.new_plus(VV.VVOD_SPISOK_return_data(3));
        POI.vkom.new_plus(VV.VVOD_SPISOK_return_data(4));
        POI.nomerpr.new_plus(VV.VVOD_SPISOK_return_data(5));
        if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
         {
          if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(6),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата приказа !"));
            goto fk4;
           }
          POI.dp=d; POI.mp=m; POI.gp=g;
         }

        POI.nomaoth.new_plus(VV.VVOD_SPISOK_return_data(7));

        if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
         {
          if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(8),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата авансового отчёта !"));
            goto fk4;
           }
          POI.dao=d; POI.mao=m; POI.gao=g;
         }
        POI.punktnaz.new_plus(VV.VVOD_SPISOK_return_data(9));
        POI.organ.new_plus(VV.VVOD_SPISOK_return_data(10));

        if(VV.VVOD_SPISOK_return_data(0)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(2)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(3)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(4)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(5)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(6)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(7)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(8)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(9)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(10)[0] != '\0')
         POI.metkapoiska=1;

        pozz=0;
        break;

      case FK4:
       VV.VVOD_clear_data();
       goto fk4;
       
     }
    goto naz1;
    
  case FK5: //Расшифровка записи

    SOOB.VVOD_delete();
    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докум. :"),dd,md,gd);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",gettext("Документ    :"),nomdok.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    naimo.new_plus("");
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     naimo.new_plus(row1[0]);
    sprintf(strsql,"%s %s %s",gettext("Контрагент  :"),kontr.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    naimo.new_plus("");
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     naimo.new_plus(row1[0]);
    sprintf(strsql,"%s %s %s",gettext("Вид командир:"),vkom.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",gettext("Номер прик. :"),nomerpr.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",gettext("Номер ав.отч:"),nomaoth.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %d.%d.%d",gettext("Дата ав.отч.:"),dp,mp,gp);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);

    goto naz;
    
  case FK10:
  case ESC:
    return;

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

/*****************************/
/*Шапка меню                 */
/*****************************/

void  dirdokskrsp(struct poisk *POI)
{
int i1,X;

clear();
if(POI->metkapros == 0)
  printw(gettext("Просмотр документов"));
if(POI->metkapros == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр неподтвержднных документов"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(POI->metkapros == 2)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр документов без проводок"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
printw(" %s:%d%s",gettext("Стартовый год"),POI->gn,
gettext("г."));

move(1,0);
printw("      1        2     3     4    5       6       7");

move(i1=2,X=COLS-21);
printw(gettext("Расшифровка полей:"));
move(++i1,X);
printw("1 %s",gettext("дата документа"));
move(++i1,X);
printw("2 %s",gettext("номер документа"));
move(++i1,X);
printw(gettext("3 код контрагента"));
move(++i1,X);
printw(gettext("4 вид командировки"));
move(++i1,X);
printw(gettext("5 номер приказа"));
move(++i1,X);
printw(gettext("6 дата аван.отчёта"));
move(++i1,X);
printw(gettext("7 номер аван.отчёта"));
move(++i1,X);
printw(gettext("? не подтвержден"));
move(++i1,X);
printw(gettext("* не вып-ны проводки"));

if(POI->metkapoiska == 1)
 {
  attron(A_BLINK);
  mvprintw(i1+=2,X,gettext("Поиск:"));

  if(POI->nomdok.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Н.д."),POI->nomdok.ravno());

  if(POI->dn != 0)
    mvprintw(++i1,X,"%s %d.%d.%d",gettext("Дата н."),POI->dn,POI->mn,POI->gn);

  if(POI->dk != 0)
    mvprintw(++i1,X,"%s %d.%d.%d",gettext("Дата к."),POI->dk,POI->mk,POI->gk);

  if(POI->kontr.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Код кон."),POI->kontr.ravno());

  if(POI->vkom.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Вид коман."),POI->vkom.ravno());

  if(POI->nomerpr.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Номер прик."),POI->nomerpr.ravno());

  if(POI->dp != 0)
    mvprintw(++i1,X,"%s %d.%d.%d",gettext("Дата прик."),POI->dp,POI->mp,POI->gp);

  if(POI->nomaoth.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Н-р ав.от."),POI->nomaoth.ravno());

  if(POI->dao != 0)
    mvprintw(++i1,X,"%s %d.%d.%d",gettext("Дата ав.от."),POI->dao,POI->mao,POI->gao);

  if(POI->punktnaz.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Пунк наз."),POI->punktnaz.ravno());
  if(POI->organ.getdlinna() > 1)
    mvprintw(++i1,X,"%s %s",gettext("Орган."),POI->organ.ravno());
   
  attroff(A_BLINK);
 }
helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2"," ? ",
"F3"," * ",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F10",gettext("выход"),NULL);

}

/*****************************/
/*Проверка записей           */
/*****************************/

int   provdirdokkr(SQL_str row,struct poisk *POI)
{

if(POI->metkapros == 1 && row[7][0] != '0')
 return(1);
if(POI->metkapros == 2 && row[6][0] != '0')
 return(1);

if(POI->metkapoiska == 0)
  return(0);

if(proverka(POI->nomdok.ravno(),row[0],4,0) != 0)
 return(1);

if(proverka(POI->kontr.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(POI->vkom.ravno(),row[5],4,0) != 0)
 return(1);

if(proverka(POI->nomerpr.ravno(),row[3],4,0) != 0)
 return(1);
if(proverka(POI->punktnaz.ravno(),row[10],4,0) != 0)
 return(1);
if(proverka(POI->organ.ravno(),row[11],4,0) != 0)
 return(1);

if(POI->dp != 0)
 {
  short d,m,g;
  rsdat(&d,&m,&g,row[4],2);
  if(sravmydat(d,m,g,POI->dp,POI->mp,POI->gp) != 0)
   return(1);
 }

if(proverka(POI->nomaoth.ravno(),row[8],4,0) != 0)
 return(1);

if(POI->dao != 0)
 {
  short d,m,g;
  rsdat(&d,&m,&g,row[9],2);
  if(sravmydat(d,m,g,POI->dao,POI->mao,POI->gao) != 0)
   return(1);
 }


return(0);

}
