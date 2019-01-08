/*$Id: spisok.c,v 5.17 2013/09/26 09:44:12 sasa Exp $*/
/*02.03.2015	06.06.2002	Белых А.И.	spisok.c
Работа со списками
*/
//#include <scr.h>
#include <glib.h>
#include        <ctype.h>
#include        "util.h"
STROKA::STROKA(const char *masiv,int dlina,int metka)
{
kolbait=dlina+1;
ukstr=new char[dlina+1];
memset(ukstr,'\0',dlina+1);
if(metka == 0 && masiv[0] != '\0')
    strncpy(ukstr,masiv,dlina);  
str_filtr=NULL;

}

STROKA::~STROKA()
 {
  delete [] ukstr;
  if(str_filtr != NULL)
     delete [] str_filtr;
 }


/***********************/
/*Создать список*/
/***********************/
void SPISOK::SPISOK_soz_spisok(int mr)
{
 metka=mr;
 //Пустой "технический узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
}

/***********************/
/*Создать список бeз параметров*/
/***********************/
void SPISOK::SPISOK_soz_spisok()
{
 metka=0;
 //Пустой "технический узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
}

/*******************************/
/*Конструктор*/
/*обязательно полное повторение кода!!! иначе валится*/
/*******************************/
SPISOK::SPISOK(int mr)
{
 metka=mr;
 //Пустой "технический узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
}  

/*******************************/
/*Конструктор без параметров*/
/*обязательно полное повторение кода!!! иначе валится*/
/*******************************/
SPISOK::SPISOK()
{
 metka=0;
 //Пустой "технический узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
}  

/*************************/
/*Деструктор*/
/*************************/
SPISOK::~SPISOK()
{
STROKA *udal=zag;
for(STROKA *sn = zag; sn != kon;)
 {
  sn=sn->next;
  delete udal;
  udal=sn;
 }
delete udal;

zag=NULL;
kon=NULL;
//SPISOK_delete();
}    

/*********************************/
/*Проверить дату*/
/*********************************/
int SPISOK::prov_dat(int nomer,int metka) /*0-если пусто - неправильно 1-если пусто-правильно*/
{
short d,m,g;

if(ravno(nomer)[0] == '\0')
 {
  if(metka == 0)
    return(1);
  if(metka == 1)
    return(0);
 }

if(rsdat(&d,&m,&g,ravno(nomer),1) != 0)
 return(1);
return(0);
}
/*********************************/
/*Проверить дату*/
/*********************************/
int SPISOK::prov_dat1(int nomer,int metka) /*0-если пусто - неправильно 1-если пусто-правильно*/
{
short m,g;

if(ravno(nomer)[0] == '\0')
 {
  if(metka == 0)
    return(1);
  if(metka == 1)
    return(0);
 }

if(rsdat1(&m,&g,ravno(nomer)) != 0)
 return(1);
return(0);
}
/*********************************/
//Удалить спиок
/*********************************/
void SPISOK::SPISOK_delete()
{
STROKA *udal=zag;
for(STROKA *sn = zag; sn != kon;)
 {
  sn=sn->next;
  delete udal;
  udal=sn;
 }
delete udal;

zag=NULL;
kon=NULL;
}    

/*************************/
/*Создать навый список*/
/***********************/
void SPISOK::free_class(void)
{
STROKA *udal=zag;
for(STROKA *sn = zag; sn != kon;)
 {
  sn=sn->next;
  delete udal;
  udal=sn;
 }
delete udal;

zag=NULL;
kon=NULL;
//SPISOK_delete();
//SPISOK_soz_spisok(0);

 //Пустой "технический узел"
 zag=kon=new STROKA("12345678",7,0);
 kon->next=NULL;
}  


/*********************************/
/*Добавить элемент в список */
/*обязательно полное повторение кода!!! иначе валится*/
/*******************************/
int SPISOK::plus(const char *masiv,int dlina)
 {
  STROKA *nov=new STROKA(masiv,dlina,0);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);
 }  
/*********************************/
/*Добавить элемент в список*/
/*обязательно полное повторение кода!!! иначе валится*/
/*********************************/
int SPISOK::plus(const char *masiv)
 {

  STROKA *nov=new STROKA(masiv,strlen(masiv)+1,0);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);

 }

/*********************************/
/*Добавить элемент в список удалив перевод строки если он есть*/
/*обязательно полное повторение кода!!! иначе валится*/
/*********************************/
int SPISOK::plus_bps(const char *masiv)
 {
  char stroka[strlen(masiv)+1];
  strcpy(stroka,masiv);
  ud_simv(stroka,"\n");
  STROKA *nov=new STROKA(stroka,strlen(stroka)+1,0);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);

 }

/****************************/
/*Добавить элемент в список*/
/*обязательно полное повторение кода!!! иначе валится*/
/****************************/
int SPISOK::plus(int dlina)
 {
  char mmm[8];
  memset(mmm,'\0',8);

  STROKA *nov=new STROKA(mmm,dlina,1);
  if(metka == 0)
   {
    kon->next=nov;      
    nov->next=NULL;
    kon=nov;
   }
  if(metka == 1)
   {
    nov->next=zag;
    zag=nov;
   }
  return(0);
 }
