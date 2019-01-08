/* $Id: scr.h,v 5.59 2013/08/13 05:50:26 sasa Exp $*/
/*04.06.2013	04.03.1999	Белых А.И.	scr.h
*/

#include <iceb.h>

#include 	<ncurses.h>
#include        <term.h>
#include 	<stdlib.h>
#include 	<string.h>
#include <libintl.h>
/*
Описание функций работы с экраном
*/
extern void		bigsuma(double,const char*);
extern int              mouseklic(int*,int,int,int,int,WINDOW*);
//extern void		GDITE(void);
extern void		clearstr(short,short);
extern void		gdite(void);
extern int		init_screen();

extern int 		mygetstr(char[], int, WINDOW *);
extern int		ostanov1(char[]);
extern void		ramka(short*,short*,short*,int);
//extern void		error2(short,short,short,const char*);
extern void		zagstr(int,int,long,long);
extern void		strzag(int,int,long,long);
extern void		znak(int,int);
extern int		OSTANOV(void);
extern int		OSTANOVW(WINDOW*);

/*
описаны коды клавиатуры
*/
#define        SUP     KEY_UP  /*Стрелка вверх */
#define        SDN     KEY_DOWN  /*Стрелка вниз  */
#define        SRITH   KEY_RIGHT  /*Стрелка вправо*/
#define        SLEFT   KEY_LEFT  /*Стрелка в лево*/
#define        ENTER   10  /*Возврат каретки*/
#define        ESC     27  /*ESC клавиша   */
#define        UMN     42  /*"*" клавиша   */
#define        OTN     45  /*"-" клавиша   */
#define        PLU     43  /*"+" клавиша   */
#define        DEL     47  /*"/" клавиша   */
#define        NOL     48  /*"0" клавиша   */

#define        FK1      KEY_F0+1
#define        FK2      KEY_F0+2
#define        FK3      KEY_F0+3
#define        FK4      KEY_F0+4
#define        FK5      KEY_F0+5  /*Коды функциональных клавиш*/
#define        FK6      KEY_F0+6
#define        FK7      KEY_F0+7
#define        FK8      KEY_F0+8
#define        FK9      KEY_F0+9
#define        FK10     KEY_F0+10
#define        FK11     KEY_F0+21
#define        FK12     KEY_F0+22

#ifndef	MAXFKEYS
  #define MAXFKEYS 12
#endif
#define        SFK1      KEY_F0+MAXFKEYS+1
#define        SFK2      KEY_F0+MAXFKEYS+2
#define        SFK3      KEY_F0+MAXFKEYS+3
#define        SFK4      KEY_F0+MAXFKEYS+4
#define        SFK5      KEY_F0+MAXFKEYS+5
#define        SFK6      KEY_F0+MAXFKEYS+6
#define        SFK7      KEY_F0+MAXFKEYS+7
#define        SFK8      KEY_F0+MAXFKEYS+8
#define        SFK9      KEY_F0+MAXFKEYS+9
#define        SFK10     KEY_F0+MAXFKEYS+10

#define        PD       KEY_NPAGE  /* Страница вниз*/
#define        PU       KEY_PPAGE  /* Страница вверх*/
#define        INS      KEY_IC  /* Клавишa Insert*/
#define        HOME     KEY_HOME  /* Клавишa Home  */
#define        END      KEY_END  /* Клавишa End   */
#define        DELETE   KEY_DC  /* Клавишa Delete*/

#ifndef KEY_BACKSPACE
#define        BACK     8    // Клавишa BackSpase
#else
#define        BACK     KEY_BACKSPACE   //Клавишa BackSpase
#endif

#define        TAB      9    /* Клавишa Tab*/

