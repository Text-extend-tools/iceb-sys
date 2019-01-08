/* $Id: smdok.c,v 5.56 2013/12/31 11:48:11 sasa Exp $ */
/*11.01.2018    15.04.1997      Белых А.И.      smdok.c
Просмотр списка документов для материального учета
*/
#include	"buhg.h"

class smdok_poi
 {
  public:
   short dn,mn,gn;
   short dk,mk,gk;
   class iceb_tu_str nomdokv;
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str kps;
   class iceb_tu_str kprrz;
   class iceb_tu_str skl11;
   int tipzz;
   class iceb_tu_str dover;
   class iceb_tu_str nomnalnak;
   class iceb_tu_str naim_kontr;
   class iceb_tu_str vid_dog;   
   short mpn;    /*0-все 1-не подтвержденный 2-без проводок*/
   short metpoi; /*0-не задан поиск 1-задан*/
   short metka_opl; /*0-все 1-только оплаченные 2 - только не оплаченные*/
   short god_start; /*Стартовый год просмортра документов*/

  smdok_poi()
   {
    clear();
    mpn=0;
    metpoi=0;
    metka_opl=0;
    dn=mn=gn=dk=mk=gk=0;
   }

  void clear()
   {
    naim_kontr.new_plus("");
    nomdokv.new_plus("");
    dat1.new_plus("");
    dat2.plus_tek_dat();
    kps.new_plus("");
    kprrz.new_plus("");
    skl11.new_plus("");
    dover.new_plus("");
    nomnalnak.new_plus("");
    tipzz=0;    
    vid_dog.new_plus("");
   }   
 };


void shsmn(class smdok_poi *poisk,int koldok);
int sravdok(SQL_str row,class smdok_poi *poisk);
void smdok_vk(class smdok_poi *poisk,SQLCURSOR *cur,int kolstr);
int eks_dok_mu(short dd,short md,short gd,const char *nomdok,int sklad,class iceb_tu_str *imaf,FILE *ff);


