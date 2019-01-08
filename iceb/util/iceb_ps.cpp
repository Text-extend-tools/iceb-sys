/* $Id: iceb_ps.c,v 5.52 2014/07/31 07:09:36 sasa Exp $*/
/*07.02.2017	15.09.2005	Белых А.И.	iceb_ps.c

Программа преобразования файлов сформированных для печати 
на матричных принтерах в PostScript файлы

Единица измерения изображения в пунктах 
Пункт = 1/72 дюйма
Дюйм равен 25.4 миллиметра
1 пунк равен 25.4/72=0.352777 миллиметра
В России 1 пункт равен 0.376 миллиметров
Формат бумаги А4 210*297 миллиметров или 596*842 пункта
Начало координат в левом нижнем углу листа

Формула получения размера в пунктах:
 Х*72/25.4
 где Х - расстояние в милиметрах

Формула переведения пункты в милиметры
 X*25.4/72
 где Х - расстояние в пунктах

Ширина шрифта в пунктах для режима 10 знаков на дюйм:
 (25.4/10)*72/25.4 или 72/10   десять знаков  

Применяемые размеры EPSON шрифтов
10 знаков на дюйм
17 знаков на дюйм (это 10 знаков в режиме сжатия)
12 знаков на дюйм 
20 знаков на дюйм (это 12 знаков в режиме сжатия)
15 знаков на дюйм 
22 знаков на дюйм (это 15 знаков в режиме сжатия)

Формула вычисления длинны строки в пунктах:

КЗ/КЗД*72

где:
КЗ  - количество знаков
КЗД - количество знаков на дюйм



Стандартное межстрочное расстояние на матричных принтерах 1/6 дюйма

Перечень команд матричных принтеров применяемых для создания отчетов
в системе iceB.
----------------------------------------------------------------------
ASCII |Десятичное|Шес-чное|           Описание
----------------------------------------------------------------------
ESC     27         1B       Признак команды для принтера
LF      10         0A       Перевод строки
FF      12         0C       Перевод формата
DC2     18         12       Отмена уплотненого режима
DC4     20         14       Отмена двойной ширины (Для одной строки)
SO      14         0E       Выбор двойной ширины (для 1 строки)
SI      15         0F       Выбор уплотнённого режима печати
ESC SI  15         0F       Выбор уплотнённого режима печати
ESC - n 45         2D       Включение/выключение подчеркивания
                            Если n = 0 режим выключен \x30
                                     1 режим включен  \x31
ESC 2   50         32       Выбор 1/6-дюймового интервала между строками
ESC 3 n 51         33       Выбор n/216-дюймового интервала между строками
ESC A n 65         41       Выбор n/72-дюймового интервала между строками
ESC E   69         45       Выбор режима выделенного шрифта
ESC F   70         46       Отмена режима выделенного шрифта
ESC G   71         47       Выбор двухударного режима
ESC H   72         48       Отмена двухударного режима
ESC M   77         4D       Двенадцать знаков на дюйм
ESC P   80         50       Десять знаков на дюйм
ESC [ n                     Изменение высоты символа, где n размер в пунктах - нестандартная команда !!!!
ESC g  103         67       Пятнадцать знаков на дюйм (отрабатывается не всеми принтерами)
ESC U n 85         55       Включение/выключение однонаправленного режима
                            Если n = 0 режим выключен \x30
                                     1 режим включен  \x31
ESC l   108        6C       Установка левого поля
ESC x n 120        78       Выбор режимов высококачественной или черновой 
                            печати. Если n = 0 - Выбор чернового режима
                                             1 - Выбор высококачественной
                                                 печати
----------------------------------------------------------------------------
Примеры использования команд в тексте программы
fprintf(ff,"\x12");  //отмена ужатого режима
fprintf(ff,"\x1b\x6C%c",10-koolk); //Установка левого поля
fprintf(ff,"\x1B\x4D"); //12-знаков
fprintf(ff,"\x0F");  //Ужатый режим
fprintf(ff,"\x1B\x33%c\n",30); //Уменьшаем межстрочный интервал

 
 **************Размеры которые приняты в расчёт в милиметрах******************
         ось Y
       ^
       |           210.0
       |<------------------------->|     12.8
       |                           |   ---------
       |                           |   |
       ------------------------------------------------------
       |   ---------------------   | ---------              ^
       |   |                   |   |   ^     ^              |
       |   |                   |   |   |     |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |        271.7          297
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |                   |   |         |              |
       |   |      197.2        |   |         |              |
       |   |<----------------->|   |    |    |              |
       |   |                   |   |    |    |              |
      0|   ---------------------   | ---------              |
------------------------------------ ----------------------------------------> ось Х
       |       6.44                     ^
     ->|---|<------------               |
       |                                |  12.5
       |                                ----------

Размер 197.2 и 271.7 взяты с тестовой распечатки страницы на принтере 
Была измеряна рамочка выведенная в тестовой распечатке.
Отступ с лева до доступной для печати зоны согласно текста в тестовой распечатке должен быть 6 мм 
но при таком отступе при выводе рамочки по краям зоны печати левая линия рамки не видна. Пришлось увеличивать пока
не увидел линию рамки.

Отступ сверху до доступной для печати зоны согласно текста в тестовой распечатке должен быт 13 мм
но реально на моём принтере он гораздо меньше. 
Остальные размеры вычисляются с учётом того что размер бумаги 210 Х 297 мм

Реальные размеры зоны печати на моём принтере больше чем 197.2 Х 271.7
но использую их так как практика показывает что на разных принтерах или
драйверах для принтера, реальные размеры зоны печати разные. Взятая в 
расчёт зона печати и отступы должны подходить для всех принтеров.


*****************/
#define VERSION	"5.2 07.02.2017"
#include <glib.h>
#include        <sys/stat.h>
#include	<stdio.h>
#include        <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <string.h>
#include	<ctype.h>
#include	<unistd.h>
#include <util.h>
#include	"iceb_ps.h"  
#include	"iceb_ps_shrift.h"  

#ifndef HDRDIR
#define HDRDIR "/etc/hp"
#endif

#ifndef HDRFILE
#define HDRFILE "iceb_ps_fonts.ps" //Фонты для принтеров в которые их надо грузить
#endif

