/* $Id: pld.c,v 5.106 2014/04/30 06:12:26 sasa Exp $ */
/*15.07.2015     18.06.1997       Белых А.И.      pld.c
Просмотр готового платежного поручения
Если вернули 0 - платежка не записана
             1 - записана
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"
#include        "dok4.h"

int		udpld(const char*,short,short,short,const char*,short,const char*,int,int);
int		zappdok(const char*);
void		printpp(char*,char*,char*,char*,char*,double,short,short,FILE*);
int             dirprpd(const char*);
void            dirpzpd(const char*);
void		klient(const char*,short,short,short,const char*);
int	        vibshbn(char*,const char*);

extern class REC rec;
extern double   okrg1; /*Округление*/
extern char     *imafkomtmp; //Имя файла куда выгружается комментарий для редактирования
extern class iceb_tu_str nameprinter;

void pld(short kp, //Количество повторов
const char *tabl) //Таблица документа
{
class iceb_tu_str printer("lpr -r");
class iceb_tu_str printerloc("prterm -r");
char dt2[128]; //дата платежа
class iceb_tu_str bros("");
char            pr[1024];
char            prom[1024];
char            str[1024];
extern short    mvs;       /*1-с разбивкой суммы по счетам 0-без*/
extern double   smp0,smp1; /*Суммы за переводы внутри банка и вне банка*/
class iceb_tu_str shban("");   /*Счёт банка за услуги*/
FILE            *ff=NULL;
int           K,kom,kom1;
short           i;
char            imaf[64];
short           kolp;  /*Количество повторов*/
short		d,m,g;
short		dp,mp,gp;
time_t		vrem;
double		sump,sumpp;   /*Подтвержденная сумма*/
class iceb_tu_str mescc("");
class iceb_tu_str kodd("");
class iceb_tu_str naim("");
double		sh;
SQL_str         row;
int		poz,komv;
SQLCURSOR       cur;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[2048];
double nds1=iceb_t_pnds(rec.dd,rec.md,rec.gd);
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");


VVOD MENU(3);
VVOD VV(0);
VVOD SOOB(1);
VVOD DANET(1);

d=m=g=dp=mp=gp=0;

if(rec.nomdk_i.getdlinna() <= 1)
 {
  dp=mp=gp=0;
  iceb_t_poldan("Банковский счёт",&shban,"nastdok.alx");
  rec.shbzu.new_plus(shban.ravno());
 }
else
 {
  dp=rec.dd; mp=rec.md; gp=rec.gd;
  shban.new_plus(rec.shbzu.ravno());
 }

naz:;

if(rec.nomdk_i.getdlinna() <= 1)
 {
  rec.uslb=smp0;
  if(SRAV(rec.mfo.ravno(),rec.mfo1.ravno(),0) != 0)
    rec.uslb=smp1;
 }

naz1:;

clear(); /*Очистить экран и закрасить фоновым цветом*/
refresh();
//Месяц платежа преобразуем в название
memset(dt2,'\0',sizeof(dt2));
if(rec.dd != 0) //Для типовых документов будет равен 0
 {
  mesc(rec.md,1,&mescc);
  sprintf(dt2,"%02d %s %d%s",rec.dd,mescc.ravno(),rec.gd,gettext("г."));
 }
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
 {

  sprintf(strsql,"select str from Alx where fil='pp.alx' order by ns asc");
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }

  if(kolstr == 0)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"pp.alx");
    iceb_t_soob(strsql);
    return;
   }

 }
if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
 {

  sprintf(strsql,"select str from Alx where fil='pt.alx' order by ns asc");
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }

  if(kolstr == 0)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"pt.alx");
    iceb_t_soob(strsql);
    return;
   }
 }
memset(str,'\0',sizeof(str));
//attron(A_ALTCHARSET); /*Альтернативная таблица*/
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  /*Такой сложный алгоритм нужен для коректного вывода на экран маленького размера*/
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"%.*s",iceb_tu_kolbait(COLS-1,row_alx[0]),row_alx[0]);
  
  printw("%s",strsql);
    
  if(pole2(strsql,'\n') == 0)
   printw("\n");  
  
  memset(str,'\0',sizeof(str));
 }
//attroff(A_ALTCHARSET); /*Альтернативная таблица*/


