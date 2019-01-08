/*$Id:$*/
/*16.04.2017	03.02.2015	Белых А.И.	impmatdok_xml_mt.c
Меню для ввода реквизитов 
*/
#include "buhg.h"

extern class iceb_tu_str spmatshets; /*Список материальных счетов*/

int impmatdok_xml_mt(class iceb_imp_xml_nn_r *rk)
{
char strsql[2048];
float pnds=iceb_t_pnds();
int konec=0;
int N=0;
int K=0;
class VVOD MENU(3);
class VVOD VV(0);
class iceb_tu_str repl("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int kom1=0;
rk->datdok.new_plus(poltekdat());


VV.VVOD_SPISOK_add_ZAG(gettext("Импорт налоговой накладной"));


repl.new_plus(gettext("Контагент"));
repl.plus(":");
repl.plus(rk->kontr_prod.ravno(),40);
repl.plus(" ",rk->naim_prod.ravno());

VV.VVOD_SPISOK_add_ZAG(repl.ravno());

repl.new_plus(gettext("Код ЕГРПОУ"));
repl.plus(":",rk->edrpuo_prod.ravno());

VV.VVOD_SPISOK_add_ZAG(repl.ravno());

repl.new_plus(gettext("Индивидуальный налоговый номер"));
repl.plus(":",rk->inn_prod.ravno());

VV.VVOD_SPISOK_add_ZAG(repl.ravno());

repl.new_plus(gettext("Номер налоговой накладной"));
repl.plus(":",rk->nom_nn.ravno());

VV.VVOD_SPISOK_add_ZAG(repl.ravno());

repl.new_plus(gettext("Дата налоговой накладной"));
repl.plus(":",rk->data_vnn.ravno());

VV.VVOD_SPISOK_add_ZAG(repl.ravno());

VV.VVOD_SPISOK_add_data(rk->datdok.ravno(),11);
VV.VVOD_SPISOK_add_data(rk->sklad.ravno(),10);
VV.VVOD_SPISOK_add_data(rk->nomdok.ravno(),10);
VV.VVOD_SPISOK_add_data(rk->kod_op.ravno(),10);
VV.VVOD_SPISOK_add_data(rk->shetu.ravno(),10);

sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
VV.VVOD_SPISOK_add_data(strsql,64);

VV.VVOD_SPISOK_add_MD(gettext("Дата....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Склад............"));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код операции....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта......."));//4
VV.VVOD_SPISOK_add_MD(gettext("НДС.............."));//5

naz:;


//clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F5",gettext("номер док."),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


while(konec == 0)
 {
  if(N > VV.KLST-1)
    N=0;

  if(N < 0)
    N=VV.KLST-1;


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

      if(N == 5)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        MENU.VVOD_delete();
        sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
        MENU.VVOD_SPISOK_add_MD(strsql);
        MENU.VVOD_SPISOK_add_MD(gettext("0% НДС реализация на територии Украины"));
        MENU.VVOD_SPISOK_add_MD(gettext("0% НДС экспорт"));
        MENU.VVOD_SPISOK_add_MD(gettext("Освобождение от НДС статья"));
        MENU.VVOD_SPISOK_add_MD(gettext("7% НДС на мед.препараты"));
        
        while(menu3w(stdscr,&MENU,&rk->nds,-1,-1,0) != 0);
        
        if(rk->nds == 0)
         {
          sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
          VV.VVOD_SPISOK_zapis_data(5,strsql);
         }
        if(rk->nds == 1)
          VV.VVOD_SPISOK_zapis_data(5,gettext("0% НДС реализация на територии Украины"));
        if(rk->nds == 2)
          VV.VVOD_SPISOK_zapis_data(5,gettext("0% НДС экспорт"));
        if(rk->nds == 3)
          VV.VVOD_SPISOK_zapis_data(5,gettext("Освобождение от НДС"));
        if(rk->nds == 4)
          VV.VVOD_SPISOK_zapis_data(5,gettext("7% НДС на мед.препараты"));
        goto naz;
       }

      VV.vgetstr(N,stdscr);
      
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*склад*/
       {
        if(VV.prov_dat(N,1) != 0)
         {
          iceb_t_soob(gettext("Не правильно введена дата!"));
          goto naz;
         }
       }

      if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*склад*/
       {
        sprintf(strsql,"select kod from Sklad where kod=%d",VV.data_ravno_atoi(N));
        if(readkey(strsql) != 1)
         {
          iceb_t_soob(gettext("Не найден код склада в справочнике!"));
          goto naz;
         }    
        if(VV.data_ravno(0)[0] != '\0')  
         {
          iceb_t_nomnak(VV.data_ravno(0),VV.data_ravno(N),&rk->nomdok,1,0,0);
          VV.data_plus(2,rk->nomdok.ravno());
         }        
        N++;
        goto naz;
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
//      iceb_t_pdoc(strsql);
      goto naz;

    case FK2:
    case PLU:
     konec=1;
     break;

    case FK3:
     MENU.VVOD_delete();
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//1
     MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//2
     MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

     clearstr(LINES-1,0);
     
     kom1=0;
     while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
     kod.new_plus("");
     naim.new_plus("");
     switch (kom1)
      {
       case 3:
       case -1:
        goto naz;


       case 0:
         if(dirsklad(1,&kod,&naim) == 0)
           VV.data_plus(1,kod.ravno());
         clear();
         goto naz;


       case 1:
         vibrek("Plansh",&rk->shetu);
         VV.VVOD_SPISOK_zapis_data(4,rk->shetu.ravno());
         goto naz;

       case 2:
        if(dirprihod(1,&kod,&naim) == 0)
         VV.data_plus(3,kod.ravno());
        goto naz;
      }
       
      goto naz;


    case FK4:
     VV.VVOD_clear_data();
     goto naz;

    case FK5:
     if(VV.data_ravno(0)[0] != '\0' || VV.data_ravno(1)[0] != '\0')  
      {
       iceb_t_nomnak(VV.data_ravno(0),VV.data_ravno(1),&rk->nomdok,1,0,0);
       VV.data_plus(2,rk->nomdok.ravno());
      }        
     goto naz;

    case FK10:
     return(1);

   }

 }

rk->datdok.new_plus(VV.data_ravno(0));
rk->sklad.new_plus(VV.data_ravno(1));
rk->nomdok.new_plus(VV.data_ravno(2));
rk->kod_op.new_plus(VV.data_ravno(3));
rk->shetu.new_plus(VV.data_ravno(4));

SQL_str row;
class SQLCURSOR cur;

if(rk->datdok.prov_dat() != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;;
 }
if(iceb_t_pbpds(rk->datdok.ravno()) != 0)
 goto naz;;
/*проверяем код склада*/
sprintf(strsql,"select kod from Sklad where kod=%d",rk->sklad.ravno_atoi());
if(readkey(strsql) != 1)
 {
  iceb_t_soob(gettext("Не найден код склада в справочнике!"));
  goto naz;
 }    

if(rk->nomdok.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён номер документа!"));
  goto naz;
  
 }
if(rk->kod_op.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён код операции!"));
  goto naz;
 }
/*проверяем код операции*/
sprintf(strsql,"select vido from Prihod where kod='%s'",rk->kod_op.ravno_filtr());
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найден код операции в справочнике!"));
  goto naz;
 }

