/*$Id: vdupl1.c,v 5.5 2013/09/26 09:43:43 sasa Exp $*/
/*20.03.2013	11.04.2012	Белых А.И.	vdupl1.c
Ввод и корректировка шапки путевых листов.
*/
#include	"buhg.h"

class vdupld_data
 {
  public:
  class iceb_tu_str naimpod;
  class iceb_tu_str naimvod;
  class iceb_tu_str gos_nomer;
  class iceb_tu_str naimavt;
  vdupld_data()
   {
    naimpod.new_plus("");
    naimvod.new_plus("");
    naimavt.new_plus("");
    gos_nomer.new_plus("");
   }

 };


void		uplnomdok(short,const char*,char*);
int             vdupl1_zap(short,short,short,const char*,const char*,VVOD*,class vdupld_data *data);
int             vdupl1_poippl(short,short,short,const char*);
int dirupldok3(short dd,short md,short gd,int kp,const char *nom_dok,int kod_avt);



int vdupl1(short *dd,short *md,short *gd,class iceb_tu_str *nomd,class iceb_tu_str *podr)
{
class vdupld_data data;

int             N=0;
int		K=0;
char		strsql[1024];
short		dv=*dd,mv=*md,gv=*gd; //Дата документа введенная
class iceb_tu_str kod("");
int		kom1=0;
SQL_str		row;
class iceb_tu_str kodn("");
SQLCURSOR curr;
short d,m,g;
short has,min,sek;
VVOD MENU(3);

VVOD VV(0);

VV.VVOD_SPISOK_add_data(11); //0
VV.VVOD_SPISOK_add_data(10); //1
VV.VVOD_SPISOK_add_data(10); //2
VV.VVOD_SPISOK_add_data(10); //3
VV.VVOD_SPISOK_add_data(10); //4
VV.VVOD_SPISOK_add_data(10); //5
VV.VVOD_SPISOK_add_data(10); //6
VV.VVOD_SPISOK_add_data(11); //7
VV.VVOD_SPISOK_add_data(10); //8
VV.VVOD_SPISOK_add_data(11); //9
VV.VVOD_SPISOK_add_data(10); //10

VV.VVOD_SPISOK_add_data(250); //11
VV.VVOD_SPISOK_add_data(10); //12

if(*dd != 0) //Читаем реквизиты шапки
 {
  //Читаем шапку документа
  sprintf(strsql,"select ka,kv,psv,psz,denn,denk,vremn,vremk,otmet,nomb from Upldok where datd='%d-%d-%d' and kp=%d and nomd='%s'",
  *gd,*md,*dd,podr->ravno_atoi(),nomd->ravno());

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    printw("\n%s",strsql);
    sprintf(strsql,"%s !",gettext("Не найдена шапка документа"));
    iceb_t_soob(strsql);
    return(1);
   }
  dv=*dd;
  mv=*md;
  gv=*gd;
  sprintf(strsql,"%d.%d.%d",dv,mv,gv);
  VV.VVOD_SPISOK_zapis_data(0,strsql);
  VV.VVOD_SPISOK_zapis_data(1,podr->ravno());
  VV.VVOD_SPISOK_zapis_data(2,nomd->ravno());
  VV.VVOD_SPISOK_zapis_data(3,row[0]);
  VV.VVOD_SPISOK_zapis_data(4,row[1]);
  VV.VVOD_SPISOK_zapis_data(5,row[2]);
  VV.VVOD_SPISOK_zapis_data(6,row[3]);

  VV.data_plus(7,iceb_tu_datzap(row[4]));
  VV.data_plus(8,row[6]);
  VV.data_plus(9,iceb_tu_datzap(row[5]));
  VV.data_plus(10,row[7]);
  VV.data_plus(11,row[8]);
  VV.data_plus(12,row[9]);
  
  //читаем наименование подразделения
  sprintf(strsql,"select naik from Uplpodr where kod=%s",VV.VVOD_SPISOK_return_data(1)); 
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   data.naimpod.new_plus(row[0]);  

  //читаем фамилию водителя
  sprintf(strsql,"select naik from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(4));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   data.naimvod.new_plus(row[0]);

  //читаем наименование автомобиля
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",VV.VVOD_SPISOK_return_data(3));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    data.gos_nomer.new_plus(row[0]);  
    data.naimavt.new_plus(row[1]);
   }
 }


