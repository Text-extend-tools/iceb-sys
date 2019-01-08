/* $Id: prpp.c,v 5.38 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015	27.04.1999	Белых А.И.	prpp.c
Проверка парности проводок
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

int  prppc(SQLCURSOR*,int,FILE*);

extern char  *imabaz;
void		prpp(void)
{
short           mnp=0; /*Количество найдених ошибок*/
FILE		*ff;
char		imaf[64];
long		kolstr;
char		strsql[1024];
int		par;
short		dn,mn,gn,dk,mk,gk;
time_t		timen;
time_t 		timek;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");

clear();
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VV.VVOD_SPISOK_add_ZAG(gettext("Проверка парности проводок"));

VV.VVOD_SPISOK_add_MD(gettext("Начальная дата..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Конечная дата...(д.м.г).."));
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);

naz:;

helstr(LINES-1,0,"F2/+",gettext("начать проверку"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);
dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
switch(par)
 {
  case FK2:
  case PLU:
    break;  
  case FK10:
  case ESC:
   return;
  default:
   goto naz;
 }
time(&timen);


dn=mn=gn=dk=mk=gk=0;
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена начальная дата !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
 if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1),0) != 0)
  {
   iceb_t_soob(gettext("Не верно введена конечная дата !"));
   goto naz;
  }

clear();
printw("%s\n",gettext("Проверка логической целосности базы данных"));
refresh();

GDITE();

SQLCURSOR cur;

class lock_tables blokt("LOCK TABLE \
Prov READ,\
Plansh READ,\
Kontragent READ,\
Skontr READ,\
Dokummat READ,\
Ukrdok READ,\
Uosdok READ,\
Pltp READ,\
Pltt READ,\
Usldokum READ,\
Alx READ,\
Kasord READ"); //Блокируем таблицы на запись для других клиентов 
  
//После того как мы взяли проводки для проверки, необходимо чтобы пока мы проверяем
//никто из других одновременно работающих операторов не удалил записи, которые мы проверяем
//для этого блокируются таблицы для удаления и корректировки в них записей

sprintf(strsql,"select * from Prov where datp >= '%04d-%02d-%02d' order by datd asc",gn,mn,dn);
if(dk != 0)
 sprintf(strsql,"select * from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

//printw("%s\n",strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
   
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

printw("%s %s\n",iceb_t_get_namebase(),iceb_t_get_pnk("00",0));
printw("%s\n",gettext("Протокол найденых ошибок"));

printw(gettext("Количество проводок %ld"),kolstr);
printw("  %d.%d.%d > %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
refresh();

mnp=0;


sprintf(imaf,"prover%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"%s %s\n",iceb_t_get_namebase(),iceb_t_get_pnk("00",0));
fprintf(ff,"%s\n",gettext("Протокол найденых ошибок"));
fprintf(ff,"%s:%ld ",gettext("Количество проводок"),kolstr);
fprintf(ff,"  %d.%d.%d => %d.%d.%d\n\n",dn,mn,gn,dk,mk,gk);

mnp=prppc(&cur,kolstr,ff);

blokt.unlock();

fclose(ff);

time(&timek);

double kii=0.;
double dii=(timek-timen)/60.;
dii=modf(dii,&kii);

printw("%s %.f%s %.f%s\n",
gettext("Время расчёта"),
kii,
gettext("мин."),
dii*60.,
gettext("сек."));

if(mnp == 0)
 {
  iceb_t_soob(gettext("Порядок, ошибок не обнаружено !"));
  unlink(imaf);
 }
else
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s: %d",gettext("База не корректна ! Количество ошибок"), mnp);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s %s",gettext("Распечатку выгружено в файл"),imaf);

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  class spis_oth oth;
  oth.spis_imaf.plus(imaf);
  oth.spis_naim.plus(gettext("Протокол найденных ошибок"));
  iceb_t_ustpeh(imaf,3);
  iceb_t_rabfil(&oth,"");
 }

}
/*************************************/
/*Проверка курсора                  */
/*************************************/

