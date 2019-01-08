/* $Id: dirtab.c,v 5.39 2013/09/26 09:43:31 sasa Exp $ */
/*31.12.2017	21.05.1999	Белых А.И.	dirtab.c
Просмотр списка табельных номеров
*/
#include        <errno.h>
#include        "buhg.h"

class dirtab_poi
 {
  public:
  class iceb_tu_str obr; //фамилия
  class iceb_tu_str inn; //идентефикационный код
  class iceb_tu_str karts;
  short metka_poi; //0-поиска нет 1-есть

  int mp;  /*0-всех 1-работающих 2-уволенных*/
  int mp1;  /*0-всех 1-имеющих настр. запись 2-не имеющих*/
  int mprov; //0-всех 1-без проводок
  char mpz[64];
  int podr;
  dirtab_poi()
   {
    podr=0;
    clear_data();
   }   
  void clear_data()
   {
    obr.new_plus("");
    inn.new_plus("");
    karts.new_plus("");
    metka_poi=0;
    mp=1;
    mp1=0;
    mprov=0;
    memset(mpz,'\0',sizeof(mpz));
   }
 };
 
void sapptab(class dirtab_poi*,short);
int  provp(class dirtab_poi*,SQL_str row);
void dirtab_rs(class SQLCURSOR *cur,class dirtab_poi *poi);

extern short	mmm,ggg;

int dirtab(int *tbn,//Табельный номер
class iceb_tu_str *fio, //Фамилия
int podr, //Код подразделения
int reg)  //0-ввод и корректир 1-выбор
{
int		kom,kom1;
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		pod;
int		i;
int		K;
int		prc;
long		tabel=0;
double		sald,saldb;
short		pokz; /*0-не показывать сальдо 1 показывать*/
class iceb_t_mstr spis_menu(0);
short		ds,ms,gs;
short		msor; /*0-по фамилиям 1-по табельным номерам*/
short		mspz; /*метка соответствия первой записи*/
short		dnei,kdn;
double		has;
class dirtab_poi poi;
class spis_oth oth;
class iceb_tu_str naimb("");
int kodbanka=0;
//Создаем класс меню
VVOD VV(0);
poi.mp=1;
msor=poi.mp1=poi.mprov=0;
poi.podr=podr;
pod=podr;
pokz=kom=pozz=pozz1=0;

/*Пpи получении 13 месяца ничего не меняем чтобы получить пpавильное сальдо*/
ds=1;
ms=mmm;
gs=ggg;

dpm(&ds,&ms,&gs,3);

naz1:;
clear();
GDITE();
short		kls=LINES-5;

K=0;
memset(strsql,'\0',sizeof(strsql));
if(pod == 0)
 {
  if(msor == 0)
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb order by fio asc");
  else
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb order by tabn asc");
 }
else
 {
  if(msor == 0)
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb where podr=%d \
order by fio asc",pod);
  else
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb where podr=%d \
order by tabn asc",pod);
 } 

SQLCURSOR cur,cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {

lll:;

  sapptab(&poi,pokz);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == FK5)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;

  goto vper;

 }

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  memset(poi.mpz,'\0',sizeof(poi.mpz));
  if(provp(&poi,row) != 0)
    continue;
  
  if(row[4][0] == '1')
   strcat(poi.mpz,"S");      
  naimb.new_plus("");
  if((kodbanka=atoi(row[6])) != 0)
   {
    sprintf(strsql,"select naik from Zarsb where kod=%d",kodbanka);
    if(readkey(strsql,&row1,&cur1) == 1)
     naimb.new_plus(row1[0]);
   }
  /*Вычислям зарплату*/
  if(pokz == 1)
   {
    tabel=atol(row[0]);
    sald=zarsald(1,ms,gs,tabel,&saldb);
    vztb(tabel,mmm,ggg,&dnei,&has,&kdn,2,NULL);
    sprintf(strsql,"%-5s|%-*.*s|%8.2f|%2d|%1s",
    row[0],
    iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
    sald,dnei,poi.mpz);
   }
  else
   sprintf(strsql,"%-5s|%-*.*s|%-2s|%s",
   row[0],
   iceb_tu_kolbait(40,row[1]),iceb_tu_kolbait(40,row[1]),row[1],
   poi.mpz,naimb.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapptab(&poi,pokz);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,2,0,NULL,strsql,prc,&K,"",COLS-24);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(provp(&poi,row) != 0)
      continue;

    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);


    if(provp(&poi,row) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();

  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);


    if(provp(&poi,row) != 0)
      continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
  POLE(spis_menu.spis_str.ravno(kom),strsql,1,'|');
  tabel=atol(strsql);
  polen(spis_menu.spis_str.ravno(kom),fio,2,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     *tbn=tabel;
     return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_5.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case ENTER : /*Проосмотр выбранного*/
     
     if(reg == 0)
      {
       vkart(tabel,1);
       goto naz1;
      }
     else
      {
       *tbn=tabel;
      }      
     return(0);
          
  case FK2 : /*Ввод*/
     vkart(0L,0);
     goto naz1;

  case FK3: /*Удалить карточку*/
    zarudkar(tabel);
    goto naz1;

  case FK4:       /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_MD(gettext("Фамилия................"));
    VV.VVOD_SPISOK_add_MD(gettext("Идентификационный код.."));
    VV.VVOD_SPISOK_add_MD(gettext("Карт счёт (+/-)........"));
    VV.VVOD_SPISOK_add_data(poi.obr.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.inn.ravno(),11);
    VV.VVOD_SPISOK_add_data(poi.karts.ravno(),2);
    
fk4:;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F10",gettext("выход"),NULL);

    kom1=VV.vvod(0,1,1,-1,-1);

    poi.obr.new_plus(VV.VVOD_SPISOK_return_data(0));
    poi.inn.new_plus(VV.VVOD_SPISOK_return_data(1));
    poi.karts.new_plus(VV.VVOD_SPISOK_return_data(2));
    poi.metka_poi=0;

    switch(kom1)
     {
      case FK10:
      case ESC:
         break;                 
      case FK2:
      case PLU:
        poi.metka_poi=1;
        break;
      default:
        goto fk4;
     }   
    clear();
    if(kolstr > 0)
      goto naz; 
    else
      goto lll;
          
  case FK5:       /*Распечатать*/
   GDITE();
   dirtab_rs(&cur,&poi);
   cur.poz_cursor(pozz);
   clear();
   goto naz;

  case FK6: /*Смена режима просмотра*/
    poi.mp++;
    if(poi.mp == 3)
     poi.mp=0;
    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK7: /*Смена режима просмотра*/
    poi.mprov++;
    if(poi.mprov == 2)
     poi.mprov=0;

    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case SFK7: /*Смена режима просмотра*/
    poi.mp1++;
    if(poi.mp1 == 3)
     poi.mp1=0;
    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;


  case FK8: /*Просмотр с расчётом зарплаты*/
    pokz++;
    if(pokz == 2)
     pokz=0;
    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK9: //Смена сортировки
    msor++;
    if(msor == 2)
     msor=0;
    clear();
    goto naz1;

   default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

 }  