VV.VVOD_SPISOK_add_MD(gettext("Дата...................(д.м.г)........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения....................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа......................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля........................"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код водителя.........................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Показания спидометра при выезде......."));//5
VV.VVOD_SPISOK_add_MD(gettext("Показания спидометра при возвращении.."));//6

VV.VVOD_SPISOK_add_MD(gettext("Дата выезда...........(д.м.г)........."));//7
VV.VVOD_SPISOK_add_MD(gettext("Время выезда..........(ч.м)..........."));//8
VV.VVOD_SPISOK_add_MD(gettext("Дата возвращения......(д.м.г)........."));//9
VV.VVOD_SPISOK_add_MD(gettext("Время возвращения.....(ч.м)..........."));//10
VV.VVOD_SPISOK_add_MD(gettext("Особенные отметки....................."));//11
VV.VVOD_SPISOK_add_MD(gettext("Номер бланка строгой отчётности......."));//12

nazad:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F10",gettext("выход"),NULL);

VV.vramka(4,0,0);

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  if(data.naimpod.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s:%s",gettext("Подразделение"),data.naimpod.ravno());
    mvprintw(0,0,"%.*s",COLS,strsql);
   }
  if(data.naimvod.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s:%s",gettext("Водитель"),data.naimvod.ravno());
    mvprintw(1,0,"%.*s",COLS,strsql);
   }
  if(data.naimavt.getdlinna() > 1)
   {
    sprintf(strsql,"%s:%s %s",gettext("Автомобиль"),data.naimavt.ravno(),data.gos_nomer.ravno());
    mvprintw(2,0,"%.*s",COLS,strsql);
   }

//  data.probeg=VV.data_ravno_atoi(6)-VV.data_ravno_atoi(5);

  VV.vprintw(N,stdscr);
  
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == KEY_RESIZE)
    goto nazad;

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата документа
     {
      if(rsdat(&dv,&mv,&gv,VV.VVOD_SPISOK_return_data(N),1) != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введена дата !"));
	goto nazad;
       }

      if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
       if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
        {
         char nomer_dok[64];
         uplnomdok(gv,VV.VVOD_SPISOK_return_data(1),nomer_dok);
         VV.data_plus(2,nomer_dok);
         goto nazad;
        }
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код подразделения
     {
      sprintf(strsql,"select naik from Uplpodr where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        data.naimpod.new_plus("");
	goto nazad;
       }
      data.naimpod.new_plus(row[0]);

      if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
       if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
        {
         char nomer_dok[64];
         uplnomdok(gv,VV.VVOD_SPISOK_return_data(1),nomer_dok);
         VV.data_plus(2,nomer_dok);
         N+=2;
         goto nazad;
        }

     }

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Номер документа
     {
      if(SRAV(nomd->ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        sprintf(strsql,"select datd from Upldok where god=%d and kp=%d and nomd='%s'",
        gv,atoi(VV.VVOD_SPISOK_return_data(1)),VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) > 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));

          sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(N));
          
          iceb_t_soob(strsql);
  	  goto nazad;
         }
       }

     }

    if(N == 3 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код автомобиля
     {

kodavt:; //По клавише F3 должно прочитать


      GDITE();

      sprintf(strsql,"select naik,nomz from Uplavt where kod=%s",VV.VVOD_SPISOK_return_data(3));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        sprintf(strsql,"%s %s !",gettext("Не найден код автомобиля"),VV.VVOD_SPISOK_return_data(3));
        iceb_t_soob(strsql);
        data.naimavt.new_plus("");
        goto nazad;
       }
      data.naimavt.new_plus(row[0]);
      data.gos_nomer.new_plus(row[1]);

      if(dv != 0 && *dd == 0)
       {
        int poks=vdupl1_poippl(dv,mv,gv,VV.VVOD_SPISOK_return_data(3));
        if(poks != 0)
         {
          sprintf(strsql,"%d",poks);
          VV.VVOD_SPISOK_zapis_data(5,strsql);
         }
       }

      N++;
      goto nazad;
     }

    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Код водителя
     {
      sprintf(strsql,"select naik from Uplouot where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        data.naimvod.new_plus("");
        goto nazad;
       }
      data.naimvod.new_plus(row[0]);

      if(dd == 0)
       {
        //Проверяем был ли уже выписан документ по этой дате
        sprintf(strsql,"select nomd from Upldok where datd='%d-%d-%d' and ka=%d and kv=%d",
        gv,mv,dv,atoi(VV.VVOD_SPISOK_return_data(3)),atoi(VV.VVOD_SPISOK_return_data(4)));
        if(readkey(strsql,&row,&curr) > 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));
          VVOD SOOB(1);

          sprintf(strsql,"%s !",gettext("Для этого водителя и этого автомобиля уже введён документ"));
          SOOB.VVOD_SPISOK_add_MD(strsql);

          sprintf(strsql,"%s %d.%d.%d %s %s",gettext("Дата"),dv,mv,gv,
          gettext("Номер документа"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);

          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto nazad;
         }
       }
     }


    if(N == 7 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата выезда
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),1) != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введена дата выезда !"));
	goto nazad;
       }
     }

    if(N == 8 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Время выезда
     {
      if(rstime(&has,&min,&sek,VV.VVOD_SPISOK_return_data(N)) != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введено время выезда !"));
	goto nazad;
       }
     }

    if(N == 9 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Дата возвращения
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),1) != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введена дата возвращения !"));
	goto nazad;
       }
     }

    if(N == 10 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Время возвращения
     {
      if(rstime(&has,&min,&sek,VV.VVOD_SPISOK_return_data(N)) != '\0')
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введено время возвращения !"));
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    GDITE();
    iceb_t_pdoc("upl2_1.txt");
    goto nazad;
   }

  if(K == PLU || K == FK2) /*Продолжать*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(iceb_t_pbpds(mv,gv) != 0)
     goto nazad;

    break;
   }

  if(K == FK3) /*Посмотреть реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
       goto nazad;


      case 0:
        kod.new_plus("");
        data.naimpod.new_plus("");
        if(vibrek("Uplpodr",&kod,&data.naimpod) == 0)
          VV.VVOD_SPISOK_zapis_data(1,kod.ravno());
        break;

      case 1:
        clear();
        data.naimvod.new_plus("");
        if(dirout(&kodn,&data.naimvod,1,2) == 0)
          VV.VVOD_SPISOK_zapis_data(4,kodn.ravno());
        break;

      case 2:
        clear();
        data.naimavt.new_plus("");
        if(diravt0(&kodn,&data.naimavt,1) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
          goto kodavt;
         }
        break;

     }
    goto nazad;
   }  
 
  if(K == FK4) //Получить новый номер документа
   {
    char nomer_dok[64];
    uplnomdok(gv,VV.VVOD_SPISOK_return_data(1),nomer_dok);
    VV.data_plus(2,nomer_dok);
    goto nazad;
   }   
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));

if(vdupl1_zap(*dd,*md,*gd,nomd->ravno(),podr->ravno(),&VV,&data) != 0)
 goto nazad;

*dd=dv; 
*md=mv;
*gd=gv;
nomd->new_plus(VV.VVOD_SPISOK_return_data(2));
podr->new_plus(VV.VVOD_SPISOK_return_data(1));
return(0);
}

/*****************************/
/*Запись шапки документа     */
/*****************************/
//Если вернули 0-записали 1-нет

int vdupl1_zap(short dd,short md,short gd,const char *nomd,const char *podr,
VVOD *VV,class vdupld_data *data)
{
short dv=0,mv=0,gv=0; //Введенная дата документа

char  strsql[512];
SQL_str row;
SQLCURSOR curr;

if(rsdat(&dv,&mv,&gv,VV->VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  return(1);
 }


if(iceb_t_pbpds(mv,gv) != 0)
 return(1);
sprintf(strsql,"select naik from Uplpodr where kod=%s",VV->VVOD_SPISOK_return_data(1));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),VV->VVOD_SPISOK_return_data(1));
  iceb_t_soob(strsql);
  return(1);
 }

