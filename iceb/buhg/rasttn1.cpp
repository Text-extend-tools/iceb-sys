/*$Id: rasttn1.c,v 5.1 2014/01/31 12:11:33 sasa Exp $*/
/*03.10.2013	28.09.2013	Белых А.И.	rasttn1.c
Распечатка товарно-транспортной накладной
*/
#include "buhg.h"

class rasttn_rek
 {
  public:
   class iceb_tu_str marka_avt;
   class iceb_tu_str gos_nomer;
   class iceb_tu_str nom_put_list;
   class iceb_tu_str fio_vod;
   class iceb_tu_str avtopredp;
   class iceb_tu_str punkt_zagr;
   class iceb_tu_str punkt_razg;
   class iceb_tu_str zakazchik;
   class iceb_tu_str gos_nomer_pric;
   rasttn_rek()
    {
     clear_rek();
    }
   void clear_rek()
    {
     marka_avt.new_plus("");
     gos_nomer.new_plus("");
     gos_nomer_pric.new_plus("");
     nom_put_list.new_plus("");
     fio_vod.new_plus("");
     avtopredp.new_plus("");
     punkt_zagr.new_plus("");
     punkt_razg.new_plus("");
     zakazchik.new_plus("");
    }
 };
 
int rasttn_vvod(short dd,short md,short gd,const char *nomdok,int sklad,class rasttn_rek *rek);

extern double okrcn;  /*Округление цены*/

int rasttn1(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth)
{
class rasttn_rek rek;
int kolstr=0;
SQL_str row,row_alx;
class SQLCURSOR cur,cur_alx;
char strsql[1024];
int metka_vs=0;
class VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Вторую страницу печатать?"));

metka_vs=danet(&DANET,2,stdscr);


/*Читаем шапку документа*/
sprintf(strsql,"select tip,kontr,pn from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,sklad,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  sklad);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
int tipz=atoi(row[0]);
class iceb_tu_str kod_kontr(row[1]);
double pnds=atof(row[2]);

class iceb_tu_str naim_kontr00("");
class iceb_tu_str naim_kontr("");
class iceb_tu_str adres00("");
class iceb_tu_str adres("");

int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=11",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);

class iceb_tu_str nomer_dover("");
class iceb_tu_str data_dover("");
class iceb_tu_str herez_kogo("");
short ddov=0,mdov=0,gdov=0;
class iceb_tu_str naim_mes_dov("");

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=1",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],&nomer_dover,1,'#');
  polen(row[0],&data_dover,2,'#');
  rsdat(&ddov,&mdov,&gdov,data_dover.ravno(),1);
  if(mdov != 0)
    mesc(mdov,1,&naim_mes_dov);
 }

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=2",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
 {
  herez_kogo.new_plus(row[0]);
 }

if(tipz == 1)
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'",kod_kontr.ravno_filtr());
else
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'","00");

if(readkey(strsql,&row,&cur) == 1)
 {
  naim_kontr00.new_plus(row[0]);
  adres00.new_plus(row[1]);
 }

if(tipz == 2)
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'",kod_kontr.ravno_filtr());
else
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'","00");

if(readkey(strsql,&row,&cur) == 1)
 {
  naim_kontr.new_plus(row[0]);
  adres.new_plus(row[1]);
 }

rek.avtopredp.new_plus(naim_kontr00.ravno());
rek.punkt_zagr.new_plus(adres00.ravno());
rek.punkt_razg.new_plus(adres.ravno());
rek.zakazchik.new_plus(naim_kontr.ravno());

if(rasttn_vvod(dd,md,gd,nomdok,sklad,&rek) != 0 )
 return(1);

/*Определяем сумму по документу*/
sprintf(strsql,"select kolih,cena from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' order by kodm asc",gd,md,dd,sklad,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

double suma=0.;
double suma_itogo=0.;
double kolih=0.;
double cena=0.;

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[0]);
  cena=atof(row[1]);
//  Для вложенного налога
  if(lnds == 0)
    suma=cena+cena*pnds/100.;
  else
    suma=cena;

  suma=okrug(suma,okrcn);
  suma=suma*kolih;
  suma_itogo+=okrug(suma,okrcn);

  

 }
