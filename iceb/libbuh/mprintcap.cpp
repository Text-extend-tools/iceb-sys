/*$Id: mprintcap.c,v 5.11 2014/06/30 06:36:40 sasa Exp $*/
/*10.11.2018	02.04.2001	Белых А.И.	mprintcap.c
Просмотр файла настройки принтeров /etc/printcap и создание
меню выбора принтера
Если имя принтера начинается с точки то он в список принтеров не 
попадает
Если вернули 0 - принтер сменили
             1 - нет
*/
#include        <errno.h>
#include	"buhl.h"

extern class iceb_tu_str nameprinter;

int	mprintcap()
{
class iceb_tu_str imafil("/etc/printcap");
FILE		*ff;
char		stroka[1024];
SPISOK	masiv;
class iceb_tu_str bros("");
class iceb_tu_str sprinter("");
class iceb_tu_str naiprinter("");
int		i;

errno=0;

if((ff = fopen(imafil.ravno(),"r")) == NULL)
 if(errno != ENOENT)
   {
    error_op_nfil(imafil.ravno(),errno,"");
    return(1);
   }

if(errno == ENOENT) /*Файл не существует*/
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено файл:"));
  SOOB.VVOD_SPISOK_add_MD(imafil.ravno());
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

/*определяем самое длинное имя принтера*/
int max_file=0;

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#' || stroka[0] == ' ' || stroka[0] == '\t' ||  stroka[0] == '\n')
   continue;

  if(polen(stroka,&bros,1,':') == 0)
   {
    sprinter.new_plus("");
    naiprinter.new_plus("");
    if(polen(bros.ravno(),&sprinter,1,'|') == 0)
      polen(bros.ravno(),&naiprinter,2,'|');
    else
      sprinter.new_plus(bros.ravno());
      
    if(naiprinter.ravno()[0] == '.')
      continue;
      
    if(iceb_tu_strlen(sprinter.ravno()) > max_file)
     max_file=iceb_tu_strlen(sprinter.ravno());
     
   }
 }
rewind(ff);

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#' || stroka[0] == ' ' || stroka[0] == '\t' ||  stroka[0] == '\n')
   continue;

  if(polen(stroka,&bros,1,':') == 0)
   {
    sprinter.new_plus("");
    naiprinter.new_plus("");
    if(polen(bros.ravno(),&sprinter,1,'|') == 0)
      polen(bros.ravno(),&naiprinter,2,'|');
    else
      sprinter.new_plus(bros.ravno());

    if(naiprinter.ravno()[0] == '.')
      continue;
      
    sprintf(stroka,"%-*s|%s",max_file,sprinter.ravno(),naiprinter.ravno());    
    masiv.plus(stroka);
   }
 }

fclose(ff);

if(masiv.kolih() != 0)
 {
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
  int kod_kl=0;
  int voz=dirmasiv(&masiv, -1, -1,0,gettext("Выберите нужный принтер"),1,&kod_kl);
  if(voz == -1 || kod_kl != ENTER)
    return(1);


  polen(masiv.ravno(voz),&sprinter,1,'|');

  nameprinter.new_plus(sprinter.ravno());

  /*переустанавливаем переменную чтобы через неё передать принтер другим подсистемам*/
  if(setenv("PRINTER",sprinter.ravno(),1) != 0)
   {
    sprintf(stroka,"%s-%s PRINTER %s",__FUNCTION__,gettext("Ошибка переустановки переменной"),sprinter.ravno());
    iceb_t_soob(stroka);
    return(1);
   }
/**************************
  if((nameprinter = getenv("PRINTER")) == NULL)
   {
    beep();
    printw("\nОшибка чтения переменной !\n");
    OSTANOV();
    return(1);
   }
********************/
 }
else
 {
  helstr(LINES-1,0,"F10","выход",NULL);

  class iceb_tu_str imapr("");
  VVOD  VVOD1(2);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя принтера"));

  if((i=vvod1(&imapr,56,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return(1);
  if(i == ESC)
   return(1);
  nameprinter.new_plus(imapr.ravno());

  /*переустанавливаем переменную чтобы через неё передать принтер другим подсистемам*/
  if(setenv("PRINTER",imapr.ravno(),1) != 0)
   {
    sprintf(stroka,"%s-%s PRINTER %s",__FUNCTION__,gettext("Ошибка переустановки переменной"),sprinter.ravno());
    iceb_t_soob(stroka);
    return(1);
   }

/**********************
  if((nameprinter = getenv("PRINTER")) == NULL)
   {
    beep();
    printw("\nОшибка чтения переменной !\n");
    OSTANOV();
    return(1);
   }
*****************/
 }

return(0);

}
