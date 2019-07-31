This file is a README for bilshell
Author: Elif Gülşah Kaşdoğan 
Version: 1
Date: 2/3/19
*You can compile and provide an executable named bilshell by using provided 
Makefile and using "make" command.
*Once you obtain the executable, you can run it by typing ./bilshell
*The program asks the size N which will be the number of bytes processed at a time. 
You can reenter the value when you are in batch mode.
*The program has two modes: Batch and interactive. Program starts in interactive 
mode as default, to activate the batch mode use command "bilshell N inputfilename.txt" 
command.
It is a built in command and supported by the bilshell. Example would be like this:

	current directory: /home/gulsah/Desktop
 	 bilshell-$ bilshell 10 example.txt

This command will read example.txt line by line and execute each line as a command. Please provide one command per line otherwise it will only read the first command you write. 
You can type a different N value for each batch statement, they are not dependent.

*You can use Linux commands with bilshell.
*You can use "cd" to change directory, "exit" for exiting.
*There are two additional builtin commands producer and consumer, producer produces M random characters and displays them. Consumer reads M characters from stdin. 
*You can use compound arguments. Pipe symbol | is supported in bilshell. 
*You can use one pipe per statement. 
It has the following form:

	producer 1000|consumer 1000

Another example:

	ps aux|sort

*Have a great time while using the bilshell!