class iceb_tu_str suma_prop("");
preobr(suma_itogo,&suma_prop,0);

sprintf(strsql,"select str from Alx where fil='ttnl1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ttnl1.alx");
  iceb_t_soob(strsql);
  return(1);
 }
char imaf[64];
sprintf(imaf,"rttn%d.lst",getpid());

class iceb_fopen rs;
if(rs.start(imaf,"w") != 0)
 return(1);

fprintf(rs.ff,"\x1B\x33%c",30); /*Уменьшение межстрочного интервала*/

int nomer_str=0;
class iceb_tu_str stroka("");
class iceb_tu_str naim_mes;
mesc(md,1,&naim_mes);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;

  nomer_str++;

  stroka.new_plus(row_alx[0]);
  
  switch(nomer_str)
   {
    case 6: /*Номер накладной*/
     iceb_tu_vstav(&stroka,nomdok,103,150,1);
     break;

    case 8: /*Дата автомобиль*/
     iceb_tu_vstav(&stroka,dd,2,6,1);
     iceb_tu_vstav(&stroka,naim_mes.ravno(),7,20,1);
     iceb_tu_vstav(&stroka,gd,17,30,1);

     sprintf(strsql,"%s %s",rek.marka_avt.ravno(),rek.gos_nomer.ravno());
     iceb_tu_vstav(&stroka,strsql,36,135,1);
     iceb_tu_vstav(&stroka,rek.nom_put_list.ravno(),160,173,1);
     break;
    
    case 10: /*автопредприятие фамилия водителя*/
     iceb_tu_vstav(&stroka,rek.avtopredp.ravno(),17,68,1);
     iceb_tu_vstav(&stroka,rek.fio_vod.ravno(),74,110,1);
     break;

    case 12: /*Замовник*/
     iceb_tu_vstav(&stroka,rek.zakazchik.ravno(),19,143,1);
     break;

    case 14: /*вантажовідправник*/
     iceb_tu_vstav(&stroka,naim_kontr00.ravno(),18,143,1);
     break;

    case 16: /*вантажоодержувач*/
     iceb_tu_vstav(&stroka,naim_kontr00.ravno(),17,143,1);
     break;

    case 18: /*пункт навантаження розвантаження*/
     iceb_tu_vstav(&stroka,rek.punkt_zagr.ravno(),19,76,1);
     iceb_tu_vstav(&stroka,rek.punkt_razg.ravno(),96,148,1);
     break;

    case 20: /*номер прицепа*/
     iceb_tu_vstav(&stroka,rek.gos_nomer_pric.ravno(),110,152,1);
     break;

    case 31: /*запись в документу*/
     sprintf(strsql,"%s N%s %s %02d.%02d.%04d %s",
     gettext("Накладная"),nomdok,gettext("от"),dd,md,gd,gettext("г."));
     
     iceb_tu_vstav(&stroka,strsql,20,160,1);
     break;

    case 40: /*сумма прописью*/
     iceb_tu_vstav(&stroka,suma_prop.ravno(),25,107,1);
     break;

    case 42: /*номер доверенности*/
     iceb_tu_vstav(&stroka,nomer_dover.ravno(),142,173,1);
     break;

    case 44: /*дата доверенности*/
     if(ddov != 0)
      {
       iceb_tu_vstav(&stroka,ddov,132,147,1);
       sprintf(strsql,"%s %d",naim_mes_dov.ravno(),gdov);
       iceb_tu_vstav(&stroka,strsql,137,170,1);
      }
     break;

    case 46: /*кому выдана доверенность*/
     iceb_tu_vstav(&stroka,herez_kogo.ravno(),134,173,1);
     break;

   };

  fprintf(rs.ff,"%s",stroka.ravno());
 }

if(metka_vs == 1)
 {
  fprintf(rs.ff,"\f");
  iceb_t_insfil("ttnl2.alx",rs.ff);
 }

podpis(rs.ff);

rs.end();

iceb_t_ustpeh(imaf,2);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Товарно-транспортная накладная")); 

