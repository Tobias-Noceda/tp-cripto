#!/bin/bash

./examples.sh -n | while read example
do
    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=all \
        --error-exitcode=1 \
        --track-origins=yes \
        $example &> valgrind_output.log

    if [ $? -ne 0 ]
    then
        echo Memory leak detected
        echo $example
        cat valgrind_output.log
    fi
done
