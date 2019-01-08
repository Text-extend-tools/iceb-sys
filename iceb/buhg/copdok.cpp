/* $Id: copdok.c,v 5.36 2013/05/17 14:55:56 sasa Exp $ */
/*13.07.2015    29.05.1997      Белых А.И.      copdok.c
Получение новой накладной на основание списка ранее полученных
накладных
*/
#include        <math.h>
#include        <errno.h>
#include	"buhg.h"

extern double	nds1;
extern double   okrg1;

void copdok(short dnk,short mnk,short gnk,const char *nomdok,
int skl,short tz) //1-приход 2-расход
{
static  class iceb_tu_str dat1(""),dat2("");
short           dn,mn,gn,dk,mk,gk;
short		d,m,g;
static class iceb_tu_str kopz("");
static class iceb_tu_str kodoz("");
static class iceb_tu_str prra("");
static class iceb_tu_str nomdokp("");
static class iceb_tu_str sklad("");
class iceb_tu_str nomdok1("");
short           kom;
int             tipzz,tp;
double          kolih,cena,cenaz,cenap;   /*Цена*/
short		prkk; /*1-привязка к карточкам 2-без привязки*/
SQL_str         row,row1,row2;
int		kolstr,kolstr1,kolstr2;
char		strsql[1024];
int		mnds;
double		fas,krt,nds;
int		skl1,kodm,nk,kodt;
class iceb_tu_str ei("");
class iceb_tu_str shbm("");
short		mud; /*1-удалить документы после копирования 2-нет*/
int		kolsz; /*Количество скопированных записей*/
int		koldok; /*Количество скопированных документов*/
double		sumkor;
short		denv=0,mesv=0,godv=0; //Дата ввода в эксплуатацию для малоценки
char		innom[16]; //Инвентарный номер для малоценки
class iceb_tu_str rnd(""); //Регистрационный номер мед. препарата
class iceb_tu_str nomz(""); //Номер заказа

innom[0]='\0';

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Копировать записи из других документов"));
VV.VVOD_SPISOK_add_ZAG(gettext("По не введенным реквизитам поиск не выполняется"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............"));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Прих./расход (+/-)....."));
VV.VVOD_SPISOK_add_MD(gettext("Номер документа..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Склад............(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(prra.ravno(),2);
VV.VVOD_SPISOK_add_data(nomdokp.ravno(),128);
VV.VVOD_SPISOK_add_data(sklad.ravno(),128);


naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("копировать"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
kopz.new_plus(VV.VVOD_SPISOK_return_data(2));
kodoz.new_plus(VV.VVOD_SPISOK_return_data(3));
prra.new_plus(VV.VVOD_SPISOK_return_data(4));
nomdokp.new_plus(VV.VVOD_SPISOK_return_data(5));
sklad.new_plus(VV.VVOD_SPISOK_return_data(6));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu2_1_2d.txt");
   clear();
   goto naz;
   
  case FK2:
  case PLU:
    break;

  case FK10:
  case ESC:
   return;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

tipzz=0;
if(prra.ravno()[0] == '+')
  tipzz=1;
if(prra.ravno()[0] == '-')
  tipzz=2;
  

prkk=0;
if(tz == 1)
 {
  move(LINES-5,0);
  printw(gettext("Если не делать привязки к карточкам, то одинаковые материалы\n\
с разных документов и разных карточек будут записаны одной записью с общим\n\
количеством и наибольшей ценой. При выполнении привязки, привязка выполняется\n\
к карточкам с такою же ценой, или заводятся новые карточки.\n"));
  VVOD DANET(1);

  DANET.VVOD_SPISOK_add_MD(gettext("Сделать привязку к карточкам ?"));

  prkk=danet(&DANET,2,stdscr);
 }

if(tz == 2)
 {
  move(20,0);
  printw(gettext("Если не сохранять привязку к карточкам, то однаковые материалы\n\
с разных документов и разных карточек будуть записаны одной записью с\n\
общим количеством и наибольшей ценой.\n"));

  VVOD DANET(1);

  DANET.VVOD_SPISOK_add_MD(gettext("Сохранить привязку к карточкам ?"));
  prkk=danet(&DANET,2,stdscr);
 }

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Удалить документы после копирования ?"));

mud=danet(&DANET,2,stdscr);

if(mud == 1)
 {
  class VVOD PAR(2);
  PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

  if(parolv(&PAR,0,0,0,0) != 0)
    goto naz;
  
   
 }
  
clear();
GDITE();

move(0,0);
printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

printw("%d.%d.%d %s %d\n",dnk,mnk,gnk,nomdok,skl);
refresh();

sprintf(strsql,"select * from Dokummat \
where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d' ",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного документа !"));
  return;
 }

short deng,mesg,godg;

koldok=kolsz=kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
/*
  printw("%s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5]);
  refresh();
*/  
  tp=atoi(row[0]);
  if(tipzz != 0 && tipzz != tp)
      continue;

  if(proverka(kodoz.ravno(),row[3],0,0) != 0)
     continue;

  if(proverka(kopz.ravno(),row[6],0,0) != 0)
     continue;

  if(proverka(sklad.ravno(),row[2],0,0) != 0)
     continue;

  if(proverka(nomdokp.ravno(),row[4],0,0) != 0)
     continue;

  nomdok1.new_plus(row[4]);

  rsdat(&d,&m,&g,row[1],2);  

  skl1=atoi(row[2]);
  /*Не копировать себя самого*/
  if(skl1 == skl && SRAV(nomdok1.ravno(),nomdok,0) == 0 && gnk == g)
    continue;

  memset(strsql,'\0',sizeof(strsql));
  if(tp == 1)
   strcpy(strsql,"+");
  if(tp == 1)
   strcpy(strsql,"-");

  printw("%s %02d.%02d.%d %-3s %-4s %-5s %s\n",
  strsql,d,m,g,row[6],row[3],row[4],row[2]);
  refresh();
  
  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%s and nomerz=13",
  g,row[4],row[2]);

  sumkor=0.;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    sumkor=atof(row1[0]);
  if(sumkor != 0)
   {
    sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
    gnk,nomdok,skl);

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      sumkor+=atof(row1[0]);
      sprintf(strsql,"delete from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
      gnk,nomdok,skl);
 
      sql_zapis(strsql,1,0);
     }
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%.2f')",gnk,skl,nomdok,13,sumkor);

    sql_zapis(strsql,1,0);

    
   }  


  sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
  g,m,d,skl1,nomdok1.ravno());
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
    continue;
         
  koldok++;
  while(cur1.read_cursor(&row1) != 0)
   {
    kodm=atoi(row1[4]);
    nk=atoi(row1[3]);
    cena=atof(row1[6]);
    kolih=atof(row1[5]);
    nds=atof(row1[9]);
    mnds=atoi(row1[10]);
    ei.new_plus(row1[7]);
    nomz.new_plus(row1[17]);
            
    /*Проверяем нет ли уже такой записи*/
    if(tz == 2)
     if(prkk == 2 || skl1 != skl)
       nk=0;
    if(tz == 1 && prkk == 2)
       nk=0;

    SQLCURSOR cur2;
    deng=mesg=godg=0;
    if(tz == 1 && skl != skl1 && nk != 0 && prkk == 1) /*Приход*/
     {

/*Читаем реквизиты карточки на чужом складе и если нет на текущем
складе карточки с такимиже реквизитами то заводим ее*/

      sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
      skl1,nk);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        beep();
        printw("Не знайшли картки %d, склад %d, матеріал %d\n",
        nk,skl1,kodm);
        OSTANOV();
        continue;
       }

      mnds=atoi(row2[3]);
      ei.new_plus(row2[4]);
      shbm.new_plus(row2[5]);
      cenaz=atof(row2[6]);
      cenap=atof(row2[7]);
      krt=atof(row2[8]);
      nds=atof(row2[9]);
      fas=atof(row2[10]);
      kodt=atoi(row2[11]);
      strcpy(innom,row2[15]);
      rnd.new_plus(row2[16]);
      rsdat(&deng,&mesg,&godg,row2[18],2);
