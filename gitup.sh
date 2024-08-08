#!/bin/sh

time=`date "+%Y-%m-%d_%H:%M:%S"`

git status
git add .
git commit -m "daily ${time}"
git push robsys master
echo "Finished Push!"
