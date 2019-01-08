/* $Id: rspis.c,v 5.31 2014/06/30 06:36:34 sasa Exp $ */
/*28.03.2018    03.12.1992      Белых А.И.      rspis.c
Распечатать список работников
*/
#include        <errno.h>
#include        "buhg.h"

int denrog(class spis_oth *);
void rspis_rp(const char *kod_podr,const char *tabnom,const char *kod_kateg,const char *kod_zvan,const char *shetu,FILE *ff);
void		rspis_dbf_h(char *imaf,long kolz);
int zarsrsl(class spis_oth*);

extern short    mmm,ggg;

int rspis(class spis_oth *oth)
{
FILE            *ff,*ff1,*ff2,*ff_all;
char            imaf[64],imaf1[64],imaf2[64],imaf_all[64];
char imaf_dbf_tmp[64];
int             kp1;
short           kolr; /*Количество работников*/
int           kom=0,kom1=0,kom2=0;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kateg("");
class iceb_tu_str podr("");
class iceb_tu_str zvan("");
short		dr,mr,gr;
class iceb_tu_str bros("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_tu_str fiov("");
static class iceb_tu_str kod_podr("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str kod_kateg("");
static class iceb_tu_str kod_zvan("");
static class iceb_tu_str shetu("");
int tbn=0;

VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Список всех работающих"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Список всех уволенных"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Список работающих совместителей"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Список всех работающих c датой рождения"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Список работников для которых применялась социальная льгота"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка работников"));

naz:;
clear();

kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch(kom)
 {
  case 5:
  case -1: 
    return(1);

  case 0:
  case 1:
  case 2:
    break;

  case 3:
   return(denrog(oth));

  case 4:
   return(zarsrsl(oth));
         
  default:
   goto naz;
 }




VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код звания.......(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,).."));


VV.VVOD_SPISOK_add_data(kod_podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_kateg.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_zvan.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu.ravno(),128);

naz2:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom2=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

kod_podr.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));
kod_kateg.new_plus(VV.data_ravno(2));
kod_zvan.new_plus(VV.data_ravno(3));
shetu.new_plus(VV.data_ravno(4));

switch(kom2)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz2;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка званий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    kod.new_plus("");
    naim.new_plus("");
    switch (kom1)
     {
      case 5 :
      case -1:
        goto naz2;
    
      case 0 :
        if(dirzarpodr(2,&kod,&naim) == 0)
          VV.data_z_plus(0,kod.ravno());
        break;

      case 1 :
        fiov.new_plus("");
        if(dirtab(&tbn,&fiov,0,1) == 0)
         {
          VV.data_z_plus(1,tbn);
         }                 
        break;

      case 2 :
        vibrek("Kateg",&kod);
        VV.data_z_plus(2,kod.ravno());
        break;

      case 3 :
        vibrek("Zvan",&kod);
        VV.data_z_plus(3,kod.ravno());
        break;

      case 4 :
        vibrek("Plansh",&kod);
        VV.data_z_plus(4,kod.ravno());
        break;
     }
    goto naz2;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz2;

  default:
     goto naz2;
 }



kp1=VV.data_ravno_atoi(0);

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Сортировка по табельным номерам ?"));
int sortzap=danet(&DANET,2,stdscr);


clear(); /*Очистить экран и закрасить фоновым цветом*/
move(20,0);
GDITE();

if(kp1 == 0)
  sprintf(strsql,"select * from Kartb");
else
  sprintf(strsql,"select * from Kartb where podr=%d",
  kp1);

if(sortzap == 1)
  bros.new_plus(" order by tabn asc");
if(sortzap == 2)
  bros.new_plus(" order by fio asc");
strcat(strsql,bros.ravno());

SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
sprintf(imaf_dbf_tmp,"srk%d.txt",getpid());
class iceb_fopen fil_dbf;
if(fil_dbf.start(imaf_dbf_tmp,"w") != 0)
 return(1);