//      rsdat(&denv,&mesv,&godv,row[14],2); не будем переписывать
            
      sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.10g \
and cenap=%.10g and krat=%.10g and mnds=%d and innom='%s' and dat_god='%04d-%02d-%02d",
      kodm,skl,shbm.ravno(),cenaz,ei.ravno(),nds,cenap,krt,mnds,innom,godg,mesg,deng);

      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        nk=nomkr(skl);

        printw("%s N%d, %s %d, %s %d\n",
        gettext("Записываем новую карточку"),
        nk,
        gettext("склад"),
        skl1,
        gettext("материал"),
        kodm);
        refresh();
        zapkar(skl,nk,kodm,cenaz,ei.ravno(),shbm.ravno(),krt,fas,kodt,nds,mnds,cenap,\
        denv,mesv,godv,innom,rnd.ravno(),nomz.ravno(),deng,mesg,godg,0);

       }
      else
       nk=atoi(row2[0]);
     }

    kolsz++;
   
    sprintf(strsql,"select kolih,cena from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d",
    gnk,mnk,dnk,skl,nomdok,kodm,nk);
      
    if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
     {
      kolih+=atof(row2[0]);
      cenaz=atof(row2[1]);
/*
      printw("Перезаписываем %d.%d.%d %s %d\n",dnk,mnk,gnk,nomn,skl);
      refresh();  
*/
      if(fabs(cenaz-cena) > 0.009)
       {
        beep();

        printw("%s %s %s %d.%d.%d, %s %d. %s %.10g %.10g\n\%s.\n",
        gettext("Документ"),
        nomdok1.ravno(),
        gettext("від"),
        d,m,g,
        gettext("материал"),
        kodm,
        gettext("Не совпадает цена !"),
        cena,cenaz,
        gettext("Берем большую цену"));

        if(cena-cenaz < 0.)
         cena=cenaz;
        OSTANOV();
       }
      
      sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g,\
kolih=%.10g,\
ktoi=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
      cena,kolih,iceb_t_getuid(),gnk,mnk,dnk,skl,nomdok,kodm,nk);


      sql_zapis(strsql,1,0);

     }
    else
     {
/*
      printw("Пишем %d.%d.%d %s %d %d\n",dnk,mnk,gnk,nomdok,skl,nk);
      refresh();  
*/
      zapvdok(dnk,mnk,gnk,skl,nk,kodm,nomdok,kolih,cena,ei.ravno(),nds,mnds,0,tz,0,"",nomz.ravno(),0,"");

     }


   }

  /*Удаление после копирования*/
  if(mud == 1)
   {
//    sprintf(strsql,"%s",iceb_t_imafnsi("blokmak.alx"));
    if(iceb_t_pblok(m,g,ICEB_PS_MU) != 0)
     {
      beep();
      printw(gettext("Документ %s удалить невозможно. Дата %d.%dг. заблокирована !"),
      nomdok1.ravno(),m,g);
      printw("\n");
      continue;
     }

    if(udprgr(ICEB_MP_MATU,d,m,g,nomdok1.ravno(),skl1,tp) != 0)
     continue;
    matudd(0,0,0,tp,d,m,g,nomdok1.ravno(),skl1,0,0);

   } 


 }

VVOD SOOB(1);
sprintf(strsql,gettext("Количество переписаних документов %d !"),koldok);
SOOB.VVOD_SPISOK_add_MD(strsql);
sprintf(strsql,gettext("Количество переписаних записей %d !"),kolsz);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);

}
