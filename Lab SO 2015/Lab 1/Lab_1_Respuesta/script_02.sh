#!/bin/sh

if [ $1 -eq 1 ]; then
 
        pwd
        exit 1
fi

if [ $1 -eq 2 ]; then
 
        ls
        exit 1
fi

if [ $1 -eq 3 ]; then
 
        date
        exit 1
fi

if [ $1 -eq 4 ]; then
 
        whoami
        exit 1
fi

