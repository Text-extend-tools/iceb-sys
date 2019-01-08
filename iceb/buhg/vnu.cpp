/* $Id: vnu.c,v 5.37 2013/09/26 09:43:43 sasa Exp $ */
/*21.02.2018	21.09.2000	Белых А.И.	vnu.c
Программа ввода и корректировки начисления/удержания 
Если вернули 0 сделали запись 
             1 нет
*/
#include        "buhg.h"

extern struct   passwd  ktor; /*Кто работает*/
extern double   okrg; /*Округление*/
extern short	ddd;

int vnu(int tabn, //Табельный номер
short knah, //Код начисления удержания
short prn,   //1-начисление 2-удержание
short dz,short mz,short gz, /*Дата записи. Для новой записи день равен нолю а месяц и год дате расчёта*/
short mp,short gp, //дата в счёт которой была сделана запись
int podri,
const char *shet,
int nomz)
{
class VVOD VV(0);
short           i;
struct OPSHET	shetv;
short           nz;
short		d,m,g;
double		suma;
short		kdn;
SQL_str         row;
char		strsql[1024];
int		podr;
static class iceb_tu_str den("");
static class iceb_tu_str datamg("");
class ZARP     zp;
class SQLCURSOR cur;
class iceb_tu_str naim_nu("");
class iceb_tu_str nomdok("");
class iceb_tu_str shet_nu("");
time_t vremz=0;
int ktov=0;
short dn=0,mn=0,gn=0; //Дата приема на работу
short du=0,mu=0,gu=0; //Дата увольнения
short dk=0,mk=0,gk=0; /*До какой даты действует*/
class iceb_tu_str fio("");

if(prn == 1)  
 sprintf(strsql,"select naik,shet from Nash where kod=%d",knah);
else
 sprintf(strsql,"select naik,shet from Uder where kod=%d",knah);

if(readkey(strsql,&row,&cur) == 1)
 {
  naim_nu.new_plus(row[0]);
  shet_nu.new_plus(row[1]);
 }
/*Читаем карточку*/
sprintf(strsql,"select datn,datk,shet,fio from Kartb where tabn=%d",tabn);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
  iceb_t_soob(strsql);
 }
else
 {
  rsdat(&dn,&mn,&gn,row[0],2);
  rsdat(&du,&mu,&gu,row[1],2);
  if(prn == 1 && shet_nu.getdlinna() <= 1)
   shet_nu.new_plus(row[2]);
  fio.new_plus(row[3]);
 }

sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn='%d' and knah=%d",tabn,prn,knah);
if(readkey(strsql,&row,&cur) != 1)
 {
  dk=mk=gk=0;
 }
else
 rsdat(&dk,&mk,&gk,row[0],2);


VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(3);//1
VV.VVOD_SPISOK_add_data(8);//2
VV.VVOD_SPISOK_add_data(11);//3
VV.VVOD_SPISOK_add_data(3);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(40);//6
VV.VVOD_SPISOK_add_data(3);//7
VV.VVOD_SPISOK_add_data(10);//8


if(dz != 0) /* корректировка уже введённой записи*/
 {
     
  sprintf(strsql,"select suma,kdn,ktoz,vrem,kom,nd from Zarp where datz='%04d-%02d-%02d' and tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d \
and podr=%d and shet='%s' and nomz=%d",
  gz,mz,dz,tabn,prn,knah,gp,mp,podri,shet,nomz);

  if(readkey(strsql,&row,&cur) != 1)
   {
    beep();
    printw("\n%s\n",strsql);

    iceb_t_soob(gettext("Не найдена запись"));
    return(1);
   }    
  suma=atof(row[0]);
  if(prn == 2)
   suma*=-1.;
  sprintf(strsql,"%.2f",suma);
//  VV.VVOD_SPISOK_zapis_data(0,strsql);
  VV.data_plus(0,suma);
  sprintf(strsql,"%d",dz);
  VV.VVOD_SPISOK_zapis_data(1,strsql);


  sprintf(strsql,"%02d.%04d",mp,gp);
  VV.VVOD_SPISOK_zapis_data(2,strsql);

  if(gk != 0)
   {
    sprintf(strsql,"%02d.%02d.%04d",dk,mk,gk);
    VV.VVOD_SPISOK_zapis_data(3,strsql);
   }

  VV.VVOD_SPISOK_zapis_data(4,row[1]);

  
  VV.VVOD_SPISOK_zapis_data(5,shet);

  VV.VVOD_SPISOK_zapis_data(6,row[4]);

  sprintf(strsql,"%d",nomz);
  VV.VVOD_SPISOK_zapis_data(7,strsql);
    

  sprintf(strsql,"%d",podri);
  VV.VVOD_SPISOK_zapis_data(8,strsql);
  

  ktov=atoi(row[2]);
  vremz=atol(row[3]);
  nomdok.new_plus(row[5]);

  zp.dz=atoi(VV.VVOD_SPISOK_return_data(1));
  rsdat1(&zp.mesn,&zp.godn,VV.VVOD_SPISOK_return_data(2));
  zp.nomz=atoi(VV.VVOD_SPISOK_return_data(7));
  zp.podr=atoi(VV.VVOD_SPISOK_return_data(8));
  strcpy(zp.shet,VV.VVOD_SPISOK_return_data(5));

 }
