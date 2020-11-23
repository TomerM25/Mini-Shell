/*
Tomer Maalumi,
203284872
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>


void sig_handler(int signo);
void printPrompt();
void freeMemory(char **arr, int count);
int checkIfSigns(char **commands, int count, char* x);
void execute(char **commands, int* Cmd_Counter, int* Letters_Counter, int count);
void executePipe(char **commands, int pipeLocation, int count, bool ifPipe, char* fileName, int redirType);
void executeRedirection(char **commands, int redirLocation, int redirType, int count);

int main()
{
	int Cmd_Counter = 0;
	int Letters_Counter = 0;
	
	while(true)
	{
		signal(SIGINT, sig_handler);
		signal(SIGCHLD, sig_handler);
		printPrompt();												//call to function that print the prompt line.
	
		char userS[510];
		fgets(userS, 510, stdin);									//get the command from the user.

		if (strcmp(userS, "\n") != 0)								//if we dont get a command.
		{
			char temp[510];
			strcpy(temp,userS);										//copy the command.
		
			char *ptr = strtok(temp," \n\"");						//breaking the user command to tokens.
			int count = 1;
		
			while(ptr != NULL)										//counting the number of words in user command.
			{
				count++;
				ptr = strtok(NULL," \n\"");
			}
			
			char **commands = (char**)malloc(sizeof(char*)*count);	//allocate memory for 2d array.
			assert(commands != NULL);
			
			ptr = strtok (userS, " \n\"");
			int i=0;
		
			while(ptr != NULL)										//set each command in a different cell.
			{
				commands[i] = (char*)malloc(strlen(ptr));
				strcpy(commands[i],ptr);
				ptr = strtok(NULL," \n\"");
				i++;
			}
			commands[i] = NULL;
			
			if (strcmp(commands[0], "done") == 0)					//if the user put 'done' command.
			{
				printf("Num of cmd: %d \n", Cmd_Counter);			//print the number of commands we get from the user.
				printf("Cmd length: %d \n", Letters_Counter);		//prints the sum of all the letters of the commands.
				printf("Bye ! \n");
				
				freeMemory(commands, count);						//free allocate memory.
				break;												//change the flag to exit the loop and end the program.
			}
			
			else if (checkIfSigns(commands, count, ">") != -1)		//check if '>' in the command and if there is go to 'executeRedirection' function.
			{
				int redirLocation = checkIfSigns(commands, count, ">");
				Cmd_Counter = Cmd_Counter + 1;
				Letters_Counter = Letters_Counter + strlen(commands[0]);
				executeRedirection(commands, redirLocation, 1, count);
				freeMemory(commands, count);						//free allocate memory.
			}
			
			else if (checkIfSigns(commands, count, ">>") != -1)		//check if '>>' in the command and if there is go to 'executeRedirection' function.
			{
				int redirLocation = checkIfSigns(commands, count, ">>");
				Cmd_Counter = Cmd_Counter + 1;
				Letters_Counter = Letters_Counter + strlen(commands[0]);
				executeRedirection(commands, redirLocation, 2, count);
				freeMemory(commands, count);						//free allocate memory.
			}
		
			else if (checkIfSigns(commands, count, "<") != -1)		//check if '<' in the command and if there is go to 'executeRedirection' function.
			{
				int redirLocation = checkIfSigns(commands, count, "<");
				Cmd_Counter = Cmd_Counter + 1;
				Letters_Counter = Letters_Counter + strlen(commands[0]);
				executeRedirection(commands, redirLocation, 3, count);
				freeMemory(commands, count);						//free allocate memory.
			}
		
			else if (checkIfSigns(commands, count, "2>") != -1)		//check if '2>' in the command and if there is go to 'executeRedirection' function.
			{
				int redirLocation = checkIfSigns(commands, count, "2>");
				Cmd_Counter = Cmd_Counter + 1;
				Letters_Counter = Letters_Counter + strlen(commands[0]);
				executeRedirection(commands, redirLocation, 4, count);
				freeMemory(commands, count);						//free allocate memory.
			}
			
			else if (checkIfSigns(commands, count, "|") != -1)
			{							//check if there is a pipe in the command and if there is go to 'executePipe' function.
				
				int pipeLocation = checkIfSigns(commands, count, "|") + 1;
				Cmd_Counter = Cmd_Counter + 1;
				Letters_Counter = Letters_Counter + strlen(commands[0]);
				executePipe(commands, pipeLocation, count, false, NULL, -1);
				freeMemory(commands, count);						//free allocate memory.
			}
			
			else
			{
				execute(commands, &Cmd_Counter, &Letters_Counter, count);
																	//call to function that executes the commands we received from the user.
				freeMemory(commands, count);
			}
		}
	}
	
	return 0;
}

void sig_handler(int signo)
{
	signal(SIGINT, sig_handler);
	signal(SIGCHLD, sig_handler);
	while(waitpid(-1,NULL,WNOHANG)>0);
}

void printPrompt()													//function that print the prompt line.
{
	struct passwd *pws;
	pws = getpwuid(0);										//get the user name.

	char cwd[256];
	getcwd(cwd, sizeof(cwd));								//get the current file directory.

	printf("%s@%s>", pws->pw_name, cwd);
}

void freeMemory(char **arr, int count)								//function that free the allocate memory of array.
{
	for(int i = 0; i < count; i++)
	{
		free(arr[i]);
	}
	free(arr);
}

int checkIfSigns(char **commands, int count, char* x)				//function that check if there is a pipe in the command.
{
	for(int i = 0; i < count-1 ; i++)
	{
		if(strcmp(commands[i], x) == 0)
		{	
			return i;
		}
	}
	return -1;
}

void execute(char **commands, int *Cmd_Counter, int *Letters_Counter, int count)
{																	//function that executes the commands we received from the user.
	signal(SIGINT, sig_handler);
	signal(SIGCHLD, sig_handler);
	bool background = false;
	
	if (strcmp(commands[0], "cd") == 0)								//'cd' command change the folder direction.
	{
		chdir(commands[1]);
	}
	
	else
	{
		int status = 0;
		*Cmd_Counter = *Cmd_Counter + 1;
		*Letters_Counter = *Letters_Counter + strlen(commands[0]);
		
		if (checkIfSigns(commands, count, "&") == count - 2)
		{
			background = true;
			commands[count-2] = NULL;
		}
	
		pid_t process = fork();										//creating a new process.
				
		if (process == -1)											//print error if the fork not succeed.
		{
			printf("error in fork \n");
		}
		
		else if (process == 0)										//son process execute the command.
		{
			if (execvp(commands[0], commands) == -1)				//check if the command is not correct and print error or execute the command. 
			{
				printf("%s: command not found \n", commands[0]);
				exit(1);
			}
		}
		
		else														//parent process is waiting for son to end.
		{
			if (background)
			{
				return;
			}
			
			wait(&status);
			if (!WIFEXITED(status))
			{
				return;
			}
		}
	}
}

void executePipe(char **commands, int pipeLocation, int count, bool ifPipe, char* fileName, int redirType)
{																	//function that executes commands with pipe that we received from the user.
	if (pipeLocation == 1)
	{
		return;
	}
	
	int status = 0;
	int fds[2];														//creating a pipe.
	pid_t leftProcess, rightProcess;
	
	if (pipe(fds) < 0)
	{
		printf("pipe error");
		exit(1);
	}
	
	if(commands[pipeLocation] == NULL)
	{
		printf("%s: cannot access '%s': No such file or directory \n", commands[0], commands[pipeLocation-1]);
		return;	
	}
																	//split the command array for 2 new arrays and allocate memory.
	char **leftCommand = (char**)malloc((pipeLocation)*sizeof(char*));
	char **rightCommand = (char**)malloc((count - pipeLocation)*sizeof(char*));
	assert(leftCommand != NULL);
	assert(rightCommand != NULL);
	

	for(int i = 0; i < pipeLocation-1 ; i++)						//copy left command to a new array.
	{
		leftCommand[i] = (char*)malloc(strlen(commands[i])*sizeof(char)); 
		assert(leftCommand[i] != NULL);
		
		strcpy(leftCommand[i], commands[i]);
	}
	leftCommand[pipeLocation-1] = NULL;
	
	for(int j = pipeLocation, i = 0; j < count-1; i++, j++)			//copy right command to a new array.
	{
		rightCommand[i] = (char*)malloc(strlen(commands[j])*sizeof(char));
		assert(rightCommand[i] != NULL);
		
		strcpy(rightCommand[i], commands[j]);
	}
	rightCommand[count-pipeLocation-1] = NULL;

	
	leftProcess = fork();
		
	if (leftProcess == -1)											//print error if the fork not succeed.
	{
		printf("error in fork \n");
	}
	
	else if (leftProcess == 0)										//left son process execute the command.
	{
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		
		if (execvp(leftCommand[0], leftCommand) == -1)				//check if the command is not correct and print error or execute the command. 
		{
			exit(1);
		}
	}
	
	int fileFd;
	rightProcess = fork();
	
	if (rightProcess == -1)											//print error if the fork not succeed.
	{
		printf("error in fork \n");
	}	
	
	else if (rightProcess == 0)										//right son process execute the command.
	{
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		close(fds[1]);
		
		if(ifPipe)													//if is a pipe in the command.
		{
			fileFd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0600);
			if(fileFd == -1)
			{
				perror("File open failed \n");
				exit(1);
			}
			
			else
			{
				dup2(fileFd, STDOUT_FILENO);
			}
		}
		
		if (execvp(rightCommand[0], rightCommand) == -1)			//check if the command is not correct and print error or execute the command. 
		{
			exit(1);
		}
	}
	
	else															//parent process is waiting for 2 sons to end.
	{
		if(ifPipe)
		{
			close(fileFd);
		}
		close(fds[0]);
		close(fds[1]);
		wait(&status);
		wait(&status);
		freeMemory(leftCommand, pipeLocation);						//free allocate memory.
		freeMemory(rightCommand, count-pipeLocation);				//free allocate memory.
	}
}

void executeRedirection(char **commands, int redirLocation, int redirType, int count)
{																	//function that executes commands with redirections that we received from the user.		
	if (redirLocation == 0)
	{
		return;
	}
	
	pid_t process;	
	int fd, status = 0;
	char fileName[510];
	
	if(commands[redirLocation+1] == NULL)
	{
		printf("%s: cannot access '%s': No such file or directory \n", commands[0], commands[redirLocation]);
		return;	
	}
	
	
	strcpy(fileName, commands[redirLocation+1]); 					// save the file name.

	
	int pipeLocation = checkIfSigns(commands, redirLocation+1, "|");	//check if there is a pipe.
	if(pipeLocation != -1)
	{
		bool ifPipe = true;
		executePipe(commands, pipeLocation, count, ifPipe, fileName, redirType);
		return;
	}
	
	else
	{
		commands[redirLocation] = NULL;
		
		process = fork();											//creating a new process.
				
		if (process == -1)											//print error if the fork not succeed.
		{
			printf("error in fork \n");
			exit(1);
		}
		
		else if(process == 0)										//son process execute the command.
		{		
			if(redirType == 1 || redirType == 4)
			{
				fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0600);
				if(fd == -1)
				{
					perror("File open failed \n");
					exit(1);
				}
			}
			
			else if(redirType == 2)
			{
				fd = open(fileName, O_WRONLY | O_CREAT | O_APPEND, 0600);
				if(fd == -1)
				{
					perror("File open failed\n");
					exit(1);
				}
			}
			
			else if(redirType == 3)
			{
				fd = open(fileName, O_RDONLY, 0600);
				if(fd == -1)
				{
					fprintf(stderr,"File open failed\n");
					exit(1);
				}
			}
			if(redirType == 1 || redirType == 2)
			{
				dup2(fd, STDOUT_FILENO);
			}
		
			else if (redirType == 3)
			{
				dup2(fd, STDIN_FILENO);
			}
		
			else if (redirType == 4)
			{
				dup2(fd, STDERR_FILENO);
			}
		
			if (execvp(commands[0], commands) == -1)					//check if the command is not correct and print error or execute the command. 
			{
				printf("%s: command not found \n", commands[0]);
				exit(1);
			}

			close(fd);
		}

		else
		{
			wait(&status);
			if (!WIFEXITED(status))
			{
				return;
			}
		}
	}
}