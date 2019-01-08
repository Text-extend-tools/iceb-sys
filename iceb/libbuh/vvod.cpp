/*$Id: vvod.c,v 5.17 2013/05/17 14:56:32 sasa Exp $*/
/*31.12.2017    02.11.1992      Белых А.И.      vvod.c
Подпрограмма общего ввода данных
Все данные вводятся в символьном виде
Необходимые преобразования производятся в вызывающей подпрограмме
Если вернули -1 не записывать
	     -2 записывать
             
*/


#include        "buhl.h"

extern int metka_utf8;

int VVOD::vvod(int N, //Номер строки на которой подсветка
int j, //Если 0 - вытирать себя, 1 - не вытирать
int mt, //0-без тени 1-с тенью
int PY,int PX) //Координаты меню
{
short           i,i1;
char            ST[1024]; /*Строка меню*/
int             K;  /*Принимает код клавиши*/

vramka(PY,PX,mt);

int vihod=0;
while(vihod == 0)
 {
  attron(CFS); 
  vprintw(N,stdscr);

  K=getch();

  attroff(CFS);
  attron(CFM);

  vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,PY,PX,MDLS,KLST,stdscr);
  if(K == ENTER)
   {
    attroff(CFM);
    attron(CFV);

    vprintw_clear(N,stdscr);


    vgetstr(N,stdscr);
    
    attroff( CFV );
    attron( CFM );

    vprintw(N,stdscr);

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
  if(N > KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=KLST-1;
   }

  if(K == ESC)
   {
/*
#ifndef M_UNIX
      getch();
#endif
*/
    attroff( CFS );
    attron( CFM );
//    move( PY+N+1, PX+1+dlr);
//    kolbait=iceb_tu_kolbait(dlinastr,data.ravno(N));
//    printw("%-*.*s",kolbait,kolbait,data.ravno(N));
    vprintw(N,stdscr);
    N=K;
    break;
   }

  switch(K)
   {
    case KEY_RESIZE:
//      getch();
    case FK1:
    case FK2:
    case FK3:
    case FK4:
    case FK5:
    case FK6:
    case FK7:
    case FK8:
    case FK9:
    case FK10:
    case SFK1:
    case SFK2:
    case SFK3:
    case SFK4:
    case SFK5:
    case SFK6:
    case SFK7:
    case SFK8:
    case SFK9:
    case SFK10:
    case PLU:
    case OTN:

      attroff( CFS );
      attron( CFM );
//      move( PY+N+1, PX+1+dlr);
//      kolbait=iceb_tu_kolbait(dlinastr,data.ravno(N));
//      printw("%-*.*s",kolbait,kolbait,data.ravno(N));
      vprintw(N,stdscr);
      N=K;
      vihod=1;
      break;    
   }

 }

attroff( CFS );
attron( CFM );

if(j == 0)
 {
  memset(ST,'\0',sizeof(ST));
  for(i1= PX;i1< PX+MDLS+2;i1++)
      strcat(ST," ");

  move( PY, PX);
  i1=0;
  for(i= PY;i<= PY+KLST+1;i++)
   {
    move(i, PX);
    printw("%s",ST);
    if(i1 > 0)
     if(mt == 1)
      printw("  ");
    i1++;
   }
  if(mt == 1)
   {
    move(i, PX+1);
    printw("%s",ST);
   }
 }

//attron( CB );
standend(); //Установка нормального режима отображения
refresh();


return(N);

}

/*************/
/*Конструктор*/
/*************/

