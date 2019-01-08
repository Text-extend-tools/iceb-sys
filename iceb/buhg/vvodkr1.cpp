/*$Id: vvodkr1.c,v 5.37 2013/09/26 09:43:44 sasa Exp $*/
/*21.09.2014	25.10.2002	Белых А.И.	vvodkr1.c
Ввод и корректировка записи в документ командировочных расходов
*/
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/

void	vvodkr1(short dd,short md,short gd,
const char *nomdok,
const char *kodkr,
const char *shetz,
int metkareg, //0-ввод новой записи 1-редактирование
int nomz)
{
char	strsql[1024];
SQL_str row;
class iceb_tu_str kodgr("");
class iceb_tu_str naimgr("");
class iceb_tu_str naimkr("");
class iceb_tu_str eiz("");
class iceb_tu_str shet("");
class iceb_tu_str naimeiz("");
class iceb_tu_str naimshet("");
struct OPSHET	shetv;
double suma_dok=0.; /*Общая сумма по документу*/
double	kolih=0.;
double  snds=0.;
class iceb_tu_str kontr("");
class iceb_tu_str naimkont("");
class iceb_tu_str naimkont_r("");
class iceb_tu_str ndrnn("");
int ktoz=0;
time_t vremz=0;
class iceb_tu_str shet_nds("");
class iceb_tu_str naim_shet_nds("");
short dvnn=0,mvnn=0,gvnn=0;
double strah_sbor=0.; 

class iceb_tu_str kdrnn(""); //Код контрагента для реестра налоговых накладных

SQLCURSOR curr;

if(metkareg == 0) //Ввод новой записи
 {
  sprintf(strsql,"select * from Ukrkras where kod=%s",kodkr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s:%s",gettext("Не найден код"),kodkr);
    iceb_t_soob(strsql);
    return;
   }

  naimkr.new_plus(row[1]);
  shet.new_plus(row[3]);
  kodgr.new_plus(row[4]);
  eiz.new_plus(row[5]);
  suma_dok=atof(row[2]);
 }

if(metkareg == 1) //Корректировка записи
 {
  sprintf(strsql,"select shet,kolih,cena,ediz,snds,kontr,kdrnn,ndrnn,ktoz,vrem,sn,dvnn,ss from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s' and kodr=%s and shet='%s' and nomz=%d",
  gd,md,dd,nomdok,kodkr,shetz,nomz);
  printw("\n%s\n",strsql);
  if(readkey(strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена запись для корректировки !"));
    sprintf(strsql,"%d.%d.%d %s %s %s",dd,md,gd,nomdok,kodkr,shetz);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return;
   }
  kolih=atof(row[1]);
  eiz.new_plus(row[3]);
  shet.new_plus(row[0]);
  snds=atof(row[4]);
  snds=okrug(snds,0.01);
  strah_sbor=atof(row[12]);
  suma_dok=atof(row[2])+snds+strah_sbor;//Общая сумма по документу
  suma_dok=okrug(suma_dok,okrcn);
  kontr.new_plus(row[5]);
  kdrnn.new_plus(row[6]);
  ndrnn.new_plus(row[7]);
  ktoz=atoi(row[8]);
  vremz=atol(row[9]);
  shet_nds.new_plus(row[10]);
  rsdat(&dvnn,&mvnn,&gvnn,row[11],2);
    
  sprintf(strsql,"select naim,kgr from Ukrkras where kod=%s",kodkr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s:%s",gettext("Не найден код"),kodkr);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return;
   }

  naimkr.new_plus(row[0]);
  kodgr.new_plus(row[1]);
  
 }
 
sprintf(strsql,"select naik from Ukrgrup where kod=%s",kodgr.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  naimgr.new_plus(row[0]);

naimeiz.new_plus("");
if(eiz.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Edizmer where kod='%s'",eiz.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimeiz.new_plus(row[0]);
 }

if(shet.ravno()[0] != '\0')
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimshet.new_plus(row[0]);
 }

if(shet_nds.ravno()[0] != '\0')
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",shet_nds.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naim_shet_nds.new_plus(row[0]);
 }

if(kontr.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimkont.new_plus(row[0]);
 }
if(kdrnn.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kdrnn.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimkont_r.new_plus(row[0]);
 }

int	N=0,K=0;
    
VVOD VV(0);

if(metkareg == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

if(metkareg == 1)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktoz,vremz,&VV);
  sprintf(strsql,"%s:%s %s:%d.%d.%d%s",gettext("Номер документа"),
  nomdok,gettext("Дата"),dd,md,gd,gettext("г."));
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }
sprintf(strsql,"%s:%s %s",gettext("Расход"),kodkr,naimkr.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%s:%s %s",gettext("Группа"),kodgr.ravno(),naimgr.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Количество......................................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Сумма с НДС......................................"));//1
VV.VVOD_SPISOK_add_MD(gettext("Сумма НДС........................................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения................................"));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт............................................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента (часное лицо)...................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента для реестра налогових накладных.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.................................."));//7
VV.VVOD_SPISOK_add_MD(gettext("Счёт НДС........................................."));//8
VV.VVOD_SPISOK_add_MD(gettext("Дта виписки налоговой накладной (д.м.г).........."));//9
VV.VVOD_SPISOK_add_MD(gettext("Страховой сбор..................................."));//10