if(SRAV(nomd,VV->VVOD_SPISOK_return_data(2),0) != 0)
 {
  sprintf(strsql,"select datd from Upldok where god=%d and kp=%d and nomd='%s'",
  gv,atoi(VV->VVOD_SPISOK_return_data(1)),VV->VVOD_SPISOK_return_data(2));
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(2));
    
    iceb_t_soob(strsql);
    return(1);
   }
 }

sprintf(strsql,"select naik from Uplavt where kod=%s",VV->VVOD_SPISOK_return_data(3));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код автомобиля"),VV->VVOD_SPISOK_return_data(3));
  iceb_t_soob(strsql);
  return(1);
 }


sprintf(strsql,"select naik from Uplouot where kod=%s",VV->VVOD_SPISOK_return_data(4));
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),VV->VVOD_SPISOK_return_data(4));
  iceb_t_soob(strsql);
  return(1);
 }
short dvz=0,mvz=0,gvz=0; //дата выезда
short hasvz=0,minvz=0,sekvz=0; //время выезда
short dvv=0,mvv=0,gvv=0; //дата возвращения
short hasvv=0,minvv=0,sekvv=0; //время возвращения

if(VV->VVOD_SPISOK_return_data(7)[0] != '\0')  //Дата выезда
 {
  if(rsdat(&dvz,&mvz,&gvz,VV->VVOD_SPISOK_return_data(7),1) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата выезда !"));
    return(1);
   }
 }

