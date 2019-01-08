/*$Id: dirmak.c,v 5.11 2013-04-07 10:22:54 sasa Exp $*/
/*23.10.2009    16.12.1992      Белых А.И.      dirmak.c
Подпрограмма прсмотра make файла и составления списка
подпрограмм входящих в проект
*/
#include        <sys/stat.h>
#include        <errno.h>
#include        "buhl.h"

char  		*editor;

int dirmak(char *name,char *proekt) //Имя make файла
{
char    	*mfa=NULL;
struct  tm      *bf;
struct stat work;
char            st[1024];
FILE            *f;
char            imaf[112];
int             dl=0,dl1=0;
int             kk,ks,ns;
int             nf;
int             voz;
int             i1,i2;
int		kolp;
int		i3;
int		ii;
int		kolfil;
int		K;
int		metkapro=0;
char		bros[1024];
class iceb_t_mstr spis_menu(0);
class iceb_tu_str prog("");
class iceb_tu_str do_ravno("");
voz=0;

if((f=fopen("Makefile.am","r")) == NULL)
 {
  error_op_nfil("Makefile.am",errno,"");
  return(1);
 }

kolfil=0;
while(fgets(st,sizeof(st),f) != NULL )
 {
//  printw("st-%s",st);
//  refresh();
  if(st[0] == '\n')
   continue;
  if(st[0] == '#')
   continue;
  if(st[0] == '\t')
   st[0]=' ';

  kolp=pole2(st,' ');  
  if(kolp == 0)
    continue;

  if(POLE(st,bros,1,'=') == 0)
   {
    polen(st,&do_ravno,1,'=');
    int kolpol=pole2(do_ravno.ravno(),'_');
    for(int nompol=0; nompol < kolpol-1; nompol++)
     {
      polen(do_ravno.ravno(),bros,sizeof(bros),nompol+1,'_');
      if(nompol == 0)
        prog.new_plus(bros);
      else
       {
        prog.plus("_",bros);
       }
     }
    
    if(SRAV(proekt,prog.ravno(),0) == 0)
        metkapro=1;
    else
        metkapro=0;
   }
  if(metkapro == 1)
  for(i2=1; i2 <= kolp; i2++)
   {

    POLE(st,imaf,i2,' ');

    if(imaf[0] == '\0')
     continue;

    i1=strlen(imaf);
       
    if(imaf[i1-1] == '\\')
     {
      imaf[i1-1]='\0';
      i1--;   
     }          

    if(imaf[i1-1] == 'c' && imaf[i1-2] == '.')
     {
      /*Проверяем есть ли файл*/
      if(stat(imaf,&work) != 0)
       {
          continue;
       }
     }
    else        
     continue;
/*****
    printw("imaf=%s\n",imaf);
    OSTANOV();
*******/
    if(kolfil == 0)
     {
      dl=strlen(imaf)+1;
      if((mfa=(char *)malloc(dl)) == NULL)
       {
        printw("Не могу выделить память !!!\n");
        refresh();
        endwin();
        exit(1);
       }
      strcpy(mfa,imaf);
      kolfil=1;
     }
    else
     {
      /*Проверяем может этот файл уже есть в списке*/
      if(pole1(mfa,imaf,' ',0,&i3) == 0)
       continue;
      dl1=strlen(imaf)+1;
      dl=dl+dl1;
      if((mfa=(char *)realloc(mfa,dl)) == NULL)
       {
        printw("*Не могу выделить память !!!\n");
        refresh();
        endwin();
        exit(1);
       }
      strcat(mfa," ");
      strcat(mfa,imaf);
      kolfil++;
     }
/*
    printw("mfa=%s\n",mfa);
    refresh;
*/
     
   }
 }
fclose(f);

//printw("\nmfa=%s\n",mfa);
//refresh();
nf=1;
kk=0;

naz:;

//Создаем класс меню
ks=LINES-5;


if(kolfil > 0 && nf > kolfil)
 nf=kolfil;
if(nf < 1)
 nf=1; 

spis_menu.spis_str.free_class();
ns=0;

for(ii=nf ;POLE(mfa,name,ii,' ') == 0 && ns<ks ; ii++)
 {
  if(stat(name,&work) != 0)
      continue;

  bf=localtime(&work.st_mtime);
  sprintf(bros,"%-*.*s %6ld %02d:%02d %02d.%02d.%d",
  iceb_tu_kolbait(20,name),iceb_tu_kolbait(20,name),name,
  work.st_size,bf->tm_hour,
  bf->tm_min,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  
  ns++;
  spis_menu.spis_str.plus(bros);


 }

if(kolfil == 1)  /*В массиве одно имя*/
 {
  if(stat(mfa,&work) != 0)
   {
    goto fff;
   }

  bf=localtime(&work.st_mtime);
  sprintf(bros,"%-*.*s %6ld %02d:%02d %02d.%02d.%d",
  iceb_tu_kolbait(20,mfa),iceb_tu_kolbait(20,mfa),mfa,
  work.st_size,bf->tm_hour,
  bf->tm_min,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

  ns++;
  spis_menu.spis_str.plus(bros);


 }

fff:;

if(kk >= ns)
  kk=ns-1;

if(kk < 0)
 kk=0;

if(ns < ks)
  spis_menu.spis_str.plus(gettext("Конец списка"));



uuu:;

helstr(LINES-1,0,"F1",gettext("редактирование"),
"P.Up",gettext("лист вверх"),
"P.Down",gettext("лист вниз"),
"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);

i1=nf*100/kolfil;
memset(bros,'\0',sizeof(bros));
kk=spis_menu.menu(kk,2,30,NULL,bros,i1,&K,"",0);


if(kk == -7) //Изменение размеров экрана
 {
  goto naz;
 }

if(K == FK1)
 {
//  POLE(MM5.MD[kk],name,1,' ');
  POLE(mfa,name,nf+kk,' ');
  sprintf(st,"%s %s",editor,name);
  clear();
  refresh();
  runsystem(st);
  clear();
  refresh();
  goto uuu;
 }

if(kk == -5)  /*Экран вниз*/
 {
  nf+=ks;
  if(nf > kolfil)
   {
    beep();
    nf=kolfil;
   }
  goto naz;
 }
if(kk == -3 )  /*Стрелка вниз*/
 {
  nf++;
  if(nf > kolfil)
   {
    beep();
    nf=kolfil;
   }
  kk=ks;
  goto naz;
 }
if(kk == -2 ) /*Стрелка вверх*/
 {

//  nf=nf-ks-ks;
  nf--;
  if(nf <1)
    nf=1;

  if(nf == 1)
   beep();
  goto naz;
 }

if(kk == -4) /*Экран в верх*/
 {
  nf-=ks;
  if(nf < 1)
   {
    beep();
    nf=1;
   }
  goto naz;
 } 

if(kk == -1)
  voz=1;
else
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kk),0) == 0)
     goto naz;
  if(kolfil > 1)
   POLE(mfa,name,nf+kk,' ');
  else
   strcpy(name,mfa);
 }
