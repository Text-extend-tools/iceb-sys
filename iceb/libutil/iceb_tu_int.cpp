/*$Id: iceb_tu_int.c,v 5.4 2013/05/17 14:56:40 sasa Exp $*/
/*02.04.2010	15.12.2004	Белых А.И.	iceb_tu_int.c
Класс для работы с int-массивами переменной длинны
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"util.h"


//******************************************
//*************** INT ******************
//******************************************

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_tu_int::iceb_tu_int(int optimz) //0-Без оптимизации 1-с оптимизацией
{
kolz=0;
optimiz=optimz;
masiv_int=NULL;
}
/****************************/
/*Конструктор объекта без параметров      */
/****************************/
iceb_tu_int::iceb_tu_int()
{
kolz=0;
optimiz=0;
masiv_int=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
iceb_tu_int::~iceb_tu_int()
{
 iceb_tu_int::masiv_delete();
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_tu_int::plus(int znah) //Значение 
{
return(plus(znah,-1));
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_tu_int::plus(int znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("iceb_tu_int::add_masiv_int znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
OSTANOV();
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_int=(int *)calloc(1,sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
//      OSTANOV();
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
//      OSTANOV();
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
//    OSTANOV();
    return(-1);
   }
  masiv_int[zapis]+=znah;
 } 
return(0);

}

/*********************/
/*Уничтожение массива*/
/**********************/
void iceb_tu_int::masiv_delete()
{

kolz=0;
if(masiv_int != NULL)
 free(masiv_int);
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int iceb_tu_int::find(int znah)
{

for(int i=0; i < kolz; i++)
 if(znah == masiv_int[i])
  return(i);
return(-1);

}
/*****************************************************/
/*Вернуть значение нужного елемента массива*/
/*****************************************************/
int iceb_tu_int::ravno(int elem)
{
if(elem > kolz)
 {
  printf("\n%s-Запрошенный елемент больше размера массива elem=%d > kolz=%d\n",__FUNCTION__,elem,kolz);
//  OSTANOV();  
  return(0);  
 }
if(kolz == 0)
 {
  printf("\n%s-Количество элементов в массиве равно нолю !!!\n",__FUNCTION__);
//  OSTANOV();  
  return(0);  
 }
return(masiv_int[elem]);

}
/************************************/
/* Создать массив*/
/************************************/
int iceb_tu_int::make_masiv(int razmer) //Размер массива
{
if(razmer == 0)
 return(0);

if(kolz != 0)
 iceb_tu_int::masiv_delete();

kolz=razmer;
if((masiv_int=(int *)calloc(razmer,sizeof(int))) == NULL)
  {
   printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
//   OSTANOV();
   return(-1);
  }
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void iceb_tu_int::print_masiv()
{
printf("%s: kolz=%d\n",__FUNCTION__,kolz);
for(int i=0; i < kolz; i++)
  printf("%d\n",masiv_int[i]);
//OSTANOV();
}

/************************************************/
/*Вернуть максимальное значение елемента массива*/
/************************************************/
int iceb_tu_int::ravno_max()
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
int iceb_tu_int::kolih()
{
return(kolz);
}
