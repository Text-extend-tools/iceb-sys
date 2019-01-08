/*$Id: polen.c,v 5.20 2013/11/05 10:51:00 sasa Exp $*/
/*17.05.2015    16.03.1989      Белых А.И.      polen.c
Подпрограмма извлечения из строки символов нужного номера поля
с контролем размера массива
Если вернули "0" значит заказанный номер поля существует
если вернули "1" - нет
*/
#include	"util.h"

/*****************************************/

int polen(char const *D, //Строка из которой извлекают
class masiv_din_int *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  K->plus(str_prom);
  return(0);
 }
return(1);
}
/*******************************/
int polen(char const *D, //Строка из которой извлекают
class iceb_tu_str *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  K->new_plus(str_prom);
  return(0);
 }
K->new_plus("");
return(1);
}
/*******************************/

int polen(char const *D, //Строка из которой извлекают
int *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  *K=atoi(str_prom);
  return(0);
 }
*K=0;
return(1);
}
/*******************************/
int polen(char const *D, //Строка из которой извлекают
short *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  *K=atoi(str_prom);
  return(0);
 }
*K=0;
return(1);
}
/*******************************/
int polen(char const *D, //Строка из которой извлекают
double *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  *K=ATOFM(str_prom);
  return(0);
 }
*K=0.;
return(1);
}
/*******************************/
int polen(char const *D, //Строка из которой извлекают
float *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  *K=ATOFM(str_prom);
  return(0);
 }
*K=0.;
return(1);
}
/*******************************/
int polen(char const *D, //Строка из которой извлекают
long *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  *K=atol(str_prom);
  return(0);
 }
*K=0;
return(1);
}

/*****************************************/

int polen(char const *D, //Строка из которой извлекают
class SPISOK *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char str_prom[strlen(D)+1];
memset(str_prom,'\0',sizeof(str_prom));
if(polen(D,str_prom,sizeof(str_prom),N,R) == 0)
 {
  K->plus(str_prom);
  return(0);
 }
return(1);
}
/**********************************/

/*******************************************/

int polen(const char *D, //Строка из которой извлекают
char *K, //Поле которое выделили из строки
int razmer, //Размер К массива
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
int             I,Z,V;
short           l,ll;


/*Проверяем наличие хоть одного символа*/
Z=0;
for(I=0; D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I] == R)
   {
    Z++;
    break;
   } 
 }
if( Z == 0)
 {
  K[0]='\0';
  return(1);
 }

Z=1;

for(I=0; D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I] == R)
   Z++;
  if(Z == N)
   {
    if(N > 1)
     I++;
    V=0;
    for(; D[I] != R && V < razmer-1 ;I++)
     {
      if(D[I] == '\0' && N == 1)
       {
	K[0]='\0';
	return(1);
       }
      if(D[I] == '\0' || D[I] == '\r')
	break;
      K[V++]=D[I];
     }
    K[V]='\0';
    l=strlen(K);
    /*Убираем пробелы в конце строки*/
/*    printf("%d\n",l);*/
    for(ll=l-1; K[ll] == ' '|| K[ll] == '\t'|| K[ll] == '\n' || K[ll] == '\r';ll--)
     {
      K[ll]='\0';
     }
/*    printf("R - %c поле - %s  %d %d \n",R,K,strlen(K),K[l]);*/
    return(0);
   }
  }
K[0]='\0';
return(1);
}
