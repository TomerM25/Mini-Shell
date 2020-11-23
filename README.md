# Mini-Shell

**********Description**********

Creating a shell and practice the system calls fork,wait.
The program include infinity while loop that every iteration get the command from the.
Promt, every command will store into dynamic 2d chars array.
The run function will get the array and run it using execvp so / chdir (cd command).
The program count how many cmd inserted and the lengths of the cmds.
The program handle commands that required execvp and chdir system call.
Every loop the memory will set free for the next data.


**********Functions**********

- void sig_handler(int signo);
- void printPrompt();
- void freeMemory(char **arr, int count);
- int checkIfSigns(char **commands, int count, char* x);
- void execute(char **commands, int* Cmd_Counter, int* Letters_Counter, int count);
- void executePipe(char **commands, int pipeLocation, int count, bool ifPipe, char* fileName, int redirType);
- void executeRedirection(char **commands, int redirLocation, int redirType, int count);


**********How to run**********

 ./ \<app\>


**********Input**********

basic shell commands.


**********Output**********

shell output,and error message if an error occurred.

