#!/usr/bin/env bash
# 
# Made by ValeriyKr (s207214)
# 
# vim: set shiftwidth=8:
#

exec 3>&2 2>>$HOME/lab1_err


read_filename() {
        old_IFS="$IFS"
        IFS=''
        read filename
        IFS="$old_IFS"
        echo "$filename"
}


cmd=''
while {
        # Indents with tabs! (Because of bash)
        cat <<- EOF
		1. Print working directory
		2. Print working directory contents
		3. Create directory
		4. Add writing permissions to directory
		5. Take writing permissions from directory
		6. exit
		Enter a command:
		EOF
        # Normal indents
        read cmd
}
do
        case $cmd in
        # Just <Return> pressing
        '')
                true
                ;;
        # Commands
        1)
                pwd -P || echo 'Error: pwd' >&3
                ;;
        2)
                ls || echo 'Error: ls' >&3
                ;;
        3)
                echo 'Enter directory name:'
                dirname="$(read_filename)"
                mkdir -- "$dirname" || echo 'Error: mkdir' >&3
                ;;
        4)
                echo 'Enter directory name:'
                dirname="$(read_filename)"
                chmod ugo+w -- "$dirname" || echo 'Error: chmod +w' >&3
                ;;
        5)
                echo 'Enter directory name:'
                dirname="$(read_filename)"
                chmod ugo-w -- "$dirname" || echo 'Error: chmod -w' >&3
                ;;
        6)
                break
                ;;
        # Anything else
        *)
                echo 'Incorrect command' >&3
                ;;
        esac
        echo
done
echo 'Bye'