return(1);
}

/*********************/
/*Шапка режима работы*/
/*********************/
void sapptab(class dirtab_poi *poi,
short pokz) //0-не показывать сальдо 1 показывать
{
int		Y=0,X=0;
move(0,0);

if(poi->mp == 0)
 {
  attron(A_BLINK);
  printw(gettext("Просмотр всех табельных номеров"));
  attroff(A_BLINK);
 }
if(poi->mp == 1)
 printw(gettext("Просмотр только работающих"));
if(poi->mp == 2)
 {
  attron(A_BLINK);
  printw(gettext("Просмотр только уволенных"));
  attroff(A_BLINK);
 }

printw(" %s %d.%d%s",gettext("Текущая дата"),mmm,ggg,
gettext("г."));

if(poi->podr != 0)
  printw(" %s %d",gettext("Подразделение"),poi->podr);


move(1,0);
if(pokz == 0)
 {
  printw("    1                     2                      3     4");
  move(Y=2,X=COLS-20);
  printw(gettext("Расшифровка полей:"));

  move(++Y,X);
  printw(gettext("1-табельный ном."));

  move(++Y,X);
  printw(gettext("2-фамилия"));

  move(++Y,X);
  printw("3-*-%s",gettext("проводки"));
/************
  move(++Y,X);
  printw(gettext("*-проводки"));
*********************/
  move(++Y,X);
  printw(gettext("  S-совместитель"));

  move(++Y,X);
  printw(gettext("  V-был вход в карт."));

  move(++Y,X);
  printw(gettext("  #-заблокирована"));

  move(++Y,X);
  printw("4-%s",gettext("банк"));
  
 }
if(pokz == 1)
 {
  printw("    1                  2                    3   4  5");
  move(Y=2,X=COLS-20);
  printw(gettext("Расшифровка полей:"));

  move(++Y,X);
  printw(gettext("1-табельный ном."));

  move(++Y,X);
  printw(gettext("2-фамилия"));

  move(++Y,X);
  printw(gettext("3-сальдо"));

  move(++Y,X);
  printw(gettext("4-дни"));

  move(++Y,X);
  printw("5-*-%s",gettext("проводки"));

  move(++Y,X);
  printw(gettext("  S-совместитель"));

  move(++Y,X);
  printw(gettext("  V-был вход в карт."));

  move(++Y,X);
  printw(gettext("  #-заблокирована"));
 }

if(poi->metka_poi != 0 || poi->mprov == 1)
 {
  attron(A_BLINK);
  move(Y+=2,X);
  printw("%s:",gettext("Поиск"));
  if(poi->metka_poi != 0)
   {
    if(poi->obr.getdlinna() > 1)
     {
      move(++Y,X);
      printw("%s",poi->obr.ravno());
     }
    if(poi->inn.getdlinna() > 1)
     {
      move(++Y,X);
      printw("%s",poi->inn.ravno());
     }
    if(poi->karts.getdlinna() > 1)
     {
      mvprintw(++Y,X,"%s",poi->karts.ravno());
     }
   }
  if(poi->mprov == 1)
   {
    move(++Y,X);
    printw(gettext("без проводок"));
   }
  attroff(A_BLINK);
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("увол."),
"F7","*",
"F8",gettext("сальдо"),
"F9",gettext("сорт."),
"F10",gettext("выход"),NULL);

}

