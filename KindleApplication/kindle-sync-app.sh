#!/bin/bash

{
    chmod +x /mnt/us/kindle-sync-app && /mnt/us/kindle-sync-app
    status=$?
    printf "\nstatus de l'execution %d\n" "$status"
} > /mnt/us/kindle-sync-app-log.txt
