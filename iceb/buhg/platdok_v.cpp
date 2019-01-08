/*$Id: platdok_v.c,v 5.5 2013/09/26 09:43:37 sasa Exp $*/
/*28.09.2014	29.11.2012	Белых А.И.	platdok_v.c
Ввод нового платёжного поручения
*/
#include "buhg.h"
#include  "dok4.h"


extern short    kp;    /*Количество повторов*/
extern class REC rec;
extern double   okrg1; /*Округление*/

void platdok_v(const char *tabl,int tipz)
{
int poz=0,komv=0;
class iceb_tu_str bros("");
char imaf[64];
char strsql[2048];
short dt=0,mt=0,gt=0;
char dat1[128];
class VVOD MENU(3);
class iceb_tu_str naim("");
static class iceb_tu_str kop("");
class iceb_tu_str kod("");
double nds1=iceb_t_pnds();
int N=0,K=0;

int mpo=0;
if(iceb_t_poldan_vkl("Поиск организаций в базе","nastdok.alx") == 0)
 mpo=1;
 
int npp=0;
class iceb_tu_str mescc("");
poltekdat(&dt,&mt,&gt);
mesc(mt,1,&mescc);

sprintf(dat1,"%02d %s %d%s",dt,mescc.ravno(),gt,gettext("г."));

clear();
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));

VV.VVOD_SPISOK_add_MD(gettext("Номер пл. документа.."));
VV.VVOD_SPISOK_add_MD(gettext("Операция............."));
VV.VVOD_SPISOK_add_MD(gettext("Сумма платежа........"));
VV.VVOD_SPISOK_add_MD(gettext("Дата платежа........."));
VV.VVOD_SPISOK_add_MD(gettext("Н.Д.С. .............."));
VV.VVOD_SPISOK_add_MD(gettext("Количетво копий......"));
VV.VVOD_SPISOK_add_MD(gettext("Дата валютирования..."));
VV.VVOD_SPISOK_add_MD(gettext("Код нерезидента......"));

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(9);


naim.new_plus("");

VV.vramka(-1,-1,0);


for(;;)
 {
  
  npp=iceb_t_nomdok(gt,tabl);

  VV.data_plus(0,npp);

  VV.data_plus(1,kop.ravno());

  VV.data_plus(3,dat1);

  /*Вторую не обнуляем*/
  if(kp > 0)
    VV.data_plus(5,kp);
  N=0;
  if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
   N=1;
  if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
   N=2;

  K=0;

naz:;

  VV.VVOD_delete_ZAG();
  VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
  if(SRAV(tabl,"Pltp",0) == 0)
     VV.VVOD_SPISOK_add_ZAG(gettext("Ввод платежного поручения"));
  if(SRAV(tabl,"Pltt",0) == 0)
     VV.VVOD_SPISOK_add_ZAG(gettext("Ввод платежного требования"));

  sprintf(strsql,"%s %d.%d.%d%s",gettext("Текущая дата"),
  dt,mt,gt,gettext("г."));
  VV.VVOD_SPISOK_add_ZAG(strsql);

  if(naim.getdlinna() > 1)
   {
    sprintf(strsql,"%s:%s",gettext("Операция"),naim.ravno());
    VV.VVOD_SPISOK_add_ZAG(strsql);
   }

  clear(); /*Очистить экран и закрасить фоновым цветом*/


  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("операции"),
  "F4",gettext("НДС"),
  "F10",gettext("выход"),NULL);

