# iceB system
Bookkeeping suite of programs.

## Depends

In order to be able to use the programs it is necessary that the MySQL database be installed on your computer. The database markup procedure must be performed. If you are already working with the terminal version of the system, then you just need to build the programs and copy them to the directory from where they will be available to run for all users (usually /usr/local/bin).

In the "data/alx" directory there are configuration files.  
In the "data/doc" directory there are documentation files.  
These files are loaded into the database at the time of marking the new database.

## Parts

### IceB

Console version. Path iceb.

#### Build

To build the system from sources on your computer, libraries must be installed:  
libncurses-dev  
libncursesw-dev  
libmysqlclient-dev  
libc-dev  

If you want to build a system in the Linux operating system, it is enough to give the commands in the iceb directory:  
cmake .  
make  

#### Work

All necessary programs will be built and placed in the "buhg" directory. Copy them to the directory from where they will be available to run for all users (usually /usr/local/bin).

In addition to libraries, the following programs must be installed:  
mc (used when the system is running)  
gcc (compiler with programs)  
cmake (make file builder)  
mysql-server (mysql or MariaDB database)  

Since the program has a terminal interface, the console must be running before launching the program itself:  
xterm -e iceb_start  

### IceBw

Graphics version. Path icebw.

#### Build

To build the system from sources on your computer, libraries must be installed:  
libgtk-3-dev  
libmysqlclient-dev  
libc-dev  

If you want to build a system in the Linux operating system, it is enough to give the commands in the iceb directory:  
cmake .  
make  

#### Work

All necessary programs will be built and placed in the "buhg_g" directory. Copy them to the directory from where they will be available to run for all users (usually /usr/local/bin).

In addition to libraries, the following programs must be installed:  
mc (used when the system is running)  
gcc (compiler with programs)  
cmake (make file builder)  
mysql-server (mysql or MariaDB database)  

Run the program:  
icebw_start  

## About
https://github.com/zvezdochiot/iceb-sys  
Author: Белых А.И.  
Homepage: http://iceb.net.ua/  