/**********/
/*Проверка*/
/**********/
int provp(class dirtab_poi *poi,
SQL_str rows)
{
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
short		mprov1;

if(poi->mp == 2 && rows[2][0] == '0')
  return(1);
  
if(poi->mp == 1 && rows[2][0] != '0')
  return(2);

if(poi->metka_poi == 1)
 {
  if(poi->obr.getdlinna() > 1)
   if(strstrm(rows[1],poi->obr.ravno()) == 0)
    return(3);

  if(poi->inn.getdlinna() > 1)
   if(strstrm(rows[3],poi->inn.ravno()) == 0)
    return(4);

  if(poi->karts.ravno()[0] == '+')
   if(rows[5][0] == '\0')
    return(5);

  if(poi->karts.ravno()[0] == '-')
   if(rows[5][0] != '\0')
    return(6);
 }


mprov1=0;
/*Проверяем наличие промежуточной записи*/

sprintf(strsql,"select prov,blok from Zarn where god=%d and mes=%d \
and tabn=%s",ggg,mmm,rows[0]);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  strcpy(poi->mpz,"V");    
  if(row[0][0] == '0')
   strcat(poi->mpz,"*");
  else
   mprov1=1;
  if(row[1][0] != '0')
   strcat(poi->mpz,"#");
 }
else 
  strcpy(poi->mpz,"*");    

if(poi->mp1 == 2 && poi->mpz[0] == 'V')
  return(4);

if(poi->mp1 == 1 && poi->mpz[0] == '*')
  return(5);


if(poi->mprov == 1 && mprov1 == 1)
  return(6);
  
return(0);

}

void dirtab_create_field(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
/***********************************************/
void dirtab_creatheaddbf(char *imaf,int kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 33 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
/******************
printw("размер заголовка=%d\n",sizeof(h));
printw("размер подзаголовка=%d\n",sizeof(DBASE_FIELD));
printw("размер записи=%d\n",sizeof(f));

OSTANOV();
************************/
memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
dirtab_create_field(&f[shetshik++],"CODE", 'N', 16, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"NUM", 'C', 10, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"NAME", 'C', 60, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"SEX", 'N', 10, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DOCCODE", 'N', 16, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"SERDOC", 'C', 10, 0,&header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"NUMDOC", 'C', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"VYDDOC", 'C', 50, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DATEDOC", 'D', 8, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"NUMTEL", 'C', 30, 0, &header_len,&rec_len);

dirtab_create_field(&f[shetshik++],"ADDR", 'C', 100, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"BIRTHDATE", 'D', 8, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"SHADER", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"WOFEE", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"TAXINSPNUM", 'C', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"WORKBOOK", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DATEADD", 'D', 8, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DATEEND", 'D', 8, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"NOTUSED", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"SPFU", 'N', 10, 0, &header_len,&rec_len);

dirtab_create_field(&f[shetshik++],"SPD", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DRFO", 'C', 12, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"EMAIL", 'C', 100, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"MOBTEL", 'C', 20, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"USEMOB", 'N', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"CERTNAME", 'C', 200, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"EDRPOU", 'C', 10, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DEPID", 'N', 16, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"DEPNAME", 'C', 200, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"LASTNAME", 'C', 100, 0, &header_len,&rec_len);

dirtab_create_field(&f[shetshik++],"FIRSTNAME", 'C', 100, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"MIDDLENAME", 'C', 100, 0, &header_len,&rec_len);
dirtab_create_field(&f[shetshik++],"UKR_GROMAD", 'N', 10, 0, &header_len,&rec_len);

h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

if(write(fd, &h, sizeof(h)) < 0)
 {
  printw("\n%s-%s\n",__FUNCTION__,strerror(errno));
  OSTANOV();
 }

for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) < 0)
   {
    printw("\n%s-%s\n",__FUNCTION__,strerror(errno));
    OSTANOV();
   }
  
 }
