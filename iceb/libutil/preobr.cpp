/*$Id: preobr.c,v 5.15 2013/10/09 07:05:14 sasa Exp $*/
/*07.10.2013   19.11.1992      Белых А.И.      preobr.c
Подпрограмма преобразования числа в текстовую строку
				      |       |       |
			     12 11 10 | 9 8 7 | 6 5 4 | 3  2  1
				      |       |       |
*/
#include	"util.h"


/***********************************/
void preobr(double hs, //Число
class iceb_tu_str *str, //Строка
short gri)  //0-грн 1-гривень 2-целую часть без гривень.
{
class iceb_tu_str s("");
class iceb_tu_str s1("");
class iceb_tu_str s2("");
int             i;
class iceb_tu_str sss("");

s.new_plus(hs,2);
polen(s.ravno(),&s2,2,'.');

str->new_plus(iceb_tu_propis(hs,1));


if(gri == 0)
 {
  sss.new_plus(str->ravno());
  sss.plus(" ",gettext("грн."));
  sss.plus(" ",s2.ravno());
  sss.plus(" ",gettext("коп."));
 }
 
if(gri == 1)
 {
  i=pole2(str->ravno(),' ');
  polen(str->ravno(),&s,i,' ');

  s1.new_plus(gettext("гривень"));

  if(SRAV(s.ravno(),gettext("одна"),0) == 0)
   s1.new_plus(gettext("гривня"));

  if(SRAV(s.ravno(),gettext("две"),0) == 0)
    s1.new_plus(gettext("гривни"));

  if(SRAV(s.ravno(),gettext("три"),0) == 0)
      s1.new_plus(gettext("гривни"));

  if(SRAV(s.ravno(),gettext("четыре"),0) == 0)
   s1.new_plus(gettext("гривни"));

  switch (s2.ravno()[1])
   {
    case '1' :
      if(s2.ravno()[0] == '1')
        s.new_plus(gettext("копеек"));
     else
        s.new_plus(gettext("копейка"));
     break;
    case '2' :
    case '3' :
    case '4' :
     s.new_plus(gettext("копейки"));
     break;
    default:
     s.new_plus(gettext("копеек"));
     break;
   }

  if(str->ravno()[0] != '\0')
   {
    sss.new_plus(str->ravno());
    sss.plus(" ",s1.ravno());
    sss.plus(" ",s2.ravno());
    sss.plus(" ",s.ravno());
   }
  else
   {
    sss.new_plus(s2.ravno());
    sss.plus(" ",s.ravno());
   }
 }

if(gri == 2)
  sss.new_plus(str->ravno());

str->new_plus(sss.ravno());
}
/***********************************/
void preobr(double hs, //Число
char *str, //Строка
short gri)  //0-грн 1-гривень 2-целую часть без гривень.
{
class iceb_tu_str stroka;
preobr(hs,&stroka,gri);
strcpy(str,stroka.ravno());

}
