/* $Id: vprov.c,v 5.55 2013/12/31 11:48:11 sasa Exp $ */
/*32.12.2017     22.02.1992      Белых А.И.      vprov.c
Подпрограмма выполнения проводок в главной книге
*/
#include        <math.h>
#include        "buhg.h"

void vprovnbs(const char*,OPSHET*);
int vprov_dnk(short *dn,short *mn,short *gn,short *dk,short *mk,short *gk);
void vprov_pros_prov(const char *shet);
void vprov_rvp(const char *shet,time_t start_v,int metka_ras_shet);


void            vprov()
{
time_t		vrem;
class iceb_tu_str nsh("");
class iceb_tu_str nai("");
class iceb_tu_str nai1("");
class iceb_tu_str naimsh("");
class iceb_tu_str nshk1("");
class iceb_tu_str komen1("");
class iceb_tu_str dkk("");

class iceb_tu_str kodo("");
class iceb_tu_str kodo1("");
short           tish;
short           i;
short           msrs,msrs1; /*Метка счета с развернутым сальдо*/
int             N=0,K;
short           d,m,g;
static class iceb_tu_str dat1("");
double		deb,kre;
class iceb_tu_str kto("");
char		strsql[2048];
double suma_deb=0.;
double suma_kre=0.;
SQL_str         row;
SQLCURSOR       cur;
short		saldo; //0-свернутое 3-развернутое
struct OPSHET	shetv;
static short metka_ras_shet; //Если равно 0-расчёт по счёту делать не надо 1- надо
static short dn,mn,gn; //Дата начала расчёта оборота по счёту
static short dk,mk,gk; //Дата конца расчёта оборота по счёту
time_t start_v=0;

int kekv=0;

N=msrs=msrs1=0;

VVOD VVOD1(2);

naz:;

clear(); /*Очистить экран и закрасить фоновым цветом*/
helstr(LINES-1,0,"F10",gettext("выход"),NULL);

nsh.new_plus("");

VVOD1.VVOD_delete();
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Ввод проводок"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер счета (субсчета)"));
if(vvod1(&nsh,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;

GDITE();

if(nsh.getdlinna() <= 1)
 {
  nsh.new_plus("");
  while(dirshet(&nsh,&naimsh,1) != 0)
   {
    clearstr(0,0);
    return;
   }
  clear();
  
 }

if(prsh1(nsh.ravno(),&shetv) != 0)
  goto naz;

naimsh.new_plus(shetv.naim);
tish=shetv.tips;
saldo=shetv.saldo;
if(shetv.stat == 1)
 {
  vprovnbs(nsh.ravno(),&shetv);
  return;
 }
clear();
start_v=time(NULL);
VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Кредит......"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дебет......."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт к-нт..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата........"));//3
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.."));//4
VV.VVOD_SPISOK_add_MD(gettext("КЭКЗ........"));//5

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(100);
VV.VVOD_SPISOK_add_data(6);

nai.new_plus("");
nai1.new_plus("");


VV.VVOD_SPISOK_zapis_data(3,dat1.ravno());

NAZ:;
double deb_kre[6];

for(;;)
 {
  clear();
  VV.VVOD_SPISOK_zapis_data(3,dat1.ravno());
  move(LINES-1,0);


  move(0,0);
  printw(gettext("Выполнение проводок для счёта %s"),nsh.ravno());
  move(1,0);
  if(tish == 0)
    printw(gettext("Счёт активный"));
  if(tish == 1)
    printw(gettext("Счёт пассивный"));
  if(tish == 2)
    printw(gettext("Счёт активнo-пассивный"));
  if(saldo == 3)
   {
    printw("  %s",gettext("Р/с"));
    msrs=1;
   }

  printw("\n%s\n",naimsh.ravno());

  short    dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  printw("%s %d.%d.%d%s",gettext("Текущая дата"),dt,mt,gt,gettext("г."));
  printw(" %s:%.2f/%.2f",gettext("Сумма выполненных проводок"),suma_deb,suma_kre);
  
  if(metka_ras_shet == 1)
   {
    GDITE();
    memset(deb_kre,'\0',sizeof(deb_kre));
    if(saldo != 3)
     rpshet_ss(nsh.ravno(),dn,mn,gn,dk,mk,gk,deb_kre);
    else
     rpshet_rs(nsh.ravno(),dn,mn,gn,dk,mk,gk,deb_kre);
    
    sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);
    mvprintw(14,0,"%-*s:%15s",iceb_tu_kolbait(20,strsql),strsql,prnbr(deb_kre[0]));
    printw(" %15s",prnbr(deb_kre[1]));
    
    mvprintw(15,0,"%-*s:%15s",iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(deb_kre[2]));
    printw(" %15s",prnbr(deb_kre[3]));
    
    sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);
    mvprintw(16,0,"%-*s:%15s",iceb_tu_kolbait(20,strsql),strsql,prnbr(deb_kre[4]));
    printw(" %15s",prnbr(deb_kre[5]));
   }     
  
  vprov_rvp(nsh.ravno(),start_v,metka_ras_shet);

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("счёт"),
  "F4",gettext("просм.проводок"),
  "F5",gettext("блок.пр."),
  "F6",gettext("пред."),
  "F7",gettext("сальдо"),
  "F8",gettext("комментарий"),
  "F10",gettext("выход"),NULL);

  int PY=5;
  int PX=0; /*Координаты вывода меню*/

  VV.vramka(5,0,0);



  for(;;)
   {
    attron(VV.VVOD_return_cs(iceb_CFS)); 
    VV.vprintw(N,stdscr);
    

    K=getch();

    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);

    if(K == KEY_RESIZE)
      break;

    if(K == KEY_MOUSE)
      K=mouseklic(&N,PY,PX,VV.MDLS,VV.KLST,stdscr);

    if(K == ENTER)
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CFV));
      VV.vprintw_clear(N,stdscr);


      if(N == 0)
       {
        VV.vgetstr(N,stdscr);
       }
      if(N == 1)
       {
        VV.vgetstr(N,stdscr);
       }

      if(N == 2)
       {
        VV.vgetstr(N,stdscr);
        if(VV.VVOD_SPISOK_return_data(N)[0] != '\0') 
         {
            attroff(VV.VVOD_return_cs(iceb_CFV));
  	  if(prsh1(VV.VVOD_SPISOK_return_data(N),&shetv) != 0)
           {
  	    break;
           }
          if(shetv.stat == 1)
           {
            sprintf(strsql,gettext("Счёт %s внебалансовый !"),VV.VVOD_SPISOK_return_data(N));
            iceb_t_soob(strsql);
            break;
           }
         }
       }

      if(N == 3)
       {
        VV.vgetstr(N,stdscr);
	dat1.new_plus(VV.VVOD_SPISOK_return_data(N));
        if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          attron(VV.VVOD_return_cs(iceb_CB));
          iceb_t_soob(gettext("Не верно введена дата проводки !"));
          break;
         }
       }
      if(N == 4)
       {
        VV.vgetstr(N,stdscr);
       }

      if(N == 5)
       {
        VV.vgetstr(N,stdscr);
       }

        attroff(VV.VVOD_return_cs(iceb_CFV));
        attron(VV.VVOD_return_cs(iceb_CFM));
      VV.vprintw(N,stdscr);

      if(N == 0)
       N++;

      N++;
     }

    if(K==SDN) /*Стрелка вниз*/
      N++;
    if(K==SUP) /*Стрелка вверх*/
      N--;
    if(N > VV.KLST-1)
      N=0;
    if(N < 0)
      N=VV.KLST-1;

   if(K == ESC || K == FK10)
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CB));
     return;
    }
   if(K == PLU || K == FK2) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
  GDITE();
     if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(3),0) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата проводки !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       iceb_t_soob(gettext("Не веден счёт корреспондент !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена сумма !"));
       break;
      }
     if(iceb_t_pvglkni(m,g) != 0)
      goto NAZ;


     deb=ATOFM(VV.VVOD_SPISOK_return_data(1));
     kre=ATOFM(VV.VVOD_SPISOK_return_data(0));
     if(fabs(deb) < 0.01 && fabs(kre) < 0.01)
      {
       iceb_t_soob(gettext("Не введён ни дебет ни кредит !"));
       break;
      }

     if(fabs(deb) > 0.009  && fabs(kre) > 0.009)
      {
       iceb_t_soob(gettext("Введен и дебет и кредит !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введён кореспондирующий счёт !"));
       break;
      }
     
     if(prsh1(VV.VVOD_SPISOK_return_data(2),&shetv) != 0)
      break;

     if(shetv.stat == 1)
      {
       sprintf(strsql,gettext("Счёт %s внебалансовый !"),VV.VVOD_SPISOK_return_data(2));
       iceb_t_soob(strsql);
       break;
      }
     
     if(iceb_t_pbsh(m,g,VV.data_ravno(2),nsh.ravno(),gettext("Невозможно записать проводку!")) != 0) /*Проверка блокировки счетов*/
      break;
      
     msrs1=0;
     if(shetv.saldo == 3)
       msrs1=1;

     /*Код организации для главного счета*/
     kodo.new_plus("");
     if(msrs == 1)
      {
iii:;
       helstr(LINES-1,0,"Enter",gettext("контрагенты"),
       "F10",gettext("выход"),NULL);

       dkk.new_plus("");
       VVOD1.VVOD_delete();
       sprintf(strsql,gettext("Введите код контрагента для счёта %s"),nsh.ravno());
       VVOD1.VVOD_SPISOK_add_MD(strsql);
       if((i=vvod1(&dkk,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        {
         clear();
         break;
        }

       if(i == ESC)
        {
         clear();
         break;
        }

       GDITE();

       if(dkk.ravno()[0] == '\0' || dkk.ravno()[0] == '+')
        {
         if(dkk.ravno()[0] == '+')
           nai.new_plus(iceb_tu_adrsimv(1,dkk.ravno()));
         else
           nai.new_plus(dkk.ravno());
         if(dirskont(nsh.ravno(),&kodo,&nai,1) != 0)
           break;
        }
       else
        kodo.new_plus(dkk.ravno());


       sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",nsh.ravno(),kodo.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),kodo.ravno(),nsh.ravno());
         iceb_t_soob(strsql);
         goto iii;
        }

       sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodo.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),kodo.ravno());
         iceb_t_soob(strsql);

         goto iii;
        }
       nai.new_plus(row[0]);
      }

     /*Код организации для субсчета*/

     kodo1.new_plus("");
     if(msrs1 == 1)
      {
iii1:;
       helstr(LINES-1,0,"Enter",gettext("контрагенты"),
       "F10",gettext("выход"),NULL);

       dkk.new_plus("");
       VVOD1.VVOD_delete();
       sprintf(strsql,gettext("Введите код контрагента для счёта %s"),VV.VVOD_SPISOK_return_data(2));
       VVOD1.VVOD_SPISOK_add_MD(strsql);
       if((i=vvod1(&dkk,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        {
         clear();
         break;
        }

       if(i == ESC)
        {
         clear();
         break;
        }

       GDITE();
       if(dkk.ravno()[0] == '\0' || dkk.ravno()[0] == '+')
        {
         if(dkk.ravno()[0] == '+')
           nai1.new_plus(iceb_tu_adrsimv(1,dkk.ravno()));
         else
           nai1.new_plus(dkk.ravno());
         if(dirskont(VV.VVOD_SPISOK_return_data(2),&kodo1,&nai1,1) != 0)
           break;
        }
       else
        kodo1.new_plus(dkk.ravno());

       sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV.VVOD_SPISOK_return_data(2),kodo1.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),kodo1.ravno(),VV.VVOD_SPISOK_return_data(2));
         iceb_t_soob(strsql);
         goto iii1;
        }

       sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodo1.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),kodo1.ravno());
         iceb_t_soob(strsql);
         goto iii1;
        }
       nai1.new_plus(row[0]);
      }
     else
       nai1.new_plus("");

     GDITE();
     if(msrs == 0)
       nai.new_plus("");

     rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(3),0);
     deb=ATOFM(VV.VVOD_SPISOK_return_data(1));
     kre=ATOFM(VV.VVOD_SPISOK_return_data(0));
     deb=okrug(deb,0.01);
     kre=okrug(kre,0.01);
     if(kodo.ravno()[0] == '\0' && kodo1.ravno()[0] != '\0')
       kodo.new_plus(kodo1.ravno());
     if(kodo1.ravno()[0] == '\0' && kodo.ravno()[0] != '\0')
       kodo1.new_plus(kodo.ravno());
       
     komen1.new_plus(VV.VVOD_SPISOK_return_data(4));

     time(&vrem);
     kekv=VV.data_ravno_atoi(5);
     suma_deb+=deb;
     suma_kre+=kre;
     zapprov(0,g,m,d,nsh.ravno(),VV.VVOD_SPISOK_return_data(2),kodo.ravno(),kodo1.ravno(),kto.ravno(),"","",deb,kre,komen1.ravno_filtr(),2,0,vrem,0,0,0,0,kekv);

     nshk1.new_plus(VV.VVOD_SPISOK_return_data(2));

     
     VV.VVOD_SPISOK_zapis_data(0,"");
     VV.VVOD_SPISOK_zapis_data(1,"");
     VV.VVOD_SPISOK_zapis_data(2,"");
     VV.VVOD_SPISOK_zapis_data(4,"");
     VV.VVOD_SPISOK_zapis_data(5,"");
     
     attroff(A_REVERSE);  /*Выключить инверсию*/
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     N=0;
     break;
    }
   if(K == FK1) /*Помощ*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     GDITE();

     iceb_t_pdoc("buhg3_1.txt");

     //clear();
     break;
    }
   if(K == FK3) /*Смотрим план счетов*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     class iceb_tu_str nomshet("");
     class iceb_tu_str naimnomshet("");
     if(dirshet(&nomshet,&naimnomshet,1) == 0)
      {
       VV.VVOD_SPISOK_zapis_data(2,nomshet.ravno());
      }

     break;
    }

   if(K == FK4) /*Смотрим проводки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     provtime();
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     break;
    }

   if(K == SFK4) /*Смотрим проводки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     vprov_pros_prov(nsh.ravno());
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     break;
    }

   if(K == FK5) /*Ввод блокировок*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     iceb_tl_blok();
//     iceb_redfil("blok.alx",0);

     clear();
     break;
    }
   if(K == FK6) //заполнение меню предыдущими реквизитами
    {
     VV.VVOD_SPISOK_zapis_data(2,nshk1.ravno());
     VV.VVOD_SPISOK_zapis_data(4,komen1.ravno());
     VV.data_plus(5,kekv);
     break;
    }
   if(K == FK7) //заполнение меню дат расчёта
    {
     metka_ras_shet=vprov_dnk(&dn,&mn,&gn,&dk,&mk,&gk);
     break;
    }

   if(K == SFK8) //настройка списка комментариев
     {
       attroff(VV.VVOD_return_cs(iceb_CFS));
       attron(VV.VVOD_return_cs(iceb_CFM));
       iceb_redfil("spis_koment_gl.alx",0);
     break;
     }
   if(K == FK8) //Выбрать комментарий из списка
     {
       attroff(VV.VVOD_return_cs(iceb_CFS));
       attron(VV.VVOD_return_cs(iceb_CFM));
       class iceb_tu_str koment_sp("");
       if(spis_koment(&koment_sp,"spis_koment_gl.alx") == 0)
         VV.data_strcat(4,koment_sp.ravno());
       clear(); //экран не очищается перед первичным выводом меню чтобы были видны реквизиты документа 
     break;
        
     }



   attroff(VV.VVOD_return_cs(iceb_CFS));
   attron(VV.VVOD_return_cs(iceb_CFM));
   attron(VV.VVOD_return_cs(iceb_CB));
  }
 }
}

/********************************/
/*Задание диапазона дат для расчёта сальдо по счёту */
/*****************************/
int vprov_dnk(short *dn,short *mn,short *gn,
short *dk,short *mk,short *gk)
{
char dat1[32];
char dat2[32];
memset(dat1,'\0',sizeof(dat1));
memset(dat2,'\0',sizeof(dat2));


if(*dn != 0)
 sprintf(dat1,"%d.%d.%d",*dn,*mn,*gn);
if(*dk != 0)
 sprintf(dat2,"%d.%d.%d",*dk,*mk,*gk);
 

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите даты для расчёта оборота по счёту"));

VV.VVOD_SPISOK_add_data(dat1,11);
VV.VVOD_SPISOK_add_data(dat2,11); 

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

naz:;

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

strcpy(dat1,VV.VVOD_SPISOK_return_data(0));
strcpy(dat2,VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(0);

  case FK2:
  case PLU:
    break;
    
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(rsdatp(dn,mn,gn,dat1,dk,mk,gk,dat2) != 0)
 goto naz;
return(1);

}
/************************************************/
/*Просмотр проводок по счёту за период*/
/***************************************/

void vprov_pros_prov(const char *shet)
{

static char dat1[11];
static char dat2[11];
 

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите даты для просмотра проводок по счёту"));

VV.VVOD_SPISOK_add_data(dat1,sizeof(dat1));
VV.VVOD_SPISOK_add_data(dat2,sizeof(dat2)); 

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

naz:;

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

strcpy(dat1,VV.VVOD_SPISOK_return_data(0));
strcpy(dat2,VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
    break;
    
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1,&dk,&mk,&gk,dat2) != 0)
 goto naz;

char strsql[512];
sprintf(strsql,"select * from Provk where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh='%s' order by datp asc",gn,mn,dn,gk,mk,dk,shet);

class poiprov_rek poi;

poi.zapros.new_plus(strsql);

dirprov(&poi);



}
/******************************/
/*поиск и показ только что выполненых проводок*/
/***********************************************/
void vprov_rvp(const char *shet,time_t start_v,int metka_ras_shet)
{
char strsql[2048];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(metka_ras_shet == 1) /*вывод оборота по счёту*/
 {
  if(LINES-22 <= 0)
   return;
  move(18,0);
 }
else /*вывода нет*/
 {
  if(LINES-18 <= 0)
   return;
  move(14,0);
 } 

sprintf(strsql,"select datp,shk,kodkon,deb,kre,komen,kekv from Prov where vrem >= %ld and ktoi=%d and sh='%s' order by vrem desc limit %d",start_v,iceb_t_getuid(),shet,LINES-22);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }


class iceb_tu_str liniq;

for(int nom=0; nom < COLS-2; nom++)
 liniq.plus("-");

printw("%s\n",liniq.ravno());

printw(gettext("  Дата    |Счёт к.|  Дебет   |  Кредит  |    Контрагент      |КЭКЗ|Коментарий\n"));

printw("%s\n",liniq.ravno());

class iceb_tu_str naim_kontr("");
while(cur.read_cursor(&row) != 0)
 {
  naim_kontr.new_plus("");
  if(row[2][0] != '\0')
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[2]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      naim_kontr.new_plus(row[2]);
      naim_kontr.plus(" ",row1[0]);
     }
   }  

  sprintf(strsql,"%s %-*s %10s %10s %-*.*s %4s %s",
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(7,row[1]),
  row[1],
  row[3],
  row[4],
  iceb_tu_kolbait(20,naim_kontr.ravno()),
  iceb_tu_kolbait(20,naim_kontr.ravno()),
  naim_kontr.ravno(),
  row[6],
  row[5]);
      
  printw("%.*s\n",iceb_tu_kolbait(COLS-2,strsql),strsql);
 }
}
