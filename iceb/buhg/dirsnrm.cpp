/*$Id:$*/
/*06.03.2017	06.03.2017	Белых А.И.	dirsnrm.c
Ввод и корректировка норм списания материалов на изготовление деталей
*/
#include        <errno.h>
#include        <buhg.h>

class dirsnrm_poi
 {
  public:
   class iceb_tu_str kod_det;
   class iceb_tu_str kod_mat;
   class iceb_tu_str naim_det;
   class iceb_tu_str naim_mat;
   
   short metka_poi;

  dirsnrm_poi()
   {
    metka_poi=0;
    naim_det.plus("");
    naim_mat.plus("");    
   }
 };
 
void dirsnrm_vvod(unsigned int nom_zap);
void dirsnrm_sap(class dirsnrm_poi*);
int dirsnrm_zap(VVOD *VV,unsigned int nom_zap,int vs,int vz);
void	dirsnrm_ras(SQLCURSOR *cur,class dirsnrm_poi *poisk);
int dirsnrm_prov(SQL_str row,class dirsnrm_poi *poisk);
int dirfortti_poi(class dirsnrm_poi *poisk);



int dirsnrm()
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class dirsnrm_poi poisk;
class iceb_tu_str naim_det("");
class iceb_tu_str naim_mat("");
class VVOD DANET(1);
class VVOD VVOD1(2);
class SQLCURSOR cur,cur1;


naz1:;

short           kls=LINES-6;
unsigned int nom_zap[kls];

sprintf(strsql,"select * from Musnrm order by kd asc,eid asc,km asc");


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirsnrm_sap(&poisk);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(dirsnrm_prov(row,&poisk) != 0)
   continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);
  else
   naim_mat.new_plus("");
   
  nom_zap[i]=atoi(row[0]);  

  sprintf(strsql,"%-6s|%-*.*s|%-6s|%-*.*s|%10.10g",
  row[1],
  iceb_tu_kolbait(20,naim_det.ravno()),
  iceb_tu_kolbait(20,naim_det.ravno()),
  naim_det.ravno(),
  row[3],
  iceb_tu_kolbait(20,naim_mat.ravno()),
  iceb_tu_kolbait(20,naim_mat.ravno()),
  naim_mat.ravno(),
  atof(row[4]));

  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirsnrm_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(dirsnrm_prov(row,&poisk) != 0)
     continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);
    if(dirsnrm_prov(row,&poisk) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
/*  
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(dirsnrm_prov(row,&poisk) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1:  /*Получить подсказку*/
    GDITE();
//    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:
  case SFK2:   /*Корректировать*/
    dirsnrm_vvod(nom_zap[kom]);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirsnrm_vvod(0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Musnrm where nz=%d",nom_zap[kom]);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

    

  case FK4:       /*Поиск*/
     
     dirfortti_poi(&poisk);

     clear();
     kom=pozz=0;
     goto naz1;

  case FK5:    //Распечатать
    dirsnrm_ras(&cur,&poisk);
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void dirsnrm_vvod(unsigned int nom_zap)
{
class iceb_tu_str kod("");
class iceb_tu_str naim("");
char		strsql[2048];
time_t		vremz=0;
int		ktozap=0;
int		N=0,K=0;
int vs=0;
int vz=0;

SQL_str row;
class SQLCURSOR cur;
class VVOD VV(0);
class VVOD MENU(3);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(10);//1
VV.VVOD_SPISOK_add_data(10);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(10);//4
VV.VVOD_SPISOK_add_data(60);//5
VV.VVOD_SPISOK_add_data(60);//6
VV.VVOD_SPISOK_add_data(60);//7



if(nom_zap != 0)
 {

  sprintf(strsql,"select * from Musnrm where nz=%d",nom_zap);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[2]);
  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  VV.VVOD_SPISOK_zapis_data(3,row[5]);

  sprintf(strsql,"%.10g",atof(row[4]));
  VV.VVOD_SPISOK_zapis_data(4,strsql);

  vs=atoi(row[6]);
  vz=atoi(row[7]);
    
  VV.VVOD_SPISOK_zapis_data(7,row[8]);


  ktozap=atoi(row[9]);
  vremz=atol(row[10]);

     
 }



VV.VVOD_SPISOK_add_MD(gettext("Код детали.............................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения детали................"));//1
VV.VVOD_SPISOK_add_MD(gettext("Код материала..........................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения материала............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Количество материала на единицу детали.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Вид списания............................"));//5
VV.VVOD_SPISOK_add_MD(gettext("Вид заготовки..........................."));//6
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.............................."));//7


naz:;

VV.VVOD_delete_ZAG();

if(nom_zap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(ktozap,vremz,&VV);
   
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
 }

if(VV.data_ravno(0)[0] != '\0')
 {
  sprintf(strsql,"select naimat from Material where kodm=%d",VV.data_ravno_atoi(0));
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  sprintf(strsql,"%s:%s",gettext("Наименование детали"),naim.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


if(VV.data_ravno(2)[0] != '\0')
 {
  sprintf(strsql,"select naimat from Material where kodm=%d",VV.data_ravno_atoi(2));
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%s:%s",gettext("Наименование материала"),naim.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


if(vs == 0)
 VV.data_plus(5,gettext("Основной"));
else
 VV.data_plus(5,gettext("Альтернативный"));

if(vz == 0)
 VV.data_plus(6,gettext("Материал"));
else
 VV.data_plus(6,gettext("Покупная"));
 


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
//"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);

for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 1)
     {
      kod.new_plus("");
      naim.new_plus("");
      if(iceb_tl_ei(2,&kod,&naim) == 0)
       VV.data_plus(N,kod.ravno());
      N++;
      goto naz;
     }
    if(N == 3)
     {
      kod.new_plus("");
      naim.new_plus("");
      if(iceb_tl_ei(2,&kod,&naim) == 0)
       VV.data_plus(N,kod.ravno());
      N++;
      goto naz;
     }

    if(N == 5)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Основной"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Альтернативный"));//1

      clearstr(LINES-1,0);
      
      while(menu3w(stdscr,&MENU,&vs,-1,-1,0) != 0);
      N++;
      goto naz; 
     }

    if(N == 6)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Материал"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Покупная"));//1

      clearstr(LINES-1,0);
      
      while(menu3w(stdscr,&MENU,&vz,-1,-1,0) != 0);
      N++;
      goto naz; 
     }


    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      N++;
      goto naz;
     }

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
     {
      N++;
      goto naz;
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }

  if(K == FK1) //Помощь
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
//    prosf(strsql);
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) //Записываем
   {
    if(dirsnrm_zap(&VV,nom_zap,vs,vz) != 0)
     goto naz;
    break;
   }


 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int dirsnrm_zap(VVOD *VV,unsigned int nom_zap,int vs,int vz)
{
char	strsql[2048];

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

sprintf(strsql,"select naimat from Material where kodm=%d",VV->data_ravno_atoi(0));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d %s!",gettext("Код детали"),VV->data_ravno_atoi(0),gettext("не найден в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select naimat from Material where kodm=%d",VV->data_ravno_atoi(2));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %d %s!",gettext("Код материала"),VV->data_ravno_atoi(0),gettext("не найден в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select naik from Edizmer where kod='%s'",VV->data_ravno(1));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Код единицы измерения"),VV->data_ravno(1),gettext("не найден в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }
sprintf(strsql,"select naik from Edizmer where kod='%s'",VV->data_ravno(3));
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Код единицы измерения"),VV->data_ravno(3),gettext("не найден в справочнике"));
  iceb_t_soob(strsql);
  return(1);
 }