if(atoi(row[0]) != 0)
 {
  iceb_t_soob(gettext("Не правильно выбрана операция! Тип операции должен быть \"Внешняя\""));
  goto naz;
 }

if(rk->shetu.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён счёт учёта материалов!"));
  goto naz;
 }

if(proverka(spmatshets.ravno(),rk->shetu.ravno(),1,1) != 0)
 {
  class iceb_tu_str repl;
  repl.plus(gettext("Счёт не материальный !"));
  repl.ps_plus(rk->shetu.ravno());
  iceb_t_soob(repl.ravno());
  goto naz;;
 }

if(prsh1(rk->shetu.ravno()) != 0)
 goto naz;

short dd=0,md=0,gd=0;
rsdat(&dd,&md,&gd,rk->datdok.ravno(),1);
short dk=dd;
dpm(&dk,&md,&gd,5);

/*проверяем может накладная с таким номером от этого поставщика в этом месяце уже была*/
sprintf(strsql,"select datd,sklad,nomd from Dokummat where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-%02d' and nomnn='%s' and kontr='%s'",
gd,md,gd,md,dk,
rk->nom_nn.ravno(),
rk->kontr_prod.ravno());

if(readkey(strsql,&row,&cur) >= 1)
 {
  sprintf(strsql,"%s %s %s %s %s %s %s %s\n%s",
  gettext("Номер налоговой накладной"),
  rk->nom_nn.ravno(),
  gettext("уже есть в документе"),
  row[2],
  gettext("склад"),
  row[1],
  gettext("дата"),
  iceb_tu_datzap(row[0]),
  gettext("Подсистема \"Материальный учёт\""));

  iceb_t_soob(strsql);

  goto naz;
 }

