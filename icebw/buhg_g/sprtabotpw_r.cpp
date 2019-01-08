/*$Id: sprtabotpw_r.c,v 1.21 2013/09/26 09:46:55 sasa Exp $*/
/*06.06.2018	26.05.2016	Белых А.И.	sprtabotpw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "sprtabotpw.h"
#include "zarrsovw.h"
class sprtabotpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class sprtabotp_poi *rp;
  class spis_oth *oth;  
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  sprtabotpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   sprtabotpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_r_data *data);
gint sprtabotpw_r1(class sprtabotpw_r_data *data);
void  sprtabotpw_r_v_knopka(GtkWidget *widget,class sprtabotpw_r_data *data);


extern SQL_baza bd;
extern char	*shetb; /*Бюджетные счета начислений*/

int sprtabotpw_r(class sprtabotp_poi *rek,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class sprtabotpw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.oth=oth;
data.rp=rek;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт отпускных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sprtabotpw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Расчёт отпускных"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(sprtabotpw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)sprtabotpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sprtabotpw_r_v_knopka(GtkWidget *widget,class sprtabotpw_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sprtabotpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/**********************************/
/*Вычисление итога по вертикали*/
/********************************/

double 	itogvert(int kolkodnvr,int kolmes,int nomkod,double *sumanvr)
{

double itog=0.;
for(int nommes=0; nommes < kolmes; nommes++)
  itog+=sumanvr[nommes*kolkodnvr+nomkod];

return(itog);

}

/*************************************/
/*поиск в месяце предыдущем началу расчёта отпускных в счёт месяца начала расчёта*/
/*********************************/

double sprtabotp_po(class sprtabotp_poi *rp,const char *kod_otp,
int metka_mes,
int metka_b, //0-общий 1-бюджет 2-хозрасчёт
FILE *ff_prot,
GtkWidget *wpredok)
{
double suma_otp=0.;
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
short dp=1,mp=rp->mn,gp=rp->gn;
short den=1,mes=rp->mn,god=rp->gn;

iceb_u_dpm(&dp,&mp,&gp,4);
if(metka_mes == 1)
 iceb_u_dpm(&den,&mes,&god,3);

sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1' and suma <> 0.\
 and godn=%d and mesn=%d",
rp->tabnom.ravno_atoi(),gp,mp,1,gp,mp,31,god,mes);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(metka_b == 1)
   if(iceb_u_proverka(shetb,row[2],0,1) != 0)
    continue;
  if(metka_b == 2)
   if(iceb_u_proverka(shetb,row[2],0,1) == 0)
    continue;
  if(iceb_u_proverka(row[0],kod_otp,0,1) == 0)
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"%s-Сумма отпускных предыдущего месяца=%s\n",__FUNCTION__,row[1]);

    suma_otp+=atof(row[1]);
   }
 }

return(suma_otp);

}



/*************************/
/*определение коэффициента*/
/**************************/
double sprtabotp_koef(int tabnom,short mr,short gr,short mk,short gk,FILE *ff,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
//short d=0,m=0,g=0;
double oklad1=0.;
double oklad2=0.;

if(ff != NULL)
 {
  fprintf(ff,"\n%s-Расчёт коэффициента %d %02d.%04d\n",__FUNCTION__,tabnom,mr,gr);
  fprintf(ff,"%s-Ищем оклады меньше этой даты или равной\n",__FUNCTION__);
 }
 
sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gr,mr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s-Не нашли коэффициент 1\n",__FUNCTION__);
  return(1.);
 }

if(ff != NULL)
 fprintf(ff,"%s-%s %10s %s\n",__FUNCTION__,iceb_u_datzap_mg(row[0]),row[1],row[2]);
/****************

iceb_u_rsdat(&d,&m,&g,row[0],2);
if(mr == m && gr == g)
 {
  if(ff != NULL)
   fprintf(ff,"Дата расчёта совпадает с датой оклада. Коэффициент 1.\n");
  return(1.);
 } 
********************/ 
oklad1=atof(row[1]);
int metka=atoi(row[2]);
if(metka == 5 || metka == 6)
  oklad1=okladtrw(mr,gr,oklad1,wpredok);