else /*новая запись*/
 {
  short denn=31;
  dpm(&denn,&mz,&gz,5); /*получаем последний день месяца*/
  if(den.getdlinna() <= 1)
   {
    short dt=0,mt=0,gt=0;
    poltekdat(&dt,&mt,&gt);
    if(mz == mt && gz == gt) /*если расчётный месяц равен текущему то вводим текущий день*/
      sprintf(strsql,"%d",dt);
    else  
     sprintf(strsql,"%d",denn);
    den.new_plus(strsql);
   }

  VV.VVOD_SPISOK_zapis_data(1,den.ravno());
  if(datamg.getdlinna() <= 1)
   {
    sprintf(strsql,"%02d.%04d",mz,gz);
    datamg.new_plus(strsql);
   }

  VV.VVOD_SPISOK_zapis_data(2,datamg.ravno());
  VV.VVOD_SPISOK_zapis_data(5,shet_nu.ravno());

  sprintf(strsql,"%d",podri);
  VV.VVOD_SPISOK_zapis_data(8,strsql);


//  zp.dz=denn;
  zp.dz=0; /*для новой записи день записи должен быть равен нолю*/
  zp.mesn=mz;
  zp.godn=gz;
  zp.nomz=0;
  zp.podr=podri;
  strcpy(zp.shet,shet);


 }

 
if(den.ravno()[0] == '\0')
 {
  if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
   den.new_plus(VV.VVOD_SPISOK_return_data(1));
  else
   den.new_plus(ddd);
 }
if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
  VV.data_plus(1,den.ravno());  

if(datamg.ravno()[0] == '\0')
 {
  if(VV.VVOD_SPISOK_return_data(2)[0] != '\0')
   datamg.new_plus(VV.VVOD_SPISOK_return_data(2));
  else
   {
    datamg.new_plus(mp);
    datamg.plus(".");
    datamg.plus(gp);
   }
 }

if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
  VV.data_plus(2,datamg.ravno());  

podr=atoi(VV.VVOD_SPISOK_return_data(8));