if(rec.prov == 1)
 {
  move(0,COLS-25);
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Проводки не сделаны !"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

move(1,COLS-25);
printw("%s%d %s:%.2f",gettext("Кол.копий:"),
kp,gettext("НДС"),rec.nds);

sump=0;
if(rec.ddi != 0 && (SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0))
  sump=sumpzpd(tabl,&dp,&mp,&gp,&sumpp);

if(fabs(sump-(rec.sumd+rec.uslb)) > 0.0099)
  attron(A_BLINK); /*Влючение мерцания*/
move(2,COLS-25);
printw("%s%.2f",gettext("Сумма подт.:"),sump);
attroff(A_BLINK); /*Выключение мерцания*/

move(3,COLS-25);
printw("%s:%s",gettext("Операция"),rec.kodop.ravno());
move(4,COLS-25);
printw("Коди конт.:%s / %s",rec.kodor.ravno(),rec.kodor1.ravno());

move(0,39);
printw("%s",rec.nomdk.ravno());
if(SRAV(tabl,"Tpltp",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
  printw("  (типова)");
move(1,18);
printw("%s",dt2);

move(2,8);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%.*s",iceb_tu_kolbait(40,rec.naior.ravno()),rec.naior.ravno());
else
  printw("%.*s",iceb_tu_kolbait(40,rec.naior1.ravno()),rec.naior1.ravno());

move(4,11);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.kod.ravno());
else
  printw("%s",rec.kod1.ravno());

if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  sprintf(prom,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
else
  sprintf(prom,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());

sh=rec.sumd+rec.uslb;

//if(rzk == 0)
//    prnb(sh,pr);
//if(rzk == 1)
    prnb1(sh,pr);

move(7,0);
printw("%-*.*s",
iceb_tu_kolbait(29,prom),
iceb_tu_kolbait(29,prom),
prom);

move(7,30);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.mfo.ravno());
else
  printw("%s",rec.mfo1.ravno());

move(7,48);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.nsh.ravno());
else
  printw("%s",rec.nsh1.ravno());

move(7,65);
printw("%s",pr);

move(9,10);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%.*s",iceb_tu_kolbait(53,rec.naior1.ravno()),rec.naior1.ravno());
else
  printw("%.*s",iceb_tu_kolbait(53,rec.naior.ravno()),rec.naior.ravno());

move(11,11);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.kod1.ravno());
else
  printw("%s",rec.kod.ravno());

pr[0]='\0';
if(rec.uslb != 0. && mvs == 1)
 {
    sh=rec.sumd;
//    if(rzk == 0)
//       prnb(sh,pr);
 
//    if(rzk == 1)
       prnb1(sh,pr);
 }

move(12,48);
if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.nsh1.ravno());
else
  printw("%s",rec.nsh.ravno());

move(12,65);
printw("%s",pr);

