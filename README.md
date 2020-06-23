
//Compiling: gcc 15CS01011_shell.c -lreadline -o 15CS01011_shell

1) Prompt prints the current working directory. It also prints machine name and username information.

2) Read input from the command line using the readline function from the GNU readline library.(execvp is used which supports relative paths) 

3) I/O Redirection STDIN and STDOUT for the new processes by using < and > with on space between each string. For example, ./df < in > out runs 'df' using STDIN for the new process to infile and STDOUT for the new process to outfile.
	-> df,in out files are there in the folder. Check once! the df program takes input from in file and prints output in out file.

4) Commands with & at the end of the command line are kept in the background.

5) History of commands is recorded (max 10). This is a FIFO list. For example, if storing 10 commands, when the 11th is issued, you would be recording commands 2 through 11.
	
6) Previously issued command can be repeated by typing, !number, where number indicates which command to repeat. !-1 would mean to repeat the last command. !1 would mean repeat the command numbered 1 in the list of command returned by history.

7) commands supported :
	->cd changes the working directory. 
	->history prints the list of previously executed commands. 
	->exit terminates shell process.
	->ls lists the files in the cwd.
        ->history   - keeps record of commands till 10 . If 11th command is entered, 1st command is removed and goes on..."
        ->exit  - exits the shell with notification, if any backround process."
	->jobs  - lists all the background processes
        ->kill %num  - kills the process with jobID num
        ->kill PID  - Kills the process with specified PID
        ->kill all - to kill all background processes
        ->!num  - to redo num command in history
        ->./df < in > out  - for taking input and sending output by running xyz file ()
	->ls > out    - outputs(prints) into out file.

8) Enter the words or characters with atleat once space in between. 


