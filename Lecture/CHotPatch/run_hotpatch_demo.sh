#!/bin/bash

make clean > /dev/null && make > /dev/null

# Run dl_demo in the background.
./dl_demo &
# Get the process id of dl_demo.
pid=$!

sleep 1
echo "Bash script: Copying my_dl2.so to my_dl.so."
cp my_dl2.so my_dl.so

# Wait for dl_demo to terminate before exiting this script.
wait $pid
