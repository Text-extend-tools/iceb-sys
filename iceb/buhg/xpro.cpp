/* $Id: xpro.c,v 5.79 2014/05/07 10:57:24 sasa Exp $ */
/*01.02.2017	16.02.1999	Белых А.И.	xpro.c
Программа продажи без привязки к карточкам
*/
#define DVERSIQ "01.05.2014"
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#endif
#include        "buhg.h"

void		poimatdok1(KASSA);
int dvden(class spis_oth*);
int rasosmat(class spis_oth*);
int rasspdok(class spis_oth*);
void            dirobq(void);

SQL_baza	bd;
extern char     *printer; /*Печать*/
//extern short    mvnp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
extern short    mdd;  /*0-обычный документ 1-двойной*/
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
int		kassa ;
extern int iceb_kod_podsystem;

/********************************/
/*Работа с кассовым регистратором*/
/*********************************/
void            rabmariq(KASSA kasr)
{
int           kom;

   VVOD MENU(3);
   MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с кассовым регистратором"));
   MENU.VVOD_SPISOK_add_MD(gettext("Печать дневного отчётного чека"));
   MENU.VVOD_SPISOK_add_MD(gettext("Очистить индикатор кассового регистратора"));
   MENU.VVOD_SPISOK_add_MD(gettext("Печать отчётного чека за период"));
   MENU.VVOD_SPISOK_add_MD(gettext("Служебное внесение"));
   MENU.VVOD_SPISOK_add_MD(gettext("Служебное изъятие"));
   MENU.VVOD_SPISOK_add_MD(gettext("Выдача из кассы клиенту"));
   MENU.VVOD_SPISOK_add_MD(gettext("Внесение в кассу клиентом"));
   MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


   kom=0;

   for(;;)
    {
     clear();

     if(menu3(&MENU,&kom,1,3,0) != 0)
       continue;

     switch (kom)
      {
       case 7:
       case -1 :
         return;
         break;

       case 0 :
        printdayshek(kasr);
        break;

       case 1 :
         GDITE();
         mariqtxt(kasr,1,"");
         break;

       case 2 :
         printpershek(kasr);
         break;

       case 3 :
         marsiv(kasr,0);
         break;

       case 4 :
         marsiv(kasr,1);
         break;

       case 5 :
         mariqvv(kasr,2);
         break;

       case 6 :
         mariqvv(kasr,1);
         break;

       default:
         kom=0;
         break;
      }
    }
}


int             main(int argc,char **argv,char **envp)
{
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
char		bros[512];
char		sio[312];
int		kom=0;
static class iceb_tu_str dat1("");
class iceb_tu_str kprr("");
class iceb_tu_str naim("");
int		lnds;
short		d,m,g;
int		skl,kgrm,kodm;
class iceb_tu_str nomdok("");
double		sumkor;

KASSA kasr;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQLCURSOR curr;
iceb_kod_podsystem=ICEB_PS_MU;

icebstart(argc,argv);

VVOD MENU(4);
VVOD VV(0);

/*Чтение настpойки*/
if(nastsys() != 0)
 exit_iceb(1);
if(matnast() != 0)
 exit_iceb(1);

kasr.handle = -1;
/* прочитаем параметры кассового регистратора из файла настройки	*/
if (nastrmariq( 1, &kasr) == 0)
   /* если успешно - пытаемся связаться с кассовым сервером		*/
#ifdef HAVE_TORGDEV
   if ((kasr.handle=iceb_open_server(kasr.host, kasr.port)) < 0)
      errorkan(kasr.handle); // ошибка открытия связи
#else
      errorkan(-1); // ошибка открытия связи
#endif

memset(bros,'\0',sizeof(bros));

iceb_t_perzap(0); /*Проверка на первичный запуск в текущем дне и удаление *.lst */
int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

kom=0;
for(;;)
 {

  clear();

  MENU.VVOD_delete();
  MENU.VVOD_SPISOK_add_ZAG(gettext("Программа выписки товара и материалов"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Ввод новой накладной"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр архива документов"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Поиск документа"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка товаров"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Снять все с очереди на печать"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение товара за день"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать абсолютные остатки"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами для сдачи налички в банк"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Оформление возврата товара"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с кассовым регистратором"));//10
  MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//11
  MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//12
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//13

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
    continue;
  class spis_oth oth;
  switch (kom)
   {
    case 13 :
    case -1 :
	   goto kon;
	   break;

    case 0 :
     clear();

     memset(bros,'\0',sizeof(bros));
     memset(sio,'\0',sizeof(sio));
     iceb_t_poldan("Список исключенных операций для выписки",sio,"matnast.alx");

     dat1.plus_tek_dat();

     kprr.new_plus("");
     naim.new_plus("");

     mdd=mvnp=0;

     if(dirrashod(1,&kprr,&naim) != 0)
      break;

     VV.VVOD_delete();
     sozvdnd(2,&VV);     
     VV.VVOD_SPISOK_zapis_data(1,dat1.ravno());
     lnds=0;
     sumkor=0.;
     if(vdnd(&VV,&dat1,2,&kprr,0,lnds,sumkor,0,1) != 0)
      break;

     clear();

     rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
     skl=atoi(VV.VVOD_SPISOK_return_data(2));
     nomdok.new_plus(VV.VVOD_SPISOK_return_data(3));

     dirdok1(d,m,g,2,skl,&nomdok,kasr);

     break;

    case 1 :
     smdok1(dt,mt,gt, kasr);
     break;

    case 2 :
     poimatdok1(kasr);
     break;

    case 3 :
     naim.new_plus("");
     dirmatr1(&kgrm,&kodm,&naim,0,bros,0,1,0);
     break;

    case 4 :
     GDITE();
     runsystem("lprm -");
     break;

    case 5 :
     if(dvden(&oth) == 0)
      iceb_t_rabfil(&oth,"");
     break;

    case 6 :
     if(rasosmat(&oth) == 0)
      iceb_t_rabfil(&oth,"");
     break;

    case 7 :
     if(rasspdok(&oth) == 0)
      iceb_t_rabfil(&oth,"");
     break;

    case 8 :
      dirobq();
      break;

    case 9 :
     clear();

     memset(bros,'\0',sizeof(bros));
     memset(sio,'\0',sizeof(sio));

     dat1.plus_tek_dat();

     kprr.new_plus("");
     naim.new_plus("");
     mdd=mvnp=0;
     if(dirprihod(1,&kprr,&naim) != 0)
       break;

     VV.VVOD_delete();
     sozvdnd(2,&VV);     
     VV.VVOD_SPISOK_zapis_data(1,dat1.ravno());
     lnds=0;
     sumkor=0.;
     if(vdnd(&VV,&dat1,1,&kprr,0,lnds,sumkor,0,1) != 0)
      break;

     clear();

     rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
     skl=atoi(VV.VVOD_SPISOK_return_data(2));
     nomdok.new_plus(VV.VVOD_SPISOK_return_data(3));

     dirdok1(d,m,g,2,skl,&nomdok,kasr);

     break;

    case 10 :
     rabmariq(kasr);
     break;

    case 11:
       mprintcap();
       break;

    case 12:
      if(smenabaz(1) == 0)
       {
        if(nastsys() != 0)
         goto kon;
        if(matnast() != 0)
            goto kon;
       }
      break;


    default:
      kom=0;
      break; 
   }
 }
kon:;

sql_closebaz(&bd);
#ifdef HAVE_TORGDEV
if (kasr.handle >=0) iceb_close_server(kasr.handle);
#endif

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("База закрыта. Программа закончила работу"));
soobsh(&SOOBN,-1,-1,1);

endwin();
return(0);

}
