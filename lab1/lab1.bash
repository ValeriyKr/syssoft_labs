#!/usr/bin/env bash
# 
# Made by ValeriyKr (s207214)
# 
# vim: set shiftwidth=8:
#

exec 3>&2 2>>$HOME/lab1_err
IFS=''
while cat <<- EOF
				1. Print working directory
				2. Print working directory contents
				3. Create directory
				4. Add writing permissions to directory
				5. Take writing permissions from directory
				6. exit
				Enter a command:
		EOF
        read cmd
do
        case $cmd in
        '') true ;;
        1) pwd -P || echo 'Error: pwd' >&3 ;;
        2) ls || echo 'Error: ls' >&3 ;;
        3) echo 'Enter directory name:'
                read dirname
                mkdir -- "$dirname" || echo 'Error: mkdir' >&3 ;;
        4|5) read -p $'Enter directory name:\n' dirname 2>&1
                chmod -- ugo$(tr 45 +- <<<$cmd)w "$dirname/" || echo 'Error: chmod' >&3 ;;
        6) break ;;
        *) echo 'Incorrect command' >&3 ;;
        esac
        echo
done
