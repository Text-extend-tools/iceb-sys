/*$Id: iceb_t_rabfil.c,v 5.47 2014/04/30 06:12:32 sasa Exp $*/
/*16.04.2013    21.11.1995      Белых А.И.      iceb_t_rabfil.c
Работа с файлами отчётов
*/
#include <errno.h>
#include <sys/stat.h>
#include	"buhl.h"

class sysnastr
 {
  public:
  class iceb_tu_str printer;    /*Системный принтер*/
  class iceb_tu_str printerloc; /*Приэкранный принтер*/
  class iceb_tu_str redaktor;   /*Редактор*/
  class iceb_tu_str smotrfil;   /*Просмотрщик*/
  class iceb_tu_str nameprinter; /*Имя принтера с которым будет работать программа lpr*/
  class iceb_tu_str showop;     /*Просмотр очереди на печать*/
  class iceb_tu_str deletop;    /*Удаление с очереди на печать*/
  class iceb_tu_str kl2s;    //Ключ для задания двухсторонней печати
  class iceb_tu_str print2s; //Список принтеров поддерживающих двухсторонюю печать
  class iceb_tu_str ppsf; //Просмотрщик PostScript файлов
  class iceb_tu_str filtr_iceb_ps; //Фильтр преобразования 
  class iceb_tu_str kluh_s_fil; /*Ключ -L/ect/iceB/logotip.ps  для печати логотипа с полным путём на файл с логотипом*/

  sysnastr()
   {
    clear_data();
   }

  void clear_data()
   {
    printer.new_plus("");
    printerloc.new_plus("");
    redaktor.new_plus("");
    smotrfil.new_plus("");
    nameprinter.new_plus("");
    showop.new_plus("");
    deletop.new_plus("");
    kl2s.new_plus("");
    print2s.new_plus("");
    ppsf.new_plus("");
    filtr_iceb_ps.new_plus("");
    kluh_s_fil.new_plus("");
   }
  
 };
 
int iceb_t_mstr_rnast(class sysnastr *nst);
void iceb_t_rabfil_f5(class sysnastr *nst,const char *imaf,const char *klush_peh);
void iceb_t_rabfil_f4(class sysnastr *nastr,const char *imaf);
void iceb_t_rabfil_f6(class sysnastr *nst,const char *imaf);
int iceb_t_progforlst(const char *imaf);

extern class iceb_tu_str nameprinter;

