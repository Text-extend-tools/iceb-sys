/* $Id: vvodf8dr.c,v 5.21 2013/09/26 09:43:44 sasa Exp $ */
/*21.04.2015	11.07.2000	Белых А.И.	vvodf8dr.c
Работа с шапкой формы 8ДР
Если вернули 0 - сделали запись
	     1 - нет
*/
#include	"buhg.h"

int		zapf8dr(VVOD*,const char *nomds,int);
int		nomdf8dr(void);


int vvodf8dr(class iceb_tu_str *nomd,
int vidd)  //0- звітний 1-новий звітний 2-уточнюючий
{
SQL_str         row;
class SQLCURSOR cur;
char		strsql[1024];
short           K;
int             N;
class iceb_tu_str nomds(nomd->ravno());

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(5);//0
VV.VVOD_SPISOK_add_data(2);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(11);//3
VV.VVOD_SPISOK_add_data(30);//4
VV.VVOD_SPISOK_add_data(20);//5
VV.VVOD_SPISOK_add_data(11);//6
VV.VVOD_SPISOK_add_data(10);//7
VV.VVOD_SPISOK_add_data(10);//8

if(nomd->ravno()[0] != '\0')
 {
  sprintf(strsql,"select * from F8dr where nomd='%s'",nomd->ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }  
  VV.data_plus(0,row[0]);
  VV.data_plus(1,row[1]);
  VV.data_plus(2,row[2]);
  if(row[5][0] != '0')
   VV.data_plus(3,iceb_tu_datzap(row[5]));
  VV.data_plus(4,row[3]);
  VV.data_plus(5,row[4]);
  if(row[6][0] != '0')
   VV.data_plus(6,iceb_tu_datzap(row[6]));
  kzvz(row[8],row[9],&VV);
  VV.data_plus(7,row[10]);
  VV.data_plus(8,row[11]);

 }
else
 {
  sprintf(strsql,"%d",nomdf8dr());
  VV.data_plus(2,strsql);
 }
SQLCURSOR curr;
VV.VVOD_SPISOK_add_MD(gettext("Год....................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Квартал................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата документа.........."));//3
VV.VVOD_SPISOK_add_MD(gettext("Вид документа..........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Входной номер ГНА......."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата регистрации в ГНА.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Количество работников..."));//7
VV.VVOD_SPISOK_add_MD(gettext("Количество совм-телей..."));//8

if(vidd == 0)
 VV.VVOD_SPISOK_zapis_data(4,gettext("Отчетный"));
if(vidd == 1)
 VV.VVOD_SPISOK_zapis_data(4,gettext("Новый отчётный"));
if(vidd == 2)
 VV.VVOD_SPISOK_zapis_data(4,gettext("Уточняющий"));
 
K=N=0;

nazad:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("номер докум."),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {

  refresh();

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 4)
     {
      vidd++;
      if(vidd == 3)
       vidd=0;
      if(vidd == 0)
        VV.VVOD_SPISOK_zapis_data(4,gettext("Отчетный"));
      if(vidd == 1)
        VV.VVOD_SPISOK_zapis_data(4,gettext("Новый отчётный"));
      if(vidd == 2)
        VV.VVOD_SPISOK_zapis_data(4,gettext("Уточняющий"));
      goto nazad;
     }

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(N == 2)
     {
      /*Проверяем номер документа*/
      sprintf(strsql,"select nomd from F8dr where nomd='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
       {
        sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(2));
        iceb_t_soob(strsql);
        goto nazad;
       }
       
     }


    VV.vprintw(N,stdscr);
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
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  
  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK1)
   {
    GDITE();
    iceb_t_pdoc("zarp4_7_1_2_1.txt");
    clear();
    goto nazad;
   }

  if(K == PLU || K == FK2) /*Продолжать*/
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       attroff(VV.VVOD_return_cs(iceb_CFM));
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto nazad;
      }
    break;
   }
  if(K == FK4) /*Номер документа*/
   {
    VV.data_plus(2,nomdf8dr());
    goto nazad;
   }   
 }



if(zapf8dr(&VV,nomds.ravno(),vidd) != 0)
  goto nazad;

nomd->new_plus(VV.data_ravno(2));

return(0);
}

