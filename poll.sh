#!/usr/bin/bash

while true; do
    echo "polling..."
    if resman; then
        resman -r 'first resman queued job ever :)' -x 'sh ./tests.sh'
        break
    else
        sleep 60
    fi
done