sprintf(strsql,"%.10g",kolih);
VV.VVOD_SPISOK_add_data(strsql,20);//0

sprintf(strsql,"%.10g",suma_dok);
VV.VVOD_SPISOK_add_data(strsql,20);//1

sprintf(strsql,"%.10g",snds);
VV.VVOD_SPISOK_add_data(strsql,20);//2

VV.VVOD_SPISOK_add_data(80);//3
VV.VVOD_SPISOK_add_data(80);//4
VV.VVOD_SPISOK_add_data(80);//5
VV.VVOD_SPISOK_add_data(80);//6
VV.VVOD_SPISOK_add_data(ndrnn.ravno(),30);//7
VV.VVOD_SPISOK_add_data(80);//8
VV.VVOD_SPISOK_add_data(11);//9
VV.VVOD_SPISOK_add_data(10);//10

if(strah_sbor != 0.)
 VV.data_plus(10,strah_sbor);

naz:;

clear();
if(eiz.getdlinna() > 1)
 {
  sprintf(strsql,"%s|%s",eiz.ravno(),naimeiz.ravno());
  VV.VVOD_SPISOK_zapis_data(3,strsql);
 }
if(shet.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s|%s",shet.ravno(),naimshet.ravno());
  VV.VVOD_SPISOK_zapis_data(4,strsql);
 }

if(kontr.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s|%s",kontr.ravno(),naimkont.ravno());
  VV.VVOD_SPISOK_zapis_data(5,strsql);
 }
if(kdrnn.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s|%s",kdrnn.ravno(),naimkont_r.ravno());
  VV.VVOD_SPISOK_zapis_data(6,strsql);
 }
if(shet_nds.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s|%s",shet_nds.ravno(),naim_shet_nds.ravno());
  VV.VVOD_SPISOK_zapis_data(8,strsql);
 }
if(dvnn != 0)
 {
  sprintf(strsql,"%d.%d.%d",dvnn,mvnn,gvnn);
  VV.VVOD_SPISOK_zapis_data(9,strsql);
 } 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

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

    if(N == 3) //Единица измерения
     {
      naimeiz.new_plus("");
      if(iceb_tl_ei(2,&eiz,&naimeiz) == 0)
        VV.VVOD_SPISOK_zapis_data(N,eiz.ravno());
      N++;
      goto naz;

     }

    if(VV.vgetstr(N,stdscr) == KEY_RESIZE)
     goto naz;

    if(N == 0) //Количество
     {
      kolih=ATOFM(VV.VVOD_SPISOK_return_data(N));
     }


    if(N == 1) //Цена
     {
      suma_dok=ATOFM( VV.VVOD_SPISOK_return_data(N));
      suma_dok=okrug(suma_dok,0.01);
     }

    if(N == 2) 
     {
      snds=ATOFM(VV.VVOD_SPISOK_return_data(N));
      snds=okrug(snds,0.01);
     }

    if(N == 4) //Счёт
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Счёт
       { 
        if(polen(VV.VVOD_SPISOK_return_data(N),&shet,1,'|') != 0)
         shet.new_plus(VV.VVOD_SPISOK_return_data(N));

        if(prsh1(shet.ravno(),&shetv) != 0)
          goto naz;

        naimshet.new_plus(shetv.naim);
        N++;
        goto naz;
       }
      else
       shet.new_plus("");
     }
     
    if(N == 5) //Контрагент физическое лицо
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Контрагент
       {
        if(polen(VV.VVOD_SPISOK_return_data(N),&kontr,1,'|') != 0)
         kontr.new_plus(VV.VVOD_SPISOK_return_data(N));
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
        if(sql_readkey(&bd,strsql,&row,&curr) != 1)
         {
          VVOD SOOB(1);
          sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          naimkont.new_plus("");
          goto naz;

         }            
        naimkont.new_plus(row[0]); 
        goto naz;
       }
      else
        kontr.new_plus("");
     }
    if(N == 6) //Контрагент для реестра налоговых накладных
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Контрагент
       {
        if(polen(VV.VVOD_SPISOK_return_data(N),&kdrnn,1,'|') != 0)
         kdrnn.new_plus(VV.VVOD_SPISOK_return_data(N));
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kdrnn.ravno());
        if(sql_readkey(&bd,strsql,&row,&curr) != 1)
         {
          VVOD SOOB(1);
          sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kdrnn.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          naimkont_r.new_plus("");
          goto naz;
         }            
        naimkont_r.new_plus(row[0]); 
        goto naz;
       }
      else
        kdrnn.new_plus("");
     }

    if(N == 8 ) //Счёт НДС
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Счёт
       { 
        if(polen(VV.VVOD_SPISOK_return_data(N),&shet_nds,1,'|') != 0)
         shet_nds.new_plus(VV.VVOD_SPISOK_return_data(N));

        if(prsh1(shet_nds.ravno(),&shetv) != 0)
          goto naz;

        naim_shet_nds.new_plus(shetv.naim);
        N++;
        goto naz;
       }
      else
       shet_nds.new_plus("");
     }
      
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }
  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
    iceb_t_pdoc("ukr2_2_2.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU ) //Записываем
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    if(dirdokkrpiz(dd,md,gd) != 0)
     goto naz;

    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || 
     VV.VVOD_SPISOK_return_data(1)[0] == '\0' || \
     VV.VVOD_SPISOK_return_data(2)[0] == '\0' || \
     VV.VVOD_SPISOK_return_data(3)[0] == '\0')
      {     
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }

     if(prsh1(shet.ravno(),&shetv) != 0)
        goto naz;

     if(shet_nds.ravno()[0] != '\0')
      if(prsh1(shet_nds.ravno(),&shetv) != 0)
        goto naz;

     if(kdrnn.ravno()[0] != '\0')
      {
       sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kdrnn.ravno());
       if(readkey(strsql) != 1)
        {
         VVOD SOOB(1);
         SOOB.VVOD_SPISOK_add_MD(gettext("Не найден код контрагента для реестра налоговых накладных !"));
         SOOB.VVOD_SPISOK_add_MD(kdrnn.ravno());
         soobshw(&SOOB,stdscr,-1,-1,0,1);
         goto naz;
        }
      }

     if(VV.data_ravno(9)[0] != '\0')
      {
       if(rsdat(&dvnn,&mvnn,&gvnn,VV.data_ravno(9),1) != 0)
        {
         iceb_t_soob(gettext("Неправильно ввели дату виписки!"));
         goto naz;
        
        }
      }

     if(metkareg == 1)
      {
       sprintf(strsql,"delete from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s' and kodr=%s and shet='%s' and nomz=%d",
       gd,md,dd,nomdok,kodkr,shetz,nomz);
       if(sql_zap(&bd,strsql) != 0)
         msql_error(&bd,"Ошибка ввода записи !",strsql);

      }
     
     ndrnn.new_plus(VV.data_ravno(7));
     strah_sbor=VV.data_ravno_atof(10);
     strah_sbor=okrug(strah_sbor,0.01);
     time_t vrem;
     time(&vrem);
     
