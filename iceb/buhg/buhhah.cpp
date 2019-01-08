/*$Id: buhhah.c,v 5.31 2014/06/30 06:36:34 sasa Exp $*/
/*13.07.2015	18.02.2006	Белых А.И.	buhhah.c
Расчёт шахматки по счетам введённым в файл
*/
#include <errno.h>
#include        "buhg.h"

int buhhar1_rs(short dn,short mn,short gn,short dk,short mk,short gk,int,class SQLCURSOR*,int,class SPISOK*,FILE*,FILE*);
void buhhah1_sort(class SPISOK *sheta);


int buhhah(class spis_oth *oth)
{

char strsql[512];
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str variant_r("");

short           dn,mn,gn;
short           dk,mk,gk;
short metka_ras=0;
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите даты для расчёта шахматки"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Вариант расчёта..(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(variant_r.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+/-",gettext("расчитать"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
variant_r.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_7.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     metka_ras=0;
     break;

  case OTN: //Расчёт в котором кореспонденция между счетами из которых 
            //составлен список не входит в  отчёт
     
     metka_ras=1;
     break;

  case FK5:
     iceb_redfil("buhhah.alx",0);
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


GDITE();

sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and deb <> 0. order by datp asc",gn,mn,dn,gk,mk,dk);

class SQLCURSOR cur;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
//printw("Количество проводок:%d\n",kolstr);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr_alx=0;
sprintf(strsql,"select str from Alx where fil='buhhah.alx' order by ns asc");
if((kolstr_alx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr_alx == 0)
 {
  iceb_t_soob("Не найдены настройки buhhah.alx");
  return(1);
 }

class iceb_tu_str variant("");

char imaf[64];
FILE *ff;

sprintf(imaf,"hax%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imaf1[64];
FILE *ff1;

sprintf(imaf1,"haxg%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

class iceb_tu_str naim_oth("");
class iceb_tu_str spis_sh("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  

  if(polen(row_alx[0],&variant,1,'|') != 0)
   continue;
  if(SRAV("Вариант расчёта",variant.ravno(),0) != 0)
   continue;

  polen(row_alx[0],&variant,2,'|');
  if(variant.ravno()[0] == '\0')
   continue;

  if(proverka(variant_r.ravno(),variant.ravno(),0,0) != 0)
   continue;
  
  class SPISOK sheta;
  
  /*Читаем наименование отчёта*/
  cur_alx.read_cursor(&row_alx);
  polen(row_alx[0],&naim_oth,2,'|');
  
  /*Читаем список счетов*/
  cur_alx.read_cursor(&row_alx);
  polen(row_alx[0],&spis_sh,2,'|');
  iceb_t_fplus(0,spis_sh.ravno(),&sheta,&cur_alx);
  
  iceb_t_zagolov(naim_oth.ravno(),dn,mn,gn,dk,mk,gk,ff);
  iceb_t_zagolov(naim_oth.ravno(),dn,mn,gn,dk,mk,gk,ff1);

  buhhar1_rs(dn,mn,gn,dk,mk,gk,metka_ras,&cur,kolstr,&sheta,ff,ff1);

 }

podpis(ff);
fclose(ff);
podpis(ff1);
fclose(ff1);


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Заказанные счета по вертикали"));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Заказанные счета по горизонтали"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}

/************************************************/
/*Расчёт по варианту*/
/*************************************************/

int buhhar1_rs(short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_ras,
class SQLCURSOR *cur,
int kolstr,
class SPISOK *sheta,
FILE *ff,
FILE *ff1)
{
SQL_str row;
char strsql[512];


char imaf_tmp[64];
FILE *ff_tmp;

sprintf(imaf_tmp,"hax%d.tmp",getpid());
if((ff_tmp = fopen(imaf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_tmp,errno,"");
  return(1);
 }

class SPISOK sheta_k;

int kolstr1=0;
int kol_zap=0;
//Создаём список счетов корреспондентов
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(metka_ras == 1)
   if(sheta->find(row[1],1) >= 0)
    continue;
    
  if(sheta->find(row[0],1) < 0)
   continue;
  
  if(sheta_k.find(row[1],1) < 0)
   sheta_k.plus(row[1]);       
    
  fprintf(ff_tmp,"%s|%s|%s|\n",row[0],row[1],row[2]);
  kol_zap++;
 }

fclose(ff_tmp);

if(kol_zap == 0)
 {
  unlink(imaf_tmp);
  iceb_t_soob(gettext("Не найдено ни одной проводки со счётом из файла настройки !"));
  return(1);
 }

buhhah1_sort(&sheta_k); //сортируем список счетов
 
if((ff_tmp = fopen(imaf_tmp,"r")) == NULL)
 {
  error_op_nfil(imaf_tmp,errno,"");
  return(1);
 }

unlink(imaf_tmp);



class masiv_din_double suma_d; //Массив сумм
suma_d.make_class(sheta->kolih()*sheta_k.kolih());

kolstr1=0;
class iceb_tu_str stroka("");
//Заполняем массив сумм по счетам
class iceb_tu_str shet("");
class iceb_tu_str shet_k("");
double suma;
int nomer_sh;
int nomer_shk;
int kol_sh_k=sheta_k.kolih();

while(iceb_tu_fgets(&stroka,ff_tmp) == 0)
 {
  
  strzag(LINES-1,0,kol_zap,++kolstr1);
  polen(stroka.ravno(),&shet,1,'|');
  polen(stroka.ravno(),&shet_k,2,'|');
  polen(stroka.ravno(),strsql,sizeof(strsql),3,'|');
  suma=atof(strsql);
  nomer_sh=sheta->find(shet.ravno(),1);
  nomer_shk=sheta_k.find(shet_k.ravno(),1);
  suma_d.plus(suma,nomer_sh*kol_sh_k+nomer_shk);
 }

fclose(ff_tmp);

GDITE();


buh_rh(0,sheta,&sheta_k,&suma_d,ff); //распечатываем массив
buh_rh(1,sheta,&sheta_k,&suma_d,ff1); //распечатываем массив



return(0);
}

/********************************/
/*Сортировка массива счетов*/
/*****************************/

void buhhah1_sort(class SPISOK *sheta)
{
SQL_str rowtmp;
class SQLCURSOR curtmp;
char strsql[512];
class iceb_t_tmptab tabtmp;
const char *imatmptab={"buhhahw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
  return;
  

for(int i=0; i < sheta->kolih(); i++)
 {

  sprintf(strsql,"insert into %s values ('%s')", imatmptab,sheta->ravno(i));

  sql_zapis(strsql,1,0);    
  
 }
sprintf(strsql,"select * from %s order by sh asc",imatmptab);
int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return;
 }

sheta->free_class();

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sheta->plus(rowtmp[0]);
 }
}