void ramka(FILE*); //Контрольная рамка для отладки
void print_logo(FILE *ffpri);
void	vstr(int*,char*,FILE *ffpri);
void iceb_ps_main(int *nomlist,FILE *ffpri,FILE *ffist);
void iceb_ps_rekur(int *nomlist,FILE *ffpri);
void iceb_ps_zapnastr(class iceb_ps_nastr *S_N,class iceb_ps_nastr *N_N,FILE *ffpri);
void  iceb_ps_nastr_hrift(char *stroka_nastr,FILE *ffpri,FILE *ffist);
FILE *iceb_ps_cod(FILE *ffist);
void iceb_ps_lands_on(FILE *ffpri);

/*
 *	Глобальные константы
 */ 
/*	Типы начертаний		*/
#define S_normal "koi8-u"
#define S_bold   "koi8-u-bold"

/* Флаги атрибутов шрифта  		*/
// жирный
#define A_bold		1
// курсив
#define A_italic	2
// подчеркивание
#define A_underline	4

/* Флаги типов интервала	*/
#define I_1_6	1
#define I_1_8	2
#define I_n_72	3
#define I_n_216	4

/* Флаги плотности знаков/дюйм	*/
#define D_10	1
#define D_12	2
// двойная ширина
#define D_double	0x10
// сжатый
#define D_compress	0x20

/* Плотность (символов/дюйм) в разных режимах печати:	*/
// Pike
#define 	col_sim_10  10.0
// Compressed Pike
#define		col_sim_10u 17.0
// Elite
#define		col_sim_12  12.0
// Compressed Elite
#define		col_sim_12u 20.0

#define		col_sim_15  15.0
#define		col_sim_15u 22.0

/* Массштаб шрифта по ширине в разных режимах печати */
#define         HS10   (120.0/col_sim_10)
#define         HS10u  (120.0/col_sim_10u)
#define         HS12   (120.0/col_sim_12)
#define         HS12u  (120.0/col_sim_12u)
#define         HS15   (120.0/col_sim_15)
#define         HS15u  (120.0/col_sim_15u)  //Неизвестно сколько поэтому сделал наобум

/* Массштаб шрифта по высоте*/
#define         VS     11.333  /*В пунктах*/


/* Максимальная длина строки	*/
//#define MAXS 102400
#define MAXS 19992

/*
 *	Глобальные переменные
 */ 

const double RAZMERLISTAY_PORT=297.; //Размер листа по высоте в портретной ориентации
const double RAZMERLISTAX_PORT=210.; //Размер листа по горизонтали в портретной ориентации
const double otstup_sverhy=12.8; //размер отступа сверху до начала зоны печати
const double otstup_sprava=6.5; //размер отступа справа до начала зоны печати
const double visota_shrifta=2.4; //Высота шрифта в милиметрах
const double zona_x=197.2; //доступная зона для печати по горизонтали в милиметрах
const double zona_y=271.7;  //доступная зона для печати по вертикали в милиметрах

double otstup_y=otstup_sverhy+visota_shrifta;//Отступ от верха листа до начала первой строки для портретной ориентации в милиметрах
double lev_zona=otstup_sprava;//Размер зоны (в милиметрах) от левого края листа недоступный для печати при ориетации Портрет 
double nig_zona=RAZMERLISTAY_PORT-otstup_sverhy-zona_y; //Размер нижней зоны листа недоступной для печати в милиметрах
double RAZMERLISTAY = RAZMERLISTAY_PORT;	//Размер листа по вертикали
double RAZMERLISTAX = RAZMERLISTAX_PORT;	//Размер листа по горизонтали
double zona_y_tek=zona_y; //текущая зона по вертикали

//Позиция в пунктах начала вывода первого символа начальная
double  poz_per_stroki_y_tek=(RAZMERLISTAY*72./25.4)-(otstup_y*72./25.4); //Установлена для портретной ориентации- меняется для ландшафтной
double  poz_per_stroki_x=lev_zona*72./25.4; //Установлена для портретной ориентации- меняется для ландшафтной ориентации
double  poz_per_stroki_x_tek=poz_per_stroki_x; //Установлена для портретной ориентации меняется когда встречаются команды на установку отступа


double max_dlinna_stroki=zona_x*72./25.4; //Максимальная длинна строки в пуктах, которая может поместится на А4 
                                         //с учётом левой и правой зон на которых текст не печатается
                                         //Размер 197.2 взят из тестовой распечатки для принтера

#define Portrait  '\0'
#define Landscape '\1'
char	orientation = Portrait;
char	*logo = NULL;
int     A4_obrez=0;  //Если равно 1 - отключен перенос того что не поместилось на следующий лист
int	ris_ramku = 0;

short metka_shrift=0; //0-10 знаков на дюйм 1- 12 знаков на дюйм 2- 15 знаков
short metka_shrift_u=0; //0-нормальный шрифт 1-уплотнённый


const double ss10=72./col_sim_10;  //7.2
const double ss10u=72./col_sim_10u; //4.235294117
const double ss12=72./col_sim_12;  //6.0
const double ss12u=72./col_sim_12u; //3.6
const double ss15=72./col_sim_15;  //4.8
const double ss15u=72./col_sim_15u; //3.27272727

//Глобальные переменные которые используются в алгоритме для всех габаритных расчётов
class iceb_ps_nastr
 {
  public:
  double tek_ss_p; //Текущая ширина шрифта в пунктах (10 знаков на дюйм нормальный режим печати)
  double tek_sag_str; //Текущий шаг между строк в милиметрах
  double tek_m_shrifta;  //Текущий масштаб шрифта по ширине
  char   naim_shrift[64];   //Текущее наименование шрифта
  int    tek_otstup;  //Количество символов отступа
  double  tek_otstup_punkt; //Текущий отступ в пунктах
  int metka_underline; /*0-выключен режим подчёркивания 1-включён*/
  double visota; /*Высота символа в пуктах*/
  iceb_ps_nastr()
   {
    tek_ss_p=72./10.;
    tek_sag_str=25.4/6.;
    tek_m_shrifta=HS10;
    strcpy(naim_shrift,S_normal );
    tek_otstup=0;   
    tek_otstup_punkt=0.;
    metka_underline=0;
    visota=VS;
   }
  void copy(class iceb_ps_nastr *obr)
   {
    tek_ss_p=obr->tek_ss_p;
    tek_sag_str=obr->tek_sag_str;
    tek_m_shrifta=obr->tek_m_shrifta;
    strcpy(naim_shrift,obr->naim_shrift);
    tek_otstup=obr->tek_otstup;    
    tek_otstup_punkt=obr->tek_otstup_punkt;
    visota=obr->visota;
   }
 };
short metka_1_stroki=0; //Метка чтения первой строки 
class iceb_ps_nastr TN; //Глабальные текущие настройки

