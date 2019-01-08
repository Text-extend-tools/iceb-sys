/*$Id: uos_otvm.c,v 5.22 2013/08/13 05:49:56 sasa Exp $*/
/*20.05.2016	09.06.2005	Белых А.И.	uos_otvm.c
Отчёт в министерство для завода Форт
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

void uos_otvm_sap(int *kolstrok,int *kollist,FILE *ff);
void uos_otvm_ss(int *kolstrok,int *kollist,FILE *ff);
void uos_otvm_h(char *imaf,long kolz);

extern int kol_strok_na_liste;

int uos_otvm(class spis_oth *oth)
{

char strsql[1024];
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
int kolstr;
short dos=0,mos=0,gos=0;
static class iceb_tu_str dat1(""),dat2(""),dat3("");
static class iceb_tu_str inv_nom("");
static class iceb_tu_str grup_buhu("");

int kom=0;
if(dat1.ravno()[0] == '\0')
 {
  poltekdat(&dos,&mos,&gos);
  sprintf(strsql,"%d.%d.%d",dos,mos,gos);
  dat1.new_plus(strsql); 
 }


VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать отчёт для министерства"));

VV.VVOD_SPISOK_add_MD(gettext("Дата остатка.........(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата индексации......(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата инвентаризации..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер......(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Группа бух.учёта.......(,,)..."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(inv_nom.ravno(),128); 
VV.VVOD_SPISOK_add_data(grup_buhu.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
dat3.new_plus(VV.data_ravno(2));
inv_nom.new_plus(VV.data_ravno(3));
grup_buhu.new_plus(VV.data_ravno(4));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }




if(rsdat(&dos,&mos,&gos,dat1.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата остатка !"));
  return(1); 
 }
short dind,mind,gind;

if(rsdat(&dind,&mind,&gind,dat2.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата индексации !"));
  return(1); 
 }
short dinv,minv,ginv;

if(rsdat(&dinv,&minv,&ginv,dat3.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата инвентаризации !"));
  return(1); 
 }
clear();
GDITE();


sprintf(strsql,"select innom,naim from Uosin");
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
char imaf_tmp[64];
FILE *ff_tmp;

sprintf(imaf_tmp,"othvm%d.tmp",getpid());
if((ff_tmp = fopen(imaf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_tmp,errno,"");
  return(1);
 }


char imaf[64];
FILE *ff;

sprintf(imaf,"othvm%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
int kolstrok=0;
int kollist=0;

class iceb_tu_str adres("");
class iceb_tu_str kod("");
class iceb_tu_str naikon("");

sprintf(strsql,"select naikon,adres,kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row1,&cur1) == 1)
 {
  naikon.new_plus(row1[0]);
  adres.new_plus(row1[1]);
  kod.new_plus(row1[2]);
 }
 
kolstrok=11;
fprintf(ff,"\
                            Перелік нерухомого державного майна, що знаходиться на балансі\n\
                            %s\n\n\
Ідентифікаційний код ЄДРПОУ балансоутримувача:%s\n\
Повне найменування балансоутримувача:%s\n\
Місце знаходження балансоутримувача:%s\n\n\
Дані про земельну ділянку:\n\n\
розмір земельної ділянки______________м.кв.\n\
відповідно до державного акта на право власності на земельну ділянку від_____________N________\n",
naikon.ravno(),kod.ravno(),naikon.ravno(),adres.ravno());

uos_otvm_sap(&kolstrok,&kollist,ff);

long inn;
int kolstr1=0;
int podr=0;
int kol=0;
int nomer_zap=0;
double bs=0.,obs=0.;
class iceb_tu_str naim("");
class bsizw_data bal_st;
double iobs=0.;
class iceb_tu_str naim_podr("");
long nomstr=0;


  fprintf(ff_tmp,"\
 \
%19d\
%19d\
%19d\
%19d\
%19d\
%19d\
%-5s\
%19d\
%1.1s\
%-170s\
%-7s\
%-150s\
%-150ld\
%-17s\
%-19s\
%-19s\
%-19s\
%-10s\
%19.2f\
%19.2f\
%-19s\
%02d%02d%04d\
%02d%02d%04d\
%-19s\
%-19s\
%1.1s\
%-19s\
%-35s\
%-10s\
%-8s\
%-10s\
%-100s\
%-10s\
%-8s\
%-19s\
%-50s\
%-19s\
%-10s\
%-8s\
%-8s\
%-19s\
%1.1s",
1,
0,
0,
0,
0,
1,
"020",
2,
"",
"",
"",
"",
0L,
"1",
"",
"",
"",
"",
0.,
0.,
"",
dind,mind,gind,
dinv,minv,ginv,
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"");


while(cur.read_cursor(&row) != 0)
 {
  if(proverka(inv_nom.ravno(),row[0],0,0) != 0)
   continue;
  if(grup_buhu.ravno()[0] != '\0')
   {
    class poiinpdw_data rekin;
    poiinpd(atol(row[0]),mos,gos,&rekin);
    if(proverka(grup_buhu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
     continue;    
   }   
  printw("%5s %s\n",row[0],row[1]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  inn=atol(row[0]);
  if(poiin(inn,dos,mos,gos,&podr,&kol) != 0)
   continue;

  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_podr.new_plus(row1[0]);
  else
   naim.new_plus("");   
  bsiz(inn,podr,dos,mos,gos,&bal_st,NULL);

/********
  //налоговый учёт
  bs=sbs+pr1nu;
  obs=bs-(siz+pr2nu+iz1);
*************/
  //бухгалтерский учёт
