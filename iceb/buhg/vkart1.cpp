/* $Id: vkart1.c,v 5.31 2013/09/26 09:43:43 sasa Exp $ */
/*25.06.2013	22.09.1998	Белых А.И.	vkart1.c
Ввод дополнительных данных к карточке
*/
#include        "buhg.h"

void            zapis(long,int,int,char[]); /*vkartn.c*/
int             dirzar2n(short,char[],char[],short,short);
short		dirzar20n(short,char[],char[],short,short,short);
int dirzarkh(int metkar,int tabnom,class iceb_tu_str *kod_banka,class iceb_tu_str *kartshet);

extern short    mmm,ggg;

void vkart1(int tabn,const char *fio,class iceb_tu_str *adres,class iceb_tu_str *nomp,
class iceb_tu_str *vidan,class iceb_tu_str *telef,short *kodg,class iceb_tu_str *harac,class iceb_tu_str *inn,
class iceb_tu_str *bankshet,short *dv,short *mv,short *gv,class iceb_tu_str *tipkk,
short *drog,short *mrog,short *grog,
short *pol,
short *dppz,short *mppz,short *gppz,int *kodbank,
class iceb_tu_str *kodss)
{
int		N,K;
class iceb_tu_str kart_shet_old(bankshet->ravno());
class iceb_tu_str sod("");
class iceb_tu_str naimkod("");
class iceb_tu_str kodd("");
class iceb_tu_str nai("");
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
int		poz,komv,kold;
short		dr=*drog,mr=*mrog,gr=*grog; /*Дата рождения*/
char		bros[512];
char		datvd[24];

N=0;
VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Адрес...................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер паспорта..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Кем выдан паспорт........"));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи паспорта....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Телефон.................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Характер работы.........."));//5
VV.VVOD_SPISOK_add_MD(gettext("Город нал. инспекции....."));//6
VV.VVOD_SPISOK_add_MD(gettext("Идентификационный номер.."));//7
VV.VVOD_SPISOK_add_MD(gettext("Счёт в банке............."));//8
VV.VVOD_SPISOK_add_MD(gettext("Тип кредитной карты......"));//9
VV.VVOD_SPISOK_add_MD(gettext("Дата рождения............"));//10
VV.VVOD_SPISOK_add_MD(gettext("Пол......................"));//11
VV.VVOD_SPISOK_add_MD(gettext("Дата повышения з/п......."));//12
VV.VVOD_SPISOK_add_MD(gettext("Код банка................"));//13
VV.VVOD_SPISOK_add_MD(gettext("Код спецстажа дата номер."));//14

VV.VVOD_SPISOK_add_data(100);//0
VV.VVOD_SPISOK_add_data(30);//1
VV.VVOD_SPISOK_add_data(100);//2
VV.VVOD_SPISOK_add_data(11);//3
VV.VVOD_SPISOK_add_data(50);//4
VV.VVOD_SPISOK_add_data(50);//5
VV.VVOD_SPISOK_add_data(50);//6
VV.VVOD_SPISOK_add_data(11);//7
VV.VVOD_SPISOK_add_data(30);//8
VV.VVOD_SPISOK_add_data(30);//9

VV.VVOD_SPISOK_add_data(11);//10
VV.VVOD_SPISOK_add_data(30);//11
VV.VVOD_SPISOK_add_data(11);//12
VV.VVOD_SPISOK_add_data(50);//13
VV.VVOD_SPISOK_add_data(64);//14


VV.VVOD_SPISOK_zapis_data(0,adres->ravno());
VV.VVOD_SPISOK_zapis_data(1,nomp->ravno());
VV.VVOD_SPISOK_zapis_data(2,vidan->ravno());
memset(datvd,'\0',sizeof(datvd));
if(*gv != 0)
 sprintf(datvd,"%d.%d.%d",*dv,*mv,*gv);
VV.VVOD_SPISOK_zapis_data(3,datvd);
VV.VVOD_SPISOK_zapis_data(4,telef->ravno());
VV.VVOD_SPISOK_zapis_data(5,harac->ravno());
if(*kodg != 0)
 {
  sprintf(strsql,"select naik from Gnali where kod=%d",*kodg);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    printw("%s %d !\n",
    gettext("Не найден код города налоговой инспекции"),*kodg);
    OSTANOV();
   }  
  else
   VV.VVOD_SPISOK_zapis_data(6,row[0]);
 }

VV.VVOD_SPISOK_zapis_data(7,inn->ravno());
VV.VVOD_SPISOK_zapis_data(8,bankshet->ravno());
VV.VVOD_SPISOK_zapis_data(9,tipkk->ravno());
if(*grog != 0)
 {
  sprintf(strsql,"%d.%d.%d",*drog,*mrog,*grog);
  VV.VVOD_SPISOK_zapis_data(10,strsql);
 }
 