VVOD::VVOD(int nomr)
{
memset(LNU,'\0',sizeof(LNU));
memset(PNU,'\0',sizeof(PNU));
memset(LVU,'\0',sizeof(LVU));
memset(PVU,'\0',sizeof(PVU));
memset(VE,'\0',sizeof(VE));
memset(PO,'\0',sizeof(PO));

/*****************
Если кодировка utf8 то в линуксе всё работает првильно а в putty неправильно отображается рамка 
для того чтобы этого небыло подставляем псевдографику, которая всегда есть в этой кодировке
и которая правильно отображается везде
************************/

if(metka_utf8 == 0)  
 {
  LNU[0]=ACS_LLCORNER;
  PNU[0]=ACS_LRCORNER;
  LVU[0]=ACS_ULCORNER;
  PVU[0]=ACS_URCORNER;
  PO[0]=ACS_HLINE;
  VE[0]=ACS_VLINE;
  CFR = A_ALTCHARSET; /*Альтернативная таблица*/
 }
else 
 {
  strcpy(LNU,"└");
  strcpy(PNU,"┘");
  strcpy(LVU,"┌");
  strcpy(PVU,"┐");
  strcpy(PO,"─");
  strcpy(VE,"│");
  CFR = A_NORMAL; /*Не включаем алтернативную таблицу*/
  
 }
TENN=' ';

CFS = A_REVERSE; /*Инверсия*/
CFM = A_NORMAL;
CB =  A_NORMAL;
CSZ= A_NORMAL;
CFV = A_NORMAL;

MDLS=0; /*Максимальная длинна строки в символах*/
KLST=0; /*Количество заданных строк*/
dlr=0;  /*Длинна реплик в символах*/

if(has_colors() != 0)  //Цветной
 {
  switch(nomr)
   {
    case 0: //Меню ввода данных
     
     CSZ |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CFV |= A_BOLD | COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_GREEN);
     CFR |= COLOR_PAIR( COLOR_RED*COLORS+COLOR_BLUE);
     CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     break;

    case 1: //Меню danet
     CSZ |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_RED);
     CFR |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_RED);
     CFS |= A_BOLD | COLOR_PAIR( COLOR_BLUE*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_RED);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     break;

    case 2: //Настройка для vvod1 меню
     CFR |= COLOR_PAIR( COLOR_BLUE*COLORS+COLOR_MAGENTA);
     CFS |= A_BOLD | COLOR_PAIR(COLOR_RED*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_MAGENTA);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CFV |= A_BOLD |COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_GREEN);
     break;

    case 3: //Настройка для menu3
     CSZ |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CFR |= COLOR_PAIR(COLOR_RED*COLORS+COLOR_BLUE);
     CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CFV |= A_BOLD | COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_GREEN);
     break;

    case 4: //Настройка для menu3 с заголовком другого цвета
     CSZ |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CFR |= COLOR_PAIR(COLOR_RED*COLORS+COLOR_BLUE);
     CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CFV |= A_BOLD | COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_GREEN);
     break;

    default:
     CSZ |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CFR |= COLOR_PAIR(COLOR_RED*COLORS+COLOR_BLUE);
     CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*COLORS+COLOR_WHITE);
     CFM |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLUE);
     CB |= COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_BLACK);
     CFV |= A_BOLD | COLOR_PAIR( COLOR_WHITE*COLORS+COLOR_GREEN);
     break;

   }
 }
}


