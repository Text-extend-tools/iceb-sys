/*$Id: vprov1.c,v 5.54 2013/11/05 10:50:57 sasa Exp $*/
/*08.10.2014	30.09.1998	Белых А.И.	vprov1.c
Ввод проводки
*/
#include        <math.h>
#include        "buhl.h"

short           pmsh(const char*,const char*,int,int,int,int,const char*);


void vprov1(VVOD *VV,const char *metkasys,short nin,//0-проверять материальные счета 1- не проверять
class iceb_tu_str *dpr, //Дата проводки - последняя запомненная дата
const char *nn, //номер документа
const char *kop, //Код операции
int skk,   //Код подразделения
int dd,int md,int gd, //Дата документа
short mt, //0-ввод 1-корректировка
time_t vremz, //Время записи корректируемой проводки
const char *kontr, //Код контрагента для корректируемой проводки если он есть
int  ktozap,  //Кто записал
int pods, //1-МУ 2-ЗП 3-УОС 4-ПЛТ 5-ТРЕ 6-УСЛ 7-КО 8-УКР
int tipz, //1-приход 2-расход
int val)//0-балансовый -1 внебалансовый
{
time_t		vrem;
short           i=0,K=0;
int             N;
short           d1,m1,g1;
class iceb_tu_str kodo("");
class iceb_tu_str kodo1("");;
class iceb_tu_str kto("");
double		deb,kre;
class iceb_tu_str datz("");
class iceb_tu_str shz("");
class iceb_tu_str shkz("");
double		debz,krez;
short           msrs,msrs1; /*Метка счета с развернутым сальдо*/
class iceb_tu_str dkk("");
class iceb_tu_str nai("");
class iceb_tu_str nai1("");;
class iceb_tu_str nsh("");
char		strsql[2048];
SQL_str         row;
SQLCURSOR       cur;
class iceb_tu_str naim("");
class iceb_tu_str komentz("");
class iceb_tu_str naimoper("");
struct OPSHET	shetv;
short		kolprov=2;
int		valz=val;
short		kolprovz=2;
short           d_pz=0,m_pz=0,g_pz=0;
class iceb_tu_str spmatshets(""); /*список материальных счетов*/

int X=-1,Y=-1;

class iceb_tu_str imaf_spis_koment("");

if(nin == 0)
 {
  iceb_t_poldan("Перечень материальных счетов",&spmatshets,"matnast.alx");
 }
 
 
if(pods == 1)
 imaf_spis_koment.new_plus("spis_koment_mu.alx");
if(pods == 2)
 imaf_spis_koment.new_plus("spis_koment_zp.alx");
if(pods == 3)
 imaf_spis_koment.new_plus("spis_koment_uos.alx");
if(pods == 4)
 imaf_spis_koment.new_plus("spis_koment_plt.alx");
if(pods == 5)
 imaf_spis_koment.new_plus("spis_koment_tre.alx");
if(pods == 6)
 imaf_spis_koment.new_plus("spis_koment_usl.alx");
if(pods == 7)
 imaf_spis_koment.new_plus("spis_koment_ko.alx");
if(pods == 8)
 imaf_spis_koment.new_plus("spis_koment_ukr.alx");
 
if(val == -1)
 kolprov=kolprovz=1;

msrs=msrs1=0;

shz.new_plus(VV->VVOD_SPISOK_return_data(0));
shkz.new_plus(VV->VVOD_SPISOK_return_data(1));
debz=atof(VV->VVOD_SPISOK_return_data(2));
krez=0.;
if(val == -1)
 {
  kolprov=kolprovz=1;
  if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    shz.new_plus(VV->VVOD_SPISOK_return_data(1));
    shkz.new_plus("");
    krez=atof(VV->VVOD_SPISOK_return_data(2));
    debz=0.;
   }
 }
datz.new_plus(dpr->ravno());
rsdat(&d_pz,&m_pz,&g_pz,datz.ravno(),1);

kto.new_plus(metkasys);
komentz.new_plus(VV->VVOD_SPISOK_return_data(4));
VV->VVOD_SPISOK_zapis_data(3,dpr->ravno());

VV->VVOD_SPISOK_add_MD(gettext("Дебет.........."));//0
VV->VVOD_SPISOK_add_MD(gettext("Кредит........."));//1
VV->VVOD_SPISOK_add_MD(gettext("Сумма.........."));//2
VV->VVOD_SPISOK_add_MD(gettext("Дата проводки.."));//3
VV->VVOD_SPISOK_add_MD(gettext("Коментарий....."));//4
VV->VVOD_SPISOK_add_MD(gettext("КЭКЗ..........."));//5