fputc('\r', ff);

fclose(ff);

}

/*************************/
/*распечтака*/
/**********************/


void dirtab_rs(class SQLCURSOR *cur,class dirtab_poi *poi)
{
SQL_str row;
char strsql[1024];
char imaf[64];
char imaf_dbf_tmp[64];
char imaf_dbf[64];
char imafz[64];
FILE *ff;
FILE *ff_dbf;
SQL_str row1;
SQLCURSOR cur1;

sprintf(imaf_dbf_tmp,"spisr%d.tmp",getpid());
sprintf(imaf_dbf,"spisr%d.dbf",getpid());
sprintf(imaf,"sr%d.lst",getpid());
sprintf(imafz,"sptnz%d.csv",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

if((ff_dbf = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return;
 }

class iceb_fopen ffz;
if(ffz.start(imafz,"w") != 0)
 return;


iceb_t_zagolov(gettext("Распечатка списка работников"),ff);


if(poi->podr != 0)
 {
  class iceb_tu_str naim_podr("");
  
  sprintf(strsql,"select naik from Podr where kod=%d",poi->podr);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   naim_podr.new_plus(row1[0]);

  fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),poi->podr,naim_podr.ravno());
 }

fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,gettext(" Т/н |   Фамилия, Имя, Отчество\n"));
fprintf(ff,"\
-----------------------------------------------------\n");