//  par=VV.vvod(par,1,1,-1,-1);
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
      goto naz;
     }

    if(K == KEY_MOUSE)
      K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

    if(K == ENTER)
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CFV));
      VV.vprintw_clear(N,stdscr);


      if(N == 1 ) /*код операции*/
       {
        kod.new_plus("");
        naim.new_plus("");

        if(SRAV(tabl,"Pltp",0) == 0)
         bros.new_plus("Opltp");

        if(SRAV(tabl,"Pltt",0) == 0)
         bros.new_plus("Opltt");

        poz=komv=0;
        if(dirmat1(bros.ravno(),&kod,&naim,-1,-1,-1,2,&poz,&komv,"","\0") == 0)
         {
          kop.new_plus(kod.ravno());
          VV.data_plus(1,kod.ravno());
          N++;
          goto naz;
         }
                
       }



      VV.vgetstr(N,stdscr);
      
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(N == 0) /*номер документа*/
       {

//        attroff(VV.VVOD_return_cs(iceb_CFV));
//        attron(VV.VVOD_return_cs(iceb_CFM));

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
      N=-1;
     // break;
      return;
     }

    if(K == FK2 || K == PLU) /*Записываем*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
      GDITE();
      rec.clear();
      if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
       {
        iceb_t_soob(gettext("Не введён номер документа !"));
        N=0;
        goto naz;
       }

      /*Проверяем код операции*/
      if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
       {
        iceb_t_soob(gettext("Не введена операция !"));
        N=1;
        goto naz;
       }

      if(SRAV(tabl,"Pltp",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",VV.VVOD_SPISOK_return_data(1));    

      if(SRAV(tabl,"Pltt",0) == 0)
       sprintf(strsql,"select naik from Opltt where kod='%s'",VV.VVOD_SPISOK_return_data(1));    

      if(readkey(strsql) != 1)
       {
        sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(1));
        iceb_t_soob(strsql);
        N=1;
        goto naz;
       }

      if(rsdat(&rec.dd,&rec.md,&rec.gd,VV.VVOD_SPISOK_return_data(3),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
        N=3;
        goto naz;
       }

      if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
      if(rsdat(&rec.dv,&rec.mv,&rec.gv,VV.VVOD_SPISOK_return_data(6),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата валютирования !"));
        N=3;
        goto naz;
       }

      strcpy(dat1,VV.VVOD_SPISOK_return_data(3));
      if(ATOFM(VV.VVOD_SPISOK_return_data(2)) == 0.)
       {
        iceb_t_soob(gettext("Не введена сумма платежа !"));
        N=2;
        goto naz;
       }

      kp=(short)ATOFM(VV.VVOD_SPISOK_return_data(5));
      if(kp == 0)
       {
        iceb_t_soob(gettext("Не введено количество копий !"));
        N=4;
        goto naz;
       }

      /*Проверяем номер платежного поручения*/

      sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",tabl,rec.gd,rec.gd,VV.VVOD_SPISOK_return_data(0));

      if(readkey(strsql) >= 1)
       {
        N=0;
        iceb_t_soob(gettext("Документ с таким номером уже есть !"));
        goto naz;
       }

      rec.nomdk.new_plus(VV.VVOD_SPISOK_return_data(0));
      rec.kodnr.new_plus(VV.VVOD_SPISOK_return_data(7));

      rec.nds=ATOFM(VV.VVOD_SPISOK_return_data(4));
      rec.sumd=ATOFM(VV.VVOD_SPISOK_return_data(2));
      if(rec.nds != 0.)
        rec.nds=okrug(rec.nds,okrg1);
      
      rec.sumd=okrug(rec.sumd,okrg1);
      
      rec.tipz=tipz;
      rec.kodop.new_plus(VV.VVOD_SPISOK_return_data(1));
      if(mpo == 0)
       {
        /*Берем реквизиты организации из файла*/
        iceb_redfil("platpor.alx",0);
        vzrkf();
       }
      else
       {
        /*Берем реквизиты организации из базы*/
        vzrkbz();
       }

      sozkom_pd(imaf,rec.nds,rec.sumd);


      vizred(imaf);

      pld(kp,tabl);

      VV.VVOD_SPISOK_zapis_data(2,"");
      VV.VVOD_SPISOK_zapis_data(4,"");
      //continue;
      break;      

     }
    if(K == FK3) /*реквизиты*/
     {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));

       clear();
       kod.new_plus("");
       naim.new_plus("");

       if(SRAV(tabl,"Pltp",0) == 0)
        bros.new_plus("Opltp");

       if(SRAV(tabl,"Pltt",0) == 0)
        bros.new_plus("Opltt");

       poz=komv=0;
       if(dirmat1(bros.ravno(),&kod,&naim,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
        {
         kop.new_plus(kod.ravno());
         VV.data_plus(1,kod.ravno());
         N=2;
         goto naz;
        }

       N=2;
       goto naz;
     }
    
    if(K == FK4) /*Записываем*/
     {
       if(dat1[0] != '\0')
         nds1=iceb_t_pnds(dat1);
       rec.nds=ATOFM(VV.VVOD_SPISOK_return_data(2))*nds1/(nds1+100);
       rec.nds=okrug(rec.nds,okrg1);
       if(rec.nds != 0.)
         VV.data_plus(4,rec.nds);
       goto naz;
     }

   }
  npp=(long)ATOFM(VV.VVOD_SPISOK_return_data(0));
  kop.new_plus(VV.VVOD_SPISOK_return_data(1));

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CB));
  clear();
  refresh();

 }
}
