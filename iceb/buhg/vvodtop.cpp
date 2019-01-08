/*$Id: vvodtop.c,v 5.37 2013/08/13 05:50:00 sasa Exp $*/
/*14.01.2010	17.04.2003	Белых А.И.	vvodtop.c
Воод топлива на приход
Если вернули 0-записали
             1-нет
*/
#include	"buhg.h"

double vvodtop_ost(short dd,short md,short gd,const char *kodvod,const char *kodtop,const char *kodavt);

extern short	startgodupl; //Стартовый год для "Учета путевых листов"

int vvodtop(short metkar, //0-новая запись 1-корректировка
short dd,short md,short gd, //дата документа
const char *podr, //Код подразделения
const char *nomd, //Номер документа
short nomz, //Номер записи
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
int nom_zap) //Порядковый номер документа в один и тотже день
{
int		N=0,K=0;
int		voz=0;
class iceb_tu_str koment("");
char		strsql[1024];
SQL_str		row;
time_t          vrem=0;
struct  tm      *bf;
int		ktozap=0;
class iceb_tu_str naimo("");
double		ostatok=0.;
short		metkavod=0; //0-заправка 1-влодитель
SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10); //0
VV.VVOD_SPISOK_add_data(10); //1
VV.VVOD_SPISOK_add_data(20); //2
VV.VVOD_SPISOK_add_data(250); //3
VV.VVOD_SPISOK_add_data(10); //4
VV.VVOD_SPISOK_add_data(10); //5


if(metkar == 1) //Читаем запись
 {
  sprintf(strsql,"select * from Upldok1 where datd='%d-%d-%d' and \
kp=%d and nomd='%s' and nzap=%d",
  gd,md,dd,atoi(podr),nomd,nomz);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }

  VV.VVOD_SPISOK_zapis_data(0,row[6]);
  VV.VVOD_SPISOK_zapis_data(1,row[8]);
  VV.VVOD_SPISOK_zapis_data(2,row[9]);
  VV.VVOD_SPISOK_zapis_data(3,row[10]);
  VV.VVOD_SPISOK_zapis_data(4,row[11]);

  vrem=atol(row[13]);
  ktozap=atoi(row[12]);

  sprintf(strsql,"select mt,naik from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s !",gettext("Не найден код поставщика"),
    VV.VVOD_SPISOK_return_data(0));
    
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
  else
   {
    metkavod=atoi(row[0]);
    naimo.new_plus(row[1]);
   }
  
 }

naz:;

VV.VVOD_delete_MD();

VV.VVOD_SPISOK_add_MD(gettext("Код поставщика..")); //0
VV.VVOD_SPISOK_add_MD(gettext("Код топлива.....")); //1
VV.VVOD_SPISOK_add_MD(gettext("Количество......")); //2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......")); //3
if(metkavod == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля..")); //4

VV.VVOD_delete_ZAG();
VV.VVOD_SPISOK_add_ZAG(gettext("Получение топлива"));

if(ktozap != 0)
 {
  sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(ktozap));
  VV.VVOD_SPISOK_add_ZAG(strsql);             
 }