/************/
/*Рамка меню на стандартном окне*/
/************/
void VVOD::vramka(int PYZ,int PXZ, //координаты вывод рамки
int TEN) //0-без тени 1-с тенью
{
short           i,i1;
short           PR;
char            ST[1024];
short		dlss;
int		kolelem=0;
int		kolelemdata=0;
int		kolelemzag=0;
int		shethik=0;

PY=PYZ;
PX=PXZ;
MDLS=0;
KLST=0;
dlr=0;

memset(ST,'\0',sizeof(ST));
sprintf(ST,"%s",MD.ravno(0));
dlr=iceb_tu_strlen(ST);

kolelemzag=ZAG.kolih();
for(i=0; i< kolelemzag ; i++)
  if((PR=iceb_tu_strlen(ZAG.ravno(i))) >(MDLS))
     MDLS=PR;


kolelemdata=data.kolih();
kolelem=MD.kolih();
dlss=0;
for(i=0; i< kolelem ; i++)
 {
  if(kolelemdata != 0)
   dlss=dls.ravno(i)-1;
  else
   dlss=0;   
  if((PR=iceb_tu_strlen(MD.ravno(i))+dlss) >(MDLS))
     MDLS=PR;
  KLST+=1;
 }

/*Вычисляем позицию по горизонтали*/
if( PX < 0)
 {
  PX=(COLS-MDLS-4)/2;
  if(PX < 0)
   PX=0;
 }
//Вычислить позицию во вертикали
if( PY < 0)
 {
  PY=(LINES-KLST-4-kolelemzag-1)/2;
  if(PY < 0)
   PY=0;
 }

if(MDLS > COLS-2)
  MDLS=COLS-2;
if( TEN == 1 && MDLS > COLS-4)
  MDLS=COLS-4;

/*******************
mvprintw(LINES-2,0,"kolelemzag=%d MDLS=%d PY=%d PX=%d COLS=%d TEN=%d\n",
kolelemzag,MDLS,PY,PX,COLS,TEN);
OSTANOV();
******************/

/********************/
/*   Рисуем рамку   */
/********************/
attron( CFR);

move(PY,PX);

printw("%s",LVU);      /*Левый уголок*/
for(i= PX; i< PX+(MDLS);i++)
  printw("%s",PO);      /*Горизонтальная полка*/
printw("%s",PVU);       /*Правый уголок*/
int iii=0;
if(kolelemzag != 0)
  iii=kolelemzag+1;
for(i= PY;i< PY+(KLST)+iii;i++)
 {
  shethik++;
  attron( CFR );
  move(i+1, PX);
  printw("%s", VE);   /*Вертикальная стенка*/
  if(kolelemzag != 0 && shethik == kolelemzag+1)
   {
    move(i+1,PX+1);
    for(i1=PX;i1<PX+MDLS;i1++)
      printw("%s",PO);      /*Горизонтальная полка*/
   }
  else
   {
    attroff( CFR );
    attron( CFM );
    for(i1= PX;i1< PX+(MDLS);i1++)
      printw(" ");

    attroff( CFM );
    attron( CFR );
   }
  move(i+1,PX+(MDLS)+1);
  printw("%s", VE);         /*Вертикальная стенка*/
  attron(CB);
  if( TEN == 1)
   {
    attron(A_REVERSE);
    printw("%c%c",TENN,TENN);
    attroff(A_REVERSE);
   }
 }
attron( CFR );
i1=i+2;


move(i+1, PX);
printw("%s",LNU);   /*Левый нижний уголок*/
for(i= PX;i< PX+(MDLS);i++)
  printw("%s",PO);    /*Горизонтальная полка*/
printw("%s",PNU);     /*Правый нижний уголок*/

attroff( CFR );

attron( CB );
if( TEN == 1)
 {
  attron(A_REVERSE);
   printw("%c%c",TENN,TENN);
  attroff(A_REVERSE);
 }

move(i1,PX+2);
if( TEN == 1)
 {
  attron(A_REVERSE);
  for(i= PX;i< PX+(MDLS)+2;i++)
  printw("%c",TENN);
  attroff(A_REVERSE);
 }
attroff( CB );


attron( CSZ );

for(i=0;i< kolelemzag;i++)
 {
  memset(ST,0,sizeof(ST));
  sprintf(ST,"%-*s",iceb_tu_kolbait(MDLS,ZAG.ravno(i)),ZAG.ravno(i));
  move( PY+i+1, PX+1);
  printw("%.*s",iceb_tu_kolbait(MDLS,ST),ST);
 }
attroff( CSZ );

attron( CFM );/*Нормальный цвет включен*/

if(kolelemzag != 0)
  PY+=kolelemzag+1;

const char *dann;
for(i=0;i< (KLST);i++)
 {
  memset(ST,0,sizeof(ST));

  sprintf(ST,"%s",MD.ravno(i));
  if((dann=data.ravno(i)) != NULL)
    strcat(ST,dann);

  move( PY+i+1, PX+1);
  printw("%.*s",iceb_tu_kolbait(MDLS,ST),ST);

 }

attroff(CFM);
attron(CB);

}

