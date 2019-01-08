/*$Id:$*/
/*22.03.2017	22.03.2017	Белых А.И.	iceb_t_nn9_hap.c
Распечатка шапки налоговой накладной
*/
#include "buhl.h"

void iceb_t_nn9_hap(int nomstr,int *kolstr,FILE *ff)
{
int kol=iceb_t_insfil("nn9_hap.alx",ff);
if(kolstr != NULL)
 *kolstr+=kol;
}
