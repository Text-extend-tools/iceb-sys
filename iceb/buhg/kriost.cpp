/*$Id: kriost.c,v 5.24 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	12.06.2003	Белых А.И.	kriost.c
Получение списка материалов у которых остаток меньше или равен критическому
*/
#include        <errno.h>
#include	"buhg.h"
#define		KOLSTRLIST 63

void            krisap(int*,int*,FILE*);


int kriost(class spis_oth *oth)
{
char		strsql[1024];
static class iceb_tu_str dat1("");
static class iceb_tu_str skladi("");
static class iceb_tu_str grupi("");
static class iceb_tu_str sheta("");
static class iceb_tu_str kodm("");
short		dos,mos,gos;
int		i=0;
int		kom1=0;
int		kgr=0;

if(dat1.ravno()[0] == '\0')
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(skladi.ravno(),128);
VV.VVOD_SPISOK_add_data(grupi.ravno(),128);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(kodm.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка материалов имеющих критический остаток"));

VV.VVOD_SPISOK_add_MD(gettext("Склад.(,,)......."));
VV.VVOD_SPISOK_add_MD(gettext("Группа..(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт....(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Материал.(,,/*).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата остатка....."));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int par=VV.vvod(0,1,1,-1,-1);

skladi.new_plus(VV.data_ravno(0));
grupi.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
kodm.new_plus(VV.data_ravno(3));
dat1.new_plus(VV.data_ravno(4));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(par)
 {
  case ESC:
  case FK10:
    return(1);
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu5_17.txt");
   clear();
   goto naz;

  case FK2:
  case PLU:
  case OTN:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(0,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&grupi,&naims);
        VV.VVOD_SPISOK_zapis_data(1,grupi.ravno());
        goto naz;

      case 2 :
        kgr=0;
        if(dirmatr(&kgr,&i,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(3,i);
         }
        
        goto naz;

      case 3 :
        vibrek("Plansh",&sheta,&naims);
        VV.VVOD_SPISOK_zapis_data(2,sheta.ravno());
        goto naz;
     }

  case FK4: /*Очистка меню*/
    VV.VVOD_clear_data();
    goto naz;    

  default:
    goto naz;    
 }


if(dat1.ravno()[0] == '\0' || rsdat(&dos,&mos,&gos,dat1.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

time_t 		tmm;
time(&tmm);

clear();
printw("%s\n",gettext("Распечатка материалов имеющих критический остаток"));
printw("%s:%d.%d.%d\n",gettext("Остатки вычислены на"),dos,mos,gos);

GDITE();
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;
SQL_str row,row1;
double kriost=0.;

sprintf(strsql,"select kodm,kodgr,naimat,kriost from Material");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

FILE *ff;
char imaf[64];

int kolstr1=0;
int kolstr2=0;
class ostatokt ost;
double   ostmat=0.;
int kolstrlist=0;
sprintf(imaf,"kro%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Распечатка материалов имеющих критический остаток"),0,0,0,0,0,0,ff);
kolstrlist=5;

fprintf(ff,"%s:%d.%d.%d\n",gettext("Остатки вычислены на"),dos,mos,gos);
kolstrlist++;

if(skladi.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),skladi.ravno());
  kolstrlist++;
 }

if(grupi.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Групы: %s\n"),grupi.ravno());
  kolstrlist++;
 }
if(sheta.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Счета учета: %s\n"),sheta.ravno());
  kolstrlist++;
 }
if(kodm.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Код материалла: %s\n"),kodm.ravno());
  kolstrlist++;
 }

int kollist=0;
krisap(&kollist,&kolstrlist,ff);

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
//  refresh();
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(kodm.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(grupi.ravno(),row[1],0,0) != 0)
    continue;

  kriost=0.;
  if(row[3] != NULL)
    kriost=atof(row[3]);
  ostmat=0.;  
  sprintf(strsql,"select sklad,nomk,shetu from Kart where kodm=%s",row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    break;
   }
  int metka=0;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s %s\n",row1[0],row1[1],row1[2]);
//    refresh();
    if(proverka(skladi.ravno(),row1[0],0,0) != 0)
      continue;
    if(proverka(sheta.ravno(),row1[2],0,0) != 0)
      continue;

    ostkar(1,1,gos,dos,mos,gos,atoi(row1[0]),atoi(row1[1]),&ost);
    metka=1;
    ostmat+=ost.ostg[3];    

   }
  if(metka == 0)
    continue;   

  if(ostmat > kriost)
   continue;  
  kolstrlist++;

  if(kolstrlist > KOLSTRLIST)
   {
    fprintf(ff,"\f");
    kolstrlist=1; //Строка которая будет выведена
    krisap(&kollist,&kolstrlist,ff);
   }

  fprintf(ff,"%-5s|%-*.*s|%10.2f|%10.2f\n",
  row[0],
  iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],kriost,ostmat);
 }

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка материалов имеющих критический остаток"));
iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);
return(0);
}

/**********************************/
/*Шапка распечатки                */
/**********************************/

void  krisap(int *kollist,int *kolstrlist,FILE *ff)
{
*kollist+=1;
*kolstrlist+=4;
fprintf(ff,"%50s%s%d\n","",gettext("Лист N"),*kollist);
fprintf(ff,"\
-----------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |   Наименование               |Кр.остаток| Остаток  |\n"));
fprintf(ff,"\
-----------------------------------------------------------\n");

}
