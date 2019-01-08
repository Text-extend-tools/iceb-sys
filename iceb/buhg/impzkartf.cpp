/*$Id: impzkartf.c,v 5.4 2014/07/31 07:09:17 sasa Exp $*/
/*13.02.2016	14.06.2013	Белых А.И.	impzkartf.c
Загрузка списка работников в подсистему "Расчёт зарплаты
Импорт всех данных в карточке работника
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"


void impzkartf(void)
{
struct stat work;
char		strsql[4096];
SQL_str		row;
SQLCURSOR curr;
static class iceb_tu_str imafim("");
static char imafprom[64];
int		kom=0;
int		razmer=0;
class iceb_tu_str strok("");

VVOD VVOD1(2);
clear();
VVOD1.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт карточек из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("zarp2_10_1.txt");
    clear();
    goto naz;

  case ENTER:
  case FK2:
  case PLU:
    if(imafim.ravno()[0] == '\0')
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  default:
    goto naz;
 }


clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

class iceb_fopen filz;
if(filz.start(imafim.ravno(),"r") != 0)
 goto naz;

sprintf(imafprom,"prom%d.tmp",getpid());

class iceb_fopen filprom;
if(filprom.start(imafprom,"w") != 0)
 goto naz;

#define KOL 26
class iceb_tu_str rek[KOL];
OPSHET reksh;
int  kolkart=0;
razmer=0;
int nom=0;
while(iceb_tu_fgets(&strok,filz.ff) == 0)
 {
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);


  if(polen(strok.ravno(),&rek[0],1,'|') != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s %d\n",gettext("Не найдено поле"),1);
    continue;
   }

  for(nom=1; nom < KOL; nom++)
   if(polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filprom.ff,"%s",strok.ravno());
     fprintf(filprom.ff,"#%s %d !\n",gettext("Не найдено поле"),nom+1);
     break;
    }

  if(nom < KOL)
   continue;


  /*Проверяем на отсутствие индентификационного кода*/
  if(rek[10].getdlinna() > 1)
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",rek[10].ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) > 0)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(rek[10].ravno());
      SOOB.VVOD_SPISOK_add_MD(gettext("Такой индентификационный код уже введён !"));
      sprintf(strsql,"%s %s",row[0],row[1]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      fprintf(filprom.ff,"%s\n#%s:%s\n",strok.ravno(),gettext("Такой индентификационный код уже введён !"),
      strsql);
      continue;
     } 
   }

  /*если подразделения нет в списке подразделений то обнуляем*/
  sprintf(strsql,"select kod from Podr where kod=%d",rek[4].ravno_atoi());
  if(readkey(strsql) == 0)
   rek[4].new_plus("0");

  /*если категории нет в списке категорий то обнуляем*/
  sprintf(strsql,"select kod from Kateg where kod=%d",rek[5].ravno_atoi());
  if(readkey(strsql) == 0)
   rek[5].new_plus("0");
   
  /*проверка счёта*/
   if(prsh1(rek[8].ravno(),&reksh) != 0)
    rek[8].new_plus("");
      
  if(rek[0].ravno_atoi() == 0)
   rek[0].new_plus(tabnom());

  /*проверяем есть ли такой табельный номер*/
  sprintf(strsql,"select tabn from Kartb where tabn=%d",rek[0].ravno_atoi());
  if(readkey(strsql) > 0)
   rek[0].new_plus(tabnom());
     
/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss код основания для учёта специального стажа
*/
      
  sprintf(strsql,"insert into Kartb (tabn,fio,dolg,zvan,podr,kateg,datn,datk,shet,sovm,inn,adres,nomp,vidan,telef,kodg,harac,lgoti,bankshet,datvd,tipkk,denrog,pl,dppz,kb,kss) \
values (%s,'%s','%s','%s',%s,%s,'%s','%s','%s',%s,'%s','%s','%s','%s','%s',%s,'%s','%s','%s','%s','%s','%s',%s,'%s',%s,'%s')",
  rek[0].ravno(),
  rek[1].ravno_filtr(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno(),
  rek[5].ravno(),
  rek[6].ravno(),
  rek[7].ravno(),
  rek[8].ravno_filtr(),
  rek[9].ravno(),
  rek[10].ravno_filtr(),
  rek[11].ravno_filtr(),
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr(),
  rek[14].ravno_filtr(),
  rek[15].ravno(),
  rek[16].ravno_filtr(),
  rek[17].ravno_filtr(),
  rek[18].ravno(),
  rek[19].ravno(),
  rek[20].ravno_filtr(),
  rek[21].ravno(),
  rek[22].ravno(),
  rek[23].ravno(),
  rek[24].ravno(),
  rek[25].ravno_filtr());
  
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    fprintf(filprom.ff,"%s",strok.ravno());
    continue;
   }    
  kolkart++;  
 }
strzag(LINES-1,0,work.st_size,work.st_size);

filz.end();
filprom.end();
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());

VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD(gettext("Загрузка завершена"));
sprintf(strsql,"%s:%d",gettext("Количество загруженых карточек"),kolkart);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);
}
