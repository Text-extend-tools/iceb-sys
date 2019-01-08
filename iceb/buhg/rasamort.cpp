/* $Id: rasamort.c,v 5.24 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015    10.07.1996      Белых А.И.      rasamort.c
Расчёт амортизаци по организации
*/
#include        "buhg.h"


void    rasamort(const char *gruppa) 
{
time_t          tmm;
class iceb_tu_str dr("");
short           m,g,i;
short		kvrt;
long		kolstr,kolstr1;
SQL_str         row;
char		strsql[512];
long		in;
int		kom;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear();

int mr=1;
if(uosprovar(gruppa,0) == 1)
 mr=0;

kvrt=m=g=0;
VVOD VV(0);
VVOD DANET(1);

if(mr == 0) //от остаточной балансовой стоимости
 {

  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации"));
  sprintf(strsql,"%s:%s",gettext("Группа"),gruppa);
  VV.VVOD_SPISOK_add_ZAG(strsql);

  VV.VVOD_SPISOK_add_MD(gettext("Квартал (1,2,3,4).."));
  VV.VVOD_SPISOK_add_MD(gettext("Год................"));
  VV.VVOD_SPISOK_add_data(2);
  VV.VVOD_SPISOK_add_data(5);
  
  sprintf(strsql,"%d",gt);
  VV.VVOD_SPISOK_zapis_data(1,strsql);

  naz:;


  helstr(LINES-1,0,"F2/+",gettext("рассчитать"),
  "F10",gettext("выход"),NULL);

  kom=VV.vvod(0,1,1,-1,-1);
  switch(kom)
   {
    case FK10:
    case ESC:
      return;

    case FK2:
    case PLU:
      break;

    default:
     goto naz;
   }  

  kvrt=(short)ATOFM(VV.VVOD_SPISOK_return_data(0));
  if(kvrt < 1 || kvrt > 4 )
   {
    iceb_t_soob(gettext("Не верно введён квартал !"));
    goto naz;
   }

  g=(short)ATOFM(VV.VVOD_SPISOK_return_data(1));
  if(g < 1997 || g > 3000)
   {
    iceb_t_soob(gettext("Не верно введён год !"));
    goto naz;
   } 
  if(kvrt == 1)
    m=1;
  if(kvrt == 2)
    m=4;
  if(kvrt == 3)
    m=7;
  if(kvrt == 4)
   m=10;

 } 


if(mr == 1) //от начальной балансовой стоимости
 {
naz1:;

  helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

  VVOD VVOD1(2);

  dr.new_plus("");
  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации"));
  sprintf(strsql,"%s:%s",gettext("Группа"),gruppa);
  VV.VVOD_SPISOK_add_ZAG(strsql);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату расчёта (м.г)"));

  if((kom=vvod1(&dr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;

  if(rsdat1(&m,&g,dr.ravno()) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz1;
   }
 } 

if(mr == 2) //этого не может быть
 {
naz2:;

  helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

  VVOD VVOD1(2);

  dr.new_plus("");
  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации"));
  VVOD1.VVOD_SPISOK_add_MD("Введите год расчёта");

  if((kom=vvod1(&dr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;
  g=dr.ravno_atoi();
  if(g < 2000)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz2;
   }
 } 

if(iceb_t_pbpds(m,g) != 0)
  goto naz;

if(mr == 0)
  for(i=m+1 ; i < m+3; i++)
    if(iceb_t_pbpds(i,g) != 0)
      goto naz1;

clear();
if(mr == 0)
 printw("%s %d %s %d%s\n",gettext("Расчёт за"),kvrt,
 gettext("квартал"),g,gettext("г."));
if(mr == 1)
  printw("%s %d.%d%s\n",gettext("Расчёт за"),m,g,
  gettext("г."));

refresh();

time(&tmm);

sprintf(strsql,"select innom from Uosin order by innom asc");
SQLCURSOR cur;
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
 
unlink("amort.lst");
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  in=atol(row[0]);
  amort(in,m,g,0,kvrt,gruppa,NULL);
 }

printw_vr(tmm);


OSTANOV();
}
