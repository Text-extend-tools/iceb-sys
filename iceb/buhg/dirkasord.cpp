/* $Id: dirkasord.c,v 5.97 2014/04/30 06:12:26 sasa Exp $ */
/*08.03.2018	27.09.2000	Белых А.И.	dirkasord.c
Просмотр записей в кассовом ордере
*/
#include        <errno.h>
#include	"buhg.h"

extern int kol_strok_na_liste;

void	sdirkasord(const char[],const char[],short,short,short,short,double,int,const char[],short,const char[]);
void	raszko(const char*,const char*,short,short,short,SQLCURSOR*,const char*,const char*);
double dirkasord_suma_den(short dd,short md,short gd,const char *kodkontr);
void vkask(const char *shetd,const char *kassa,short dd,short md,short gd,short tipz,const char *nomd,const char *kod_kontr);

extern class iceb_tu_str nameprinter;

void dirkasord(const char *kassadok,const char *nomdok,short dd,short md,short gd,short tipz)
{
class iceb_t_mstr spis_menu(0);
class spis_oth oth;
class iceb_tu_str vstr("");
char		strsql[1024];
short		mpn; /*Метка пустой накладной*/
short           x=0,y=3;
short		kls;
SQL_str		row,row1;
SQLCURSOR curr;
int		kolstr;
class iceb_tu_str shetd(""); //Счёт документа
class iceb_tu_str shetkasd(""); //Счёт кассы для записи в кассовій ордер
class iceb_tu_str kodop("");
class iceb_tu_str osnovo("");
class iceb_tu_str osnov1("");
class iceb_tu_str osnov2("");
class iceb_tu_str dopol("");
class iceb_tu_str fio("");
class iceb_tu_str dokum("");
int		prc;
int		pozz,pozz1;
int		i,i1,K,kom;
double		suma;
short		prov;
class iceb_tu_str naimkont("");
short		d,m,g;
class iceb_tu_str kontr("");
char		imaf[64];
int		tipop; //0-для операции не нужно делать проводок 1-нужно
class iceb_tu_str bros("");
class iceb_tu_str naimop("");
class iceb_tu_str nomv("");
class iceb_tu_str nom_dok(nomdok);
class iceb_tu_str kas_sa(kassadok);

VVOD  VVOD1(2);
VVOD DANET(1);
VVOD MENU(3);
naz2:;
pozz=mpn=0;

GDITE();

/*Читаем шапку документа*/
sprintf(strsql,"select shetk,kodop,osnov,dopol,fio,dokum,prov,nomv from Kasord \
where nomd='%s' and kassa=%s and tp=%d and god=%d",
nom_dok.ravno(),kas_sa.ravno(),tipz,gd);
if(readkey(strsql,&row,&curr) != 1)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s %s %s %s %s %d.%d.%d%s %s !",
  gettext("Документ"),
  nom_dok.ravno(),
  gettext("Касса"),
  kas_sa.ravno(),
  gettext("Дата"),  
  dd,md,gd,
  gettext("г."),gettext("не найден"));
  SOOB.VVOD_SPISOK_add_MD(strsql);

  if(tipz == 1)
    sprintf(strsql,gettext("Приход"));
  if(tipz == 2)
    sprintf(strsql,gettext("Расход"));

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
nomv.new_plus(row[7]);
shetd.new_plus(row[0]);
kodop.new_plus(row[1]);

if(polen(row[2],&osnov1,1,'|') != 0)
  osnov1.new_plus(row[2]);
else
   polen(row[2],&osnov2,2,'|');

dopol.new_plus(row[3]);
fio.new_plus(row[4]);
dokum.new_plus(row[5]);
prov=atoi(row[6]);

