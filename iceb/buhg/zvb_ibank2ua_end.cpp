/*$Id: zvb_ibank2ua_end.c,v 5.7 2013/05/17 14:56:19 sasa Exp $*/
/*03.10.2016	18.09.2008	Белых А.И.	zvb_ibank2ua_end.c
Концовка формирования данных для зачисления на карт-счета для системы iBank2 UA
*/
#include "buhg.h"


void zvb_ibank2ua_end(double sumai,const char *vid_nah,FILE *ffr,FILE *ff)
{
class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;

iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);


fprintf(ffr,"\
--------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_tu_kolbait(61,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());
podpis(ffr); 
fclose(ffr);
class iceb_tu_str shet_bp("29242000000110");


fprintf(ff,"\
ONFLOW_TYPE=%s\n\
AMOUNT=%.2f\n\
PAYER_BANK_ACCOUNT=%s\n\
COMMISSION_PERCENT=0.00\n",
vid_nah,
sumai,
shet_bp.ravno());

fclose(ff);


}
