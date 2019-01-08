/*$Id: zvb_hreshatik_end.c,v 5.8 2013/05/17 14:56:19 sasa Exp $*/
/*05.11.2009	23.05.2007	Белых А.И.	zvb_hreshatik_end.c
Концовка формирования данных для зачисления на карт-счета для банка Крещатик
*/
#include "buhg.h"


void zvb_hreshatik_end(double sumai,int kolzap,FILE *ffr,FILE *ff)
{
class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;

iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);



fprintf(ffr,"\
-------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(61,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());
podpis(ffr); 
fclose(ffr);



fprintf(ff,"99%010d%012.2f%012.2f\n",kolzap,sumai,sumai);
fclose(ff);


}
