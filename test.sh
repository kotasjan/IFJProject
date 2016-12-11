#!/bin/sh

GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)
RED=$(tput setaf 1)

printf "\nPozitivni testy:\n\n"

cd testy/pozitivni_testy/

FILES=`find . -type f -name \*` 

cd ../..

File="testy/vystup_pozitiv.txt"

for path in $FILES; do
   path=`echo $path` 
   path=`echo $path | sed 's/^.\//testy\/pozitivni_testy\//g'` 
   $r./prekladac $path >testy/vystup_pozitiv.txt 2>&1

   if [ "$(cat "$File")" == "OK" ] ; then
      printf "$path - ${GREEN}OK\n${NORMAL}"
   else
     printf  "$path - ${RED}NOT OK\n${NORMAL}" 
   fi

   > vystup_pozitiv.txt
done


printf "\nChybove testy:\n\n"

cd testy/tests/

FILES=`find . -type f -name \*` 

cd ../..

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

printf "\nCizi testy:\n\n"

chmod +x testy/testy_cizi/ifj_tester-master/ifj_test.py
python3 testy/testy_cizi/ifj_tester-master/ifj_test.py prekladac