if(nom_zap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Musnrm (kd,eid,km,kol,eim,vs,vz,kom,ktoz,vrem) values(%d,'%s',%d,%.10g,'%s',%d,%d,'%s',%d,%ld)",
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(4),
  VV->data_ravno_filtr(3),
  vs,
  vz,
  VV->data_ravno_filtr(7),
  iceb_t_getuid(),time(NULL));
else
  sprintf(strsql,"update Musnrm set \
kd=%d,\
eid='%s',\
km=%d,\
kol=%.10g,\
eim='%s',\
vs=%d,\
vz=%d,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",  
  VV->data_ravno_atoi(0),
  VV->data_ravno_filtr(1),
  VV->data_ravno_atoi(2),
  VV->data_ravno_atof(4),
  VV->data_ravno_filtr(3),
  vs,
  vz,
  VV->data_ravno_filtr(7),
  iceb_t_getuid(),
  time(NULL),
  nom_zap);

if(sql_zapis(strsql,0,0) != 0)
 return(1);



return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	dirsnrm_sap(class dirsnrm_poi *poisk)
{
short		y=2;
short           x=COLS-20;



clear();
printw("%s\n",gettext("Ввод и корректировка процента выполнения"));

mvprintw(y++,0,"     1            2              3            4               5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код дет."));
mvprintw(y++,x,"2-%s",gettext("наим.дет."));
mvprintw(y++,x,"3-%s",gettext("код мат."));
mvprintw(y++,x,"4-%s",gettext("наим мат."));
mvprintw(y++,x,"5-%s",gettext("количество"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);

  mvprintw(y++,x,"%s:",gettext("Поиск"));
 
 
  attroff(A_BLINK);
  
 }
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
}