sprintf(imaf,"sr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
sprintf(imaf_all,"sra%d.csv",getpid());

if((ff_all = fopen(imaf_all,"w")) == NULL)
 {
  error_op_nfil(imaf_all,errno,"");
  return(1);
 }
sprintf(imaf1,"sr1%d.lst",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

sprintf(imaf2,"sr2%d.lst",getpid());

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

char imaf_ks[64];
sprintf(imaf_ks,"sr_ks%d.lst",getpid());
FILE *ff_ks;
if((ff_ks = fopen(imaf_ks,"w")) == NULL)
 {
  error_op_nfil(imaf_ks,errno,"");
  return(1);
 }

char imaf_okl[64];
sprintf(imaf_okl,"sr_ok%d.lst",getpid());
FILE *ff_okl;
if((ff_okl = fopen(imaf_okl,"w")) == NULL)
 {
  error_op_nfil(imaf_okl,errno,"");
  return(1);
 }

fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(ff1,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(ff2,"%s\n\n",iceb_t_get_pnk("00",0));
fprintf(ff_ks,"%s\n\n",iceb_t_get_pnk("00",0));
if(kp1 == 0)
 {
  printw("%s\n",gettext("Список по всем подразделениям"));
 }

if(kom == 2)
 {
  printw("%s\n",gettext("Список работающих совместителей"));
  fprintf(ff,"%s\n",gettext("Список работающих совместителей"));
  fprintf(ff1,"%s\n",gettext("Список работающих совместителей"));
  fprintf(ff2,"%s\n",gettext("Список работающих совместителей"));
  fprintf(ff_ks,"%s\n",gettext("Список работающих совместителей"));
  fprintf(ff_okl,"%s\n",gettext("Список работающих совместителей"));
 }
GDITE();

iceb_t_zagolov(gettext("Список работников с окладами"),ff_okl);

rspis_rp(kod_podr.ravno(),tabnom.ravno(),kod_kateg.ravno(),kod_zvan.ravno(),shetu.ravno(),ff);
rspis_rp(kod_podr.ravno(),tabnom.ravno(),kod_kateg.ravno(),kod_zvan.ravno(),shetu.ravno(),ff1);
rspis_rp(kod_podr.ravno(),tabnom.ravno(),kod_kateg.ravno(),kod_zvan.ravno(),shetu.ravno(),ff2);
rspis_rp(kod_podr.ravno(),tabnom.ravno(),kod_kateg.ravno(),kod_zvan.ravno(),shetu.ravno(),ff_ks);
rspis_rp(kod_podr.ravno(),tabnom.ravno(),kod_kateg.ravno(),kod_zvan.ravno(),shetu.ravno(),ff_okl);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("Т/н |               Ф. И. О.                 |Инд. номер|Счёт |Дата приём./увольнения |  Должность |Категория|Подразделение|Льготы| Звание |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
--------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
Т/н |               Ф. И. О.                 |          |          |\n"));
fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff2,gettext("\
Т/н |               Ф. И. О.                 |Инд. номер|Дата рожд.|                 Адрес                            | N паспорта    |               Кем выдан                          |Дата в/п  | Телефон\n"));
fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");
fprintf(ff_ks,gettext("\
 Т/н |               Ф. И. О.                 |Инд. номер|     Карт-счёт      |\n"));
fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");

fprintf(ff_okl,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff_okl,gettext("\
 Т/н |               Ф. И. О.                 |   Оклад  |          |          |          |          |\n"));
fprintf(ff_okl,"\
------------------------------------------------------------------------------------------------------\n");
/*
12345 1234567890123456789012345678901234567890 12345678901234567890
*/
short dv,mv,gv;
double sum_okl=0.;
int metka_hs=0;
kolr=0;
kolstr1=0;
int nomstr=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;    
  if(proverka(kod_kateg.ravno(),row[5],0,0) != 0)
   continue;    
  if(proverka(kod_zvan.ravno(),row[3],0,0) != 0)
   continue;    
  if(proverka(shetu.ravno(),row[8],0,0) != 0)
   continue;    
  if(kom == 0 && row[7][0] != '0')
     continue;
  if(kom == 1 && row[7][0] == '0')
    continue;
  if(kom == 2 && (row[9][0] == '0' || row[7][0] != '0' ))
    continue;

  kolr++;
  printw("%-4s %-30s %4s %s\n",row[0],row[1],row[8],row[5]);

  /*Определяем наименование подразделения*/
  sprintf(strsql,"select naik from Podr where kod=%s",row[4]);
  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("TN%s: %s %s !\n",row[0],gettext("Не найден код подразделения"),row[4]);
    OSTANOV();
    podr.new_plus("");
   }  
  else
   podr.new_plus(row1[0]);

  /*Определяем наименование категории*/
  sprintf(strsql,"select naik from Kateg where kod=%s",row[5]);
  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("TN%s: %s %s !\n",row[0],gettext("Не найден код категории"),row[5]);
    OSTANOV();
    kateg.new_plus("");
   }  
  else
   kateg.new_plus(row1[0]);

  /*Определяем наименование звания*/
  sprintf(strsql,"select naik from Zvan where kod=%s",row[3]);
  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("TN%s: %s %s !\n",row[0],gettext("Не найден код звания"),row[3]);
    zvan.new_plus("");
   }  
  else
   zvan.new_plus(row1[0]);

  fprintf(ff,"%-4s|%-*s|%-10s|%-*s|%-11s|%-11s|%-*.*s|%-*.*s|%-*.*s|%-6.6s|%s\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  row[10],
  iceb_tu_kolbait(5,row[8]),row[8],
  row[6],
  row[7],
  iceb_tu_kolbait(12,row[2]),
  iceb_tu_kolbait(12,row[2]),
  row[2],
  iceb_tu_kolbait(9,kateg.ravno()),
  iceb_tu_kolbait(9,kateg.ravno()),
  kateg.ravno(),
  iceb_tu_kolbait(13,podr.ravno()),
  iceb_tu_kolbait(13,podr.ravno()),
  podr.ravno(),
  row[17],zvan.ravno());

  fprintf(ff_all,"%-4s|%-*s|%-10s|%-5s|%-11s|%-11s|%-12s|%-9s|%-13s|%-6s|%s\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  row[10],row[8],row[6],row[7],row[2],kateg.ravno(),podr.ravno(),row[17],zvan.ravno());

  fprintf(ff1,"%-4s %-*s|%10s|%10s|\n\
....................................................................\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),
  row[1],
  " "," ");

  dr=mr=gr=0;

  rsdat(&dr,&mr,&gr,row[21],2);
  rsdat(&dv,&mv,&gv,row[19],2);
      
  fprintf(ff2,"%-4s|%-*s|%-10s|%02d.%02d.%04d|%-*.*s|%-*s|%-*.*s|%02d.%02d.%04d|%s\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  row[10],
  dr,mr,gr,
  iceb_tu_kolbait(50,row[11]),
  iceb_tu_kolbait(50,row[11]),
  row[11],
  iceb_tu_kolbait(15,row[12]),
  row[12],
  iceb_tu_kolbait(50,row[13]),
  iceb_tu_kolbait(50,row[13]),
  row[13],
  dv,mv,gv,row[14]);

  nomstr++;
  fprintf(fil_dbf.ff," %10s%-*.*s%-*.*s%02d%02d%04d%-*.*s%-*.*s%-*.*s%02d%02d%04d%-*.*s",
  row[0], 
  iceb_tu_kolbait(60,row[1]),
  iceb_tu_kolbait(60,row[1]),
  row[1],
  iceb_tu_kolbait(15,row[10]),
  iceb_tu_kolbait(15,row[10]),
  row[10],
  dr,mr,gr,
  iceb_tu_kolbait(100,row[11]),
  iceb_tu_kolbait(100,row[11]),
  row[11],
  iceb_tu_kolbait(30,row[12]),
  iceb_tu_kolbait(30,row[12]),
  row[12],
  iceb_tu_kolbait(100,row[13]),
  iceb_tu_kolbait(100,row[13]),
  row[13],
  dv,mv,gv,
  iceb_tu_kolbait(50,row[14]),
  iceb_tu_kolbait(50,row[14]),
  row[14]);

  fprintf(ff_ks,"%-5s|%-*s|%10s|%-20s\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  row[10],row[18]);

  memset(strsql,'\0',sizeof(strsql));

  sum_okl=oklad(atoi(row[0]),mmm,ggg,&metka_hs);

  if(sum_okl > 0.)
   {
    if(metka_hs == 0) /*Оклад*/
     {
      sprintf(strsql,"%.10g",sum_okl);
     }
    if(metka_hs == 1) /*часовая ставка*/
     {
      sprintf(strsql,"%.10gчс",sum_okl);
     }
   }
  
  fprintf(ff_okl,"%-5s|%-*s|%10s|%10s|%10s|%10s|%10s|\n",
  row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  strsql,"","","","");

  fprintf(ff_okl,"\
------------------------------------------------------------------------------------------------------\n");
  
 }

fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");


fprintf(ff,"\n%s:%d\n\n",
gettext("Количество работников"),kolr);

podpis(ff);
podpis(ff1);
podpis(ff2);
podpis(ff_ks);
podpis(ff_all);
podpis(ff_okl);

fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ff_ks);
fclose(ff_all);
fclose(ff_okl);