sprintf(strsql,"select datd,podr,nomd from Usldokum where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-%02d' and nomnn='%s' and kontr='%s'",
gd,md,gd,md,dk,
rk->nom_nn.ravno(),
rk->kontr_prod.ravno());

if(readkey(strsql,&row,&cur) >= 1)
 {
  sprintf(strsql,"%s %s %s %s %s %s %s %s\n%s",
  gettext("Номер налоговой накладной"),
  rk->nom_nn.ravno(),
  gettext("уже есть в документе"),
  row[2],
  gettext("подразделение"),
  row[1],
  gettext("дата"),
  iceb_tu_datzap(row[0]),
  gettext("Подсистема \"Учёт услуг\""));

  iceb_t_soob(strsql);

  goto naz;
 }


pnds=iceb_t_pnds(rk->datdok.ravno()); /*основная ставка записывается в шапку документа*/

if(rk->nds == 4)
 {
  pnds=7.;
 }

if(rk->nds == 0 || rk->nds == 4) /*записывается в каждую запись документа*/
 rk->pnds=pnds;
else 
 rk->pnds=0.;

class lock_tables lokt("LOCK TABLES Dokummat WRITE,icebuser READ");

/*Проверяем есть ли такой номер документа*/
sprintf(strsql,"select nomd from Dokummat where datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
rk->datdok.ravno_god(),rk->datdok.ravno_god(),rk->sklad.ravno(),rk->nomdok.ravno());
if(readkey(strsql) >= 1)
  {
   sprintf(strsql,gettext("С номером %s документ уже есть !"),rk->nomdok.ravno());
   iceb_t_soob(strsql);
   goto naz;  
  }

sprintf(strsql,"insert into Dokummat (tip,datd,sklad,kontr,nomd,nomnn,kodop,ktoi,vrem,datpnn,pn,k00) values(%d,'%s',%d,'%s','%s','%s','%s',%d,%ld,'%s',%.2f,'%s')",
1,
rk->datdok.ravno_sqldata(),
rk->sklad.ravno_atoi(),
rk->kontr_prod.ravno_filtr(),
rk->nomdok.ravno_filtr(),
rk->nom_nn.ravno_filtr(),
rk->kod_op.ravno_filtr(),
iceb_t_getuid(),
time(NULL),
rk->datdok.ravno_sqldata(),
pnds,
"00");

if(sql_zapis(strsql,1,0) != 0)
 goto naz;;

lokt.unlock();/*разблокируем таблицы*/

if(rk->nds != 0 )
 {
  sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,%d)",
  rk->datdok.ravno_god(),rk->sklad.ravno(),rk->nomdok.ravno_filtr(),11,rk->nds);

  sql_zapis(strsql,1,0);

}

return(0);

}
