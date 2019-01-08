/* $Id: vnu.c,v 5.32 2011-01-13 13:43:24 sasa1 Exp $ */
/*31.12.2017	21.09.2000	Белых А.И.	vnu.c
Программа ввода и корректировки начисления/удержания 
Если вернули 0 сделали запись 
             1 нет
*/
#include        <time.h>
#include        <pwd.h>
#include        "buhg.h"

extern struct   passwd  ktor; /*Кто работает*/
extern double   okrg; /*Округление*/
extern short	ddd;

int          vnu(VVOD *VV,long tabn, //Табельный номер
short knah, //Код начисления удержания
const char *n1,  //Наименование кода
short na,   //1-начисление 2-удержание
short mr, //0-с запросом -1 без запроса
short mp,short gp, //дата расчета
short mv, //0-ввод нового 1-корректировка*/
short dn,short mn,short gn, //Дата приема на работу
short du,short mu,short gu, //Дата увольнения
int ktov,  //Кто записал
time_t vrem, //Время записи
const char *nomdok) //номер документа
{
short           i;
struct OPSHET	shetv;
short           nz;
short		d,m,g;
double		suma;
short		kdn;
SQL_str         row;
char		strsql[512];
int		podr;
static char	den[10];
static char	datamg[32];
struct ZARP     zp;
SQLCURSOR curr;
memset(&zp,'\0',sizeof(zp));
if(mv == 1)
 {
  zp.dz=atoi(VV->VVOD_SPISOK_return_data(1));
  rsdat1(&zp.mesn,&zp.godn,VV->VVOD_SPISOK_return_data(2));
  zp.nomz=atoi(VV->VVOD_SPISOK_return_data(7));
  zp.podr=atoi(VV->VVOD_SPISOK_return_data(8));
  strcpy(zp.shet,VV->VVOD_SPISOK_return_data(5));
 }
 
if(den[0] == '\0')
 {
 if(VV->VVOD_SPISOK_return_data(1)[0] != '\0')
   strcpy(den,VV->VVOD_SPISOK_return_data(1));
 else
   sprintf(den,"%d",ddd);
 }
if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
  VV->data_plus(1,den);  
//  VV->data_plus(1),den);  

if(datamg[0] == '\0')
 {
 if(VV->VVOD_SPISOK_return_data(2)[0] != '\0')
   strcpy(datamg,VV->VVOD_SPISOK_return_data(2));
 else
   sprintf(datamg,"%d.%d",mp,gp);
 }
if(VV->VVOD_SPISOK_return_data(2)[0] == '\0')
  VV->data_plus(2,datamg);  
//  VV->data_plus(2),datamg);  

podr=atoi(VV->VVOD_SPISOK_return_data(8));


if(mr == 0)
 {
  sprintf(strsql,"%d %s",knah,n1);
  VV->VVOD_SPISOK_add_ZAG(strsql);

  if(nomdok[0] != '\0')
   {
    sprintf(strsql,"%s:%s",gettext("Номер документа"),nomdok);
    VV->VVOD_SPISOK_add_ZAG(strsql);
   }
  kzvz(ktov,vrem,VV);

  VV->VVOD_SPISOK_add_MD(gettext("Сумма......................"));
  VV->VVOD_SPISOK_add_MD(gettext("День записи.......(д)......"));
  VV->VVOD_SPISOK_add_MD(gettext("В счет какого месяца (м.г)."));
  VV->VVOD_SPISOK_add_MD(gettext("До какой даты ....(д.м.г).."));
  VV->VVOD_SPISOK_add_MD(gettext("Количество дней............"));
  VV->VVOD_SPISOK_add_MD(gettext("Счёт......................."));
  VV->VVOD_SPISOK_add_MD(gettext("Коментарий................."));
  VV->VVOD_SPISOK_add_MD(gettext("Номер записи..............."));
  VV->VVOD_SPISOK_add_MD(gettext("Код подразделения.........."));

naz:;
    

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F10",gettext("выход"),NULL);


  i=VV->vvod(0,1,1,-1,-1);

  GDITE();

  switch(i)
   {
    case FK1:   /*Помощь*/
     GDITE();
     iceb_t_pdoc("zarp2_2.txt");
     clear();
     goto naz;

    case FK2:
    case PLU:
      if(provblokzarp(tabn,mp,gp) != 0)
       goto naz;
       
      strncpy(den,VV->VVOD_SPISOK_return_data(1),sizeof(den)-1);
      strncpy(datamg,VV->VVOD_SPISOK_return_data(2),sizeof(datamg)-1);
      break;

    case FK10:
    case ESC:
      return(1);

    default:
     goto naz;
   }

  if(nomdok[0] != '\0')
   {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Записи сделанные из документов, корректируются только в документа!"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
   }   

  if(VV->VVOD_SPISOK_return_data(8)[0] == '\0')
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён код подразделения !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
  /*Проверяем есть ли код подразделения в списке подразделений*/
  podr=(int)ATOFM(VV->VVOD_SPISOK_return_data(8));
  sprintf(strsql,"select kod from Podr where kod=%d",podr);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   { 
    VVOD SOOB(1);
    sprintf(strsql,"%s %s !",
    gettext("Не найден код подразделения"),VV->VVOD_SPISOK_return_data(8));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
   
  if(VV->VVOD_SPISOK_return_data(5)[0] == '\0')
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счет !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }

  if(rsdat1(&m,&g,VV->VVOD_SPISOK_return_data(2)) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }

  if(SRAV1(g,m,31,gn,mn,dn) > 0)
   {
    //Проверяем может он догда работал
    sprintf(strsql,"select datn from Zarn where god=%d and mes=%d and tabn=%ld",
    g,m,tabn);
    if(sql_readkey(&bd,strsql,&row,&curr) < 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,"%s %d.%d.%d !",gettext("Принят на работу"),dn,mn,gn);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }
   } 
  
  if(na == 1)
  if(SRAV1(g,m,1,gu,mu,du) < 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %d.%d.%d !",gettext("Уволен с работы"),du,mu,gu);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
     
   } 

  if(prsh1(VV->VVOD_SPISOK_return_data(5),&shetv) != 0)
    goto naz;

  sprintf(strsql,"%s.%d.%d",VV->VVOD_SPISOK_return_data(1),mp,gp);
  if(rsdat(&d,&m,&g,strsql,0) != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введён день начисления/удержания !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
    

  rsdat1(&m,&g,VV->VVOD_SPISOK_return_data(2));
  if(mv == 0 || //Ввод новой записи
  (mv == 1 && //Корректировка записис
   (zp.dz != d || zp.mesn != m || zp.godn != g || zp.nomz != atoi(VV->VVOD_SPISOK_return_data(7)) ||
    zp.podr != podr || SRAV(zp.shet,VV->VVOD_SPISOK_return_data(5),0) != 0)))
   {
    sprintf(strsql,"select datz from Zarp where datz='%04d-%02d-%02d' and \
tabn=%ld and prn='%d' and knah=%d and godn=%d and mesn=%d and podr=%d \
and shet='%s' and nomz=%d and suma != 0.",
    gp,mp,atoi(VV->VVOD_SPISOK_return_data(1)),tabn,na,knah,g,m,podr,
    VV->VVOD_SPISOK_return_data(5),atoi(VV->VVOD_SPISOK_return_data(7)));
//    printw("\n%s\n",strsql);
    if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Такая запись уже есть !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }
   }

 }