//printw("nf=%d kk=%d mfa=%s\n",nf,kk,mfa);
//OSTANOV();

if(mfa != NULL)
  free(mfa);

return(voz);
}
/*********************************/
/*********************************/
int	dirmak1(char *imaf,char *imafv,int Y,int X)
{
FILE	*ff;
char	strok[1024];
char		*masiv=NULL;
class iceb_tu_str bros("");
class iceb_tu_str do_ravno("");
int		kolfil=0;
int		K=0;
class iceb_tu_str prog("");
int kolpol=0;
if((ff=fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
class iceb_t_mstr spis_menu(0);

while(fgets(strok,sizeof(strok),ff) != NULL )
 {
  if(strstr(strok,"_SOURCES") == NULL)
    continue;
//  printw("%s",strok);
//  OSTANOV();
  polen(strok,&do_ravno,1,'=');
  prog.new_plus("");
  kolpol=pole2(do_ravno.ravno(),'_');
  for(int nompol=0; nompol < kolpol-1; nompol++)
   {
    polen(do_ravno.ravno(),&bros,nompol+1,'_');
    if(nompol == 0)
      prog.new_plus(bros.ravno());
    else
     {
      prog.plus("_",bros.ravno());
     }
   }
  spis_menu.spis_str.plus(prog.ravno());
  kolfil++;    
 }
fclose(ff);

uuu:;

memset(strok,'\0',sizeof(strok));
int nomer=spis_menu.menu(0,Y,X,NULL,strok,100,&K,"Выберите проэкт",0);


if(nomer == -7) //Изменение размеров экрана
 {
  goto uuu;
 }  

if(nomer == -3) /*Стрелка вниз*/
 {
  goto uuu;
 }
if(nomer == -2) /*Стрелка вверх*/
 {
  goto uuu;
 }
if(nomer == -5) /*Страница вниз*/
  goto uuu;
if(nomer == -4) /*Страница вверх*/
  goto uuu;
int voz=0;
if(K == ENTER)
 {
  strcpy(imafv,spis_menu.spis_str.ravno(nomer));
 }
else
 voz=1;
free(masiv);
return(voz);
}