fputc(26,fil_dbf.ff);

fil_dbf.end();

perecod(2,imaf_dbf_tmp);
char imafdbf[64];

sprintf(imafdbf,"srk%d.dbf",getpid());

rspis_dbf_h(imafdbf,nomstr);

/*Сливаем два файла*/
iceb_t_cat(imafdbf,imaf_dbf_tmp);
unlink(imaf_dbf_tmp);




printw("\n%s - %d\n\n\
%s %s %s %s.\n\n",
gettext("Количество работников"),kolr,
gettext("Распечатка выгружена в файл"),imaf,imaf1,imaf2);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список работников"));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Пустографка"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Список работников для оформления карт-счетов"));
oth->spis_imaf.plus(imaf_ks);
oth->spis_naim.plus(gettext("Список работников с карт-счетами"));
oth->spis_imaf.plus(imaf_okl);
oth->spis_naim.plus(gettext("Список работников с окладами"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imafdbf);
oth->spis_naim.plus(gettext("Список работников для оформления карт-счетов"));

oth->spis_imaf.plus(imaf_all); /*ненужно устанавливать режими печати это для загрузки в электронные таблицы*/
oth->spis_naim.plus(gettext("Список работников"));

return(0);

}

/***********************/
/*распечатка реквизитов поиска*/
/*****************************/
void rspis_rp(const char *kod_podr,
const char *tabnom,
const char *kod_kateg,
const char *kod_zvan,
const char *shetu,
FILE *ff)
{
if(kod_podr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),kod_podr);
if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
if(kod_kateg[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код категории"),kod_kateg);
if(kod_zvan[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код звания"),kod_zvan);
if(shetu[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shetu);

}

/*****************************************************************/

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void rspis_dbf_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void		rspis_dbf_h(char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  9
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

memset(&h,'\0',sizeof(h));

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

rspis_dbf_f(&f[shetshik++],"TN", 'N', 10, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"FIO", 'C', 60, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"INN", 'N', 15, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"DR", 'D', 8, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"ADRES", 'C',100, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"NP", 'C',30, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"KV", 'C', 100, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"DVP", 'D', 8, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"TELEF", 'C', 50, 0,&header_len,&rec_len);

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