if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  sprintf(prom,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
else
  sprintf(prom,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());

pr[0]='\0';

if(rec.uslb != 0. && mvs == 1)
 {
//  if(rzk == 0)
//    prnb(rec.uslb,pr);

//  if(rzk == 1)
    prnb1(rec.uslb,pr);

 }
move(14,0);
printw("%-*.*s",
iceb_tu_kolbait(29,prom),
iceb_tu_kolbait(29,prom),
prom);
move(14,30);

if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
  printw("%s",rec.mfo1.ravno());
else
  printw("%s",rec.mfo.ravno());

move(14,48);
printw("%s",shban.ravno());

move(14,65);
printw("%s",pr);

sh=rec.sumd+rec.uslb;

memset(str,'\0',sizeof(str));
preobr(sh,str,0);

move(17,0);
printw("%s",str);

/*Файл назначения платежа на экран*/
if((ff = fopen(imafkomtmp,"r")) == NULL)
 {
  error_op_nfil(imafkomtmp,errno,"");
  return;
 }

move(19,0);
for(i=0; i < LINES-20 && i < 5 ; i++)
 {
  if(fgets(str,sizeof(str),ff) == NULL)
   break;
  if(str[0] == '#')
   continue;

  sprintf(strsql,"%.*s",iceb_tu_kolbait(COLS-1,str),str);
  printw("%s",strsql);
  if(pole2(strsql,'\n') == 0)
   printw("\n");
//  printw("%.80s",str);

 }
fclose(ff);
/*
move(23,0);
if(SRAV(rec.mfo,rec.mfo1.ravno(),0) == 0)
  printw("МФО равны %.2f  %d (%.2f/%.2f)",smp,mvs,smp0,smp1);
else
  printw("МФО неравны %.2f %d (%.2f/%.2f)",smp,mvs,smp0,smp1);
  */

helstr(LINES-1,0,
"F1",gettext("пом."),
"F2",gettext("корек."),
"F3",gettext("удалить"),
"F4",gettext("пров."),
"F5",gettext("печать"),
"F6",gettext("смена сче."),
"F7",gettext("подтв."),
"F8",gettext("нов.док."),
"F9",gettext("пас"),
"F10",gettext("выход"),NULL);

K=getch();

if(K == KEY_MOUSE)
 {
  int N=0;
  int PY=0,PX=0;
  int KLST=0,MDLS=0;  
  K=mouseklic(&N,PY,PX,MDLS,KLST,stdscr);
 }
switch (K)
 {
  case ESC:
  case FK10:

    if(rec.ddi != 0 && (SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0))
       provpd(tabl);

    return;
    break;

  case FK1:   /*Помощь*/
    GDITE();
    iceb_t_pdoc("dokum2_1_1.txt");
    clear();
    goto naz;

  case FK2:
    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
      goto naz;
    if(iceb_t_pbpds(rec.mdi,rec.gdi) != 0)
      goto naz;
    
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Корректировать реквизиты документа"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Корректировать комментарий к документу"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Корректировать контрагента из списка базы"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Корректировать контрагента из списка файла"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


    kom=0;
    while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

    switch (kom)
     {
      case -1 :
	goto naz;
	break;

      case 0 :

        VV.VVOD_delete();

	VV.VVOD_SPISOK_add_MD(gettext("Сyмма платежа............"));//0
	VV.VVOD_SPISOK_add_MD(gettext("Счёт банка 1............."));//1
	VV.VVOD_SPISOK_add_MD(gettext("Счёт банка 2............."));//2
	VV.VVOD_SPISOK_add_MD(gettext("Сумма за услуги банка...."));//3
	VV.VVOD_SPISOK_add_MD(gettext("Номер документа.........."));//4
	VV.VVOD_SPISOK_add_MD(gettext("Дата платежа............."));//5
	VV.VVOD_SPISOK_add_MD(gettext("Операция................."));//6
	VV.VVOD_SPISOK_add_MD(gettext("НДС......................"));//7
        VV.VVOD_SPISOK_add_MD(gettext("Номер счета плательщика.."));//8
        VV.VVOD_SPISOK_add_MD(gettext("МФО плательщика.........."));//9
        VV.VVOD_SPISOK_add_MD(gettext("Дата валютирования......."));//10
        VV.VVOD_SPISOK_add_MD(gettext("Код нерезидента.........."));//11
        VV.VVOD_SPISOK_add_MD(gettext("Количество копий........."));//12
        VV.VVOD_SPISOK_add_MD(gettext("Код ЕГРПОУ получателя...."));//13
        VV.VVOD_SPISOK_add_MD(gettext("МФО получателя..........."));//14
        VV.VVOD_SPISOK_add_MD(gettext("Наименование банка получ."));//15

        VV.VVOD_SPISOK_add_data(20);//0
        VV.VVOD_SPISOK_add_data(30);//1
        VV.VVOD_SPISOK_add_data(30);//2
        VV.VVOD_SPISOK_add_data(20);//3
        VV.VVOD_SPISOK_add_data(10);//4
        VV.VVOD_SPISOK_add_data(20);//5
        VV.VVOD_SPISOK_add_data(12);//6
        VV.VVOD_SPISOK_add_data(10);//7
        VV.VVOD_SPISOK_add_data(20);//8
        VV.VVOD_SPISOK_add_data(10);//9
        VV.VVOD_SPISOK_add_data(11);//10
        VV.VVOD_SPISOK_add_data(10);//11
        VV.VVOD_SPISOK_add_data(4); //12
        VV.VVOD_SPISOK_add_data(20);//13
        VV.VVOD_SPISOK_add_data(10);//14
        VV.VVOD_SPISOK_add_data(256);//15

	sprintf(strsql,"%.2f",rec.sumd);
	VV.data_plus(0,strsql);
	VV.data_plus(1,rec.nsh1.ravno());
	VV.data_plus(2,shban.ravno());
	if(rec.uslb != 0.)
	  VV.data_plus(3,rec.uslb);

	VV.data_plus(4,rec.nomdk.ravno());
	
	sprintf(strsql,"%d.%d.%d",rec.dd,rec.md,rec.gd);
	VV.data_plus(5,strsql);
	
	VV.data_plus(6,rec.kodop.ravno());
        if(rec.nds != 0.)
          VV.data_plus(7,rec.nds);
        VV.data_plus(8,rec.nsh.ravno());
        VV.data_plus(9,rec.mfo.ravno());
        if(rec.dv != 0)
         {
          sprintf(strsql,"%d.%d.%d",rec.dv,rec.mv,rec.gv);
          VV.data_plus(10,strsql);
         }
        VV.data_plus(11,rec.kodnr.ravno());
        VV.data_plus(12,kp);
        VV.data_plus(13,rec.kod1.ravno());
        VV.data_plus(14,rec.mfo1.ravno());
        VV.data_plus(15,rec.naiban1.ravno());
obr:;

        helstr(LINES-1,0,"F2/+",gettext("запись"),
        "F3",gettext("операции"),
        "F4",gettext("НДС"),
        "F10",gettext("выход"),NULL);
        

        kom1=VV.vvod(0,1,1,-1,-1);

        switch(kom1)
         {
          case FK10:
          case ESC:
             goto naz;

          case FK2:
          case PLU:
            break;
            
          case FK3:
            clear();
            kodd.new_plus("");
            naim.new_plus("");

            if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
              bros.new_plus("Opltp");
            if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
              bros.new_plus("Opltt");
            poz=komv=0;
            if(dirmat1(bros.ravno(),&kodd,&naim,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
             {
              VV.data_plus(6,kodd.ravno());
              clear();
              goto obr;
             }
            clear();
            goto obr;

          case FK4:

            rec.nds=ATOFM(VV.VVOD_SPISOK_return_data(0))*nds1/(nds1+100);
            rec.nds=okrug(rec.nds,okrg1);
            if(rec.nds != 0.)
               VV.data_plus(7,rec.nds);
            goto obr;

          default:
            goto obr;
                                
         }

        if((SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0) &&
         VV.VVOD_SPISOK_return_data(5)[0] == '\0')
         {
          iceb_t_soob(gettext("Не введена дата !"));
          goto obr;
         }  
        
        if((SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0) &&
         VV.VVOD_SPISOK_return_data(5)[0] != '\0')  
        if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(5),0) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата !"));
          goto obr;
         }  
        rec.dd=d;
        rec.md=m;
        rec.gd=g;
        
//        if(sravmydat(rec.dd,rec.md,rec.gd,rec.ddi,rec.mdi,rec.gdi) != 0)/*Если есть проводки по старой дате и они заблокированы, то менять дату нельзя*/
        if(rec.mdi != rec.md || rec.gdi != rec.gd) 
        if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0 )
         {
          int metka_t=0;
          if(SRAV(tabl,"Pltt",0) == 0 )
           metka_t=5;
          if(SRAV(tabl,"Pltp",0) == 0 )
           metka_t=4;
          /*Проверяем заблокированы ли проводки по обоим датам*/ 
          if(iceb_t_pvkdd(metka_t,rec.ddi,rec.mdi,rec.gdi,rec.md,rec.gd,0,rec.nomdk_i.ravno(),rec.tipz) != 0)
           goto obr;
         }

        if(VV.VVOD_SPISOK_return_data(10)[0] != '\0')  
         {
          if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(10),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата валютирования !"));
            goto obr;
           }  

          rec.dv=d;
          rec.mv=m;
          rec.gv=g;
         }

        if((SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0) &&
         VV.VVOD_SPISOK_return_data(6)[0] == '\0')
         {
          iceb_t_soob(gettext("Не введена операция !"));
          goto obr;
         }  

        if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
         {
          if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
           sprintf(strsql,"select naik from Opltp where kod='%s'",
           VV.VVOD_SPISOK_return_data(6));

          if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
           sprintf(strsql,"select naik from Opltp where kod='%s'",
           VV.VVOD_SPISOK_return_data(6));

          if(sql_readkey(&bd,strsql,&row,&cur) != 1)
           {
            sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(6));
            iceb_t_soob(strsql);
            goto obr;
           }
         }
        if(VV.VVOD_SPISOK_return_data(8)[0] == '\0')
         {
          iceb_t_soob(gettext("Не введён номер счета плательщика !"));
          goto obr;
         }  

        if(SRAV(VV.VVOD_SPISOK_return_data(4),rec.nomdk_i.ravno(),0) != 0)
         {
          if(rec.nomdk_i.getdlinna() > 1)
          if(SRAV(tabl,"Tpltt",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
           {
            iceb_t_soob(gettext("Для типовых документов номер документа корректировать нельзя !"));
            goto obr;
           }
          /*Проверяем может документ с таким номером уже есть*/
          sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",tabl,rec.gd,rec.gd,VV.VVOD_SPISOK_return_data(4));
          if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
           {
            sprintf(strsql,gettext("Номер документа %s уже есть !"),VV.VVOD_SPISOK_return_data(4));
            iceb_t_soob(strsql);
            goto obr;
           }          
         }

       if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
        {
         if(udpld(tabl,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz) != 0)
          goto obr;
         zappdok(tabl);
        }

        kp=(short)ATOFM(VV.VVOD_SPISOK_return_data(12));
        rec.kod1.new_plus(VV.VVOD_SPISOK_return_data(13));
        rec.mfo1.new_plus(VV.VVOD_SPISOK_return_data(14));
	rec.nomdk.new_plus(VV.VVOD_SPISOK_return_data(4));

        rec.sumd=ATOFM(VV.VVOD_SPISOK_return_data(0));
	rec.nsh1.new_plus(VV.VVOD_SPISOK_return_data(1));

	shban.new_plus(VV.VVOD_SPISOK_return_data(2));
	rec.shbzu.new_plus(VV.VVOD_SPISOK_return_data(2));

	rec.uslb=ATOFM(VV.VVOD_SPISOK_return_data(3));
         

	strcpy(dt2,VV.VVOD_SPISOK_return_data(5));
	rec.kodop.new_plus(VV.VVOD_SPISOK_return_data(6));
        rec.nds=ATOFM(VV.VVOD_SPISOK_return_data(7));
        rec.nds=okrug(rec.nds,0.01);
	rec.nsh.new_plus(VV.VVOD_SPISOK_return_data(8));
	rec.mfo.new_plus(VV.VVOD_SPISOK_return_data(9));
	rec.kodnr.new_plus(VV.VVOD_SPISOK_return_data(11));
        rec.naiban1.new_plus(VV.data_ravno(15));
	goto naz1;

      case 1 :

        vizred(imafkomtmp);
        if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
         {
          if(udpld(tabl,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),1,rec.tipz) == 0)
            zappdok(tabl);
         }
	goto naz;

      case 2 :
        vzrkbz();          
        if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
         {
          if(udpld(tabl,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz) == 0)
            zappdok(tabl);
         }
        goto naz;

      case 3 :
        if(iceb_redfil("platpor.alx",0) == 0)
          vzrkf();
        if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
         {
          if(udpld(tabl,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz) == 0)
            zappdok(tabl);
         }
        goto naz;

      default :
	goto naz;
     }
    goto naz;
    break;

  case FK3: //Удаление 
   
 //   sprintf(strsql,"%s",iceb_t_imafnsi("blokdok.alx"));
    if(iceb_t_pbpds(rec.md,rec.gd) != 0)
      goto naz;
   

    if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Pltt",0) == 0 )
     {
      int metka_ps=0;
      if(SRAV(tabl,"Pltp",0) == 0)
       metka_ps=4;
      if(SRAV(tabl,"Pltt",0) == 0)
       metka_ps=5;
      /*Смотрим заблокированы ли проводки к этому документа*/
      if(iceb_t_pbp(metka_ps,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.tipz,gettext("Удалить документ невозможно!")) != 0)
       goto naz;
     }


    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить документ ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(udpld(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.kodop.ravno(),1,rec.tipz) == 0)
        return;
     }
    goto naz;

  case FK4:  /* Проводки по платежке*/
    if(SRAV(tabl,"Tpltp",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
     {
      iceb_t_soob(gettext("Для типовых документов это не работает !"));
      goto naz;
     }
     if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Документ не записан !"));
      goto naz;
     }

    if(sump == 0.)
     {
      iceb_t_soob(gettext("Документ не подтвержден !"));
      goto naz;
     }

    if(dirprpd(tabl) == 0)
      return;

    goto naz;
               

  case FK7:
    if(SRAV(tabl,"Tpltp",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
     {
      iceb_t_soob(gettext("Для типовых документов не бывает подтверждающих записей !"));
      goto naz;
     }
    if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Документ не записан !"));
      goto naz;
     }

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(20);
    if(sump == 0.)
     {
      sprintf(strsql,"%d.%d.%d",rec.dd,rec.md,rec.gd);
      VV.data_plus(0,strsql);
     }
    VV.data_plus(1,rec.sumd+rec.uslb-sump);
    
    VV.VVOD_SPISOK_add_MD(gettext("Дата...."));
    VV.VVOD_SPISOK_add_MD(gettext("Сумма..."));
nz:;

    helstr(LINES-1,0,"F2/+",gettext("запись"),
    "F10",gettext("выход"),NULL);

    kom1=VV.vvod(0,1,1,-1,-1);
    switch(kom1)
     {
      case FK10:
      case ESC:
        goto naz;   

      case FK2:
      case PLU:
        if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(0),0) != 0)
         {
          SOOB.VVOD_delete();;
          SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата !"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
  	  goto nz;
         }

    //    sprintf(strsql,"%s",iceb_t_imafnsi("blokdok.alx"));
        if(iceb_t_pbpds(m,g) != 0)
          goto naz;

        if(SRAV1(g,m,d,rec.gd,rec.md,rec.dd) > 0)
         {
          iceb_t_soob(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"));
  	  goto nz;
         }

        if(ATOFM(VV.VVOD_SPISOK_return_data(1)) == 0.)
         {
          iceb_t_soob(gettext("Сумма равна нолю ! Подтверждение невозможно !"));
  	  goto nz;
         }

        time(&vrem);

        if(SRAV(tabl,"Pltp",0) == 0)
         sprintf(strsql,"insert into Pzpd \
  values (0,'%d-%02d-%02d','%d-%02d-%02d','%s',%s,%d,%ld)",
         rec.gd,rec.md,rec.dd,g,m,d,rec.nomdk.ravno(),VV.VVOD_SPISOK_return_data(1),iceb_t_getuid(),vrem);   

        if(SRAV(tabl,"Pltt",0) == 0)
         sprintf(strsql,"insert into Pzpd \
  values (1,'%d-%02d-%02d','%d-%02d-%02d','%s',%s,%d,%ld)",
         rec.gd,rec.md,rec.dd,g,m,d,rec.nomdk.ravno(),VV.VVOD_SPISOK_return_data(1),iceb_t_getuid(),vrem);

        if(sql_zapis(strsql,0,0) != 0)
         {
          goto nz;
         }

        provpd(tabl);

        break;    

      default:
        goto nz;

     }

    goto naz;

  case FK6: //Выбор счета
    char kontr[32];
    memset(kontr,'\0',sizeof(kontr));
    
    if(vibshbn(kontr,tabl) == 0)
     {
//      printw("\n%s %s %s\n",novshet,novmfo,novshbn);
//      OSTANOV();

//        printw("\nЧитаем из базы код=%s\n",kontr);
//        OSTANOV();
      sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb from \
Kontragent where kodkon='%s'",kontr);
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        SOOB.VVOD_delete();;
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
       }
      else
       {
        if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
         {
          rec.kodor.new_plus(kontr);

          if(polen(row[0],&rec.naior,1,'(') != 0)    
           rec.naior.new_plus(row[0]);
          rec.kod.new_plus(row[1]);
          rec.naiban.new_plus(row[2]);
          rec.mfo.new_plus(row[3]);
          rec.nsh.new_plus(row[4]);
          rec.gorod.new_plus(row[5]);
         }
        else
         {
          rec.kodor1.new_plus(kontr);

          if(polen(row[0],&rec.naior1,1,'(') != 0)    
           rec.naior1.new_plus(row[0]);
          rec.kod1.new_plus(row[1]);
          rec.naiban1.new_plus(row[2]);
          rec.mfo1.new_plus(row[3]);
          rec.nsh1.new_plus(row[4]);
          rec.gorod1.new_plus(row[5]);
         }
       }


     }
    goto naz;

  case SFK7:
    if(SRAV(tabl,"Tpltp",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
     {
      iceb_t_soob(gettext("Для типовых документов не бывает подтверждающих записей !"));
      goto naz;
     }

    dirpzpd(tabl);

    provpd(tabl);

    goto naz;

  case FK8:
    if(SRAV(tabl,"Tpltp",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
     {
      iceb_t_soob(gettext("Для типовых документов это не работает !"));
      goto naz;
     }
    clear();
    move(10,0);
    rec.nomdk.new_plus(iceb_t_nomdok(gt,tabl));
    mesc(mt,1,&mescc);
    sprintf(dt2,"%d %s %d%s",dt,mescc.ravno(),gt,gettext("г."));
    rec.dd=dt;
    rec.md=mt;
    rec.gd=gt;
    
    rec.ddi=0;
    rec.mdi=0;
    rec.gdi=0;
    rec.nomdk_i.new_plus("");
    
    sprintf(strsql,gettext("Делаем новый документ ! N %s Дата %s"),rec.nomdk.ravno(),dt2);
    iceb_t_soob(strsql);
    goto naz;

  case FK9: /*Установить метку передачи платёжки в банк-клиент*/

    if(SRAV(tabl,"Tpltp",0) == 0)
     {
      iceb_t_soob(gettext("Типовые платежки не передаются !"));
      goto naz;
     }
    if(SRAV(tabl,"Tpltt",0) == 0)
     {
      iceb_t_soob(gettext("Типовые требования не передаются !"));
      goto naz;
     }
    
    if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Документ не записан в журнал !"));
      goto naz;
     }
    sprintf(strsql,"update %s set vidpl='1' where datd='%04d-%02d-%02d' and nomd='%s'",
    tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
    sql_zapis(strsql,1,0);
    iceb_t_soob(gettext("Документ отмечен для передачи в банк"));
    goto naz;
    
  case FK5:
    MENU.VVOD_delete();
    memset(strsql,'\0',sizeof(strsql));

    sprintf(strsql,"%s:%-.20s",gettext("Принтер"),nameprinter.ravno());

    MENU.VVOD_SPISOK_add_ZAG(strsql);

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка на системном принтере"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатка на приэкранном принтере"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Запись в журнал регистрации"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Сделать файл распечатки"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр перед печатью"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

fff5:;
    kom=0;

   while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

   if(kom == -1 || kom == 6)
     goto naz;

    GDITE();

   if(kom == 5) //Смена принтера
    {
     mprintcap();
     goto fff5;
    }
  
    /*Проверяем номер платежного поручения*/
    if(SRAV(rec.nomdk.ravno(),rec.nomdk_i.ravno(),0) != 0)
     {

proverka:;

      sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",tabl,rec.gd,rec.gd,rec.nomdk.ravno());   

      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        helstr(LINES-1,0,
        "F4",gettext("номер докум."),
        "F10",gettext("выход"),NULL);
        VVOD VVOD1(2);
        sprintf(strsql,gettext("С номером %s документ уже есть !"),rec.nomdk.ravno());
        VVOD1.VVOD_SPISOK_add_ZAG(strsql);
        VVOD1.VVOD_SPISOK_add_MD(gettext("Введите новый номер документа"));

vvodnomer:;
        class iceb_tu_str vvstr(rec.nomdk.ravno());

        if((kom1=vvod1(&vvstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
          goto naz;
        rec.nomdk.new_plus(vvstr.ravno());       
        if(kom1 == FK4)
         {
          rec.nomdk.new_plus(iceb_t_nomdok(gt,tabl));
          goto vvodnomer;
         }

        goto proverka;          

       }
     }

    if(rec.kodop.getdlinna() <= 1)
     {
      iceb_t_soob(gettext("Не введён код операции!"));
      goto naz;
     }

    if(iceb_t_pbpds(rec.md,rec.gd) == 0)
     {
      //Удаляем предыдущий документ 
      if(rec.nomdk_i.getdlinna() > 1)
       udpld(tabl,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz);
      
      zappdok(tabl);
      dp=mp=gp=0;
      
      if(kom == 2)
        goto naz;
     }      

    
    i=(short)ATOFM(rec.nomdk.ravno());
    sprintf(imaf,"pp%d.lst",i);
    
    if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      return;
     }
    startfil(ff);
    for(kolp=0; kolp <kp ; kolp++)
     {
      if(SRAV(tabl,"Pltp",0) == 0 || SRAV(tabl,"Tpltp",0) == 0)
       if(platp(imafkomtmp,kp,kolp,ff,0) != 0)
        {
         fclose(ff);
         goto naz;
        }
      if(SRAV(tabl,"Pltt",0) == 0 || SRAV(tabl,"Tpltt",0) == 0)
       if(platp(imafkomtmp,kp,kolp,ff,1) != 0)
        {
         fclose(ff);
         goto naz;
        }
     }
   fclose(ff);
   
   if(kom == 3)
    {
     class spis_oth oth;
     oth.spis_imaf.plus(imaf);
     oth.spis_naim.plus(gettext("Распечатка платёжного документа"));
     iceb_t_rabfil(&oth,"");
     goto naz;
    }

   if(kom == 4) //Просмотр перед печатью
    {
     pros_fil(imaf,"");
     goto naz;
    }
   bros.new_plus("");
   if(kom == 0)
     bros.new_plus(printer.ravno());
   if(kom == 1)
     bros.new_plus(printerloc.ravno());

   pehf(imaf,1,bros.ravno());
   clear();
   goto naz;


  default:
    goto naz;
    break;
 }

}

/*************************************************************/
/*Работа с файлом платежек для передачи в систему Клиент-банк*/
/*************************************************************/
void	klient(const char *nomdok,short dd,short md,short gd,const char *tabl)
{
char	stt[2048];
FILE	*ff,*ff1;
char	imaf1[64];
char	imaf[64];
class iceb_tu_str bros("");
short	d,m,g;
class iceb_tu_str nomdok1("");
int	mt=0;
char	strsql[512];
class iceb_tu_str koment("");
/*
printw("\nklient-%s %d.%d.%d\n",nomdok,dd,md,gd);
refresh();
*/

/*
Читаем комментарий, внутри подпрограммы проверка на длинну комментария и 
выдача предупреждающего сообщения
*/
readkom(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment);

/*Проверяем нет ли платежки с таким номером*/

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
   goto vp;
  else
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
 }
sprintf(imaf1,"ttt%d.tmp",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
   {
    error_op_nfil(imaf1,errno,"");
    return;
   }

mt=0;
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
  polen(stt,&bros,1,'|');
  polen(stt,&nomdok1,2,'|');
  rsdat(&d,&m,&g,bros.ravno(),1);      
  if(sravmydat(d,m,g,dd,md,gd) == 0 && SRAV(nomdok,nomdok1.ravno(),0) == 0)
   {
    VVOD SOOB(1);;
    sprintf(strsql,gettext("Документ %s уже есть !"),rec.nomdk.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Удаляем старую запись"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    mt=1;
    continue;
   }   
  fprintf(ff1,"%s",stt);
 }

fclose(ff);
fclose(ff1);

if(mt == 1)
 {
  unlink(imaf);
  if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

  if((ff1 = fopen(imaf1,"r")) == NULL)
   {
    error_op_nfil(imaf1,errno,"");
    return;
   }
  while(fgets(stt,sizeof(stt),ff1) != NULL)
    fprintf(ff,"%s",stt);

  fclose(ff);
  fclose(ff1);
 }

unlink(imaf1);

vp:;
/*
printw(gettext("Запись документа N%s в файл для передачи.\n"),rec.nomdk.ravno());
refresh();
*/

if((ff = fopen(imaf,"a")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

fprintf(ff,"%02d.%02d.%d|%s\n",dd,md,gd,nomdok);

fclose(ff);

sprintf(strsql,gettext("Документ выгружен в файл %s !"),imaf);
iceb_t_soob(strsql);

}
