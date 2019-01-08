/*$Id:$ */
/*22.03.2017    20.01.1993      Белых А.И.      rashod.c
Ввод расходов в уже существующие карточки материалов
Возвращяем 1 если не записали
	   0 записали
*/
#include        <math.h>
#include	"buhg.h"

extern short    mdd;  /*0-обычный документ 1-двойной*/
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление1 */
extern class iceb_tu_str shrt;    /*Счета розничной торговли*/

int rashod(short dd,short md,short gd,int skl,const char *nomdok,
int nk, //Номер карточки
int kmm, //Если = 0 привязать карточку
double kolih,
const char *kpos,
const char *nomdokp, //Номер встречного документа
const char *kodop, //Код операции
float pnds)
{
int mvnp=0;
class ostatokt ost;
int km=0; //Код материалла
double cnn=0.; //Цена
class iceb_tu_str ei("");
int mnds=0;
double ndsg=0.;
double krt=0.;
double fas=0.;
int kodt=0;
double cenav=0.;
class iceb_tu_str shbm("");
short denv,mesv,godv; //Дата ввода в эксплуатацию для малоценки
class iceb_tu_str innom(""); //Инвентарный номер для малоценки
class iceb_tu_str rnd(""); //Регистрационный номер мед. препарата

char		mkl[10][128];
char		mst[10][128];
char		bros[1024];
SQL_str         row;
char		strsql[1024];
short           voz=0;
double          cenaz;
class iceb_tu_str shbmz("");
class iceb_tu_str eiz("");
class iceb_tu_str eipr(""); //Единица измерения для приходного документа
double		nds,ndscen;
int		i,i1;
double		bb,bb1;
int		N,K;
int		mndsv;
short		mvo;/*1-по карточке 2-по документам*/
double		ostnak,ostkart;
short		mreal=0; /*Метка передачи в розничную торговлю*/
class iceb_tu_str naimat("");
int		kgrm=0;
int		skl1=0,nkv=0;
double		cena=0.;
class iceb_tu_str naigr("");
int		nomkarp=0; //Номер карточки парного документа
double		suma=0.,sumasnds=0.;
class iceb_tu_str nomz("");
class iceb_tu_str kod("");
double		kolihpr=kolih; //Количество в приходном документе
short deng,mesg,godg;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
struct OPSHET shetv;
SQLCURSOR cur;
sprintf(strsql,"select vido from Rashod where kod='%s'",kodop);
if(readkey(strsql,&row,&cur) == 1)
 mvnp=atoi(row[0]);

//Читаем реквизиты карточки с которых оформляется расход
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",skl,nk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d %s %d !",
  gettext("Не найдена карточка"),nk,
  gettext("на складе"),skl);

  iceb_t_soob(strsql);
  return(1);
 }
km=atoi(row[2]);
cnn=atof(row[6]); //Цена
ei.new_plus(row[4]);
mnds=atoi(row[3]);
ndsg=atof(row[9]);

krt=atof(row[8]);
kodt=atoi(row[11]);
fas=atof(row[10]);
cena=okrug(cena,okrcn);
cenav=atof(row[7]);
shbm.new_plus(row[5]);
rsdat(&denv,&mesv,&godv,row[14],2);
innom.new_plus(row[15]);
rnd.new_plus(row[16]);
nomz.new_plus(row[17]); //Передается в списке параметров и размер неизвестен
rsdat(&deng,&mesg,&godg,row[18],2);

int kodopuc=0; //Метка операции списания по учетной цене

if(iceb_t_poldan("Коды операций расхода по учетным ценам",bros,"matnast.alx") == 0)
 if(proverka(bros,kodop,0,1) == 0)
   kodopuc=1;  //Списание по учетной цене

i=i1=0;
memset(mkl,'\0',sizeof(mkl));
memset(mst,'\0',sizeof(mst));

sprintf(mkl[i++],"F1");
sprintf(mst[i1++],gettext("помощь"));

sprintf(mkl[i++],"F2/+");
sprintf(mst[i1++],gettext("запись"));

if(ATOFM(kpos) == 0. && POLE(kpos,bros,2,'-') == 0 && kpos[0] == '0')
 {
  sprintf(mkl[i++],"F3");
  sprintf(mst[i1++],gettext("номер карт."));
 }

