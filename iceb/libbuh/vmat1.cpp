/*$Id: vmat1.c,v 5.74 2013/09/26 09:43:55 sasa Exp $*/
/*28.03.2018	05.06.2002	Белых А.И.	vmat1.c
Ввод и корретировка справочников
*/

#include        <ctype.h>
#include	"buhl.h"



void vmat1(SQL_str roww,const char *tz,
short prov,  //0-ввод 1-корректировка
const char *vidop)
{
char		strsql[1024];
class iceb_tu_str kodz("");
time_t		vrem;
class iceb_tu_str bros("");
char		imaf[64];
short		metkor=0;
int		K,N;
int             kt=0;
time_t          vr=0;
int		nomkod=1;
VVOD		SOOB(1);

VVOD		DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

VVOD VV(0);


K=metkor=N=0;

if(SRAV(tz,"Ukrvkr",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код затрат....."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование..."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(80);

 }
if(SRAV(tz,"Grup",0) == 0 || SRAV(tz,"Ukrgrup",0) == 0 \
|| SRAV(tz,"Reegrup1",0) == 0 || SRAV(tz,"Reegrup2",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код группы...."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(80);

 }




if(SRAV(tz,"Uslgrup",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код группы услуги...."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование услуги.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }



if(SRAV(tz,"Foroplat",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код формы оплаты..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование формы оплаты.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }




if(SRAV(tz,"Zargrupp",0) == 0)
 {
  VV.VVOD_SPISOK_add_MD(gettext("Код группы..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование группы.."));
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(100);

 }

if(SRAV(tz,"Gkont",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код группы...................."));
  VV.VVOD_SPISOK_add_MD(gettext("Наим-ние группы контрагентов.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }

if(SRAV(tz,"Kateg",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код категории..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование категории.."));
  
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }
if(SRAV(tz,"Gnali",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код города..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование города.."));
  VV.VVOD_SPISOK_add_MD(gettext("Код контрагента......"));
  
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
  VV.VVOD_SPISOK_add_data(10);
 }
if(SRAV(tz,"Tabel",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код табеля..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование табеля.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }

if(SRAV(tz,"Opltp",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код операции..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование операции.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }

if(SRAV(tz,"Opltt",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код операции..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование операции.."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }

/*Коды фамилий сдающих наличку в банк*/
if(SRAV(tz,"Ksn",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код......."));
  VV.VVOD_SPISOK_add_MD(gettext("Фамилия..."));
  
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }


/*Коды операций сдачи налички в банк*/
if(SRAV(tz,"Oznvb",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Код операции..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование операции.."));
  
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }



/*Группы основных средств*/
if(SRAV(tz,"Uoshau",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Шифр аналитического учета.."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование..............."));

  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }

/*Шифры затрат*/
if(SRAV(tz,"Uoshz",0) == 0)
 {

  VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат..."));
  VV.VVOD_SPISOK_add_MD(gettext("Наименование.."));
  
  VV.VVOD_SPISOK_add_data(10);
  VV.VVOD_SPISOK_add_data(60);
 }





if(prov == 0) //Ввод новой записи
 {
  nomkod=oprsk(tz);
  if(nomkod != 0)
   {
    sprintf(strsql,"%d",nomkod);
    VV.VVOD_SPISOK_zapis_data(0,strsql);
   }
 
 
    kt=vr=0;

 }

if(prov == 1) //Корректировка записи
 {

  VV.VVOD_SPISOK_zapis_data(0,roww[0]);
  VV.VVOD_SPISOK_zapis_data(1,roww[1]);

  kt=vr=0;
  if(roww[2] != NULL)      
     kt=atoi(roww[2]);
  if(roww[3] != NULL)      
     vr=atoi(roww[3]);




  if(SRAV(tz,"Gnali",0) == 0 )
   {
    VV.VVOD_SPISOK_zapis_data(2,roww[2]);
    kt=0;
    if(roww[3] != NULL)      
      kt=atoi(roww[3]);
    vr=0;
    if(roww[4] != NULL)
      vr=atoi(roww[4]);
   }


 }

/*
printw("kt=%d vr=%ld\n",kt,vr);
refresh();
*/

if(prov == 1)
 {
  clearstr(1,0);
  kzvz(kt,vr,&VV);
 }



kodz.new_plus(VV.VVOD_SPISOK_return_data(0));
if(kodz.ravno()[0] != '\0')
 N=1;

naz1:;


 helstr(LINES-1,0,"F2",gettext("запись"),"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {

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
    case KEY_RESIZE:
//      getch();
      clear();
      goto naz1;
      
    case FK10:
    case ESC:
      clear();
      return;

    case ENTER:

      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CFV));

      VV.vprintw_clear(N,stdscr);


      VV.vgetstr(N,stdscr);
      

      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      VV.vprintw(N,stdscr);
      N++;
      break;      

    case FK1:
      memset(imaf,'\0',sizeof(imaf));
      
      if(imaf[0] == '\0')
       {
        SOOB.VVOD_delete();
        SOOB.VVOD_SPISOK_add_MD(gettext("Не задано имя файла с документацией !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("       Сообщите разработчику !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        clear();
        goto naz1;
       }
      iceb_t_pdoc(imaf);

      clear();

      goto naz1;

    case FK2:
    case PLU:

      if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
       {
        iceb_t_soob(gettext("Не введён код !"));
        goto naz1;
       }

      if(ATOFM(VV.VVOD_SPISOK_return_data(0)) < 0)
       {
        iceb_t_soob(gettext("Код не может быть отрицательным !"));
        goto naz1;
       }

      if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
       {
        iceb_t_soob(gettext("Не введено наименование !"));
        goto naz1;
       }
      if(SRAV(tz,"Grup",0) == 0 ||\
      SRAV(tz,"Kateg",0) == 0 ||\
      SRAV(tz,"Tabel",0) == 0 ||\
      SRAV(tz,"Gnali",0) == 0 || \
      SRAV(tz,"Uslgrup",0) == 0\
      || SRAV(tz,"Gkont",0) == 0 \
      || SRAV(tz,"Uplpodr",0) == 0)
       {
         if(isdigit(VV.VVOD_SPISOK_return_data(0)[0]) == 0)  /*Значит символ не число*/
          {
           iceb_t_soob(gettext("Код должен быть цифровым !"));
           goto naz1;
          }
         if(atoi(VV.VVOD_SPISOK_return_data(0)) == 0)
          {
           iceb_t_soob(gettext("Код не может быть нулевым !"));
           goto naz1;
          }
        }
      /*Проверяем есть ли уже такой код*/
      if(prov == 0 || (prov == 1 && SRAV(kodz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0))
       {
        if(SRAV(tz,"Grup",0) == 0 ||\
        SRAV(tz,"Kateg",0) == 0 ||\
        SRAV(tz,"Tabel",0) == 0 ||\
        SRAV(tz,"Gnali",0) == 0 || \
        SRAV(tz,"Uslgrup",0) == 0\
        || SRAV(tz,"Gkont",0) == 0 \
        || SRAV(tz,"Uplpodr",0) == 0 )
          sprintf(strsql,"select kod from %s where kod=%s",tz,VV.VVOD_SPISOK_return_data(0));
        else
          sprintf(strsql,"select kod from %s where kod='%s'",tz,VV.VVOD_SPISOK_return_data(0));

        if(sql_readkey(&bd,strsql) >= 1)
         {

          sprintf(strsql,gettext("Код %s уже есть в списке !"),VV.VVOD_SPISOK_return_data(0));

          iceb_t_soob(strsql);
          goto naz1;
         }
       }


      if(SRAV(tz,"Gnali",0) == 0 && VV.VVOD_SPISOK_return_data(2)[0] != '\0' )
       {
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
        VV.VVOD_SPISOK_return_data(2));
        if(sql_readkey(&bd,strsql) != 1)
         {
          iceb_t_soob(gettext("Не найден код контрагента !"));
          goto naz1;
         }
       }  





      metkor=0;
      if(prov == 1 && SRAV(kodz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
       {

        if(SRAV(tz,"Ukrvkr",0) == 0 || SRAV(tz,"Ukrgrup",0) == 0 \
         || SRAV(tz,"Foroplat",0) == 0 \
         || SRAV(tz,"Zargrupp",0) == 0 \
         || SRAV(tz,"Uplpodr",0) == 0 || SRAV(tz,"Gnali",0) == 0 \
         || SRAV(tz,"Ksn",0) == 0 || SRAV(tz,"Oznvb",0) == 0 )
         {
          SOOB.VVOD_delete();
          SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка кода невозможна !"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto naz1;
         }
        else
         {
          if(danet(&DANET,2,stdscr) == 2)
            goto naz1;
          metkor=1;
          
         }
       }


      time(&vrem);

      /*Записываем*/
       
      if(prov == 0) //Новая запись
       {   
        if(SRAV(tz,"Grup",0) == 0 || SRAV(tz,"Uslgrup",0) == 0 ||
        SRAV(tz,"Kateg",0) == 0 ||
        SRAV(tz,"Tabel",0) == 0 ||
        SRAV(tz,"Gkont",0) == 0\
        || SRAV(tz,"Uplpodr",0) == 0)
          sprintf(strsql,"insert into %s \
values (%s,'%s',%d,%ld)",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),iceb_t_getuid(),vrem);   
        else
          sprintf(strsql,"insert into %s \
values ('%s','%s',%d,%ld)",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),iceb_t_getuid(),vrem);   


         if(SRAV(tz,"Gnali",0) == 0 )
          sprintf(strsql,"insert into %s \
values (%s,'%s','%s',%d,%ld)",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),VV.VVOD_SPISOK_return_data(2),iceb_t_getuid(),vrem);   


       }

      /*Корректировка записи*/
      if(prov == 1)
       {   
        if(SRAV(tz,"Grup",0) == 0 ||\
        SRAV(tz,"Kateg",0) == 0 ||\
        SRAV(tz,"Tabel",0) == 0 ||\
        SRAV(tz,"Uslgrup",0) == 0\
        || SRAV(tz,"Gkont",0) == 0\
        || SRAV(tz,"Uplpodr",0) == 0)
          sprintf(strsql,"update %s \
set \
kod=%s,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%s",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),iceb_t_getuid(),vrem,kodz.ravno());
        else
          sprintf(strsql,"update %s \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod = '%s'",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),iceb_t_getuid(),vrem,kodz.ravno());
        
        if(SRAV(tz,"Gnali",0) == 0)
          sprintf(strsql,"update %s \
set \
kod=%s,\
naik='%s',\
kontr='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%s",tz,VV.VVOD_SPISOK_return_data(0),VV.data_ravno_filtr(1),VV.VVOD_SPISOK_return_data(2),iceb_t_getuid(),vrem,kodz.ravno());


       }

//      printw("\n%s\n",strsql);
//      OSTANOV();

      GDITE();
      if(sql_zap(&bd,strsql) != 0)
       {
         if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
          {
           SOOB.VVOD_delete();
           SOOB.VVOD_SPISOK_add_MD(gettext("У вас нет полномочий для выполнения этой операции !"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           goto naz1;
          }
         else
          {
           msql_error(&bd,gettext("Ошибка записи !"),strsql);
           goto naz1;
          }
       }

      if(metkor == 1)
        korkod(tz,kodz.ravno(),VV.VVOD_SPISOK_return_data(0));
      clear();
      return;
//     }
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
 }
clear();
}

