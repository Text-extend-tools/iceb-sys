/* $Id: readdok.c,v 5.17 2013/09/26 09:43:39 sasa Exp $ */
/*03.06.2013    16.04.1997      Белых А.И.      readdok.c
Поиск реквизитов документа в материальном учете по номеру
Если вернули 0-нашли
	     1-нет
*/
#include	"buhg.h"

int readdok(short *dd,short *md,short *gd,//Дата документа
const char *nomdok, //Номер документа
int skk,       //Код склада
int *tipz,     //1-приход 2-расход
class iceb_tu_str *kpos,   //Контрагент
class iceb_tu_str *naimo,  //Наименование контрагента
class iceb_tu_str *kprr,   //Код операции
class iceb_tu_str *naimpr, //Наименование операции
int *metkaprov, //0-выполнять проводки для операции 1-нет
class iceb_tu_str *naiskl, //Наименование склада
int *skl1,     //Код встречного склада
int *lnds,     //Метка льгот по НДС
short *mvnp,  //0-внешняя 1 -внутреняя 2-изменение стоимости
short *mdd,   //0-обычный документ 1-двойной
int *pro,     //0-не выполнены проводки 1-выполнены
int *ktoi,    //Кто записал
class iceb_tu_str *nomnn, //Номер налоговой накладной
class iceb_tu_str *nomon, //Номер ответной накладной
class iceb_tu_str *sheta, //Счета по операции
int *blokpid, //Кто заблокировал документ
int *loginrash, //Кто распечатал чек
short *dpnn,short *mpnn,short *gpnn, //дата получения налоговой накладной
double *sumandspr, //Сумма НДС для приходного документа введенная вручную
float *pnds,
time_t *vrem)
{
char		strsql[1024];
SQL_str         row;
class iceb_tu_str bros("");
int		i;
short		d,m,g;
SQLCURSOR       cur;
/*Читаем шапку документа*/
/*
move(10,0);
printw("Читаем шапку документа.\n");
refresh();
*/

*sumandspr=*metkaprov=*loginrash=0;

if(*dd != 0)
 sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
 *gd,*md,*dd,skk,nomdok);
else /*передали только год*/
 sprintf(strsql,"select * from Dokummat \
where datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%d and \
nomd='%s'",
 *gd,*gd,skk,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) < 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найден документ !"));

  if(*dd != 0)
   sprintf(strsql,"%s-%s:%d.%d.%d %s:%s %s:%d",
   __FUNCTION__,
   gettext("Дата"),
   *dd,*md,*gd,
   gettext("Документ"),
   nomdok,
   gettext("Склад"),
   skk);
  else
   sprintf(strsql,"%s-%s:%s %s:%d",
   __FUNCTION__,
   gettext("Документ"),
   nomdok,
   gettext("Склад"),
   skk);

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  
  return(1);
 }

if(*dd == 0)
 {
  rsdat(&d,&m,&g,row[1],2);
  *dd=d;
  *md=m;
  *gd=g;
 } 
kpos->new_plus(row[3]);
kprr->new_plus(row[6]);
*tipz=atoi(row[0]);
*pro=atoi(row[8]);
*ktoi=atoi(row[9]);
nomnn->new_plus(row[5]);
*vrem=atol(row[10]);
nomon->new_plus(row[11]);
rsdat(dpnn,mpnn,gpnn,row[12],2);
*pnds=(float)atof(row[13]);

*mdd=0;
/****************
if(kodopsrvc != NULL)
if(SRAV(kprr->ravno(),kodopsrvc,0) == 0)
 *mdd=1;
****************/
/*Для внутреннего перемещения*/
*skl1=0;
if(kpos->ravno_atof() == 0. && polen(kpos->ravno(),&bros,2,'-') == 0 && kpos->ravno()[0] == '0')
 {
  *mdd=1;
  polen(kpos->ravno(),skl1,2,'-');
//  *skl1=(int)ATOFM(bros);
 }


/*
printw("mdd=%d kpos=%s\n",*mdd,kpos);
OSTANOV();
*/

/*Читаем наименование организации*/
if(kpos->ravno_atof() == 0. && polen(kpos->ravno(),&bros,2,'-') == 0 && kpos->ravno()[0] == '0')
    bros.new_plus("00");
else
    bros.new_plus(kpos->ravno());

sprintf(strsql,"select naikon from Kontragent \
where kodkon='%s'",
bros.ravno());
naimo->new_plus("");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  beep();
  printw("%s-%s %s !\n\n",__FUNCTION__,gettext("Не найден контагент"),bros.ravno());
  OSTANOV();
 }
else
 naimo->new_plus(row[0]);


/*Читаем наименование операции*/
/*
printw("Читаем наименование операции.\n");
refresh();
*/
if(*tipz == 1)
 bros.new_plus("Prihod");
if(*tipz == 2)
 bros.new_plus("Rashod");
 
sprintf(strsql,"select * from %s \
where kod='%s'",
bros.ravno(),kprr->ravno());
naimpr->new_plus("");
sheta->new_plus("");

if(readkey(strsql,&row,&cur) == 1)
 {
  naimpr->new_plus(row[1]);
  if(*tipz == 2)
   {
    sheta->new_plus(row[5]);
    *metkaprov=atoi(row[6]);
   }
  else
    *metkaprov=atoi(row[5]);
  *mvnp=atoi(row[4]);
 }


/*Читаем наименование склада*/
/*
printw("Читаем наименование склада.\n");
refresh();
*/
sprintf(strsql,"select naik from Sklad \
where kod=%d",
skk);
naiskl->new_plus("");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  beep();
  printw("\n%s-%s %d!\n\n",__FUNCTION__,gettext("Не найден склад"),skk);
  OSTANOV();  
 }
else
 naiskl->new_plus(row[0]);

/*Читаем логин распечатавшего чек на кассе*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=5",
*gd,nomdok,skk);

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 *loginrash=atoi(row[0]);

if(*tipz == 1)
 {
  /*Читаем сумму НДС введенную вручную*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
  *gd,nomdok,skk);

  if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
   *sumandspr=atof(row[0]);
 }
/*Читаем метку НДС*/
/*
printw("Читаем метку НДС.\n");
refresh();
*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=11",
*gd,nomdok,skk);
*lnds=0;

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 *lnds=atoi(row[0]);

/*Читаем кто заблокировал*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=12",
*gd,nomdok,skk);
*blokpid=0;

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 *blokpid=atoi(row[0]);


/*
printw("strsql=%s\ni=%d row=%s\nlnds=%d\n",strsql,i,row[0],*lnds);
OSTANOV();
*/
return(0);

}