return(0);

}
/********************************************/
/*ввод реквизитов*/
/***************************************/
int rasttn_vvod(short dd,short md,short gd,const char *nomdok,int sklad,class rasttn_rek *rek)
{
int kom=0;
char strsql[2048];
class VVOD VV(0);
SQL_str row;
class SQLCURSOR cur;


sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=10",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->gos_nomer.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=15",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->nom_put_list.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=16",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->marka_avt.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=17",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->gos_nomer_pric.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=18",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->avtopredp.new_plus(row[0]);


sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=19",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->zakazchik.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=20",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->fio_vod.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=21",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->punkt_zagr.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=22",gd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 1)
  rek->punkt_razg.new_plus(row[0]);

VV.VVOD_SPISOK_add_data(rek->avtopredp.ravno(),80);//0
VV.VVOD_SPISOK_add_data(rek->fio_vod.ravno(),80);//1
VV.VVOD_SPISOK_add_data(rek->nom_put_list.ravno(),80);//2
VV.VVOD_SPISOK_add_data(rek->marka_avt.ravno(),80);//3
VV.VVOD_SPISOK_add_data(rek->gos_nomer.ravno(),80);//4
VV.VVOD_SPISOK_add_data(rek->punkt_zagr.ravno(),80);//5
VV.VVOD_SPISOK_add_data(rek->punkt_razg.ravno(),80);//6
VV.VVOD_SPISOK_add_data(rek->zakazchik.ravno(),80);//7
VV.VVOD_SPISOK_add_data(rek->gos_nomer_pric.ravno(),80);//8


VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка товарно-транспортной накладной"));
sprintf(strsql,"%s N%s %s %02d.%02d.%d",gettext("К накладной"),nomdok,gettext("от"),dd,md,gd);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Автопредприятие........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Фамилия водителя......."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер путевого листа..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Марка автомобиля......."));//3
VV.VVOD_SPISOK_add_MD(gettext("Гос. номер автомобиля.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Пунк загрузки.........."));//5
VV.VVOD_SPISOK_add_MD(gettext("Пунк разгрузки........."));//6
VV.VVOD_SPISOK_add_MD(gettext("Заказчик (плательщик).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Гос. номер прицепа....."));//8


naz:;

clear();
 
helstr(LINES-1,0,"F2/+",gettext("ввод"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rek->avtopredp.new_plus(VV.VVOD_SPISOK_return_data(0));
rek->fio_vod.new_plus(VV.VVOD_SPISOK_return_data(1));
rek->nom_put_list.new_plus(VV.VVOD_SPISOK_return_data(2));
rek->marka_avt.new_plus(VV.VVOD_SPISOK_return_data(3));
rek->gos_nomer.new_plus(VV.VVOD_SPISOK_return_data(4));
rek->punkt_zagr.new_plus(VV.VVOD_SPISOK_return_data(5));
rek->punkt_razg.new_plus(VV.VVOD_SPISOK_return_data(6));
rek->zakazchik.new_plus(VV.VVOD_SPISOK_return_data(7));
rek->gos_nomer_pric.new_plus(VV.VVOD_SPISOK_return_data(8));

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



sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,10,rek->gos_nomer.ravno_filtr()); 
if(sql_zapis(strsql,1,0) != 0)
 goto naz;

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,15,rek->nom_put_list.ravno_filtr()); 
sql_zapis(strsql,1,0);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,16,rek->marka_avt.ravno_filtr()); 
sql_zapis(strsql,1,0);

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,17,rek->gos_nomer_pric.ravno_filtr()); 
sql_zapis(strsql,1,0);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,18,rek->avtopredp.ravno_filtr()); 
sql_zapis(strsql,1,0);

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,19,rek->zakazchik.ravno_filtr()); 
sql_zapis(strsql,1,0);

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,20,rek->fio_vod.ravno_filtr()); 
sql_zapis(strsql,1,0);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,21,rek->punkt_zagr.ravno_filtr()); 
sql_zapis(strsql,1,0);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,22,rek->punkt_razg.ravno_filtr()); 
sql_zapis(strsql,1,0);





return(0);

}

