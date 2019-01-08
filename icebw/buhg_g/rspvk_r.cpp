/*$Id: rspvk_r.c,v 1.31 2014/04/30 06:14:58 sasa Exp $*/
/*23.05.2016	28.03.2004	Белых А.И.	rspvk_r.c
Расчёт сальдо по контрагентам
*/
#define	   KOLST  77 /*Количество строк на листе*/
#include <errno.h>
#include <math.h>
#include "buhg_g.h"
#include "rspvk.h"

extern int kol_strok_na_liste_l_u;
class rspvk_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  class iceb_u_spisok imaf;
  class iceb_u_spisok naim;

  rspvk_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;


 };

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data);
gint rspvk_r1(class rspvk_r_data *data);
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data);
int xbudkzw(int dk,const char *kodkontr,const char *shet,short dz,short mz,short gz,double sum_z,class iceb_u_str *dataz,GtkWidget *wpredok);

extern SQL_baza bd;
extern short	startgodb; /*Стартовый год*/

void rspvk_r(class rspvk_rr *data_rr)
{
int gor=0;
int ver=0;
class rspvk_r_data data;

data.rek_r=data_rr;
data.name_window.plus(__FUNCTION__);

char strsql[512];
class iceb_u_str soob;



if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rspvk_r_key_press),&data);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s %s",gettext("Расчёт сальдо по всем контрагентам"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчёт за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rspvk_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rspvk_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data)
{
// printf("rspvk_r_key_press\n");

switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/**********/
/*Шапка 1*/
/*********/
void sal1(int kli,short metka_r,FILE *ff,FILE *ff1)
{

fprintf(ff,"%90s%s N%d\n","",gettext("Лист"),kli);
if(metka_r == 0)
 {
  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"%50s%s N%d\n","",gettext("Лист"),kli);
  fprintf(ff1,"\
-----------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |\n"));

  fprintf(ff1,"\
-----------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"%50s%s N%d\n","",gettext("Лист"),kli);
  fprintf(ff1,"\
----------------------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------\n");
 }
}
/*************************/
/*Счётчик строк и листов*/
/*************************/
void sstrsl(int *kst, //Количество строк
int *kli, //Количество листов
short metka_r,
FILE *ff,FILE *ff1)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  fprintf(ff1,"\f");
  *kli=*kli+1;
  sal1(*kli,metka_r,ff,ff1);
  *kst=5;
 }  

}
/**********/
/*Шапка 2*/
/*********/
void sal2(int kli,short metka_dk,short metka_r,FILE *ff1)
{

fprintf(ff1,"%35s%s N%d\n","",gettext("Лист"),kli);
if(metka_r == 0) /*без даты*/
 {
  fprintf(ff1,"\
-----------------------------------------------------\n");

  if(metka_dk == 2)
    fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |   Сальдо  |\n\
       |                         |      | дебетовое |\n"));
  else
    fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |   Сальдо  |\n\
       |                         |      | кредитовое|\n"));


  fprintf(ff1,"\
-----------------------------------------------------\n");
 }
else
 {
  fprintf(ff1,"\
----------------------------------------------------------------\n");

  if(metka_dk == 2)
    fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |   Сальдо  |   Дата   |\n\
       |                         |      | дебетовое |  сальдо  |\n"));
  else
    fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |   Сальдо  |   Дата   |\n\
       |                         |      | кредитовое|  сальдо  |\n"));


  fprintf(ff1,"\
----------------------------------------------------------------\n");
 }
}

/*************************/
/*Счётчик строк и листов*/
/*************************/
void sstrsl1(int *kst, //Количество строк
int *kli, //Количество листов
int metka_dk,
int metka_r,
FILE *ff)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  *kli+=1;
  sal2(*kli,metka_dk,metka_r,ff);
  *kst=5;
 }  

}
/*********************/
/*запись контрагента во временную таблицу*/
/*******************************************/
int salallk_zvt(const char *kod_kontr,int *nom_zap,const char *imatab,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod_kontr from %s where kod_kontr='%s'",imatab,kod_kontr);
if(iceb_sql_readkey(strsql,wpredok) == 1)
 return(0);

sprintf(strsql,"insert into %s values('%s',%d)",imatab,kod_kontr,*nom_zap);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

*nom_zap+=1;

return(0);
}


