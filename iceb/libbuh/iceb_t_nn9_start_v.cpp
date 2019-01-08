/*$Id:$*/
/*22.03.2017	22.03.2017	iceb_t_nn9_start_v.c
Ввод дополнительных реквизитов к налоговой накладной
*/

#include "buhl.h"

int iceb_t_nn9_start_v(class iceb_tu_str *zved_pn,class iceb_tu_str *snoz,class iceb_tu_str *nevid,class iceb_tu_str *tippr)
{
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты"));

VV.VVOD_SPISOK_add_MD("Зведена податкова накладна............................");//0
VV.VVOD_SPISOK_add_MD("Складена на операції, звільнені від оподаткування.....");//1
VV.VVOD_SPISOK_add_MD("Не підлягає наданню отримовачу/покупцю................");//2
VV.VVOD_SPISOK_add_MD("Тип причини...........................................");//3

VV.VVOD_SPISOK_add_data(zved_pn->ravno(),3);//0
VV.VVOD_SPISOK_add_data(snoz->ravno(),3);//1
VV.VVOD_SPISOK_add_data(nevid->ravno(),4);//1
VV.VVOD_SPISOK_add_data(tippr->ravno(),3);//2

naz:;

 
helstr(LINES-1,0,
"F2/+",gettext("ввести"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

zved_pn->new_plus(VV.VVOD_SPISOK_return_data(0));
snoz->new_plus(VV.VVOD_SPISOK_return_data(1));
nevid->new_plus(VV.VVOD_SPISOK_return_data(2));
tippr->new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

return(0);



}