//  bs=sbsby+pr1by;
//  obs=bs-(sizby+pr2by+izby1);
  bs=bal_st.sbsby+bal_st.bsby;
  obs=bs-(bal_st.sizby+bal_st.izby+bal_st.iz1by);
  
  obs/=1000.; //в тис.грн.
  bs/=1000.;
    
  naim.new_plus(row[1]);

  uos_otvm_ss(&kolstrok,&kollist,ff);

  fprintf(ff,"%5d %-10s %-*.*s %-*.*s %16.3f %02d.%02d.%d\n",
  ++nomer_zap,
  " ",
  iceb_tu_kolbait(13,naim.ravno()),iceb_tu_kolbait(13,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(16,naim_podr.ravno()),iceb_tu_kolbait(16,naim_podr.ravno()),naim_podr.ravno(),bs,dind,mind,gind);

  fprintf(ff,"%5s %-10ld %-*.*s %-*.*s %16.3f %02d.%02d.%d\n",
  " ",
  inn,
  iceb_tu_kolbait(13,iceb_tu_adrsimv(13,naim.ravno())),
  iceb_tu_kolbait(13,iceb_tu_adrsimv(13,naim.ravno())),
  iceb_tu_adrsimv(13,naim.ravno()),
  iceb_tu_kolbait(16,iceb_tu_adrsimv(16,naim_podr.ravno())),
  iceb_tu_kolbait(16,iceb_tu_adrsimv(16,naim_podr.ravno())),
  iceb_tu_adrsimv(16,naim_podr.ravno()),
  obs,dinv,minv,ginv);
  
  fprintf(ff,"%5s %10s %-*.*s\n",
  " "," ",
  iceb_tu_kolbait(13,iceb_tu_adrsimv(13,naim.ravno())),
  iceb_tu_kolbait(13,iceb_tu_adrsimv(13,naim.ravno())),
  iceb_tu_adrsimv(13,naim.ravno()));
  
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
  iobs+=obs;

  nomstr++;  
  fprintf(ff_tmp,"\
 \
%19d\
%19d\
%19d\
%19d\
%19d\
%19d\
%-5s\
%19d\
%1.1s\
%-*s\
%-7s\
%-*s\
%-150ld\
%-17s\
%-19s\
%-19s\
%-19s\
%-10s\
%19.2f\
%19.2f\
%-19s\
%02d%02d%04d\
%02d%02d%04d\
%-19s\
%-19s\
%1.1s\
%-19s\
%-35s\
%-10s\
%-8s\
%-10s\
%-100s\
%-10s\
%-8s\
%-19s\
%-50s\
%-19s\
%-10s\
%-8s\
%-8s\
%-19s\
%1.1s",
1,
gos,
mos,
dos,
0,
1,
"020",
2,
"",
iceb_tu_kolbait(170,naim.ravno()),naim.ravno(),
"",
iceb_tu_kolbait(150,adres.ravno()),adres.ravno(),
inn,
"1",
"",
"",
"",
"",
bs,
obs,
"",
dind,mind,gind,
dinv,minv,ginv,
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"");

 }