sprintf(mkl[i++],"F4");
sprintf(mst[i1++],gettext("прос.карт."));

sprintf(mkl[i++],"F5");
sprintf(mst[i1++],gettext("остат."));
sprintf(mkl[i++],"F10");
sprintf(mst[i1++],gettext("выход"));



VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Количество......."));  //0
VV.VVOD_SPISOK_add_MD(gettext("Цена с НДС......."));  //1
VV.VVOD_SPISOK_add_MD(gettext("Цена............."));  //2
VV.VVOD_SPISOK_add_MD(gettext("Сумма без НДС....")); //3
VV.VVOD_SPISOK_add_MD(gettext("Количество тары.."));  //4
VV.VVOD_SPISOK_add_MD(gettext("Счёт списания...."));  //5
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(10);

if(mdd == 1)
 {
  VV.VVOD_SPISOK_add_MD(gettext("Номер карточки...")); //6
  VV.VVOD_SPISOK_add_MD(gettext("Един. измерения..")); //7
  VV.VVOD_SPISOK_add_MD(gettext("НДС..............")); //8
  VV.VVOD_SPISOK_add_data(10); 
  VV.VVOD_SPISOK_add_data(10); 
  VV.VVOD_SPISOK_add_data(10);
 }

N=0;

if(kmm != 0)
 {
  sprintf(strsql,"update Dokummat1 \
set \
nomkar=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=0",nk,gd,md,dd,skl,nomdok,km);

  sql_zapis(strsql,1,0);

  return(0);
 }   

mndsv=0;
ndscen=bb=0.;

/*Читаем цену продажи из списка материалов*/
sprintf(strsql,"select cenapr,kodgr,naimat,nds from Material where kodm=%d",km);

if((i=sql_readkey(&bd,strsql,&row,&cur)) != 1)
 {
  VVOD SOOB(1);

//  sprintf(bros,"%s",sql_error(&bd));
  SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));

  sprintf(strsql,"%s %d !",gettext("Не нашли код материалла"),km);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(bros,"i=%d strsql=%s",i,strsql);
  SOOB.VVOD_SPISOK_add_MD(bros);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
else
 {
  bb=atof(row[0]); 
  kgrm=atoi(row[1]);
  naimat.new_plus(row[2]);
  ndscen=atof(row[3]); /*0-цена с НДС 1-без НДС*/
 }

//if(cnsnds == 1 && bb != 0. && ndscen == 0.)
if( bb != 0. && ndscen == 0.)
 {
  bb=bb-bb*pnds/(100.+pnds);
  bb=okrug(bb,okrcn);
 }

if(bb == 0. || mvnp == 1 || kodopuc == 1)
 {
  bb=cnn;
 } 

sprintf(strsql,"%.10g",bb);
VV.VVOD_SPISOK_zapis_data(2,strsql);
bb=bb+bb*pnds/100.;
bb=okrug(bb,okrg1);
sprintf(strsql,"%.10g",bb);
VV.VVOD_SPISOK_zapis_data(1,strsql);

if(mdd == 1)
 {
  if(ATOFM(kpos) == 0. && POLE(kpos,bros,2,'-') == 0 && kpos[0] == '0')
    skl1=atoi(bros);

  VV.VVOD_SPISOK_zapis_data(5,shbm.ravno());

  sprintf(strsql,"%d",nomkr(skl1));
  VV.VVOD_SPISOK_zapis_data(6,strsql);

  VV.VVOD_SPISOK_zapis_data(7,ei.ravno());

  memset(strsql,'\0',sizeof(strsql));
  if(mnds == 1)
    sprintf(strsql,"+%.5g",ndsg);
  else
    sprintf(strsql,"%.5g",ndsg);
  VV.VVOD_SPISOK_zapis_data(8,strsql);
 }

/*Остаток по карточкам*/
ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
ostkart=ost.ostg[3];

/*Остаток по документам*/
ostdok(1,1,gt,31,mt,gt,skl,nk,&ost);

mvo=2;
ostnak=ost.ostg[3];
  
cenaz=cnn;  /*Запоминаем цену карточки*/
shbmz.new_plus(shbm.ravno());
eiz.new_plus(ei.ravno());
eipr.new_plus(ei.ravno());

naz1:;

clear();

vivost(6,0,mvo,&ost);

voz=0;
move(0,0);