/******************************************/
/******************************************/

gint rspvk_r1(class rspvk_r_data *data)
{
char strsql[512];
iceb_u_str soob;
iceb_clock sss(data->window);
SQL_str row1;
class SQLCURSOR cur1;

time_t vremn=time(NULL);
class iceb_tmptab tabtmp;
const char *imatmptab={"saldoall"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(32) not null DEFAULT '',\
nomz INT NOT NULL DEFAULT 0,\
index(nomz),\
unique(kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


sprintf(strsql,"select ns from Plansh where saldo=3 order by ns asc");

SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("В плане счетов нет счетов с развёрнутым сальдо !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok sheta_srs;

int pozz=0;
SQL_str row;
class iceb_u_str sh("");
while(cur.read_cursor(&row) != 0)
 {
  sh.new_plus(row[0]);
  if( pozz < kolstr-1)
   {
    cur.poz_cursor(pozz+1);
    cur.read_cursor(&row);

    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz);
      cur.read_cursor(&row);
     }
   }
  pozz++;
  
  sheta_srs.plus(row[0]);
  
 }
sprintf(strsql,"%s: %d\n",gettext("Количество счетов с развёрнутым сальдо"),
sheta_srs.kolih());
soob.new_plus(strsql);

iceb_printw(soob.ravno(),data->buffer,data->view);


  /*Определяем все организации и открываем массивы дебетов и
  кредитов организаций
  */
int nomer_zapisi=0;
/*Смотрим по каким контрагентам введено сальдо*/
sprintf(strsql,"select kodkon from Saldo where kkk=1 and gs=%d",data->godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[0],0,0) != 0)
    continue;
  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row[1],0,0) != 0)
      continue;
   }
//  data->spkontr.plus(row[0]);
  salallk_zvt(row[0],&nomer_zapisi,imatmptab,data->window);
 }

/*Смотрим по каким контрагентам были сделаны проводки*/
sprintf(strsql,"select distinct kodkon from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d' and kodkon <> ''",data->godn,data->gk,data->mk,data->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[0],0,0) != 0)
    continue;
  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row[1],0,0) != 0)
      continue;
   }
//  if(data->spkontr.find_r(row[0]) < 0)
//    data->spkontr.plus(row[0]);
  salallk_zvt(row[0],&nomer_zapisi,imatmptab,data->window); /*Запись в таблицу*/
 }

sprintf(strsql,"select * from %s",imatmptab);
int koo=iceb_sql_readkey(strsql,data->window);

sprintf(strsql,"%s: %d\n",gettext("Количество контрагентов"),koo);
soob.new_plus(strsql);

iceb_printw(soob.ravno(),data->buffer,data->view);

if(koo == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одного контрагента !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_double mdo;
mdo.make_class(koo*sheta_srs.kolih()*4);

//  printw(gettext("Вычисляем стартовое сальдо по всем контрагентам.\n"));
sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk='%d' and gs=%d",
1,data->godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int i,i1;
int kls=sheta_srs.kolih(); 
int rzm;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if((i=sheta_srs.find(row[0])) < 0)
   continue;
   
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
    continue;

  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
       continue;
   }
/*************   
  if((i1=data->spkontr.find_r(row[1])) >= 0)
   {
    rzm=(i1*kls*4)+(i*4);
    mdo.plus(atof(row[2]),rzm);
    mdo.plus(atof(row[3]),rzm+1);
   }
****************/
  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    i1=atoi(row1[0]);
    rzm=(i1*kls*4)+(i*4);
    mdo.plus(atof(row[2]),rzm);
    mdo.plus(atof(row[3]),rzm+1);
   }
  else
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено код контрагента в массиве кодов!"));
    repl.ps_plus(row[1]);
    iceb_menu_soob(&repl,data->window);
   }
  i1=atoi(row1[0]);

 }