/**********************************/
/*Вернуть преобразованным в целый тип*/
/***********************************/
int SPISOK::ravno_atoi(int nom)
{
 return(atoi(ravno(nom)));
}
/**********************************/
/*Вернуть преобразованным в вещественный тип*/
/***********************************/
double SPISOK::ravno_atof(int nom)
{
 return(atof(ravno(nom)));
}
/**********************************/
/*Вернуть адрес элемента из списка*/
/***********************************/
const char *SPISOK::ravno(int nom)
 {
 //     printw("\nSPISOK_return номер=%d\n",nom);
//     refresh();
  int nomer=0;
  STROKA *sn=NULL; 

  if(metka == 0)
    sn=zag->next;
  if(metka == 1)
    sn=zag;

  if(sn == NULL)
   {
    return(NULL);
   }

  for( ; ; )
   {
    if(nomer++ == nom)
     {
      return(sn->ukstr);
     }      

    if(sn == kon)
      break;

    sn=sn->next;
   }
  return(NULL);
 }

/**********************************/
//Вернуть адрес элемента из списка отфильтрованный для SQL запроса
/***********************************/
const char *SPISOK::ravno_filtr(int nom)
{
class STROKA *ss=(STROKA*)return_all(nom);
if(ss->str_filtr != NULL)
 delete [] ss->str_filtr;
int razmer=strlen(ss->ukstr)+1;
ss->str_filtr=new char[razmer*2];

int i1=0;
for(int i=0; i < razmer; i++)
 {
  switch (ss->ukstr[i])
   {
    case '\'':
      ss->str_filtr[i1++]='\'';
      ss->str_filtr[i1++]=ss->ukstr[i];
      break;
    case '\\':
      ss->str_filtr[i1++]='\\';
      ss->str_filtr[i1++]=ss->ukstr[i];
      break;

    default:
      ss->str_filtr[i1++]=ss->ukstr[i];
      break;      
   }
 }
ss->str_filtr[i1]='\0';

return(ss->str_filtr);

}
 
/****************************/
 //Вернуть количество элементов в списке
/***************************/
int SPISOK::kolih(void)
 {
  int kol=0;
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
    kol++;
  return(kol);
 }

/*******************************/
//Найти в списке нужную строку с учетом регистра
/*****************************/
//Если вернули -1 значит не нашли
int SPISOK::find(const char *obr,int metka_srav)
 {
  int i;
  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
//    printw("%s\n",sn1->ukstr);
    if(metka_srav == 1) //Сравнение до нулевого байта в любой строке
     {
      int metka=0;
      for(i=0; sn1->ukstr[i] != '\0' && obr[i] != '\0'; i++)
        if(sn1->ukstr[i] != obr[i])
         {
          metka=1;
          break;
         }
      if(metka == 0)
        return(nomer);
     }    

    if(metka_srav == 0) //Полное сравнение
    if(dlina == (int)strlen(sn1->ukstr))
     {
      for(i=0; i < dlina ; i++)
       {
        if(sn1->ukstr[i] != obr[i])
           break;
       }
      if(i == dlina)
       return(nomer);
     }
    nomer++;
   }

  return(-1);
 }
/*******************************/
//Найти в списке нужную строку с полным сравнением*/
/*****************************/
//Если вернули -1 значит не нашли
int SPISOK::find(const char *obr)
{
 return(find(obr,0));
}

/*******************************/
//Найти в списке нужную строку без учета регистра
/*****************************/
//Если вернули -1 значит не нашли
int SPISOK::find_r(const char *obr)
 {
  int i;

  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
    int dlina0=strlen(sn1->ukstr);
    
    if(dlina == dlina0)
     {
      char obr1[dlina+1];
      char obr2[dlina0+1];
      strcpy(obr2,sn1->ukstr);
      strcpy(obr1,obr);

      iceb_tu_toupper_str(obr2);
      iceb_tu_toupper_str(obr1);
      
      for(i=0; i < dlina ; i++)
       if(obr2[i] != obr1[i])
        break;

      if(i == dlina)
       return(nomer);
     }
    nomer++;
   }
  return(-1);
 }
/*******************************/
//Найти в списке нужную строку без учета регистра
/*****************************/
//Если вернули -1 значит не нашли
int SPISOK::find_r(const char *obr,int metka)
 {

  int dlina=strlen(obr);
  int nomer=0;
  STROKA *sn1;       
  
  for(STROKA *sn = zag; sn != kon;sn=sn->next)
   {
    sn1=sn->next;
    int dlina0=strlen(sn1->ukstr);
    
    if(dlina == dlina0)
     {
      char obr1[dlina+1];
      char obr2[dlina0+1];
      strcpy(obr2,sn1->ukstr);
      strcpy(obr1,obr);

      iceb_tu_toupper_str(obr2);
      iceb_tu_toupper_str(obr1);

      if(SRAV(obr2,obr1,metka) == 0)
       return(nomer);
     }
    nomer++;
   }
  return(-1);
 }

