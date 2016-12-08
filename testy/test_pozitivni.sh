#!/bin/sh

cd pozitivni_testy/

FILES=`find . -type f -name \*` 

cd ../..

GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)
RED=$(tput setaf 1)

for path in $FILES; do
   path=`echo $path` 
   path=`echo $path | sed 's/^.\//testy\/pozitivni_testy\//g'` 
   $r./prekladac $path >testy/vystup_pozitiv.txt 2>&1
   ret=$?
   if [ cat vystup_pozitiv.txt -eq "OK" ]; then
      printf "$path - ${GREEN}OK\n${NORMAL}"
   else
     printf  "$path - ${RED}NOT OK\n${NORMAL}" 
   fi

   > vystup_pozitiv.txt
done



 