sprintf(strsql,"%d %s",knah,naim_nu.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

sprintf(strsql,"%d %s",tabn,fio.ravno());
VV.VVOD_SPISOK_add_ZAG(strsql);

if(nomdok.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Номер документа"),nomdok.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

kzvz(ktov,vremz,&VV);

VV.VVOD_SPISOK_add_MD(gettext("Сумма......................"));//0
VV.VVOD_SPISOK_add_MD(gettext("День записи.......(д)......"));//1
VV.VVOD_SPISOK_add_MD(gettext("В счёт какого месяца (м.г)."));//2
VV.VVOD_SPISOK_add_MD(gettext("До какой даты ....(д.м.г).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Количество дней............"));//4
VV.VVOD_SPISOK_add_MD(gettext("Счёт......................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Коментарий................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Номер записи..............."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.........."));//8

naz:;
    

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


i=VV.vvod(0,1,1,-1,-1);

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
      if(provblokzarp(tabn,mz,gz) != 0)
       goto naz;
       
      den.new_plus(VV.VVOD_SPISOK_return_data(1));
      datamg.new_plus(VV.VVOD_SPISOK_return_data(2));
      break;

    case FK10:
    case ESC:
      return(1);

    default:
     goto naz;
 }

if(nomdok.getdlinna() > 1)
 {
  iceb_t_soob(gettext("Записи сделанные из документов, корректируются только в документа!"));
  goto naz;
 }   

if(VV.VVOD_SPISOK_return_data(8)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код подразделения !"));
  goto naz;
 }
/*Проверяем есть ли код подразделения в списке подразделений*/
podr=(int)ATOFM(VV.VVOD_SPISOK_return_data(8));
sprintf(strsql,"select kod from Podr where kod=%d",podr);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 { 
  sprintf(strsql,"%s %s !",
  gettext("Не найден код подразделения"),VV.VVOD_SPISOK_return_data(8));
  iceb_t_soob(strsql);
  goto naz;
 }
   
if(VV.VVOD_SPISOK_return_data(5)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт !"));
  goto naz;
 }

if(rsdat1(&m,&g,VV.VVOD_SPISOK_return_data(2)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(SRAV1(g,m,31,gn,mn,dn) > 0)
 {
  //Проверяем может он тогда работал
  sprintf(strsql,"select datn from Zarn where god=%d and mes=%d and tabn=%d",
  g,m,tabn);
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    sprintf(strsql,"%s %d.%d.%d !",gettext("Принят на работу"),dn,mn,gn);
    iceb_t_soob(strsql);
    goto naz;
   }
 } 

if(prn == 1)
if(SRAV1(g,m,1,gu,mu,du) < 0)
 {
  sprintf(strsql,"%s %d.%d.%d !",gettext("Уволен с работы"),du,mu,gu);
  iceb_t_soob(strsql);
   
 } 

if(prsh1(VV.VVOD_SPISOK_return_data(5),&shetv) != 0)
  goto naz;

sprintf(strsql,"%s.%d.%d",VV.VVOD_SPISOK_return_data(1),mp,gp);
if(rsdat(&d,&m,&g,strsql,0) != 0)
 {
  iceb_t_soob(gettext("Не верно введён день начисления/удержания !"));
  goto naz;
 }
  

rsdat1(&m,&g,VV.VVOD_SPISOK_return_data(2));

if(dz == 0 || //Ввод новой записи
(dz != 0 && //Корректировка записис
 (zp.dz != d || zp.mesn != m || zp.godn != g || zp.nomz != atoi(VV.VVOD_SPISOK_return_data(7)) ||
  zp.podr != podr || SRAV(zp.shet,VV.VVOD_SPISOK_return_data(5),0) != 0)))
 {
  sprintf(strsql,"select datz from Zarp where datz='%04d-%02d-%02d' and \
tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and podr=%d \
and shet='%s' and nomz=%d and suma != 0.",
  gp,mp,atoi(VV.VVOD_SPISOK_return_data(1)),tabn,prn,knah,g,m,podr,
  VV.VVOD_SPISOK_return_data(5),atoi(VV.VVOD_SPISOK_return_data(7)));
  if(readkey(strsql,&row,&cur) >= 1)
   {
    printw("\n%s-%s\n",__FUNCTION__,strsql);
    char ttt[512];
    sprintf(ttt,"%s-%s",__FUNCTION__,gettext("Такая запись уже есть !"));
    iceb_t_soob(gettext("Такая запись уже есть !"));
    goto naz;
   }
 }


suma=ATOFM(VV.VVOD_SPISOK_return_data(0));
suma=okrug(suma,okrg);
if(prn == 2)
  suma*=(-1);


d=(int)ATOFM(VV.VVOD_SPISOK_return_data(1));
rsdat1(&m,&g,VV.VVOD_SPISOK_return_data(2));
nz=(int)ATOFM(VV.VVOD_SPISOK_return_data(7));

kdn=(int)ATOFM(VV.VVOD_SPISOK_return_data(4));

short dd,md,gd;
rsdat(&dd,&md,&gd,VV.VVOD_SPISOK_return_data(3),1);


if(zapzarpv(d,mz,gz,tabn,prn,knah,suma,VV.VVOD_SPISOK_return_data(5),m,g,kdn,nz,VV.data_ravno_filtr(6),podr,dd,md,gd,nomdok.ravno(),zp) != 0)
 {
  goto naz;
 }
return(0);

}
