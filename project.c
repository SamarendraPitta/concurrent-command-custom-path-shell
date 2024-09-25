#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAXIMUM_INP_LENGTH 512
#define MAX_TOKENS 32
#define MAX_LINES 500

char *Path_name_env[MAXIMUM_INP_LENGTH];
char *Commands[MAX_TOKENS];
int Num_Of_Directories = 0;
//below all the flags used for functionality
int flag = 0;
int quitfound;
int ping = 0;
int batch_mode = 0;

char* Trim_trailing_leading_spaces(char* String) {
    while (isspace((unsigned char)*String)) String++;
    char *last_pos = String + strlen(String) - 1;
    while (last_pos > String && isspace((unsigned char)*last_pos)) last_pos--;
    *(last_pos + 1) = '\0';
    
	return String;
}

void Current_Path() {
	if(Num_Of_Directories == 0){
		printf("there are no directories, please append for executing the commands");
	}
    int k=0;
    while(Num_Of_Directories>k){		
        printf("%s:", Path_name_env[k]);
        k++;
    }
	printf("\n");
}

void Divide_input(char *Input, char *Commands[]){
    //splitting the input based on semicolon to ee=xecute the multiple commands sequentially.
	//and storing it in the Commands, here Commands is a pointer which means directly writing into the memory location not to the variable.
	char *tok = strtok(Input, ";");	
    int i = 0;
    while (tok != NULL) {
        Commands[i++] = tok;
        tok = strtok(NULL, ";");
    }
    Commands[i] = NULL;
}

/*  -> This function executes commands of both batch and intercative mode.
	->intially im accepting the each command into this function. After that splitting the command and its argument.
	->Then, here i have implemented batch_mode flag because to print the command only in the case of batch mode,
	->so i kept one flag and this flag value is assigned when the batch mode is called.
	->creating the child process and executing the commands in it. and wwaiting the parent process for the child process to terminate.
*/
void Execute_Each_Command(char *Each_Command){
	
	char *args[MAX_TOKENS];
    char *tt = strtok(Each_Command, " ");	
    int y = 0;
    while (tt != NULL) {
        args[y++] = tt;
        tt = strtok(NULL, " ");
    }
    args[y] = NULL;
	
	char construct_Path[MAXIMUM_INP_LENGTH];
	
	if(batch_mode == 1){
		printf("%s\n",Each_Command);
	}
    if (strcmp(Each_Command, "quit") == 0) {
        exit(EXIT_FAILURE);
    }
	else if(strcmp(Each_Command, " ") == 0){
		printf("not valid command\n");
	}
	else{
		pid_t pid = fork();				//creating child process to execute the command.
		if (pid < 0) {
			perror("fork failed\n");
		} else if (pid == 0) {			//executing the commands in child process.
			for (int m = 0; m < Num_Of_Directories; m++) {
				snprintf(construct_Path, sizeof(construct_Path), "%s/%s", Path_name_env[m], args[0]);
				if (access(construct_Path, X_OK) == 0) {	//checking if command is executable or not.
					if (execv(construct_Path,args) == -1) {
						perror("Error executing command");
						exit(EXIT_FAILURE);
					}
					break;
				}
			}
			exit(1);
		}
		else{				//waiting for child processs to terminate the parent process.
			wait(NULL);
		}
	} 
}

int Execute_Commands(char *Commands[]){
	/*  ->This functions just checks the quit command. if found at last terminates the process.
		->If found at middle, a flag value is stored and then it is checked at last to terminate the process, after executing all the succeding commands.
	*/
	
	int count=0;
	while(Commands[count]!=NULL){
		count++;
	}
	for(int x = 0; x<count;x++){
		if(strcmp(Commands[x], "quit") == 0 && strcmp(Commands[x+1], "NULL") != 0){		//checking whether the quit if found in the middle.
			quitfound = 1;				//if found, then keeping the flag and then based on flag, executing quit command at last after executing all commands.
		}
		//searching for quit command and after that if no command is found then exiting the shell.
		else if(strcmp(Commands[x],"quit") == 0){
			printf("Exiting Shell\n");
			return 0;
		}
		else if(Commands[x] == NULL){
			printf("only space");
			continue;
		}
		else{
			Execute_Each_Command(Trim_trailing_leading_spaces(Commands[x])); 
		}
	}
	if(quitfound == 1){
		return 0;
	}
	return 1;
}

void removedirectory(char *New_Path_Directory){
	if(New_Path_Directory != NULL && New_Path_Directory[0] == '/'){
		for (int i = 0; i < Num_Of_Directories; i++) {
			if (strcmp(Path_name_env[i], New_Path_Directory) == 0) {
				free(Path_name_env[i]);				//if the directory found in the added directory list, then free(), means it is used remove the directory and allocate the space to next one.
				for (int a = i; a < Num_Of_Directories - 1; a++) {
					Path_name_env[a] = Path_name_env[a + 1];
				}
				Num_Of_Directories--;
			}
		}
	}
	else{
        printf("failed to add the directory to the path name environment, specified format if path - /bin.\n");
    }
}