//  printw(gettext("Просматриваем проводки.\n"));

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,sh asc",data->godn,1,1,data->gk,data->mk,data->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 } 

short d,m,g;
kls=sheta_srs.kolih();
float kolstr1=0.;  
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if((i=sheta_srs.find(row[1])) < 0)
   continue;

  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[3],0,0) != 0)
    continue;

  if(data->rek_r->kodgr.getdlinna() > 1)
   {
    SQLCURSOR cur1;
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
       continue;
   }

/********************
  if((i1=data->spkontr.find_r(row[3])) < 0)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не найдено код контрагента в массиве кодов!"));
    repl.ps_plus(row[3]);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],
    row[5]);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);

    continue;
   }   
************************/
  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 0)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не найдено код контрагента в массиве кодов!"));
    repl.ps_plus(row[3]);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],
    row[5]);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);

    continue;
   }
  i1=atoi(row1[0]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  rzm=(i1*kls*4)+(i*4);

   /*Период*/

  if(iceb_u_sravmydat(d,m,g,data->dn,data->mn,data->gn) >= 0)
  if(iceb_u_sravmydat(d,m,g,data->dk,data->mk,data->gk) <= 0)
    {
   /*     printw("Период\n");*/
     mdo.plus(atof(row[4]),rzm+2);
     mdo.plus(atof(row[5]),rzm+3);
    }

   /*До периода*/
  if(iceb_u_sravmydat(d,m,g,data->dn,data->mn,data->gn) < 0)
    {
     /*     printw("До периода\n");*/
     mdo.plus(atof(row[4]),rzm);
     mdo.plus(atof(row[5]),rzm+1);
    }

 }


FILE *ff,*ff1,*ffdeb,*ffkre;
char imaf[64];

