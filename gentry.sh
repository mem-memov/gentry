#!/bin/bash

# sudo apt-get install inotify-tools

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

inotifywait --event "close_write" --monitor --recursive --exclude "$\." . |
while read -r DIRECTORY EVENTS FILENAME; do
    CLASPATH="$DIRECTORY$FILENAME"
    ./gentry "$SCRIPTPATH" ${CLASPATH:1}
done
