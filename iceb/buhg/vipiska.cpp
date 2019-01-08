/*$Id: vipiska.c,v 5.34 2013/09/26 09:43:43 sasa Exp $*/
/*25.06.2013	11.06.2003	Белых А.И.	vipiska.c
Разноска выписки
*/
#include <sys/stat.h>
#include        <errno.h>
#include        "buhg.h"
#define          AVTZAG "vp"

void   vipiska1(int,int*,const char*,FILE*,FILE*,char*);

int  vipkont(class iceb_tu_str *kontr,const char *shetk,const char *kod,class iceb_tu_str *naimkontr,const char *rshet,const char *mfo);
int   vipvvod(int metkapr,short d,short m,short g,const char *nomdok,double suma,const char *kod,const char *mfo,const char *rshet,const char *naimkont,const char *koment,const char *rshet00,const char *shet,class iceb_tu_str *shetk,class iceb_tu_str *kontrshk,FILE *fftmp);
void   vtmp(int metkapr,short d,short m,short g,const char *nomdok,double suma,const char *kod,const char *mfo,const char *rshet,const char *naimkont,const char *koment,const char *rshet00,const char *shetk,const char *kontrshk,FILE *fftmp);



void	vipiska()
{
struct stat work;
char		strsql[512];
static class iceb_tu_str imafim("");
static char	imafprom[64],imaftmp[64];
int		kom=0;
int		razmer=0;
FILE		*ff,*ffprom;
char		strok[1024];

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Розноска банковской виписки"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,COLS-6,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("buhg3_6.txt");
    clear();
    goto naz;

  case ENTER:
  case FK2:
  case PLU:
    if(imafim.getdlinna() <= 1)
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  default:
    goto naz;
 }

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Разнести все"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Разнести только приходы"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Разнести только расходы"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

clearstr(LINES-1,0);

int kom1=0;
while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

if(kom1 == 3 || kom1 == -1)
  return;

clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  iceb_t_soob(gettext("Нет такого файла !"));
  goto naz;
 }


if((ff = fopen(imafim.ravno(),"r")) == NULL)
 {
  error_op_nfil(imafim.ravno(),errno,"");
  goto naz;
 }
sprintf(imaftmp,"promt%d.tmp",getpid());
sprintf(imafprom,"prom%d.tmp",getpid());

if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  error_op_nfil(imafprom,errno,"");
  goto naz;
 }

