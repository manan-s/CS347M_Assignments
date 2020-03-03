# Assignment 1: Implementing a lightweight shell

Instructions for running: Compile and run the executable as following

$ gcc shell.c -o shell\
$ shell\
myshell>

This shell supports 4 commands {checkcpupercentage, checkresidentmemory, listFiles, sortFile}.\
'checkcpupercentage' and 'checkresidentmemory' require a PID as a space-separated argument.\
e.g.

myshell> checkresidentmemory 1\
576

For using 'sortFile', there must be a pre-existing file in same directory.\
Since 'sortFile' uses 'sort' as a subroutine, the text is sorted line-wise.