int  prppc(SQLCURSOR *cur,
int kolstr,
FILE *ff)
{
SQL_str	        row;
struct OPSHET	shetv;
int             mnp=0;
time_t          tmm;
struct  tm      *bf;
int		par;
int             kz=0;
int             pozz=0;
short		d,m,g;
double		deb,kre;
char strsql[1024];
class iceb_tu_str koment("");
class iceb_tu_str nomd("");
class iceb_tu_str oper("");

while(cur->read_cursor(&row) != 0)
 {
/*********
  wprintw(win1,"%s %s %s %s %s %s %s %s\n",
  row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
  wrefresh(win1);
*************/  
  if(kolstr != 0)
   strzag(LINES-1,0,kolstr,++kz);

  if(prsh(row[2],&shetv) == 0)
   {
    mnp++;
    beep();
    printw("%s %s %s %s %s %s %s %s\n",
    row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
    printw(gettext("Нет счета %s в плане счетов !"),row[2]);
    printw("\n");
    refresh();
    fprintf(ff,gettext("Нет счета %s в плане счетов !"),row[2]);

    tmm=atol(row[12]);
    bf=localtime(&tmm);
    fprintf(ff,"\n%s %s %s %s %s %s %s %s %s %s- %s/%s %d.%d.%d %d:%d:%d\n",
    row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],
    row[7],row[11],row[12],
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,
    bf->tm_sec);
//    OSTANOV();
   }

  if(shetv.saldo == 3)
   {
    if(row[4][0] == '\0')
     {
      mnp++;
      beep();
      printw("%s %s %s %s %s %s %s %s\n",
      row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
      printw(gettext("Не введён код контрагента !"));
      printw("\n");
      refresh();
      fprintf(ff,gettext("Не введён код контрагента !"));

      tmm=atol(row[12]);
      bf=localtime(&tmm);
      fprintf(ff,"\n%s %s %s %s %s %s %s %s %s %s- %s/%s %d.%d.%d %d:%d:%d\n",
      row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],
      row[7],row[11],row[12],
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,
      bf->tm_sec);

//      OSTANOV();

     }
    if((par=provkont(row[2],row[4])) != 0)
     {       
      mnp++;
      if(par == 1)
       {
        printw("%s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
        printw(gettext("Не найден код контрагента %s в списке счета %s !"),row[4],row[2]);
        printw("\n");
        refresh();
        fprintf(ff,gettext("Не найден код контрагента %s в списке счета %s !"),row[4],row[2]);
        fprintf(ff,"\n");

       }
      if(par == 2)
       {
        printw("%s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
        printw("%s %s !\n",gettext("Не найден код контрагента"),row[4]);
        refresh();
        fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),row[4]);

       }
    
      tmm=atol(row[12]);
      bf=localtime(&tmm);
      fprintf(ff,"%s %s %s %s %s %s %s %s %s %s- %s/%s %d.%d.%d %d:%d:%d\n",
      row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],
      row[7],row[11],row[12],
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,
      bf->tm_sec);
//      OSTANOV();
     }
   }
  
  if(row[5][0] != '\0')
   {
    //Проверяем есть ли документ к прововке
    if(SRAV(row[5],ICEB_MP_MATU,0) == 0) //Материальный учёт
     {
      //Читаем шапку документа
      sprintf(strsql,"select tip from Dokummat where datd='%s' and sklad=%s and nomd='%s' and kodop='%s' \
and tip=%s",row[14],row[7],row[6],row[8],row[15]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();
        
        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }


    if(SRAV(row[5],ICEB_MP_UKR,0) == 0) //Учёт командировочных расходов
     {
      //Читаем шапку документа
      sprintf(strsql,"select god from Ukrdok where datd='%s' and nomd='%s'",row[14],row[6]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }

    if(SRAV(row[5],ICEB_MP_UOS,0) == 0) //Учёт основных средств
     {
      //Читаем шапку документа
      sprintf(strsql,"select tipz from Uosdok where datd='%s' and nomd='%s' and kodop='%s' \
and tipz=%s",row[14],row[6],row[8],row[15]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }

    if(SRAV(row[5],ICEB_MP_PPOR,0) == 0) //Платёжные поручения
     {
      //Читаем шапку документа 
      sprintf(strsql,"select datd from Pltp where datd='%s' and nomd='%s'",row[14],row[6]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }

    if(SRAV(row[5],ICEB_MP_PTRE,0) == 0) //Платёжные требования
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select datd from Pltt where datd='%s' and nomd='%s'",row[14],row[6]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }

    if(SRAV(row[5],ICEB_MP_USLUGI,0) == 0) //Учёт услуг
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select tp from Usldokum where datd='%s' and podr=%s and nomd='%s' and kodop='%s' \
and tp=%s",row[14],row[7],row[6],row[8],row[15]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }

    if(SRAV(row[5],ICEB_MP_KASA,0) == 0) //Учёт кассовых ордеров
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select tp from Kasord where datd='%s' and kassa=%s and nomd='%s' \
and tp=%s",row[14],row[7],row[6],row[15]);
      if(readkey(strsql) != 1)
       {
        printw("%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        printw("%s\n",gettext("Не найден документ !"));
        refresh();

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s\n",gettext("Не найден документ !"));
        mnp++;
        continue;      
       }
     }
      
   }  
  rsdat(&d,&m,&g,row[1],2);
//  vrem=atol(row[12]);  
  deb=atof(row[9]);
  deb=okrug(deb,0.01);
  kre=atof(row[10]);
  kre=okrug(kre,0.01);
  
/*
  printw("Кількість проводок %ld\n%d.%d.%d %s %s %s %ld %s - %.2f/%.2f\n",
  ++kz,bh2n.den,bh2n.mes,bh2n.god,bh2n.sh,bh2n.shk,bh2n.kor,bh2n.vrem,
  bh2n.kto,bh2n.deb,bh2n.kre);
*/
  if(atoi(row[0]) != 0)
   {
    pozz++;
    continue;
   }

  koment.new_plus(row[13]);
  nomd.new_plus(row[6]);  
  oper.new_plus(row[8]);
  
  sprintf(strsql,"select val from Prov where val=%s and datp='%s' and sh='%s' \
and shk='%s' and kto='%s' and nomd='%s' and pod=%s and oper='%s' and deb=%s and kre=%s \
and ktoi=%s and vrem=%s and komen='%s' and datd='%s'",
   row[0],row[1],row[3],row[2],row[5],nomd.ravno_filtr(),row[7],oper.ravno_filtr(),row[10],
   row[9],row[11],row[12],koment.ravno_filtr(),row[14]);

  if(readkey(strsql) != 1)
   {
    mnp++;
    beep();
    tmm=atol(row[12]);
    bf=localtime(&tmm);

    printw("%s %s %s %s %s %s %s %s\n",
    row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);

    printw(gettext("Не нашли парную проводку !\n"));
    refresh();

    fprintf(ff,"%s %s %s %s %s %s %s %s %s %s- %s/%s %d.%d.%d %d:%d:%d\n",
    row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],
    row[7],row[11],row[12],
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,
    bf->tm_sec);
   }

 }
return(mnp);

}