fprintf(ffz.ff,"#%s %s\n",gettext("Список работников для загрузки в базу"),iceb_t_get_pnk("00",1));
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(ffz.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



fprintf(ffz.ff,"#Описание полей в записи:\n\
#1  табельный номер\n\
#2  фамилия имя отчество\n\
#3  должность\n\
#4  звание\n\
#5  код подразделения\n\
#6  код категории\n\
#7  дата начала работы\n\
#8  дата конца работы\n\
#9  счёт учета\n\
#10 метка совместителя 0-не совм. 1-совместитель\n\
#11 индивидуальный налоговый номер\n\
#12 адрес\n\
#13 номер паспорта\n\
#14 кем выдан\n\
#15 телефон\n\
#16 код города налоговой инспекции\n\
#17 характер работы\n\
#18 код льгот для формы 8ДР\n\
#19 банковский счёт на который перечисляется зарплата\n\
#20 дата выдачи документа\n\
#21 тип кредитной карты\n\
#22 день рождения\n\
#23 пол 0-мужчина 1-женщина\n\
#24 дата последнего повышения зарплаты\n\
#25 код банка для перечисления зарплаты на карт счёт\n\
#26 код основания для учёта специального стажа\n");


int kolih_rab=0;
class iceb_tu_str seria_dok("");
class iceb_tu_str nom_dok("");
short d,m,g;
char data_vid[64];
int trud_knig=0;
char data_nr[32];
char data_kr[32];
char data_dr[32];
class iceb_tu_str famil("");
class iceb_tu_str ima("");
class iceb_tu_str othestvo("");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(provp(poi,row) != 0)
    continue;
  fprintf(ff,"%-5s %s\n",row[0],row[1]);

  sprintf(strsql,"select * from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }
  kolih_rab++;

  polen(row1[12],&seria_dok,1,' ');
  polen(row1[12],&nom_dok,2,' ');

  rsdat(&d,&m,&g,row1[19],2);
  memset(data_vid,'\0',sizeof(data_vid));
  if(d != 0)
   sprintf(data_vid,"%04d%02d%02d",g,m,d);
  trud_knig=1;
  if(atoi(row1[9]) == 1)  
    trud_knig=0;
  memset(data_nr,'\0',sizeof(data_nr));
  rsdat(&d,&m,&g,row1[6],2);
  if(d != 0)
   sprintf(data_nr,"%04d%02d%02d",g,m,d);
   
  memset(data_kr,'\0',sizeof(data_kr));
  rsdat(&d,&m,&g,row1[7],2);
  if(d != 0)
   sprintf(data_kr,"%04d%02d%02d",g,m,d);
  
  memset(data_dr,'\0',sizeof(data_dr));
  rsdat(&d,&m,&g,row1[21],2);
  if(d != 0)
   sprintf(data_dr,"%04d%02d%02d",g,m,d);
        
  if(polen(row1[1],&famil,1,' ') != 0)
   famil.new_plus(row1[0]);
  polen(row1[1],&ima,2,' ');
  polen(row1[1],&othestvo,3,' ');
  
  fprintf(ff_dbf,"\
 %16.16s\
%-*.*s\
%-*.*s\
%-10.10s\
%-16d\
%-*.*s\
%-*.*s\
%-*.*s\
%-8.8s\
%-*.*s\
%-*.*s\
%-8.8s\
%-10d\
%-10d\
%-10.10s\
%-10d\
%-8.8s\
%-8.8s\
%-10d\
%-10d\
%-10d\
%-12.12s\
%-100.100s\
%-20.20s\
%-10d\
%-200.200s\
%-10.10s\
%-16d\
%-200.200s\
%-*.*s\
%-*.*s\
%-*.*s\
%-10d",
 "", //16
 iceb_tu_kolbait(10,row1[10]),iceb_tu_kolbait(10,row1[10]),row1[10],//10
 iceb_tu_kolbait(60,row1[1]),iceb_tu_kolbait(60,row1[1]),row1[1],   //60
 row1[22], //10
 3,  //16

 iceb_tu_kolbait(10,seria_dok.ravno()),iceb_tu_kolbait(10,seria_dok.ravno()),seria_dok.ravno(),//10
 iceb_tu_kolbait(10,nom_dok.ravno()),iceb_tu_kolbait(10,nom_dok.ravno()),nom_dok.ravno(), //10
 iceb_tu_kolbait(50,row1[13]),iceb_tu_kolbait(50,row1[13]),row1[13], //50
 data_vid, //8
 iceb_tu_kolbait(30,row1[14]),iceb_tu_kolbait(30,row1[14]),row1[14],//30
 iceb_tu_kolbait(100,row1[11]),iceb_tu_kolbait(100,row1[11]),row1[11],//100
 data_dr,//8

 0, //10
 0, //10
 "", //10

 trud_knig,//10
 data_nr, //8
 data_kr, //8

 0, //10
 0, //10
 0, //10

 "", //12
 "", //100
 "", //20

 0, //10
 "", //200
 "", //10

 0,  //16
 "", //200
 iceb_tu_kolbait(100,famil.ravno()),iceb_tu_kolbait(100,famil.ravno()),famil.ravno(), //100
 iceb_tu_kolbait(100,ima.ravno()),iceb_tu_kolbait(100,ima.ravno()),ima.ravno(),  //100
 iceb_tu_kolbait(100,othestvo.ravno()),iceb_tu_kolbait(100,othestvo.ravno()),othestvo.ravno(), //100
 1); //10

/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss код основания для учёта специального стажа
*/
 fprintf(ffz.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
 row1[0],row1[1],row1[2],row1[3],row1[4],row1[5],row1[6],row1[7],row1[8],row1[9],row1[10],row1[11],row1[12],row1[13],
 row1[14],row1[15],row1[16],row1[17],row1[18],row1[19],row1[20],row1[21],row1[22],row1[23],row1[24],row1[25]);

   
 }
//cur.poz_cursor(pozz);
fprintf(ff,"\
-----------------------------------------------------\n\
%s - %d\n",gettext("Общее количество"),kolih_rab);
podpis(ff);

fclose(ff);

fputc(26,ff_dbf);
fclose(ff_dbf);

perecod(2,imaf_dbf_tmp);

dirtab_creatheaddbf(imaf_dbf,kolih_rab);

/*Сливаем два файла*/
iceb_t_cat(imaf_dbf,imaf_dbf_tmp);

unlink(imaf_dbf_tmp);
ffz.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка работников"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Распечатка списка работников в dbf формате"));


oth.spis_imaf.plus(imafz);
oth.spis_naim.plus(gettext("Cписок работников для загрузки в базу"));

iceb_t_rabfil(&oth,"");

}
