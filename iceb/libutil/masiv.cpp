/*$Id: masiv.c,v 5.46 2013/09/26 09:44:09 sasa Exp $*/
/*29.04.2015	02.02.2001	Белых А.И.	masiv.c
Программы работы с динамическими массивами
*/
#include	"util.h"

//******************************************
//*************** DOUBLE ******************
//******************************************

/****************************/
/*Конструктор объекта       */
/****************************/
masiv_din_double::masiv_din_double(int optimz) //0-Без оптимизации 1-с оптимизацией
{
kolz=0;
optimiz=optimz;
masiv_double=NULL;
}

/****************************/
/*Конструктор объекта без параметров*/
/****************************/
masiv_din_double::masiv_din_double()
{
kolz=0;
optimiz=0;
masiv_double=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
masiv_din_double::~masiv_din_double()
{
 masiv_din_double::free_class();
}
/******************************/
/*Вернуть количество элементов*/
/*******************************/
int masiv_din_double::kolih()
{
return(kolz);
}
/*************************************************/
/*Записать значение в добавленную ячейку массива*/
/*************************************************/

int masiv_din_double::plus(double znah) //Значение 
{
return(plus(znah,-1));
}
/********************************************************************/
/*проверить есть ли не нулевые значения в колоке двумерного массива*/
/*Если вернули 0 - значит все значения нулевые*/
/********************************************************************/
int masiv_din_double::prov_kol(int kol_str, /*Количество строк в двумерном массиве*/
int kol_kol, /*Количество колонок в массиве*/
int nom_kol) /*Номер колонки которую нужно проверить*/
{
if(kolz < kol_str*kol_kol)
 return(-1);
for(int nom=0; nom < kol_str; nom++)
 {
  if(masiv_double[nom*kol_kol+nom_kol] != 0.)
   return(1);
 }
return(0);
}
/************************************************/
/*получить сумму по колонке двухмерного массива*/
/************************************************/
double masiv_din_double::suma_kol(int kol_str, /*Количество строк в двумерном массиве*/
int kol_kol, /*Количество колонок в массиве*/
int nom_kol) /*Номер колонки которую нужно проверить*/
{
double suma=0.;
if(kolz < kol_str*kol_kol)
 return(0.);
for(int nom=0; nom < kol_str; nom++)
  suma+=masiv_double[nom*kol_kol+nom_kol];
return(suma);
}

/*************************************************/
/*Увеличить размер массива на заданное количество елементов*/
/*************************************************/

int masiv_din_double::plus_str(int znah) //Значение 
{
if((masiv_double=(double *)realloc(masiv_double,(kolz+znah)*sizeof(double))) == NULL)
 {
  printf("\n%s-Не могу добавить память для masiv_double double !!!\n",__FUNCTION__);
  return(-1);
 }  

for(int ii=kolz; ii < kolz+znah; ii++)
  masiv_double[ii]=0.;
kolz+=znah;

return(0);
}  
/******************************************************/
/*Добавить значение в масив  просумировав его с предыдущим значением и если нужно с увеличением его размера*/
/*******************************************************/

int masiv_din_double::plus(double znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("masiv_din_double::add_masiv_double znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_double=(double *)calloc(1,sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_double=(double *)realloc(masiv_double,kolz*sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_double double !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_double[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_double[zapis]+=znah;
 } 
return(0);

}

/******************************************************/
/*Добавить значение в масив не просумировав его с предыдущим значением и если нужно с увеличением его размера*/
/*******************************************************/

int masiv_din_double::new_plus(double znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("masiv_din_double::add_masiv_double znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_double=(double *)calloc(1,sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_double=(double *)realloc(masiv_double,kolz*sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_double double !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_double[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_double[zapis]=znah;
 } 
return(0);

}
/*********************/
/*Уничтожение массива*/
/**********************/
void masiv_din_double::free_class()
{
kolz=0;
if(masiv_double != NULL)
 {
  free(masiv_double);
  masiv_double=NULL;
 }
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int masiv_din_double::find(double znah)
{

for(int i=0; i < kolz; i++)
 if(znah == masiv_double[i])
  return(i);
return(-1);

}
/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int masiv_din_double::find(const char *znah)
{
 return(find(atof(znah)));
}
/*****************************************************/
/*Вернуть значение нужного елемента массива*/
/*****************************************************/
double masiv_din_double::ravno(int elem)
{
if(elem > kolz)
 {
  printf("\n%s-Запрошенный елемент больше размера массива elem=%d > kolz=%d\n",__FUNCTION__,elem,kolz);
  return(0.);
 }
return(masiv_double[elem]);

}
/************************************/
/* Создать массив*/
/************************************/
int masiv_din_double::make_class(int razmer) //Размер массива
{
if(razmer == 0)
 return(0);
 
kolz=razmer;
if((masiv_double=(double *)calloc(razmer,sizeof(double))) == NULL)
 {
  printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
  return(1);
 }
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void masiv_din_double::print_class()
{
printf("%s: kolz=%d\n",__FUNCTION__,kolz);
for(int i=0; i < kolz; i++)
  printf("%f\n",masiv_double[i]);
}
/******************/
/*Очистить массив*/
/******************/
void masiv_din_double::clear_class()
{
for(int i=0; i < kolz;i++)
  masiv_double[i]=0.;
}

/**************************************/
/*Вернуть сумму всех элементов массива*/
/**************************************/
double masiv_din_double::suma(void)
{
double suma=0.;
for(int i=0; i < kolz; i++)
  suma+=masiv_double[i];
return(suma);
}


//******************************************
//*************** INT ******************
//******************************************

/****************************/
/*Конструктор объекта       */
/****************************/
masiv_din_int::masiv_din_int(int optimz) //0-Без оптимизации 1-с оптимизацией
{
kolz=0;
optimiz=optimz;
masiv_int=NULL;
}

masiv_din_int::masiv_din_int() //конструктор без параметров
{
kolz=0;
optimiz=0;
masiv_int=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
masiv_din_int::~masiv_din_int()
{
 masiv_din_int::free_class();
}
/****************************************/
/*Записать в добавленный элемент массива*/
/*****************************************/
int masiv_din_int::plus(int znah) //Значение 
{
return(plus(znah,-1));
}
/****************************************/
/*Записать в добавленный элемент массива*/
/*****************************************/
int masiv_din_int::plus(const char *znah) //Значение 
{
return(plus(atoi(znah),-1));
}
/****************************************/
/*Записать в добавленный элемент массива*/
/*****************************************/
int masiv_din_int::plus(char *znah) //Значение 
{
return(plus(atoi(znah),-1));
}
/******************/
/*Очистить массив*/
/******************/
void masiv_din_int::clear_class()
{
for(int i=0; i < kolz;i++)
  masiv_int[i]=0.;
}

/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int masiv_din_int::plus(int znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("masiv_din_int::add_masiv_int znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_int=(int *)calloc(1,sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_int=(int *)realloc(masiv_int,kolz*sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_int[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_int[zapis]+=znah;
 } 
return(0);

}
/******************************************************/
/*Добавить значение в масив с увеличением его размера удалив предыдущее значение*/
/*******************************************************/

int masiv_din_int::new_plus(int znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("masiv_din_int::add_masiv_int znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_int=(int *)calloc(1,sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_int=(int *)realloc(masiv_int,kolz*sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_int[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_int[zapis]=znah;
 } 
return(0);

}
/******************************************************/
/*Добавить значение в символьном виде в масив с увеличением его размера*/
/*******************************************************/

int masiv_din_int::plus(const char *znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{

return(plus(atoi(znah),zapis));
}
/******************************************************/
/*Добавить значение в символьном виде в масив с увеличением его размера*/
/*******************************************************/

int masiv_din_int::plus(char *znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{

return(plus(atoi(znah),zapis));
}

/*********************/
/*Уничтожение массива*/
/**********************/
void masiv_din_int::free_class()
{

kolz=0;
if(masiv_int != NULL)
 {
  free(masiv_int);
  masiv_int=NULL;
 }
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int masiv_din_int::find(int znah)
{

for(int i=0; i < kolz; i++)
 if(znah == masiv_int[i])
  return(i);
return(-1);

}
/*****************************************************/
/*Найти в каком елементе массива находится значение в символьном виде*/
/*****************************************************/
int masiv_din_int::find(const char *znah)
{
return(find(atoi(znah)));


}
/*****************************************************/
/*Найти в каком елементе массива находится значение в символьном виде*/
/*****************************************************/
int masiv_din_int::find(char *znah)
{
return(find(atoi(znah)));


}
/*****************************************************/
/*Вернуть значение нужного елемента массива*/
/*****************************************************/
int masiv_din_int::ravno(int elem)
{
if(elem > kolz)
 {
  printf("\n%s-Запрошенный елемент больше размера массива elem=%d > kolz=%d\n",__FUNCTION__,elem,kolz);
  return(0);  
 }
if(kolz == 0)
 {
  printf("\n%s-Количество элементов в массиве равно нолю !!!\n",__FUNCTION__);
  return(0);  
 }
return(masiv_int[elem]);

}
/*****************************************************/
/*Вернуть значение нужного елемента массива в символьном виде*/
/*****************************************************/
char *masiv_din_int::ravno_char(int elem)
{
static char str[96];
sprintf(str,"%d",ravno(elem));
return(str);
}
/************************************/
/* Создать массив*/
/************************************/
int masiv_din_int::make_class(int razmer) //Размер массива
{
if(razmer == 0)
 return(0);
kolz=razmer;
if((masiv_int=(int *)calloc(razmer,sizeof(int))) == NULL)
  {
   printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
   return(-1);
  }
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void masiv_din_int::print_class()
{
printf("%s: kolz=%d\n",__FUNCTION__,kolz);
for(int i=0; i < kolz; i++)
  printf("%d\n",masiv_int[i]);
}

/************************************************/
/*Вернуть максимальное значение елемента массива*/
/************************************************/
int masiv_din_int::ravno_max()
{
int	max=0;
for(int i=0; i < kolz;i++)
 if(masiv_int[i] > max)
  max=masiv_int[i];
return(max);
}
/****************************************/
/*Вернуть количество элементов в массиве*/
/****************************************/
int masiv_din_int::kolih()
{
return(kolz);
}
/*************************/
int masiv_din_int::suma()
{
int suma=0.;
for(int i=0; i < kolz; i++)
  suma+=masiv_int[i];
return(suma);
}


//******************************************
//*************** CHAR ******************
//******************************************


/****************************/
/*Конструктор объекта       */
/****************************/
masiv_din_char::masiv_din_char(char kk)
{
razdel=kk;
kol=razmer=0;
masiv_char=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
masiv_din_char::~masiv_din_char()
{
 masiv_din_char::free_masiv_char();
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/
int masiv_din_char::add_masiv_char(char *znah) //Значение 
{

if(razmer == 0)
 {
  razmer=strlen(znah)+2;
  if((masiv_char=(char *)calloc(razmer,sizeof(char))) == NULL)
   {
    printf("\n%s-Не могу выделить память для masiv_char char !!!\n",__FUNCTION__);
    return(-1);
   }
  kol++;
 }
else
 {
  razmer+=strlen(znah)+1;

  if((masiv_char=(char *)realloc(masiv_char,razmer*sizeof(char))) == NULL)
   {
    printf("\n%s-Не могу добавить память для masiv_char char !!!\n",__FUNCTION__);
    return(-1);
   }  
  kol++;
 }
strcat(masiv_char,znah);
char bros[16];
sprintf(bros,"%c",razdel);
strcat(masiv_char,bros);
return(kol);

}
/*********************/
/*Уничтожение массива*/
/**********************/
void masiv_din_char::free_masiv_char()
{
//printf("\nmasiv_din_char::free_masiv_char\n");

kol=razmer=0;
if(masiv_char != NULL)
 {
  free(masiv_char);
  masiv_char=NULL;
 }
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/

int masiv_din_char::find_masiv_char(char *znah)
{
int	i1;

if(masiv_char == NULL)
 return(-1);
/*
printf("masiv_din_char::find_masiv_char:\n\
masiv_char=%s znah=%s razd=%c\n",masiv_char,znah,razd);
*/
if(pole1(masiv_char,znah,razdel,2,&i1) != 0)
  return(-1);

return(i1);
}

/********************************************/
/*Получить значение нужного элемента массива*/
/*********************************************/
int masiv_din_char::pol_masiv_char(int elem,char *znah)
{
if(masiv_char == NULL)
 return(-1);

if(POLE(masiv_char,znah,elem,razdel) != 0)
 return(-1);
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void masiv_din_char::print_masiv_char()
{
//printf("masiv_din_char::print_masiv_char:\n");
if(masiv_char != NULL)
 printf("%s\n",masiv_char);
else
 printf("Масив пуст !\n");
}
/*****************************/
/*получить количество элементов массива*/
/****************************/
int masiv_din_char::kolelem_masiv_char()
{
return(kol);
}