fputc(26, ff_tmp);
fclose(ff_tmp);

fprintf(ff,"\n\
Загалом структурних компонентів:___________________\n\
Усього державного майна:%d, залишковою вартістю %.3f тис.грн.\n",
nomer_zap,iobs);
fprintf(ff,"\n\
        ------------------------             -----------------------           -----------------------\n\
               (Керівник)                            (Підпис)                   (Прізвище та ініціали)\n");
               
podpis(ff);

fclose(ff);



//Перекодировка
perecod(1,imaf_tmp);
char imafdbf[64];

sprintf(imafdbf,"ovm.dbf");
uos_otvm_h(imafdbf,nomstr);

/*Сливаем два файла*/
iceb_t_cat(imafdbf,imaf_tmp);
unlink(imaf_tmp);



oth->spis_imaf.plus(imaf);
oth->spis_naim.plus("Отчёт в министерство");
oth->spis_imaf.plus(imafdbf);
oth->spis_naim.plus("Отчёт в министерство в электронном виде");

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);

}

/********************************************/
/*Счётчик сторок*/
/********************************************/
void uos_otvm_ss(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=4;
if(*kolstrok <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kolstrok=0;

uos_otvm_sap(kolstrok,kollist,ff);
*kolstrok+=4; 

}

/*********************************************/
/*Шапка*/
/***********************************************/

void uos_otvm_sap(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=12;
*kollist+=1;

fprintf(ff,"%100s %s N%d\n",
" ",
gettext("Лист"),
*kollist);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n\
     |          |             |                |Вартість об'єкта|Дата інде-|  Розмір  |      |             Дані про реєстрацію           |\n\
     |Код та наз|             |                |     тис.грн.   |  ксації  |земельної |      |                                           |\n\
     |ва об'єкту|             |                |----------------|основних  |ділянки   |      |--------------------------------------------\n\
 N   |          |             |                |   балансова    | фондів   | м.кв.    |      |           |          |          |         |\n\
п/п  |          |             |Місцезнаходження|----------------|          |          |      |Кадастровий|  Орган   |          |         |\n\
     |----------|Назва об'єкта|   об'єкта      |   залишкова    |----------|----------|Ознака|  номер    |реэстрації|  Дата    |  Номер  |\n\
     |Інвен-ний |             |                |----------------|Дата інве-|Площа     |      | згідно з  |          |          |         |\n\
     | номер    |             |                |   експертна    |нтаризації|забудована|      |    БТІ    |          |          |         |\n\
     |          |             |                |                |          |  м.кв.   |      |           |          |          |         |\n\
------------------------------------------------------------------------------------------------------------------------------------------\n");
//12345 1234567890 1234567890123 1234567890123456 1234567890123456
}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void uos_otvm_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void uos_otvm_h(char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  42
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
uos_otvm_f(&f[shetshik++],"user", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"god", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"mes", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"days", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"kv", 'N', 19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"nom", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"cod", 'C', 5, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"rz", 'N',19,5,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"nototal", 'L',1, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"namep", 'C',170, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"coda", 'C', 7, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codb", 'C', 150, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codc", 'C', 150, 0,&header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codd", 'C',17, 0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codn", 'N', 19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codnc", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codnx", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"codz", 'C',10,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val1", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val2", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val3", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val4", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val5", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val6", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val7", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val8", 'L',1,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val9", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val10", 'C',35,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val11", 'C',10,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val12", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val13", 'C',10,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val14", 'C',100,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val15", 'C',10,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val16", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val17", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val18", 'C',50,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val19", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val20", 'C',10,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val21", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val22", 'D',8,0, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"val23", 'N',19,5, &header_len,&rec_len);
uos_otvm_f(&f[shetshik++],"sp", 'C',1,0, &header_len,&rec_len);


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
