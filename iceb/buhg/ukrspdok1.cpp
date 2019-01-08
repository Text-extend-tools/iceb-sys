/*$Id: ukrspdok1.c,v 5.11 2013/08/13 05:49:56 sasa Exp $*/
/*16.07.2015	04.12.2008	Белых А.И.	ukrspdok1.c
Распечатка списка документов по датам приказа
*/
#include        <errno.h>
#include        "buhg.h"



int ukrspdok1(class spis_oth *oth)
{
char strsql[512];
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kontr("");
static class iceb_tu_str vkom("");

VVOD VV(0);
VVOD MENU(3);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(vkom.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Вид кома-ировки.(,,).."));

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка документов по датам приказов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kontr.new_plus(VV.data_ravno(2));
vkom.new_plus(VV.data_ravno(3));

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("ukr4_2.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;

      case 1 :
        vibrek("Ukrvkr",&vkom);
        VV.VVOD_SPISOK_zapis_data(3,vkom.ravno());
        goto naz;


     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont,nompr,datp,vkom from Ukrdok where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,nompr asc",gn,mn,dn,gk,mk,dk);

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

FILE *ff;
char imaf[64];
sprintf(imaf,"ukrspdp%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Распечатка списка документов по датам приказов"),dn,mn,gn,dk,mk,gk,ff);

fprintf(ff,"\
-----------------------------------------------------------------\n");

fprintf(ff,"\
Дата прик.|Номер пр.|Дата докум|Номер док.|Контрагент\n");
/**********
1234567890 123456789 1234567890 1234567890
*****************/
fprintf(ff,"\
-----------------------------------------------------------------\n");

int kolstr1=0;
short dp,mp,gp;
short dd,md,gd;
class iceb_tu_str naim_kontr("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kontr.ravno(),row[2],0,0) != 0)
    continue;

  if(proverka(vkom.ravno(),row[3],0,0) != 0)
    continue;
  /*узнаём наименование контрагента*/
  naim_kontr.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kontr.new_plus(row1[0]);
   
  rsdat(&dp,&mp,&gp,row[4],2);
  rsdat(&dd,&md,&gd,row[1],2);
  
  fprintf(ff,"%02d.%02d.%04d %-9s %02d.%02d.%04d %-*s %s %s\n",
  dp,mp,gp,row[3],dd,md,gd,
  iceb_tu_kolbait(10,row[0]),row[0],
  row[2],
  naim_kontr.ravno());

 }


fprintf(ff,"\
-----------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка списка документов по датам приказов"));
iceb_t_ustpeh(imaf,3);
return(0);

}
