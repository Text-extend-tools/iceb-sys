/*$Id: bb.c,v 5.42 2013/05/17 14:55:56 sasa Exp $*/
/*17.09.2009    02.11.1992      Белых А.И.      bb.c
Программа организующая удобную работу для
программиста работающего с языком "C"
*/
#include "../iceb.h"
#include        <unistd.h>
#include        <locale.h>
#include <ctype.h>
#include	"buhg.h"

void 		menci(void);
void		menmak();

//для построения с разделяемыми библиотеками
short		startgodb; /*Стартовый год главной книги*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
SQL_baza	bd;
//********************************

char            imqfm[80]; /*Имя проэкта*/
char            imqf[80]; /*Имя файла*/
int             mle; /*Метка входа в редактор*/
extern char     *editor;
extern int	iscolor;
char		imafmake[20];

int main(int argc,char **argv,char **envp)
{
class iceb_tu_str vstr("");
short		i;
int             ns;
char            bros[512],bros1[20];
char		imaf[40];
class iceb_tu_str strkey("");
class iceb_tu_str strkom("");

(void)setlocale(LC_ALL,"");

init_screen();  /*Запуск экранного режима*/

strcpy(imafmake,"Makefile.am");

//printw("\nMD=%s\n",mmm.MD[0]);
//OSTANOV();
if((editor = getenv("EDITOR")) == NULL)
 {
  if((editor=(char*)malloc(56)) == NULL)
   {
    beep();
    printw("1 Не могу выделить память !!!\n");
    exit(16);
   }
  beep();
  printw("Не установлена переменная editor !!!\n");
  printw("Введите имя редактора - ");
  my_getstr(editor,20,20,20,stdscr);
 }
memset(imqfm,'\0',sizeof(imqfm));
memset(imqf,'\0',sizeof(imqf));
if(argc > 1)
 strcpy(imqf,argv[1]);

VVOD  VVOD1(2);

VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD("Выбрать файл");//0
MENU.VVOD_SPISOK_add_MD("Работать с make файлами");//1
MENU.VVOD_SPISOK_add_MD("Редактировать");//2
MENU.VVOD_SPISOK_add_MD("Компилировать");//3
MENU.VVOD_SPISOK_add_MD("Строить задачу");//4
MENU.VVOD_SPISOK_add_MD("Выполнить задачу");//5
MENU.VVOD_SPISOK_add_MD("Выполнить команду");//6
MENU.VVOD_SPISOK_add_MD("Сменить редактор");//7
MENU.VVOD_SPISOK_add_MD("Ввести строку ключей");//8
MENU.VVOD_SPISOK_add_MD("Конец работы");//9
mle=ns=0;

/**************************
char ss[50];
strcpy(ss,"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯІЇЄҐ");
printw("\n%s\n",ss);
iceb_tu_tolower_str(ss);
printw("\n%s\n",ss);
OSTANOV();
*********************/
for(;;)
 {

  attroff(A_REVERSE);
  clear();
  move(0,COLS-25);
  printw("Версия:%s",VERSION);

  move(0,0);
  printw("Имя файла : %s Редактор: %s",imqf,editor);
  move(1,0);
  printw("Имя проэкта: %s",imqfm);
  move(2,0);
  printw("Строка ключей: %s",strkey.ravno());
  move(LINES-1,0);
//  printw("г.Винница Белых А.И. 1992г Тел. /0432/512973 e-mail:sasa@vc.ukrtel.net");
  printw("г.Винница Белых А.И. 1992г Тел. /0432/512973 e-mail:%s",ICEBEMAIL);
  refresh();
  znak(2,COLS-22);
  
  if(menu3(&MENU,&ns,1,3,0) != 0)
   continue;

  switch (ns)
   {
    case 9:
    case -1:
	 goto kon;
	   break;
    case 0:
	menci();
	break;

    case 1:
	menmak();
	break;

    case 2:
	memset(bros,'\0',sizeof(bros));
#ifndef __linux__
	if(mle == 0)
	 {
#endif
	  sprintf(bros,"%s %s",editor, imqf);
#ifndef __linux__
	 }
	else
	  sprintf(bros,"%s",editor);
#endif	  
	mle++;
	clear();
        refresh();
	if(runsystem(bros) != 0)
	  OSTANOV();

	break;

    case 3:
      helstr(LINES-1,0,"F10","выход",NULL);

      memset(bros1,'\0',sizeof(bros1));
      strcpy(bros1,imqf);

      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD("Введите имя файла для компиляции");
      vstr.new_plus(bros1);
      if((i=vvod1(&vstr,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       break;
      if(i == ESC)
       break;


	if(vstr.ravno()[0] == '\0')
         {
          POLE(imqf,imaf,1,'.');
	  sprintf(bros,"make %s.o",imaf);
         }
	else
         {
          POLE(imqf,bros1,1,'.');
	  sprintf(bros,"make %s.o",bros1);
         }
	if(imqf[0] == '\0')
	  continue;
	clear();
	printw("Компилируем !!!\n");
	refresh();
        runsystem1(bros,0);

      break;

    case 4:
	if(imqfm[0] != '\0')
	  sprintf(bros,"make %s",imqfm);
	else
	 {
	  if(imqf[0] == '\0' )
	   {
	    clear();
	    move(12,10);
	    printw("Не выбран *.mak файл !!!");
	    OSTANOV();
	    break;
	   }
	 }
	clear();
	printw("Построение !!!\n");
        refresh();
        runsystem1(bros,0);

       break;


    case 5:
      if(strkey.ravno()[0] == '\0')
        sprintf(bros,"./%s",imqfm);
      else
        sprintf(bros,"./%s %s",imqfm,strkey.ravno());
      clear();
      refresh();

      runsystem1(bros,0);

      break;

    case 6:
      helstr(LINES-1,0,"F10","выход",NULL);

      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD("Введите команду");

      if((i=vvod1(&strkom,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       break;
      if(i == ESC)
       break;

	clear();
	refresh();


      runsystem1(strkom.ravno(),1);

      break;

     case 7:
       helstr(LINES-1,0,"F10","выход",NULL);

       memset(bros,'\0',sizeof(bros));

       VVOD1.VVOD_delete();
       VVOD1.VVOD_SPISOK_add_MD("Введите имя редактора");
       vstr.new_plus("");
       if((i=vvod1(&vstr,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
         break;
       if(i == ESC)
        break;

       if(setenv("EDITOR",vstr.ravno(),1) != 0)
        {
         beep();
         printw("Ошибка переустановки переменной !\n");
         OSTANOV();
        }

       if((editor = getenv("EDITOR")) == NULL)
        {
         beep();
         printw("\nОшибка чтения переменной !\n");
         OSTANOV();
        }
       break;

     case 8:
       helstr(LINES-1,0,"F10","выход",NULL);

       VVOD1.VVOD_delete();
       VVOD1.VVOD_SPISOK_add_MD("Введите строку ключей");

       vvod1(&strkey,112,&VVOD1,NULL,stdscr,-1,-1);
       break;

     default:
       ns=0;
       break;
    }
 }
kon:;
clear();
refresh();
endwin();


}

/****************************/
/*Меню работы с make файлами*/
/****************************/
void		menmak()
{
int     ns;
char            bros[40];

VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD("Выбрать проэкт");
MENU.VVOD_SPISOK_add_MD("Редактировать make файл");
MENU.VVOD_SPISOK_add_MD("Выход");


  ns=0;
  while(menu3w(stdscr,&MENU,&ns,-1,-1,0) != 0);


  switch (ns)
   {
    case 3 :
    case -1 :
	 return;
	   break;
    case 0:
      if(dirmak1(imafmake,bros,-1,-1) == 0)
       {
        memset(imqfm,'\0',sizeof(imqfm));
        strcpy(imqfm,bros);
       }

      break;
    case 1:
	if(imqfm[0] == '\0')
	  break;
	sprintf(bros,"%s %s",editor,imafmake);
	clear();
	refresh();

	runsystem(bros);
	mle=0;
	break;
   }

}

/****************************/
/*Меню работы с ci файлами*/
/****************************/
void		menci()
{
int     ns;
char            bros[40];

VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD("Выбрать файл из списка проекта");
MENU.VVOD_SPISOK_add_MD("Выбрать файл из общего списка");
MENU.VVOD_SPISOK_add_MD("Откыть новый файл");
MENU.VVOD_SPISOK_add_MD("Выбрать include файл");
MENU.VVOD_SPISOK_add_MD("Выбрать файл листинга");
MENU.VVOD_SPISOK_add_MD("Выход");
  ns=0;
while(menu3w(stdscr,&MENU,&ns,-1,-1,0) != 0);


  switch (ns)
   {
    case -1 :
	 return;
	   break;
    case 0:
	if(imqfm[0] == '\0')
	 {

          if(dirmak1(imafmake,bros,-1,-1) == 0)
           {
 	    memset(imqfm,'\0',sizeof(imqfm));
	    strcpy(imqfm,bros);
           }
          else
           break;
	 }
        memset(bros,'\0',sizeof(bros));       
        if(dirmak(bros,imqfm) == 0)
         {
          strcpy(imqf,bros); 
	   mle=0;
         }

	break;
    case 1:
	strcpy(bros,"c");
	if(dirf(bros,2,2,30,0) == 0)
	 {
	  memset(imqf,'\0',sizeof(imqf));
	  strcpy(imqf,bros);
	  mle=0;
	 }
	break;


    case 2:
	move(20,0);
	printw("Введите имя нового файла - ");
	refresh();
        memset(bros,'\0',sizeof(bros));
        my_getstr(bros,40,40,40,stdscr);   
/*	getstr(bros);*/
	if(bros[0] == '\0')
	  break;
	sprintf(imqf,"%s",bros);
	sprintf(bros,"%s %s",editor,imqf);
	mle=0;
	clear();
	refresh();

	if(runsystem(bros) != 0)
	  OSTANOV();

	break;

    case 3:
	strcpy(bros,"h");
	if(dirf(bros,2,2,30,0) == 0)
	 {
	  memset(imqf,'\0',sizeof(imqf));
	  strcpy(imqf,bros);
	  mle=0;
	 }
	break;

    case 4:
	strcpy(bros,"lst");
	if(dirf(bros,2,2,30,0) == 0)
	 {
	  memset(imqf,'\0',sizeof(imqf));
	  strcpy(imqf,bros);
	  mle=0;
	 }
	break;



   }

}