N=0;

if(mt == 1)
 {
  naim.new_plus("");
  sprintf(strsql,"select nais from Plansh where ns='%s'",VV->VVOD_SPISOK_return_data(0));  
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);

  sprintf(strsql,"%s:%s %s",gettext("Счет"),VV->VVOD_SPISOK_return_data(0),naim.ravno());

  VV->VVOD_SPISOK_add_ZAG(strsql);
  
  naim.new_plus("");
  sprintf(strsql,"select nais from Plansh where ns='%s'",
  VV->VVOD_SPISOK_return_data(1));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);

  sprintf(strsql,"%s:%s %s",gettext("Счет к-нт"),
  VV->VVOD_SPISOK_return_data(1),naim.ravno());

  VV->VVOD_SPISOK_add_ZAG(strsql);

  naimoper.new_plus("");
  if(pods == 1)
   {
    if(tipz == 1)
      sprintf(strsql,"select naik from Prihod where kod='%s'",kop);  
    if(tipz == 2)
      sprintf(strsql,"select naik from Rashod where kod='%s'",kop);  

    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }

  if(pods == 3)
   {
    if(tipz == 1)
      sprintf(strsql,"select naik from Uospri where kod='%s'",kop);  
    if(tipz == 2)
      sprintf(strsql,"select naik from Uosras where kod='%s'",kop);  

    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }


  if(pods == 4)
   {
    sprintf(strsql,"select naik from Opltp where kod='%s'",kop);  
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }
  if(pods == 5)
   {
    sprintf(strsql,"select naik from Opltt where kod='%s'",kop);  
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }

  if(pods == 6)
   {
    if(tipz == 1)
      sprintf(strsql,"select naik from Usloper1 where kod='%s'",kop);  
    if(tipz == 2)
      sprintf(strsql,"select naik from Usloper2 where kod='%s'",kop);  

    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }

  if(pods == 7)
   {
    if(tipz == 1)
      sprintf(strsql,"select naik from Kasop1 where kod='%s'",kop);  
    if(tipz == 2)
      sprintf(strsql,"select naik from Kasop2 where kod='%s'",kop);  

    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }

  if(pods == 8)
   {
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",kop);  
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naimoper.new_plus(row[0]);
   }
  if(naimoper.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s:%s %s",gettext("Код операции"),kop,naimoper.ravno());
    VV->VVOD_SPISOK_add_ZAG(strsql);
   }

  if(kontr[0] != '\0')
   {   
    naim.new_plus("");
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);  
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);
    sprintf(strsql,"%s:%s %s",gettext("Кон-гент"),kontr,naim.ravno());
    VV->VVOD_SPISOK_add_ZAG(strsql);
   }

  kzvz(ktozap,vremz,VV);

 }

naz:;
//clear(); Убрал для того чтобы видно было на экране реквизиты документа и суммы   
kodo.new_plus("");
kodo1.new_plus("");

move(0,0);
printw(gettext("Ввод и корректировка проводок"));

VV->VVOD_SPISOK_zapis_data(3,dpr->ravno());
if(pods != 6)
 helstr(LINES-1,0,"F2/+",gettext("запись"),
 "F5",gettext("настройка"),
 "F6",gettext("комментарий"),
 "F10",gettext("выход"),NULL);
else
 helstr(LINES-1,0,"F2/+",gettext("запись"),
 "F3",gettext("комментарий"),
 "F5",gettext("настройка"),
 "F6",gettext("комментарий"),
 "F10",gettext("выход"),NULL);

VV->vramka(Y,X,0);


