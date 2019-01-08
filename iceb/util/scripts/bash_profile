# ~/.bash_profile: executed by bash(1) for login shells.

# see /usr/share/doc/bash/examples/startup-files for examples.

# the files are located in the bash-doc package.



# the default umask is set in /etc/login.defs

#umask 022



# include .bashrc if it exists

if [ -f ~/.bashrc ]; then

    . ~/.bashrc

fi



# set PATH so it includes user's private bin if it exists

if [ -d ~/bin ] ; then

    PATH=~/bin:"${PATH}"

fi



BASH_ENV=$HOME/.bashrc

#MYHOST=ho am i
MYHOST=`who am i`
export BASH_ENV



if [ -z "$DISPLAY" -a -x /usr/local/bin/usermenu ]; then

  if [ "$TERM" = "kron-kg" ]; then

    mapchan -f koiumain.map -s /usr/local/bin/usermenu

  else

    /usr/local/bin/usermenu

  fi

  exit

fi