if(ff != NULL)
 fprintf(ff,"%s-Ищем последнее повышение зарплаты\n",__FUNCTION__);

//sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt > '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gr,mr);
sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gk,mk);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s-Не нашли коэффициент 1\n",__FUNCTION__);
  return(1.);
 }
if(ff != NULL)
 fprintf(ff,"%s-%s %10s %s\n",__FUNCTION__,iceb_u_datzap_mg(row[0]),row[1],row[2]);

oklad2=atof(row[1]);
metka=atoi(row[2]);
if(metka == 5 || metka == 6)
  oklad2=okladtrw(mr,gr,oklad2,wpredok);


double koef=oklad2/oklad1;
koef=iceb_u_okrug(koef,0.000001);
if(ff != NULL)
 fprintf(ff,"%s-Расчитываем коэффициент %.2f/%.2f=%f\n",__FUNCTION__,oklad2,oklad1,koef);

return(koef);

}



/********************************/
/* Распечатка результата        */
/********************************/
void rasrez(class sprtabotp_poi *rp,
const char *kodnvr,
const char *fio,
const char *dolg,
const char *kodpodr,
const char *naipod,
double *sumanmes,
double *sumanvr,
int kolmes,int kolkodnvr,
class iceb_u_int *kolkd,
class iceb_u_int *kolkd_vdr,
class iceb_u_int *kol_pd,
class iceb_u_spisok *DATAI,
class iceb_u_double *KOF,
FILE *ff,
FILE *ff_prot,
int metka, //0-общий 1-бюджет 2-хозрасчёт
int kolkodnvrind, //Количество кодов не входящих в расчёт индексации
double *sumanvi, //Массив сумм не входящих в расчёт индексации
const char *kodnvrind, //Коды не входящие в расчёт индексации
const char *kodt_nvr, /*коды табеля не входящие в расчёт*/
GtkWidget *wpredok)
{
int nomkod=0;
int nommes=0;
char strsql[1024];
short d,m,g;
SQL_str row;
int kolkdi=0;
char bros[1024];
double kof=0.;
//int  nomer=0;
double  sumaindekc=0.;
SQLCURSOR curr;
//Вычисляем количество колонок
int kolkol=0;
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) != 0.)
   kolkol++;

for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) != 0.)
   kolkol++;

char stroka[51+(kolkol+1)*11+1];
memset(stroka,'\0',sizeof(stroka));
memset(stroka,'-',sizeof(stroka)-1);


class iceb_u_str kod_otp("");
iceb_poldan("Коды начислений отпускных",&kod_otp,"zarotp.alx",wpredok);

iceb_zagolov(gettext("Расчётный лист на оплату отпускных"),1,rp->mn,rp->gn,rp->dk,rp->mk,rp->gk,ff,wpredok);

fprintf(ff,"\n%s:%d\n",gettext("Табельный номер"),rp->tabnom.ravno_atoi());
fprintf(ff,"%s:%s\n",gettext("Фамилия Имя Отчество"),fio);
fprintf(ff,"%s:%s\n",gettext("Должность"),dolg);
fprintf(ff,"%s:%s %s\n",gettext("Подразделение"),kodpodr,naipod);
fprintf(ff,"%s:%s\n",gettext("Вид отпуска"),rp->vid_otp.ravno());
fprintf(ff,"%s:%s\n",gettext("Приказ"),rp->prikaz.ravno());
fprintf(ff,"%s:%s\n",gettext("Период з"),rp->period.ravno());
fprintf(ff,"%s:%s\n",gettext("Количество дней отпуска"),rp->kol_dnei.ravno());
fprintf(ff,"\n");