if(vrem != 0)
 {
  bf=localtime(&vrem);
  sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
  gettext("Дата записи :"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(naimo.ravno()[0] != '\0')
 {
  if(metkavod == 0)
    sprintf(strsql,"%s:%s",gettext("Заправка"),naimo.ravno());
  if(metkavod == 1)
    sprintf(strsql,"%s:%s",gettext("Водитель"),naimo.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);


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
   {
//    getch();
    goto naz;
   }

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

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Код поставщика
     {
      GDITE();
      naimo.new_plus("");
      sprintf(strsql,"select mt,naik from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код поставщика"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        goto naz;
       }

      metkavod=atoi(row[0]);
      naimo.new_plus(row[1]);
      
      if(metkavod == 1 && SRAV(kodvod,VV.VVOD_SPISOK_return_data(N),0) != 0) //Водитель
       {
        
        SPISOK KTC;
        masiv_din_double OST;

        ostatok=ostva(dd,md,gd,VV.VVOD_SPISOK_return_data(N),kodavt,nom_zap,&KTC,&OST,NULL);
        if(ostatok < 0.009)
         {
          iceb_t_soob(gettext("Нет на остатке топлива у этого водителя на этом автомобиле !"));
          goto naz;
         }
        if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
         {
          sprintf(strsql,"%.2f",ostatok);
          VV.VVOD_SPISOK_zapis_data(2,strsql);
         }
       }
      else
       VV.VVOD_SPISOK_zapis_data(4,"");
      N++;
      goto naz;
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Код топлива
     {
      sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),VV.VVOD_SPISOK_return_data(N));
        
        iceb_t_soob(strsql);
        goto naz;
       }
     }

    
    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Код автомобиля
     {
      if(SRAV(VV.VVOD_SPISOK_return_data(N),kodavt,0) == 0)
      if(SRAV(VV.VVOD_SPISOK_return_data(0),kodvod,0) == 0)
       {
        iceb_t_soob(gettext("Передавать топливо самаму себе нет смысла !"));
        goto naz;
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    voz=1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || \
       VV.VVOD_SPISOK_return_data(1)[0] == '\0' || \
       VV.VVOD_SPISOK_return_data(2)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
      goto naz;
     }

    if(SRAV(VV.VVOD_SPISOK_return_data(4),kodavt,0) == 0)
    if(SRAV(VV.VVOD_SPISOK_return_data(0),kodvod,0) == 0)
     {
      iceb_t_soob(gettext("Передавать топливо самаму себе нет смысла !"));
      goto naz;
     }


    sprintf(strsql,"select mt,naik from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(0));
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код поставщика"),
      VV.VVOD_SPISOK_return_data(0));
      
      iceb_t_soob(strsql);
      goto naz;
     }

    if(row[0][0] == '1' ) //Водитель
     {
      if(VV.data_ravno(4)[0] == '\0')
       {
        iceb_t_soob(gettext("Не введён код автомобиля !"));
        goto naz;
       }
      //Проверяем код автомобиля 
      sprintf(strsql,"select kod from Uplavt where kod=%s",VV.data_ravno(4));
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код автомобиля"),VV.data_ravno(4));
        iceb_t_soob(strsql);
        goto naz;
       }
      ostatok=vvodtop_ost(dd,md,gd,VV.data_ravno(0),VV.data_ravno(1),VV.data_ravno(4));
      if(ostatok-VV.data_ravno_atof(2) < -0.009)
       {
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Нет на остатке столько топлива у этого водителя на этом автомобиле !"));
        sprintf(strsql,"%.2f",ostatok);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
//        goto naz;
       }
     }

    sprintf(strsql,"select kodt from Uplmt where kodt='%s'",VV.VVOD_SPISOK_return_data(1));
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),
      VV.VVOD_SPISOK_return_data(1));
      
      iceb_t_soob(strsql);
      goto naz;
     }

    time(&vrem);
    
    koment.new_plus(VV.VVOD_SPISOK_return_data(3));


        
    if(metkar == 0)
     {
      if(upl_zapvdok(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),
      VV.data_ravno_atoi(0),
      1,
      VV.data_ravno(1),
      VV.data_ravno_atof(2),
      koment.ravno_filtr(),
      VV.data_ravno_atoi(4),
      nom_zap) != 0)
        goto naz;
     }
    else
     {
       sprintf(strsql,"update Upldok1 set \
kodsp=%s,\
tz=1,\
kodtp='%s',\
kolih=%.10g,\
kom='%s',\
kap=%d,\
ktoz=%d,\
vrem=%ld \
where datd='%d-%d-%d' and kp=%d and nomd='%s' and nzap=%d",
       VV.VVOD_SPISOK_return_data(0),
       VV.VVOD_SPISOK_return_data(1),
       ATOFM(VV.VVOD_SPISOK_return_data(2)),
       VV.VVOD_SPISOK_return_data(3),
       atoi(VV.VVOD_SPISOK_return_data(4)),
       iceb_t_getuid(),vrem,
       gd,md,dd,atoi(podr),nomd,nomz);

       sql_zapis(strsql,0,0);

     }

    break;
   }

  if(K == FK3) //Реквизиты
   {
    VVOD MENU(3);
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка объектов учета топлива"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка марок топлива"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    int kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    class iceb_tu_str kodn("");
    class iceb_tu_str naimn("");    
    switch (kom1)
     {
      case 3:
      case -1:
        break;
        
      case 0:
        clear();
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,0) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(0,kodn.ravno());
          naimo.new_plus("");
          sprintf(strsql,"select mt,naik from Uplouot where kod=%s",kodn.ravno());
          if(sql_readkey(&bd,strsql,&row,&curr) != 1)
           {
            sprintf(strsql,"%s %s !",gettext("Не найден код поставщика"),kodn.ravno());
            
            iceb_t_soob(strsql);
            goto naz;
           }

          metkavod=atoi(row[0]);
          naimo.new_plus(row[1]);
         }
        break;

      case 1:
        if(dirmt(&kodn,1) == 0)
          VV.VVOD_SPISOK_zapis_data(1,kodn.ravno());
        break;

      case 2:
        clear();
        if(diravt0(&kodn,&naimn,1) == 0)
         if(metkavod == 1)
          VV.VVOD_SPISOK_zapis_data(4,kodn.ravno());
        break;

     }
    goto naz;
   }
   
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

clear();

return(voz);

}
/****************************/
/*Получение остатка топлива по нужной марке топлива водителю и автомобилю*/
/***********************************************************/

double vvodtop_ost(short dd,short md,short gd,
const char *kodvod,const char *kodtop,const char *kodavt)
{
char strsql[512];
int kolstr;
short gn=startgodupl;

sprintf(strsql,"select sal from Uplsal where god=%d and ka=%s and kv=%s and kmt='%s' ",
gn,kodavt,kodvod,kodtop);

SQLCURSOR cur;
SQL_str row;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }

sprintf(strsql,"select tz,kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and ka=%s and kv=%s and kap=0 order by datd,nomd asc",
gn,1,1,gd,md,dd,kodavt,kodvod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  if(atoi(row[0]) == 2)
    kolih+=atof(row[1])*-1;
  else
     kolih+=atof(row[1]);
 }
//printw("kolih=%f\n",kolih);

//Смотрим передачи топлива другим водителям
sprintf(strsql,"select kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 and kodtp='%s' order by datd,nomd asc",
gn,1,1,gd,md,dd,kodavt,kodvod,kodtop);

//printw("\nostva-%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
//printw("strsql=%s\n",strsql);
//OSTANOV();
while(cur.read_cursor(&row) != 0)
 {
//  printw("3 - %s\n",row[0]);
  
  kolih-=atof(row[0]);
  
 }


kolih=okrug(kolih,0.001);
//printw("\nvoodtop_ost- kolih=%.2f\n",kolih);
//OSTANOV();
return(kolih);

}