suma=ATOFM(VV->VVOD_SPISOK_return_data(0));
suma=okrug(suma,okrg);
if(na == 2)
  suma*=(-1);


d=(int)ATOFM(VV->VVOD_SPISOK_return_data(1));
rsdat1(&m,&g,VV->VVOD_SPISOK_return_data(2));
nz=(int)ATOFM(VV->VVOD_SPISOK_return_data(7));

kdn=(int)ATOFM(VV->VVOD_SPISOK_return_data(4));

//sqlfiltr(VV->VVOD_SPISOK_return_data(6),sizeof(VV->VVOD_SPISOK_return_data(6)));
short dd,md,gd;
rsdat(&dd,&md,&gd,VV->VVOD_SPISOK_return_data(3),1);

if(zapzarpv(d,mp,gp,tabn,na,knah,suma,VV->VVOD_SPISOK_return_data(5),m,g,kdn,nz,VV->data_ravno_filtr(6),podr,dd,md,gd,nomdok,zp) != 0)
  goto naz;

return(0);

}

/******************************************/

void	sozvnu(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(3);
VV->VVOD_SPISOK_add_data(8);
VV->VVOD_SPISOK_add_data(11);
VV->VVOD_SPISOK_add_data(3);
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(30);
VV->VVOD_SPISOK_add_data(3);
VV->VVOD_SPISOK_add_data(10);
}