//Читаем счёт кассы
sprintf(strsql,"select shet from Kas where kod=%s",kas_sa.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  shetkasd.new_plus(row[0]);
class iceb_tu_str kod_cel_naz("");
//Читаем нужно ли делать проводки для этой операции и код целевого назначения
if(tipz == 1)
 sprintf(strsql,"select naik,shetkas,shetkor,metkapr,kcn from Kasop1 where kod='%s'",kodop.ravno());
if(tipz == 2)
 sprintf(strsql,"select naik,shetkas,shetkor,metkapr,kcn from Kasop2 where kod='%s'",kodop.ravno());



tipop=0;
if(readkey(strsql,&row1,&curr) == 1)
 {
  naimop.new_plus(row1[0]);
  if(row1[3][0] == '1')
    tipop=1;
  kod_cel_naz.new_plus(row1[4]);
 }
if(tipop == 0)
 prov=1;

kom=0;

naz1:;

GDITE();
kls=LINES-5;
//Создаем класс меню
/*MEN5 MM5(kls,COLS,0);*/
suma=0.;
sprintf(strsql,"select * from Kasord1 where datd='%d-%02d-%02d' and \
kassa=%s and nomd='%s' and tp=%d order by kontr asc",
gd,md,dd,kas_sa.ravno(),nom_dok.ravno(),tipz);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
  {

lll:;
   sdirkasord(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,tipz,suma,kolstr,shetd.ravno(),prov,kodop.ravno());

   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
   K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
   mpn=1;
   
   if(K == FK4 || K == SFK2 || K == FK3 || K == FK7 || K == SFK7 || K == FK5 || K == FK8 || K == ENTER)
      goto lll;
   if(K == KEY_RESIZE)
     goto naz1;
     
   goto vper;
  }
 else
  {
   move(0,0);
   printw(gettext("Вычисляем сумму по документу\n"));
   refresh();
   while(cur.read_cursor(&row) != 0)
     suma+=atof(row[6]);
   printw(gettext("Конец расчёта\n"));
   refresh();
  }

naz:;

cur.poz_cursor(pozz);
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  mpn=0;
  pozz1++;
  /*Читаем наименование контрагента*/
  naimkont.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[5]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naimkont.new_plus(row1[0]);
  
  rsdat(&d,&m,&g,row[3],2);

  if(d == 0)
   sprintf(strsql,"%-*s|%-*.*s|%8.2f|%10s|%s",
   iceb_tu_kolbait(8,row[5]),row[5],
   iceb_tu_kolbait(30,naimkont.ravno()),iceb_tu_kolbait(30,naimkont.ravno()),naimkont.ravno(),
   atof(row[6]),"",row[11]);
  else
   sprintf(strsql,"%-*s|%-*.*s|%8.2f|%02d.%02d.%d|%s",
   iceb_tu_kolbait(8,row[5]),row[5],
   iceb_tu_kolbait(30,naimkont.ravno()),iceb_tu_kolbait(30,naimkont.ravno()),naimkont.ravno(),
   atof(row[6]),d,m,g,row[11]);

  spis_menu.spis_str.plus(strsql);
  i++;

 }


if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
    
if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

clear();

sdirkasord(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,tipz,suma,kolstr,shetd.ravno(),prov,kodop.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  if(pozz != 0)
    pozz--;
  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i == kls)
    pozz+=kls;
  else
    beep();