/********************/
/*Распечатать       */
/********************/

void	dirsnrm_ras(SQLCURSOR *cur,class dirsnrm_poi *poisk)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
class iceb_tu_str naim_det("");
class iceb_tu_str naim_mat("");
class iceb_tu_str naim_vidsp("");
class iceb_tu_str naim_vidzag("");
char strsql[1024];

int vs=0,vz=0;

sprintf(imaf,"sns%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Справочник норм списания материалов на изготовление деталей"),ff);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код.дт|    Наименование детали       |Ед.изм|Код ма|  Наименование материала      |Ед.изм|Количество|Вид списания|Вид заготовки|\n"));
/*
123456|123456789012345678901234567890|123456|123456|123456789012345678901234567890|123456 1234567890 123456789012 1234567890123
*/
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {

  if(dirsnrm_prov(row,poisk) != 0)
   continue;

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);
  else
   naim_mat.new_plus("");
    
  vs=atoi(row[6]);
  vz=atoi(row[7]);

  if(vs == 0)
   naim_vidsp.new_plus(gettext("Основной"));
  else
   naim_vidsp.new_plus(gettext("Альтернативный"));

  if(vz == 0)
   naim_vidzag.new_plus(gettext("Материал"));
  else
   naim_vidzag.new_plus(gettext("Покупная"));
   
  fprintf(ff,"%6s %-*.*s %-*s %6s %-*.*s %-*s %10.10g %-*.*s %-*.*s\n",
  row[1],
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  iceb_tu_kolbait(6,row[2]),
  row[2],
  row[3],
  iceb_tu_kolbait(30,naim_mat.ravno()),
  iceb_tu_kolbait(30,naim_mat.ravno()),
  naim_mat.ravno(),
  iceb_tu_kolbait(6,row[5]),
  row[5],
  atof(row[4]),
  iceb_tu_kolbait(12,naim_vidsp.ravno()),
  iceb_tu_kolbait(12,naim_vidsp.ravno()),
  naim_vidsp.ravno(),
  iceb_tu_kolbait(13,naim_vidzag.ravno()),
  iceb_tu_kolbait(13,naim_vidzag.ravno()),
  naim_vidzag.ravno());
  
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");


podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник норм списания материалов на изготовление деталей"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");

}

/*********************************/
/*ввод реквизитов поиска*/
/******************************/
int dirfortti_poi(class dirsnrm_poi *poisk)
{
class VVOD MENU(3);
class VVOD VV(0);
int kl=0;
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Код детали.............(,,).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код материала..........(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Наименование материала.(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Наименование детали....(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Наименование материала.(,,).."));//4

VV.VVOD_SPISOK_add_data(poisk->kod_det.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->kod_mat.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->naim_det.ravno(),128);
VV.VVOD_SPISOK_add_data(poisk->naim_mat.ravno(),128);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kl=VV.vvod(0,1,1,-1,-1);

poisk->kod_det.new_plus(VV.data_ravno(0));
poisk->kod_mat.new_plus(VV.data_ravno(1));
poisk->naim_det.new_plus(VV.data_ravno(3));
poisk->naim_mat.new_plus(VV.data_ravno(4));

switch(kl)
 {
  case FK10:
  case ESC:
    poisk->metka_poi=0; 
    break;

  case FK2:
    poisk->metka_poi=1; 
    break;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
               
  default:
    goto naz;
 }

return(poisk->metka_poi);
}
/******************************/
/*проверка реквизитов поиска*/
/******************************/
int dirsnrm_prov(SQL_str row, //Строка для проверки 
class dirsnrm_poi *poisk)
{
SQL_str row1;
class SQLCURSOR cur1;

char strsql[1024];
if(poisk->metka_poi == 0)
 return(0);

if(proverka(poisk->kod_det.ravno(),row[1],0,0) != 0)
 return(1);
if(proverka(poisk->kod_mat.ravno(),row[3],0,0) != 0)
 return(1);
 

if(poisk->naim_det.getdlinna() > 1)
 {
  class iceb_tu_str naim_det("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);

  if(proverka(poisk->naim_det.ravno(),naim_det.ravno(),4,0) != 0)
   return(1);
 }  
  
if(poisk->naim_mat.getdlinna() > 1)
 {
  class iceb_tu_str naim_mat("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);

  if(proverka(poisk->naim_mat.ravno(),naim_mat.ravno(),4,0) != 0)
   return(1);

 }



return(0);
}