//**************************

#define STROKA_NASTR "#nast end strok#" //Настройки в конце обрезанной строки
short metka_h_list=0; //0-всё поместилось по ширине на лист 1-не поместилось

char  imaf_tmp_r[64];   //имя файла который подаётся на вход фильтра в который выгружено то что не поместрилось по ширине листа
char  imaf_tmp_w[64];   //имя файла в который выводится то что не поместилось по ширине
char  imaf_tmp_perecod[64]; /*Имя файла в который будет выгружен перекодированное содержимое входных данных*/
char  imaf_tmp_potok[64]; /*Сюда будет выгружен входной поток, сделано так потому что потоку нельзя сделать rewind*/
/*=================================================================================*
 *=================================================================================*
 *
 *	Main program
 */
int             main(int argc,char *argv[],char *envp[])
{
FILE	*ffpri=NULL; 	// output file
FILE	*ffist=NULL; 
char	*putnansi;
int	i;


char    Usage[] = "Usage:\n\t%s [-v] [-l] [-b] [-c] [-L logo.file] [filename]\n\
\t%s job user title copies options [filename]\n";
int	errflg = 0;

#ifdef DEBUG
fprintf(stderr,"WARNING: iceb_ps warning\n");
#endif
/* Просмотр ключей    */
while ((i = getopt(argc, argv, "vlbcL:hH?")) != EOF) 
 {
  switch (i) 
   {
    case 'v': /* печать версии */
              printf("iceb_ps v.%s\n", VERSION);
              exit(0);
    case 'l': /* ориентация - Landscape */
              orientation = Landscape;
              break;
    case 'b': /* показывать рамочку вокруг поля печати */
              ris_ramku = 1;
              break;
    case 'c': //Не выполнять перенос того что не помещается по ширине на А4 на следующий лист
              A4_obrez = 1;
              break;
    case 'L': /* логотип */
              logo = optarg;
              break;
    default:  /* Help  */
              errflg++;
              break;
  }
}
//getchar();
if (errflg) 
 {                 /* Если была хотя бы одна ошибка */
   fprintf(stderr, Usage, argv[0], argv[0]);    /* Выведем подсказку             */
   return(1);                   /* и выйдем с ошибкой            */
 }

argc = argc - optind + 1;
if(argc == 6 || argc == 1)
 ffist=stdin;
else
 {

  if(argc != 2 && argc != 7)
   {
    fprintf(stderr, Usage, argv[0], argv[0]);    /* Выведем подсказку             */
    return(1);
   }   

  putnansi = argc == 2 ? argv[optind] : argv[optind+5] ;

  if((ffist = fopen(putnansi,"r")) == NULL)
   {
    char bros[1024];  //Строка прочитанная из файла
    sprintf(bros, "ERROR: %s", putnansi);
    perror(bros);
    return(1);
   }
 }



sprintf(imaf_tmp_w,"%s%siceb_ps_w%d.tmp",g_get_tmp_dir(),G_DIR_SEPARATOR_S,getpid());
sprintf(imaf_tmp_r,"%s%siceb_ps_r%d.tmp",g_get_tmp_dir(),G_DIR_SEPARATOR_S,getpid());
sprintf(imaf_tmp_perecod,"%s%siceb_ps_perecod%d.tmp",g_get_tmp_dir(),G_DIR_SEPARATOR_S,getpid());
sprintf(imaf_tmp_potok,"%s%siceb_ps_potok%d.tmp",g_get_tmp_dir(),G_DIR_SEPARATOR_S,getpid());

ffist=iceb_ps_cod(ffist); /*Перекодировка в koi8u если источник в кодировке utf8*/

ffpri=stdout;

fprintf(ffpri,"%s",zagolovok); //Записываем заголовок файла находится в iceb_ps.h

if (orientation == Landscape) //ориентация может быть задана ключом -l
 {
  iceb_ps_lands_on(ffpri);
 }



/*Определяем путь на файлы настройки*/
char imaf_nastr[strlen(HDRDIR)+strlen(HDRFILE)+2];
sprintf(imaf_nastr,"%s/%s",HDRDIR,HDRFILE);
//fprintf(stderr,"%s-%s\n",__FUNCTION__,imaf_nastr);

FILE *ffsap;
if((ffsap = fopen(imaf_nastr,"r")) != NULL)
 {

  //переписываем фонты
  while((i=fgetc(ffsap)) != EOF)
    fputc(i, ffpri);  
  fclose(ffsap);
 }
else
 {
  fprintf(ffpri,"%s",zagolovok_shrift); //Записываем заголовок файла находится в iceb_ps_shritf.h
 }

fprintf(ffpri,"%%Устанавливаем шрифт по умолчанию\n\
/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
TN.naim_shrift, TN.tek_m_shrifta, TN.visota);


int	nomlist=0;
iceb_ps_main(&nomlist,ffpri,ffist);


if ((logo != NULL) && (nomlist == 1)) 
  print_logo(ffpri);



fprintf(ffpri,"showpage\n");
fprintf(ffpri,"%%%%Trailer\n");
fprintf(ffpri,"%%%%Pages: %d\n",nomlist);
fprintf(ffpri,"%%%%EOF\n");

fclose(ffist);
fclose(ffpri);

unlink(imaf_tmp_w);
unlink(imaf_tmp_r);
unlink(imaf_tmp_perecod);
unlink(imaf_tmp_potok);

#ifdef DEBUG
closelog();
#endif
return(0);
}

/**************************************/
/*Печать логотипа на вершине страницы*/
/**************************************/
void print_logo(FILE *ffpri) 
{
 FILE *flogo;
 char	stroka_logo[2048]; 

 if ((flogo = fopen(logo, "r")) == NULL) 
  {
    sprintf(stroka_logo, "ERROR: %s", logo);
    perror(stroka_logo);
    logo = NULL;
  }
 else 
  {
    /* начало функции */
    fprintf(ffpri, "\nsave\n/showpage {} def\n%%%%BeginDocument\n");
    while (fgets(stroka_logo,sizeof(stroka_logo), flogo) != NULL) 
     {
       /* переписываем все описание логотипа, кроме %%EOF */
       if (strncmp("%%EOF", stroka_logo, 5) != 0)
          fputs(stroka_logo, ffpri);
     }
    /* конец функции */
    fprintf(ffpri, "%%%%EndDocument\nrestore\n");
    fclose(flogo);
  }
}

