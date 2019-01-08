/*$Id: smenabaz.c,v 5.44 2014/08/31 06:18:26 sasa Exp $*/
/*04.09.2018	29.01.2000	Белых А.И.	smenabaz.c
Смена базы данных
Возвращаем 0 если базу сменили успешно
	  -1 если базу не меняли
*/
#include       <errno.h>
#include        <pwd.h>
#include        "buhl.h"

void smenabaz_dumpbazout(const char *namebases);

extern char *host;
extern char *parol_pol;
extern char *imabaz;

int smenabaz(int mopen) //0-открыть и закрыть 1-закрыть и открыть
{
SQL_baza bd_open_close;
int kod_kl=0;
char host_v[1024];
char bros[1024];
class iceb_tu_str imabaz_v("");
memset(host_v,'\0',sizeof(host_v));

FILE *ff;
if((ff = fopen(".spisokbaz.alx","r")) == NULL)
 {
  if(errno != ENOENT)
   {
    error_op_nfil(".spisokbaz.alx",errno,"");
    return(1);
   }

  if(errno == ENOENT) /*Файл не существует*/
   {
    strncpy(host_v,host,sizeof(host_v)-1);

    if(smenabaz_mysql(&imabaz_v,0) != 0)
     {
      return(1);
     }
   }
 }
else
 {
  char bros1[512];
  char str_menu[512];
  char stroka[1024];
  class SPISOK masiv;
  class SPISOK sp_host;
  while(fgets(stroka,sizeof(stroka),ff) != NULL)
   {
  //  printw("%s\n",stroka);
  //  refresh();
    if(stroka[0] == '#')
      continue;
    if(polen(stroka,bros,sizeof(bros),1,'|') != 0)
      continue;

    polen(stroka,bros1,sizeof(bros1),2,'|');
    sprintf(str_menu,"%-*s|%s",iceb_tu_kolbait(6,bros),bros,bros1);
    masiv.plus(str_menu);


    polen(stroka,bros1,sizeof(bros1),3,'|');
    sp_host.plus(bros1);    
   }

  fclose(ff);

naz:;

  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F7",gettext("выгрузка"),
  "F10",gettext("выход"),NULL);
  
  int voz=dirmasiv(&masiv, -1,-1, 0,gettext("Выберите нужную базу данных"),1,&kod_kl);

//  if(voz < 0 || kod_kl != ENTER)
  if(voz < 0 )
    return(-1);

  switch(kod_kl)
   {
    case ENTER:
     break;

    case FK10:
     return(-1);

    case FK7:
      polen(masiv.ravno(voz),&imabaz_v,1,'|');
      smenabaz_dumpbazout(imabaz_v.ravno());
      goto naz;

    default:
      goto naz;     
   };
   
  strncpy(bros1,masiv.ravno(voz),sizeof(bros1)-1);
  polen(bros1,&imabaz_v,1,'|');
  strncpy(host_v,sp_host.ravno(voz),sizeof(host_v)-1);
 }

 
struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());

class iceb_tu_str host_open;
if(host_v[0] == '\0')
  host_open.plus(host);
else
 {
  host_open.plus(host_v);
 }
//printw("\nhost_open=%s\n",host_open);
//OSTANOV();
if(mopen == 0)
 if(sql_openbaz(&bd_open_close,imabaz_v.ravno(),host_open.ravno(),ktor->pw_name,parol_pol) != 0)
  {
   eropbaz(imabaz_v.ravno(),ktor->pw_uid,ktor->pw_name,1);
   return(-1);
  }

if(mopen == 1)
 {
  sql_closebaz(&bd);

  if(sql_openbaz(&bd,imabaz_v.ravno(),host_open.ravno(),ktor->pw_name,parol_pol) != 0)
   {
    eropbaz(imabaz_v.ravno(),ktor->pw_uid,ktor->pw_name,1);

    if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol_pol) != 0)
     {
    /*  printw("%s\n",sql_error(&bd));*/
    /*  OSTANOV();*/
     }

    return(-1);
   }

 }


if(host_v[0] != '\0')
 {
  delete [] host;
  host=new char[strlen(host_v)+1];
  strcpy(host,host_v);
 }

delete [] imabaz;

imabaz=new char[strlen(imabaz_v.ravno())+1];
strcpy(imabaz,imabaz_v.ravno());