if(kodt_nvr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Коды табеля не входящие в расчёт"),kodt_nvr);
 
if(metka == 1)
  fprintf(ff,"%s.\n",gettext("Бюджет"));

if(metka == 2)
  fprintf(ff,"%s.\n",gettext("Хозрасчёт"));

int metka_pk=iceb_poldan_vkl("Печать наименований кодов не входящих в расчёт","zarotp.alx",wpredok);

if(metka_pk == 1)
 {
  fprintf(ff,"%s:%.*s\n",
  gettext("Коды, которые не вошли в расчёт"),
  iceb_u_kolbait(60,kodnvr),
  kodnvr);

  for(int nom=60; nom < iceb_u_strlen(kodnvr); nom+=90)
   fprintf(ff,"%.*s\n",
   iceb_u_kolbait(90,iceb_u_adrsimv(nom,kodnvr)),
   iceb_u_adrsimv(nom,kodnvr));
 }
else
 {
  if(kolkodnvr != 0)
   {
    class iceb_u_str kod("");
    class iceb_u_str naikod("");
    fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчёт"));
    short shethik=0;
    char str_prom[1024];
    char str2[1024];
    
    for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
     {
      if(iceb_u_polen(kodnvr,&kod,nomkod+1,',') != 0)
        kod.new_plus(kodnvr);
      sprintf(strsql,"select naik from Nash where kod=%s",kod.ravno());
      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       naikod.new_plus(row[0]);    
      else
       naikod.new_plus("");  
      if(shethik == 0)
       {
        memset(str_prom,'\0',sizeof(str_prom));
        sprintf(str_prom,"%2s %-*.*s",
        kod.ravno(),
        iceb_u_kolbait(45,naikod.ravno()),iceb_u_kolbait(45,naikod.ravno()),naikod.ravno());
       }

      if(shethik == 1)
       {
        sprintf(str2,"  %2s %-*.*s",
        kod.ravno(),
        iceb_u_kolbait(45,naikod.ravno()),iceb_u_kolbait(45,naikod.ravno()),naikod.ravno());
        strcat(str_prom,str2);
       }


      
      shethik++;
      if(shethik == 2)
       {
        fprintf(ff,"%s\n",str_prom);
        shethik=0;
       }
    
    
     }
    if(shethik != 0)
        fprintf(ff,"%s\n",str_prom);

   }
 }
 
if(kolkodnvrind != 0)
 {
  class iceb_u_str kod("");
  class iceb_u_str naikod("");
  fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчёт индексации"));
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
    if(iceb_u_polen(kodnvrind,&kod,nomkod+1,',') != 0)
      kod.new_plus(kodnvrind);

    sprintf(strsql,"select naik from Nash where kod=%s",kod.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     naikod.new_plus(row[0]);    
    else
     naikod.new_plus("");
    fprintf(ff,"%s %s\n",kod.ravno(),naikod.ravno());
   }

 }

fprintf(ff,"\n");

fprintf(ff,"%s\n",stroka);

fprintf(ff,gettext(" Дата  |К.д|П.д|В.р|Начислено |"));
//Печатаем коды не входящие в расчёт
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
    continue;
    
  if(iceb_u_polen(kodnvr,bros,sizeof(bros),nomkod+1,',') != 0)
    strcpy(bros,kodnvr);

  sprintf(strsql,"select naik from Nash where kod=%d",atoi(bros));
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   sprintf(strsql,"%s %s",bros,row[0]);
  else
   strcpy(strsql,bros);

  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(10,strsql),
  iceb_u_kolbait(10,strsql),
  strsql);
 }

fprintf(ff,"%*.*s|",
iceb_u_kolbait(10,gettext("В расчёт")),iceb_u_kolbait(10,gettext("В расчёт")),gettext("В расчёт"));

//Печатаем коды не входящие в расчёт индексации
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
    continue;
    
  if(iceb_u_polen(kodnvrind,bros,sizeof(bros),nomkod+1,',') != 0)
    strcpy(bros,kodnvrind);

  sprintf(strsql,"select naik from Nash where kod=%d",atoi(bros));
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   sprintf(strsql,"%s %s",bros,row[0]);
  else
   strcpy(strsql,bros);

  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(10,strsql),
  iceb_u_kolbait(10,strsql),
  strsql);


//  fprintf(ff,"%*s|",iceb_u_kolbait(10,strsql),strsql);
 }


fprintf(ff,"%-*.*s|%-*.*s|\n",
iceb_u_kolbait(8,gettext("Коэф.")),iceb_u_kolbait(8,gettext("Коэф.")),gettext("Коэф."),
iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);

d=1; m=rp->mn; g=rp->gn;
double itog=0.;
double itog1=0.;
double itogind=0.;
int kold_vdr=0;
int itogo_dni[3];
double itogo_v_rashet=0.;
memset(itogo_dni,'\0',sizeof(itogo_dni));