/**********************************/
/*Начало новой страницы*/
/***********************************/
void	newpage(int *nomlist,FILE *ffpri,FILE **ff_tmp_w)
{

if(*ff_tmp_w != NULL)
 {
  fclose(*ff_tmp_w);
  *ff_tmp_w=NULL;
 } 



iceb_ps_rekur(nomlist,ffpri);

if(*nomlist > 0)
 {
  if ((logo != NULL) && (*nomlist == 1)) 
    print_logo(ffpri);
  fprintf(ffpri,"showpage\n");
 }
*nomlist+=1;

fprintf(ffpri,"%%%%Page: %d %d\n",*nomlist,*nomlist);

/*разворот по часовой стрелке на 90 градусов и смещение оси координат вниз на горизонталь листа
разворот только такой так как в расчётах участвует левый отступ до зоны печати*/

if (orientation == Landscape) //задаётся для каждого листа
 fprintf(ffpri,"\
gsave                 %%сохраним графический контекст\n\
%.2f milim 0 translate %%смещаем ось координат вправо на 210 милиметров по оси Х\n\
90 rotate             %%поворачиваем ось координат на 90 градусов по часовой стрелки\n\
save                  %%сохраним полностью образ памяти виртуальной машины\n",RAZMERLISTAX_PORT);
/*************
 этот заворот не подходит
 
 fprintf(ffpri,"\
gsave                 %%сохраним графический контекст\n\
0 %.2f milim translate %%смещаем ось координат вверх на 297 милиметров по оси Y\n\
-90 rotate            %%поворачиваем ось координан против часовой стрелке на 90 градусов\n\
save                  %%сохраним полностью образ памяти виртуальной машины\n",RAZMERLISTAY_PORT);
*******************/
if (ris_ramku != 0)
  ramka(ffpri); //Контрольная рамка для отладки

//Устанавливаем текущюю точку в начало вывода первой стороки на листе

fprintf(ffpri,"\
/tekpx %f def\n\
/tekpy %f def\n\
tekpx tekpy moveto\n",
poz_per_stroki_x_tek,
poz_per_stroki_y_tek);

/*Записываем текущие настройки действовавшие на конец предыдущего листа*/
if(*nomlist > 1)
 {
  fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);

  fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",TN.naim_shrift, TN.tek_m_shrifta,TN.visota);
   fprintf(ffpri,"/mstrr %f milim def \n",TN.tek_sag_str);

  if(strcmp(TN.naim_shrift,S_bold) == 0)
     fprintf(ffpri,"/tektl tln2 def\n");
  if(strcmp(TN.naim_shrift,S_normal) == 0)
     fprintf(ffpri,"/tektl tln def\n");

  fprintf(ffpri, "%d otstup\n",TN.tek_otstup);
  if(TN.metka_underline == 1) 
    fprintf(ffpri,"/metkaunderline 1 def\n");

 }

if((*ff_tmp_w = fopen(imaf_tmp_w,"w")) == NULL)
 {
  char bros[312];
  sprintf(bros, "ERROR: %s", imaf_tmp_w);
  perror(bros);
 }

}

/******************************/
/*Вывод строки символов       */
/******************************/
void vstr(int *kolsv,char *strokav,FILE *ffpri)
{

if(*kolsv == 0)
 return;

fprintf(ffpri,"(%s) SV\n",strokav);
*kolsv=0;
memset(strokav,'\0',MAXS);

}

/*************************************/
/*Вывод контрольной рамки для отладки*/
/*************************************/
void	ramka(FILE *ffpri)
{
fprintf(ffpri,"newpath\n");            // начало графического пути
if (orientation == Landscape) 
 {

  fprintf(ffpri,"/Y %.2f milim def\n",nig_zona);   //Начало по Х
  fprintf(ffpri,"/X %.2f milim def\n",lev_zona);   //Начало по Y

  fprintf(ffpri,"X Y moveto\n");   // становимся в точку начала вывода символа
/****************
  fprintf(ffpri,"271.7 milim 0 rlineto\n");     // проводим линию 
  fprintf(ffpri,"0 197.2 milim rlineto\n");     // проводим линию 
  fprintf(ffpri,"-271.7 milim 0 rlineto\n");     // проводим линию 
  fprintf(ffpri,"0 -197.2 milim rlineto\n");     // проводим линию 
****************/
  fprintf(ffpri,"%.2f milim 0 rlineto\n",zona_y);     // проводим линию 
  fprintf(ffpri,"0 %.2f milim rlineto\n",zona_x);     // проводим линию 
  fprintf(ffpri,"%.2f milim 0 rlineto\n",zona_y*-1);     // проводим линию 
  fprintf(ffpri,"0 %.2f milim rlineto\n",zona_x*-1);     // проводим линию 

 }

else
 {
  fprintf(ffpri,"/X %.2f milim def\n",lev_zona);   //Начало по Х
  fprintf(ffpri,"/Y %.2f milim def\n",nig_zona);   //Начало по Y

  fprintf(ffpri,"X Y moveto\n");   // становимся в точку начала вывода символа
/*************
  fprintf(ffpri,"0 271.7 milim rlineto\n");     // проводим линию 
  fprintf(ffpri,"197.2 milim 0 rlineto\n");     // проводим линию 
  fprintf(ffpri,"0 -271.7 milim rlineto\n");     // проводим линию 
  fprintf(ffpri,"-197.2 milim 0 rlineto\n");     // проводим линию 
***************/
  fprintf(ffpri,"0 %.2f milim rlineto\n",zona_y);     // проводим линию 
  fprintf(ffpri,"%.2f milim 0 rlineto\n",zona_x);     // проводим линию 
  fprintf(ffpri,"0 %.2f milim rlineto\n",zona_y*-1);     // проводим линию 
  fprintf(ffpri,"%.2f milim 0 rlineto\n",zona_x*-1);     // проводим линию 
 }
fprintf(ffpri,"closepath\n");          // замыкание графического пути

fprintf(ffpri,"1 setlinewidth\n");     // установка толщины линии
fprintf(ffpri,"stroke\n");             // построение линии вдоль пути
}
/*****************************************/
/*основной алгоритм*/
/**************************/