if(VV->VVOD_SPISOK_return_data(8)[0] != '\0')  //Время выезда
 {
  if(rstime(&hasvz,&minvz,&sekvz,VV->VVOD_SPISOK_return_data(8)) != 0)
   {
    iceb_t_soob(gettext("Не верно введено время выезда !"));
    return(1);
   }
 }

if(VV->VVOD_SPISOK_return_data(9)[0] != '\0')  //Дата возвращения
 {
  if(rsdat(&dvv,&mvv,&gvv,VV->VVOD_SPISOK_return_data(9),1) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата возвращения !"));
    return(1);
   }
 }

if(VV->VVOD_SPISOK_return_data(10)[0] != '\0')  //Время возвращения
 {
  if(rstime(&hasvv,&minvv,&sekvv,VV->VVOD_SPISOK_return_data(10)) != 0)
   {
    iceb_t_soob(gettext("Не верно введено время возвращения !"));
    return(1);
   }
 }


time_t   vrem;
time(&vrem);

if(dd == 0) //Ввод нового документа
 {
  int nom_zap=0;
  //Определяем номер записи для документа
  sprintf(strsql,"select nz from Upldok where datd='%d-%d-%d' and ka=%d order by nz desc limit 1",
  gv,mv,dv,atoi(VV->VVOD_SPISOK_return_data(3)));
  if(readkey(strsql,&row,&curr) > 0)
   nom_zap=atoi(row[0])+1;

  sprintf(strsql,"insert into Upldok \
values ('%04d-%d-%d',\
%s,\
'%s',\
%d,\
'%s',\
%s,\
%s,\
%d,\
%d,\
'%04d-%d-%d',\
'%04d-%d-%d',\
'%02d:%02d:%02d',\
'%02d:%02d:%02d',\
0,0,\
'%s',\
%d,\
%ld,\
%d)",
  gv,mv,dv,
  VV->VVOD_SPISOK_return_data(1),
  VV->VVOD_SPISOK_return_data(2),
  gv,
  VV->data_ravno_filtr(12),
  VV->VVOD_SPISOK_return_data(3),
  VV->VVOD_SPISOK_return_data(4),
  atoi(VV->VVOD_SPISOK_return_data(5)),
  atoi(VV->VVOD_SPISOK_return_data(6)),
  gvz,mvz,dvz,
  gvv,mvv,dvv,
  hasvz,minvz,sekvz,
  hasvv,minvv,sekvv,
  VV->data_ravno_filtr(11),
  iceb_t_getuid(),
  vrem,
  nom_zap);
    
 }