printw("%s.\n",gettext("Расход"));

move(1,0);
printw(gettext("Материал - %d/%s"),km,naimat.ravno());

if(kgrm != 0)
 {
  sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найдено группу"),kgrm);
    iceb_t_soob(strsql);
   }
  else
   naigr.new_plus(row[0]);
 }
 
move(2,0);
printw(gettext("Группа    - %d %s"),kgrm,naigr.ravno());

move(3,0);
printw(gettext("Склад:%d Карт.:%d Счёт:%s Ед. изм.:%s Цена:%.10g Фас.:%.10g Крат.:%.10g"),
skl,nk,shbm.ravno(),ei.ravno(),cnn,fas,krt);

move(4,0);

if(mnds == 1)
  attron(A_BLINK); /*Включение мерцания*/
 
if(mnds == 1)
 printw("%s +%.6g",gettext("НДС:"),ndsg);
else
 printw("%s %.6g",gettext("НДС:"),ndsg);

attroff(A_BLINK); /*Выключение мерцания*/


naz:;

if(mdd == 1)
 if(mreal == 0)
  if(proverka(shrt.ravno(),VV.VVOD_SPISOK_return_data(5),0,1) == 0)
   { 
    VV.VVOD_SPISOK_add_MD(gettext("Цена реализации.."));
    VV.VVOD_SPISOK_add_data(20);
    if(cenav != 0.)
     {
      sprintf(strsql,"%.10g",cenav);
      VV.VVOD_SPISOK_zapis_data(9,strsql);
     }

    mreal=1;
   }

if(mreal == 1)
  if(proverka(shrt.ravno(),VV.VVOD_SPISOK_return_data(5),0,1) != 0)
   {
    VV.VVOD_SPISOK_zapis_data(8,"0");
   }
move(10,0);


if(bb == 0.)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Не введена цена продажи. Взята цена из карточки. "));
  attroff(A_BLINK); /*Выключение мерцания*/
 } 



attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
attroff(A_REVERSE);  /*Выключить инверсию*/


helstr(LINES-1,0,mkl[0],mst[0],mkl[1],mst[1],mkl[2],mst[2],mkl[3],mst[3],
mkl[4],mst[4],mkl[5],mst[5],NULL);


VV.vramka(11,0,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  mvprintw(10,COLS-30," %s:%.2f/%.2f",gettext("Сумма"),suma,sumasnds);

  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);
  
  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    if(mdd == 1 && N == 7) //Единица измерения
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      class iceb_tu_str naimkod("");
      if((i1=vvodkod("Edizmer",&kod,&naimkod,-1,-1,-1)) == 0)
        goto naz1;

      VV.VVOD_SPISOK_zapis_data(N,kod.ravno());
      eipr.new_plus(kod.ravno());      
      N++;
      goto naz1;

     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Ввод количества*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));

      suma=bb*ATOFM(VV.VVOD_SPISOK_return_data(2));
      
      sprintf(strsql,"%.10g",suma);
      VV.VVOD_SPISOK_zapis_data(3,strsql);

      sumasnds=bb*ATOFM(VV.VVOD_SPISOK_return_data(1));

      if(bb != 0. && krt != 0.)
       {
	bb=bb/krt;
	bb=okrug(bb,0.01);
	sprintf(strsql,"%.10g",bb);
        VV.data_plus(4,strsql);
       }
      N++;
      goto naz;
     }

    if(N == 1) /*Ввод цены с ндс*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      /*Ввод цены с процентом наценки*/
      if(POLE(VV.VVOD_SPISOK_return_data(N),bros,2,'/') == 0)
       {
	bb1=ATOFM(bros);
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
	bb=bb+bb*bb1/100.;
       }
      else
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      sumasnds=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);

      bb=bb-bb*pnds/(100.+pnds);
      bb=okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql);

      suma=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));
      sprintf(strsql,"%.10g",suma);
      VV.VVOD_SPISOK_zapis_data(3,strsql);

      N++;
      goto naz;
     }

    if(N == 2) /*Ввод цены без НДС*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      /*Ввод цены с процентом наценки*/
      if(POLE(VV.VVOD_SPISOK_return_data(N),bros,2,'/') == 0)
       {
	bb1=ATOFM(bros);
	bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
	bb=bb+bb*bb1/100.;
       }
      else
        bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      bb=okrug(bb,okrcn);

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql);
      
      suma=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));

      sprintf(strsql,"%.10g",suma);
      VV.VVOD_SPISOK_zapis_data(3,strsql);
      
      bb=bb+bb*pnds/100.;
      bb=okrug(bb,okrg1);
      sumasnds=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);
      N++;  
      goto naz;

     }

    if(N == 3) //Ввод суммы без НДС
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      suma=ATOFM(VV.VVOD_SPISOK_return_data(N));

      bb=suma/ATOFM(VV.VVOD_SPISOK_return_data(0));
      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(2,strsql);

      bb=bb+bb*pnds/100.;
      bb=okrug(bb,okrg1);
      sumasnds=bb*ATOFM(VV.VVOD_SPISOK_return_data(0));

      sprintf(strsql,"%.10g",bb);
      VV.VVOD_SPISOK_zapis_data(1,strsql);

      N++;  
      goto naz;

     }

    if(N == 4) /*Количество тары*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      bb=ATOFM(VV.VVOD_SPISOK_return_data(N));
      bb=bb*krt;
      bb=okrug(bb,0.000001);
      sprintf(strsql,"%.10g",bb);
      VV.data_plus(0,strsql);
      goto naz;
       
     }

    if(N == 5) /*Счёт учета*/
     {
      prsh1(VV.VVOD_SPISOK_return_data(N),&shetv);
      if(mdd == 1) //Только для внутреннего перемещения
       N++;
      goto naz;
     }


    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
