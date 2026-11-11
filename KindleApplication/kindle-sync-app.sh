#!/bin/bash

sleep 2
{
    chmod +x /mnt/us/kindle-sync-app && /mnt/us/kindle-sync-app
    status=$?
    printf "\nstatus de l'execution %d\n" "$status"
} > /mnt/us/kindle-sync-app-log.txt 2>&1