/************/
/*Рамка меню на не стандартном окне*/
/************/
void VVOD::wvramka(int PYZ,int PXZ, //координаты вывод рамки
int *PYstd,int *PXstd, //координаты вывода первой строки меню выбора на предыдущем окне
int TEN, //0-без тени 1-с тенью
int plusstr, //0-обычно 1-для "данет
int kolih_simv,/*максимальное количество символов- применяется для vvod1 */
WINDOW *win1,   //Окно на котором будет расположено новое окно
WINDOW **win2)   //Новое окно
{
short           i,i1;
short           PR;
char            ST[1024];
short		dlss;
int		kolelem=0;
int		kolelemdata=0;
int		kolelemzag=0;
int		shethik=0;
short		ten=0,ten1=0;

PY=PYZ;
PX=PXZ;

/*Вытолкнем содержимое предыдущего экрана*/
wrefresh(win1);
if(TEN == 1)
 {
  ten=2;
  ten1=1;
 }

MDLS=0;
KLST=0;
dlr=kolih_simv;
if(plusstr == 1)
  MDLS=dlr;  //Длинна строки для ввода в vvod1
  
if(MD.kolih() > 0)
 {
  sprintf(ST,"%s",MD.ravno(0));
  dlr=iceb_tu_strlen(ST);
 }
kolelemzag=ZAG.kolih();
for(i=0; i< kolelemzag ; i++)
  if((PR=iceb_tu_strlen(ZAG.ravno(i))) >(MDLS))
     MDLS=PR;

int iii=0;
if(kolelemzag != 0)
  iii=kolelemzag+1;

kolelemdata=data.kolih();
kolelem=MD.kolih();

dlss=0;
for(i=0; i< kolelem ; i++)
 {
  if(kolelemdata > 0)
   dlss=dls.ravno(i)-1;
  else
   dlss=0;   

  if((PR=iceb_tu_strlen(MD.ravno(i))+dlss) >(MDLS))
     MDLS=PR;
  KLST+=1;
 }
KLST+=plusstr;


if(MDLS > COLS-2-ten)
  MDLS=COLS-2-ten;

/*Вычисляем позицию по горизонтали*/
if( PX < 0)
 {
  PX=(COLS-(MDLS)-2-ten)/2;
  if(PX < 0)
   PX=0;
 }
//Вычислить позицию во вертикали
if( PY < 0)
 {
  PY=(LINES-(KLST)-2-ten1-kolelemzag-1)/2;
  if(PY < 0)
   PY=0;
 }

*PYstd=PY;
if(kolelemzag != 0)
  *PYstd=PY+(kolelemzag+1);

*PXstd=PX;

if((*win2=newwin((int)(KLST+2+ten1+iii),(int)(MDLS+ten+2),PY,PX)) == NULL)
 {
  beep();
  wprintw(win1,"\n%s-Ошибка создания окна ! %d/%d\n",__FUNCTION__,KLST+2+ten1,MDLS+ten+2);
  OSTANOVW(win1);
  endwin();
  exit(1);
 }

keypad(*win2,TRUE); /*Разрешить распознавать функциональные клавиши*/

/**************
mvprintw(LINES-2,0,"kolelemzag=%d kolelem=%d MDLS=%d PY=%d PX=%d\n",
kolelemzag,kolelem,MDLS,PY,PX);
OSTANOV();
*************/

overwrite(win1,*win2);
PY=PX=0;

/********************/
/*   Рисуем рамку   */
/********************/
wattron(*win2,CFR);

wmove(*win2,PY,PX);

wprintw(*win2,"%s",LVU);      /*Левый уголок*/
for(i= PX; i< PX+(MDLS);i++)
  wprintw(*win2,"%s",PO);      /*Горизонтальная полка*/
wprintw(*win2,"%s",PVU);       /*Правый уголок*/

for(i= PY;i< PY+(KLST)+iii;i++)
 {
  shethik++;
  wattron(*win2, CFR );
  wmove(*win2,i+1, PX);
  wprintw(*win2,"%s", VE);   /*Вертикальная стенка*/
  if(kolelemzag != 0 && shethik == kolelemzag+1)
   {
    wmove(*win2,i+1,PX+1);
    for(i1=PX;i1<PX+MDLS;i1++)
      wprintw(*win2,"%s",PO);      /*Горизонтальная полка*/
   }
  else
   {
    wattroff(*win2, CFR );
    wattron(*win2, CFM );
    for(i1= PX;i1< PX+(MDLS);i1++)
      wprintw(*win2," ");

    wattroff(*win2, CFM );
    wattron(*win2, CFR );
   }
  wmove(*win2,i+1, PX+(MDLS)+1);
  wprintw(*win2,"%s", VE);         /*Вертикальная стенка*/
  wattron(*win2, CB );
  if( TEN == 1)
   {
    wattron(*win2,A_REVERSE);
    wprintw(*win2,"%c%c",TENN,TENN);
    wattroff(*win2,A_REVERSE);
   }
 }
wattron(*win2, CFR );

i1=i+2;

wmove(*win2,i+1, PX);
wprintw(*win2,"%s",LNU);   /*Левый нижний уголок*/
for(i= PX;i< PX+(MDLS);i++)
  wprintw(*win2,"%s",PO);    /*Горизонтальная полка*/
wprintw(*win2,"%s",PNU);     /*Правый нижний уголок*/

wattroff(*win2, CFR );

wattron(*win2, CB );
if( TEN == 1)
 {
  wattron(*win2,A_REVERSE);
   wprintw(*win2,"%c%c",TENN,TENN);
  wattroff(*win2,A_REVERSE);
 }

wmove(*win2,i1,PX+2);
if( TEN == 1)
 {
  wattron(*win2,A_REVERSE);
  for(i= PX;i< PX+(MDLS)+2;i++)
    wprintw(*win2,"%c",TENN);
  wattroff(*win2,A_REVERSE);
 }
wattroff(*win2, CB );

wattron(*win2, CSZ );

for(i=0;i< kolelemzag;i++)
 {
  memset(ST,0,sizeof(ST));
  sprintf(ST,"%-*s",iceb_tu_kolbait(MDLS,ZAG.ravno(i)),ZAG.ravno(i));
  wmove(*win2, PY+i+1, PX+1);
  wprintw(*win2,"%.*s",iceb_tu_kolbait(MDLS,ST),ST);
 }

wattroff(*win2, CSZ );

wattron(*win2, CFM );/*Нормальный цвет включен*/

if(kolelemzag != 0)
  PY+=kolelemzag+1;

const char *dann;
for(i=0;i< (KLST)-plusstr;i++)
 {
  memset(ST,0,sizeof(ST));
  sprintf(ST,"%s",MD.ravno(i));
  if((dann=data.ravno(i)) != NULL)
    strcat(ST,dann);
  wmove(*win2, PY+i+1, PX+1);
  wprintw(*win2,"%.*s",iceb_tu_kolbait(MDLS,ST),ST);

 }

wattroff(*win2, CFM );
wattron(*win2, CB );

wrefresh(*win2);

}