/*
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  pozz-=kls;
  if(pozz < 0)  
   {
    beep();
    pozz=0;
   }
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&kontr,1,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     break;


  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("kas2_1_1.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case SFK1:   /*Корректировать шапку*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

   d=dd; m=md; g=gd;
   if(vkasord(tipz,1,&kas_sa,&nom_dok,&d,&m,&g) == 0)
    {
     dd=d; md=m; gd=g;
    }
   goto naz2;

  case FK2:   /*Ввести запись*/
  case PLU:   /*Ввести запись*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }


    vkask(shetd.ravno(),kas_sa.ravno(),dd,md,gd,tipz,nom_dok.ravno(),"");
    goto naz1;

  case ENTER:
  case SFK2:   /*Корректировать запись*/

    
    vkask(shetd.ravno(),kas_sa.ravno(),dd,md,gd,tipz,nom_dok.ravno(),kontr.ravno());
    goto naz1;

  case FK3:   /*Удалить запись*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Kasord1 where kassa=%s and tp=%d \
and nomd='%s' and kontr='%s' and god=%d",
      kas_sa.ravno(),tipz,nom_dok.ravno(),kontr.ravno(),gd);
     if(sql_zap(&bd,strsql) != 0)
      {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      goto naz1;
     }          
    goto naz;

  case SFK3:   /*Удалить весь документ*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    /*Смотрим заблокированы ли проводки к этому документа*/
    if(iceb_t_pbp(7,dd,md,gd,nom_dok.ravno(),atoi(kas_sa.ravno()),tipz,gettext("Удалить документ невозможно!")) != 0)
      goto naz;
    
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      if(udprgr(ICEB_MP_KASA,dd,md,gd,nom_dok.ravno(),atoi(kas_sa.ravno()),tipz) != 0)
       goto naz1;
      if(udkasdok(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,tipz) == 0)
        return;
     }          
    goto naz1;

  case FK4:   //Проводки
    if(tipop == 0)
     {
      iceb_t_soob(gettext("Для этой операции проводки не выполняются!"));
      goto naz;
     }

    if(dirprkas(dd,md,gd,nom_dok.ravno(),kas_sa.ravno(),kodop.ravno(),tipz,shetd.ravno()) != 0)
      break;

    goto naz2;

  case FK5:   /*Распечатать*/

