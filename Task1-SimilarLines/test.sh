#!/bin/bash

PROJECT=$1
DIRECTORY=$2

CORRECT=0
WRONG=0
MEM_LEAKS=0

for f in "${DIRECTORY}/"*.in
do

  FILE=${f%.in}
  FILE=${FILE#"${DIRECTORY}/"}

  TMP_OUT=$(mktemp)
  TMP_ERR=$(mktemp)

  echo -e Processing: ${FILE}
  echo -e "Valgrind looking for memory leaks... "
  
  valgrind --error-exitcode=69 --leak-check=full --show-leak-kinds=all\
  --errors-for-leak-kinds=all -q "./${PROJECT}" <"$f" &>/dev/null
  
  VALGRIND_EXIT=$?
 
  
  if [ $VALGRIND_EXIT -eq 0 ];
  then
	echo -e "Valgrind exit code: $VALGRIND_EXIT, no memory leaks"
  else
	MEM_LEAKS=$((MEM_LEAKS+1))
	echo -e "Valgrind exit code: $VALGRIND_EXIT, MEMORY LEAK!!!"
  fi
  
  echo -e "\nTesting program without valgrind..."
  
  time("./${PROJECT}" <"$f" >$TMP_OUT 2>$TMP_ERR)
  
  diff $TMP_OUT ${f%.in}.out >/dev/null
  OUT_EXIT=$?
  
  diff $TMP_ERR ${f%.in}.err >/dev/null
  ERR_EXIT=$?

  if [ $OUT_EXIT -eq 0 ] && [ $ERR_EXIT -eq 0 ];
  then
	CORRECT=$((CORRECT+1))
    echo -e "\nCORRECT!!!"
    echo -e "------------------------------"
  else
	WRONG=$((WRONG+1))
	if [ $OUT_EXIT -ne 0 ];
	then
	  echo -e "\nWRONG STDOUT!!!"
	fi
	
	if [ $ERR_EXIT -ne 0 ];
	then
	  echo -e "WRONG STDERR!!!"
	fi
    echo -e "------------------------------"
  fi
  rm -f $TMP_OUT $TMP_ERR

done

echo -e "Tests passed:" $CORRECT
echo -e "Tests failed:" $WRONG
echo -e "Memory leaks:" $MEM_LEAKS