void Append_directory_to_pathname_env(char *New_Path_Directory){
	if(Num_Of_Directories > MAXIMUM_INP_LENGTH){
        printf("pathname environment length exceeded.");
    }
    else if(New_Path_Directory != NULL && New_Path_Directory[0] == '/'){
        Path_name_env[Num_Of_Directories++] = strdup(New_Path_Directory);		//Duplicating the new directory and then assigning to Path_name_env by incrementing path count.
		flag = 1;			// This falg is used for executing the commands when the path is added.
		//Current_Path();
    }
    else{
        printf("failed to add the directory to the path name environment, specified format is path + /bin.\n");
    }
}

/*	-> Intercative mode is called and every propmpt is asked eventhough the input is given or not.
	-> Initially single command is not executed untill the path direvtory is added.
	-> implemented adding and removal of paths.
	-> then splitting the commands, if the commands are seperated by semicolon.
*/
void Call_Interactive_mode(){
	
	char Input_prompt[MAXIMUM_INP_LENGTH];
    while(1){
        printf("Interactive_Mode>");
        if(sizeof(Input_prompt)>MAXIMUM_INP_LENGTH){
			printf("Input_prompt length is too long");
			continue;
		}
		//below condition is if the input is null or when press CTRL+D that means END OF File. So it terminates from shell.
		if(fgets(Input_prompt, sizeof(Input_prompt), stdin) == NULL){
            break;
        }
		Input_prompt[strcspn(Input_prompt, "\n")] = '\0';		//This means setting the last charecter to null,simply like null terminator.
        if(strncmp(Input_prompt, "path", 4) == 0){
			char *Arg_pos = Input_prompt + 6;
            if(strcmp(Input_prompt, "path") == 0){		//When only the path command is enetered, then showing the current path.
                Current_Path();
            }
			else if(Input_prompt[5] == '-' && strlen(Arg_pos + 1)>0){	//Append the directory to path name environment
                removedirectory(Arg_pos+1);
            }
            else if(Input_prompt[5] == '+' && strlen(Arg_pos + 1)>0){	//delete directory from pathname environment
                Append_directory_to_pathname_env(Arg_pos+1);
            }
            else{
                printf("Invalid pathname");
            }
        }
        else if(strcmp(Input_prompt, "quit") == 0){
            exit(EXIT_FAILURE);
        }
		else if(strcmp(Input_prompt,"") == 0){
			continue;
		}
        else{
			if(flag == 1){
				Divide_input(Input_prompt, Commands);
				if(Execute_Commands(Commands) == 0){
					exit(EXIT_FAILURE);
				}
			}
			else{
				printf("First append the directory to the pathname environment \n");
			}
        }
    }
}

void Batch_mode(char filename[100]) {
    int file_discriptor;
    char Charecter;
    char line[MAXIMUM_INP_LENGTH];
    char *lines[MAX_LINES];
    int Lines_count = 0, Charecter_index = 0;
	batch_mode = 1;

    // Initialize the lines array to NULL
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = NULL;
    }

    file_discriptor = open(filename, O_RDONLY);		//opening the file, O_RDONLY means read mode. when open() is used O_RDONLY is standard format.
    if (file_discriptor == -1) {					//if file discriptor value is leas than 0 then file is not present or error in opening file.
        perror("Error opening file");
        return;
    }

    while (read(file_discriptor, &Charecter, 1) > 0) {
        if (Charecter == '\n') {
            line[Charecter_index] = '\0'; 
            if (Lines_count < MAX_LINES) {
                lines[Lines_count] = (char *)malloc((Charecter_index + 1) * sizeof(char));
                if (lines[Lines_count] == NULL) {
                    perror("Memory allocation failed");
                    close(file_discriptor);
                    return;
                }
                strcpy(lines[Lines_count], line);
                Lines_count++;
            } else {
                printf("Reached maximum line limit of %d\n", MAX_LINES);
                break;
            }
            Charecter_index = 0;
        } else if (Charecter_index < MAXIMUM_INP_LENGTH - 1) {
            // Ensuring line buffere should not be overflow.
            line[Charecter_index++] = Charecter;
        }
    }

    // handling if the last line does nt end with new line
    if (Charecter_index > 0) {
        line[Charecter_index] = '\0';
        if (Lines_count < MAX_LINES) {
            lines[Lines_count] = (char *)malloc((Charecter_index + 1) * sizeof(char));
            if (lines[Lines_count] == NULL) {
                perror("Memory allocation failed");
                close(file_discriptor);
                return;
            }
            strcpy(lines[Lines_count], line);
            Lines_count++;
        }
    }

    close(file_discriptor);

    for (int i = 0; i < Lines_count; i++) {
		ping++;						//This flag is used because first line of file specifies directory path.
									//Inorder to sedn path to append directory function and to execute only once i used ping flag.
		if(ping == 1){			
			Append_directory_to_pathname_env(Trim_trailing_leading_spaces(lines[i]));
		}
		else{
			Divide_input(lines[i], Commands);
			if(Execute_Commands(Commands)==0){
				exit(EXIT_FAILURE);
			}
		}
        free(lines[i]);		//freeing the memory allocated for each line.
    }
	batch_mode = 0;
}

int main(int arg_count, char *arg_vector[]){
	//based on the input, interactive or batch mode is called.
	//if batch file is specified then batch mode is on else intercative.
	
    if(arg_count == 2){
        Batch_mode(arg_vector[1]);
    }
    else if(arg_count == 1){
        Call_Interactive_mode();
    }
    else{
        printf("Please enter correct Input_prompt");
    }
    return 0;
}
