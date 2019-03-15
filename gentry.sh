#!/bin/bash

# sudo apt-get install inotify-tools


inotifywait --event "close_write" --monitor --recursive --exclude "$\." . |
while read -r directory events filename; do
    ./gentry "$directory$filename"
done
