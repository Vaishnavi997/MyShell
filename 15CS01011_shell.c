/*---------------------------------------------------------------------------------------------15CS01011( SHELL )------------------------------------------------------------------------------------------*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <fcntl.h>
#include<dirent.h>
#include <termios.h>
static char* currentDirectory;

pid_t pid;
 
#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")

#define COLOR_NONE "\033[m"
#define COLOR_RED "\033[1;37;41m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_GREEN "\033[0;32;32m"


struct job {
    int id; 	
    char job_name[10];
    pid_t pid;
    char status[20];	 
};
 
struct job arr_job[20];

int count=0;
char history[10][MAXCOM + 1];
int history_count = 0;


void init_shell()
{
    clear();
    char* username = getenv("USER");
    printf("\nNavigating to MY TERMINAL : @15CS01011");
    printf("\n");
    sleep(2);
    clear();
}

/*Input*/ 
int readInput(char* str)
{
    char* buf;
    buf = readline("\033[35m\033[1m~$\033[m\033[m ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
 
void printDir()
{
    char cwd[1024];
    char cwd2[1024];
    gethostname(cwd2,sizeof(cwd2));
    getcwd(cwd, sizeof(cwd));
    printf("\n\033[35m\033[1m%s\033[m\033[m:%s",cwd2, cwd);
}

/*Jobs*/ 
void printfn(){
	int i;
	printf(COLOR_GREEN"\n  Background Processes: "COLOR_NONE);
	printf("\n  ID\tPID\t COMMAND\t STATUS");
	for(i=0;i<count;i++){
		//printf("in for");
		printf("\n  [%2d ]\t%4d\t%7s%15s",arr_job[i+1].id,arr_job[i+1].pid,arr_job[i+1].job_name,arr_job[i+1].status);	
	}
}
 
/*Printing History*/ 
void print_history(char history[][MAXCOM + 1], int history_count) {
	if (history_count == 0) {
		printf("No history yet");
		return;
	}

	int i, j = 10;
	for (i = history_count; i > 0 && j > 0; i --, j--) {
		printf("%4d\t%s\n", i, history[i % 10]);
	}
}


/*Execution*/ 
void execute(char **args, int background){	 
	 int err = -1;
	 
	if((pid=fork())<0){
		 printf("Child process could not be created\n");
		 return;
	 }
	else if(pid==0){
		if (execvp(args[0],args)==err){
			printf("Command not found");
			//kill(getpid(),SIGKILL);
		}
		exit(0);
	}
	else{
		if (background == 0){
			 waitpid(pid,NULL,0);
			 return;
	 	}
		else{
			//printf("insiddee elseee back");
			printf("[%d] %d\n",count,pid);
		}	
	 }	 
}


/*I/O redirection*/ 
void fileIO(char * args[], char* inputFile, char* outputFile, int option){ 
	int err = -1;
	int fileDescriptor; // between 0 and 19, describing the output or input file
	if((pid=fork()) < 0){
		printf("Child process could not be created\n");
		return;
	}
	else if(pid==0){
		//printf("\033[35m\033[1m[ %d ]\033[m\033[m\n",getpid());
		// Option 0: output redirection	
		if (option == 0){
			//printf("Done!\n");	
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		// Option 1: input and output redirection
		}else if (option == 1){
			//printf("Here!\n");
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);		 
		}		
		if (execvp(args[0],args)==err){
			printf("err");
			kill(getpid(),SIGTERM);
		}
		exit(0);		 
	}
	waitpid(pid,NULL,0);
}

// Help command
void Help()
{
    puts("\nList of Commands supported:" COLOR_YELLOW
       "\n>cd dir/cd .. - to change the directory"
        "\n>ls ( /bin/ls )  - lists all the files present in that directory"
	"\n>history   - keeps record of commands till 10 . If 11th command is entered, 1st command is removed and goes on..."
        "\n>exit  - exits the shell with notification, if any backround process."
	"\n>xyz & - valid commands ending with &(Background processes)."
	"\n>jobs  - lists all the background processes" "\n>kill %num  - kills the process with jobID num" "\n>kill PID  - Kills the process with specified PID" "\n>kill all - to kill all background 		processes" "\n>!num  - to redo num command in history" "\n>i/o Redirection - for taking input and sending output by running xyz file"COLOR_NONE );
    return;
}
 
// Function to execute builtin commands
int commandFinder(char** parsed)
{
    char cwd[1024];char command[1024];char jobchar[1024];
    int NoOfOwnCmds = 7, i, switchcase = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];char* username;
    pid_t  ppid;
    int st;
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "ls";
    ListOfOwnCmds[4] = "Wrong";
    ListOfOwnCmds[5] = "jobs";
    ListOfOwnCmds[6] = "kill";	 

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchcase = i + 1;
            if(strcmp(parsed[0],"ls")==0){
		int j=0;
		while(parsed[j]!=NULL){
			if(strcmp(parsed[j],">")==0 || strcmp(parsed[j],"<") == 0 ){
				return 0;			
			}
			j++;
		}
	    }
	    	
            break;
        }
	else if(strcmp(parsed[0],"/bin/ls")==0) {switchcase=4;break;}
    }
 
    switch (switchcase) {
    case 1:
        
	for(i=0;i<count;i++){
			if(strcmp(arr_job[i+1].status,"running")==0){
				printf(COLOR_RED"\nWARNING: "COLOR_NONE);
				printf("There are background processes running... If you want to exit enter 'kill all' command to kill all background processes and then enter 'exit' to exit the shell.\n");
				return 1;			
			}
		}
	printf(COLOR_YELLOW"\nExiting.....See you soon!\n"COLOR_NONE);
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        Help();
        return 1;
    case 4:
	     
       if ((ppid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
           exit(1);
      	} 
       else if (ppid == 0) { if (execvp(*parsed, parsed) < 0) {     
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     	}
     else {                                  /* for the parent:      */
          while (wait(&st) != ppid)       /* wait for completion  */
               ;
     	}
        return 1;
    case 5:
	//printf("in Wrong");	
	return 0;
    case 6:
	//printf("in jobs");
	snprintf(jobchar,sizeof(jobchar),"ps -j --ppid %d",(int)getpid());	
	system(jobchar);
	printfn();
	return 1;
    case 7:
	//printf("in kill");
	//printfn();
	if((parsed[1][0]-'%')!=0 && strcmp(parsed[1],"all")!=0)
	{
		for(i=0;i<count;i++){
			if(arr_job[i+1].pid==atoi(parsed[1])){
				snprintf(jobchar,sizeof(jobchar),"kill %s",parsed[1]);	
				system(jobchar);
				strcpy(arr_job[i+1].status,"terminated");
				return 1;
			}
		}
	}
	else if(parsed[1][0]-'%'==0 && parsed[1][1]-'0'<=count){
		for(i=0;i<count;i++){
		if(parsed[1][1]-'0'==arr_job[i+1].id){
			//printf("%d",arr_job[i+1].pid);
			snprintf(jobchar,sizeof(jobchar),"kill -9 %d",arr_job[i+1].pid);	
			system(jobchar);
			strcpy(arr_job[i+1].status,"terminated");
			return 1;
		}
	}	
	}
	else if(strcmp(parsed[1],"all")==0){
		for(i=0;i<count;i++){
			snprintf(jobchar,sizeof(jobchar),"kill -9 %d",arr_job[i+1].pid);	
			system(jobchar);
			strcpy(arr_job[i+1].status,"terminated");
		}
		return 1;
	}
	else return 0;
   default:
	//printf("Commmand not found.");
        break;
    }
 
    return 0;
}
 
 
int processString(char* str, char** parsed)
{
 	int j=0;int i;
 
        for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
 
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }

	while(parsed[j]!=NULL){	
		if(strcmp(parsed[j],"&") == 0)
		{	return 3; 
		}
		else if(strcmp(parsed[j],"<") == 0 || strcmp(parsed[j],">") == 0)
		{	return 2; }
		j++;	
	}
    if (commandFinder(parsed))
        return 0;
	else{
		return 4;
	}
}