void smdok(short dnp,short mnp,short gnp) //Дата начала поиска
{
static class smdok_poi poisk;
long		kolstr;
long		pozz=0; /*Позиция первой строчки меню*/
long            pozz1=0; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
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
short		mspz=0; /*метка соответствия первой записи*/
class iceb_tu_str kontr("");
class iceb_tu_str oper("");
class iceb_tu_str skladch("");
class iceb_t_mstr spis_menu(0);
int metka_naim_skl=0;
class iceb_tu_str naim_skl("");
char str_menu[1024];
poisk.metpoi=0; /*Обязательно*/
poisk.mpn=0;
poisk.metka_opl=0;
short metka_sort_dok=0; /*0-сортировка по возрастанию дат 1-по убыванию*/

short dt,mt,gt;

poltekdat(&dt,&mt,&gt);

kom=0;

if(dnp != 0)
 poisk.metpoi=1;
 
 
poisk.dn=dnp;
poisk.mn=mnp;
poisk.gn=gnp;
poisk.god_start=gnp;
if(poisk.dat1.getdlinna() <= 1)
 {
  sprintf(strsql,"01.01.%d",gnp);
  poisk.dat1.new_plus(strsql);
 }
 
dnp=0;
pozz=0;
//Создаем класс меню
VVOD SOOB(1);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

naz1:;

clear();
GDITE();

short           kls=LINES-5;
int kto_vvel[kls];

if(poisk.gn == 0)
 poisk.gn=gt;

sprintf(strsql,"select * from Dokummat where datd >= '%04d-01-01'",poisk.god_start);

if(poisk.dn != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d'",poisk.gn,poisk.mn,poisk.dn);

if(poisk.dn != 0 && poisk.dk != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",poisk.gn,poisk.mn,poisk.dn,poisk.gk,poisk.mk,poisk.dk);

if(metka_sort_dok == 0)
  strcat(strsql,"order by datd asc,tip asc,nomd asc");
else
  strcat(strsql,"order by datd desc,tip asc,nomd asc");

SQLCURSOR cur;
SQLCURSOR cur1;

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
for(i=0; i < kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;

  if(sravdok(row,&poisk) != 0)
    continue;

  naim_skl.new_plus("");  

  if(metka_naim_skl == 1)
   {
    sprintf(strsql,"select naik from Sklad where kod=%s",row[2]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_skl.new_plus(row1[0]);
   }

  if(row[0][0] == '1')
   bros.new_plus("+");
  if(row[0][0] == '2')
   bros.new_plus("-"); 
  if(row[7][0] == '0')
   bros.plus("?");
  if(row[8][0] == '0')
   bros.plus("*");
  if(row[14][0] == '0')
   bros.plus("$");

  kto_vvel[i]=atoi(row[9]);  

  rsdat(&d,&m,&g,row[1],2);

  if(metka_naim_skl == 0)  
   sprintf(str_menu,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-*s|%-*s|%-*s|%s",
   d,m,g,
   iceb_tu_kolbait(8,row[4]),row[4],
   iceb_tu_kolbait(6,row[3]),row[3],
   iceb_tu_kolbait(3,row[6]),row[6],
   iceb_tu_kolbait(3,row[2]),row[2],
   iceb_tu_kolbait(6,row[5]),row[5],
   iceb_tu_kolbait(6,row[11]),row[11],
   bros.ravno());

  if(metka_naim_skl == 1)  
   sprintf(str_menu,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-*s|%-*s|%-*s|%s|%s",
   d,m,g,
   iceb_tu_kolbait(8,row[4]),row[4],
   iceb_tu_kolbait(6,row[3]),row[3],
   iceb_tu_kolbait(3,row[6]),row[6],
   iceb_tu_kolbait(3,row[2]),row[2],
   iceb_tu_kolbait(6,row[5]),row[5],
   iceb_tu_kolbait(6,row[11]),row[11],
   bros.ravno(),naim_skl.ravno());


  spis_menu.spis_str.plus(str_menu);
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
shsmn(&poisk,kolstr);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-26);


if(kom == -7) //Изменение размера экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(sravdok(row,&poisk) != 0)
      continue;
    i++;
   }
  kom=kls;
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

    if(sravdok(row,&poisk) != 0)
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
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  GDITE();
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
    if(sravdok(row,&poisk) != 0)
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

  polen(str_vib.ravno(),&bros,1,'|');
  rsdat(&dnp,&mnp,&gnp,bros.ravno(),0);

  polen(str_vib.ravno(),&skld,5,'|');

  polen(str_vib.ravno(),&nomdokd,2,'|');
  polen(str_vib.ravno(),&kontr,3,'|');
  polen(str_vib.ravno(),&oper,4,'|');
  polen(str_vib.ravno(),&skladch,5,'|');


  polen(str_vib.ravno(),&bros,8,'|');
  if(bros.ravno()[0] == '+')
    tipzd=1;  
  if(bros.ravno()[0] == '-')
    tipzd=2;  

 }

vper:;
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(K)
 {  
  case TAB: /*переключение сортировки документов*/

    metka_sort_dok++;
    if(metka_sort_dok >= 2)
     metka_sort_dok=0;
    goto naz1;
    break;

  case ENTER:
  
     clear();
     dirdok(dnp,mnp,gnp,tipzd,skld,&nomdokd);
     dnp=mnp=gnp=0;
     
     clear();
    
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("matu2_3.txt");
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

    sprintf(strsql,"update Dokummat \
set \
pod=1 \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
    gnp,mnp,dnp,skld,nomdokd.ravno());

    sql_zapis(strsql,1,0);
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


    sprintf(strsql,"update Dokummat \
set \
pro=1 \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
    gnp,mnp,dnp,skld,nomdokd.ravno());

    sql_zapis(strsql,1,0);
    goto naz1;


  case FK4:  /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа.....(,,).."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));//1
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.....(,,).."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Код прихода/расхода.(,,).."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Код склада..........(,,).."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)......."));//6
    VV.VVOD_SPISOK_add_MD(gettext("Довереность.........(,,).."));//7
    VV.VVOD_SPISOK_add_MD(gettext("Номер нал. накладной.(,,)."));//8
    VV.VVOD_SPISOK_add_MD(gettext("Наименование конт-та.(,,)."));//9
    VV.VVOD_SPISOK_add_MD(gettext("Вид договора.........(,,)."));//10

    VV.VVOD_SPISOK_add_data(poisk.nomdokv.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(poisk.dat2.ravno(),11);
    VV.VVOD_SPISOK_add_data(poisk.kps.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.kprrz.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.skl11.ravno(),128);
    VV.VVOD_SPISOK_add_data(2);
    VV.VVOD_SPISOK_add_data(poisk.dover.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.nomnalnak.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.naim_kontr.ravno(),128);
    VV.VVOD_SPISOK_add_data(poisk.vid_dog.ravno(),128);


naz2:;
    poisk.metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);
    
    kl=VV.vvod(0,1,1,-1,-1);

    switch(kl)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
        poisk.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
        poisk.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
        poisk.nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));

        poisk.dn=poisk.mn=poisk.gn=0;
        poisk.dk=poisk.mk=poisk.gk=0;
                
        if(poisk.dat1.getdlinna() > 1)
         {
          if(rsdat(&d,&m,&g,poisk.dat1.ravno(),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto naz2;
           }
          poisk.dn=poisk.dk=d; poisk.mn=poisk.mk=m; poisk.gn=poisk.gk=g;
         }

        if(poisk.dat2.getdlinna() <= 1)
         {
          dpm(&poisk.dk,&poisk.mk,&poisk.gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,poisk.dat2.ravno(),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto naz2;
          }
         else
          {
           poisk.dk=d; poisk.mk=m; poisk.gk=g;
          }
         

        poisk.kps.new_plus(VV.VVOD_SPISOK_return_data(3));
        poisk.kprrz.new_plus(VV.VVOD_SPISOK_return_data(4));
        poisk.skl11.new_plus(VV.data_ravno(5));
        poisk.tipzz=0;
        if(VV.VVOD_SPISOK_return_data(6)[0] == '+')
          poisk.tipzz=1;
        if(VV.VVOD_SPISOK_return_data(6)[0] == '-')
          poisk.tipzz=2;
      
        poisk.dover.new_plus(VV.VVOD_SPISOK_return_data(7));
        poisk.nomnalnak.new_plus(VV.VVOD_SPISOK_return_data(8));
        poisk.naim_kontr.new_plus(VV.data_ravno(9));
        poisk.vid_dog.new_plus(VV.data_ravno(10));
        
        if(VV.VVOD_SPISOK_return_data(0)[0] != '\0' || VV.VVOD_SPISOK_return_data(1)[0] != '\0' || VV.VVOD_SPISOK_return_data(2)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(3)[0] != '\0' || VV.VVOD_SPISOK_return_data(4)[0] != '\0' || VV.VVOD_SPISOK_return_data(5)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(6)[0] != '\0' || VV.VVOD_SPISOK_return_data(7)[0] != '\0' || VV.VVOD_SPISOK_return_data(8)[0] != '\0' || VV.VVOD_SPISOK_return_data(9)[0] != '\0')
         poisk.metpoi=1;
        pozz=0;
        goto naz1;

      case FK3:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
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
            if(dirsklad(1,&kods,&naims) == 0)
             VV.data_z_plus(5,kods.ravno());
            clear();
            goto naz2;

          case 1 :
            clear();
            if(vibrek("Kontragent",&kods) == 0)
             VV.data_plus(3,kods.ravno());
            clear();
            goto naz2;

          case 2 :
            clear();
            if(dirprihod(1,&kods,&naims) == 0)
             VV.data_plus(4,kods.ravno());
            clear();
            goto naz2;

          case 3 :
            clear();
            if(dirrashod(1,&kods,&naims) == 0)
             VV.data_z_plus(4,kods.ravno());
            clear();
            goto naz2;
         }

        clear();
        goto naz2;

      case FK4:
        VV.VVOD_clear_data();
        goto naz2;
                   
      default:
        goto naz2;
     }

    goto naz1;

  case FK5: /*Расшифровка*/

    /*Читаем наименование организации*/

    if(kontr.ravno()[0] == '0' && kontr.ravno()[1] == '0' && kontr.ravno()[2] == '-')
      polen(kontr.ravno(),&bros,1,'-');
    else
      bros.new_plus(kontr.ravno());

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw("\n%s %s\n",gettext("Не найден код контрагента"),kontr.ravno());
      OSTANOV();
      naimo.new_plus("");
     }
    else
     {
      naimo.new_plus(row1[0]);
     } 

    /*Читаем наименование операции*/
    if(tipzd == 1)
      bros.new_plus("Prihod");
    if(tipzd == 2)
      bros.new_plus("Rashod");
    sprintf(strsql,"select naik from %s where kod='%s'",
    bros.ravno(),oper.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден код операции %s !"),oper.ravno());
      printw("\n");
      OSTANOV();
     naimpr.new_plus("");
     }
    else
     {
      naimpr.new_plus(row1[0]);
     } 

    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Sklad where kod=%s",
    skladch.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден склад %d в списке складов !"),atoi(skladch.ravno()));
      printw("\n");
      OSTANOV();
      naiskl.new_plus("");
     }
    else
     {
      naiskl.new_plus(row1[0]);
     } 
    SOOB.VVOD_delete();
    
    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докум. :"),dnp,mnp,gnp);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",gettext("Документ    :"),nomdokd.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Склад       :"),skladch.ravno(),naiskl.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);             

    sprintf(strsql,"%s %s %s",gettext("Контрагент  :"),kontr.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Операция    :"),oper.ravno(),naimpr.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);             

    if(kto_vvel[kom] != 0)
     {
      sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(kto_vvel[kom]));
      SOOB.VVOD_SPISOK_add_MD(strsql);             
     }
    /*Читаем время записи*/
    sprintf(strsql,"select vrem from Dokummat where tip=%d and \
datd='%04d-%d-%d' and sklad=%s and nomd='%s'",
    tipzd,gnp,mnp,dnp,skladch.ravno(),nomdokd.ravno());

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    if(atoi(row1[0]) > 0)
     {
      tmm=atoi(row1[0]);
      bf=localtime(&tmm);
      sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
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
      clearstr(LINES-1,0);
      smdok_vk(&poisk,&cur,kolstr);
     
      goto naz1;

  case FK7: //День назад
      poisk.metpoi=1;
      dpm(&poisk.dn,&poisk.mn,&poisk.gn,2);
      poisk.dk=poisk.dn; poisk.mk=poisk.mn; poisk.gk=poisk.gn;
      goto naz1;

  case SFK7: //День вперед
      poisk.metpoi=1;
      dpm(&poisk.dn,&poisk.mn,&poisk.gn,1);
      poisk.dk=poisk.dn; poisk.mk=poisk.mn; poisk.gk=poisk.gn;
      goto naz1;

  case FK8: /*с наименованием склада*/
    metka_naim_skl++;
    if(metka_naim_skl > 1)
     metka_naim_skl=0;
    goto naz1;

  case FK9: /*переключение просмотра оплаченных и неоплаченных документов*/
    poisk.metka_opl++;
    if(poisk.metka_opl > 2)
     poisk.metka_opl=0;
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
void shsmn(class smdok_poi *poisk,int koldok)
{
int		i1;
int		X;
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
printw(" %s:%d%s %s:%d",gettext("Стартовый год"),poisk->god_start,gettext("г."),gettext("Кол.док."),koldok);

move(1,0);
printw("      1          2      3     4   5     6     7");

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
printw("5 %s",gettext("код склада"));
move(++i1,X);
printw(gettext("6 номер нал. накл."));
move(++i1,X);
printw(gettext("7 номер парн.докум."));
//move(++i1,X);
//printw(gettext("8 кто ввел"));
move(++i1,X);
printw(gettext("+/- приход/расход"));
move(++i1,X);
printw(gettext("? не подтвержден"));
move(++i1,X);
printw(gettext("* не вып-ны проводки"));
move(++i1,X);
printw("$ %s",gettext("не оплачено"));

move(i1+=2,X=COLS-21);
if(poisk->metpoi == 1)
 {
  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(poisk->nomdokv.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Н.д."),poisk->nomdokv.ravno());
   }

  if(poisk->dn != 0)
   {
    move(++i1,X);
    printw("%s %d.%d.%d",gettext("Дата н."),poisk->dn,poisk->mn,poisk->gn);
   }

  if(poisk->dk != 0)
   {
    move(++i1,X);
    printw("%s %d.%d.%d",gettext("Дата к."),poisk->dk,poisk->mk,poisk->gk);
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
    printw("%s %s",gettext("Склад"),poisk->skl11.ravno());
   }
  if(poisk->tipzz != 0)
   {
    move(++i1,X);
    if(poisk->tipzz == 1)
      printw(gettext("ТОЛЬКО ПРИХОДЫ"));
    if(poisk->tipzz == 2)
      printw(gettext("ТОЛЬКО РАСХОДЫ"));
   }
  if(poisk->dover.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Довер."),poisk->dover.ravno());
   }  
  if(poisk->nomnalnak.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Нал.накладная"),poisk->nomnalnak.ravno());
   }  
  if(poisk->naim_kontr.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Наим.контр."),poisk->naim_kontr.ravno());
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
"F7",gettext("пред.день"),
"F8",gettext("наим.склада"),
"F9",gettext("оплата"),
"F10",gettext("выход"),NULL);
}

/*****************************
Проверка реквизитов поиска
Возвращяем 0- если все равно
           1-если не совпадает
*******************************/           
int sravdok(SQL_str row, //Строка для проверки
class smdok_poi *poisk)
{
short		d,m,g;
SQL_str         row1;
int		tipz1;
char		strsql[1024];
short		pro,pod;

/*
printw("sravdok-mpn=%d pro=%d\n",mpn,pro);
OSTANOV();
*/
pro=atoi(row[8]);
int metka_opl=atoi(row[14]);

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
rsdat(&d,&m,&g,row[1],2);

if(poisk->dn != 0)
 if(SRAV1(poisk->gn,poisk->mn,poisk->dn,g,m,d) < 0)
   return(1);

if(poisk->dk != 0)
 if(SRAV1(poisk->gk,poisk->mk,poisk->dk,g,m,d) > 0)
   return(1);
  
if(proverka(poisk->kps.ravno(),row[3],0,0) != 0)
  return(1);

if(proverka(poisk->kprrz.ravno(),row[6],0,0) != 0)
  return(1);

if(proverka(poisk->skl11.ravno(),row[2],0,0) != 0)
 return(1);

if(poisk->tipzz != 0 && poisk->tipzz != tipz1)
   return(1);

if(proverka(poisk->nomdokv.ravno(),row[4],0,0) != 0)
 if(proverka(poisk->nomdokv.ravno(),row[11],0,0) != 0)
  return(1);

if(poisk->dover.getdlinna() > 1)
 {
  SQLCURSOR cur1;
  /*Читаем номер доверенности*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%s and nomerz=1",
  g,row[4],row[2]);

  if(readkey(strsql,&row1,&cur1) == 1)
   { 
    class iceb_tu_str dov("");
    polen(row1[0],&dov,1,'#');
    if(proverka(poisk->dover.ravno(),dov.ravno(),0,0) != 0)
     return(1);
   }
  else
   return(1);
 }


if(proverka(poisk->nomnalnak.ravno(),row[5],0,0) != 0)
 return(1);

if(poisk->naim_kontr.getdlinna() > 1)
 {
  class SQLCURSOR cur1;
  class iceb_tu_str kodkontr("");
  if(polen(row[3],&kodkontr,1,'-') != 0)
   kodkontr.new_plus(row[3]);
  sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'",kodkontr.ravno_filtr());
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(poisk->naim_kontr.ravno(),row1[0],4,0) != 0)
     if(proverka(poisk->naim_kontr.ravno(),row1[1],4,0) != 0)
      return(1);     
   }
  
 }

if(poisk->vid_dog.getdlinna() > 1)
 {
  class SQLCURSOR cur1;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and nomd='%s' and nomerz=7",g,row[2],row[4]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(poisk->vid_dog.ravno(),row1[0],4,0) != 0)
      return(1);     
        
   }
  else
   return(1);
 }
return(0);

}
/*************************/
/*Создание кассовых ордеров*/
/**************************/
void smdok_ko(int tipz, /*1-приходный 2-расходный*/
class smdok_poi *poisk,SQLCURSOR *cur)
{
int tipz_mat=0;
char		strsql[1024];
SQL_str		row,row1;
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

VV.VVOD_SPISOK_add_data(kassa.ravno(),10);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kodop.ravno(),20);

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

    kod.new_plus("");
    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        naimkas.new_plus("");
        if(dirkas(1,&kod,&naimkas) == 0)
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

short vt=0;
iceb_t_poldan("Код группы возвратная тара",strsql,"matnast.alx");
vt=atoi(strsql);

class SPISOK kod_kontr;
class SPISOK koment_kontr;
class masiv_din_double suma_kontr;



class iceb_tu_str nomd("");


class iceb_tu_str koment("");
double suma=0.;
double sumabn=0.;
double sumkor=0.;
short d,m,g;
int lnds=0;
double kolih=0.;
double suma_nds=0.;
double suma_dok=0.;
float pnds=0.;
if(tipz == 1)
 tipz_mat=2;
else
 tipz_mat=1;
int nomer_kontr=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(tipz == atoi(row[0])) /*Если приходный кассовый ордер то нам нужны расходные документы и наооборот*/
   continue;
  
  if(sravdok(row,poisk) != 0)
    continue;
  
  rsdat(&d,&m,&g,row[1],2);

  pnds=atof(row[13]);

  /*Узнаем НДС документа*/
  lnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(readkey(strsql,&row1,&cur1) == 1)
    lnds=atoi(row1[0]);
  suma_nds=0.;
  if(lnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      suma_nds=atof(row1[0]);
   }

  sumzap1(d,m,g,row[4],atoi(row[2]),&suma,&sumabn,lnds,vt,&sumkor,tipz_mat,&kolih);

  if(lnds == 0 && suma_nds == 0.)
    suma_nds=(suma+sumkor)*pnds/100.;
  
  suma_dok=suma+sumkor+suma_nds+sumabn;
    
//  sprintf(koment,"%s:%.10g",gettext("Количество"),kolih);  
  koment.new_plus(gettext("Количество"));
  koment.plus(":");
  koment.plus(kolih);
  
  if(metka_ks == 0)
   {
    lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_t_nomnak(gko,kassa.ravno(),&nomd,tipz,2,1);
    zaphkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),shetk.ravno(),kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","");
    printw("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    zapzkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),row[4],suma_dok,"",koment.ravno());
   }
  else
   {
    if((nomer_kontr=kod_kontr.find_r(row[3])) < 0)
     {
      kod_kontr.plus(row[3]);
      koment_kontr.plus(koment.ravno());
     }

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
    
    zapzkor(0,kassa.ravno(),tipz,dko,mko,gko,nomd.ravno(),kod_kontr.ravno(nomer_kontr),suma_kontr.ravno(nomer_kontr),"",koment_kontr.ravno(nomer_kontr));
   }
 }
OSTANOV();

}
/**************************************/
/*Экспорт накладных*/
/***************************************/
void smdok_eks_nak(class smdok_poi *poisk)
{
SQL_str row;
short dd=0,md=0,gd=0;
class iceb_tu_str imaf("eksdokmu");
imaf.plus(getpid());
imaf.plus(".csv");
char strsql[1024];
int kolstr=0;
class SQLCURSOR cur;

GDITE();

/*Сначала должны выгружаться приходные документы и только потом расходные*/
sprintf(strsql,"select * from Dokummat where datd >= '%04d-01-01'",poisk->gn);

if(poisk->dn != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d'",poisk->gn,poisk->mn,poisk->dn);

if(poisk->dn != 0 && poisk->dk != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",poisk->gn,poisk->mn,poisk->dn,poisk->gk,poisk->mk,poisk->dk);

strcat(strsql,"order by tip asc,datd asc,nomd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }




class iceb_fopen fil;
if(fil.start(imaf.ravno(),"w") != 0)
 return;
iceb_t_zageks(gettext("Экспорт документов из подсистемы \"Материальный учёт\""),fil.ff);
//cur->poz_cursor(0);
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(sravdok(row,poisk) != 0)
    continue;
  rsdat(&dd,&md,&gd,row[1],2);
  
  eks_dok_mu(dd,md,gd,row[4],atoi(row[2]),&imaf,fil.ff);
 
 }

fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf.ravno());
oth.spis_naim.plus(gettext("Экспорт документов"));

iceb_t_rabfil(&oth,"");

}



/**********************/
/*Выбор типа кассового ордера*/
/*****************************/
void smdok_vk(class smdok_poi *poisk,SQLCURSOR *cur,int kolstr)
{
int tipz=0;
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Виберите нужное и нажми те Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Экспортировать кассовые ордера для приходных документов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Экспортировать кассовые ордера для расходных документов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Экспортировать документы в файл"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3:
  case -1 :
    return;

  case 0:
    tipz=2;
    smdok_ko(tipz,poisk,cur);
    break;

  case 1:
    tipz=1;
    smdok_ko(tipz,poisk,cur);
    break;

  case 2:
    smdok_eks_nak(poisk);
    break;
 }

}