/*читаем после присвоения имени базы*/
icebstart_rf(); //чтение и выполнение запросов к базе сразу после открытия

/*Читаем наименование организации*/
char strsql[512];
SQL_str row;
SQLCURSOR cur;
sprintf(strsql,"select naikon from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_t_vkk00();
 }

if(mopen == 0)
 sql_closebaz(&bd_open_close);

return(0);
 

}
/*************************************/
/*просмотр списка баз в Mysql*/
/********************************/

int smenabaz_mysql(class iceb_tu_str *imabazv,
int metka) /* 0 - показать список если база только одна 1- не показывать а установить ее как выбаранную*/
{
char strsql[1024];
class SQLCURSOR cur,cur1;
SQL_str row,row1;
int kolstr;
class SPISOK masiv;
class iceb_tu_str naim_org("");
class iceb_tu_str imaendbaz("");

int mdib=0; /*Максимальная длинна имени базы*/
GDITE();

/*Смотрим имя последей базы с которой работал оператор*/
poldan("NAMEBAZ",&imaendbaz,".iceb.dat");

if((kolstr=cur.make_cursor(&bd,"SHOW DATABASES")) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной базы !"));
  return(1);
 }
int nom_baz=0;
int kol_db=0; /*Количество доступных баз*/
int kodv=0;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);

  if((kodv=sql_readkey(&bd,strsql,&row1,&cur1)) < 0)
    continue;
  if(mdib < (int)strlen(row[0]))
   mdib=strlen(row[0]);
 }

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  naim_org.new_plus("");

  if((kodv=sql_readkey(&bd,strsql,&row1,&cur1)) < 0)
    continue;
 
  if(kodv == 1)
   naim_org.new_plus(row1[0]);


  sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(mdib,row[0]),row[0],naim_org.ravno());
  masiv.plus(strsql);
  if(proverka(imaendbaz.ravno(),row[0],0,1) == 0)
   nom_baz=kol_db;
  kol_db++;
 }

if(kol_db == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной базы, к которой вам разрешён доступ!"));
  return(1);
 }

if(kol_db == 1 && metka == 1)
 {
  polen(masiv.ravno(0),imabazv,1,'|');
 
  return(0);
  
 }

naz:;

helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F7",gettext("выгрузка"),
"F10",gettext("выход"),NULL);
int kod_kl=0;

sprintf(strsql,"%s. %s:%s",gettext("Выберите нужную базу данных"),gettext("Хост"),host);


//int voz=dirmasiv(&masiv, -1,-1,nom_baz,gettext("Выберите нужную базу данных"),1,&kod_kl);
int voz=dirmasiv(&masiv, -1,-1,nom_baz,strsql,1,&kod_kl);

//if(voz < 0 || kod_kl != ENTER)
if(voz < 0 )
    return(-1);

switch(kod_kl)
 {
  case ENTER:
   break;

  case FK10:
   return(-1);

  case FK7:
    polen(masiv.ravno(voz),&naim_org,1,'|');
    smenabaz_dumpbazout(naim_org.ravno());
    goto naz;

  default:
    goto naz;     
 };
  
polen(masiv.ravno(voz),imabazv,1,'|');

return(0);

}
/***********************************************/
/*Выгрузка дампа базы данных*/
/*********************************/
void smenabaz_dumpbazout(const char *namebases)
{
int		kom;
char strsql[512];
VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(gettext("Выгрузка дампа баз данных"));

sprintf(strsql,"%s %s",gettext("Выгрузить дамп базы"),namebases);
MENU.VVOD_SPISOK_add_MD(gettext(strsql));//0
MENU.VVOD_SPISOK_add_MD(gettext("Выгрузить дамп всех баз данных"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

naz:;

clearstr(LINES-1,0);
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2 :
  case -1 :
    return;

  case 0 :
    delete [] imabaz;

    imabaz=new char[strlen(namebases)+1];
    strcpy(imabaz,namebases);

    if(iceb_t_prov_vdb(namebases) != 0)
     return;

    iceb_t_dumpbazout(namebases,iceb_tu_getlogin(),parol_pol,host);
    return;

  case 1:
    
    if(iceb_t_prov_vdb("") != 0)
     return;
    iceb_t_dumpbazout("",iceb_tu_getlogin(),parol_pol,host);
    return;

  default:
    kom=0;
    goto naz;
 }
}
