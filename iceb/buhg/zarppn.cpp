/*$Id: zarppn.c,v 5.71 2013/09/26 09:43:45 sasa Exp $*/
/*28.03.2018	23.04.2004	Белых А.И.	zarppn.c
Перерасчёт подоходного налога действующий с 1.1.2004г.
*/
#include        <errno.h>
#include        "buhg.h"


int  zarppn1(short mnr,short mkr,short godr,const char *tabnom,const char *podr,class spis_oth *oth);
int zarppn_read_nastr();


extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    kodpn;   /*Код подоходного налога*/
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern float    prog_min_ng; //Прожиточный минимум на начало года
extern float    kof_prog_min; //Коэффициент прожиточного минимума
extern float  pomzp; //Процент от минимальной заработной платы
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern float  ppn; //Процент подоходного налога

extern short	*kodnvpen; /*Коды не входящие в расчёт пенсионного отчисления*/
extern short	*kodsocstrnv; //Коды не входящие в расчёт соцстраха
extern short	*kodbzrnv; //Коды не входящие в расчёт отчислений на безработицу

int  zarppn(class spis_oth *oth)
{
int kom1=0;
int tabn=0;
class iceb_tu_str fam("");
char strsql[1024];
static class iceb_tu_str mn("");
static class iceb_tu_str mk("");
static class iceb_tu_str god("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str podr("");
short mnr,mkr,godr;
class iceb_tu_str fiov("");
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VV.VVOD_SPISOK_add_ZAG(gettext("Перерасчёт подоходного налога"));

VV.VVOD_SPISOK_add_data(mn.ravno(),3);
VV.VVOD_SPISOK_add_data(mk.ravno(),3);
VV.VVOD_SPISOK_add_data(god.ravno(),5);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Месяц начала............"));//0
VV.VVOD_SPISOK_add_MD(gettext("Месяц конца............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Год....................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//4

naz:;

clear();

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчeт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom1=VV.vvod(0,1,1,-1,-1);

mn.new_plus(VV.data_ravno(0));
mk.new_plus(VV.data_ravno(1));
god.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(3));
podr.new_plus(VV.VVOD_SPISOK_return_data(4));

switch(kom1)
 {
  case FK1:
    GDITE();
//    prosf(bros);
    goto naz;

  case FK2:
  case PLU:
   break;

  case FK10:
   return(1);

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        clear();
        tabn=0;
        fiov.new_plus("");
        if(dirtab(&tabn,&fiov,0,1) == 0)
         {
          sprintf(strsql,"%d",tabn);
          VV.data_z_plus(3,strsql);
         }
        
        break;

      case 1 :
        if(dirzarpodr(2,&podr,&fam) == 0)
         VV.data_z_plus(4,podr.ravno());
        break;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5:
   iceb_redfil("zarppn.alx",0);
   goto naz;

  default:
   goto naz;
 }

mnr=mn.ravno_atoi();
mkr=mk.ravno_atoi();
godr=god.ravno_atoi();

if(mnr == 0 || mnr > 11)
 {
  iceb_t_soob(gettext("Не верно введён месяц начала !"));
  goto naz;
 }

if(mkr == 0 || mkr > 12 || mkr < 2)
 {
  iceb_t_soob(gettext("Не верно введён месяц конца !"));
  goto naz;
 }

if(godr == 0)
 {
  iceb_t_soob(gettext("Не верно введён год !"));
  goto naz;
 }

clear();
GDITE();

return(zarppn1(mnr,mkr,godr,tabnom.ravno(),podr.ravno(),oth));

}

/****************************/
/*Чтение настроек для расчёта*/
/*****************************/

int zarppn_read_nastr()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str pole("");


sprintf(strsql,"select str from Alx where fil='zarppn.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarppn.alx");
  iceb_t_soob(strsql);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&pole,1,'|');

  if(SRAV(pole.ravno(),"Коэффициент прожиточного минимума",0) == 0)
   {
    polen(row_alx[0],&kof_prog_min,2,'|');
//    kof_prog_min = ATOFM(pole);
    continue;
   }
  if(SRAV(pole.ravno(),"Процент подоходного налога",0) == 0)
   {
    polen(row_alx[0],&ppn,2,'|');
//    ppn = ATOFM(pole);
    continue;
   }

  if(SRAV(pole.ravno(),"Процент от минимальной заработной платы",0) == 0)
   {
    polen(row_alx[0],&pomzp,2,'|');
//    pomzp = ATOFM(pole);
    continue;
   }
 }
return(0);

}