void  iceb_t_rabfil(class spis_oth *oth,const char *klush_peh) //Дополнительные ключи для печати
{
char razmerfil[128];

struct stat bufstat;
char            bros[1024];
class sysnastr nst;
int kod_klav=0;
int kom=0;
int nomer_str=0;
int metka_fil=0; //0-комментарий-имя файла 1-имя файла-комментарий
int metka_pn=0; /*Метка просмотра настроек*/
char		*mc5;
char kluh_s_fil[1024];
int metka_sohran[oth->spis_imaf.kolih()]; /*0- не сохранять 1-сохранять Метка сохранения файла при выходе*/
int kodirovka[oth->spis_imaf.kolih()]; /*0-в родной кодировке 1-в cp1251*/

memset(kluh_s_fil,'\0',sizeof(kluh_s_fil));
memset(metka_sohran,'\0',sizeof(metka_sohran));
memset(kodirovka,'\0',sizeof(kodirovka));


for(int nomer=0; nomer < oth->spis_ms.kolih(); nomer++)
 {
  int kkk=0;
  if((kkk=oth->spis_imaf.find(oth->spis_ms.ravno(nomer))) >= 0)
   metka_sohran[kkk]=1;
 }

for(int nomer=0; nomer < oth->spis_mk.kolih(); nomer++)
 {
  int kkk=0;
  if((kkk=oth->spis_imaf.find(oth->spis_mk.ravno(nomer))) >= 0)
   kodirovka[kkk]=1;
 }

mc5=tigetstr("mc5"); /*переменная описана в terminfo терминала и сообщает включён принтер или нет*/
/*********
int metka_pp=0; //Метка возможности предпечатного просмотра
if(nst.filtr_iceb_ps.getdlinna() > 1 && nst.ppsf.getdlinna() > 1)
 metka_pp=1;
rkka=0;
***********/

class VVOD VVOD1(2);
iceb_t_mstr_rnast(&nst);

if(strstrm(klush_peh,"-L") == 1)
 strcpy(kluh_s_fil,nst.kluh_s_fil.ravno());

VVOD DANET(1);

naz:;

clear();

if(nst.nameprinter.getdlinna() > 1)
 strcpy(bros,nst.nameprinter.ravno());
else
 strcpy(bros,gettext("По умолчанию"));   

int nomerstr_menu=-1;
move(++nomerstr_menu,0);
printw("%s",gettext("Список распечаток"));

if(metka_pn == 0)
 {
  printw("  %s:%s",gettext("Принтер"),bros);

 }

move(++nomerstr_menu,0);
printw("%s:%s",gettext("Текущий каталог"),iceb_tu_getcurrentdir());

if(metka_pn == 1)
 {
  move(++nomerstr_menu,0);
  printw("%s %s / %s",gettext("Системный принтер :"),nst.printer.ravno(),bros);
  move(++nomerstr_menu,0);
  printw("%s %s",gettext("Редактор файлов   :"),nst.redaktor.ravno());
  move(++nomerstr_menu,0);
  printw("%s %s",gettext("Просмотрщик файлов:"),nst.smotrfil.ravno());
  move(++nomerstr_menu,0);
  printw("%s %s",gettext("Удаление с очереди на печать:"),nst.deletop.ravno());
  move(++nomerstr_menu,0);
  printw("%s %s",gettext("Просмотр очереди на печать:"),nst.showop.ravno());

  if(nst.ppsf.getdlinna() > 1)
   mvprintw(++nomerstr_menu,0,"%s:%s",gettext("Просмотр печати"),nst.ppsf.ravno());

  if(nst.kluh_s_fil.getdlinna() > 1)
   mvprintw(++nomerstr_menu,0,"%s:%s",gettext("Ключ для печати с логотипом"),nst.kluh_s_fil.ravno());

  if(nst.filtr_iceb_ps.getdlinna() > 1)
   mvprintw(++nomerstr_menu,0,"%s:%s",gettext("Фильтр"),nst.filtr_iceb_ps.ravno());
  if(klush_peh[0] != '\0')
   {
    move(++nomerstr_menu,0);
    printw("%s:%s",gettext("Дополнительные ключи для печати"),klush_peh);
   } 

  if(mc5 != NULL && mc5 != (char*)-1)
   {
    move(++nomerstr_menu,0);
    printw("%s %s",gettext("Приэкранный принтер :"),nst.printerloc.ravno());
   }
 } 

class iceb_t_mstr spisok_menu(0);

int max_dlina_stroki=0;
int max_dlina_imafil=0;
for(int ii=0; ii < oth->spis_imaf.kolih(); ii++)
 {
  if(metka_fil == 0)
   {
    if(iceb_tu_strlen(oth->spis_naim.ravno(ii)) > max_dlina_stroki)
     max_dlina_stroki=iceb_tu_strlen(oth->spis_naim.ravno(ii));
    if(iceb_tu_strlen(oth->spis_imaf.ravno(ii)) > max_dlina_imafil)
     max_dlina_imafil=iceb_tu_strlen(oth->spis_imaf.ravno(ii));
   }
  else
   {
    if(iceb_tu_strlen(oth->spis_imaf.ravno(ii)) > max_dlina_stroki)
     max_dlina_stroki=iceb_tu_strlen(oth->spis_imaf.ravno(ii));
    if(iceb_tu_strlen(oth->spis_naim.ravno(ii)) > max_dlina_stroki)
     max_dlina_imafil=iceb_tu_strlen(oth->spis_naim.ravno(ii));
   }
 }

for(int ii=0; ii < oth->spis_imaf.kolih(); ii++)
 {
  char metka_s[8];
  memset(metka_s,'\0',sizeof(metka_s));
  if(metka_sohran[ii] == 1)
   strcat(metka_s,"*");

  if(kodirovka[ii] == 1)
   strcat(metka_s,"W");
  if(metka_s[0] == '\0')
   metka_s[0]=' ';
      
  memset(razmerfil,'\0',sizeof(razmerfil));
  if(stat(oth->spis_imaf.ravno(ii),&bufstat) == 0)
   {
    if(bufstat.st_size > 1000000)
     {
      double razm=bufstat.st_size/1000000.;
      razm=okrug(razm,0.01);
      sprintf(razmerfil,"%.2fM",razm);
     }
    else
     sprintf(razmerfil,"%ld",bufstat.st_size);
   }    
  if(metka_fil == 0)
   sprintf(bros,"%s|%-*s|%-*s|%s",
   metka_s,
   iceb_tu_kolbait(max_dlina_stroki,oth->spis_naim.ravno(ii)),
   oth->spis_naim.ravno(ii),
   iceb_tu_kolbait(max_dlina_imafil,oth->spis_imaf.ravno(ii)),
   oth->spis_imaf.ravno(ii),
   razmerfil);
  else
   sprintf(bros,"%s|%-*s|%-*s|%s",
   metka_s,
   iceb_tu_kolbait(max_dlina_stroki,oth->spis_imaf.ravno(ii)),
   oth->spis_imaf.ravno(ii),
   iceb_tu_kolbait(max_dlina_imafil,oth->spis_naim.ravno(ii)),
   oth->spis_naim.ravno(ii),
   razmerfil);

  spisok_menu.spis_str.plus(bros);
 }

char strih[112];
helstr(LINES-1,0,\
"F1",gettext("помощь"),
"F2",gettext("просмотр"),
"F3",gettext("редакт-ние"),
"F4",gettext("копировать"),
"F5",gettext("печать"),
"F6",gettext("очередь"),
"F7",gettext("настройка"),
"F8",gettext("перекодировать"),
"F9",gettext("сохранить"),
"F10",gettext("выход"),NULL);

kom=spisok_menu.menu(nomer_str,nomerstr_menu+1,0,NULL,strih,0,&kod_klav,"",0);

if(kom == -7) //Изменение размеров экрана
  goto naz;

if(kom >= 0)
 nomer_str=kom;
switch (kod_klav)
 {
  case FK1: /*просмотр подсказки*/
    GDITE();
    iceb_t_pdoc("iceb_t_rabfil.txt");
    goto naz;

  case FK10: /*Выход с удалением файлов с отчётами*/
  case ESC : /*Выход с удалением файлов с отчётами*/
    for(int ii=0; ii < oth->spis_imaf.kolih(); ii++)
     if(metka_sohran[ii] == 0)
       unlink(oth->spis_imaf.ravno(ii));
    return;

  case ENTER:
  case FK2: /*просмотр файла*/
    prosf(oth->spis_imaf.ravno(kom));
    goto naz;

  case SFK2: /*просмотр файла*/
    GDITE();
    pros_fil(oth->spis_imaf.ravno(kom),kluh_s_fil);
    goto naz;

  case FK3: /*редактирование*/
    GDITE();
    vizred(oth->spis_imaf.ravno(kom),nst.redaktor.ravno());
    goto naz;

  case SFK3: /*Ввод нового редактора*/
     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя программы редактирования"));

     vvod1(&nst.redaktor,40,&VVOD1,NULL,stdscr,-1,-1);
    
    goto naz;

  case FK4: /*копировать*/
    iceb_t_rabfil_f4(&nst,oth->spis_imaf.ravno(kom));
    goto naz;

  case FK5: /*печать*/
    iceb_t_rabfil_f5(&nst,oth->spis_imaf.ravno(kom),klush_peh);
    goto naz;

  case FK6: /*работа с очередью на печать*/
    iceb_t_rabfil_f6(&nst,oth->spis_imaf.ravno(kom));
    goto naz;


  case FK7: /*Просмотр настроек*/
    metka_pn++;
    if(metka_pn > 1)
     metka_pn=0;
    goto naz;

  case FK8: /*Перекодировать в cp1251 и обратно в родную при повторном нажатии*/
    DANET.VVOD_delete();
    if(kodirovka[kom] == 0)
     {
      DANET.VVOD_SPISOK_add_MD(gettext("Перекодировать в WINDOWS-1251? Вы уверены?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz;      
      if(perecod(2,oth->spis_imaf.ravno(kom)) != 0)
       goto naz;
      kodirovka[kom]=1;
      goto naz;
     }

    if(kodirovka[kom] == 1)
     {
      DANET.VVOD_SPISOK_add_MD(gettext("Вернуть предыдущюю кодировку?"));

      if(danet(&DANET,2,stdscr) == 2)
        goto naz;      
      if(perecod(3,oth->spis_imaf.ravno(kom)) != 0)
       goto naz;
      kodirovka[kom]=0;
     }
    goto naz;


  case FK9: /*Выход с сохранением файлов с отчётами*/
    metka_sohran[kom]++;
    if(metka_sohran[kom] > 1)
     metka_sohran[kom]=0;
    nomer_str++;
    goto naz;    

  case TAB: /*Перемена местами имени файла и комментария*/
    metka_fil++;
    if(metka_fil > 1)
     metka_fil=0;
    goto naz;
    
  default:
    goto naz;
    
 }
 

}


/**************************/
/*чтение нужных системных настроек*/
/**********************************/
int iceb_t_mstr_rnast(class sysnastr *nst)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char st1[2048];

sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  POLE(row_alx[0],st1,1,'|');

  if(SRAV(st1,"Системный принтер",0) == 0)
   {
     polen(row_alx[0],&nst->printer,2,'|');
     continue;
   }

  if(SRAV(st1,"Приэкранный принтер",0) == 0)
   {
     polen(row_alx[0],&nst->printerloc,2,'|');
     continue;
   }
  if(SRAV(st1,"Редактор",0) == 0 )
   {
    polen(row_alx[0],&nst->redaktor,2,'|');
    continue;
   
   }

  if(SRAV(st1,"Снять с очереди на печать",0) == 0)
   {
    polen(row_alx[0],&nst->deletop,2,'|');
    continue;
   }
  if(SRAV(st1,"Просмотр очереди на печать",0) == 0)
   {
    polen(row_alx[0],&nst->showop,2,'|');
    continue;
   }

  if(SRAV(st1,"Просмотрщик файлов",0) == 0)
   {
    polen(row_alx[0],&nst->smotrfil,2,'|');
    continue;
   }

  if(SRAV(st1,"Принтеры поддерживающие двухсторонюю печать",0) == 0)
   {
    polen(row_alx[0],&nst->print2s,2,'|');
    continue;
   }
  if(SRAV(st1,"Ключ для задания двухсторонней печати",0) == 0)
   {
    polen(row_alx[0],&nst->kl2s,2,'|');
    continue;
   }

  if(SRAV(st1,"Просмотрщик PostScript файлов",0) == 0)
   {
    polen(row_alx[0],&nst->ppsf,2,'|');
    continue;
   }
  if(SRAV(st1,"Фильт Epson->PostScript",0) == 0)
   {
    polen(row_alx[0],&nst->filtr_iceb_ps,2,'|');
    continue;
   }

  if(SRAV(st1,"Путь на файл с логотипом",0) == 0)
   {
    polen(row_alx[0],&nst->kluh_s_fil,2,'|');
    continue;
   }

   
 }

nst->nameprinter.new_plus(nameprinter.ravno());

return(0);
}

