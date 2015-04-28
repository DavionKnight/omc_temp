#!/bin/sh
find . -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.s" -o -name "*.S" > cscope.list
cscope -Rbkq -i cscope.list
ctags -R
