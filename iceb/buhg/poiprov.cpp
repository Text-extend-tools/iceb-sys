/* $Id: poiprov.c,v 5.53 2014/01/31 12:11:33 sasa Exp $ */
/*16.04.2017	27.09.1999	Белых А.И.	poiprov.c
Поиск проводок
*/
#include        "buhg.h"
 
void	forzap(class poiprov_rek *poi);


void		poiprov()
{
static class poiprov_rek poi;

int		kom,kom1;
short		dn,mn,gn;
short		dk,mk,gk;
short		dnz,mnz,gnz;
short		dkz,mkz,gkz;
char		strsql[5000];
struct  tm      bf;
time_t		vrem,vremk;
class iceb_tu_str bros("");
int metka_sort=0;

VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_ZAG(gettext("Поиск проводок"));


VV.VVOD_SPISOK_add_ZAG(gettext("По не веденым реквизитам поиск не выполняется."));

VV.VVOD_SPISOK_add_MD(gettext("Счёт.....................(,,)...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Счёт к-нт................(,,)...."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.............(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Коментарий......................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Дебет............................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Кредит..........................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(по дате проводки).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(по дате проводки).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Метка проводки...............(,,)"));//8
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.................."));//9
VV.VVOD_SPISOK_add_MD(gettext("Код операции................(,,)."));//10
VV.VVOD_SPISOK_add_MD(gettext("Группа контр-нта............(,,)."));//11
VV.VVOD_SPISOK_add_MD(gettext("Статус счетов..............(+/-)."));//12
VV.VVOD_SPISOK_add_MD(gettext("КЭКЗ........................(,,)."));//13
VV.VVOD_SPISOK_add_MD(gettext("Дата начала....(по дате записи).."));//14
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.....(по дате записи).."));//15

VV.VVOD_SPISOK_add_data(poi.sh.ravno(),128);//0
VV.VVOD_SPISOK_add_data(poi.shk.ravno(),128);//1 
VV.VVOD_SPISOK_add_data(poi.kor.ravno(),128);//2
VV.VVOD_SPISOK_add_data(poi.raz.ravno(),128);//3
VV.VVOD_SPISOK_add_data(poi.deb.ravno(),16);//4
VV.VVOD_SPISOK_add_data(poi.kre.ravno(),16);//5
VV.VVOD_SPISOK_add_data(poi.dat1.ravno(),11);//6
VV.VVOD_SPISOK_add_data(poi.dat2.ravno(),11);//7
VV.VVOD_SPISOK_add_data(poi.kto.ravno(),128);//8
VV.VVOD_SPISOK_add_data(poi.nn.ravno(),128);//9
VV.VVOD_SPISOK_add_data(poi.kop.ravno(),128);//10
VV.VVOD_SPISOK_add_data(poi.grupa.ravno(),128);//11
VV.VVOD_SPISOK_add_data(poi.status.ravno(),2);//12
VV.VVOD_SPISOK_add_data(poi.kekv_poi.ravno(),128);//13
VV.VVOD_SPISOK_add_data(poi.dat1z.ravno(),11);//14
VV.VVOD_SPISOK_add_data(poi.dat2z.ravno(),11);//15

for(;;)
 {

  naz:;
  
  clear();

  mvprintw(LINES-4,0,gettext("Если дата конца не введена, то поиск выполняется до конця месяца с даты начала"));
  mvprintw(LINES-3,0,gettext("Если вместо даты конца введено символ \"*\", то поиск выполняется до конца базы"));
  mvprintw(LINES-2,0,gettext("Если вместо дебета/кредита введено \"+\", то будут найдены все дебеты/кредиты"));

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("поиск"),
  "F3",gettext("реквизиты"),
  "F4",gettext("очистить"),
  "F5",gettext("сортировка"),
  "F10",gettext("выход"),NULL);


  kom=VV.vvod(0,1,0,-1,-1);

  poi.sh.new_plus(VV.data_ravno(0));
  poi.shk.new_plus(VV.data_ravno(1));
  poi.kor.new_plus(VV.data_ravno(2));
  poi.raz.new_plus(VV.data_ravno(3));
  poi.deb.new_plus(VV.data_ravno(4));
  poi.kre.new_plus(VV.data_ravno(5));
  poi.dat1.new_plus(VV.data_ravno(6));
  poi.dat2.new_plus(VV.data_ravno(7));
  poi.kto.new_plus(VV.data_ravno(8));
  poi.nn.new_plus(VV.data_ravno(9));
  poi.kop.new_plus(VV.data_ravno(10));
  poi.grupa.new_plus(VV.data_ravno(11));
  poi.status.new_plus(VV.data_ravno(12));
  poi.kekv_poi.new_plus(VV.data_ravno(13));
  poi.dat1z.new_plus(VV.data_ravno(14));
  poi.dat2z.new_plus(VV.data_ravno(15));

 
  switch (kom)
   {
    case FK10:
    case ESC:
       return;
       break;

    case FK1:
      GDITE();
      iceb_t_pdoc("buhg3_3.txt");
      clear();
      goto naz;

    case FK2:
    case PLU:
       break;

    case FK3:
       MENU.VVOD_delete();
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка меток проводок"));
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));
       MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

       clearstr(LINES-1,0);
       kom1=0;       
       while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

       switch (kom1)
        {
         case 4 :
         case -1:
          goto naz;

         case 0 :
          
           vibrek("Plansh",&bros);
           break; 

         case 1 :
           vibrek("Kontragent",&poi.kor);
           VV.VVOD_SPISOK_zapis_data(2,poi.kor.ravno());
           break;

         case 2 :
           MENU.VVOD_delete();
           sprintf(strsql,"%-3s %s",ICEB_MP_MATU,gettext("Материальный учет"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_ZARP,gettext("Заработная плата"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_UOS,gettext("Учет основных средств"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_PPOR,gettext("Платежные поручения"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_PTRE,gettext("Платежные требования"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_USLUGI,gettext("Учет услуг"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_KASA,gettext("Учет кассовых ордеров"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           sprintf(strsql,"%-3s %s",ICEB_MP_UKR,gettext("Учет командировочных расходов"));
           MENU.VVOD_SPISOK_add_MD(strsql);
           MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
           clearstr(LINES-1,0);
           kom1=0;           
           while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

           switch (kom1)
            {
             case 0:
              VV.data_z_plus(8,ICEB_MP_MATU);
              break;
             case 1:
              VV.data_z_plus(8,ICEB_MP_ZARP);
              break;
             case 2:
              VV.data_z_plus(8,ICEB_MP_UOS);
              break;
             case 3:
              VV.data_z_plus(8,ICEB_MP_PPOR);
              break;
             case 4:
              VV.data_z_plus(8,ICEB_MP_PTRE);
              break;
             case 5:
              VV.data_z_plus(8,ICEB_MP_USLUGI);
              break;
             case 6:
              VV.data_z_plus(8,ICEB_MP_KASA);
              break;
             case 7:
              VV.data_z_plus(8,ICEB_MP_UKR);
              break;
            }
           break;
           
         case 3 :
           vibrek("Gkont",&poi.grupa);
           VV.VVOD_SPISOK_zapis_data(11,poi.grupa.ravno());
           break;
        }
      goto naz;

    case FK4: //Очистить реквизиты
      VV.VVOD_clear_data();
      goto naz;

    case FK5: //Задание метода сортировки
       MENU.VVOD_delete();
       MENU.VVOD_SPISOK_add_ZAG(gettext("Виберите метод сортировки"));

       MENU.VVOD_SPISOK_add_MD(gettext("В порядке увеличения дебетов")); //0
       MENU.VVOD_SPISOK_add_MD(gettext("В порядке увеличения кредитов"));//1
       MENU.VVOD_SPISOK_add_MD(gettext("В порядке уменьшения дебетов")); //2

       MENU.VVOD_SPISOK_add_MD(gettext("В порядке уменьшения кредитов"));//3
       MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4
  
       kom1=0;           
       while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

       switch (kom1)
        {
         case -1:
         case 4:
           goto naz;
                            
        }
      metka_sort=kom1+1;
      break;
            
    default:
      goto naz;
   }


  dn=mn=gn=dk=mk=gk=0;
  dnz=mnz=gnz=dkz=mkz=gkz=0;
  if(poi.dat1.getdlinna() > 1)
  if(rsdat(&dn,&mn,&gn,poi.dat1.ravno(),0) != 0 )
   { 
    iceb_t_soob(gettext("Не верно введена дата начала !"));
    goto naz;
   } 
  if(poi.dat2.ravno()[0] == '\0')
   {
    if(poi.dat1.ravno()[0] != '\0')
     { 
      dk=dn; mk=mn; gk=gn;
      dpm(&dk,&mk,&gk,5); 
     }
   } 
  else   
   if(rsdat(&dk,&mk,&gk,poi.dat2.ravno(),0) != 0)
    { 
     if(poi.dat2.ravno()[0] == '*')
      {
       dk=mk=gk=0; 
      }
     else
      {
       iceb_t_soob(gettext("Не верно введена дата конца !"));
       goto naz;
      } 
    } 

  if(rsdat(&dnz,&mnz,&gnz,poi.dat1z.ravno(),0) != 0 && poi.dat1z.ravno()[0] != '\0')
   { 
    iceb_t_soob(gettext("Не верно введена дата начала !"));
    goto naz;
   } 
  if(poi.dat2z.ravno()[0] == '\0')
   {
    if(poi.dat1z.ravno()[0] != '\0')
     { 
      dkz=dnz; mkz=mnz; gkz=gnz;
      dpm(&dkz,&mkz,&gkz,5); 
     }
   } 
  else   
   if(rsdat(&dkz,&mkz,&gkz,poi.dat2z.ravno(),0) != 0)
    { 
     if(poi.dat2z.ravno()[0] == '*')
      {
       dkz=mkz=gkz=0; 
      }
     else
      {
       iceb_t_soob(gettext("Не верно введена дата конца !"));
       goto naz;
      } 
    } 

  memset(strsql,'\0',sizeof(strsql));
  poi.zapros.new_plus("select * from Prov where");
  
  if(dk == 0)
    sprintf(strsql," datp >= '%04d-%02d-%02d'",gn,mn,dn);
  else
    sprintf(strsql," datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d'",
    gn,mn,dn,gk,mk,dk);

  poi.zapros.plus(strsql);
  
  if(dnz != 0)
   {  
    bf.tm_mday=(int)dnz;
    bf.tm_mon=(int)(mnz-1);
    bf.tm_year=(int)(gnz-1900);

    bf.tm_hour=(int)0;
    bf.tm_min=(int)0;
    bf.tm_sec=0;

    vrem=0;
    
    vrem=mktime(&bf);
    
    sprintf(strsql," and vrem >= %ld",vrem);

    poi.zapros.plus(strsql);
   }

  if(dkz != 0)
   {
    bf.tm_mday=(int)dkz;
    bf.tm_mon=(int)(mkz-1);
    bf.tm_year=(int)(gkz-1900);

    bf.tm_hour=(int)23;
    bf.tm_min=(int)59;
    bf.tm_sec=59;

    vremk=0;

    vremk=mktime(&bf);
    sprintf(strsql," and vrem <= %ld",vremk);

    poi.zapros.plus(strsql);
   }    




  forzap(&poi);

  memset(strsql,'\0',sizeof(strsql));  

  if(metka_sort == 0)
     strcpy(strsql," order by datp asc,nomd asc");

  if(metka_sort == 1)
     strcpy(strsql," order by deb asc");

  if(metka_sort == 2)
    strcpy(strsql," order by kre asc");

  if(metka_sort == 3)
     strcpy(strsql," order by deb desc");

  if(metka_sort == 4)
     strcpy(strsql," order by kre desc");
    

  poi.zapros.plus(strsql);

  dirprov(&poi);
 }

}

/***********************************/
/*Формирование запроса             */
/***********************************/
void forzap(class poiprov_rek *poi)
{
char		bros[512];
char		bros1[512];
int		kolshet=0;

if(poi->sh.ravno()[0] != '\0')
 {
  kolshet=pole2(poi->sh.ravno(),',');

  if(kolshet == 0)
   {
    sprintf(bros," and sh like '%s%%'",poi->sh.ravno());
    poi->zapros.plus(bros);    
   }
  else
   {

    for(int i1=1; i1 <= kolshet;i1++)
     if(polen(poi->sh.ravno(),bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(poi->sh.ravno()[0] != '-')
          sprintf(bros1," and (sh like '%s%%'",bros);
         else
          sprintf(bros1," and (sh not like '%s%%'",bros);
        }
       else          
        {
         if(poi->sh.ravno()[0] != '-')
           sprintf(bros1," or sh like '%s%%'",bros);
         else
           sprintf(bros1," and sh not like '%s%%'",bros);
        }
       poi->zapros.plus(bros1);    
      }        
    poi->zapros.plus(")");
   }
 }

if(poi->shk.ravno()[0] != '\0')
 {
  kolshet=pole2(poi->shk.ravno(),',');

  if(kolshet == 0)
   {
    sprintf(bros," and shk like '%s%%'",poi->shk.ravno());
    poi->zapros.plus(bros);    
   }
  else
   {
    for(int i1=1; i1 <= kolshet;i1++)
     if(polen(poi->shk.ravno(),bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(poi->shk.ravno()[0] != '-')
          sprintf(bros1," and (shk like '%s%%'",bros);
         else
          sprintf(bros1," and (shk not like '%s%%'",bros);
        }
       else          
        {
         if(poi->shk.ravno()[0] != '-')
          sprintf(bros1," or shk like '%s%%'",bros);
         else
          sprintf(bros1," and shk not like '%s%%'",bros);
        }
       poi->zapros.plus(bros1);    
      }        
    poi->zapros.plus(")");
   }
 }

if(poi->kor.ravno()[0] != '\0')
 {
  kolshet=pole2(poi->kor.ravno(),',');

  if(kolshet == 0)
   {
    sprintf(bros," and kodkon='%s'",poi->kor.ravno());
    poi->zapros.plus(bros);    
   }
  else
   {
    for(int i1=1; i1 <= kolshet;i1++)
     if(polen(poi->kor.ravno(),bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(poi->kor.ravno()[0] != '-')
          sprintf(bros1," and (kodkon='%s'",bros);
         else
          sprintf(bros1," and (kodkon <> '%s'",bros);
        }
       else          
        {
         if(poi->kor.ravno()[0] != '-')
          sprintf(bros1," or kodkon='%s'",bros);
         else
          sprintf(bros1," and kodkon <> '%s'",bros);
        }
       poi->zapros.plus(bros1);    
      }        
    poi->zapros.plus(")");
   }
 }
if(poi->status.ravno()[0] == '+')
 {
  poi->zapros.plus(" and val=0");
 }
if(poi->status.ravno()[0] == '-')
 {
  poi->zapros.plus(" and val=1");
 }

}
