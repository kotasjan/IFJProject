#!/bin/sh

cd testy/tests/

FILES=`find . -type f -name \*` 

cd ../..

GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)
RED=$(tput setaf 1)

for path in $FILES; do
   path=`echo $path` 
   path=`echo $path | sed 's/^.\//testy\/tests\//g'` 
   RET_CODE=`echo $path | grep -o -E '[0-9]+' | head -1 | sed -e 's/^\+//'`
   $r./prekladac $path >/dev/null 2>&1
   ret=$?
   if [ $RET_CODE -eq $ret ]; then
      printf "$path - ${GREEN}OK\n${NORMAL}"
   else
     printf  "$path ${RED}Ret code $ret - spravny $RET_CODE\n${NORMAL}" 
   fi
done

printf "-------------"
printf "\nCizi testy:\n\n"

./testy/testy_cizi/ifj_tester-master/ifj_test.py ../..



 
