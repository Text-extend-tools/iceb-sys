/*$Id: sprtab.c,v 5.31 2013/09/26 09:43:41 sasa Exp $*/
/*25.05.2016	01.01.2003	Белых А.И.	sprtab.c
Меню для получения справок по табельному номеру
*/
#include        "buhg.h"

int zarsprpr(long,short,short,short,short,class spis_oth*);
void		zarsprvmn(long,short,short,short,short);
void		rlop(long,short,short,short,short);
int sprtabotp(int,short,short,short,short,class spis_oth*);
int zarsrbold(short dn,short mn,short gn,short dk,short mk,short gk,const char *tabnom,const char *fam,const char *inn,class spis_oth *oth);
int zarsrbol(short dn,short mn,short gn,short dk,short mk,short gk,const char *tabnom,const char *fam,const char *inn,class spis_oth *oth);
int zarsdrv(short mn,short gn,short mk,short gk,int tabnom,class spis_oth *oth);

void	sprtab(int metkaoth)
//Если metkaoth= 1 - расчёт справки о зарплате
//               2 - расчёт средней зарплаты для больничного
//               3 - расчёт справки для расчёта выплат на случай безработицы
//               4 - расчёт справок по датам в счёт которых были сделаны начисления
//               5 - расчёт средней зарплаты для больничного по беременности и родам
{
static class iceb_tu_str dat1(""),dat2("");
static int	tabn;
class iceb_tu_str fam("");
class iceb_tu_str inn("");
class iceb_tu_str datnr("");
char		strsql[1024];
SQL_str		row;
SQLCURSOR curr;
int		ttn1=0;
short		dn,mn,gn;
short		dk,mk,gk;
int		N=0,K=0;
int		konec=0;
class iceb_tu_str fiov("");

VVOD VV(0);
if(tabn == 0)
 VV.VVOD_SPISOK_add_data(10);
else
 {
  sprintf(strsql,"%d",tabn);
  VV.VVOD_SPISOK_add_data(strsql,10);

  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
    iceb_t_soob(strsql);
   }
  else
   {
    fam.new_plus(row[0]);
   }
 }

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);

VV.VVOD_SPISOK_add_MD(gettext("Табельний номер....."));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(м.г)..."));

naz:;


clear();

VV.VVOD_delete_ZAG();

if(metkaoth == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт справок о зарплате"));
if(metkaoth == 2)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт средней зарплаты для начисления больничного"));
if(metkaoth == 3)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт средней зарплаты для социальной помощи"));

if(metkaoth == 4)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт справок о зарплате"));

if(metkaoth == 5)
  VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать расчёт больничного по беременности и родам"));


if(fam.ravno()[0] != '\0')
 {
  VV.VVOD_SPISOK_add_ZAG(fam.ravno());
 }

if(datnr.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Дата начала работы"),datnr.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 } 

if(metkaoth == 2 || metkaoth == 3 || metkaoth == 5)
 {
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);
 }
else
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


while(konec == 0)
 {
  if(N > VV.KLST-1)
    N=0;

  if(N < 0)
    N=VV.KLST-1;

  tabn=atol(VV.VVOD_SPISOK_return_data(0));
  dat1.new_plus(VV.data_ravno(1));
  dat2.new_plus(VV.data_ravno(2));

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);


  switch(K)
   {

    case KEY_RESIZE: //Изменение размеров экрана
      goto naz;

    case ENTER:

      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CFV));
      VV.vprintw_clear(N,stdscr);

      VV.vgetstr(N,stdscr);
      
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Табельный номер
       {

        sprintf(strsql,"select fio,datn from Kartb where tabn=%s",VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          fam.new_plus("");
          datnr.new_plus("");
         }
        else
         {
          fam.new_plus(row[0]);
          datnr.new_plus(iceb_tu_datzap(row[1]));
         }

        N++;
        goto naz;
       }

      if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата начала
       {
        if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(N)) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата начала !"));
          goto naz;
         }
       }
      if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата начала
       {
        if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(N)) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата конца !"));
          goto naz;
         }
       }
      VV.vprintw(N,stdscr);
      N++;

      break;
      
    case SDN: //Стрелка вниз
     N++;
     break;

    case SUP:  //Стрелка вверх
      N--;
      break;

    case FK1:
      GDITE();
      if(metkaoth == 1)
        sprintf(strsql,"%s","zarp4_3_2_1.txt");
      if(metkaoth == 2)
        sprintf(strsql,"%s","zarp4_3_2_2.txt");
      if(metkaoth == 3)
        sprintf(strsql,"%s","zarp4_3_2_5.txt");

      if(metkaoth == 4)
       sprintf(strsql,"%s","zarp4_3_2_3.txt");


      if(metkaoth == 5)
        sprintf(strsql,"%s","zarp4_3_2_2.txt");

      iceb_t_pdoc(strsql);
      goto naz;

    case FK2:
    case PLU:
     konec=1;
     break;

    case FK3:
     clear();
      ttn1=0;
      fiov.new_plus("");
      if(dirtab(&ttn1,&fiov,0,1) == 0)
       {
        tabn=ttn1;           
        sprintf(strsql,"%d",ttn1);
        VV.VVOD_SPISOK_zapis_data(0,strsql);
        fam.new_plus(fiov.ravno());
        sprintf(strsql,"select datn from Kartb where tabn=%d",ttn1);
        if(readkey(strsql,&row,&curr) == 1)
          datnr.new_plus(iceb_tu_datzap(row[0]));

       }
       
      goto naz;

    case FK5:

      if(metkaoth == 2 )
       iceb_redfil("zarsrbol.alx",0);
      if(metkaoth == 3 )
       iceb_redfil("zarsruv.alx",0);

      if(metkaoth == 5 )
       iceb_redfil("zarsrbold.alx",0);

      goto naz;

    case FK4:
     VV.VVOD_clear_data();
     fam.new_plus("");
     goto naz;

    case FK10:
     return;

   }

 }

konec=0; //Чтобы не зациклило


if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён табельный номер !"));
  goto naz;
 }

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(1)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(2)[0] != '\0')
 {
  if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(2)) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата конца !"));
    goto naz;
   }
 }
else
 {
  mk=mn;
  gk=gn;
 } 

sprintf(strsql,"select fio,inn from Kartb where tabn=%d",tabn);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
  iceb_t_soob(strsql);
  goto naz;
 }

fam.new_plus(row[0]);
inn.new_plus(row[1]);
class spis_oth oth;

if(metkaoth == 1)
 if(zarsprpr(tabn,mn,gn,mk,gk,&oth) == 0)
  iceb_t_rabfil(&oth,"");

dn=1;
dpm(&dk,&mk,&gk,5);
class iceb_tu_str tabnom;
tabnom.plus(tabn);
//sprintf(tabnom,"%d",tabn);
if(metkaoth == 2)
  if(zarsrbol(dn,mn,gn,dk,mk,gk,tabnom.ravno(),fam.ravno(),inn.ravno(),&oth) == 0)
   iceb_t_rabfil(&oth,"");

if(metkaoth == 3)
  if(zarsdrv(mn,gn,mk,gk,tabn,&oth) == 0)
   iceb_t_rabfil(&oth,"");

if(metkaoth == 4)
  zarsprvmn(tabn,mn,gn,mk,gk);

if(metkaoth == 5)
  if(zarsrbold(dn,mn,gn,dk,mk,gk,tabnom.ravno(),fam.ravno(),inn.ravno(),&oth) == 0)
   iceb_t_rabfil(&oth,"");


}