/********************************/
/*Вернуть значение цветовой пары*/
/********************************/
long VVOD::VVOD_return_cs(int nomer)
{

switch(nomer)
 {
  case 0:
    return(CSZ);
  case 1:
    return(CFV);
  case 2:
    return(CB);
  case 3:
    return(CFM);
  case 4:
    return(CFS);
  case 5:
    return(CFR);
    

 }

printw("\nVVOD_return_cs-Не известный номер цветовой пары !\n");
OSTANOV();
return(0);

}

/*****************************/
/*Очистить список данных меню*/
/*****************************/
void VVOD::VVOD_clear_data(void)
{
int kol=data.kolih();
for(int i=0; i < kol ;i++)
  data.copy(i,"");
//  data.ravno(i)[0]='\0';
}

/*****************************/
/*Запись в список данных double*/
/*****************************/

void VVOD::data_plus(int nomer,double stroka)
 {
  data_plus(nomer,stroka,10);
 }   
/*****************************/
/*Запись в список данных double с указанной разрядностью после запятой*/
/*****************************/

void VVOD::data_plus(int nomer,double stroka,int pz)
 {
  if(stroka == 0.)
   {
    data.plus(nomer,"");
    return;
   }
  char bros[50];
  sprintf(bros,"%.*g",pz,stroka);
//  strncpy(data.ravno(nomer),bros,dls.ravno(nomer)-1);
  data.copy(nomer,bros);
 }   

/*****************************/
/*Запись в список данных int*/
/*****************************/
void VVOD::data_plus(int nomer,int stroka)
 {
  if(stroka == 0)
   {
    data.plus(nomer,"");
    return;
   }
  char bros[56];
  sprintf(bros,"%d",stroka);
//  strncpy(data.ravno(nomer),bros,dls.ravno(nomer)-1);
  data.copy(nomer,bros);
 }   
/*****************************/
/*Запись в список данных int*/
/*****************************/
void VVOD::data_plus(int nomer,long stroka)
 {
  if(stroka == 0)
   {
    data.plus(nomer,"");
    return;
   }
  char bros[56];
  sprintf(bros,"%ld",stroka);
  data.plus(nomer,bros);
 }   
/**************************/
/*Вернут данные в double*/
/*************************/
double VVOD::data_ravno_atof(int nomer)
 {
  return(ATOFM(data.ravno(nomer)));
 }