fff5:;
    MENU.VVOD_delete();

    memset(strsql,'\0',sizeof(strsql));
    if(nameprinter.getdlinna() > 1)
     sprintf(strsql,"%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,nameprinter.ravno()),nameprinter.ravno());
    else
     sprintf(strsql,"%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));

    MENU.VVOD_SPISOK_add_ZAG(strsql);

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка кассового ордера"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка списка контрагентов в документе"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3
    clearstr(LINES-1,0);
    i1=0;
    while(menu3w(stdscr,&MENU,&i1,-1,-1,0) != 0);
    switch(i1)
     {
      case FK10:
      case -1:
      case 3:
        goto naz;

      case 0:

        if(fio.ravno()[0] == '\0' && kolstr == 1)
         {
          cur.poz_cursor(0);
          cur.read_cursor(&row);
          /*Читаем наименование контрагента*/
          sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[5]);
          if(readkey(strsql,&row1,&curr) == 1)
            fio.new_plus(row1[0]);
          
          if(tipz == 2 && dokum.ravno()[0] == '\0')
           {
             if(iceb_tu_srav_r(ICEB_ZAR_PKTN,row[5],1) == 0)
              {
               i=strlen(ICEB_ZAR_PKTN);
               sprintf(strsql,"select nomp,vidan,datvd from Kartb where tabn=%s",&row[5][i]);
               if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
                {
                 short d=0,m,g;
                 rsdat(&d,&m,&g,row1[2],2);
                 if(d == 0)
                  {
                   sprintf(strsql,"%s N%s %s",gettext("Паспорт"),row1[0],row1[1]);
                  }
                 else
                  sprintf(strsql,"%s N%s %02d.%02d.%d%s %s",
                  gettext("Паспорт"),row1[0],
                  d,m,g,
                  gettext("г."),
                  row1[1]);

                 dokum.new_plus(strsql);
                }
              }
             else
              {
               sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Паспорт|%%'",row[5]);
               if(readkey(strsql,&row1,&curr) == 1)
                 polen(row1[0],&dokum,2,'|');
              }
           }
         }     

        osnovo.new_plus(""); //Обязательно
        osnovo.plus(osnov1.ravno());
        osnovo.plus(" ",osnov2.ravno());
        if(tipz == 1)
         {
          sprintf(imaf,"kopp%d.lst",getpid());
          kasord1(imaf,nom_dok.ravno(),dd,md,gd,shetd.ravno(),shetkasd.ravno(),suma,fio.ravno(),osnovo.ravno(),dopol.ravno(),kod_cel_naz.ravno(),NULL);

          oth.clear();
          oth.spis_imaf.plus(imaf);
          oth.spis_naim.plus(gettext("Приходной кассовый ордер"));
          iceb_t_rabfil(&oth,"");
         }

        if(tipz == 2)
         {
          short ddp=dd,mdp=md,gdp=gd;
          class iceb_tu_str nomdok_p(nom_dok.ravno());
          
          if(iceb_t_poldan("Не печатать в расходных ордерах дату и номер документа",strsql,"kasnast.alx") == 0)
           if(SRAV("Вкл",strsql,1) == 0)
            {
           //  ddp=mdp=0;
             nomdok_p.new_plus("");
            }  
          sprintf(imaf,"kopr%d.lst",getpid());
          kasord2(imaf,nomdok_p.ravno(),ddp,mdp,gdp,shetd.ravno(),shetkasd.ravno(),suma,fio.ravno(),osnovo.ravno(),dopol.ravno(),dokum.ravno(),kod_cel_naz.ravno(),NULL);
          
          oth.clear();
          oth.spis_imaf.plus(imaf);
          oth.spis_naim.plus(gettext("Расходный кассовый ордер"));
          iceb_t_rabfil(&oth,"");
         }
        break;

      case 1:
	raszko(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,&cur,naimop.ravno(),nomv.ravno());
        break;

      case 2 :  //Сменить имя принтера
        mprintcap();
        goto fff5;
     }        
    goto naz;

  case FK7:   /*Подтвердить запись*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }
    sprintf(strsql,"%d.%d.%d",dd,md,gd);
    vstr.new_plus(strsql);
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату подтверждения"));
    
    if(vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       goto naz;


    if(rsdat(&d,&m,&g,vstr.ravno(),0) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto naz;
     }

   sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s' and kontr='%s'",
   g,m,d,kas_sa.ravno(),gd,tipz,nom_dok.ravno(),kontr.ravno());

   sql_zapis(strsql,0,0);

   goto naz1;

  case SFK7:   /*Подтвердить все записи*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить весь документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
      
    sprintf(strsql,"%d.%d.%d",dd,md,gd);
    vstr.new_plus(strsql);
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату подтверждения"));

    if(vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       return;


    if(rsdat(&d,&m,&g,vstr.ravno(),0) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      return;
     }
   sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
   g,m,d,kas_sa.ravno(),gd,tipz,nom_dok.ravno());

   sql_zapis(strsql,0,0);
      
   goto naz1;

  case FK8:   /*Снять подтверждение*/
    if(pbkas(md,gd) != 0)
     {
      if(kolstr > 0)
        goto naz;
      else
       goto lll;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять подтверждение со всего документа ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

   sprintf(strsql,"UPDATE Kasord1 set datp='0000-00-00' where \
kassa=%s and god=%d and tp=%d and nomd='%s'",
   kas_sa.ravno(),gd,tipz,nom_dok.ravno());

   sql_zapis(strsql,0,0);
      
   goto naz1;
       
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

if(mpn == 1)
 {
  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ?"));

  if(danet(&DANET,2,stdscr) == 1)
   {
    GDITE();
    if(udprgr(ICEB_MP_KASA,dd,md,gd,nom_dok.ravno(),atoi(kas_sa.ravno()),tipz) != 0)
     goto naz1;
    udkasdok(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,tipz);
   }
  return;
 }

provpodko(kas_sa.ravno(),nom_dok.ravno(),dd,md,gd,tipz);
provprkas(kas_sa.ravno(),tipz,nom_dok.ravno(),dd,md,gd,kodop.ravno());

if(nomv.getdlinna() > 1)
 {
  sprintf(strsql,"update Kasnomved set suma=%.2f,ktoi=%d,vrem=%ld  where god=%d and nomv='%s'",
  suma,
  iceb_t_getuid(),
  time(NULL),
  gd,
  nomv.ravno());
  sql_zapis(strsql,0,0);
 }     

}

/****************************/
/*  Шапка                   */
/*****************************/
void sdirkasord(const char kassa[],const char nomd[],short dd,short md,short gd,
short tipz,double suma,int kolz,const char shetd[],short prov,const char kodop[])
{
clear();

printw("%s:%s %s:%d.%d.%d%s %s:%s ",
gettext("Касса"),kassa,
gettext("Дата"),dd,md,gd,gettext("г."),
gettext("Документ"),nomd);
if(tipz == 1)
  printw("%s %s",kodop,gettext("ПРИХОД"));
if(tipz == 2)
  printw("%s %s",kodop,gettext("РАСХОД"));
if(prov == 0)
 {
  attron(A_BLINK);
  printw(" %s",gettext("Проводки не сделаны !"));
  attroff(A_BLINK);
 }
move(1,0);
printw("%s:%s %s:%d %s:%.2f",
gettext("Счёт"),shetd,
gettext("Количество записей"),kolz,
gettext("Сумма"),suma);

short		y=2;
short           x=COLS-20;

mvprintw(2,0,"   1                     2                    3        4      5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код контр."));
mvprintw(y++,x,"2-%s",gettext("ФИО"));
mvprintw(y++,x,"3-%s",gettext("cумма"));
mvprintw(y++,x,"4-%s",gettext("дата подтв."));
mvprintw(y++,x,"5-%s",gettext("комментарий"));



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("пров."),
"F5",gettext("печать"),
"F7",gettext("подтв."),
"F8",gettext("подтв.-"),
//"F9",gettext("меню"),
"F10",gettext("вых."),NULL);

}

