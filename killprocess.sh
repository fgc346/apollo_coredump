#!/bin/bash

# 杀死特定进程的脚本命令

NAME=$1
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "-----------------"
for id in $ID
do
	kill -9 $id
	echo "kill $id"
done
echo "------------------"