void iceb_ps_main(int *nomlist,FILE *ffpri,FILE *ffist)
{

int	kolsv=0;	//Количество символов в строке вывода
int metkaff=0; //Метка позволяющая исключить ситуацию двойного объявления нового листа
               //Когда программа сделала новый лист потому что закончился предыдущий 
               //и в начале следующей строки есть команда создать новый лист которая уже не нужна
int     metkaps=0;      //Перевод строки
int i=0;
int shet=0;
int	shetsimv=0; //Счетчик выведенных символов в строке
int	dlinastr=0;
double	razmer_Y=visota_shrifta; //Размер пройденый по вертикали листа от первой строки в милиметрах
double  razmer_X=0;  //Ширина строки в пунктах
char	strokav[MAXS]; 	//Строка сформированная для вывода в текущий лист
char	stroka[MAXS];  //Строка прочитанная из файла
char    ost_str[MAXS]; //Отаток строки который не поместился на лист
FILE *ff_tmp_w=NULL;
short metka_2hir=0; //0-нормальная ширина символа 1-двойная ширина символа
memset(ost_str,'\0',MAXS);
int razmer_str=0; 
memset(stroka,'\0',MAXS);
memset(strokav,'\0',MAXS);
int metka_str_1=0;
class iceb_tu_str strtmp("");

while(fgets(stroka,sizeof(stroka),ffist) != NULL)
 {

  /*Избавляемся от пробелов в конце строки и возврата каретки*/
  int metka_konc=0;
  razmer_str=strlen(stroka)-1;

  if(stroka[razmer_str] == '\n')
   {
    razmer_str--;
    metka_konc=1;
    if(stroka[razmer_str] == '\r')
     {
      stroka[razmer_str]='\n';
      stroka[razmer_str+1]='\0';
      razmer_str--;
     }
   }


  for(int nom=razmer_str; nom > 0; nom--)
   {
    if(stroka[nom] != ' ')
      break;
    if(metka_konc == 1)
     {

      stroka[nom]='\n';
      stroka[nom+1]='\0';
     }
    else
     stroka[nom]='\0';
   }
  /******************
  Необходимо помнить что в начале файла может быть несколько 
  команд для принтера, которые могут образовать псевдо-перевод 
  строки и псевдо табулятор
  **************/
  
  if(metka_1_stroki == 0)
   {
    if (orientation != Landscape) //ориентация может быть задана ключом -l
    if(stroka[0] == 27 && stroka[1] == 50 && stroka[2] == 27 && stroka[3] == 50)
     {
      orientation = Landscape;
      iceb_ps_lands_on(ffpri);
     }
    metka_1_stroki=1;
   }
  
  if(metka_str_1 == 0)
   {
    //запускаем новую страницу здесь так как нужно учитывать ориентацию которая определена только что
    newpage(nomlist,ffpri,&ff_tmp_w);
    if(ff_tmp_w == NULL)
      fprintf(stderr,"%s-ff_tmp_w=NULL\n",__FUNCTION__);
    fprintf(ffpri, "%d otstup\n",0); //Так как подпрограмма работает рекурсивно обязательно устанавливаем отступ равным нолю
    metka_str_1=1;
   }

//  fprintf(stderr,"11111111111\n");
  if(strstr(stroka,STROKA_NASTR) != NULL)
   {
    iceb_ps_nastr_hrift(stroka,ffpri,ffist);
    continue;
   }

  razmer_X=TN.tek_otstup_punkt; 
//  razmer_X=TN.tek_otstup_punkt+(lev_zona*72./25.4); 


  metkaff=0;
  if(metkaps == 1)
   {
    fprintf(ffpri,"MV\n");

    fprintf(ff_tmp_w,"%s\n%f\n%f\n%f\n%s\n",
    STROKA_NASTR,
    TN.tek_ss_p,
    TN.tek_sag_str,
    TN.tek_m_shrifta,
    TN.naim_shrift);

    if(ost_str[0] == '\0')
       fprintf(ff_tmp_w,"\n");
    else
       fprintf(ff_tmp_w,"%s",ost_str);

//    if(razmer_Y >= (RAZMERLISTAY - deltay))
    if(razmer_Y > zona_y_tek)
     {
      metkaff=1;
      newpage(nomlist,ffpri,&ff_tmp_w);
      razmer_Y=TN.tek_sag_str;
     }
    metkaps=0;
    
   }
  memset(ost_str,'\0',sizeof(ost_str));
  
  dlinastr=strlen(stroka);

  shetsimv=0;  

  for(shet=0; shet < dlinastr; shet++)
   if(iscntrl(i=stroka[shet])) //Проверка на управляющий символ
   {
    switch(i)
     {
     /*
      *   ESC-последовательности
      */
      case 27: //Esc
       vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть

       shet++;
       switch (stroka[shet])
        {
          case 15: /*Включение уплотнённого режима печати*/
            goto kom15;
            
          case '-': //Включить/выключить режим подчеркивания

             shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки

             if(stroka[shet] == 49) //Включить
              {
               fprintf(ffpri,"/metkaunderline 1 def\n");
               TN.metka_underline=1;
              }

             if(stroka[shet] == 48) //Выключить
              {
               fprintf(ffpri,"/metkaunderline 0 def\n");
               TN.metka_underline=0;
              }
             continue;

          case '0':		/* межстрочный интервал 1/8 inch */
             TN.tek_sag_str=25.4/8.;
             fprintf(ffpri,"/mstrr %f milim def \n",TN.tek_sag_str);
             continue;

          case '2': //Межстрочный интервал 1/6 дюйма
              TN.tek_sag_str=25.4/6.;
              fprintf(ffpri,"/mstrr %f milim def \n",TN.tek_sag_str);
              continue;

          case '3': //Межстрочный интервал n/216 дюйма
             shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
             TN.tek_sag_str=stroka[shet]*25.4/216.;
             fprintf(ffpri,"/mstrr %f milim def \n",TN.tek_sag_str);

             continue;

          case '[': //Установка высоты символа - сам придумал эту команду
             shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
//             fprintf(stderr,"изменение высоты символа\n");
             TN.visota=stroka[shet];
             fprintf(ffpri,"%%Устанавливаем шрифт по умолчанию\n/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
             continue;

          case 'A': //Межстрочный интервал n/72 дюйма

             shet++;  //Увеличиваем счётчик потому, что нужен следующий элемент строки

             TN.tek_sag_str=stroka[shet]*25.4/72.;
             fprintf(ffpri,"/mstrr %f milim def \n",TN.tek_sag_str);
             continue;

          case 'G': //Двуударный режим печати
          case 'E': //Выделенный шрифт

                 strcpy(TN.naim_shrift,S_bold);

                 fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
                 TN.naim_shrift, TN.tek_m_shrifta, TN.visota);

                 fprintf(ffpri,"/tektl tln2 def\n");

                 continue;

          case 'H': //Отмена двуударного режима печати
          case 'F': //Отмена выделенного шрифта

            strcpy(TN.naim_shrift,S_normal);

            fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
            TN.naim_shrift, TN.tek_m_shrifta, TN.visota);

            fprintf(ffpri,"/tektl tln def\n");

            continue;

          case 'P': //Десять знаков на дюйм
               if(metka_shrift != 0) /*Устанавливат если не установлен*/
                {
                 metka_shrift=0;
                 if(metka_shrift_u == 0)
                  {
                   TN.tek_ss_p=ss10;
                   TN.tek_m_shrifta=HS10;
                  }
                 if(metka_shrift_u == 1)
                  {
                   TN.tek_ss_p=ss10u;
                   TN.tek_m_shrifta=HS10u;
                  }
                 fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);
                 
                 fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
                 TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
                }
               continue;

          case 'M': //Двенадцать знаков на дюйм
               if(metka_shrift != 1) /*Устанавливат если не установлен*/
                {
                 metka_shrift=1;
                 if(metka_shrift_u == 0)
                  {
                   TN.tek_ss_p=ss12;
                   TN.tek_m_shrifta=HS12;
                  }
                 if(metka_shrift_u == 1)
                  {
                   TN.tek_ss_p=ss12u;
                   TN.tek_m_shrifta=HS12u;
                  }
                 fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);

                 fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
                 TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
                }
               continue;

          case 'g': //Пятнадцать знаков на дюйм (применяестся не на всех принтерах)
               if(metka_shrift != 2) /*Устанавливат если не установлен*/
                {
                 metka_shrift=2;
                 if(metka_shrift_u == 0)
                  {
                   TN.tek_ss_p=ss15;
                   TN.tek_m_shrifta=HS15;
                  }
                 if(metka_shrift_u == 1)
                  {
                   TN.tek_ss_p=ss15u;
                   TN.tek_m_shrifta=HS15u;
                  }
                 fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);

                 fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
                 TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
                }
               continue;

                 
          case 'l': //Установка левого поля

                 shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки

                /* Изменение отступа	*/
                 fprintf(ffpri, "%d otstup\n", stroka[shet]);
                 TN.tek_otstup=stroka[shet];
/************************
                //Отступ устанавливается от края листа, а длинну строки
                //считаем от начала печати поэтому надо отнять зону недоступную для печати

                 TN.tek_otstup_punkt=TN.tek_ss_p*stroka[shet]-(lev_zona*72./25.4);

                 if(TN.tek_otstup_punkt < 0)
                   TN.tek_otstup_punkt=0;

                 poz_per_stroki_x_tek=poz_per_stroki_x+TN.tek_otstup_punkt;

                 if(razmer_X == 0)
                  razmer_X=TN.tek_otstup_punkt; 
****************************/
                //Отступ указывается от края границы печати
                 TN.tek_otstup_punkt=TN.tek_ss_p*stroka[shet];
                 /*отступ может увеличиваться и уменьшаться поэтому используем poz_per_stroki_x*/
                 poz_per_stroki_x_tek=poz_per_stroki_x+TN.tek_otstup_punkt +(lev_zona*72./25.4);

                 if(razmer_X == 0)
                  razmer_X=TN.tek_otstup_punkt; 
                 continue;
                 
          /* Нереализованные трехсимвольные команды*/
          case 'U': //Включение/выключение однонаправленного режима печати
          case 'x': //Включение/выключение высококачественного режима печати
          case 'S': //Выбор режима печати индексов
          case 'W': //Включение/выключение  режима печати расширенными символами
          case 'a': //Выравнивание текста по правую или левую границу печати
          case 'J': //Перевод бумаги в прямом направлении на заданное количество елементрарных шагов 
          case 'j': //Перевод бумаги в обратном направлении на заданное количество елементрарных шагов
            shet++;  //Трех-символьная команда игнорируем третий символ
            continue;
                 
          case '@': //Инициализация принтера
             //счётчик не увеличиваем-двубайтная последовательность
             continue;

                 
          default: //Игнорируются неизвестные двубайтные последовательности
            continue;
        }
   
     /*
      *   1-байтные управляющие символы
      */
     case 14: //^N - Двойная ширина символа для одной строки
       if(metka_2hir == 0) /*Устанавливать если ещё не установлена*/
        {
         vstr(&kolsv,strokav,ffpri);
         TN.tek_ss_p*=2.;
         TN.tek_m_shrifta*=2.;
         fprintf(ffpri,"/tekhs tekhs 2 mul def\n");
         fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",TN.naim_shrift,TN.tek_m_shrifta, TN.visota);
        }
       metka_2hir=1;
       continue;


     case 15: //^O - Включить уплотненный режим печати