/**********************************/
/*Ввод и корректировка записи     */
/**********************************/

void vkask(const char *shetd,const char *kassa,short dd,short md,
short gd,short tipz,const char *nomd,const char *kod_kontr)
{
int		K=0,N=0;
struct OPSHET   shetv;
class iceb_tu_str naimkont("");
char		strsql[1024];
SQL_str		row;
class iceb_tu_str kontrz("");
class iceb_tu_str kods("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");
SQLCURSOR cur;
double suma_den=0.;
double max_sum=0.;
class iceb_tu_str kod_val("");
sprintf(strsql,"%04d-%02d-%02d",gd,md,dd);
class iceb_tu_str data_ord(strsql);
class iceb_tu_str bros("");
int ktoz=0;
time_t vrem=0;
int metka=0;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(100);

iceb_t_poldan("Максимальная сумма за день от контрагента",&bros,"kasnast.alx");

sprintf(strsql,"select kv from Kas where kod=%d",atoi(kassa));
if(readkey(strsql,&row,&cur) == 1)
 kod_val.new_plus(row[0]);

class iceb_getkue_dat getval(kod_val.ravno());

max_sum=getval.toue(bros.ravno(),data_ord.ravno());



if(kod_kontr[0] != '\0')
 {
  metka=1;
  sprintf(strsql,"select suma,ktoi,vrem,koment from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
  atoi(kassa),gd,tipz,nomd,kod_kontr);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;
   }
  VV.data_plus(0,kod_kontr);
  VV.data_plus(1,row[0]);
  ktoz=atoi(row[1]);
  vrem=atol(row[2]);
  VV.data_plus(2,row[3]);  
  
  if(tipz == 1)
    suma_den=dirkasord_suma_den(dd,md,gd,VV.data_ravno_filtr(0));
 }


VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Сумма............"));//1
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......."));//2


kontrz.new_plus(VV.VVOD_SPISOK_return_data(0));

/*Определяем тип счета*/
prsh(shetd,&shetv);

K=N=0;

if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  VV.VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naimkont.new_plus(row[0]);
 }

naz:;

VV.VVOD_delete_ZAG();

if(naimkont.ravno()[0] != 0)
 {
  sprintf(strsql,"%s:%s",gettext("Контрагент"),naimkont.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(tipz == 1 && suma_den != 0.)
 {

  sprintf(strsql,"%s:%.2f (%.2f)",gettext("Cумма уже полученная за день"),suma_den,max_sum);
  VV.VVOD_SPISOK_add_ZAG(strsql);
  
 }
if(ktoz != 0)
 kzvz(ktoz,vrem,&VV); 

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("контрагенты"),
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

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      if(SRAV(kontrz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
       {
        /*Проверяем может он уже введён в ордер*/
        sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%d and tp=%d and nomd='%s' and kontr='%s'",
        kassa,gd,tipz,nomd,VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          sprintf(strsql,gettext("Контрагент %s в ордере уже есть !"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          VV.data_plus(N,"");
          goto naz;
         }
       }

      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        VV.data_plus(N,"");
        goto naz;
       }
      else
        naimkont.new_plus(row[0]);

      if(shetv.saldo == 3)
       {
        sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shetd,VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
         {
          sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",
          shetd,
          VV.VVOD_SPISOK_return_data(N),
          iceb_t_getuid(),time(NULL));
          if(sql_zapis(strsql,1,0) != 0)
           goto naz;

         }
       }      

      if(tipz == 1)
        suma_den=dirkasord_suma_den(dd,md,gd,VV.data_ravno_filtr(0));

      N++;
      goto naz;
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0' && tipz == 1)
     {
      if(max_sum < suma_den+VV.data_ravno_atoi(N))
       {
        sprintf(strsql,"%s %.2f %s %.2f",gettext("Сумма полученная от контрагента за день"),suma_den+VV.data_ravno_atoi(N),gettext("больше максимально возможной"),max_sum);
        iceb_t_soob(strsql);
       }       
            
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
 
     if(pbkas(md,gd) != 0)
      goto naz;
      
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }

     if(SRAV(kontrz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
      {
       /*Проверяем может он уже введён в ордер*/
       sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%d and tp=%d and nomd='%s' and kontr='%s'",
       kassa,gd,tipz,nomd,VV.VVOD_SPISOK_return_data(0));
       if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
        {
         sprintf(strsql,gettext("Контрагент %s в ордере уже есть !"),VV.VVOD_SPISOK_return_data(0));
         iceb_t_soob(strsql);
         goto naz;
        }
      }


     if(tipz == 1 && max_sum != 0.)
      {
       suma_den=dirkasord_suma_den(dd,md,gd,VV.data_ravno_filtr(0));
       if(max_sum < suma_den+VV.data_ravno_atoi(1))
        {
         sprintf(strsql,"%s %.2f %s %.2f",gettext("Сумма полученная от контрагента за день"),suma_den+VV.data_ravno_atoi(1),gettext("больше максимально возможной"),max_sum);
         iceb_t_soob(strsql);
        }       
      }
      
     int kodv=0;

     if((kodv=zapzkor(metka,kassa,tipz,dd,md,gd,nomd,VV.VVOD_SPISOK_return_data(0),ATOFM(VV.VVOD_SPISOK_return_data(1)),
     kontrz.ravno(),VV.data_ravno_filtr(2))) != 0)
      {
       if(kodv == 1)
        {
         sprintf(strsql,gettext("Контрагент %s в ордере уже есть !"),VV.VVOD_SPISOK_return_data(0));
         iceb_t_soob(strsql);
        }
       goto naz;

      }
     break;
    }

   if(K == FK3) /*Просмотр списка контрагентов*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     memset(strsql,'\0',sizeof(strsql));
     if(shetv.saldo == 3)
      {
       class iceb_tu_str naim("");
       if(dirskont(shetd,&kods,&naim,1) == 0)
        {
         VV.VVOD_SPISOK_zapis_data(0,kods.ravno());
         naimkont.new_plus(naim.ravno());
         goto naz;
        }
      }
     else
      {
       naim.new_plus("");
       if(dirkontr(&kod,&naim,1) == 0)
        {
         VV.VVOD_SPISOK_zapis_data(0,kod.ravno());
         naimkont.new_plus(naim.ravno());
         goto naz;
        }
      }
     goto naz;
    }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();



}
/***************************************/
/*Распечатать записи в кассовом ордере*/
/**************************************/

void raszko(const char *kassa,const char *nomd,short dd,short md,short gd,SQLCURSOR *cur,const char *naimop,const char *nomerved)
{
char	imaf[64],imafved[64];
FILE	*ff,*ffved;
class iceb_tu_str naimkont("");
SQL_str row,row1;
char	strsql[512];
double  itogo=0.;
double  suma=0.;
SQLCURSOR curr;

sprintf(imaf,"kasor%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Распечатка списка контрагентов в кассовом ордере"),0,0,0,0,0,0,ff);

fprintf(ff,"%s:%s\n",gettext("Касса"),kassa);
fprintf(ff,"%s:%d.%d.%d%s\n",gettext("Дата документа"),dd,md,gd,
gettext("г."));
fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomd);


fprintf(ff,"\
-----------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код |   Наименование контрагента   |  Сумма   |Коментарий\n"));
fprintf(ff,"\
-----------------------------------------------------------------\n");

sprintf(imafved,"kasved%d.lst",getpid());

if((ffved = fopen(imafved,"w")) == NULL)
 {
  error_op_nfil(imafved,errno,"");
  return;
 }


int listn=1;
int kolst=0;

kolst=0;
if(zarstfn53("",md,gd,"",nomerved,&kolst,ffved) != 0)
  return;
sspp1(ffved,listn,&kolst);
int nomer_ved=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  /*Читаем наименование контрагента*/
  naimkont.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[5]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naimkont.new_plus(row1[0]);
  suma=atof(row[6]);

  fprintf(ff,"%-*s|%-*.*s|%10.2f|%.*s\n",
  iceb_tu_kolbait(5,row[5]),row[5],
  iceb_tu_kolbait(30,naimkont.ravno()),iceb_tu_kolbait(30,naimkont.ravno()),naimkont.ravno(),
  suma,
  iceb_tu_kolbait(29,row[11]),
  row[11]);

  for(int ii=29; ii < iceb_tu_strlen(row[11]); ii+=29)
    fprintf(ff,"%-5s|%-30s|%10s|%.*s\n",
    "","","",iceb_tu_kolbait(29,iceb_tu_adrsimv(ii,row[11])),row[11]);
    
  prnb(suma,strsql);
  kolst+=2;
  if(kolst >= kol_strok_na_liste)
   {
    kolst=2;
    sspp1(ffved,++listn,&kolst);

   }

  zarstved(&nomer_ved,row[5],naimkont.ravno(),suma,ffved);

  itogo+=suma;
 }
itog(ffved,itogo);

fprintf(ff,"\
-----------------------------------------------------------------\n");

fprintf(ff,"%42s:%10.2f\n",gettext("Итого"),itogo);

podpis(ff);


fclose(ff);
fclose(ffved);

masiv_din_double SUM(0);
masiv_din_int KOL(0);
SUM.plus(itogo,-1);
KOL.plus(1,-1);
sumprved(&SUM,&KOL,imafved);

class spis_oth oth;
oth.spis_imaf.plus(imafved);
oth.spis_naim.plus(gettext("Ведомость на получение денежных средств"));
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список контрагентов"));

for(int nomer=0; nomer < oth.spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nomer),1);

iceb_t_rabfil(&oth,"");
}
/********************************/
/*Получение суммы полученной от контрагента за день*/
/***********************************************/
double dirkasord_suma_den(short dd,short md,short gd,const char *kodkontr)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select suma from Kasord1 where datp='%04d-%02d-%02d' and kontr='%s' and tp=1",gd,md,dd,kodkontr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0.);
 }
double suma=0.;
while(cur.read_cursor(&row) != 0)
  suma+=atof(row[0]);

return(suma);
}
