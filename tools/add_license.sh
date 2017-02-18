#!/bin/bash
# zdd / zddhub@gmail.com
if [ $# != 2 ]
then
    echo "Usage: add_license project_dir license_file"
    exit 
fi

for file in `find $1 -name "*.[h|c]*"`
do
    echo "/*************************************************************************" > temp
    while read line
    do
        echo " * $line" >> temp
    done < $2    
    echo "**************************************************************************/" >> temp
    cat $file >> temp 
    mv -f temp $file
done

for file in `find $1 -name "*.pr[o|i]"`
do
    echo "#-------------------------------------------------------------------------" > temp
    while read line
    do
        echo "# $line" >> temp
    done < $2
    echo "#-------------------------------------------------------------------------" >> temp
    cat $file >> temp
    mv -f temp $file
done
