#!/bin/bash

size_flag=0

while getopts ":s" option; do
	case "$option" in
		s) size_flag=1
			;;
		?) printf "Error: Unknown option '-%s'.\n" $OPTARG >&2
			exit 1
			;;
	esac
done

if [ $size_flag -eq 1 ]; then
	echo "Yay, size_flag"
fi

declare -a file_array

index=0
shift "$((OPTIND-1))"
for f in $@; do
	file_array[$index]=$f
	(( ++index ))
done

echo "${file_array[*]}"
exit 0