kom15:;
       if(metka_shrift_u == 0) /*Включать если выключен*/
        {
         vstr(&kolsv,strokav,ffpri);
         metka_shrift_u=1;
         if(metka_shrift == 0)
          {
           TN.tek_ss_p=ss10u;
           TN.tek_m_shrifta=HS10u;
          }
         if(metka_shrift == 1)
          {
           TN.tek_ss_p=ss12u;
           TN.tek_m_shrifta=HS12u;
          }
         if(metka_shrift == 2)
          {
           TN.tek_ss_p=ss15u;
           TN.tek_m_shrifta=HS15u;
          }
         fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);
         fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
         TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
        }
       continue;
   
     case 18: //^R - Выключить уплотненный режим печати
       if(metka_shrift_u == 1) /*Выключать если включен*/
        {
         vstr(&kolsv,strokav,ffpri);
         metka_shrift_u=0;
         if(metka_shrift == 0)
          {
           TN.tek_ss_p=ss10;
           TN.tek_m_shrifta=HS10;
          }
         if(metka_shrift == 1)
          {
           TN.tek_ss_p=ss12;
           TN.tek_m_shrifta=HS12;
          }
         if(metka_shrift == 2)
          {
           TN.tek_ss_p=ss15;
           TN.tek_m_shrifta=HS15;
          }
         fprintf(ffpri,"/tekhs %f def\n",TN.tek_ss_p);
         fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
         TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
        }
       continue;

     case 20: //^T - Отмена двойной ширины символа для одной строки
      
       if(metka_2hir == 1) /*Отменять если она установлена*/
        {
         vstr(&kolsv,strokav,ffpri);
         TN.tek_ss_p/=2.;
         TN.tek_m_shrifta/=2.;
         fprintf(ffpri,"/tekhs tekhs 2 div def\n");
         fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
        }
       metka_2hir=0;
       continue;
   
     case '\n':  //Перевод строки. 
       //Строка всегда заканчивается символом перевода строки, поэтому после 
       //этого последует выход из цикла
       vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
       razmer_Y+=TN.tek_sag_str; //именно здесь так как строка может состоять из команд принтеру и заканчиваться псевдо переводом строки
       metkaps=1;

       continue;

     case '\r':  /*возврат каретки игнорируем*/
       continue;

     case '\f':  //Перевод формата
       vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
