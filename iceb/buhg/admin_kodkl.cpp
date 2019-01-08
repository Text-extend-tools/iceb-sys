/*$Id:$*/
/*17.05.2016	13.05.2016	Белых А.И.	admin_kodkl.c
Распечатать коды клавиш
*/
#include "buhg.h"

void admin_kodkl()
{
clear();

printw("MAXFKEYS=%d\n",MAXFKEYS);
printw("F1  Dec:%d Hex:%X OC:%o intchar=%d\n",FK1,FK1,FK1,(char)FK1);
printw("F2  Dec:%d Hex:%X OC:%o intchar=%d\n",FK2,FK2,FK2,(char)FK2);
printw("F3  Dec:%d Hex:%X OC:%o intchar=%d\n",FK3,FK3,FK3,(char)FK3);
printw("F4  Dec:%d Hex:%X OC:%o intchar=%d\n",FK4,FK4,FK4,(char)FK4);
printw("F5  Dec:%d Hex:%X OC:%o intchar=%d\n",FK5,FK5,FK5,(char)FK5);
printw("F6  Dec:%d Hex:%X OC:%o intchar=%d\n",FK6,FK6,FK6,(char)FK6);
printw("F7  Dec:%d Hex:%X OC:%o intchar=%d\n",FK7,FK7,FK7,(char)FK7);
printw("F8  Dec:%d Hex:%X OC:%o intchar=%d\n",FK8,FK8,FK8,(char)FK8);
printw("F9  Dec:%d Hex:%X OC:%o intchar=%d\n",FK9,FK9,FK9,(char)FK9);
printw("F10 Dec:%d Hex:%X OC:%o intchar=%d\n",FK10,FK10,FK10,(char)FK10);

printw("\nShift+F1  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK1,SFK1,SFK1,(char)SFK1);
printw("Shift+F2  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK2,SFK2,SFK2,(char)SFK2);
printw("Shift+F3  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK3,SFK3,SFK3,(char)SFK3);
printw("Shift+F4  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK4,SFK4,SFK4,(char)SFK4);
printw("Shift+F5  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK5,SFK5,SFK5,(char)SFK5);
printw("Shift+F6  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK6,SFK6,SFK6,(char)SFK6);
printw("Shift+F7  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK7,SFK7,SFK7,(char)SFK7);
printw("Shift+F8  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK8,SFK8,SFK8,(char)SFK8);
printw("Shift+F9  Dec:%d Hex:%X OC:%o intchar=%d\n",SFK9,SFK9,SFK9,(char)SFK9);
printw("Shift+F10 Dec:%d Hex:%X OC:%o intchar=%d\n",SFK10,SFK10,SFK10,(char)SFK10);


OSTANOV();

} 