int i=0;
razmer=0;
class iceb_tu_str bros("");
int kolprov=0;
int   metkapr=0;

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

  printw("%s",strok);
  strzag(LINES-1,0,work.st_size,razmer);
  if(polen(strok,&bros,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  metkapr=0;

  if(bros.ravno()[0] == '+')
    metkapr=1;

  if(bros.ravno()[0] == '-')
    metkapr=2;

  if(kom1 == 1 && metkapr == 2)
    metkapr=0;

  if(kom1 == 2 && metkapr == 1)
    metkapr=0;
  
  if(metkapr == 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  vipiska1(metkapr,&kolprov,strok,ff,ffprom,imaftmp);

 }

fclose(ff);
fclose(ffprom);
unlink(imafim.ravno());
unlink(imaftmp);

rename(imafprom,imafim.ravno());

iceb_t_soob(gettext("Загрузка завершена"));

}
/************************************/

void   vipiska1(int metkapr, //1-приход 2-расход
int *kolprov,
const char *strokan,
FILE *ff,FILE *ffprom,
char *imaftmp)
{
char	strok[2048];
int  val=0;
short d=0,m=0,g=0;
class iceb_tu_str shet("");
class iceb_tu_str shetk("");
class iceb_tu_str kontrsh("");
class iceb_tu_str kontrshk("");
time_t  vrem;
double  deb=0.,kre=0.;
class iceb_tu_str koment("");
char	strsql[1024];
class iceb_tu_str bros("");
class iceb_tu_str nomdok("");
class iceb_tu_str kod("");
class iceb_tu_str mfo("");
class iceb_tu_str rshet("");
class iceb_tu_str naimkont("");
class iceb_tu_str rshet00("");
double	suma=0.;
struct OPSHET	op_sh;
struct OPSHET	op_shk;
FILE	*fftmp=NULL;
int kekv=0;


if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  error_op_nfil(imaftmp,errno,"");
  return;
 }
fprintf(fftmp,"%s",strokan);

int metkaoh=0;
int dlina=0;
for(int i=0; i < 11; i++)
 {
  if(fgets(strok,sizeof(strok),ff) == NULL)
    break;

  dlina=strlen(strok);
  if(strok[dlina-2] == '\r') //Возврат каретки меняем на пробел
   strok[dlina-2]=' ';

  printw("%s",strok);
  refresh();
  fprintf(fftmp,"%s",strok);

  if(polen(strok,&bros,2,'|') != 0)
   {
    metkaoh=1;
    break;
   }

  if(bros.ravno()[0] == '-' || bros.ravno()[0] == '+')
   {
    metkaoh=1;
    break;
   }

  switch (i)
   {

    case 0: //Дата
      if(rsdat(&d,&m,&g,bros.ravno(),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата проводки !"));
        metkaoh=1;
        break;
       }
      if(iceb_t_pvglkni(m,g) != 0)
       {
        metkaoh=1;
        break;
       }

      break;

    case 1: //Номер документа
      nomdok.new_plus(bros.ravno());
      break;

    case 2: //Сумма
      suma=atof(bros.ravno());
      if(metkapr == 1)
        kre=suma;
      if(metkapr == 2)
        deb=suma;
      break;

    case 3: //Код ЕДРПУО
      kod.new_plus(bros.ravno());
      break;

    case 4: //МФО
      mfo.new_plus(bros.ravno());
      break;

    case 5: //Расчётный счет
      rshet.new_plus(bros.ravno());
      break;

    case 6: //Наименование контрагента
      naimkont.new_plus(bros.ravno());
      break;

    case 7: //Коментарий
      koment.new_plus(bros.ravno());
      break;

    case 8: //Расчётный счёт организации
      rshet00.new_plus(bros.ravno());
      //Читаем бухгалтерский счёт для расчётного счета
      if(iceb_t_poldan(rshet00.ravno(),&shet,"avtprodok.alx") != 0)
       {
        sprintf(strsql,"Не найден бухгалтерский счёт для расчётного счета %s !",rshet00.ravno());
        iceb_t_soob(strsql);
        metkaoh=1;
        break;
       }

      if(prsh1(shet.ravno(),&op_sh) != 0)
       {
        metkaoh=1;
        break;
       }

      break;

    case 9: //Счёт корреспондент
      if(polen(bros.ravno(),&shetk,2,',') != 0)
        shetk.new_plus(bros.ravno());
      else
       {
        polen(bros.ravno(),&shet,1,',');

        if(prsh1(shet.ravno(),&op_sh) != 0)
         {
          metkaoh=1;
          break;
         }
        }
      
      if(shetk.ravno()[0] != '\0')
       if(prsh1(shetk.ravno(),&op_shk) != 0)
        {
         metkaoh=1;
         break;
        }
      
      break;

    case 10: //Код контрагента
      kontrshk.new_plus(bros.ravno());
      break;

   }  

  if(metkaoh != 0)
   break;  //Выходим из цикла
 }

fclose(fftmp);

if(metkaoh == 1)
 {
  if((fftmp = fopen(imaftmp,"r")) == NULL)
   {
    error_op_nfil(imaftmp,errno,"");
    return;
   }

  while(fgets(strok,sizeof(strok),fftmp) != NULL)
   fprintf(ffprom,"%s",strok);

  fclose(fftmp);
  return;
 }


if(vipkont(&kontrshk,shetk.ravno(),kod.ravno(),&naimkont,rshet.ravno(),mfo.ravno()) == 0) //Определение кода контрагента
 {
  if(shetk.ravno()[0] != '\0' && op_shk.saldo == 3)
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shetk.ravno(),kontrshk.ravno());
    if(sql_readkey(&bd,strsql) < 1)
     {
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shetk.ravno(),kontrshk.ravno(),iceb_t_getuid(),vrem);
      sql_zapis(strsql,1,1);
     }  
   }
  if(shet.ravno()[0] != '\0' && op_sh.saldo == 3)
   {
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shet.ravno(),kontrshk.ravno());
    if(sql_readkey(&bd,strsql) < 1)
     {
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shet.ravno(),kontrshk.ravno(),iceb_t_getuid(),vrem);
      sql_zapis(strsql,1,1);
     }  
   }
 }
 