/**************************************/
/*   Запись или корректировка шапки   */
/**************************************/
/*Если вернули 0- записали
               1-нет
*/
               
int zapf8dr(VVOD *VV,
const char *nomds,  //Номер документа который исправлен
int vidd)  //0-новые записи 1-удаление записей
{
char		strsql[512];
time_t		vrem;
short		d1,m1,g1;
short		d2,m2,g2;
SQLCURSOR curr;
GDITE();

if(VV->data_ravno_atoi(1) < 1 || VV->data_ravno_atoi(1) > 4)
 {
  iceb_t_soob(gettext("Неправильно ввели квартал !"));
  return(1);

 }
if(VV->data_ravno(3)[0] != '\0')
if(rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(3),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата документа !"));
  return(1);
 }
if(VV->data_ravno(6)[0] != '\0')
if(rsdat(&d2,&m2,&g2,VV->VVOD_SPISOK_return_data(6),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата документа ГНИ !"));
  return(1);
 }
class lock_tables kkk("LOCK TABLE F8dr WRITE,icebuser READ");
/*Проверяем есть ли такой номер документа*/
if(SRAV(VV->VVOD_SPISOK_return_data(2),nomds,0) != 0)
 {
  sprintf(strsql,"select nomd from F8dr where nomd='%s'",VV->VVOD_SPISOK_return_data(2));
  if(readkey(strsql) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(2));
    iceb_t_soob(strsql);
    return(1);

   }
 }

 
time(&vrem);

/*Ввод новой записи*/
if(nomds[0] == '\0')
 {
  sprintf(strsql,"insert into F8dr \
values (%d,%d,'%s',%d,'%s','%04d-%d-%d','%04d-%d-%d',%d,%d,%ld,%d,%d)",
  atoi(VV->VVOD_SPISOK_return_data(0)),atoi(VV->VVOD_SPISOK_return_data(1)),VV->VVOD_SPISOK_return_data(2),vidd,VV->VVOD_SPISOK_return_data(5),g1,m1,d1,g2,m2,d2,
  0,iceb_t_getuid(),vrem,VV->data_ravno_atoi(7),VV->data_ravno_atoi(8));
  if(sql_zapis(strsql,1,0) != 0)
    return(1);
 }
else/*Корректировка записи*/
 {
  sprintf(strsql,"update F8dr set \
god=%d,\
kvrt=%d,\
nomd='%s',\
vidd=%d,\
nomdn='%s',\
datad='%04d-%d-%d',\
datan='%04d-%d-%d',\
metbl=%d,\
ktoz=%d,\
vrem=%ld,\
kr=%d,\
ks=%d \
where nomd='%s'",
atoi(VV->VVOD_SPISOK_return_data(0)),atoi(VV->VVOD_SPISOK_return_data(1)),VV->VVOD_SPISOK_return_data(2),vidd,VV->VVOD_SPISOK_return_data(5),g1,m1,d1,g2,m2,d2,0,
iceb_t_getuid(),vrem,
VV->data_ravno_atoi(7),
VV->data_ravno_atoi(8),
nomds);

  if(sql_zapis(strsql,1,0) != 0)
    return(1);

  if(SRAV(VV->VVOD_SPISOK_return_data(2),nomds,0) != 0)
   {
    sprintf(strsql,"update F8dr1 set nomd='%s' where nomd='%s'",VV->VVOD_SPISOK_return_data(2),nomds);
    sql_zapis(strsql,1,0);

    sprintf(strsql,"update F1dfvs set nomd='%s' where nomd='%s'",VV->VVOD_SPISOK_return_data(2),nomds);
    sql_zapis(strsql,1,0);

   }

 }

return(0);

}
/******************************/
/*Определение номера документа*/
/******************************/

int	nomdf8dr()
{
int		kolstr;
SQL_str         row;
char		strsql[512];
int             mpn;  /*Максимальный порядковый номер*/
int             pn;   /*Порядковый номер*/

GDITE();

sprintf(strsql,"select nomd from F8dr");

//printw("\n%s\n",strsql);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
mpn=pn=0;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  pn=atoi(row[0]);
  if(pn > mpn)
   mpn=pn;

 }

mpn++;

clearstr(LINES-1,0);
return(mpn);


}