else  //Корректировка старого документа 
 {

  //исправляем записи в таблице списания топлива по счетам и объектам
  sprintf(strsql,"update Upldok2a set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s' \
where datd='%d-%d-%d' and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(VV->VVOD_SPISOK_return_data(1)),
  VV->VVOD_SPISOK_return_data(2),
  gd,md,dd,atoi(podr),nomd);

  if(sql_zapis(strsql,0,0) != 0)
   return(1);

  //Исправлям записи в документе
  
  sprintf(strsql,"update Upldok1 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
ka=%s,\
kv=%s \
where datd='%d-%d-%d' and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(VV->VVOD_SPISOK_return_data(1)),
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(3),
  VV->VVOD_SPISOK_return_data(4),
  gd,md,dd,atoi(podr),nomd);

  if(sql_zapis(strsql,0,0) != 0)
   return(1);

  //Исправлям записи в документе
  
  sprintf(strsql,"update Upldok3 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s' \
where datd='%d-%d-%d' and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(VV->VVOD_SPISOK_return_data(1)),
  VV->VVOD_SPISOK_return_data(2),
  gd,md,dd,atoi(podr),nomd);

  if(sql_zapis(strsql,0,0) != 0)
   return(1);




  //Исправляем шапку документа

  sprintf(strsql,"update Upldok set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
god=%d,\
nomb='%s',\
ka=%s,\
kv=%s,\
psv=%d,\
psz=%d,\
denn='%04d-%02d-%02d',\
denk='%04d-%02d-%02d',\
vremn='%02d:%02d:%02d',\
vremk='%02d:%02d:%02d',\
otmet='%s',\
ktoz=%d,\
vrem=%ld \
where god=%d and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(VV->VVOD_SPISOK_return_data(1)),
  VV->VVOD_SPISOK_return_data(2),
  gv,
  VV->data_ravno_filtr(12),
  VV->VVOD_SPISOK_return_data(3),
  VV->VVOD_SPISOK_return_data(4),
  VV->data_ravno_atoi(5),
  VV->data_ravno_atoi(6),
  gvz,mvz,dvz,
  gvv,mvv,dvv,
  hasvz,minvz,sekvz,
  hasvv,minvv,sekvv,
  VV->data_ravno_filtr(11),
  iceb_t_getuid(),vrem,
  gd,
  atoi(podr),
  nomd);
  

  
 }

//printw("\n%s\n",strsql);
//OSTANOV();
 
if(sql_zapis(strsql,0,0) != 0)
 return(1);

return(0);

}
/******************************************************************/
/*Поиск предыдущего путевого листа и возврат показания спидометра*/
/******************************************************************/

int	vdupl1_poippl(short dd,short md,short gd,const char *kodavt)
{
int	poksp=0;
char	strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select psz from Upldok where datd <= '%d-%d-%d' and ka=%s and psz != 0 order by datd desc,nz desc limit 1",
gd,md,dd,kodavt);

if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 poksp=atoi(row[0]);

return(poksp);

}