if(shetk.ravno()[0] == '\0' || shet.ravno()[0] == '\0' || (kontrshk.ravno()[0] == '\0' && op_shk.saldo == 3))
 {
  if(vipvvod(metkapr,d,m,g,nomdok.ravno(),suma,kod.ravno(),mfo.ravno(),rshet.ravno(),naimkont.ravno(),koment.ravno(),rshet00.ravno(),shet.ravno(),&shetk,&kontrshk,ffprom) != 0)
    vtmp(metkapr,d,m,g,nomdok.ravno(),suma,kod.ravno(),mfo.ravno(),rshet.ravno(),naimkont.ravno(),koment.ravno(),rshet00.ravno(),shetk.ravno(),kontrshk.ravno(),ffprom);
  return;
 }

time(&vrem);

if(zapprov(val,g,m,d,shet.ravno(),shetk.ravno(),kontrsh.ravno(),kontrshk.ravno(),AVTZAG,"","",deb,kre,koment.ravno(),2,0,vrem,0,0,0,0,kekv) == 0)
  *kolprov+=1;



}
/***********************************/

void   vtmp(int metkapr,
short d,short m,short g,
const char *nomdok,
double suma,
const char *kod,
const char *mfo,
const char *rshet,
const char *naimkont,
const char *koment,
const char *rshet00,
const char *shetk,
const char *kontrshk,
FILE *fftmp)
{
char priz[5];
memset(priz,'\0',sizeof(priz));

if(metkapr == 1)
  priz[0]='+';
if(metkapr == 2)
  priz[0]='-';

fprintf(fftmp,"\
Приход/расход (+/-)        |%s\n\
Дата документа             |%d.%d.%d\n\
Номер Документа            |%s\n\
Сумма                      |%.2f\n\
Код ЕГРПУО                 |%s\n\
МФО                        |%s\n\
Счёт                       |%s\n\
Наименование контрагента   |%s\n\
Назначение платежа         |%s\n\
Счёт получателя/плательщика|%s\n\
Счёт корреспондент         |%s\n\
Код контрагента            |%s\n\n",
priz,
d,m,g,
nomdok,
suma,
kod,
mfo,
rshet,
naimkont,
koment,
rshet00,
shetk,
kontrshk);

}

/************************************************/
/*Просмотр записи и ввод счета и код контрагента*/
/*************************************************/
//Если вернули 0- записали 1-нет
int   vipvvod(int metkapr,
short d,short m,short g,
const char *nomdok,
double suma,
const char *kod,
const char *mfo,
const char *rshet,
const char *naimkont,
const char *koment,
const char *rshet00,
const char *shet,
class iceb_tu_str *shetk,
class iceb_tu_str *kontrshk,
FILE *fftmp)
{
int		N=0,K=0;
char            strsql[1024];
struct OPSHET	shetv;
SQL_str         row;
int             voz=0;
class iceb_tu_str kodd("");
short		razm=20;
WINDOW		*win2=NULL;;
SQLCURSOR       cur;
int kekv=0;
memset(&shetv,'\0',sizeof(shetv));

VVOD VV(0);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(kontrshk->ravno(),10);

naz:;

VV.VVOD_delete_MD();


VV.VVOD_SPISOK_add_MD(gettext("Счёт корреспондент.."));
if(shetv.saldo == 3)
 VV.VVOD_SPISOK_add_MD(gettext("Код контрагента...."));

VV.VVOD_delete_ZAG();
if(metkapr == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Приход"));
if(metkapr == 2)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расход"));

sprintf(strsql,"%-*s:%d.%d.%d",
razm,
gettext("Дата"),
d,m,g);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("Номер документа"),
nomdok);

VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%.2f",
razm,
gettext("Сумма"),
suma);

VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("Код ЕДРПУО"),
kod);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("МФО"),
mfo);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("Счёт"),
rshet);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("Контрагент"),
naimkont);
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%-*s:%s",
razm,
gettext("Коментарий"),
koment);
VV.VVOD_SPISOK_add_ZAG(strsql);
if(koment[COLS-2-razm-1] != '\0')
  VV.VVOD_SPISOK_add_ZAG(&koment[COLS-2-razm-1]);

if(shetv.naim[0] != '\0')
 {
  sprintf(strsql,"%-*s:%s",
  razm,
  gettext("Наименование счета"),
  shetv.naim);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


int PYstd=0;
int PXstd=0;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

delwin(win2);
touchwin(stdscr);
wrefresh(stdscr);

VV.wvramka(-1,-1,&PYstd,&PXstd,0,0,0,stdscr,&win2);

for(;;)
 {

  wattron(win2,VV.VVOD_return_cs(iceb_CFS)); 
/******
  int dlinastr=VV.VVOD_SPISOK_return_dls(N)-1;
  if(dlinastr > VV.MDLS-VV.dlr)
    dlinastr=VV.MDLS-VV.dlr;
**********/
//  wmove(win2,PY+N+1,PX+1+VV.dlr);
//  wprintw(win2,"%-*.*s",dlinastr,dlinastr,VV.VVOD_SPISOK_return_data(N));
  VV.vprintw(N,win2);
  
  wrefresh(win2);
  K=wgetch(win2);


  wattroff(win2,VV.VVOD_return_cs(iceb_CFS));
  wattron(win2,VV.VVOD_return_cs(iceb_CFM));

//  wmove(win2,PY+N+1,PX+1+VV.dlr);
//  wprintw(win2,"%-*.*s",dlinastr,dlinastr,VV.VVOD_SPISOK_return_data(N));
  VV.vprintw(N,win2);

  if(K == KEY_RESIZE)
   {
//    wgetch(win2);
    goto naz;
   }


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,win2);

   
  if(K == ENTER)
   {
    wattroff(win2,VV.VVOD_return_cs(iceb_CFM));
    wattron(win2,VV.VVOD_return_cs(iceb_CFV));
//    wmove(win2,PY+N+1,PX+1+VV.dlr);
//    wprintw(win2,"%*s",dlinastr," ");
//    wmove(win2,PY+N+1,PX+1+VV.dlr);
    VV.vprintw_clear(N,win2);
    
//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),win2);
    VV.vgetstr(N,win2);
    
    wattroff(win2,VV.VVOD_return_cs(iceb_CFV));
    wattron(win2,VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      prsh1(VV.VVOD_SPISOK_return_data(N),&shetv);
      goto naz; 
     }
    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
      VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),
        VV.VVOD_SPISOK_return_data(N));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }
     }
    wattroff(win2,VV.VVOD_return_cs(iceb_CFV));
    wattron(win2,VV.VVOD_return_cs(iceb_CFM));

//    wmove(win2,PY+N+1,PX+1+VV.dlr);
//    wprintw(win2,"%-*.*s",dlinastr,dlinastr,VV.VVOD_SPISOK_return_data(N));
    VV.vprintw(N,win2);
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
    wattroff(win2,VV.VVOD_return_cs(iceb_CFS));
    wattron(win2,VV.VVOD_return_cs(iceb_CFM));
    voz=1;
    N=-1;
    break;
   }

  if(K == FK1) //Помощь
   {
    wattroff(win2,VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    

    if(prsh1(VV.VVOD_SPISOK_return_data(0),&shetv) != 0)
        goto naz; 

    if(shetv.saldo == 3)
     {   
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
      VV.VVOD_SPISOK_return_data(1));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),
        VV.VVOD_SPISOK_return_data(1));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto naz;
       }

      sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
