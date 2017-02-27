#!/usr/bin/env bash
# 
# Made by ValeriyKr (s207214)
# 
# vim: set shiftwidth=8:
#

task_list='1. Print working directory
2.
3.
4.
5.
6. exit'


log() {
        echo "$1" 2>&1 | tee -a ~/lab1_err 1>&2
        if [ $? -ne 0 ]
        then 
                echo "Cannot write the log: " ~/lab1_err "!" 1>&2
        fi
}


nop() {
        return
}


do_pwd() {
        pwd
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
                nop
                ;;
        3)
                nop
                ;;
        4)
                nop
                ;;
        5)
                nop
                ;;
        6)
                do_exit
                ;;
        # Anything else
        *)
                log "Incorrect command: $cmd"
                ;;
        esac
        printf "\n"
done
