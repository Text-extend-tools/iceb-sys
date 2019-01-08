/*30.03.2017	27.11.2016	Белых А.И.	fort_rz.c
отчёт
*/
#include <buhl.h>

class fort_rz_rp
 {
  public:
   class iceb_tu_str datn;
   class iceb_tu_str datk;
   class iceb_tu_str pod;
   class iceb_tu_str hz;

  fort_rz_rp()
   {
    clear();
   }

  void clear()
   {
    hz.new_plus("");
    datn.new_plus("");
    datk.new_plus("");
    pod.new_plus("");
    
   }   
 };
 

int fort_rz(class spis_oth *oth)
{
char strsql[1024];
int kom=0;
class fort_rz_rp rp;


class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.......(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца........(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат........(,,).."));//3

VV.VVOD_SPISOK_add_data(rp.datn.ravno(),8);
VV.VVOD_SPISOK_add_data(rp.datk.ravno(),8);
VV.VVOD_SPISOK_add_data(rp.pod.ravno(),128);
VV.VVOD_SPISOK_add_data(rp.hz.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rp.datn.new_plus(VV.data_ravno(0));
rp.datk.new_plus(VV.data_ravno(1));
rp.pod.new_plus(VV.data_ravno(2));
rp.hz.new_plus(VV.data_ravno(3));
switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
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

if(rp.datn.prov_dat1() != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата начала!"));
  goto naz;
 }

if(rp.datk.getdlinna() <=1)
 {
  rp.datk.new_plus(rp.datn.ravno());
 }

if(rp.datk.prov_dat1() != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата конца!"));
  goto naz;
 }
 
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
class masiv_din_int podr;
class masiv_din_int hz;
class masiv_din_double podr_hz_v;
class masiv_din_double podr_hz_sum;
/*Узнаём подразделения*/
sprintf(strsql,"select distinct pd from Fortzn where datan >= '%s' and datan <= '%s'",rp.datn.ravno_sqldata(),rp.datk.ravno_sqldata());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  goto naz;
 }
 
while(cur.read_cursor(&row) != 0)
 {

  if(proverka(rp.pod.ravno(),row[0],0,0) != 0)
   continue;
   
  podr.plus(atoi(row[0]));
 }

/*Узнаём шифры затрат*/
sprintf(strsql,"select distinct hz from Fortzn where datan >= '%s' and datan <= '%s'",rp.datn.ravno_sqldata(),rp.datk.ravno_sqldata());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(rp.hz.ravno(),row[0],0,0) != 0)
   continue;
  hz.plus(atoi(row[0]));
 }

/*создаём массив*/
sprintf(strsql,"select hz,pd,nv,rc,kol  from Fortzn where datan >= '%s' and datan <= '%s'",rp.datn.ravno_sqldata(),rp.datk.ravno_sqldata());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

podr_hz_v.make_class(podr.kolih()*hz.kolih());
podr_hz_sum.make_class(podr.kolih()*hz.kolih());

double vrem=0.;
double suma=0.;
int nomer_pod=0;
int nomer_hz=0;
int kol_podr=podr.kolih();

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(rp.pod.ravno(),row[1],0,0) != 0)
   continue;
  if(proverka(rp.hz.ravno(),row[0],0,0) != 0)
   continue;

  vrem=atof(row[2])*atof(row[4])/60.;
  suma=atof(row[3])*atof(row[4])/100.;          

  nomer_pod=podr.find(row[1]);  
  nomer_hz=hz.find(row[0]);
  
  podr_hz_v.plus(vrem,nomer_hz*kol_podr+nomer_pod);
  podr_hz_sum.plus(suma,nomer_hz*kol_podr+nomer_pod);
 }

char imaf[64];
sprintf(imaf,"fort%d.lst",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return(1);
 
iceb_t_zagolov("Зведення по підрозділам и шифрам витрат",rp.datn.ravno(),rp.datk.ravno(),fil.ff);

if(rp.pod.getdlinna() > 1)
 fprintf(fil.ff,"Підрозділ:%s\n",rp.pod.ravno());

if(rp.hz.getdlinna() > 1)
 fprintf(fil.ff,"Шифр витрат:%s\n",rp.hz.ravno());
  
class iceb_tu_str liniq("-------");
for(int nom=0; nom <= kol_podr; nom++)
 liniq.plus("----------------------");

class iceb_tu_str liniq1;
for(int nom=0; nom <= kol_podr; nom++)
 liniq1.plus("----------------------");

fprintf(fil.ff,"%s\n",liniq.ravno());
 
fprintf(fil.ff," Шифр |");

for(int nom=0; nom < kol_podr;nom++)
  fprintf(fil.ff,"%21d|",podr.ravno(nom));

fprintf(fil.ff,"        Разом        |\n");

fprintf(fil.ff,"витрат|%s\n",liniq1.ravno());

fprintf(fil.ff,"      |");
for(int nom=0; nom <= kol_podr;nom++)
  fprintf(fil.ff,"Норма часу|   Сума   |");

fprintf(fil.ff,"\n");

fprintf(fil.ff,"%s\n",liniq.ravno());


double itogo[2];

for(int nom_hz=0; nom_hz < hz.kolih(); nom_hz++)
 {
  itogo[0]=itogo[1]=0.;
  
  fprintf(fil.ff,"%6d|",hz.ravno(nom_hz));
  for(int nom_pod=0; nom_pod < kol_podr; nom_pod++)
   {
    vrem=podr_hz_v.ravno(nom_hz*kol_podr+nom_pod);
    suma=podr_hz_sum.ravno(nom_hz*kol_podr+nom_pod);
 
    if(suma != 0. || vrem != 0.)
     fprintf(fil.ff,"%10.2f|%10.2f|",vrem,suma);
    else
     fprintf(fil.ff,"%10s|%10s|","","");

    itogo[0]+=vrem;
    itogo[1]+=suma;
   }  
  fprintf(fil.ff,"%10.2f|%10.2f|\n",itogo[0],itogo[1]);
 }

fprintf(fil.ff,"%s\n",liniq.ravno());

fprintf(fil.ff,"%*s|",iceb_tu_kolbait(6,"Разом"),"Разом");
for(int nom=0; nom < kol_podr; nom++)
 {
  fprintf(fil.ff,"%10.2f|%10.2f|",podr_hz_v.suma_kol(hz.kolih(),kol_podr,nom),podr_hz_sum.suma_kol(hz.kolih(),kol_podr,nom));
 }
fprintf(fil.ff,"%10.2f|%10.2f|\n",podr_hz_v.suma(),podr_hz_sum.suma());
fprintf(fil.ff,"%s\n",liniq.ravno());

podpis(fil.ff);
fil.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),3);
return(0);
}
