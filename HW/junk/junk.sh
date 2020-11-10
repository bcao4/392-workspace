###############################################################################
# Author: Brandon Cao
# Date: 1/29/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: A simple bash script to provide the basic functionality of a recycle bin.
###############################################################################
#!/bin/bash

flag=0
readonly JUNKDIR=~/.junk

if [ ! -d "$JUNKDIR" ]; then
    #echo "$JUNKDIR does not exist"
    mkdir $JUNKDIR
fi

function show_help() {
	cat<<-HELPMESSAGE
	Usage: $(basename "~/$0") [-hlp] [list of files]
	   -h: Display help.
	   -l: List junked files.
	   -p: Purge all files.
	   [list of files] with no other arguments to junk those files.
	HELPMESSAGE
}

while getopts ":hlp" option; do
	case "$option" in
		h) show_help
			;;
		l) flag=2
			;;
		p) flag=3
			;;
		?) printf "Error: Unknown option '-%s'.\n" $OPTARG >&2
			show_help
			exit 1
			;;
	esac
done

#echo "$OPTIND OPTIND"
#echo "$# total arguments"

if [ $OPTIND -gt 2 ]; then 
	echo "Error: Too many options enabled." # More than 1 option arg given
	show_help
	exit 1
fi


if [[ $# -ge 2 && $OPTIND -ge 2 ]]; then 
	echo "Error: Too many options enabled." # Have option arg and a file name
	show_help
	exit 1
fi

if [ $flag -eq 2 ]; then # -l
	cd $JUNKDIR
	ls -lAF
	exit 0
fi


if [ $flag -eq 3 ]; then # -p
	shopt -s dotglob
	rm -r -f $JUNKDIR/*
	exit 0
fi

input_arg=${file_array[$index]}
if [[ $# -ge 1 && $OPTIND -lt 2 ]]; then # Only file args given
	#echo "FILES TO DELETE" 
	index=0
	for f in $@; do
		file_array[$index]=$f
		if [ -e ${file_array[$index]} ]; then
			#echo "${file_array[$index]} exists"
			mv "${file_array[$index]}" /home/cs392/.junk
		else
			echo "Warning: '${file_array[$index]}' not found."
		fi
		(( ++index ))
	done
	exit 1
fi


if [ $OPTIND -eq 1 ]; then # No option args given
	show_help
	exit 1
fi


#if [ $OPTIND -gt 2 ]; then 
#	echo "Error: Too many options enabled."
#	show_help
#fi

shift $((OPTIND-1))
exit 0

