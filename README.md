# concurrent-command-custom-path-shell

#Project Description.
In This project I had developed custom shell which would work like the original shell, but with some customizations.
I implemented two modes. one is Interactive and Other is Batch mode.
After compiling the C program, the object file is created lets say ./a.out.
If i give only  the object file then the shell will run on Interactive mode where as if i specify batch file beside object file, then shell enters into batch mode.
So that means based on the input specified the shell enters into batch or interactive.
One more important thing is, we used execv() system call. If we want to execute the command, a path directory is set and along with arguments a path directory is sent to execv(path,args) system call inorder to execute the command. 
In Interactive mode:
Initially the path would be empty. At that no single command is executed. Only after adding the path to the directory, the command will be executed only after setting directory path.
I have implemented it like if there no command specified, it continues to take the input again.
This shell can accept multiple commands in the same line by separating with the semicolon';'.
This will continue until it sees wait command. If the command is in the middle of series, then, quit command is executed at last after executing all the succeeding commands. 
Every time child process is created, and the command is executed in child process. The parent process will not terminate until the child terminates.
In batch mode:
Here we initially set the path directory and execute all the commands present in the file until we saw the CTRL_D or EOF or quit command.
In batch mode also, the shell will execute the multiple commands separated by semi colon sequentially.
In this mode a prompt is not asked every time, It executes all the commands present it.


#Compiling instruction.
I have attached MAKE FILE also. So, you just need to run MAKE command in the shell.
For executing the Interactive mode - simple type "make run".
For executing the Batch mode - type "make run-batch".
