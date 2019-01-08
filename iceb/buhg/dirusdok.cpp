/* $Id: dirusdok.c,v 5.46 2013/12/31 11:48:10 sasa Exp $ */
/*11.01.2018    28.02.2000      Белых А.И.      dirusdok.c
Просмотр документов на услуги
*/
#include	"buhg.h"
class dirusdok_poi
 {
  public:
  class iceb_tu_str dat1,dat2;
  class iceb_tu_str kprrz;
  class iceb_tu_str nomdokv;
  class iceb_tu_str nomnalnak;
  class iceb_tu_str naikontrp;
  class iceb_tu_str dover;  
  class iceb_tu_str kps;
  class iceb_tu_str vid_dog;
  int   tipzz;
  class iceb_tu_str skl11;

  short metpoi; //0-без проверки 1-с проверкой
  short		mpn; /*0-все 1-не подтвержденный 2-без проводок*/
  short metka_opl; /*0-все 1-оплаченые 2-неоплаченные*/
  
  dirusdok_poi()
   {
    metka_opl=mpn=0;
    clear_data();
   }
  void clear_data()
   {
    metpoi=0;
    dat1.new_plus("");
    dat2.plus_tek_dat();
    kprrz.new_plus("");
    nomdokv.new_plus("");
    nomnalnak.new_plus("");
    naikontrp.new_plus("");
    kps.new_plus("");
    dover.new_plus("");
    skl11.new_plus("");
    vid_dog.new_plus("");
    tipzz=0;
   }

 };

int		poivcurd(short,int,char[],int,SQLCURSOR*);
void 		shsmn(class dirusdok_poi*,int koldok);
int		sravdok1(SQL_str,class dirusdok_poi*);
void dirusdok_vk(class dirusdok_poi *poisk,SQLCURSOR *cur);

extern short	startgodus; /*Стартовый год просмотров услуг*/