for(nommes=0; nommes < kolmes; nommes++)
 {
  fprintf(ff,"%02d.%04d|%3d|%3d|%3d|%10.2f",m,g,kolkd->ravno(nommes),kol_pd->ravno(nommes),kolkd_vdr->ravno(nommes)-kol_pd->ravno(nommes),sumanmes[nommes]);
  kolkdi+=kolkd->ravno(nommes);
  kold_vdr+=kolkd_vdr->ravno(nommes);
  itog1+=sumanmes[nommes];
  itog=0.;
  itogo_dni[0]+=kolkd->ravno(nommes);
  itogo_dni[1]+=kol_pd->ravno(nommes);
  itogo_dni[2]+=kolkd_vdr->ravno(nommes)-kol_pd->ravno(nommes);


  for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
   {
    if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
      continue;
    fprintf(ff," %10.2f",sumanvr[nommes*kolkodnvr+nomkod]);
    itog+=sumanvr[nommes*kolkodnvr+nomkod];
   }

  double otp=0.;

  if(nommes == 0 || nommes == 1)/*прибавляем сумму отпускных в счёт этого месяца*/
   {

    otp=sprtabotp_po(rp,kod_otp.ravno(),nommes,metka,ff_prot,wpredok);
    
   }

  double v_rashet=sumanmes[nommes]-itog+otp;
  itogo_v_rashet+=v_rashet;
  /*печатаем колонку В расчёт*/

  fprintf(ff," %10.2f",v_rashet);

  itogind=0.;
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
    if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
      continue;
    fprintf(ff," %10.2f",sumanvi[nommes*kolkodnvrind+nomkod]);
    itogind+=sumanvi[nommes*kolkodnvrind+nomkod];
   }

  if(rp->metka_ri == 1)
   {
    int nomer=0;
    sprintf(bros,"%d.%d",m,g);
    if((nomer=DATAI->find(bros)) >= 0)
     {
      kof=KOF->ravno(nomer);
      fprintf(ff," %6.6g %10.2f",kof,kof*(v_rashet-itogind));
     }  
    else
     {
      kof=1.;
      fprintf(ff," %6s %10.2f","",kof*(v_rashet-itogind));
     }
   }
  else
   {
    kof=sprtabotp_koef(rp->tabnom.ravno_atoi(),m,g,rp->mk,rp->gk,ff_prot,wpredok);
    fprintf(ff," %8.8g %10.2f",kof,kof*(v_rashet-itogind));
   }
   
  sumaindekc+=kof*(v_rashet-itogind);

  if(otp != 0.)
   fprintf(ff," %s %.2f",gettext("отпускные"),otp);
  fprintf(ff,"\n");
  
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s\n",stroka);

//Печатаем итоговую строку
fprintf(ff,"%*s:%3d %3d %3d %10.2f",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),itogo_dni[0],itogo_dni[1],itogo_dni[2],itog1);
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < kolmes; nommes++)
    itog+=sumanvr[nommes*kolkodnvr+nomkod];
  fprintf(ff," %10.2f",itog);
  itog1-=itog;
 }
//fprintf(ff," %10.2f",itog1);
itogind=0.;
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < kolmes; nommes++)
    itog+=sumanvi[nommes*kolkodnvrind+nomkod];
  fprintf(ff," %10.2f",itog);
  itogind+=itog;
 }
fprintf(ff," %10.2f",itogo_v_rashet);

fprintf(ff," %8s %10.2f"," ",sumaindekc);
fprintf(ff,"\n");

if(itogind != 0.)
 {
  fprintf(ff,"\n%.2f+%.2f=%.2f\n",itogind,sumaindekc,itogind+sumaindekc);
 }
double suma_otp=0.;
class iceb_u_str kol_d(""); 

int kol_pol=iceb_u_pole2(rp->kol_dnei.ravno(),',');