/*****************************************/
/*Записать в список данных через запятую*/
/*****************************************/
void VVOD::data_z_plus(int nomer,const char *stroka)
{
if(iceb_tu_strlen(data.ravno(nomer)) == 0)
 {
  data.plus(nomer,stroka);
 }
else
 {
  if((int)iceb_tu_strlen(data.ravno(nomer))+(int)iceb_tu_strlen(stroka)+1 > dls.ravno(nomer)-1)
   return;    
//  strcat(data.ravno(nomer),",");
//  strcat(data.ravno(nomer),stroka);
  data.cat(nomer,",");
  data.cat(nomer,stroka);
 }
}
/*****************************************/
/*Записать целое в список данных через запятую*/
/*****************************************/
void VVOD::data_z_plus(int nomer,int stroka)
{
char bros[50];
sprintf(bros,"%d",stroka);
data_z_plus(nomer,bros);
}
/*****************************************/
/*Записать длинное целое в список данных через запятую*/
/*****************************************/
void VVOD::data_z_plus(int nomer,long stroka)
{
char bros[50];
sprintf(bros,"%ld",stroka);
data_z_plus(nomer,bros);
}
/*************************************/
/*Добавить в строку */
/****************************/
void VVOD::data_strcat(int nomer,const char *stroka)
{
//int kolsimv=dls.ravno(nomer)-iceb_tu_strlen(data.ravno(nomer));
//strncat(data.ravno(nomer),stroka,kolsimv-1);
data.cat(nomer,stroka);
}
/**********************************/
/*Вернуть год если записана дата в формате дд.мм.гггг*/
/*****************************************************/
short VVOD::data_ravno_god(int nomer)
{
char bros[512];
memset(bros,'\0',sizeof(bros));
int nomer_s=0;
int metka=0;
for(int ii=0; ii < (int)strlen(data.ravno(nomer)) ; ii++)
 {
  if(data.ravno(nomer)[ii] == '.')
   metka++;
  if(metka == 2)
   if(data.ravno(nomer)[ii] != '.')
    bros[nomer_s++]=data.ravno(nomer)[ii];
 }
return(atoi(bros));
}
/******************************/
/*Записать*/
/***********************************/
void VVOD::VVOD_SPISOK_zapis_data(int nomer,const char *stroka)
{
if(nomer >= data.kolih())
 {
  printw("\n%s-Нет в списке строки %d!!\n",__FUNCTION__,nomer);
  OSTANOV();
  return;

 }
data.copy(nomer,stroka); 
}
/*************************/
/*ввод с клавиатуры*/
/*************************/
int VVOD::vgetstr(int nom,WINDOW *okno)
{

int dlinastr=dvh_str(nom);
//char stroka[dls.ravno(nom)*sizeof(wchar_t)];
int kolbait=data_kolbait(nom);
char stroka[kolbait];

memset(stroka,'\0',sizeof(stroka));
strncpy(stroka,data.ravno(nom),data_kolbait(nom)-1);

int voz=my_getstr(stroka,dlinastr,dls.ravno(nom),kolbait,okno);
data.plus(nom,stroka);

return(voz);
//return(my_getstr(data.ravno(nom),dlinastr,dls.ravno(nom),okno));
   
}  
/*******************************/
/*Вывод на стандартный экран строки меню*/
/***************************************/
void VVOD::vprintw(int nomer_str,WINDOW *win)
{
int dlinastr=dvh_str(nomer_str);

int kolbait=iceb_tu_kolbait(dlinastr,data.ravno(nomer_str));

wmove(win,PY+nomer_str+1,PX+1+dlr);
wprintw(win,"%-*.*s",kolbait,kolbait,data.ravno(nomer_str));
wmove(win,PY+nomer_str+1,PX+1+dlr);
wrefresh(win);
}
/****************************/
/*Заполнить строку пробелами*/
/****************************/
void VVOD::vprintw_clear(int nomer_str,WINDOW *win)
{
int dlinastr=dvh_str(nomer_str);

wmove(win,PY+nomer_str+1,PX+1+dlr);
//wprintw(win,"%*s",dlinastr," ");
wprintw(win,"%*s",iceb_tu_kolbait(dlinastr," ")," ");
wmove(win,PY+nomer_str+1,PX+1+dlr);
wrefresh(win);

}
/****************************/
/*Длинна видимой части строки в символах для ввода данных*/
/****************************/
int VVOD::dvh_str(int nomer_str)
{
int dlinastr=dls.ravno(nomer_str)-1;
if(dlinastr > MDLS-dlr)
  dlinastr=MDLS-dlr;
return(dlinastr);
}

/****************************/
/*Записать дату*/
/****************************/
void VVOD::data_plus_dat(int nomer,short d,short m,short g)
{
char bros[32];
sprintf(bros,"%d.%d.%d",d,m,g);
data_plus(nomer,bros);
}
/*************************/
/*Вернуть количество байт выделенное для строки ввода*/
/*****************************/
int VVOD::data_kolbait(int nomer)
{
return(data.kolbait(nomer));
}