void dirusdok(short dnp,short mnp,short gnp) //Дата начала поиска
{
class iceb_t_mstr spis_menu(0);
long		kolstr;
long		pozz; /*Позиция первой строчки меню*/
long            pozz1; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
short           voz;  /*Возврат*/
short           dn,mn,gn,dk,mk,gk;
short		d,m,g;
int             K;
short           x=0,y=2;
int             i,kom,kom1;
int		prc;
int		skld=0;
short		kl;
class iceb_tu_str nomdokd("");
class iceb_tu_str bros("");
int		tipzd=0;
class iceb_tu_str naimo("");
class iceb_tu_str naiskl(""),naimpr("");
time_t		tmm;
short		mspz; /*метка соответствия первой записи*/
int		poz,komv;
class iceb_tu_str kor("");
class iceb_tu_str kontr("");
class iceb_tu_str podr("");
class iceb_tu_str kodop("");
static class dirusdok_poi poisk;
class iceb_tu_str kodv("");
class iceb_tu_str naimv("");
short metka_sort_dat=0; /*0-сортировка по возростанию дат 1-по убыванию*/
short dt,mt,gt;

if(poisk.dat1.getdlinna() <= 1)
 {
  if(dnp == 0)
    sprintf(strsql,"01.01.%d",startgodus);
  else
    sprintf(strsql,"%02d.%02d.%d",dnp,mnp,gnp);

  poisk.dat1.new_plus(strsql);
 }

poisk.metpoi=0; //0-без проверки 1-с проверкой
poisk.mpn=0; /*0-все 1-не подтвержденный 2-без проводок*/
poisk.metka_opl=0; /*0-все 1-оплаченые 2-неоплаченные*/

poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
//Создаем класс меню
VVOD SOOB(1);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

kom=voz=0;

dn=dk=0;
 
dn=dnp;
mn=mnp;
gn=gnp;

dk=mk=gk=0;

clear();
dnp=0;
pozz=0;

naz1:;

clear();
GDITE();
short           kls=LINES-5;

if(gn == 0)
 gn=gt;

bros.new_plus("tp,datd,podr,kontr,nomd,nomnn,kodop,pod,pro,ktoi,nomdp,vrem,dover,nds,sumkor,sumnds,pn,mo,uslpr");

sprintf(strsql,"select %s from Usldokum where datd >= '%d-01-01'",bros.ravno(),gn);

if(dn != 0)
  sprintf(strsql,"select %s from Usldokum where datd >= '%d-%02d-%02d'",bros.ravno(),gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select %s from Usldokum where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",bros.ravno(),gn,mn,dn,gk,mk,dk);

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc,tp asc,nomd asc");
else
 strcat(strsql," order by datd desc,tp asc,nomd asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

  shsmn(&poisk,kolstr);
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

  if(sravdok1(row,&poisk) != 0)
    continue;

  if(row[0][0] == '1')
   bros.new_plus("+");
  if(row[0][0] == '2')
   bros.new_plus("-"); 
  if(row[7][0] == '0')
   bros.plus("?");
  if(row[8][0] == '0')
   bros.plus("*");
  if(row[17][0] == '0')
   bros.plus("$");
  
  rsdat(&d,&m,&g,row[1],2);
  
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-2s|%-*s|%-*s|%s",
  d,m,g,
  iceb_tu_kolbait(8,row[4]),row[4],
  iceb_tu_kolbait(5,row[3]),row[3],
  iceb_tu_kolbait(5,row[6]),row[6],
  row[2],
  iceb_tu_kolbait(8,row[5]),row[5],
  iceb_tu_kolbait(4,row[10]),row[10],
  bros.ravno());

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
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

shsmn(&poisk,kolstr);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-26);


if(kom == -7) //Изменение размеров экрана
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

    if(sravdok1(row,&poisk) != 0)
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

    if(sravdok1(row,&poisk) != 0)
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
    if(sravdok1(row,&poisk) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str strv(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),strv.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(strv.ravno(),&bros,1,'|');
  rsdat(&dnp,&mnp,&gnp,bros.ravno(),0);

  polen(strv.ravno(),&skld,5,'|');

  polen(strv.ravno(),&nomdokd,2,'|');
  polen(strv.ravno(),&kontr,3,'|');
  polen(strv.ravno(),&kodop,4,'|');
  polen(strv.ravno(),&podr,5,'|');

  polen(strv.ravno(),&bros,8,'|');

  tipzd=2;
  if(bros.ravno()[0] == '+')
   tipzd=1;

  sprintf(strsql,"select * from Usldokum where datd='%d-%d-%d' and podr=%s and nomd='%s' and tp=%d",
  gnp,mnp,dnp,podr.ravno(),nomdokd.ravno(),tipzd);  

  if(readkey(strsql,&row,&curr) != 1 )
   {
    SOOB.VVOD_delete();
    sprintf(strsql,"%s %s %s %s %s %d.%d.%d%s %s !",
    gettext("Документ"),
    nomdokd.ravno(),
    gettext("в подразделении"),
    podr.ravno(),
    gettext("за"),  
    dnp,mnp,gnp,
    gettext("г."),gettext("не найден"));

    SOOB.VVOD_SPISOK_add_MD(strsql);

    if(tipzd == 1)
      sprintf(strsql,gettext("Приход"));
    else   
      sprintf(strsql,gettext("Расход"));

    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
 }

vper:;
switch(K)
 {  
  case ENTER:
  
     clear();


     dirudok(dnp,mnp,gnp,tipzd,skld,nomdokd.ravno());
     dnp=mnp=gnp=0;
     
     clear();
    
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("usl2_3.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  /*Не подтвержденные накладные*/
    if(poisk.mpn == 1)
      poisk.mpn=0;
    else
      poisk.mpn=1;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK2:  /*Снять метку не подтвержденного документа*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку не подтвержденного документа ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;

    sprintf(strsql,"update Usldokum \
set \
pod=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s'",
    gnp,mnp,dnp,skld,nomdokd.ravno());

    sql_zapis(strsql,0,0);
    goto naz1;

  case FK3:  /*Накладные без проводок*/
    if(poisk.mpn == 2)
      poisk.mpn=0;
    else
      poisk.mpn=2;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK3:  /*Снять отметку о не выполненых проводках*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку о невыполненых проводках ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"update Usldokum \
set \
pro=1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s'",
    gnp,mnp,dnp,skld,nomdokd.ravno());

    sql_zapis(strsql,0,0);
    goto naz1;


  case FK4:  /*Поиск*/

    VV.VVOD_delete();

    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа.....(,,).."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));//1
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.....(,,).."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Код прихода/расхода.(,,).."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Код подразделения...(,,).."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)......."));//6
    VV.VVOD_SPISOK_add_MD(gettext("Наименование кон-та.(,,).."));//7
    VV.VVOD_SPISOK_add_MD(gettext("Номер нал. накладной.(,,)."));//8
    VV.VVOD_SPISOK_add_MD(gettext("Доверенность........(,,).."));//9
    VV.VVOD_SPISOK_add_MD(gettext("Вид договора........(,,).."));//10

    VV.VVOD_SPISOK_add_data(poisk.nomdokv.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(poisk.dat2.ravno(),11);
    VV.VVOD_SPISOK_add_data(poisk.kps.ravno(),128);

    VV.VVOD_SPISOK_add_data(poisk.kprrz.ravno(),128);

    VV.VVOD_SPISOK_add_data(poisk.skl11.ravno(),128);
    
    VV.VVOD_SPISOK_add_data(2);
    VV.VVOD_SPISOK_add_data(poisk.naikontrp.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.nomnalnak.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.dover.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.vid_dog.ravno(),128);
    
naz2:;

    poisk.metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

    kl=VV.vvod(0,1,1,-1,-1);

    poisk.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
    poisk.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
    poisk.nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));
    poisk.naikontrp.new_plus(VV.VVOD_SPISOK_return_data(7));
    poisk.kps.new_plus(VV.VVOD_SPISOK_return_data(3));
    poisk.kprrz.new_plus(VV.VVOD_SPISOK_return_data(4));
    poisk.skl11.new_plus(VV.VVOD_SPISOK_return_data(5));
    poisk.tipzz=0;
    if(VV.VVOD_SPISOK_return_data(6)[0] == '+')
      poisk.tipzz=1;
    if(VV.VVOD_SPISOK_return_data(6)[0] == '-')
      poisk.tipzz=2;
    poisk.nomnalnak.new_plus(VV.VVOD_SPISOK_return_data(8));
    poisk.dover.new_plus(VV.VVOD_SPISOK_return_data(9));
    poisk.vid_dog.new_plus(VV.VVOD_SPISOK_return_data(10));

    switch(kl)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
//        dn=mn=gn=0;
//        dk=mk=gk=0;        
        if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
         {
          if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto naz2;
           }
          dn=dk=d; mn=mk=m; gn=gk=g;
         }

        if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
         {
          dk=dn;
          mk=mn;
          gk=gn;
          dpm(&dk,&mk,&gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(2),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto naz2;
          }
         else
          {
           dk=d; mk=m; gk=g;
          }
        poisk.metpoi=1;
        pozz=0;
        goto naz1;

      case FK3:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        kom1=0;        
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

        switch (kom1)
         {
          case 4 :
          case -1:
           goto naz2;

          case 0 :
            clear();
            naimo.new_plus("");
            if(dirpodrusl(1,&kor,&naimo) == 0)
             {
              VV.data_z_plus(5,kor.ravno());
             }
            clear();
            goto naz2;

          case 1 :
            clear();
            naimv.new_plus("");
            if(dirkontr(&kodv,&naimv,1) == 0)
             {
              VV.data_z_plus(3,kodv.ravno());
             }
            clear();
            goto naz2;

          case 2 :
            clear();
            naimo.new_plus("");
            poz=komv=0;
            if(dirmat1("Usloper1",&kor,&naimo,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
             {
              VV.data_z_plus(4,kor.ravno());
             }
             
            clear();
            goto naz2;

          case 3 :
            clear();
            naimo.new_plus("");
            if(diruslop2(1,&kodv,&naimv) == 0)
              VV.data_z_plus(4,kodv.ravno());

            clear();
            goto naz2;
         }

        goto naz2;

      case FK4:
        VV.VVOD_clear_data();
//        metpoi=0;
        goto naz2;
                   
      default:
        goto naz2;
     }

    goto naz1;

  case FK5: /*Расшифровка*/

    /*Читаем наименование организации*/
    naimo.new_plus("");

    if(kontr.ravno()[0] == '0' && kontr.ravno()[1] == '0' && kontr.ravno()[2] == '-')
      polen(kontr.ravno(),&bros,1,'-');
    else
      bros.new_plus(kontr.ravno());

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("\n%s %s\n",gettext("Не найден код контрагента"),kontr.ravno());
      OSTANOV();
     }
    else
     {
      naimo.new_plus(row1[0]);
     } 

    /*Читаем наименование операции*/
    if(tipzd == 1)
      bros.new_plus("Usloper1");
    if(tipzd == 2)
      bros.new_plus("Usloper2");
    naimpr.new_plus("");
    sprintf(strsql,"select naik from %s where kod='%s'",bros.ravno(),kodop.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      naimpr.new_plus(row1[0]);

    /*Читаем наименование подразделения*/
    naiskl.new_plus("");
    sprintf(strsql,"select naik from Uslpodr where kod=%s",
    podr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      naiskl.new_plus(row1[0]);

    SOOB.VVOD_delete();
    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докум.  :"),dnp,mnp,gnp);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",      gettext("Документ     :"),nomdokd.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",  gettext("Подразделение:"),podr.ravno(),naiskl.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",   gettext("Контрагент   :"),kontr.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",   gettext("Операция     :"),kodop.ravno(),naimpr.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    if(row[21] != NULL)
    if(atoi(row[21]) != 0)
     {
      sprintf(strsql,"%s %s",gettext("Записал"),iceb_t_kszap(row[21]));
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }

    if(atoi(row[22]) > 0)
     {
      tmm=atoi(row[22]);
      bf=localtime(&tmm);
      sprintf(strsql,"%s %d.%d.%d%s %s:%d:%d:%d",
      gettext("Дата записи :"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    goto naz;

    case FK6: /*Просмотр от текущего дня*/
      dirusdok_vk(&poisk,&cur);
      goto naz1;

  case FK9: /*переключение просмотра оплаченных и неоплаченных документов*/
    poisk.metka_opl++;
    if(poisk.metka_opl > 2)
     poisk.metka_opl=0;
    goto naz1;

  case TAB: /*включение/выключение сортировки по убыванию дат*/
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

/*******/
/*Шапка*/
/*******/
void shsmn(class dirusdok_poi *poisk,int koldok)
{
int		i1,X;

clear();

move(0,0);

if(poisk->mpn == 0)
  printw(gettext("Просмотр документов"));
if(poisk->mpn == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр неподтвержднных документов"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(poisk->mpn == 2)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр документов без проводок"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
printw("  %s %d",gettext("Стартовый год"),startgodus);
printw(" %s:%d",gettext("Кол.док."),koldok);
move(1,0);
printw("      1          2      3     4   5     6       7");

move(i1=2,X=COLS-21);
printw(gettext("Расшифровка полей:"));
move(++i1,X);
printw("1 %s",gettext("дата документа"));
move(++i1,X);
printw("2 %s",gettext("номер документа"));
move(++i1,X);
printw(gettext("3 код контрагента"));
move(++i1,X);
printw(gettext("4 код приход./расхода"));
move(++i1,X);
printw(gettext("5 код подразделения"));
move(++i1,X);
printw(gettext("6 номер нал. накл."));
move(++i1,X);
printw(gettext("7 номер парн.докум."));
move(++i1,X);
printw(gettext("+/- приход/расход"));
move(++i1,X);
printw(gettext("? не подтвержден"));
move(++i1,X);
printw(gettext("* не вып-ны проводки"));
move(++i1,X);
printw("$ %s",gettext("не оплачено"));

move(i1+=2,X);

if(poisk->metpoi == 1)
 {
  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(poisk->nomdokv.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Н.д."),poisk->nomdokv.ravno());
   }

  if(poisk->dat1.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Дата н."),poisk->dat1.ravno());
   }

  if(poisk->dat2.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Дата к."),poisk->dat2.ravno());
   }

  if(poisk->kps.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Код кон."),poisk->kps.ravno());
   }
  if(poisk->kprrz.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Код оп."),poisk->kprrz.ravno());
   }
  if(poisk->skl11.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Подразделение"),poisk->skl11.ravno());
   }
  if(poisk->tipzz != 0)
   {
    move(++i1,X);
    if(poisk->tipzz == 1)
      printw(gettext("ТОЛЬКО ПРИХОДЫ"));
    if(poisk->tipzz == 2)
      printw(gettext("ТОЛЬКО РАСХОДЫ"));
   }
  if(poisk->naikontrp.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Наи.к-та"),poisk->naikontrp.ravno());
   }
  if(poisk->nomnalnak.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Нал.накладная"),poisk->nomnalnak.ravno());
   }  
  if(poisk->dover.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Довер."),poisk->dover.ravno());
   }  
  if(poisk->vid_dog.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Договор"),poisk->vid_dog.ravno());
   }  

  attroff(A_BLINK);
 }

if(poisk->metka_opl == 1)
 {
  move(++i1,X);
  printw("%s",gettext("Только оплаченные"));
 }

if(poisk->metka_opl == 2)
 {
  move(++i1,X);
  printw("%s",gettext("Только неоплаченные"));
 }


helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2"," ? ",
"F3"," * ",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F6",gettext("экспорт"),
"F9",gettext("оплата"),
"F10",gettext("выход"),NULL);
}

/*****************************
Проверка реквизитов поиска
Возвращяем 0- если все равно
           1-если не совпадает
*******************************/           
int	sravdok1(SQL_str row,class dirusdok_poi *poisk)
{
int		skl111;
int		tipz1;
short		pro,pod;

/*
printw("sravdok-mpn=%d pro=%d\n",mpn,pro);
OSTANOV();
*/
pro=atoi(row[8]);
int metka_opl=atoi(row[17]);

if(poisk->metka_opl == 1 && metka_opl != 1)
 return(1);

if(poisk->metka_opl == 2 && metka_opl != 0)
 return(1);

if(poisk->mpn == 2 && pro == 1)
   return(1);
  
pod=atoi(row[7]);
if(poisk->mpn == 1 && pod == 1)
   return(1);

if(poisk->metpoi == 0)
 return(0);
 
tipz1=atoi(row[0]);
skl111=atoi(row[2]);

  
if(proverka(poisk->kps.ravno(),row[3],0,0) != 0)
 return(1);

if(proverka(poisk->kprrz.ravno(),row[6],0,0) != 0)
 return(1);
 

if(proverka(poisk->skl11.ravno(),skl111,0,0) != 0)
 return(1);

if(poisk->tipzz != 0 && poisk->tipzz != tipz1)
   return(1);

if(proverka(poisk->nomdokv.ravno(),row[4],0,0) != 0)
 if(proverka(poisk->nomdokv.ravno(),row[11],0,0) != 0)
  return(1);

if(poisk->naikontrp.getdlinna() > 1)
 {
  SQLCURSOR curr;
  char strsql[512];
  SQL_str row1;

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);

  if(sql_readkey(&bd,strsql,&row1,&curr) == 1 )
   if(proverka(poisk->naikontrp.ravno(),row1[0],4,0) != 0)
     return(1);
   
 }

if(proverka(poisk->nomnalnak.ravno(),row[5],1,0) != 0)
  return(1);
if(proverka(poisk->dover.ravno(),row[12],4,0) != 0)
  return(1);
if(proverka(poisk->vid_dog.ravno(),row[18],4,0) != 0)
  return(1);
  
return(0);

}
/*********************************************/
/*Определение в курсоре позиции нужной записи*/
/*********************************************/
/*Если вернули -1 значит не нашли*/

int		poivcurd(short god,int sklad,char nomd[],int pozzn,
SQLCURSOR *cur)
{
SQL_str         row;
int		pozz;
short		d,m,g;

pozz=pozzn;
cur->poz_cursor(pozz);
while(cur->read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[1],2);
  if(SRAV(row[4],nomd,0) == 0 && god == g && sklad == atoi(row[2]))
   {
    return(pozz);        
   }
  pozz++;
 }
return(-1);
}
/*************************/
/*Создание кассовых ордеров*/
/**************************/
void dirusdok_ko(int tipz, /*1-приходный 2-расходный*/
class dirusdok_poi *poisk,SQLCURSOR *cur)
{
int tipz_usl=0;
char		strsql[1024];
SQL_str		row;
static class iceb_tu_str kassa("");
static class iceb_tu_str dat1("");
static class iceb_tu_str kodop("");
int		kom=0,kom1=0;
class iceb_tu_str kod("");
class iceb_tu_str naimkas("");
class iceb_tu_str naimop("");
short		dko,mko,gko;
class iceb_tu_str shetk("");
SQLCURSOR 	cur1;



VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_MD(gettext("Касса..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции...."));

VV.VVOD_SPISOK_add_data(kassa.ravno(),16);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kodop.ravno(),32);

naz:;
clear();

VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых ордеров\""));
if(naimkas.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimkas.ravno());
if(naimop.ravno()[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(naimop.ravno());

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

kassa.new_plus(VV.VVOD_SPISOK_return_data(0));
dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
kodop.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
    break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        naimkas.new_plus("");
        if(dirkas(0,&kod,&naimkas) == 0)
          VV.VVOD_SPISOK_zapis_data(0,kod.ravno());
        goto naz;

      case 1 :
        naimop.new_plus("");
        if(dirkasop(tipz,1,&kod,&naimop) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kod.ravno());
        goto naz;

     }

    break;
         
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

//Проверяем кассу
sprintf(strsql,"select kod from Kas where kod=%s",VV.VVOD_SPISOK_return_data(0));
if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдена касса"),VV.VVOD_SPISOK_return_data(0));
  iceb_t_soob(strsql);
  goto naz;
 }

//Проверяем код операции
if(tipz == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",VV.VVOD_SPISOK_return_data(2));
if(tipz == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",VV.VVOD_SPISOK_return_data(2));

if(readkey(strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(2));
  iceb_t_soob(strsql);
  goto naz;
 }

naimop.new_plus(row[0]);
shetk.new_plus(row[1]);

//Проверяем дату
if(rsdat(&dko,&mko,&gko,VV.VVOD_SPISOK_return_data(1),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }


MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Для каждого документа отдельный кассовый ордер"));
MENU.VVOD_SPISOK_add_MD(gettext("Все документы в один кассовый оредр"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

clearstr(LINES-1,0);
int metka_ks=0; /*0-ордер для каждого документа 1- все в одном*/
while(menu3w(stdscr,&MENU,&metka_ks,-1,-1,0) != 0);
switch (metka_ks)
 {
  case 2 :
  case -1:
   return;

 }
/**********
short vt=0;
iceb_t_poldan("Код группы возвратная тара",strsql,"matnast.alx");
vt=atoi(strsql);
*************/
class SPISOK kod_kontr;
class masiv_din_double suma_kontr;



class iceb_tu_str nomd("");


double suma=0.;
double sumabn=0.;
double sumkor=0.;
short d,m,g;
int lnds=0;
double suma_nds=0.;
double suma_dok=0.;
float pnds=0.;
if(tipz == 1)
 tipz_usl=2;
else
 tipz_usl=1;
int nomer_kontr=0;
cur->poz_cursor(0);
/************
tp,datd,podr,kontr,nomd,nomnn,kodop,pod,pro,ktoi,nomdp,vrem,dover,nds,sumkor,sumnds,pn
***************/
while(cur->read_cursor(&row) != 0)
 {
  if(tipz == atoi(row[0])) /*Если приходный кассовый ордер то нам нужны расходные документы и наооборот*/
   continue;
  
  if(sravdok1(row,poisk) != 0)
    continue;
  
  rsdat(&d,&m,&g,row[1],2);

  pnds=atof(row[16]);

  /*Узнаем НДС документа*/
  lnds=atoi(row[13]);

  suma_nds=0.;
  if(lnds == 0)
   {
    suma_nds=atof(row[15]);
   }

  sumzap1u(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,lnds,tipz_usl);

  if(lnds == 0 && suma_nds == 0.)
    suma_nds=(suma+sumkor)*pnds/100.;
  
/*  printw("suma=%f sumabn=%f sumkor=%f\n",suma,sumabn,sumkor);*/
  suma_dok=suma+sumkor+suma_nds+sumabn;
    


  if(metka_ks == 0)
   {
    lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_t_nomnak(gko,kassa.ravno(),&nomd,tipz,2,1);
    zaphkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
    printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    zapzkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),row[4],suma_dok,"","");
   }
  else
   {
    if((nomer_kontr=kod_kontr.find_r(row[3])) < 0)
      kod_kontr.plus(row[3]);

    suma_kontr.plus(suma_dok,nomer_kontr);


   }  

 }

if(metka_ks == 1)
 {
  lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_t_nomnak(gko,kassa.ravno(),&nomd,tipz,2,1);
  zaphkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
  printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

  for(nomer_kontr=0; nomer_kontr < kod_kontr.kolih(); nomer_kontr++)
   {
    
    zapzkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),kod_kontr.ravno(nomer_kontr),suma_kontr.ravno(nomer_kontr),"","");
   }
 }
OSTANOV();

}
/**********************/
/*Выбор типа кассового ордера*/
/*****************************/
void dirusdok_vk(class dirusdok_poi *poisk,SQLCURSOR *cur)
{
int tipz=0;
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Виберите нужное и нажми те Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Експортировать кассовые ордера для приходных документов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Експортировать кассовые ордера для расходных документов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2:
  case -1 :
    return;

  case 0:
    tipz=2;
    break;

  case 1:
    tipz=1;
    break;
 }
dirusdok_ko(tipz,poisk,cur);

}