sprintf(imaf,"sl%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Cальдо по всем контрагентам"));
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"slk%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Cальдо по всем контрагентам конечное"));
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"sldeb%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Контрагенты дебиторы"));
if((ffdeb = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"slkre%d.lst",getpid());
data->imaf.plus(imaf);
data->naim.plus(gettext("Контрагенты кредиторы"));
if((ffkre = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff1,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffdeb,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffkre,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

iceb_u_zagolov(gettext("Сальдо по всем контрагентам"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_get_pnk("00",0,data->window),ff);
iceb_u_zagolov(gettext("Сальдо по всем контрагентам"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_get_pnk("00",0,data->window),ff1);
iceb_u_zagolov(gettext("Контрагенты дебиторы"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_get_pnk("00",0,data->window),ffdeb);
iceb_u_zagolov(gettext("Контрагенты кредиторы"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_get_pnk("00",0,data->window),ffkre);

int kst,kstdeb,kstkre;
int kli,klideb,klikre;

kst=kstdeb=kstkre=5;
kli=klideb=klikre=1;
if(data->rek_r->shet.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rek_r->shet.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Счёт"),data->rek_r->shet.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Счёт"),data->rek_r->shet.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Счёт"),data->rek_r->shet.ravno());
 }
if(data->rek_r->kodgr.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Группа"),data->rek_r->kodgr.ravno());
 }


if(data->rek_r->kontr.getdlinna() > 1)
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ff1,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ffdeb,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
  fprintf(ffkre,"%s:%s\n",gettext("Контрагент"),data->rek_r->kontr.ravno());
 }

sal1(kli,data->rek_r->metka_r,ff,ff1);
sal2(klideb,2,data->rek_r->metka_r,ffdeb);
sal2(klikre,1,data->rek_r->metka_r,ffkre);

kst+=5;
kstdeb+=5;
kstkre+=5; //Увеличиваем на шапку

double mas2[6];
double mas1[6];

for(int i=0; i< 6 ;i++)
  mas2[i]=0.;

int i2;
int  mro,mrodeb,mrokre;
double  itdeb,itkre,db,kr;
class iceb_u_str kor("");
kls=sheta_srs.kolih();
//int koo=data->spkontr.kolih();

double deb_end=0.,kre_end=0.;
class iceb_u_str data_vs("");
class iceb_u_str bros("");              
kolstr1=0.;
for(int i1=0;i1<koo;i1++)
 {
  iceb_pbar(data->bar,koo,++kolstr1);    

  for(i2=0; i2< 6 ;i2++)
    mas1[i2]=0.;
  mro=mrodeb=mrokre=0;

  itdeb=itkre=db=kr=0;
//  kor.new_plus(data->spkontr.ravno(i1));
  sprintf(strsql,"select kod_kontr from %s where nomz=%d",imatmptab,i1);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
    continue;
  kor.new_plus(row1[0]);

  for(int i=0; i < kls; i++)
   {

    sh.new_plus(sheta_srs.ravno(i));
    
    if(iceb_u_proverka(data->rek_r->shet.ravno(),sh.ravno(),1,0) != 0)
     continue;
     
    rzm=(i1*kls*4)+(i*4);

    if(fabs(mdo.ravno(rzm)) < 0.01 && 
       fabs(mdo.ravno(rzm+1)) < 0.01 &&
       fabs(mdo.ravno(rzm+2)) < 0.01 &&
       fabs(mdo.ravno(rzm+3)) < 0.01)
        continue;
    if(fabs(mdo.ravno(rzm)-mdo.ravno(rzm+1)) < 0.01 && 
       fabs(mdo.ravno(rzm+2)) < 0.01 &&
       fabs(mdo.ravno(rzm+3)) < 0.01)
        continue;
    

     mas1[2]+=mdo.ravno(rzm+2);
     mas1[3]+=mdo.ravno(rzm+3);

    if(mro == 0)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kor.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        iceb_u_str repl;
        repl.plus(gettext("Не найдено код контрагента в общем списке !"));
        repl.ps_plus(kor.ravno());
        iceb_menu_soob(&repl,data->window);
        continue;
       }
/*********
      sprintf(strsql,"%*s %s\n",iceb_u_kolbait(6,kor.ravno()),kor.ravno(),row[0]);
      iceb_printw(strsql,data->buffer,data->view);
***********/
      bros.new_plus(row[0]);
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff,"%*s %-*.*s %-*s %11.2f %11s %11.2f %11.2f",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        
        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff,"%*s %-*.*s %-*s %11s %11.2f %11.2f %11.2f",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        " ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
       } 
     }
    else
     {
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff,"%7s %-25.25s %-*s %11.2f %11s %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff,"%7s %-25.25s %-*s %11s %11.2f %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        " ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));
        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
       }
     }    

    if(mdo.ravno(rzm)+mdo.ravno(rzm+2) > mdo.ravno(rzm+1)+mdo.ravno(rzm+3))
     {
      deb_end=(mdo.ravno(rzm)+mdo.ravno(rzm+2))-(mdo.ravno(rzm+1)+mdo.ravno(rzm+3));
      if(data->rek_r->metka_r == 1)
        xbudkzw(2,kor.ravno(),sh.ravno(),data->dk,data->mk,data->gk,deb_end,&data_vs,data->window);
      if(mro == 0)
       {
        fprintf(ff1,"%*s %-*.*s %-*s %11.2f %11s %s\n",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        deb_end,
        "",
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1,"%7s %-25.25s %-*s %11.2f %11s %s\n",
        " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        deb_end,
        "",
        data_vs.ravno());

       }

      if(fabs((mdo.ravno(rzm)+mdo.ravno(rzm+2))-(mdo.ravno(rzm+1)+mdo.ravno(rzm+3))) > 0.009)
       {
        deb_end=(mdo.ravno(rzm)+mdo.ravno(rzm+2))-(mdo.ravno(rzm+1)+mdo.ravno(rzm+3));
        itdeb+=deb_end;
        if(mrodeb == 0)
         {
          fprintf(ffdeb,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          deb_end,
          data_vs.ravno());
         }
        else
         {
          fprintf(ffdeb,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          deb_end,
          data_vs.ravno());
         }
        mrodeb++;
        sstrsl1(&kstdeb,&klideb,2,data->rek_r->metka_r,ffdeb);
       }        
      fprintf(ff," %11.2f %11s %s\n",deb_end,"",data_vs.ravno());
      mas1[4]+=deb_end;
     }
    else
     {
      kre_end=(mdo.ravno(rzm+1)+mdo.ravno(rzm+3))-(mdo.ravno(rzm)+mdo.ravno(rzm+2));
      if(data->rek_r->metka_r == 1)
        xbudkzw(1,kor.ravno(),sh.ravno(),data->dk,data->mk,data->gk,kre_end,&data_vs,data->window);
      if(mro == 0)
       {
        fprintf(ff1,"%*s %-*.*s %-*s %11.2f %11s %s\n",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        kre_end,
        "",
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1,"%7s %-25.25s %-*s %11.2f %11s %s\n",
        " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        kre_end,
        "",
        data_vs.ravno());

       }

      if(fabs((mdo.ravno(rzm+1)+mdo.ravno(rzm+3))-(mdo.ravno(rzm)+mdo.ravno(rzm+2))) > 0.009)
       {
        itkre+=kre_end;

        if(mrokre == 0)
         {
          fprintf(ffkre,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          kre_end,
          data_vs.ravno());
         }
        else
         {
          fprintf(ffkre,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          kre_end,
          data_vs.ravno());

         }
        mrokre++;

        sstrsl1(&kstkre,&klikre,1,data->rek_r->metka_r,ffkre);
       }      
      fprintf(ff," %11s %11.2f %s\n"," ",kre_end,data_vs.ravno());
      mas1[5]+=kre_end;
     }
    sstrsl(&kst,&kli,data->rek_r->metka_r,ff,ff1);
    mro++;
   }

  if(mro > 1)
   {
    sprintf(strsql,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus(strsql);
    if(mas1[0] > mas1[1])
     {
      fprintf(ff,"%*s %11.2f %11s %11.2f %11.2f",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno(),mas1[0]-mas1[1]," ",mas1[2],mas1[3]);
     }
    else
     {
      fprintf(ff,"%*s %11s %11.2f %11.2f %11.2f",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno()," ",mas1[1]-mas1[0],mas1[2],mas1[3]);
     }

    if(mas1[0]+mas1[2] > mas1[1]+mas1[3])
     {
      fprintf(ff1,"%*s %11.2f\n",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno(),
      (mas1[0]+mas1[2])-(mas1[1]+mas1[3]));
      fprintf(ff," %11.2f\n",(mas1[0]+mas1[2])-(mas1[1]+mas1[3]));

     }
    else
     {
      fprintf(ff1,"%*s %11s %11.2f\n",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno()," ",
      (mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
      fprintf(ff," %11s %11.2f\n"," ",(mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
     }
    sstrsl(&kst,&kli,data->rek_r->metka_r,ff,ff1);

   }

  if(mrodeb > 1)
   {
    sprintf(strsql,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus(strsql);
    fprintf(ffdeb,"%*s %11.2f\n",
    iceb_u_kolbait(40,bros.ravno()),bros.ravno(),itdeb);
    sstrsl1(&kstdeb,&klideb,2,data->rek_r->metka_r,ffdeb);
   }
  if(mrokre > 1)
   {
    sprintf(strsql,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus(strsql);
    fprintf(ffkre,"%*s %11.2f\n",
    iceb_u_kolbait(40,bros.ravno()),bros.ravno(),itkre);
    sstrsl1(&kstkre,&klikre,1,data->rek_r->metka_r,ffkre);
   }
  mas2[0]+=mas1[0];
  mas2[1]+=mas1[1];
  mas2[2]+=mas1[2];
  mas2[3]+=mas1[3];
  mas2[4]+=mas1[4];
  mas2[5]+=mas1[5];

 }

if(data->rek_r->metka_r == 0) /*без даты*/
 {
  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
----------------------------------------------------------------\n");
  fprintf(ffdeb,"\
-----------------------------------------------------\n");
  fprintf(ffkre,"\
-----------------------------------------------------\n");
 }
else
 {
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
---------------------------------------------------------------------------\n");
  fprintf(ffdeb,"\
----------------------------------------------------------------\n");
  fprintf(ffkre,"\
----------------------------------------------------------------\n");
 }

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL);

sprintf(strsql,"\n%20s",iceb_u_prnbr(mas2[0]));
soob.new_plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[1]));
soob.plus(strsql);
sprintf(strsql,"%20s",iceb_u_prnbr(mas2[2]));
soob.plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[3]));
soob.plus(strsql);
sprintf(strsql,"%20s",iceb_u_prnbr(mas2[4]));
soob.plus(strsql);
sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[5]));
soob.plus(strsql);

iceb_printw(soob.ravno(),data->buffer,data->view,"naim_shrift");

iceb_printw("\n\n",data->buffer,data->view);

fprintf(ff,"%*s %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),mas2[0],mas2[1],mas2[2],mas2[3],mas2[4],mas2[5]);

fprintf(ff1,"%*s %11.2f %11.2f\n",
iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),mas2[4],mas2[5]);

fprintf(ffdeb,"%*s %11.2f\n",
iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),mas2[4]);