//       if (razmer_Y > tek_sag_str) 
       if(metkaff == 0)
        { /* пустые листы не прогоняем	*/
         razmer_Y=TN.tek_sag_str;	//устанавливаем в позицию первой строки
         newpage(nomlist,ffpri,&ff_tmp_w);
        }

       continue;

     case '\t':  //Табулятор
       //Тупо заменять табулостопы в строке нельзя так как в ЕSC командах с 
       //с параметрами они могут быть в роли параметра
/***************
       char strtmp[MAXS];
       memset(strtmp,'\0',MAXS);
       strcpy(strtmp,&stroka[shet+1]);
******************/
       strtmp.new_plus(&stroka[shet+1]);
       
       i = 8 - (shetsimv % 8);	// кол-во пробелов до следующего табулостопа 	
       dlinastr+=i;
       char bros[20];
       memset(bros,'\0',sizeof(bros));
       memset(bros,' ',i);

       //чистим концовку строки
       for(int ii=shet; ii < MAXS; ii++)
        stroka[ii]='\0';

       strcat(stroka,bros);
//       strcat(stroka,strtmp);
       strcat(stroka,strtmp.ravno());
      
       shet--; //возвращаем счётчик на табулостоп
              
/************* 
       while (i--) 
        {		//    добавляем столько же пробелов	
         if (kolsv < MAXS)
          {
           strokav[kolsv++] = ' ';
           shetsimv++; 
          }
        }
****************/
       continue;

     default:
       /* остальные управляющие символы пропускаем */
       continue;
       
     }
   } 
  else 
   {
   /*** 
    ***  Алфавитно-цифровой символ
    ***/
    if(A4_obrez == 0)
    if(razmer_X+TN.tek_ss_p > max_dlinna_stroki)
     {
//      fprintf(stderr,"razmer_X=%f TN.tek_ss_p=%f количество символов=%d\n",razmer_X,TN.tek_ss_p,shetsimv);
//      getchar();      
      vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
      metkaps=1;

      razmer_Y+=TN.tek_sag_str; //именно здесь так как строка может состоять из команд принтеру и заканчиваться псевдо переводом строки
      strcpy(ost_str,&stroka[shet]);
      metka_h_list=1;
      break;
     }
    razmer_X+=TN.tek_ss_p;
    shetsimv++;
    switch (i) 
     {
       /* псевдографические линии */
//       case '─':
       case -128:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"liniq\n");
         continue;
//       case '┌':
       case -126:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"vlugol\n");
         continue;
//       case '┐':
       case -125:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"vpugol\n");
         continue;
//       case '┘':
       case -123:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"npugol\n");
         continue;
//       case '└':
       case -124:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"nlugol\n");
         continue;
//       case '│':
       case -127:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"vliniq\n");
         continue;
//       case '┬':
       case -120:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"tvniz\n");
         continue;
//       case '┴':
       case -119:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"tvverh\n");
         continue;
//       case '┤':
       case -121:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"tvlevo\n");
         continue;
//       case '├':
       case -122:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"tvpravo\n");
         continue;
//       case '┼':
       case -118:
         vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть
         fprintf(ffpri,"krest\n");
         continue;
       
       /* символы, имеющие спец. значение в PostScript - экранируем */
       case '(':
       case ')':
       case '\\':
         if (kolsv < MAXS) 
           strokav[kolsv++]='\\';

       /* остальные символы считаем печатными и выводим как есть */
       default:
         if (kolsv < MAXS) 
           strokav[kolsv++]=i;
         break;
     }
   }
  memset(stroka,'\0',MAXS);
  if(metka_2hir == 1) /*Отменяем потому что закончилась строка*/
   {
    metka_2hir=0;
    TN.tek_ss_p/=2.;
    TN.tek_m_shrifta/=2.;
    fprintf(ffpri,"/tekhs tekhs 2 div def\n");
    fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",TN.naim_shrift, TN.tek_m_shrifta, TN.visota);
   }
 }

vstr(&kolsv,strokav,ffpri); //Выводим строку если она есть


fprintf(ff_tmp_w,"%s\n%f\n%f\n%f\n%s\n",
STROKA_NASTR,
TN.tek_ss_p,
TN.tek_sag_str,
TN.tek_m_shrifta,
TN.naim_shrift);

if(ost_str[0] == '\0')
   fprintf(ff_tmp_w,"\n");
else
   fprintf(ff_tmp_w,"%s",ost_str);

if(ff_tmp_w != NULL)
 {
  fclose(ff_tmp_w);
  ff_tmp_w=NULL;
 }

