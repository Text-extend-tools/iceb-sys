/*18.06.2015	10.06.2004	Белых  xkus.c
Программа расчёта коммунальных услуг
*/

#define DVERSION "01.09.2017"

#include <iceb.h>
#include <buhg.h>

int kusnast(void);
void raskus(void);
int energy(char*,short,short);
void dirpmd();
void dirlg();
void  dirsk();
void karta(char*,char*);

SQL_baza	bd;

extern float kvartp; //квартирная плата
extern float otop;   //отопление
extern float gas;    //газ
extern float vbv;    //вода без ванны
extern float vsv;    //вода с ванной
extern float sbv;    //стоки без ванны
extern float ssv;    //стоки с ванной
extern float elek;   //електроенергия
extern float nds;    //НДС
extern float radio;  //радиоточка
extern float cena_vodi; //Цена воды по счётчику
extern float cena_stoka; //Цена стоков по счётчику
extern int iceb_kod_podsystem;

int main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_UU;



icebstart(argc,argv);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);


VVOD MENU(4);
int k=0;


/*Ищем настройку*/

if(nastsys() != 0)
  goto kon;
if(kusnast() != 0) 
  goto kon;



MENU.VVOD_SPISOK_add_ZAG("Програма розрахунку комунальних послуг");
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));

MENU.VVOD_SPISOK_add_MD("Перегляд списку квартир");//0
MENU.VVOD_SPISOK_add_MD("Розрахунок комунальних послуг");//1
MENU.VVOD_SPISOK_add_MD("Работа з файлом налаштовки");//2
MENU.VVOD_SPISOK_add_MD("Перегляд списку показань лічільників");//3
MENU.VVOD_SPISOK_add_MD("Перегляд списку пільг");//4
MENU.VVOD_SPISOK_add_MD("Выхід");//5

for (;;)
{

 clear();
 int nomer=0;
 move (nomer++,COLS-24);
 printw ("Разробник  Бєлих О.");
 move (nomer++,COLS-24);
 printw ("Версія:  1.2 ");
 move (nomer++,COLS-24);
 printw ("Дата: %s",DVERSION);
 nomer+=2;
 move (nomer++,COLS-24);
 printw ("Квартплата:%.3f", kvartp);
 move (nomer++,COLS-24);
 printw ("Отоплення:%.3f",otop);
 move (nomer++,COLS-24);
 printw ("Газ:%.3f",gas);
 move (nomer++,COLS-24);
 printw ("Вода без ванни:%.3f",vbv);
 move (nomer++,COLS-24);
 printw ("Вода з ванной:%.3f",vsv);
 move (nomer++,COLS-24);
 printw ("Стоки без ванни:%.3f",sbv);
 move (nomer++,COLS-24);
 printw ("Стоки з ванной:%.3f",ssv);
 move (nomer++,COLS-24);
 printw ("Електроенергия:%.3f",elek);
 move (nomer++,COLS-24);
 printw ("НДС:%.3f",nds);
 move (nomer++,COLS-24);
 printw ("Радіоточка:%.3f",radio);
 move (nomer++,COLS-24);
 printw ("Вода по лічільн.:%.3f",cena_vodi);
 move (nomer++,COLS-24);
 printw ("Стоки по лічільн.:%.3f",cena_stoka);


 if(menu3(&MENU,&k,1,0,0) != 0)
   continue;


 switch (k)
  {
   case -1:
   case 5:
   goto kon;
     break;

  case 0:
   dirsk();
   break;

  case 1:
   raskus();
   break;
  
  case 2:
   iceb_redfil("kusnast.alx",1);
   if(kusnast() != 0) 
     goto kon;
   break;
  
  case 3:
   dirpmd();
   break;

  case 4:
   dirlg();
   break;
  default:
   break; 
  }
}
kon:;

sql_closebaz(&bd);


endwin();


}
