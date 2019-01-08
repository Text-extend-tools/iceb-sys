/* $Id: zagf8dr1.c,v 5.28 2013/08/13 05:50:01 sasa Exp $ */
/*11.01.2016	12.07.2000	Белых А.И. 	zagf8dr1.c
Загрузка dbf файла с формой 8ДР в архив
Измененная форма. Наказ ДПА від 08.04.2003 N 164
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"

extern short	mmm;

int zagf8dr1(const char *nomd, //Номер документа*
int kolst, //Количество строк уже введенных в документ*/
int kvrt) //Квартал
{
SQL_str         row;
char		strsql[1024];
char		str[1024];
char		bros[1024];
class iceb_tu_str imadbf("");
FILE		*ff;
int		i,i1;
char		inn[16];
double		sumad,suman;
double		sumadn,sumann;
short		d1,m1,g1,d2,m2,g2;
char		priz[4],lgota[4];
time_t		vrem;
class iceb_tu_str fio("");
SQLCURSOR curr;
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Загрузка данных из dbf файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя dbf файла"));

naz:;

clear();
printw(gettext("Загрузка данных из dbf файла"));

helstr(LINES-1,0," F10 ",gettext("выход"),NULL);



sprintf(strsql,"da0000%02d.%d",1,kvrt);
imadbf.new_plus(strsql);

if(iceb_t_poldan("Имя DBF файла",str,"zarsdf1df.alx") == 0)
 {
  sprintf(strsql,"%s%02d.%d",str,1,kvrt);
  
  imadbf.new_plus(strsql);
 }
i=vvod1(&imadbf,32,&VVOD1,NULL,stdscr,-1,-1);

clearstr(LINES-1,0);
if(i == FK10 || i == ESC)
 return(1);
 
if(imadbf.ravno()[0] == '\0')
  return(1);


struct stat work;

if(stat(imadbf.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s: %s !",gettext("Не найден файл"),imadbf.ravno());
  iceb_t_soob(strsql);
  return(1);
 }

if((ff = fopen(imadbf.ravno(),"r")) == NULL)
 {
 if(errno == ENOENT)
  {
   sprintf(strsql,"%s: %s !",gettext("Не найден файл"),imadbf.ravno());
   iceb_t_soob(strsql);
   goto naz;   
  }
 else
  {
   sprintf(strsql,"%s %s: %d !",gettext("Ошибка открытия файла"),imadbf.ravno(),errno);
   iceb_t_soob(strsql);
   goto naz;
  }
 }

if(kolst != 0)
 {
  VVOD DANET(1);
  DANET.VVOD_SPISOK_add_MD(gettext("Удалить уже введенные данные ?"));
  if(danet(&DANET,2,stdscr) == 1)
   {
    sprintf(strsql,"delete from F8dr1 where nomd='%s'",nomd);
    if(sql_zapis(strsql,0,0) == 0)
     kolst=0;    
   }
 }

time(&vrem);

int razmer=513;

fseek(ff,513L,SEEK_SET);
char vidz[2];

while(fgets(str,103,ff) != NULL)
 {
  printw("%s*\n",str);
  razmer+=103;
  strzag(LINES-1,0,work.st_size,razmer);

  if(str[1] == '9' && str[2] == '9' && str[3] == '9' && str[4] == '9')
    break;

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=6; i < 7; i++)
    bros[i1++]=str[i];
  kvrt=atoi(bros);
 // printw("bros=%s\n",bros);    

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=7; i < 11; i++)
    bros[i1++]=str[i];


  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=21; i < 22; i++)
    bros[i1++]=str[i];

  memset(inn,'\0',sizeof(inn));
  i1=0;
  for(i=22; i < 32; i++)
    inn[i1++]=str[i];
//  printw("inn=%s*\n",inn);
  fio.new_plus("");

  /*Проверяем наличие в базе индентификационного кода*/
  //У частных предпринимателей нет карточки
  sprintf(strsql,"select inn from Kartb where inn='%s'",inn);
  if(sql_readkey(&bd,strsql,&row,&curr) < 1)
   {
    sprintf(strsql,"select naikon from Kontragent where kod='%s'",inn);
    if(sql_readkey(&bd,strsql,&row,&curr) < 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,"%s %s !",
      gettext("Не найден индентификационный код"),inn);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("Запись не загружена !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;
     }
    fio.new_plus(row[0]);
   }

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=32; i < 44; i++)
    bros[i1++]=str[i];
  sumadn=atof(bros);

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=44; i < 56; i++)
    bros[i1++]=str[i];
  sumad=atof(bros);
//  printw("sumad=%s*\n",bros);

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=56; i < 68; i++)
    bros[i1++]=str[i];
  sumann=atof(bros);
  sumann=okrug(sumann,0.01);
  
  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=68; i < 80; i++)
    bros[i1++]=str[i];
  suman=atof(bros);
  
  
//  printw("suman=%s*\n",bros);
  
  memset(priz,'\0',sizeof(priz));
  i1=0;
  for(i=80; i < 83; i++)
    priz[i1++]=str[i];

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=83; i < 87; i++)
    bros[i1++]=str[i];
  d1=m1=g1=0;
  g1=atoi(bros);
  if(g1 != 0)
   {
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=87; i < 89; i++)
      bros[i1++]=str[i];
    m1=atoi(bros);
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=89; i < 91; i++)
      bros[i1++]=str[i];
    d1=atoi(bros);
   }
  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=91; i < 95; i++)
    bros[i1++]=str[i];
  d2=m2=g2=0;
  g2=atoi(bros);
  if(g2 != 0)
   {
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=95; i < 97; i++)
      bros[i1++]=str[i];
    m2=atoi(bros);
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=97; i < 99; i++)
      bros[i1++]=str[i];
    d2=atoi(bros);
   }
  memset(lgota,'\0',sizeof(lgota));
  i1=0;
  for(i=99; i < 101; i++)
    lgota[i1++]=str[i];
  if(atoi(lgota) == 0)
    memset(lgota,'\0',sizeof(lgota));

  memset(vidz,'\0',sizeof(vidz));
  vidz[0]=str[101];    
/*
  printw("KOD=%s kvrt=%d god=%d tip=%d inn=%s %.2f %.2f %d.%d.%d %d.%d.%d lgota=%s\n",
  KOD,kvrt,god,tip,inn,sumad,suman,d1,m1,g1,d2,m2,g2,lgota);
  OSTANOV();
*/

  /*Делаем запись в базу*/
  sprintf(strsql,"insert into F8dr1 \
values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%s)",
  nomd,inn,sumad,suman,priz,g1,m1,d1,g2,m2,d2,lgota,iceb_t_getuid(),vrem,fio.ravno_filtr(),sumadn,sumann,vidz);

zp:;

  if(sql_zap(&bd,strsql) != 0)
   {
    if(kolst != 0 && sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
     {
      printw("Повторяющаяся запись. Увеличивем значения в уже введенной !\n");
      sprintf(strsql,"update F8dr1 set sumad=sumad+%.2f, suman=suman+%.2f,\
sumadn=sumadn+%.2f, sumann=sumann+%.2f where nomd='%s' and inn='%s' and priz='%s' and lgota='%s'",
      sumad,suman,sumadn,sumann,nomd,inn,priz,lgota);
      goto zp;
     }
    msql_error(&bd,"Ошибка ввода записи !",strsql);
   }
 }

fclose(ff);

//OSTANOV();

sprintf(strsql,"%s !",gettext("Загрузка завершена"));
iceb_t_soob(strsql);

return(0);
}
