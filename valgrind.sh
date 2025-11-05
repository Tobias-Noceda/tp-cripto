#!/bin/bash

./examples.sh -n | while read example
do
    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=all \
        --error-exitcode=1 \
        --track-origins=yes \
        --log-file=valgrind.log \
        $example &> /dev/null

    if [ $? -ne 0 ]
    then
        echo -e "\e[31m==Memory leak detected==\e[0m"
        cat valgrind.log
        echo
    fi
done