void historycheck(char *buffer,char** parsed){

	int x = parsed[0][1]- '0'; 
	int z = parsed[0][2]- '0'; 
	//printf("x=%d z=%d  historycount=%d",x,z,history_count);
	if(x>history_count) 
	{
		printf("\nNo Such Command in the history\n");
		strcpy(parsed[0],"Wrong command");
	} 
	else if (z!=-48) 
	{
		if(z!=1)
		{
	 		printf("\nNo Such Command in the history. Enter <=!9 (buffer size is 10 along with current command)\n");
	 		strcpy(buffer,"Wrong command");
		}
		if(x==-3 && z==1)
		{	 
			strcpy(buffer,history[history_count%10]); 
		}
		}
	else
	{
		if(x==-3 && z==1)//Checking for '!-1',ascii value of '!' is 33.
		{	
			strcpy(buffer,history[history_count%10]); 
		}
		else if(x==0) //Checking for '!0'
		{	 printf("Enter proper command");
			 strcpy(buffer,"Wrong command");
		}
		else if(x>=1) //Checking for '!n', n >=1
		{
			strcpy(buffer,history[x]);
		}
	}
 	return;
}


int main()
{
    char buffer[MAXCOM+1],jobstr[MAXCOM+1],*parsed[MAXLIST];
    int retval = 0;
    memset(buffer, 0, sizeof(buffer));
    int length, args_num;
    init_shell();
    char *args_aux[256];	

    while (1) {
		
		int i=0;int aux;int j=0;int background=0;
	        printDir(); //print directory
	        if (readInput(buffer)) //scan input string
	            continue;       
	 	strcpy(jobstr,buffer);
        	retval = processString(buffer,parsed);
		if(strcmp(buffer, "history") == 0){
			print_history(history,history_count);		
		}
		else if (parsed[0][0]=='!')
		{
			historycheck(buffer,parsed);
			retval = processString(buffer,parsed);
		}
		history_count ++;	
		memcpy(history[history_count% 10], jobstr, MAXCOM + 1);
		
				
		while(parsed[i]!=NULL){
			i++;	
		}
		if(retval==3){
			background=1;
			parsed[--i]=NULL;
			count++;
			memcpy(arr_job[count].job_name, jobstr, MAXCOM + 1);		
		}
		i=0;
		//printf("retval: %d",retval);
		if(retval == 2){////File i/o Redirection
			while ( parsed[j] != NULL){
				if ( (strcmp(parsed[j],">") == 0) || (strcmp(parsed[j],"<") == 0) || (strcmp(parsed[j],"&") == 0)){
					break;
				}
				args_aux[j] = parsed[j];
				j++;
			}

			while (parsed[i] != NULL){
				if (strcmp(parsed[i],"&") == 0){
					background = 1;
				}else if (strcmp(parsed[i],"<") == 0){
					aux = i+1;
					if (parsed[aux] == NULL || parsed[aux+1] == NULL || parsed[aux+2] == NULL ){
						printf("Not enough input arguments\n");
						return -1;
					}else{
						if (strcmp(parsed[aux+1],">") != 0){
							printf("Usage: Expected '>' and found %s\n",parsed[aux+1]);
							return -2;
							}
						}
					fileIO(args_aux,parsed[i+1],parsed[i+3],1);
					break;
				}else if (strcmp(parsed[i],">") == 0){
					if (parsed[i+1] == NULL){
						printf("Not enough input arguments\n");
						return -1;
					}
					fileIO(args_aux,NULL,parsed[i+1],0);
				}	
				i++;	
			}
			args_aux[i] = NULL;		
		}
	         
		else if (retval == 1 || retval == 3)
		{
			int err = -1;
	 		if((pid=fork())<0){
				 printf("Child process could not be created\n");
				 return 0;
	 		}	
			else if(pid==0){
				if (execvp(parsed[0],parsed)<0){
					printf("Command not found");
					//kill(getpid(),SIGKILL);
				}
				exit(0);
			}	
			else{
				if (background == 0){
					 waitpid(pid,NULL,0);
					 return 0;
	 			}
				else{
					arr_job[count].pid=pid;
					arr_job[count].id=count;
					strcpy(arr_job[count].status,"running");
					printf("[%d] %d\n",count,pid);
				}	
			 }		 
		}
		else if(retval!=0){
			printf(COLOR_RED"Invalid Command. Enter 'help' for help!\n"COLOR_NONE);
		}
	
        }
    return 0;
}

//Compiling: gcc mainshell.c -lreadline -o mainshell
//Put your arguments for the linker after instead of before your other arguments to gcc:
