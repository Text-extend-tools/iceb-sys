/* $Id: kassa.c,v 5.41 2013/11/24 08:23:03 sasa Exp $ */
/*10.10.2011	30.04.1999	Белых А.И. 	kassa.c
Программа работы с кассовым аппаратом 
*/
#define         DVERSIQ "05.06.2008"
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <pwd.h>
#include        <time.h>
#include        <ctype.h>
#include	<errno.h>
//#include	<libconfig.h>
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#else
struct cassa_pars_struct {
  int type;
  char *port;
  int num;
  char *passwd;
};
typedef struct cassa_pars_struct cassa_pars;
char bros[1024];
#define BUFSIZE 1024
#define MAX_CASS 0
struct cassa_tovar_struct {
     uint       cod;
     uint       gr_NDS;         /* группа НДС 0-4 ERA, 0-8 - Datecs */
     uint       gr_TOV;         /* товарная группа 1-32 ERA, 1-99 Datecs*/
     uint       depart;         /* отдел 0-31 ERA, 0-9 Datecs   */
     uint       drob;           /* 0 - если кол-во только целое,
     иначе м.б. дробное (ERA)        */
     uint       test_ost;       /* 0 - не проверять остатки при продаже,
     иначе - проверять       */
     uint       free_c;         /* может ли цена устанавливаться оператором ? */
     double     cena;           /* цена единицы                 */
     double     ost;            /* запас товара (остаток)       */
     double     oborot;         /* оборот товара                */
//     uchar      name[16];       /* наименование 12 симв. для ERA
//     14 для Datecs                */
     char       barcode[14];    /* штрих-код, 13 символов, ERA  */
     };
typedef struct cassa_tovar_struct cassa_tovar;
typedef struct cassa_pars_struct cassa_pars;
#endif
#include        <locale.h>
#include	"buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef CONFIG_INI
#define CONFIG_INI "iceb.ini"
#endif

void		loadtov(int,int);
void		readkas(int);
int		procentt(int);
void		naskap(int,char[],int,int);
extern int	printcas(cassa_tovar*);
void		sumtab2(int,char[],int,int);
void		sumtab3(int,char[],int,int);
void		sumtab56(int,char[],int,int);
int		vibkas(char[],int*,int*,int*);

int		ktorab=0;
SQL_baza	bd;
pid_t		mypid;
//extern char	*organ;
extern char 	*imabaz;
int		kassa=0;
char		mylogin[L_cuserid];
short dt,mt,gt;

cassa_pars *cp[MAX_CASS];

#define default_passwd "00000"

void read_cassa_list()
{
  
#ifdef HAVE_TORGDEV
  config_t 	foo;
//  strncpy(bros, CONFIG_PATH, 60);
  p = getenv("HOME");
  sprintf(bros, "%s/", p);
  strncat(bros, CONFIG_INI, 29);
  config_new(&foo);
  if(config_read(&foo, bros, CS_INI, NULL)) 
   {
    move(10,0);
    perror(bros);
    printw("\nНе найден конфигурационный файл !\n");
    OSTANOV();
   } 
  else 
   {
//    fprintf(stderr, "Reading config file ...\n");
    for (i=0; i<MAX_CASS; i++) {
      sprintf(bros, "CASSA_%1d", i+1);
      if ((j=read_vars(&foo, bros, 
  	     "model", CHARPTR_T, &p,
             NULL)) == 0) {
         
         if ((strncmp(p, "SAMSUNG", 7) == 0) || 
             (strncmp(p, "DATECS", 6) == 0)) {
            free(p);
            cp[i] = (cassa_pars *)malloc(sizeof(cassa_pars));
            cp[i]->type = T_DATECS;
            if (read_vars(&foo, bros,
  	     "device", CHARPTR_T, &p,
  	     "number", INT_T, &n,
  	      NULL) == 0) {
               cp[i]->port = p;
               cp[i]->num = n;
  	    }
  	    else {
               cp[i]->port = NULL;
               cp[i]->num = 0;
            }
         }
         else if (strncmp(p, "ERA", 3) == 0) {
                 free(p);
                 cp[i] =(cassa_pars *) malloc(sizeof(cassa_pars));
                 cp[i]->type = T_ERA;
                 cp[i]->num = 0;
                 if (read_vars(&foo, bros,
       	     		"device", CHARPTR_T, &p,
       	     		"password", CHARPTR_T, &p1,
       	     		NULL) == 0) {
                    cp[i]->port = p;
                    cp[i]->passwd = p1;
       	         }
       	         else cp[i]->port = NULL;
       	 }
      }
      else {
         cp[i] = NULL;
         break;
      }
    }
    config_destroy(&foo);
   }
#endif
}

int             main(int argc,char **argv,char **envp)
{
struct  passwd  *ktor; /*Кто работает*/
SQL_str   	row;
int		kom;
short		i;
char		bros[512];
char		imqkas[512];
char		kto[23];
char		*host=NULL;
char		*parol=NULL;
//char		*port_name;
//const char *ps;
int		nomer;
int		termlin,nomernl,tipkas;
int		Y,X;
SQLCURSOR curr;
ktor=getpwuid(getuid());
ktorab=ktor->pw_uid;
mypid=getpid();
(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа


init_screen();  /*Запуск экранного режима*/

VVOD MENU(3);

umask(0117); /*Установка маски для записи и чтения группы*/

strcpy(kto,ktor->pw_name);

/*Читаем ключи запуска программы*/
readkey_par(argc,argv,&parol,&imabaz,&host);


/* прочитать инициализационный файл (параметры устройств) */
read_cassa_list();

/*************
printw("Casses list:\n");
for (i=0; i<MAX_CASS; i++) {
 if (cp[i] != NULL)
   printw("%d) Type: %d; Line: %s\n", i, cp[i]->type, cp[i]->port);
 else break;
}
refresh();
*****************/


//port_name = NULL;
//ps = default_passwd;


if(sql_openbaz(&bd,imabaz,host,kto,parol) != 0)
  eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

icebstart_rf();


/*Читаем нормативно-справочную информацию*/
if(nastsys() != 0)
 goto kon;
if(matnast() != 0)
 goto kon;


kom=0;
termlin=nomernl=0;
nomer=-1;
memset(imqkas,'\0',sizeof(imqkas));

MENU.VVOD_SPISOK_add_MD("Вибрати касовий апарат.");
MENU.VVOD_SPISOK_add_MD("Одержати звіт продажу з каси.");
MENU.VVOD_SPISOK_add_MD("Завантажити перелік товарів в кассу.");
MENU.VVOD_SPISOK_add_MD("Налаштовка касового апарата.");
MENU.VVOD_SPISOK_add_MD("Кінець роботи.");

for(;;)
 {

  clear();


  move(0,0);
  printw("Програма роботи з касовим апаратом.");
  move(1,0);
  printw("%.40s",iceb_t_get_pnk("00",0));

  znak(0,54);
  poltekdat(&dt,&mt,&gt);
  move(Y=13,X=54);
  printw("Поточна дата %d.%d.%dр.",dt,mt,gt);

  move(++Y,X);
  printw("Логин - %s",ktor->pw_name);

  move(++Y,X);
  printw("Ім'я:%-20.20s",ktor->pw_gecos);

  move(++Y,X);
  printw("Номер-%d Група-%d",ktor->pw_uid,ktor->pw_gid);

  move(++Y,X);
  printw("Номер каси:%d",nomer);
  move(++Y,X);
  printw("Номер лінії:%d",termlin);
  move(++Y,X);
  printw("Номер на лінії:%d",nomernl);
  move(++Y,X);
  printw("Каса:%s",imqkas);

  move(LINES-1,20);
  printw("%s %s %s %s%s",gettext("Версия"),VERSION,
  gettext("от"),DVERSIQ,
  gettext("г."));


  if(menu3(&MENU,&kom,1,2,0) != 0)
    continue;

  switch (kom)
   {
    case 4 :
    case -1 :
	   goto kon;

    case 0 :
      nomer=vibkas(imqkas,&termlin,&nomernl,&tipkas);
      break;

    case 1 :
     if(nomer < 0) 
       nomer=vibkas(imqkas,&termlin,&nomernl,&tipkas);
     readkas(nomer);

     break;

    case 2 :
     if(nomer < 0) 
       nomer=vibkas(imqkas,&termlin,&nomernl,&tipkas);
     loadtov(nomer,tipkas);
     break;

    case 3 :
     if(nomer < 0) 
       nomer=vibkas(imqkas,&termlin,&nomernl,&tipkas);
     naskap(nomer,imqkas,termlin,nomernl);
     break;

    default:
      kom=0;
      break;
   }
 }

kon:;



sql_closebaz(&bd);
move(LINES-2,0);
printw("%s\n",gettext("База закрыта. Программа закончила работу."));
refresh();
endwin();
return(0);
}

/************************************/
/*Загрузка товаров в кассовый аппарат*/
/************************************/
void		loadtov(int nomer,int tipkas)
{
#ifdef HAVE_TORGDEV
long		kolstr,kolstr1,ks;
SQL_str         row;
char		strsql[512];
int		i;
uint		kodgr;
uint		kodm;
double		cena;
uint		nds; /*группа НДС 0-4*/
uint		otdel; /*Отдел 0-32*/
uint		drob; /*0-если количество только целое иначе дробное*/
uint		ost_test;  /*0- не проверять остатки при продаже иначе проверять*/
int		kom;
cassa_tovar     *tlist[MAX_TOVAR];
int		maxkolt;
time_t		t1,t2;

if(nomer < 0)
 {
  iceb_t_soob("Не вибран кассовый аппарат !");
  return;
 }

maxkolt=100;
if(cp[nomer]->type == T_DATECS)
 maxkolt=MAX_TOVAR_DATECS;
if(cp[nomer]->type == T_ERA)
 maxkolt=MAX_TOVAR_ERA;

kom=0;

naz:;

clear();
attron(A_BOLD);
attron(A_BLINK);
attron(COLOR_PAIR(COLOR_RED*COLORS+COLOR_BLACK));
mvprintw(0,0,"\
                          У в а г а !!!\n\n");
attroff(A_BLINK);
attroff(COLOR_PAIR(COLOR_RED*COLORS+COLOR_BLACK));
                      
attron(COLOR_PAIR(COLOR_GREEN*COLORS+COLOR_BLACK));
printw("\
       Перед виконанням цієї операції необхідно скинути накопичення\n\
            проданого товару в пам'яті касового апарату.\n\
    Для отримання більш докладної інформації з цього питання виберіть\n\
                 в меню \"Просмотр документації\".");
attroff(COLOR_PAIR(COLOR_BLUE*COLORS+COLOR_BLACK));
attroff(A_BOLD);

VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD("Почати завантаження.");
MENU.VVOD_SPISOK_add_MD("Просмотр документації.");
MENU.VVOD_SPISOK_add_MD("Вихід.");

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2 :
  case -1 :
   return;

  case 0 :
   break;

  case 1 :
   GDITE();
    if(tipkas == T_DATECS)
     sprintf(strsql,"%s","datecs_reset.txt");

   if(tipkas == T_ERA)
     sprintf(strsql,"%s","era_reset.txt");

   iceb_t_pdoc(strsql);
   goto naz;
  default:
    return;  
 }

time(&t1);

clear();
GDITE();

sprintf(strsql,"select kodm,kodgr,naimat,cenapr,strihkod from Material \
order by kodm asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }



if(kolstr == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не знайдено жодного товару !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;  
 }
ost_test=0;
nds=1;  
otdel=1;
drob=1;
kolstr1=ks=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%4s %2s %-30s %5s %s\n",row[0],row[1],row[2],row[3],row[4]);
  strzag(LINES-1,0,kolstr,kolstr1);
  kolstr1++;
  kodm=atoi(row[0]);
  kodgr=atoi(row[1]);
  cena=atof(row[3]);
  if(cena == 0.)
   {
    beep();
    printw("%4s %2s %-30s %5s %s\n",row[0],row[1],row[2],row[3],row[4]);
    printw("Товар має нульову ціну ! Не буде завантажен в касу !\n");
//    OSTANOV();
    continue;
   }   

  tlist[ks]=(cassa_tovar*)malloc(sizeof(cassa_tovar));
  tlist[ks]->cod=kodm;
  tlist[ks]->gr_NDS=nds;
  tlist[ks]->gr_TOV=kodgr;
  tlist[ks]->depart=otdel;
  tlist[ks]->drob=drob;
  tlist[ks]->test_ost=ost_test;
  tlist[ks]->free_c=0;
  tlist[ks]->cena=cena;
  tlist[ks]->ost=0.;
  strncpy((char*)tlist[ks]->name,row[2],14);
  strcpy(tlist[ks]->barcode,row[4]);
  
  ks++;
  tlist[ks]=NULL;
  if(ks >= maxkolt)
    break;

 }
GDITE();

if((i=Cassa_prog_tovar(cp[nomer],tlist,procentt,printcas)) != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Помилка завантаження в кассу !");
  sprintf(strsql,"%s %d",CA_error(i),i);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  for(i=0; tlist[i] != NULL;i++)
   free(tlist[i]);
  return;
 }
for(i=0; tlist[i] != NULL;i++)
 free(tlist[i]);

time(&t2);
printw("\nЧас завантаження %ldсек.\n",t2-t1);

VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD("Завантаження завершене !");
soobshw(&SOOB,stdscr,-1,-1,0,1);
#endif
}

/*************************/
/*Чтение отчёта из кассы*/
/************************/
void		readkas(int nomer)
{
#ifdef		HAVE_TORGDEV
char		imaf[20];
FILE		*ff;
struct  tm      *bf;
time_t          tmm;
int		np=1,nk;
double		its;
int		i;
time_t		t1,t2;
cassa_tovar     *tlist[MAX_TOVAR];
char		strsql[512];

if(nomer < 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не вибраний касовий апарат !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

nk=100;
if(cp[nomer]->type == T_DATECS)
 nk=MAX_TOVAR_DATECS;
if(cp[nomer]->type == T_ERA)
 nk=MAX_TOVAR_ERA;

time(&tmm);
bf=localtime(&tmm);

clear();

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD("      Формування касового звіту.");
sprintf(strsql,"Просмотраються товари з номера %d по %d.",np,nk);
SOOBN.VVOD_SPISOK_add_MD(strsql);
soobsh(&SOOBN,LINES-6,0,1);

move(20,0);
GDITE();
time(&t1);

if((i=Cassa_otch_tovar(cp[nomer],np,nk,tlist,procentt)) != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Помилка отримання звіту !");
  sprintf(strsql,"%s %d",CA_error(i),i);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  for(i=0; tlist[i] != NULL;i++)
   free(tlist[i]);
  return;

 }
GDITE();



clear();

printw("Печатьуемо звіт.\n");
refresh();


sprintf(imaf,"kasa%d.lst",mypid);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(ff);

fprintf(ff,"Звіт продажу товарів з касового апарата.\n");
fprintf(ff,"Станом на %d.%d.%d  %d:%d\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min);

/*
printw("\
-----------------------------------------------------------------\n\
N/в|Код | Найменування | Ціна   |Кількіс.|  Сумма  |Штріх код    |\n\
-----------------------------------------------------------------\n");
refresh();
*/

fprintf(ff,"Просмотраються товари з номера %d по %d.\n",np,nk);
fprintf(ff,"\
-----------------------------------------------------------------\n\
N/в|Код | Найменування | Ціна   |Кількіс.|  Сумма  |Штріх код    |\n\
-----------------------------------------------------------------\n");
its=0.;

for(i=0; tlist[i] != NULL; i++)
 {
/*
  printw("%3d %4d %-14s %8.2f %8.2f %8.2f %s\n",
  tlist[i]->depart,tlist[i]->cod,tlist[i]->name,tlist[i]->cena,
  tlist[i]->ost,tlist[i]->oborot,tlist[i]->barcode);
  GDITE();
*/
  fprintf(ff,"%3d %4d %-14s %8.2f %8.2f %8.2f %s\n",
  tlist[i]->depart,tlist[i]->cod,tlist[i]->name,tlist[i]->cena,
  tlist[i]->ost,tlist[i]->oborot,tlist[i]->barcode);


  its+=tlist[i]->oborot;

 }

//printw("%41s %8.2f\n","Всього",its);
fprintf(ff,"%41s %8.2f\n","Всього",its);
fclose(ff);

for(i=0; tlist[i] != NULL;i++)
 free(tlist[i]);

time(&t2);
printw("Час формування звіту %ldсек.\n",t2-t1);


printw("\nРозпечать вивантажено в файл %s\n",imaf);
OSTANOV();

//rabfil("Звіт з кассового апарата.",imaf);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Звіт з кассового апарата"));
iceb_t_rabfil(&oth,"");
#endif
}

/******************************/
/*Настройка кассового аппарата*/
/******************************/

void	naskap(int nomer,char imqkas[],int termlin,int nomernl)
{
#ifdef		HAVE_TORGDEV
int		kom;

if(nomer < 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не вибраний касовий апарат !");
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG("Налаштовка касового апарата.");

MENU.VVOD_SPISOK_add_MD("Тип обслуговування (Таблиця 2).");
MENU.VVOD_SPISOK_add_MD("Список операторів (Таблиця 3).");
MENU.VVOD_SPISOK_add_MD("Зовнішний вигляд чека (Таблиця 5,6).");
MENU.VVOD_SPISOK_add_MD("Кінець роботи.");

kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  switch (kom)
   {
    case 3 :
    case -1 :
      return;
    case 0:
      sumtab2(nomer,imqkas,termlin,nomernl);
      break;
    case 1:
      sumtab3(nomer,imqkas,termlin,nomernl);
      break;
    case 2:
      sumtab56(nomer,imqkas,termlin,nomernl);
      break;
    
    default:
      kom=0;
      break;
   }
 }
#endif
}
/******************************/
int		procentt(int pr)
{
int		kolstr,kolstr1;

kolstr=COLS-1;
kolstr1=(kolstr*pr)/100;
strzag(LINES-1,0,kolstr,kolstr1);

return(0);
}
/********************************/

int		printcas(cassa_tovar *tlist)
{
#ifdef HAVE_TORGDEV
printw("%3d %4d %-14s %8.2f %8.2f %8.2f %s\n",
tlist->depart,tlist->cod,tlist->name,tlist->cena,
tlist->ost,tlist->oborot,tlist->barcode);
#endif
return(0);
}