if(kol_pol > 0)
 {
  fprintf(ff,"\n");
  for(int nom=1;nom <= kol_pol ; nom++)
   {
    iceb_u_polen(rp->kol_dnei.ravno(),&kol_d,nom,',');
    if(kol_d.ravno_atoi() == 0)
     continue;
    suma_otp=sumaindekc/itogo_dni[2]*kol_d.ravno_atoi();
    fprintf(ff,"%.2f/%d*%d=%.2f\n",sumaindekc,itogo_dni[2],kol_d.ravno_atoi(),suma_otp);
    
   }
 }
else
 if(rp->kol_dnei.getdlinna() > 1)
  {
   suma_otp=sumaindekc/itogo_dni[2]*rp->kol_dnei.ravno_atoi();
   fprintf(ff,"\n%.2f/%d*%d=%.2f\n",sumaindekc,itogo_dni[2],rp->kol_dnei.ravno_atoi(),suma_otp);
  }
fprintf(ff,"\n-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");

}

/*********************************/
/*Чтение коєффициентов индексации*/
/*********************************/
void readki(iceb_u_spisok *DATAI,iceb_u_double *KOF,GtkWidget *wpredok)
{
class iceb_u_str bros("");
class iceb_u_str bros1("");
short m,g;
int   nomer;
double kof=0.;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarotp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarotp.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0)
    continue;
  if(iceb_u_polen(bros.ravno(),&bros1,1,'.') != 0)
    continue;
  m=atoi(bros.ravno());

  if(iceb_u_polen(bros.ravno(),&bros1,2,'.') != 0)
    continue;
  g=bros1.ravno_atoi();  

  if(iceb_u_polen(row_alx[0],&bros1,2,'|') != 0)
    continue;
  kof=bros1.ravno_atof();  

  sprintf(strsql,"%d.%d",m,g);
  if((nomer=DATAI->find(strsql)) < 0)
    DATAI->plus(strsql); 

  KOF->plus(kof,nomer);