if(*gppz != 0)
 {
  sprintf(strsql,"%d.%d.%d",*dppz,*mppz,*gppz);
  VV.VVOD_SPISOK_zapis_data(12,strsql);
 }

naz:;

clear();

if(*pol == 0)
  VV.VVOD_SPISOK_zapis_data(11,gettext("Мужчина"));
if(*pol == 1)
  VV.VVOD_SPISOK_zapis_data(11,gettext("Женщина"));


if(*kodbank != 0)
 {
  sprintf(strsql,"select naik from Zarsb where kod=%d",*kodbank);
  if(readkey(strsql,&row,&cur) == 1)
   {
    sprintf(strsql,"%d %s",*kodbank,row[0]);
    VV.data_plus(13,strsql);
   }
  else
   *kodbank=0;
 }
else
  VV.data_plus(13,"");


VV.data_plus(14,kodss->ravno());


move(0,0);
printw("%d %s",tabn,fio);

if(gr != 0)
 {
  move(1,0);
  printw("%s%d",gettext("Возраст:"),ggg-gr);
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F3",gettext("город"),
"F4",gettext("карт.счета"),
"F10",gettext("выход"),NULL);



VV.vramka(2,0,0);





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

    if(N == 6)      /*Код города*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      sod.new_plus("");
      poz=komv=0;
      naimkod.new_plus("");
      if(dirmat1("Gnali",&sod,&naimkod,LINES-5,30,2,2,&poz,&komv,"","\0") == 0)
       {
        VV.data_plus(N,naimkod.ravno());
	*kodg=sod.ravno_atoi();
       }
      goto naz;
     }  

    if(N == 11)   //Пол человека
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      VVOD MENU(3);
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

      MENU.VVOD_SPISOK_add_MD(gettext("Мужчина"));
      MENU.VVOD_SPISOK_add_MD(gettext("Женщина"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход."));
      int kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      switch (kom1)
       {
        case  2 :
        case -1 :
         goto naz;

       }
      *pol=kom1;
      goto naz;
     }

    if(N == 13)      /*Код банка*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      class iceb_tu_str kod_b("");
      class iceb_tu_str naimb("");
      
      if(dirsb(2,&kod_b,&naimb) == 0)
       {
        VV.data_plus(N,naimb.ravno());
	*kodbank=kod_b.ravno_atoi();
        sprintf(strsql,"select nks from Zarkh where tn=%d and kb=%d",tabn,*kodbank);
        if(readkey(strsql,&row,&cur) == 1)
          VV.data_plus(8,row[0]);         
       }
      goto naz;
     }  
     
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 7 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')     //Идентификационный номер
     {
      if(strlen(VV.VVOD_SPISOK_return_data(N)) != 10)
       {
        attroff(VV.VVOD_return_cs(iceb_CFS));
        attron(VV.VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Количество знаков в идентификационном коде не равно 10 !"));
       }

      inn->new_plus(VV.VVOD_SPISOK_return_data(N));

      /*Проверяем*/
      sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        if(tabn != atol(row[0]))
         {
          attroff(VV.VVOD_return_cs(iceb_CFS));
          attron(VV.VVOD_return_cs(iceb_CFM));
          VVOD SOOB(1);
          SOOB.VVOD_SPISOK_add_MD(gettext("Такой индентификационный код уже введён !"));
          sprintf(strsql,"%s %s",row[0],row[1]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
         }
       }

      if(strlen(VV.VVOD_SPISOK_return_data(N)) == 10)
       {
        if(VV.VVOD_SPISOK_return_data(10)[0] == '\0')
         {
          sprintf(bros,"%5.5s",VV.VVOD_SPISOK_return_data(N));
          kold=atoi(bros);
          dr=0;
          mr=1;
          gr=1900;
          denrog(&dr,&mr,&gr,kold);
          sprintf(bros,"%d.%d.%d",dr,mr,gr);
          VV.VVOD_SPISOK_zapis_data(10,bros);

          *pol=0;
          if(VV.VVOD_SPISOK_return_data(N)[8] == '0' || VV.VVOD_SPISOK_return_data(N)[8] == '2' 
          || VV.VVOD_SPISOK_return_data(N)[8] == '4' || VV.VVOD_SPISOK_return_data(N)[8] == '6' || 
          VV.VVOD_SPISOK_return_data(N)[8] == '8')
            *pol=1;

         }                

       }

      N++;
      goto naz;
     }

    if(N == 8)      /*номер картсчёта*/
     {
      if(VV.data_ravno(N)[0] == '\0')
       {
        *kodbank=0;
       }
      goto naz;
     }  

    if(N == 10 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')     //Дата рождения
     {
      if(VV.VVOD_SPISOK_return_data(10)[0] != '\0')
       if(rsdat(drog,mrog,grog,VV.VVOD_SPISOK_return_data(N),1) != 0)
        {
         attroff(VV.VVOD_return_cs(iceb_CFS));
         attron(VV.VVOD_return_cs(iceb_CFM));
         VVOD SOOB(1);
         SOOB.VVOD_SPISOK_add_MD(gettext("Не правильно введена дата рождения !"));
         soobshw(&SOOB,stdscr,-1,-1,0,1);
        }
      N++;
      goto naz;
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

  switch (K)
   {
    case ESC :
#ifndef M_UNIX
      K=getch();
#endif
      K=ESC;

    case FK10:
    case TAB:
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));

      adres->new_plus(VV.VVOD_SPISOK_return_data(0));
      nomp->new_plus(VV.VVOD_SPISOK_return_data(1));
      vidan->new_plus(VV.VVOD_SPISOK_return_data(2));
      strcpy(datvd,VV.VVOD_SPISOK_return_data(3));
      telef->new_plus(VV.VVOD_SPISOK_return_data(4));
      harac->new_plus(VV.VVOD_SPISOK_return_data(5));
      inn->new_plus(VV.VVOD_SPISOK_return_data(7));
      bankshet->new_plus(VV.VVOD_SPISOK_return_data(8));
      tipkk->new_plus(VV.VVOD_SPISOK_return_data(9));
      kodss->new_plus(VV.data_ravno(14));
                   
      *dv=*mv=*gv=0;
      if(datvd[0] != '\0')
       if(rsdat(dv,mv,gv,datvd,1) != 0)
        {
         attroff(VV.VVOD_return_cs(iceb_CFS));
         attron(VV.VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не правильно введена дата получения паспорта !"));
         goto naz;
        }

      *drog=*mrog=*grog=0;
      if(VV.VVOD_SPISOK_return_data(10)[0] != '\0')
       if(rsdat(drog,mrog,grog,VV.VVOD_SPISOK_return_data(10),1) != 0)
        {
         attroff(VV.VVOD_return_cs(iceb_CFS));
         attron(VV.VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не правильно введена дата рождения !"));
         goto naz;
        }

      *dppz=*mppz=*gppz=0;
      if(VV.VVOD_SPISOK_return_data(12)[0] != '\0')
       if(rsdat(dppz,mppz,gppz,VV.VVOD_SPISOK_return_data(12),1) != 0)
        {
         attroff(VV.VVOD_return_cs(iceb_CFS));
         attron(VV.VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не правильно введена дата повышения зарплаты !"));
         goto naz;
        }
       
      /*Проверяем*/
      if(inn->ravno()[0] != '\0')
       {
        sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",inn->ravno());
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          if(tabn != atol(row[0]))
           {
            attroff(VV.VVOD_return_cs(iceb_CFS));
            attron(VV.VVOD_return_cs(iceb_CFM));

            VVOD SOOB(1);
            SOOB.VVOD_SPISOK_add_MD(gettext("Такой индентификационный код уже введён !"));
            sprintf(strsql,"%s %s",row[0],row[1]);
            SOOB.VVOD_SPISOK_add_MD(strsql);
            soobshw(&SOOB,stdscr,-1,-1,0,1);

            goto naz;
           }
         }
       }

      /*проверяем есть ли этот код банка в списке картсчетов*/
      if(*kodbank != 0)
       {
        sprintf(strsql,"select kb from Zarkh where tn=%d and kb=%d",tabn,*kodbank);
        if(readkey(strsql) == 0)
         {
          sprintf(strsql,"insert into Zarkh values(%d,%d,'%s',%d,%ld)",tabn,*kodbank,bankshet->ravno(),iceb_t_getuid(),time(NULL));
          sql_zapis(strsql,0,0);
         }       
        else
         if(SRAV(bankshet->ravno(),kart_shet_old.ravno(),0) != 0)
          if(bankshet->ravno()[0] != '\0')
           {
            sprintf(strsql,"replace into Zarkh values(%d,%d,'%s',%d,%ld)",tabn,*kodbank,bankshet->ravno(),iceb_t_getuid(),time(NULL));
            sql_zapis(strsql,0,0);
           }
        }
      return;

    case FK1:  /*Просмотр подсказки*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      GDITE();
      iceb_t_pdoc("zarp2_3.txt");
      clear();
      goto naz;

    case FK3: /*Просмотр списка городов налоговой инспекции*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      clear();
      nai.new_plus("");
      poz=komv=0;
      if(dirmat1("Gnali",&kodd,&nai,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
       {
        VV.data_plus(7,nai.ravno());       
        goto naz;
       }
      goto naz;

    case FK4: /*Просмотр списка картсчетов*/
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      clear();
      if(dirzarkh(1,tabn,&kodd,&nai) == 0)
       {
        VV.data_plus(8,nai.ravno());
        *kodbank=kodd.ravno_atoi(); 
       }

      goto naz;
   } 
 }

}