snova:;
               
     sprintf(strsql,"insert into Ukrdok1 values \
('%04d-%d-%d','%s',%s,'%s',%.10g,%.10g,'%s',%.2f,%d,%ld,%d,'%s','%s','%s','%s','%04d-%d-%d',%.2f)",
     gd,md,dd,nomdok,kodkr,shet.ravno(),kolih,
     suma_dok-snds-strah_sbor,eiz.ravno(),snds,iceb_t_getuid(),vrem,nomz,kontr.ravno(),kdrnn.ravno(),ndrnn.ravno_filtr(),shet_nds.ravno(),gvnn,mvnn,dvnn,strah_sbor);

     if(sql_zap(&bd,strsql) != 0)
      {
       if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
        {
         nomz++;
         goto snova;
/********
         VVOD SOOB(1);
         SOOB.VVOD_SPISOK_add_MD(gettext("Такая запись уже есть !"));
         soobshw(&SOOB,stdscr,-1,-1,0,1);
         goto naz;
**********/
        }
       else
        {
         msql_error(&bd,gettext("Ошибка записи !"),strsql);
         goto naz;
        }
      }
    break;
   }

  if(K == FK3 ) //Реквизиты
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VVOD MENU(3);

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка единиц измерения"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    class iceb_tu_str kod("");
    class iceb_tu_str nai("");
    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;


      case 0 :
        
        if(vibrek("Edizmer",&kod) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(3,kod.ravno());
          eiz.new_plus(kod.ravno());
          naimeiz.new_plus(strsql);
         }
        break;

      case 1 :
        
        if(vibrek("Plansh",&kod,&naimshet) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(4,kod.ravno());
          shet.new_plus(kod.ravno());
         }
        break;

      case 2 :
        
        if(vibrek("Kontragent",&kod,&nai) == 0)
         {
          VVOD DANET(1);
          DANET.VVOD_SPISOK_add_MD(gettext("Код контрагента для реестра налоговых накладных ?"));

          if(danet(&DANET,2,stdscr) == 2)
           {
            kontr.new_plus(kod.ravno());
            naimkont.new_plus(nai.ravno());
           }
          else
           {
            kdrnn.new_plus(kod.ravno());
            naimkont_r.new_plus(nai.ravno());
           }
          
         }
        break;
     }
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}