/*****************************/
/*Записать данные в уже существующий елемент списка с удалением его предыдущего содержимого*/
/*************************************/
/*Если вернули 0-записали 1-нет*/
int SPISOK::new_plus(int nomer,const char *znah)
{
if(nomer >= kolih())
 return(1);


class STROKA *ss=(STROKA*)return_all(nomer);

delete [] ss->ukstr;
int razmer=strlen(znah)+1;
ss->kolbait=razmer;
ss->ukstr=new char[razmer];
memset(ss->ukstr,'\0',razmer);

if(znah[0] != '\0') //Только так
  strcpy(ss->ukstr,znah);  

return(0);
}

/*****************************/
/*Добавить к существующей строке c перераспределением памяти*/
/*************************************/
/*Если вернули 0-записали 1-нет*/
int SPISOK::plus_strcat(int nomer,const char *znah)
{
if(nomer >= kolih())
 return(1);

class STROKA *ss=(STROKA*)return_all(nomer);

//char *stroka_sp=ravno(nomer);

int dlina=strlen(znah)+ss->kolbait+1;

if((ss->ukstr=(char *)realloc(ss->ukstr,sizeof(char)*dlina)) == NULL)
 {
  printf("\n%s-Не могу перераспределить память !!!\n",__FUNCTION__);
  return(-1);
 }  
ss->kolbait=dlina;
strcat(ss->ukstr,znah);
return(0);
}
/*************************************/
/*Записать вместо существующих данных*/
/**************************************/
void SPISOK::plus(int nomer,const char *znah)
{
if(nomer >= kolih())
 return;

class STROKA *ss=(STROKA*)return_all(nomer);

strncpy(ss->ukstr,znah,ss->kolbait);
}

/***************************/
/*Удалить елемент из списка*/
/***************************/

int SPISOK::del(int nomer)
{
/*Необходимо помнить что имеется первая техническая запись*/
/*поэтому номер удаляемой строки будет на единицу больше*/
//printw("\n%s-nomer=%d\n",__FUNCTION__,nomer);

class STROKA *pred_zap=NULL;
int nom=0;
class STROKA *sn;
for(sn = zag; sn != kon; sn=sn->next)
 {
//  printw("%s\n",sn->ukstr);
  if(nom < nomer+1) /*Запоминаем предыдущюю запись*/
   {
    pred_zap=sn;
   }
  if(nom == nomer+1) /*Удаляем запись*/
   {
    /*переключаем ссылку на следующюю запись*/
    pred_zap->next=sn->next;

    delete sn;

    return(0);
   }

  nom++;
 }
/*Нужно удалить последнюю запись в списке а это она и есть*/
if(kolih() == nomer+1)
 {
  delete sn;
  sn=NULL;
  kon=pred_zap;
 }
//OSTANOV();
return(1);
}

/******************/
/*Вернуть адрес записи всего класса*/
/**************************/
void *SPISOK::return_all(int nom)
{
  int nomer=0;
  STROKA *sn=NULL; 

  if(metka == 0)
    sn=zag->next;
  if(metka == 1)
    sn=zag;

  if(sn == NULL)
   {
    return(NULL);
   }

  for( ; ; )
   {
    if(nomer++ == nom)
     {
      return(sn);
     }      

    if(sn == kon)
      break;

    sn=sn->next;
   }
  return(NULL);
}

/****************************/
/*Вернуть дату в SQL-формате*/
/****************************/
const char *SPISOK::ravno_sqldat(int nom)
{
class STROKA *ss=(STROKA*)return_all(nom);
if(ss->str_filtr != NULL)
 delete [] ss->str_filtr;
int razmer=64;
ss->str_filtr=new char[razmer];
/***********
short d=0,m=0,g=0;
rsdat(&d,&m,&g,ravno(nom),1);
sprintf(ss->str_filtr,"%04d-%02d-%02d",g,m,d);
************/
strncpy(ss->str_filtr,iceb_tu_tosqldat(ravno(nom)),razmer);

return(ss->str_filtr);
}
/**************/
/*Вернуть год*/
/**************/
short SPISOK::ravno_god(int nom)
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,ravno(nom),1);
return(g);
}
/*******************************/
/********************************/
int SPISOK::cat(int nom,const char *str)
{
if(nom >= kolih())
 return(1);

class STROKA *ss=(STROKA*)return_all(nom);
if(ss->kolbait < (int)(strlen(ss->ukstr)+strlen(str)+1))
  return(1);
strcat(ss->ukstr,str);
return(0);
}
/*******************************/
/********************************/
int SPISOK::copy(int nom,const char *str)
{
if(nom >= kolih())
 return(1);

class STROKA *ss=(STROKA*)return_all(nom);
memset(ss->ukstr,'\0',ss->kolbait);
strncpy(ss->ukstr,str,ss->kolbait-1);
return(0);
}

/*******************************/
/********************************/
int SPISOK::kolbait(int nom)
{
if(nom >= kolih())
 return(-1);

class STROKA *ss=(STROKA*)return_all(nom);
return(ss->kolbait);
}