/*************************
        VVOD SOOB(1);
        sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),
        VV.VVOD_SPISOK_return_data(1),
        VV.VVOD_SPISOK_return_data(0));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
*************************/
        time_t vrem;
        time(&vrem);
        
        sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",
        VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),
        iceb_t_getuid(),vrem);
        sql_zapis(strsql,1,1);
       }
     }

    shetk->new_plus(VV.VVOD_SPISOK_return_data(0));
    kontrshk->new_plus(VV.VVOD_SPISOK_return_data(1));
    if(shetv.saldo != 3)
      kontrshk[0]='\0';
          
    int val=0;
    double deb=0.,kre=0.;
    if(metkapr == 1)
     kre=suma;
    if(metkapr == 2)
     deb=suma;

    time_t vrem=0;
    time(&vrem);
//    char kontrsh[10];
//    kontrsh[0]='\0';
    zapprov(val,g,m,d,shet,shetk->ravno(),"",kontrshk->ravno(),AVTZAG,"","",deb,kre,koment,2,0,vrem,0,0,0,0,kekv);
    voz=0;
    break;

   }
  if(K == FK3) //Просмотр списков
   {
    wattroff(win2,VV.VVOD_return_cs(iceb_CFS));
    VVOD MENU(3);
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов."));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов."));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход."));//2

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
        break;

      case 0 :
       if(vibrek("Plansh",&kodd) == 0)
         VV.VVOD_SPISOK_zapis_data(0,kodd.ravno());
       break;

      case 1 :
       if(vibrek("Kontragent",&kodd) == 0)
         VV.VVOD_SPISOK_zapis_data(1,kodd.ravno());
       break;
     }
    goto naz;
   }


 }
wattroff(win2,VV.VVOD_return_cs(iceb_CFS));
wattron(win2,VV.VVOD_return_cs(iceb_CB));
wrefresh(win2);

delwin(win2);
clearstr(LINES-1,0);
touchwin(stdscr);
wrefresh(stdscr);
return(voz);
}

/**************************************/
/*Определение кода контрагента        */
/***************************************/
//Если вернули 0-контрагент определен 1-нет

int  vipkont(class iceb_tu_str *kontr,const char *shetk,const char *kod,class iceb_tu_str *naimkontr,const char *rshet,const char *mfo)
{
char     strsql[1024];
SQL_str  row;
SQLCURSOR cur;

if(kontr->ravno()[0] != '\0') //Проверяем есть ли в списке
 {
  sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr->ravno());
  if(sql_readkey(&bd,strsql) == 1)
    return(0);

  //проверяем может с таким наименованием уже есть контрагент
  sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",naimkontr->ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naimkontr->plus(".");
  
 }
else
 {
  sprintf(strsql,"select kodkon from Kontragent where kod='%s'",kod);
  int kolzap=0;
  if((kolzap=sql_readkey(&bd,strsql,&row,&cur)) == 1)
   {
    kontr->new_plus(row[0]);
    return(0);
   } 

  sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",naimkontr->ravno());
  if((kolzap=sql_readkey(&bd,strsql,&row,&cur)) == 1)
   {
    kontr->new_plus(row[0]);
    return(0);
   } 

  sprintf(strsql,"select kodkon from Kontragent where nomsh='%s'",rshet);
  if((kolzap=sql_readkey(&bd,strsql,&row,&cur)) == 1)
   {
    kontr->new_plus(row[0]);
    return(0);
   } 
 }

time_t vrem;
time(&vrem);

//Записываем
sprintf(strsql,"LOCK TABLES Kontragent WRITE,icebuser READ");
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql);

//sprintf(kontr,"%d",norn());
kontr->new_plus(iceb_t_getnkontr(1));

sprintf(strsql,"insert into Kontragent (kodkon,naikon,kod,mfo,nomsh,ktoz,vrem) \
values ('%s','%s','%s','%s','%s',%d,%ld)",
kontr->ravno(),
naimkontr->ravno(),
kod, //код ЕДРПУО
mfo,
rshet,
iceb_t_getuid(),
vrem);

if(sql_zapis(strsql,1,1) != 0)
 kontr[0]='\0';

sprintf(strsql,"UNLOCK TABLES");
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql);



if(kontr->ravno()[0] != '\0')
  return(0);
else
  return(1);
  
}