for(;;)
 {
  attron(VV->VVOD_return_cs(iceb_CFS)); 
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }


  if(N > VV->KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV->KLST-1;
   }

  VV->vprintw(N,stdscr);

  K=getch();

  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  VV->vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV->PX,VV->PX,VV->MDLS,VV->KLST,stdscr);


  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));
    attron(VV->VVOD_return_cs(iceb_CFV));

    VV->vprintw_clear(N,stdscr);

    VV->vgetstr(N,stdscr);

    if(N == 0)  /*Счет*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(VV->VVOD_SPISOK_return_data(0)[0] != '\0')
       {
        if(prsh1(VV->VVOD_SPISOK_return_data(0),&shetv) != 0)
          goto naz;
        if(nin == 0)
        if(pmsh(VV->VVOD_SPISOK_return_data(0),nn,skk,dd,md,gd,spmatshets.ravno()) != 0)
         {
          VVOD DANET(1);
          sprintf(strsql,gettext("Счет %s материальный и по нему нет записей в документе !"),VV->VVOD_SPISOK_return_data(0));
          DANET.VVOD_SPISOK_add_MD(strsql);
          DANET.VVOD_SPISOK_add_MD(gettext("                Все верно ? Вы уверены ?"));
          if(danet(&DANET,2,stdscr) == 2)
           {
            VV->data_plus(0,"");
            goto naz;
           }
          N++;
          goto naz;
         }
       }     
     }

    if(N == 1)   /*Счет*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(VV->VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if(prsh1(VV->VVOD_SPISOK_return_data(N),&shetv) != 0)
          goto naz;
       if(nin == 0)
       if(pmsh(VV->VVOD_SPISOK_return_data(1),nn,skk,dd,md,gd,spmatshets.ravno()) != 0)
        {
         VVOD DANET(1);
         sprintf(strsql,gettext("Счет %s материальный и по нему нет записей в документе !"),VV->VVOD_SPISOK_return_data(1));
         DANET.VVOD_SPISOK_add_MD(strsql);
         DANET.VVOD_SPISOK_add_MD(gettext("                Все верно ? Вы уверены ?"));
         if(danet(&DANET,2,stdscr) == 2)
  	  {
           VV->data_plus(1,"");
  	   goto naz;
  	  }
         N++;
         goto naz;
        }
       }
     }

    attroff(VV->VVOD_return_cs(iceb_CFV));
    attron(VV->VVOD_return_cs(iceb_CFM));
    VV->vprintw(N,stdscr);
    N++;
   }


  if(K == ESC || K == FK10)
   {
    N=-1;
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CFM));
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
     attroff(VV->VVOD_return_cs(iceb_CFS));
     attron(VV->VVOD_return_cs(iceb_CFM));
     GDITE();
     dpr->new_plus(VV->VVOD_SPISOK_return_data(3));
     if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' && VV->VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }
     val=0; kolprov=2;
     if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' || VV->VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       val=-1; kolprov=1;
      }

     if( VV->VVOD_SPISOK_return_data(2)[0] == '\0' || VV->VVOD_SPISOK_return_data(3)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto naz;
      }

     if(ATOFM(VV->VVOD_SPISOK_return_data(2)) == 0.)
      {
       iceb_t_soob(gettext("Сумма равна нолю !"));
       goto naz;
      }

     if(rsdat(&d1,&m1,&g1,VV->VVOD_SPISOK_return_data(3),0) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto naz;
      }

     if(iceb_t_pvglkni(m1,g1) != 0)
      goto naz;
     
     if(m_pz != 0)
      if(iceb_t_pvglkni(m_pz,g_pz) != 0)
       goto naz;

    memset(&shetv,'\0',sizeof(shetv));
    if(val == 0 || VV->VVOD_SPISOK_return_data(0)[0] != '\0')
     if(prsh1(VV->VVOD_SPISOK_return_data(0),&shetv) != 0)
       goto naz;

    if(val == 0 && shetv.stat == 1)
     {
      sprintf(strsql,gettext("Счет %s внебалансовый !"),VV->VVOD_SPISOK_return_data(0));
      iceb_t_soob(strsql);
      goto naz;
     }       

    /*Для счетов с развернутым сальдо*/
    msrs=0;
    if(shetv.saldo == 3)
     msrs=1;

    memset(&shetv,'\0',sizeof(shetv));
    if(val == 0 || VV->VVOD_SPISOK_return_data(1)[0] != '\0')
     if(prsh1(VV->VVOD_SPISOK_return_data(1),&shetv) != 0)
      goto naz;
    if(val == 0 && shetv.stat == 1)
     {
      sprintf(strsql,gettext("Счет %s внебалансовый !"),VV->VVOD_SPISOK_return_data(1));
      iceb_t_soob(strsql);
      goto naz;
     }       

    /*Для счетов с развернутым сальдо*/
    msrs1=0;
    if(shetv.saldo == 3)
     msrs1=1;

    /*Код контрагента для главного счета*/
    dkk.new_plus("");
    kodo.new_plus("");
    if(msrs == 1)
     {
      VVOD  VVOD1(2);
      sprintf(strsql,gettext("Введите код конрагента для счёта %s"),VV->VVOD_SPISOK_return_data(0));
      VVOD1.VVOD_SPISOK_add_MD(strsql);
iii:;
      helstr(LINES-1,0,"Enter",gettext("контрагенты"),
      "F10",gettext("выход"),NULL);

      dkk.new_plus("");
      if((i=vvod1(&dkk,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        goto naz;
      if(i == ESC)
        goto naz;

      if(dkk.getdlinna() <= 1)
       {
	nai.new_plus(dkk.ravno());
        nsh.new_plus(VV->VVOD_SPISOK_return_data(0));
	if(dirskont(nsh.ravno(),&kodo,&nai,1) != 0)
	 {
	  clear(); /*Очистить экран и закрасить фоновым цветом*/
          kodo.new_plus("");
          goto naz;
	 }
	clear();
       }
      else
       kodo.new_plus(dkk.ravno());

      sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV->VVOD_SPISOK_return_data(0),kodo.ravno());
      if(sql_readkey(&bd,strsql) != 1)
       {
        sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),kodo.ravno(),VV->VVOD_SPISOK_return_data(0));
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

    /*Код контрагента для субсчета*/

    kodo1.new_plus("");
    dkk.new_plus("");
    if(msrs1 == 1)
     {
      VVOD  VVOD1(2);
      sprintf(strsql,gettext("Введите код конрагента для счёта %s"),VV->VVOD_SPISOK_return_data(1));
      VVOD1.VVOD_SPISOK_add_MD(strsql);
iii1:;
      helstr(LINES-1,0,"Enter",gettext("контрагенты"),
      "F10",gettext("выход"),NULL);
      dkk.new_plus("");
      if((i=vvod1(&dkk,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        goto naz;

      if(i == ESC)
        goto naz;
      if(dkk.getdlinna() <= 1)
       {
	nai1.new_plus(dkk.ravno());
        nsh.new_plus(VV->VVOD_SPISOK_return_data(1));   
	if(dirskont(nsh.ravno(),&kodo1,&nai1,1) != 0)
	 {
	  clear();
	  kodo1.new_plus("");
          goto naz;
	 }
       }
      else
       kodo1.new_plus(dkk.ravno());

      sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV->VVOD_SPISOK_return_data(1),kodo1.ravno());
      if(sql_readkey(&bd,strsql) != 1)
       {
        sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),kodo1.ravno(),VV->VVOD_SPISOK_return_data(1));
        iceb_t_soob(strsql);
        goto iii1;
       }
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      kodo1.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),kodo1.ravno());
        iceb_t_soob(strsql);
        goto iii1;
       }
      nai1.new_plus(row[0]);

     }

     deb=ATOFM(VV->VVOD_SPISOK_return_data(2));
     
     deb=okrug(deb,0.01);
     kre=0.;

     if(kodo.ravno()[0] != '\0' && kodo1.ravno()[0] == '\0')
	kodo1.new_plus(kodo.ravno());
     if(kodo1.ravno()[0] != '\0' && kodo.ravno()[0] == '\0')
	kodo.new_plus(kodo1.ravno());
     time(&vrem);
     if(val == 0)
      {
       if(zapprov(val,g1,m1,d1,VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),kodo.ravno(),kodo1.ravno(),kto.ravno(),nn,kop,deb,kre,VV->data_ravno_filtr(4),kolprov,skk,vrem,gd,md,dd,tipz,VV->data_ravno_atoi(5)) != 0)
        {
         break;
        }
      }
     if(val == -1)
      {
       if(VV->VVOD_SPISOK_return_data(1)[0] != '\0')
        {
         if(zapprov(val,g1,m1,d1,VV->VVOD_SPISOK_return_data(1),VV->VVOD_SPISOK_return_data(0),kodo.ravno(),kodo1.ravno(),kto.ravno(),nn,kop,kre,deb,VV->data_ravno_filtr(4),kolprov,skk,vrem,gd,md,dd,tipz,VV->data_ravno_atoi(5)) != 0)
          break;          
        }         
       else
        if(zapprov(val,g1,m1,d1,VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),kodo.ravno(),kodo1.ravno(),kto.ravno(),nn,kop,deb,kre,VV->data_ravno_filtr(4),kolprov,skk,vrem,gd,md,dd,tipz,VV->data_ravno_atoi(5)) != 0)
          break;
      }     

     if(mt == 1)
      { 
       rsdat(&d1,&m1,&g1,datz.ravno(),0);
//       sqlfiltr(komentz,sizeof(komentz));       
       udprov(valz,g1,m1,d1,shz.ravno(),shkz.ravno(),vremz,debz,krez,komentz.ravno_filtr(),kolprovz);
      }

     break;

   }

  if(K == FK3) //Взять в комментарий наименование первой строки в документе
   {
     attroff(VV->VVOD_return_cs(iceb_CFS));
     attron(VV->VVOD_return_cs(iceb_CFM));
     if(pods == 6) //Учет услуг
      {
       //Читаем номер документа поставщика
       class iceb_tu_str nomdp("");
       sprintf(strsql,"select nomdp from Usldokum where \
datd='%d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
       gd,md,dd,skk,nn,tipz);
       if(sql_readkey(&bd,strsql,&row,&cur) > 0)
         nomdp.new_plus(row[0]);

       //Читаем перую запись в документе
       sprintf(strsql,"select metka,kodzap from Usldokum1 where \
datd='%d-%d-%d' and podr=%d and nomd='%s'",
       gd,md,dd,skk,nn);       

       if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
         goto naz;       

       //Узнаем наименование кода записи
       int metka=atoi(row[0]);
       int kodzap=atoi(row[1]);
       if(metka == 0) //Материал
        sprintf(strsql,"select naimat from Material where kodm=%d",kodzap);       
       if(metka == 1) //Услуга
        sprintf(strsql,"select naius from Uslugi where kodus=%d",kodzap);       
       if(sql_readkey(&bd,strsql,&row,&cur) == 1)
        {
         sprintf(strsql,"N%s %s",nomdp.ravno(),row[0]);
         VV->VVOD_SPISOK_zapis_data(4,strsql);
        }
      }
     goto naz;
   }

  if(K == FK5) //настройка списка комментариев
   {
     attroff(VV->VVOD_return_cs(iceb_CFS));
     attron(VV->VVOD_return_cs(iceb_CFM));
     iceb_redfil(imaf_spis_koment.ravno(),0);
     goto naz;
   }
  if(K == FK6) //Выбрать комментарий из списка
   {
     attroff(VV->VVOD_return_cs(iceb_CFS));
     attron(VV->VVOD_return_cs(iceb_CFM));
     class iceb_tu_str koment_sp("");
     if(spis_koment(&koment_sp,imaf_spis_koment.ravno()) == 0)
      { 
       VV->data_strcat(4,koment_sp.ravno());
      }
     clear(); //экран не очищается перед первичным выводом меню чтобы были видны реквизиты документа 
     goto naz;
      
   }

 }
attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CB));
clear();
}
/*****************/
/*Проверка счета */
/*****************/
/*Если счёт материальный и нет записи материала
в накладной с этим счетом то счёт введён ошибочно
Если вернули 0 - счёт правильный
	     1 - нет
	     */


short pmsh(const char *sh,  //Счет для проверки
const char *nn,int skk,int d,int m,int g,
const char *mtsh)
{
char		strsql[1024];
short           voz;
int		i;
int		nk;
long		kolstr;
SQL_str         row,row1;
voz=0;
if(mtsh == NULL)
  return(0);
if(pole1(mtsh,sh,',',1,&i) == 0)
  voz=1;

if(voz == 0)
 return(0);

/*
printw("\nsh-%s nn-%s skk-%d %d.%d.%d %s\n",
sh,nn,skk,d,m,g,mtsh);
refresh();
*/

sprintf(strsql,"select nomkar from Dokummat1 where datd='%d-%02d-%02d' \
and sklad=%d and nomd='%s'",
g,m,d,skk,nn);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }

voz = 1;

while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[0]);
  /*Читаем карточку материала*/
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",
    skk,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw(gettext("Не нашли карточки %d на складе %d !\n"),
      nk,skk);
      OSTANOV();
     }
    if(SRAV(sh,row1[0],1) == 0)
     {
      voz = 0;
      break;
     }
   }


 }
return(voz);

}
/************************/
/*Создание списка данных*/
/************************/
void  sozvprov1(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);//0
VV->VVOD_SPISOK_add_data(10);//1
VV->VVOD_SPISOK_add_data(20);//2
VV->VVOD_SPISOK_add_data(11);//3
VV->VVOD_SPISOK_add_data(100);//4
VV->VVOD_SPISOK_add_data(6);//5
}