//    VV.vprintw_clear(N,stdscr);

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
//  attron(VV.VVOD_return_cs(iceb_CFS)); 
//  VV.vprintw_clear(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }

   if(K == FK1) //Помощь
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     GDITE();
     if(mdd == 0) /*Одинарный документ*/
      sprintf(strsql,"%s","matu2_1_2j.txt");
     if(mdd == 1) /*Двойной документ*/
      sprintf(strsql,"%s","matu2_1_2j1.txt");
     iceb_t_pdoc(strsql);
     clear();
     goto naz1;
    }
    
   if(K == FK2 || K == PLU) /*Записываем*/
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
    
     if( VV.VVOD_SPISOK_return_data(0)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введено количество !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена цена !"));
       goto naz;
      }

     if(mreal == 1 && VV.VVOD_SPISOK_return_data(9)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена цена реализации !"));
       goto naz;
      }

     if(mdd == 0) //Одинарный документ
      {
       /*Проверяем счет*/
       if(VV.VVOD_SPISOK_return_data(5)[0] != '\0')
        {
         if(prsh1(VV.VVOD_SPISOK_return_data(5),&shetv) != 0)
          goto naz;

        }
      }

     if(mdd == 1) /*Двойной документ*/
      {
       if( VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(6)[0] == '\0' || VV.VVOD_SPISOK_return_data(7)[0] == '\0')
        {
         iceb_t_soob(gettext("Не введены все реквизиты !"));
         goto naz;
        }

       if(skl == skl1)
	if(cenaz == ATOFM(VV.VVOD_SPISOK_return_data(2))  && SRAV(shbmz.ravno(),VV.VVOD_SPISOK_return_data(5),0) == 0  &&
	 SRAV(eiz.ravno(),VV.VVOD_SPISOK_return_data(7),0) == 0 )
          {
           if(mreal == 0)
            {
             iceb_t_soob(gettext("Перемещение внутри склада. Не изменен ни один реквизит !"));
             goto naz;
            }
           else
            {
             if(fabs(cenav - VV.data_ravno_atof(9)) < 0.01)
              {
               printw("\n%f %f\n",cenav,VV.data_ravno_atof(9));
               iceb_t_soob(gettext("Перемещение внутри склада. Не изменен ни один реквизит !"));
               goto naz;
              }

            }
          }
       //Проверяем номер карточки
       if( atoi(VV.VVOD_SPISOK_return_data(6)) < 0)
        {
         iceb_t_soob(gettext("Номер карточки не может быть отрицательным !"));
         goto naz;
        }       
       /*Проверяем счет*/
       if(prsh1(VV.VVOD_SPISOK_return_data(5),&shetv) != 0)
         goto naz;

       }
    
    
      kolih=ATOFM(VV.VVOD_SPISOK_return_data(0));
      cena=ATOFM(VV.VVOD_SPISOK_return_data(2));
      cena=okrug(cena,okrcn);
      if(mreal == 1)
        cenav=ATOFM(VV.VVOD_SPISOK_return_data(9));

      /*Остаток по карточкам*/
      ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
      ostkart=ost.ostg[3];

      /*Остаток по документам*/
      ostdok(1,1,gt,31,mt,gt,skl,nk,&ost);
      ostnak=ost.ostg[3];
      mvo=2;
               
      if(ostnak - kolih < -0.0001 || ostkart-kolih < -0.0001)
       {
        move(LINES-4,0);
        printw(gettext("По карточке:%.10g  По документам:%.10g"),ostnak-kolih,ostkart-kolih);
        
        memset(bros,'\0',sizeof(bros));
        if(iceb_t_poldan("Запрет привязки к карточкам с недостаточным остатком для списания",bros,"matnast.alx") == 0)
         if(SRAV(bros,"Вкл",1) == 0)
          {
           iceb_t_soob(gettext("Расход больше остатка на карточке !"));
           goto naz;
          }
        
        VVOD DANET(1);

        DANET.VVOD_SPISOK_add_MD(gettext("Расход больше остатка на карточке !"));
        DANET.VVOD_SPISOK_add_MD(gettext("        Ввести расход ?"));

        if(danet(&DANET,2,stdscr) == 2)
          goto naz1;
       }
      /*Внутреннее перемещение*/
      if(mdd == 1)
       {
        mndsv=0;
	if(VV.VVOD_SPISOK_return_data(8)[0] == '+')
          mndsv=1;
	nds=ATOFM(VV.VVOD_SPISOK_return_data(8));
        nds=okrug(nds,0.01);
        nkv=(int)ATOFM(VV.VVOD_SPISOK_return_data(6));
           
       /*Проверяем может в выходном документе уже есть этот материал
        с такойже ценой. Если есть то просто увеличиваем количество*/
        if(innom.ravno()[0] == '\0')
         {        
          sprintf(strsql,"select nomkar from Dokummat1 where \
datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
          gd,md,dd,skl1,nomdokp,km,cena);
         
         if(sql_readkey(&bd,strsql,&row,&cur) == 1)
          {
           nomkarp=atoi(row[0]);
           //Проверяем совпадают ли счета учета
           sprintf(strsql,"select shetu from Kart where nomk=%d and \
sklad=%d",nomkarp,skl1);
           if(sql_readkey(&bd,strsql,&row,&cur) == 1)
            {
             if(SRAV(VV.VVOD_SPISOK_return_data(5),row[0],0) != 0)
               goto vpered;
            }

           VVOD SOOB(1);
           SOOB.VVOD_SPISOK_add_MD(gettext("В выходном документе уже есть этот материал !"));
           SOOB.VVOD_SPISOK_add_MD(gettext("Увеличиваем его количество"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           
           sprintf(strsql,"update Dokummat1 set \
kolih=kolih+%.10g where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
           kolih,gd,md,dd,skl1,nomdokp,km,cena);

           sql_zapis(strsql,1,0);

           /*Делаем запись в расходный документ*/
           zapvdok(dd,md,gd,skl,nk,km,nomdok,kolih,cenaz,ei.ravno(),nds,mnds,0,2,nomkarp,VV.data_ravno(5),nomz.ravno(),0,"");
           voz=0;
           break;
          }
        }
vpered:;

       kolihpr=kolih;

       if(SRAV(eiz.ravno(),VV.VVOD_SPISOK_return_data(7),0) != 0 )
        {
         class iceb_tu_str vstr("");
         VVOD VVOD1(2);
         VVOD1.VVOD_SPISOK_add_MD(gettext("Изменена единица измерения"));
         VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество для приходного документа"));
         memset(strsql,'\0',sizeof(strsql));
         int kom1=0;
         if((kom1=vvod1(&vstr,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
           goto naz;
         if(kom1 == ESC)
           goto naz;
         kolihpr=vstr.ravno_atof();
        }

        VVOD SOOB(1);

        SOOB.VVOD_SPISOK_add_MD(gettext("Внутреннее перемещение"));
        
        /*Проверяем есть ли карточка с такой ценой
        если есть узнаем ее номер, если нет определяем
        последний номер карточки по этому складу*/

        //Округляем также как при записи
        cena=okrug(cena,okrcn);
        nds=okrug(nds,0.01);
        krt=okrug(krt,0.000001);
        cenav=okrug(cenav,okrcn);

        if(innom.ravno()[0] != '\0' || nomz.ravno()[0] != '\0')
         goto zapkr;
         
        sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.6g \
and cenap=%.10g and krat=%.6g and mnds=%d and dat_god='%04d-%02d-%02d'",
        km,skl1,VV.VVOD_SPISOK_return_data(5),cena,eipr.ravno(),nds,cenav,krt,mndsv,godg,mesg,deng);

//        mvprintw(LINES-10,0,"%s\n",strsql);
//        OSTANOV();

        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
//          mk=1;
          nkv=atoi(row[0]);
          sprintf(strsql,gettext("Такой материал есть в карточке %d, записываем туда"),nkv);
          SOOB.VVOD_SPISOK_add_MD(strsql);
         }
        else
         {
          /*Проверяем нет ли уже карточки с таким номером*/
          sprintf(strsql,"select nomk from Kart where sklad=%d \
and nomk=%d",skl1,atoi(VV.VVOD_SPISOK_return_data(6)));
          if(sql_readkey(&bd,strsql,&row,&cur) == 1)
           {
            sprintf(strsql,gettext("Карточка %s есть на складе %d с другим материалом !"),
             VV.VVOD_SPISOK_return_data(6),skl1);
            SOOB.VVOD_SPISOK_add_MD(strsql);
            soobshw(&SOOB,stdscr,-1,-1,0,1);
            
            goto naz;
           }
zapkr:;
          sprintf(strsql,gettext("Делаем запись в карточку %s на складе %d"),
          VV.VVOD_SPISOK_return_data(6),skl1);
          SOOB.VVOD_SPISOK_add_MD(strsql);

          if(zapkar(skl1,nkv,km,cena,eipr.ravno(),VV.VVOD_SPISOK_return_data(5),krt,fas,kodt,nds,mndsv,cenav,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),deng,mesg,godg,0) != 0)
            goto naz;
         }

       sprintf(strsql,gettext("Номер встречного документа - %s"),nomdokp);
       SOOB.VVOD_SPISOK_add_MD(strsql);
       soobshw(&SOOB,stdscr,-1,-1,0,1);

       /*Делаем запись в расходный документ*/
       zapvdok(dd,md,gd,skl,nk,km,nomdok,kolih,cenaz,ei.ravno(),nds,mnds,0,2,nkv,VV.data_ravno(5),nomz.ravno(),0,"");

/*       
       printw("%d.%d.%d skl1:%d nkv:%d km:%d nomdokp:%s kolih:%f cena:%f\n",
       dd,md,gd,skl1,nkv,km,nomdokp,kolih,cena);
       OSTANOV();
*/
       /*Делаем запись в приходный документ*/
        zapvdok(dd,md,gd,skl1,nkv,km,nomdokp,kolihpr,cena,eipr.ravno(),nds,mndsv,0,1,nk,shbmz.ravno(),nomz.ravno(),0,"");
       }
      else
       {
        
        zapvdok(dd,md,gd,skl,nk,km,nomdok,kolih,cena,ei.ravno(),ndsg,mnds,0,2,0,VV.data_ravno(5),nomz.ravno(),0,"");
       }
     voz=0;
     break;
     
    }
  
   if(K == FK3) /*Номер карточки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS)); 
     if(mdd == 1)
      {
       GDITE();
//       sprintf(VV.VVOD_SPISOK_return_data(6),"%d",nomkr(skl1));
       VV.data_plus(6,nomkr(skl1));
       goto naz;
      }
     goto naz;
    }
   if(K == FK4) /*Просмотр карточки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS)); 
     clear();

     dirzkart(skl,nk);
     goto naz1;
    }   

   if(K == FK5) /*Остатки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS)); 
     if(mvo == 1)
      {
       /*Остаток по документам*/
       ostdok(1,1,gt,31,mt,gt,skl,nk,&ost);
       mvo=2;
       goto naz1;
      }
     if(mvo == 2)
      {
       /*Остаток по карточкам*/
       ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
       mvo=1;
      }     
     goto naz1;
    }   
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();

return(voz);
}
