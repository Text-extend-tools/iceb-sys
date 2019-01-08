#include <stdio.h>
#include <stdlib.h>

#include <buhl.h>
int main(int argc,char **argv,char **envp)
{
init_screen();
//icebstart(argc,argv);
//printw("111111111111111\n");
short d=1,m=1,g=2000;
dpm(&d,&m,&g,1);

//printw("%d.%d.%d\n",d,m,g);
//refresh();
//OSTANOV();
endwin();
return(0);
}
