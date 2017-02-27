#!/usr/bin/env bash
# 
# Made by ValeriyKr (s207214)
# 
# vim: set shiftwidth=8:
#

task_list='1. Print working directory
2. Print working directory contents
3. Create directory
4. Add writing permissions to directory
5. Take writing permissions from directory
6. exit'

LOGFILE="${HOME}/lab1_err"
WARN_MSG="An error occurred!"


nop() {
        return
}


do_pwd() {
        pwd 2>>"$LOGFILE" || echo "$WARN_MSG" 1>&2
}


do_ls() {
        ls 2>>"$LOGFILE" || echo "$WARN_MSG" 1>&2
}


do_mkdir() {
        echo "Enter directory name:"
        dirname="$(read_filename)"
        mkdir "$dirname" 2>>"$LOGFILE" || echo "$WARN_MSG" 1>&2
}


do_allow_writing() {
        echo "Enter directory name:"
        dirname="$(read_filename)"
        if [ -d "${dirname}" ]
        then
                chmod ugo+w "$dirname" 2>>"$LOGFILE" || echo "$WARN_MSG" 1>&2
        else
                echo "No such directory" | tee -a "$LOGFILE" 1>&2
        fi
}


do_deny_writing() {
        echo "Enter directory name:"
        dirname="$(read_filename)"
        if [ -d "${dirname}" ]
        then
                chmod ugo-w "$dirname" 2>>"$LOGFILE" || echo "$WARN_MSG" 1>&2
        else
                echo "No such directory" | tee -a "$LOGFILE" 1>&2
        fi
}


do_exit() {
        exit 0;
}


# Reads and returns filename
# Use double quotes when receive value!
# It can start with space or another symbol, exclude backslash
read_filename() {
        old_IFS="$IFS"
        IFS=''
        read filename
        IFS="$old_IFS"
        echo "$filename"
}


cmd=''
while [ 1 ]
do
        echo -e "$task_list"
        read cmd

        # End of input stream, i guess
        if [ $? -ne 0 ]
        then
                do_exit
        fi

        case $cmd in
        # Just <Return> pressing
        '')
                nop
                ;;
        # Commands
        1)
                do_pwd
                ;;
        2)
                do_ls
                ;;
        3)
                do_mkdir
                ;;
        4)
                do_allow_writing
                ;;
        5)
                do_deny_writing
                ;;
        6)
                do_exit
                ;;
        # Anything else
        *)
                echo "Incorrect command"
                ;;
        esac
        echo
done
