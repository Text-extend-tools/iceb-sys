/*$di:$*/
/*30.03.2017	26.11.2016	Белых А.И.	fort_vhn.c
Ввод шапки наряда
*/

#include <buhl.h>

int fort_vhn_zap(short mn,short gn,const char *nn,int podr,class VVOD *VV);

int fort_vhn(short *mn,short *gn,class iceb_tu_str *nn,int *podr)
{
int voz=0;
int N=0;
int K=0;
char strsql[1024];
static class iceb_tu_str datan("");
if(datan.getdlinna() <= 0)
 {
  short dt=0,mt=0,gt=0;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%02d.%d",mt,gt);
  datan.new_plus(strsql);
 }

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(8);//0
VV.VVOD_SPISOK_add_data(8);//1
VV.VVOD_SPISOK_add_data(8);//2
VV.VVOD_SPISOK_add_data(64);//3
VV.VVOD_SPISOK_add_data(64);//4
VV.VVOD_SPISOK_add_data(128);//5



if(*mn != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  
  sprintf(strsql,"select * from Forthn where datan='%04d-%02d-01' and nn='%s' and pd=%d",*gn,*mn,nn->ravno(),*podr);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    printw("\n%s\n",strsql);
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);   
   }
  VV.VVOD_SPISOK_zapis_data(0,iceb_tu_datzap_mg(row[0]));
  VV.VVOD_SPISOK_zapis_data(1,row[1]);
  VV.VVOD_SPISOK_zapis_data(2,row[2]);
  VV.VVOD_SPISOK_zapis_data(3,row[3]);
  VV.VVOD_SPISOK_zapis_data(4,row[4]);
  VV.VVOD_SPISOK_zapis_data(5,row[5]);



  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка наряда"));
  kzvz(atoi(row[6]),atol(row[7]),&VV);
     
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод нового наряда"));
  VV.VVOD_SPISOK_zapis_data(0,datan.ravno());
 }



VV.VVOD_SPISOK_add_MD(gettext("Дата наряда..(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер наряда........"));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Заказ..............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Исполнитель........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.........."));//5


naz:;


helstr(LINES-1,0,
//"F1","допомога"),
"F2/+",gettext("запись"),
//"F3",gettext("реквизиты"),
"F10",gettext("виход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    if(N == 5 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    voz=1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {

    if(fort_vhn_zap(*mn,*gn,nn->ravno(),*podr,&VV) != 0)
     goto naz;

    rsdat1(mn,gn,VV.data_ravno(0));

    nn->new_plus(VV.data_ravno(1));
    *podr=VV.data_ravno_atoi(2);
    break;
   }

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(voz);



}
/*************************/
/*запись*/
/************************/
int fort_vhn_zap(short mn,short gn,const char *nn,int podr,class VVOD *VV)
{
char strsql[1024];

short m=0,g=0;

if(rsdat1(&m,&g,VV->data_ravno(0)) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата наряда!"));
  return(1);
  
 }

if(VV->data_ravno_atoi(2) == 0)
 {
  iceb_t_soob(gettext("Не введён код подрозделения!"));
  return(1);
  
 }

if(mn == 0)
 sprintf(strsql,"insert into Forthn values('%04d-%02d-01','%s',%d,'%s','%s','%s',%d,%ld)",
 g,m,
 VV->data_ravno_filtr(1),
 VV->data_ravno_atoi(2),
 VV->data_ravno_filtr(3),
 VV->data_ravno_filtr(4),
 VV->data_ravno_filtr(5),
 iceb_t_getuid(),
 time(NULL));
else
 sprintf(strsql,"update Forthn set \
datan='%04d-%02d-01',\
nn='%s',\
pd=%d,\
zak='%s',\
isp='%s',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where datan='%04d-%02d-01' and nn='%s'",
 g,m,
 VV->data_ravno_filtr(1),
 VV->data_ravno_atoi(2),
 VV->data_ravno_filtr(3),
 VV->data_ravno_filtr(4),
 VV->data_ravno_filtr(5),
 iceb_t_getuid(),
 time(NULL),
 gn,mn,
 nn);


if(sql_zapis(strsql,0,0) != 0)
 return(1);
 
if(mn != 0)
 {
  sprintf(strsql,"update Fortzn set datan='%04d-%02d-01',nn=%s,pd=%d where datan='%04d-%02d-01' and nn=%s and pd=%d",g,m,VV->data_ravno(1),VV->data_ravno_atoi(2),gn,mn,nn,podr);
  sql_zapis(strsql,0,0);
 }
return(0);
}