iceb_ps_rekur(nomlist,ffpri);

}
/*********************************/
/*рекурсивное продолжение                               */
/***********************************/
void iceb_ps_rekur(int *nomlist,FILE *ffpri)
{
if(metka_h_list == 0)
 return;

class iceb_ps_nastr ZAP; 
ZAP.copy(&TN); //Запоминаем настройки действовавшие в конце предыдущего листа

metka_h_list=0;

//переименовываем файл в который было выведено то что не поместилось по ширине
rename(imaf_tmp_w,imaf_tmp_r);

FILE *ff_tmp_r;

if((ff_tmp_r = fopen(imaf_tmp_r,"r")) == NULL)
 {
  char bros[312];
  sprintf(bros, "ERROR: %s", imaf_tmp_r);
  perror(bros);
 }

iceb_ps_main(nomlist,ffpri,ff_tmp_r);

iceb_ps_zapnastr(&TN,&ZAP,ffpri);
  
}
/******************************/
/*Установка новых характеристик текста*/
/**************************************/
void  iceb_ps_nastr_hrift(char *stroka_nastr,FILE *ffpri,FILE *ffist)
{

class iceb_ps_nastr NN; //настройки в файлах в которые выгружено то что не поместилось

char bros[112];

if(fgets(bros,sizeof(bros),ffist) != NULL)
 NN.tek_ss_p=atof(bros);

if(fgets(bros,sizeof(bros),ffist) != NULL)
 NN.tek_sag_str=atof(bros);

if(fgets(bros,sizeof(bros),ffist) != NULL)
 NN.tek_m_shrifta=atof(bros);

if(fgets(NN.naim_shrift,sizeof(NN.naim_shrift),ffist) != NULL);
 
 
//избавляемся от перевода строки
int dlinna=strlen(NN.naim_shrift);
NN.naim_shrift[dlinna-1]='\0';

iceb_ps_zapnastr(&TN,&NN,ffpri);

}
/*************************************/
/*Запись изменившихся настроек в файл*/
/**************************************/
void iceb_ps_zapnastr(class iceb_ps_nastr *S_N, //Старые настройки
class iceb_ps_nastr *N_N, //Новые настройки
FILE *ffpri)
{

//размер масштаба шрифта всегда меняется вслед за изменением ширины шрифта или
//наименования - выделенный или обычный

if(fabs(N_N->tek_ss_p- S_N->tek_ss_p) > 0.0000009)
  fprintf(ffpri,"/tekhs %f def\n",N_N->tek_ss_p);

if(fabs(N_N->tek_ss_p- S_N->tek_ss_p) > 0.0000009 || strcmp(N_N->naim_shrift,S_N->naim_shrift) != 0)
 {
  fprintf(ffpri,"/%s findfont [%f 0 0 %f 0 0] makefont setfont\n",
  N_N->naim_shrift, N_N->tek_m_shrifta, TN.visota);
 }
//if(N_N->tek_sag_str != S_N->tek_sag_str)
if(fabs(N_N->tek_sag_str- S_N->tek_sag_str) > 0.0000009)
 fprintf(ffpri,"/mstrr %f milim def \n",N_N->tek_sag_str);

if(strcmp(N_N->naim_shrift,S_N->naim_shrift) != 0)
 {
  if(strcmp(N_N->naim_shrift,S_bold) == 0)
     fprintf(ffpri,"/tektl tln2 def\n");
  if(strcmp(N_N->naim_shrift,S_normal) == 0)
     fprintf(ffpri,"/tektl tln def\n");
 }
if(N_N->tek_otstup != S_N->tek_otstup)
  fprintf(ffpri, "%d otstup\n",N_N->tek_otstup);

S_N->copy(N_N);
}
/*********************************************************/
/*Определение кодировки и перекодировка при необходимости*/
/*********************************************************/
FILE *iceb_ps_cod(FILE *ffist)
{
int razmer=0;
int nomer=0;
int kod_simv=0;
FILE *ff_tmp;


if((ff_tmp = fopen(imaf_tmp_potok,"w")) == NULL)
 {
  fprintf(stderr,"%s-Ошибка открытия файла %s\n%d %s\n",__FUNCTION__,imaf_tmp_potok,errno,strerror(errno));
  return(ffist);
 }

/*Переписываем в промежуточный файл так как если это входной поток то ему нельзя сделать rewind*/
while((kod_simv=fgetc(ffist)) != EOF)
 {
  razmer++;
  fputc(kod_simv,ff_tmp);  
 }
fclose(ff_tmp);

if((ff_tmp = fopen(imaf_tmp_potok,"r")) == NULL)
 {
  fprintf(stderr,"%s-Ошибка открытия файла %s\n%d %s\n",__FUNCTION__,imaf_tmp_potok,errno,strerror(errno));
  return(ffist);
 }

fclose(ffist);

char *masiv=new char[razmer];


while((kod_simv=fgetc(ff_tmp)) != EOF)
 masiv[nomer++]=kod_simv;  

rewind(ff_tmp);

gchar *masivn=NULL;
gsize razmern=0;
if(g_utf8_validate(masiv,razmer,NULL) == FALSE)
 {
  delete [] masiv;
  return(ff_tmp);
 }
gchar fallback[8];
strcpy(fallback,"?");
//fprintf(stderr, "Файл в кодировке utf8\n");
//if((masivn=g_convert(masiv,razmer,"koi8u//TRANSLIT","utf8//TRANSLIT",NULL,&razmern,NULL)) == NULL)
if((masivn=g_convert_with_fallback(masiv,razmer,"koi8-u","utf-8",fallback,NULL,&razmern,NULL)) == NULL)
  fprintf(stderr,"%s-ошибка конвертации файла utf-8 -> koi8-u\n",__FUNCTION__);

delete [] masiv;

if(masivn == NULL)
  return(ff_tmp);

fclose(ff_tmp);
FILE *ffto=NULL;

if((ffto = fopen(imaf_tmp_perecod,"w")) == NULL)
 {
  fprintf(stderr,"%s-Ошибка открытия файла %s\n%d %s\n",__FUNCTION__,imaf_tmp_perecod,errno,strerror(errno));
  g_free(masivn);
  return(ffist);
 }
//fprintf(stderr, "razmern=%d\n",razmern);

for(gsize nom=0; nom < razmern; nom++)
  fputc(masivn[nom],ffto);  

fclose(ffto);
if((ffto = fopen(imaf_tmp_perecod,"r")) == NULL)
 {
  fprintf(stderr, "%s-Ошибка открытия файла %s\n%d %s\n",__FUNCTION__,imaf_tmp_perecod,errno,strerror(errno));
  g_free(masivn);
  return(ffist);
 }

g_free(masivn);


return(ffto);
}
/*****************************/
/*установки для ландшафтной ориентации*/
/***********************************/
void iceb_ps_lands_on(FILE *ffpri)
{
double dd = 0.;
/*базовые размеры не используются - это более универсальный способ*/
dd=RAZMERLISTAX;
RAZMERLISTAX = RAZMERLISTAY;
RAZMERLISTAY = dd;

otstup_y=lev_zona+visota_shrifta; //где 2 высота шрифта (померял линейкой)

dd=lev_zona;
lev_zona=nig_zona;
nig_zona=dd;

//Размер нижней зоны листа недоступной для печати в милиметрах
fprintf(ffpri,"/OTSTXMIN %f milim def %%Граница начала зоны печати с левой стороны листа\n",lev_zona);

//deltay=10.; //подобрано экспериментально

max_dlinna_stroki=zona_y*72./25.4; //Максимальная длинна строки в пуктах, которая может поместится на А4 


poz_per_stroki_y_tek=RAZMERLISTAY*72/25.4-otstup_y*72/25.4; 
poz_per_stroki_x=lev_zona*72/25.4;  /*не меняется после изменения ориентации*/
poz_per_stroki_x_tek=poz_per_stroki_x;  /*меняется когда задан отступ может увеличиваться и уменьшаться*/

zona_y_tek=zona_x;

}