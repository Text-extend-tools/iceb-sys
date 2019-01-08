/* $Id: startxbu.c,v 5.62 2014/08/31 06:18:18 sasa Exp $ */
/*12.03.2018    25.06.1999  Белых А.И.  startxbu.c
Главнаяя программа. Запускает все программы из которых
состоит бухгалтерский учет
*/
#include    "../iceb.h"
#define  DVERSIQ "12.03.2018"
#include        <pwd.h>
#include        "buhg.h"

void startxbu_sb();
void optimbaz(void);
void startxbu_ins(int argc,char **argv);
class iceb_tu_str naime_kontr00("");

extern int  iscolor;
extern char *imabaz;
extern char *host;
extern char *parol_pol;
SQL_baza    bd;
short       startgodb;//для построения с разделяемыми библиотерами
short       vplsh;//для построения с разделяемыми библиотерами


int main(int argc,char **argv,char **envp)
{
    short           ddd,mmm,ggg; /*Месяц год*/
    int     kom;
    char        strsql[1024];
    SQLCURSOR       cur;
    short       x,y;
    time_t      tmm;
    struct  tm      *bf;
    int uid=0;
    class iceb_tu_str fioop;

    icebstart(argc,argv);

    if(setenv("PAROLICEB",parol_pol,1) != 0)
     {
      printw("\n%s!\n",gettext("Ошибка установки переменной пароля"));
      OSTANOV();
     }
    uid=iceb_t_getuid();
    fioop.plus(iceb_t_getfioop());

    /*Ищем настройку*/
    if(nastsys() != 0)
     exit_iceb(1);

    iceb_t_perzap(1);  /*Проверка на первичный запуск в текущем дне и удаление *.lst */
    naime_kontr00.new_plus(iceb_t_get_pnk("00",0));
    sql_closebaz(&bd);

    time(&tmm);
    bf=localtime(&tmm);
    ddd=bf->tm_mday;
    mmm=bf->tm_mon+1;
    ggg=bf->tm_year+1900;

    VVOD MENU(1);

    MENU.VVOD_SPISOK_add_MD(gettext("Главная книга"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Материальный учет"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Платежные документы"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Заработная плата"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Учет основных средств"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Учет услуг"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Учет кассовых ордеров"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Учет командировочных расходов"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Учет путевых листов"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Реестр налоговых накладных"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Учёт доверенностей"));//10
    MENU.VVOD_SPISOK_add_MD(gettext("Инстументы"));//11
    MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//12

    kom = 0;

    for(;;)
     {

      MENU.VVOD_delete_ZAG();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Бухгалтерский учёт"));
    //  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

      sprintf(strsql,"%.*s",iceb_tu_kolbait(40,naime_kontr00.ravno()),naime_kontr00.ravno());
      MENU.VVOD_SPISOK_add_ZAG(strsql);

      clear();
      znak(0,COLS-25);

      infosys(y=12,x=COLS-27,host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,uid,fioop.ravno());

      if(menu3(&MENU,&kom,1,0,0) != 0)
       continue;
       
      switch (kom)
       {
        case 12 :
        case -1 :
          refresh();
          endwin();
          return(0);

        case 0 :
          sprintf(strsql,"iceb_xbu -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 1 :
          sprintf(strsql,"iceb_xmu -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 2 :
          sprintf(strsql,"iceb_xdk -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 3 :
          sprintf(strsql,"iceb_xzp -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 4 :
          sprintf(strsql,"iceb_xuos -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 5 :
          sprintf(strsql,"iceb_xusl -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 6 :
          sprintf(strsql,"iceb_xkas -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 7 :
          sprintf(strsql,"iceb_xukr -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 8 :
          sprintf(strsql,"iceb_xupl -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 9 :
          sprintf(strsql,"iceb_xrnn -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 10:
          sprintf(strsql,"iceb_xud -h %s -b %s",host,imabaz);
          clear();
          refresh();
          if(runsystem(strsql) != 0)
            OSTANOV();
          break;

        case 11:
          startxbu_ins(argc,argv);
          break;

        default:
           kom=0;
           break;
       }
    }

}
/***********************/
/*Оптимизация базы данных*/
/**************************/
void startxbu_ob(int argc,char **argv)
{

    icebstart(argc,argv);


    optimbaz();

    sql_closebaz(&bd);
}
/***********************/
/*личные настройки*/
/**************************/
void startxbu_lno(int argc,char **argv)
{

    icebstart(argc,argv);


    iceb_t_lnom();

    sql_closebaz(&bd);
}
/***********************/
/*Проверка физической целосноти базы*/
/**************************/
void startxbu_check_tables(int argc,char **argv)
{

    icebstart(argc,argv);

    class spis_oth oth;   

    if(check_table(&oth,imabaz) == 0)
     iceb_t_rabfil(&oth,"");


    sql_closebaz(&bd);
}
/***********************/
/*Восстановелние таблиц*/
/**************************/
void startxbu_repair_tables(int argc,char **argv)
{

    icebstart(argc,argv);

    class spis_oth oth;   

    if(repair_table(&oth,imabaz) == 0)
     iceb_t_rabfil(&oth,"");

    sql_closebaz(&bd);
}
/********************************/
/*Смена базы*/
/********************/
void startxbu_sb()
{
    struct  passwd  *ktor; /*Кто работает*/
    ktor=getpwuid(getuid());

    /*открываем базу для того чтобы прочитать список баз данных*/
    if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol_pol) != 0)
      eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

    icebstart_rf(); //выполняем команды после подключения

    smenabaz(1); /*Закрываем старую базу и открываем новую*/

    naime_kontr00.new_plus(iceb_t_get_pnk("00",0));

    sql_closebaz(&bd);
}

/*****************************************/
/*Физическая целосность базы*/
/******************************************/
void startxbu_check_repair(int argc,char **argv)
{
    int kom=0;
    VVOD MENU(3);

    MENU.VVOD_SPISOK_add_ZAG(gettext("Инстументы"));
    char strsql[512];
    sprintf(strsql,"%.*s",iceb_tu_kolbait(40,naime_kontr00.ravno()),naime_kontr00.ravno());
    MENU.VVOD_SPISOK_add_ZAG(strsql);

    MENU.VVOD_SPISOK_add_MD(gettext("Проверка физической целосности таблиц базы данных"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Восстановление таблиц базы данных"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clear();

    if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
     return;

    switch (kom)
    {
      case 2:
      case -1 :
        return;

       case 0:
         startxbu_check_tables(argc,argv);
         break;

       case 1:
         startxbu_repair_tables(argc,argv);
         break;
             
      default:
         kom=0;
         break;

    }


}

/*****************************************/
/*Инструменты*/
/******************************************/
void startxbu_ins(int argc,char **argv)
{
    int kom=0;
    VVOD MENU(3);

    MENU.VVOD_SPISOK_add_ZAG(gettext("Инстументы"));
    char strsql[512];
    sprintf(strsql,"%.*s",iceb_tu_kolbait(40,naime_kontr00.ravno()),naime_kontr00.ravno());
    MENU.VVOD_SPISOK_add_ZAG(strsql);


    MENU.VVOD_SPISOK_add_MD(gettext("Смена базы"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Оптимизация базы данных"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выгрузить дамп текущей базы данных"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Администрирование баз данных"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Проверка физической целосности таблиц базы данных"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Личные настройки оператора"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//8

    for(;;)
    {
      clear();

      if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
       return;
      class spis_oth oth;   
      switch (kom)
      {
        case 8:
        case -1 :
          return;

        case 0:
          GDITE();

          startxbu_sb();
          break;      

        case 1:
           mprintcap();
           break;

        case 2:
           startxbu_ob(argc,argv);
           break;

        case 3:
           if(iceb_t_menudpr(1) != 0)
            {
             iceb_t_soob(gettext("Не найдена настройка для дополнительных программ! Файл:./.iceB/menu_dpr.alx"));
            }
           break;

         case 4:

           icebstart(argc,argv);

           if(iceb_t_prov_vdb(imabaz) != 0)
            {
             sql_closebaz(&bd);
             break;
            }
            sql_closebaz(&bd);

           iceb_t_dumpbazout(imabaz,iceb_tu_getlogin(),parol_pol,host);
           break;

         case 5:
           clear();
           refresh();
           if(runsystem("iceb_admint") != 0)
              OSTANOV();
           break;

         case 6:
           startxbu_check_repair(argc,argv);
           break;
               
         case 7:
           startxbu_lno(argc,argv);
           break;
               
        default:
           kom=0;
           break;

       }
    }
}
