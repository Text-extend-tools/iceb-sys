/* $Id: rscena.c,v 5.8 2013-04-07 10:22:44 sasa Exp $ */
/*29.07.2002	19.03.1997	Белых А.И.	rscena.c
Определение цены на материалы и товары для ввода в материальный учет
    1200 шт    539 гр
    539гр/1200шт=0.44|9166666гр Цена отсчета без округления
    1200шт*0.44гр=528гр
    539гр-528гр=11гр
    Вчикле делим 11гр сначала на 0.01,0.02,0.03 и т.д.
    11/0.01=1100 (Это количество по увеличенной цене)
    1100*(0.44+0.01)=495гр
    1200шт-1100шт=100шт (Это количество по начальной цене)
    100шт*0.44=44гр
    44гр+495гр=539гр 
*/
#include        <math.h>
#include	"buhg.h"


 
void		rscena()
{
double		suma,suma1;
double		kolih,kolih1,kolih2;
double		cena1,cena2;
short		par,i,i1;
double		pr;
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт цены."));

VV.VVOD_SPISOK_add_MD(gettext("Количество...."));
VV.VVOD_SPISOK_add_MD(gettext("Общая сумма..."));

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);


naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

if(par == FK2 || par == PLU)
 {
  clear();
  if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введено количество !"));
    goto naz;
   } 
  if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введена сумма !"));
    goto naz;
   } 
  kolih=ATOFM(VV.VVOD_SPISOK_return_data(0)); 
  suma=ATOFM(VV.VVOD_SPISOK_return_data(1));
  cena1=suma/kolih;
  /*Отбросить все что после второго знака после запятой*/
  i=(short)(cena1*100);
  cena1=(double)(i/100.);
//  kolihp=suma-kolih*cena1;  
  printw("%s - %.2f %s - %.2f\n%s - %.5g \n\
%.2f * %.2f = %.2f %s - %.2f\n",
  gettext("Количество"),
  kolih,
  gettext("Сумма"),
  suma,
  gettext("Цена стартовая"),
  cena1,
  kolih,cena1,kolih*cena1,
  gettext("Разница"),
  suma-kolih*cena1);

  printw("\
-------------------------------------------------------------------------\n");
  printw(gettext(" Цена       | Количество   | Сумма        |Количество    | Общая сумма  |\n"));
  printw("\
-------------------------------------------------------------------------\n");
  i1=0;
  for(i=1; i1 <= 15;i++)
   {
    cena2=cena1+(double)(i/100.);
    pr=i;
    pr/=100.;
    kolih1=(suma-kolih*cena1)/pr; 
    kolih2=kolih-kolih1;
    suma1=kolih1*cena2;
    
//    suma2=kolih2*cena1;
    
      i1++;
      printw("%12.3f %14.3f %14.3f %14.3f %14.3f\n",
      cena2,kolih1,suma1,kolih2,suma1+kolih2*cena1);

   }   

 } 
OSTANOV();
}