//  printw("%s**%s\n",row_alx[0],bros);    

 }


}
/******************************************/
/*Распечатка начислений вошедших в расчёт*/
/******************************************/
void sprtabotp_rnvvr(int tabn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_int *skodnvr,
class iceb_u_double *sum_vr,FILE *ff,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naik;
char strsql[512];
double itogo_gor=0.;
double suma=0.;
int kolmes=iceb_u_period(1,mn,gn,dk,mk,gk,1);

short d=1,m=mn,g=gn;

iceb_zagolov(gettext("Начисления взятые в расчёт отпускных"),1,mn,gn,dk,mk,gk,ff,wpredok);

int kolih_kod=skodnvr->kolih();
class iceb_u_double itogo_kol;

itogo_kol.make_class(kolmes);
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
naik.new_plus("");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naik.new_plus(row[0]);

fprintf(ff,"%d %s\n",tabn,naik.ravno());


class iceb_u_str liniq;
liniq.plus("-------------------------------------");

for(int nom=0; nom <= kolmes; nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff," Код |   Наименование начисления    |");
/*          12345 123456789012345678901234567890*/

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 {
  fprintf(ff," %02d.%04d  |",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));
fprintf(ff,"%s\n",liniq.ravno());

for(int nom_kod=0; nom_kod < kolih_kod; nom_kod++)
 {
  naik.new_plus("");
  
  sprintf(strsql,"select naik from Nash where kod=%d",skodnvr->ravno(nom_kod));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naik.new_plus(row[0]);

  fprintf(ff,"%5d %-*.*s|",skodnvr->ravno(nom_kod),
  iceb_u_kolbait(30,naik.ravno()),
  iceb_u_kolbait(30,naik.ravno()),
  naik.ravno());
  itogo_gor=0.;
  for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
   {
    suma=sum_vr->ravno(nom_mes*kolih_kod+nom_kod);
    if(suma != 0.)
     {
      fprintf(ff,"%10.2f|",suma);
      itogo_gor+=suma;    
      itogo_kol.plus(suma,nom_mes);
     }
    else
      fprintf(ff,"%10s|","");
     
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
  iceb_u_dpm(&d,&m,&g,3);

 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(36,gettext("Итого")),
iceb_u_kolbait(36,gettext("Итого")),
gettext("Итого"));

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 fprintf(ff,"%10.2f|",itogo_kol.ravno(nom_mes));
fprintf(ff,"%10.2f|\n",itogo_kol.suma());


}

/******************************************/
/******************************************/

gint sprtabotpw_r1(class sprtabotpw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
short		d,m,g;

d=1;

if(iceb_rsdatp1(&data->rp->mn,&data->rp->gn,data->rp->datan.ravno(),&data->rp->mk,&data->rp->gk,data->rp->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%d.%d%s => %d.%d%s\n",
data->rp->mn,data->rp->gn,gettext("г."),
data->rp->mk,data->rp->gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);


int kolstr=0;
float kolstr1=0.;



SQL_str		row,row1;
char		imaf[64],imafbu[64],imafxr[64];
class SQLCURSOR cur,cur1;
short		dk=31;
class iceb_u_str kodnvr(""); //Коды не входящие в расчёт отпускных
class iceb_u_str kodnvrind(""); //Коды не входящие в расчёт индексации отпускных
int		kolkodnvr=0;
int		kolkodnvrind=0;
int		nommes=0;
int		nomkod=0;
double		suma=0.;
class iceb_u_str fio("");
class iceb_u_str kodpodr("");
class iceb_u_str naipod("");
class iceb_u_str dolg("");
short		dnr,mnr,gnr;
SQLCURSOR curr;




printf("%s-%s\n",__FUNCTION__,data->rp->tabnom.ravno());

sprintf(strsql,"select fio,dolg,podr,datn from Kartb where tabn=%d",data->rp->tabnom.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->rp->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fio.new_plus(row[0]);
dolg.new_plus(row[1]);
kodpodr.new_plus(row[2]);
iceb_u_rsdat(&dnr,&mnr,&gnr,row[3],2);

sprintf(strsql,"%d %s\n",data->rp->tabnom.ravno_atoi(),fio.ravno());
iceb_printw(strsql,data->buffer,data->view);

iceb_u_dpm(&data->rp->dk,&data->rp->mk,&data->rp->gk,5); //получить последний день месяца

class iceb_fopen fil_prot;
char imaf_prot[64];
sprintf(imaf_prot,"otpp%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Протокол хода расчёта отпускных"),1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,fil_prot.ff,data->window);

class iceb_u_str kodt_nvr("");

iceb_poldan("Коды не входящие в расчёт отпускных",&kodnvr,"zarotp.alx",data->window);
iceb_poldan("Коды не входящие в расчёт индексации отпускных",&kodnvrind,"zarotp.alx",data->window);
iceb_poldan("Коды табеля не входящие в расчёт",&kodt_nvr,"zarotp.alx",data->window);

kolkodnvr=iceb_u_pole2(kodnvr.ravno(),',');
if(kolkodnvr == 0 && kodnvr.ravno()[0] != '\0')
  kolkodnvr=1;

kolkodnvrind=iceb_u_pole2(kodnvrind.ravno(),',');
if(kolkodnvrind == 0 && kodnvrind.ravno()[0] != '\0')
  kolkodnvrind=1;
  
int kolmes=iceb_u_period(1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,1);

class iceb_u_spisok DATAI(0); //Список дат м.год индексации
class iceb_u_double KOF(0); //Коффициенты индексации по датам

//Читаем коэффициенты индексации
readki(&DATAI,&KOF,data->window);

double sumanmes[kolmes]; //Сумма начисленная
memset(&sumanmes,'\0',sizeof(sumanmes));
double sumanvr[kolmes*kolkodnvr]; //Суммы не входящие в расчёт
memset(&sumanvr,'\0',sizeof(sumanvr));

double sumanmesxr[kolmes]; //Сумма начисленная хозрасчёт
memset(&sumanmesxr,'\0',sizeof(sumanmesxr));
double sumanvrxr[kolmes*kolkodnvr]; //Суммы не входящие в расчёт хозрасчёт
memset(&sumanvrxr,'\0',sizeof(sumanvrxr));

double sumanmesbu[kolmes]; //Сумма начисленная бюджет
memset(&sumanmesbu,'\0',sizeof(sumanmesbu));
double sumanvrbu[kolmes*kolkodnvr]; //Суммы не входящие в расчёт бюджет
memset(&sumanvrbu,'\0',sizeof(sumanvrbu));


double sumanvi[kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных
memset(sumanvi,'\0',sizeof(sumanvi));

double sumanvixr[kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных хозрасчёта
memset(sumanvixr,'\0',sizeof(sumanvixr));

double sumanvibu[kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных бюджета
memset(sumanvibu,'\0',sizeof(sumanvibu));

class iceb_u_int  kolkd; //Количество календарных дней в месяце
kolkd.make_class(kolmes);

class iceb_u_int  kol_pd; //Количество праздничных дней в месяце
kol_pd.make_class(kolmes);

class iceb_u_int  kolkd_vdr; //Количество календарных дней взятых для расчёта
kolkd_vdr.make_class(kolmes);

class iceb_u_int skodnvr; /*список кодов начислений вошедшх в расчёт*/



sprintf(strsql,"select datz,knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1'",
data->rp->tabnom.ravno_atoi(),data->rp->gn,data->rp->mn,1,data->rp->gk,data->rp->mk,data->rp->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчёт его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    

  sumanmes[nommes]+=suma;

  if(iceb_u_proverka(shetb,row[3],0,1) == 0)
    sumanmesbu[nommes]+=suma;
  else 
    sumanmesxr[nommes]+=suma;

  if(iceb_u_proverka(kodnvr.ravno(),row[1],0,1) == 0)
   {
    iceb_u_pole1(kodnvr.ravno(),row[1],',',0,&nomkod);
    sumanvr[nommes*kolkodnvr+nomkod]+=suma;
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      sumanvrbu[nommes*kolkodnvr+nomkod]+=suma;
    else 
      sumanvrxr[nommes*kolkodnvr+nomkod]+=suma;

   }
  else
   {
    if(skodnvr.find(row[1]) < 0)
     skodnvr.plus(row[1]);
   }

  if(iceb_u_proverka(kodnvrind.ravno(),row[1],0,1) == 0)
   {
    iceb_u_pole1(kodnvrind.ravno(),row[1],',',0,&nomkod);
    sumanvi[nommes*kolkodnvrind+nomkod]+=suma;
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      sumanvibu[nommes*kolkodnvrind+nomkod]+=suma;
    else 
      sumanvixr[nommes*kolkodnvrind+nomkod]+=suma;

   }
 }

/*Создаём масив для сумм начислений вошедших в расчёт*/
class iceb_u_double sum_vr;
int kolih_kod=skodnvr.kolih();
sum_vr.make_class(kolih_kod*kolmes);
kolstr1=0;
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчёт его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    
  if((nomkod=skodnvr.find(row[1])) < 0)
   continue;
  sum_vr.plus(suma,nommes*kolih_kod+nomkod);
 }

class iceb_zarrsov svod_otvr; /*свод по отработанному времени*/
 
//Узнаем количество календарных дней
if(data->rp->gn != data->rp->gk)
  sprintf(strsql,"select kodt,god,mes,dnei,kdnei,has from Ztab where tabn=%d and \
((god > %d and god < %d) \
or (god=%d and mes >= %d) \
or (god=%d and mes <= %d)) order by god asc,mes asc",
  data->rp->tabnom.ravno_atoi(),data->rp->gn,data->rp->gk,data->rp->gn,data->rp->mn,data->rp->gk,data->rp->mk);
else
  sprintf(strsql,"select kodt,god,mes,dnei,kdnei,has from Ztab where tabn=%d and \
god=%d and mes >= %d and mes <= %d order by god asc,mes asc",
  data->rp->tabnom.ravno_atoi(),data->rp->gn,data->rp->mn,data->rp->mk);

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
kolstr1=0;
d=1;
int nomvs=0;
int mes_z=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  g=atoi(row[1]);
  m=atoi(row[2]);
  nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    
  kolkd.plus(atoi(row[4]),nommes);

  if(iceb_u_proverka(kodt_nvr.ravno(),row[0],0,1) != 0)
   kolkd_vdr.plus(atoi(row[4]),nommes);

  /*узнаём количество праздничных дней в месяце*/
  if(mes_z != m)
   {
    sprintf(strsql,"select kpd from Zarskrdh where data='%04d-%02d-01'",g,m);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     kol_pd.plus(atoi(row1[0]),nommes);
    mes_z=m;
   }

  if(svod_otvr.kod.find(row[0]) < 0)
     svod_otvr.kod.plus(row[0]);

  sprintf(strsql,"%s|%02d.%04d",row[0],m,g);

  if((nomvs=svod_otvr.kod_mes_god.find(strsql)) < 0)
   svod_otvr.kod_mes_god.plus(strsql);

  svod_otvr.kolrd.plus(row[3],nomvs);    
  svod_otvr.kolkd.plus(row[4],nomvs);    
  svod_otvr.kolhs.plus(atof(row[5]),nomvs);    

 }

//Читаем наименование подразделения
sprintf(strsql,"select naik from Podr where kod=%s",kodpodr.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 naipod.new_plus(row[0]);
 
//Распечатываем общий итог
class iceb_fopen fil;
sprintf(imaf,"otp%d.lst",getpid());
if(fil.start(imaf,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(fil.ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

rasrez(data->rp,kodnvr.ravno(),fio.ravno(),dolg.ravno(),kodpodr.ravno(),naipod.ravno(),sumanmes,
sumanvr,kolmes,kolkodnvr,&kolkd,&kolkd_vdr,&kol_pd,&DATAI,&KOF,fil.ff,fil_prot.ff,0,kolkodnvrind,sumanvi,kodnvrind.ravno(),kodt_nvr.ravno(),data->window);

iceb_podpis(fil.ff,data->window);
fil.end();

char imaf_sn[64];
sprintf(imaf_sn,"otpn%d.lst",getpid());
class iceb_fopen fil_rsvod;
if(fil_rsvod.start(imaf_sn,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
sprtabotp_rnvvr(data->rp->tabnom.ravno_atoi(),data->rp->mn,data->rp->gn,dk,data->rp->mk,data->rp->gk,&skodnvr,&sum_vr,fil_rsvod.ff,data->window);

zarrsovw(data->rp->mn,data->rp->gn,data->rp->mk,data->rp->gk,data->rp->tabnom.ravno_atoi(),&svod_otvr,fil_rsvod.ff,data->window);

iceb_podpis(fil_rsvod.ff,data->window);
fil_rsvod.end();

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт отпускных"));
data->oth->spis_imaf.plus(imaf_sn);
data->oth->spis_naim.plus(gettext("Свод начислений и отработанного времени"));

if(shetb != NULL)
 {
  sprintf(imafbu,"otpbu%d.lst",getpid());

  if(fil.start(imafbu,"w",data->window) != 0)
   {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  rasrez(data->rp,kodnvr.ravno(),fio.ravno(),dolg.ravno(),kodpodr.ravno(),naipod.ravno(),sumanmesbu,sumanvrbu,kolmes,kolkodnvr,&kolkd,&kolkd_vdr,&kol_pd,&DATAI,&KOF,fil.ff,fil_prot.ff,1,kolkodnvrind,sumanvibu,kodnvrind.ravno(),kodt_nvr.ravno(),data->window);

  iceb_podpis(fil.ff,data->window);
  fil.end();


  sprintf(imafxr,"otpxr%d.lst",getpid());
  if(fil.start(imafxr,"w",data->window) != 0)
   {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  rasrez(data->rp,kodnvr.ravno(),fio.ravno(),dolg.ravno(),kodpodr.ravno(),naipod.ravno(),sumanmesxr,sumanvrxr,kolmes,kolkodnvr,&kolkd,&kolkd_vdr,&kol_pd,&DATAI,&KOF,fil.ff,fil_prot.ff,2,kolkodnvrind,sumanvixr,kodnvrind.ravno(),kodt_nvr.ravno(),data->window);

  iceb_podpis(fil.ff,data->window);

  fil.end();

  
  data->oth->spis_imaf.plus(imafbu);
  data->oth->spis_naim.plus(gettext("Расчёт бюджетных отпускных"));
  
  data->oth->spis_imaf.plus(imafxr);
  data->oth->spis_naim.plus(gettext("Расчёт хозрасчётных отпускных"));
 }

data->oth->spis_imaf.plus(imaf_prot);
data->oth->spis_naim.plus(gettext("Протокол хода расчёта отпускных"));


for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
