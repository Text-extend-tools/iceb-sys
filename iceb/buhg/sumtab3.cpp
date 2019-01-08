/* $Id: sumtab3.c,v 5.11 2013/05/17 14:56:12 sasa Exp $ */
/*19.01.2005	03.03.2000	Белых А.И.	sumtab3.c
Просмотр и установка фамилий и паролей операторов
*/
//#include	<libconfig.h>
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#endif
#include	"buhg.h"

#ifdef HAVE_TORGDEV
extern cassa_pars 	*cp[MAX_CASS];
#endif

void		sumtab3(int nomer,char imqkas[],int termlin,int nomernl)
{
#ifdef HAVE_TORGDEV
int		i,i1,i2;
char		parol[9],fio[11];
SPISOK	masivmenu;
long		dlinm;
char		bros[512];
char		nomerpol;

VVOD SOOBN(1);
VVOD VV(0);

naz1:;
clear();

SOOBN.VVOD_delete();
SOOBN.VVOD_SPISOK_add_MD("Чекайте ! Читається список операторів !");
soobsh(&SOOBN,LINES-6,0,1);

move(LINES-1,0);

/*Создаем массив операторов*/
dlinm=i2=0;
for(i=1 ; i <= 30 ; i++)
 {
  strzag(LINES-1,0,30,i2);
  i2++;
  if((i1=SG_read_oper(cp[nomer],i,fio,parol)) != 0)
   {
    beep();
    printw("%s %d !\n",CA_error,i1);
    OSTANOV();
    continue;
   }
  sprintf(bros,"%02d|%-10s|%-9s|",i,fio,parol);
  masivmenu.plus(bros);

 }
clear();
printw("Каса:%s Термінальна лінія:%d Номер на лінії:%d",
imqkas,termlin,nomernl);
helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
"F10",gettext("выход"),NULL);
int kod_kl=0;
nomerpol=dirmasiv(&masivmenu,2,0,0,"  N   Прізвище   Пароль",0);
if(nomerpol >= 0 && kod_kl == ENTER)
 {
  strncpy(bros,masivmenu.ravno(nomerpol),sizeof(bros)-1);
  polen(bros,fio,sizeof(fio),2,'|');
  polen(bros,parol,sizeof(parol),3,'|');
 }
else
 return;
 
if(nomerpol < 0)
  return;

VV.VVOD_delete();

VV.VVOD_SPISOK_add_data(fio,sizeof(fio));
VV.VVOD_SPISOK_add_data(parol,sizeof(parol));
VV.VVOD_SPISOK_add_MD("Прізвище оператора..");
VV.VVOD_SPISOK_add_MD("Пароль..............");

naz:;


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

//i=vvod(&LL,0,1,3);
i=VV.vvod(0,1,1,-1,-1);

switch(i)
 {
  case FK10:
  case ESC:
    return;
  case FK2:
  case PLU:
    memset(fio,'\0',sizeof(fio));
    strncpy(fio,VV.VVOD_SPISOK_return_data(0),sizeof(fio)-1);
    memset(parol,'\0',sizeof(parol));
    strncpy(parol,VV.VVOD_SPISOK_return_data(1),sizeof(parol)-1);
    
    if((i1=SG_prog_oper(cp[nomer],nomerpol,fio,parol)) != 0)
     {
      VVOD SOOB(1);
      sprintf(bros,"Помилка запису !");
      SOOB.VVOD_SPISOK_add_MD(bros);
      sprintf(bros,"%s %d",CA_error,i1);
      SOOB.VVOD_SPISOK_add_MD(bros);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
     }
 
    goto naz1;
    
  default:
    goto naz;
 }
#endif


}