fprintf(ffkre,"%*s %11.2f\n",
iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),mas2[5]);

if(mas2[0] > mas2[1])
    fprintf(ff,"%*s %11.2f %11s %11.2f %11.2f",
    iceb_u_kolbait(40,gettext("Итого (свернуто)")),gettext("Итого (свернуто)"),mas2[0]-mas2[1]," ",mas2[2],mas2[3]);
  else
    fprintf(ff,"%*s %11s %11.2f %11.2f %11.2f",
    iceb_u_kolbait(40,gettext("Итого (свернуто)")),gettext("Итого (свернуто):")," ",mas2[1]-mas2[0],mas2[2],mas2[3]);
if(mas2[0]+mas2[2] > mas2[1]+mas2[3])
 {
  fprintf(ff1,"%*s %11s %11.2f\n",
  iceb_u_kolbait(40,gettext("Итого (свернуто)")),gettext("Итого (свернуто)"),"",
  (mas2[0]+mas2[2])-(mas2[1]+mas2[3]));
  fprintf(ff," %11.2f\n",(mas2[0]+mas2[2])-(mas2[1]+mas2[3]));

 }
else
 {
  fprintf(ff1,"%*s %11.2f\n",
  iceb_u_kolbait(40,gettext("Итого (свернуто)")),gettext("Итого (свернуто)"),
  (mas2[1]+mas2[3])-(mas2[0]+mas2[2]));

  fprintf(ff," %11s %11.2f\n"," ",(mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
 }
iceb_podpis(ff,data->window);

iceb_podpis(ff1,data->window);

iceb_podpis(ffdeb,data->window);

iceb_podpis(ffkre,data->window);


fclose(ff);
fclose(ff1);
fclose(ffdeb);
fclose(ffkre);


for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),1,data->window);



//gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));
iceb_label_set_text_color(data->label,gettext("Расчёт закончен"),"red"); /*красый цвет*/

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//Доступна

gtk_widget_grab_focus(data->knopka);
iceb_printw_vr(vremn,data->buffer,data->view);

iceb_printw(" \n",data->buffer,data->view);

return(FALSE);

}