/*******************************/
/*печать*/
/*******************************/
void iceb_t_rabfil_f5(class sysnastr *nst,const char *imaf,const char *klush_peh)
{
int metka2s=0;  
int kom1=0;
char bros[1024];
char *mc5=tigetstr("mc5");
//char *tipterm=getenv("TERM");

VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать на принтере"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//1
if(nst->kluh_s_fil.getdlinna() > 1)
 MENU.VVOD_SPISOK_add_MD(gettext("Распечатать на принтере с логотипом"));//2

//if(strncmp(tipterm,"linux",5) == 0)
 if(mc5 != NULL && mc5 != (char*)-1)
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать на приэкранном принтере"));//2/3

//MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//0

naz:;

while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

char kl_peh[1024];
memset(kl_peh,'\0',sizeof(kl_peh));
strncpy(kl_peh,klush_peh,sizeof(kl_peh));

switch (kom1)
 {
//  case  4 :
  case -1 :
    return;

  case  0 :

c0:;
    metka2s=0;  
    if(proverka(nst->print2s.ravno(),nst->nameprinter.ravno(),0,1) == 0)
     {
      VVOD MENU1(3);

      MENU1.VVOD_SPISOK_add_MD(gettext("Односторонняя печать"));
      MENU1.VVOD_SPISOK_add_MD(gettext("Двухсторонняя печать"));

      MENU1.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));    
      metka2s=0;
      if(menu3w(stdscr,&MENU1,&metka2s,-1,-1,0) != 0)
       goto naz;
      if(metka2s == -1)
       return;
     }



    clear();
    move(LINES-5,0);
    GDITE();

    sprintf(bros,"%s%s%s",g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);
    iceb_t_cp(imaf,bros,0);    

    if(metka2s == 0)
      sprintf(bros,"%s %s %s%s%s",nst->printer.ravno(),kl_peh,g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);
    if(metka2s == 1)
      sprintf(bros,"%s %s %s %s%s%s",nst->printer.ravno(),kl_peh,nst->kl2s.ravno(),g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);

    if(iceb_t_system(bros,0) != 0)
     OSTANOV();
    return;

  case  1 :
    if(mprintcap() == 0)
     {
   
      nst->nameprinter.new_plus(nameprinter.ravno());
      clearstr(0,0);
      move(0,0);
      printw("%s %s / %s",gettext("Системный принтер :"),nst->printer.ravno(),nst->nameprinter.ravno());
     }
    goto naz;
    return;

  case  2 :
    GDITE();
    if(nst->kluh_s_fil.getdlinna() > 1)
     {
      sprintf(kl_peh,"-o logo=%s",nst->kluh_s_fil.ravno());
      goto c0;
     }
    else
     {    
c2:;

      sprintf(bros,"%s%s%s",g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);
      iceb_t_cp(imaf,bros,0);    

      sprintf(bros,"%s %s%s%s",nst->printerloc.ravno(),g_get_tmp_dir(),G_DIR_SEPARATOR_S,imaf);
      runsystem1(bros,1);
     }
    return;    

  case  3 :
    goto c2;
    
 }


}
/************************/
/*копирование  файлов*/
/**********************/
void iceb_t_rabfil_copfil(const char *imaf)
{
int metka_imeni=0;
int metka_kodir=0;
class VVOD MENU(3);
static class iceb_tu_str katalog("");
class iceb_tu_str imaf_new(imaf);

clearstr(LINES-1,0);

MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Копировать файл со старым именем"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Копировать файл с новым именем"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


while(menu3w(stdscr,&MENU,&metka_imeni,-1,-1,0) != 0);

switch (metka_imeni)
 {
  case  2 :
  case -1 :
    return;
  
  case  0 :
    break;

  case  1 :
    break;

    
 }

if(metka_imeni == 1)
 {
  VVOD  VVOD1(2);

  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

  if(vvod1(&imaf_new,64,&VVOD1,NULL,stdscr,-1,-1) == FK10)
   return;

  if(SRAV(imaf_new.ravno(),imaf,0) == 0)
   {
    iceb_t_soob(gettext("Имя файла не изменено!"));
    return;
   }

 }



MENU.VVOD_delete_MD();

MENU.VVOD_SPISOK_add_MD(gettext("Копировать файл как есть"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Копировать файл с перекодировкой в WINDOWS-1251"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


while(menu3w(stdscr,&MENU,&metka_kodir,-1,-1,0) != 0);

switch (metka_kodir)
 {
  case  2 :
  case -1 :
    return;
  
  case  0 :
    break;

  case  1 :
    break;
    
 }

class VVOD VVOD1(2);

helstr(LINES-1,0,
"Enter",gettext("ввод"),
"F10",gettext("выход"),NULL);

//VVOD1.VVOD_delete();
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите полный путь на каталог"));
naz:;
if(vvod1(&katalog,112,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;
if(katalog.getdlinna() <= 1)
 {
   class SPISOK puti;
   int nomer_str=0;
   if((nomer_str=menufil("putiforcp.alx",-1,-1,0,&puti,gettext("Выберите нужный каталог"))) < 0)
    return;
   polen(puti.ravno(nomer_str),&katalog,2,'|');
   goto naz;
 }

class iceb_tu_str komanda("");
char imaf_tmp[512];
strncpy(imaf_tmp,imaf,sizeof(imaf_tmp));

if(metka_kodir == 1)
 {
  sprintf(imaf_tmp,"perecod%d.tmp",getpid());

  /*Делаем копию файла, которую перекодируем*/
  iceb_t_cp(imaf,imaf_tmp,0);

  perecod(2,imaf_tmp);
 }

komanda.new_plus(katalog.ravno());
komanda.plus(G_DIR_SEPARATOR_S,imaf_new.ravno());
iceb_t_cp(imaf_tmp,komanda.ravno(),0);

if(metka_kodir == 1)
  unlink(imaf_tmp);

}

/************************/
/*копирование  файлов*/
/**********************/
void iceb_t_rabfil_f4(class sysnastr *nastr,const char *imaf)
{
int kom1=0;
class VVOD MENU(3);
class VVOD VVOD1(2);
static class iceb_tu_str katalog("");
clearstr(LINES-1,0);

MENU.VVOD_SPISOK_add_MD(gettext("Переслать файл на Windows машину"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Записать в нужный каталог"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выполнить программу"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Настроить список каталогов для копирования файлов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4


while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

switch (kom1)
 {
  case  4 :
  case -1 :
    return;
  
    
  case  0 :
    iceb_t_spis_komp(imaf);
    break;

  case  1:
     iceb_t_rabfil_copfil(imaf);
    break;

  case  2:
    iceb_t_progforlst(imaf);
    break;    

  case 3:
    iceb_redfil("putiforcp.alx",0);
    break;
 }
}
/************************/
/*работа с очередьб на печать*/
/**********************/
void iceb_t_rabfil_f6(class sysnastr *nst,const char *imaf)
{
int kom1=0;
int kom2=0;
class iceb_tu_str bros("");
char bros1[1024];
int kodoh=0;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_MD(gettext("Просмотр очереди на печать"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Снятие с очереди на печать"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

naz:;

while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

switch (kom1)
 {
  case  3:
  case -1 :
    return;
  
  case  0 :
    clear();
    refresh();
//    runsystem1(nst->showop.ravno(),0);
    iceb_t_system(nst->showop.ravno(),0);
    OSTANOV();
    break;
  
  case  1 :
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

    bros.new_plus("");

    VVOD  VVOD1(2);
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер очереди на печать"));
    
    if((kom2=vvod1(&bros,20,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom2 == ESC)
      goto naz;
   
     if(bros.getdlinna() <= 1)
      goto naz;

     sprintf(bros1,"%s %s",nst->deletop.ravno(),bros.ravno());
  //   printw("\n%s\n",bros1);
  //   OSTANOV();
     GDITE();
//     runsystem1(bros1,1);
    if((kodoh=iceb_t_system(bros1,1)) != 0)
     {
      printw("\nОшибка %d\n",kodoh);
      OSTANOV();
     }
    break;
    
 }
}

