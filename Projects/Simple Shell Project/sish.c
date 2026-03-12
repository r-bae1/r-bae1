//rmb210005 & kky210002
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>

#define arg_limit 25
#define HISTORY_SIZE 100

//Function prototypes
int min(int x, int y);
void process_command(char *line, int *counter, int *start_index, char **history);
void add_to_history(char *line, int *counter, int *start_index, char **history);
int space_tokenize(char *line, char **myargs);
bool built_in_function(char **myargs, int *counter, int *start_index, char **history);
void execute(char **myargs);
void pipe_command(char *user_input, int piped_cmds, int *counter, int *start_index, char **history);
int count_cmds(char *user_input);
void clean_string(char **str);


int main(){

    char *line;
    size_t size = 0;
    
    char **history = (char**) malloc(HISTORY_SIZE * sizeof(char*));
    int counter = 0;
    int start_index = 0;

    while (1) {
        //char* line;
        printf("sish> ");
        getline(&line, &size, stdin);
        line[strlen(line) - 1] = '\0';      //Replace newline at end with \0
        
        add_to_history(line, &counter, &start_index, history);

        //count how many pipes are in the line
        int piped_commands = count_cmds(line);
        
        if (piped_commands <= 1)
        {
            //this is a non-piped command
            process_command(line, &counter, &start_index, history);;    
        }
        else
        {
            //this is a piped command
            pipe_command(line, piped_commands, &counter, &start_index, history);
        }
        
    }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Function definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//This function takes an input line and executes it
//Other parameters: &counter, &start_index, history
void process_command(char *line, int *counter, int *start_index, char **history) {
    char** myargs = (char**) malloc(arg_limit * sizeof(char*));
    
    int i;
    switch (space_tokenize(line, myargs))
    {
        case 0:         //this is a blank line
            return;
        case -1:        //this is an exit command
            for (i = 0; i < HISTORY_SIZE; i++)      //free history (myargs freed within space_tokenize)
            {
                history[i] = NULL;
                free(history[i]);
            }
            history = NULL;
            free(history);
            exit(EXIT_SUCCESS);                     //end program
        default:
            break;
    }
    
    if(!built_in_function(myargs, counter, start_index, history)) {     //if built in function, execute in method
        execute(myargs);        //if not a built in function, just execute from path
    }
    
    for (i = 0; i < arg_limit - 1; i++) {           //free myargs
        myargs[i] = NULL;
        free(myargs[i]);
    }
    
    myargs = NULL;
    free(myargs);
}


//simple function to get minimum of two values
int min(int x, int y) {
    if (x < y) {
        return x;
    }
    else {
        return y;
    }
}

//function to add a line to the history
//increment counter by 1, increase start_index and wrap-around if needed
void add_to_history(char *line, int *counter, int *start_index, char **history) {
    history[*counter % HISTORY_SIZE] = strdup(line);
    if (*counter >= HISTORY_SIZE) {         //once counter has exceeded HISTORY_SIZE, start_index will increase for each line
        *start_index = (*start_index + 1) % HISTORY_SIZE;
    }
    (*counter)++;
}


//Tokenize a string by spaces, updates myargs as it dereferences the pointer
//return 0 if blank line, return -1 if quit, return number of args otherwise
int space_tokenize(char *line, char **myargs) {
    char *token;
    char *str1;
    char *saveptr1;
    int i = 0;
    
    //check if blank line
    if (isspace(line[0]) != 0) {
        return 0;
    }
    
    // New line character
    if (strlen(line) <= 1) {
        return 0;
    }
    
    // Tokenize the string 
    for (i = 0, str1 = strdup(line); ; i++, str1 = NULL) {
        token = strtok_r(str1, " ", &saveptr1);
        if (token == NULL) {
            break;
        }
        else if ( (strcmp(token, "exit") == 0) && i == 0) {     //If 1st token is "exit", exit
            //free myargs
            for (i = 0; i < arg_limit; i++)
            {
                myargs[i] = NULL;
                free(myargs[i]);
            }
            
            myargs = NULL;
            free(myargs);
            
            return -1;  //different function then clears history, reduces arguement size for this function
        }
        
        myargs[i] = strdup(token);
        
    }
    
    return i;
}


//returns true and executes built in command if myargs describes a built in command
//returns false when not a built in command
//Other parameters: &counter, &start_index, history
bool built_in_function(char **myargs, int *counter, int *start_index, char **history) {
    int i = 0;
    
    //Built in Function: cd
    if (strcmp(myargs[0], "cd") == 0) {
        if (chdir(myargs[1]) == -1) {           //call chdir with myargs[1]
            printf("Error: failed to change directories.\n");
        }
        return true;
    }
    //Built in Function: history
    else if (strcmp(myargs[0], "history") == 0) {
        if (myargs[1] == NULL) {
            //no arguements: print all of the stored history elements; up to HISTORY_SIZE
            for (i = 0; i < min(*counter, HISTORY_SIZE); i++) {
                
                printf("%d %s\n", i, history[(i + *start_index) % HISTORY_SIZE]);
            }
        }
        else if (strcmp(myargs[1], "-c") == 0 ) {
	        //-c: free and clear the history array, reset counter and start_index
            for (i = 0; i < HISTORY_SIZE; i++)
    	    {
    	    	history[i] = NULL;
    	    	free(history[i]);
            }

	        history = NULL;
	        free(history);
	        
	        history = (char**) malloc(HISTORY_SIZE * sizeof(char*));
            *counter = 0;
            *start_index = 0;
        }
        else {
            //number: access element of history at this offset and execute as line
            bool isnum = 1;
            
            for (i = 0; i < strlen(myargs[1]); i++) {           //check if is a digit
                if (!(int)isdigit(myargs[1][i])) {
                    isnum = 0;
                    break;
                }
            }
            if (isnum) {
                //move to offset
                int offset = atoi(myargs[1]);
                offset = (*start_index + offset) % HISTORY_SIZE;
                if (*counter > HISTORY_SIZE) {
                    offset--;
                }
                if (history[offset] == NULL) {
                    printf("Error: invalid offset\n");
                    
                }
                else {
                    //run the command at this point in history
                    int piped_commands = count_cmds(history[offset]);
        
                    if (piped_commands <= 1)
                    {
                        process_command(history[offset], counter, start_index, history);;    
                    }
                    else
                    {
                        pipe_command(history[offset], piped_commands, counter, start_index, history);
                    }
                }
            }
            else {
                //not a proper history command
                printf("Error: invalid arguments.\n");
            }
        }
        //this was a built in function, even if it was not formatted properly
        return true;
    }
    else {
        //this is not a built in function
        return false;
    }
}

//fork and execute the command in path with the child
void execute(char **myargs) {
    int rc = fork();

    if (rc == 0) {          // child (new process)
        execvp(myargs[0], myargs);
        printf("Error: invalid command.\n");
        kill(getpid(), SIGKILL);
    }
    else {                  // parent (main)
        wait(NULL);
    }
    return;
}

//count the number of "|", add 1 and return this value
int count_cmds(char *user_input) {
    int i;
    int num_pipes = 1;
    for (i = 0; i < strlen(user_input); i++) {
        if (user_input[i] == '|') {
            num_pipes++;
        }
    }
    return num_pipes;
}

//take a piped command and its number of total commands and execute it
//Other parameters: &counter, &start_index, history
void pipe_command(char* user_input, int total_cmds, int *counter, int *start_index, char **history)
{
    int i;
    char* temptoken;
    temptoken = strtok_r(user_input, "|", &user_input);                 //get the first token
    clean_string(&temptoken);                                           //clean leading and ending spaces
    
    char **myargs = (char**) malloc(arg_limit * sizeof(char*));
    
    int left_pipe[2];
    
    int myargs_size = space_tokenize(temptoken, myargs);                //get number of arguments in this command
    
    if (myargs_size < 1)
    {
        printf("Error: Invalid command");
        return;            
    }
    
    
    if (pipe(left_pipe) == -1)                  //open the left pipe
    {
        printf("Error! Left pipe failed.");
        return;
    }
    
    
    
    pid_t cpidfirst = fork();                                   //fork the first child
    
    if (cpidfirst == 0) // first child
    {
        close(left_pipe[0]);                                    //close the left_pipe read in child
        if (dup2(left_pipe[1], STDOUT_FILENO) == -1){           //replace stdout with left_pipe write in child
			printf("dup2 failed in first child");
			kill(getpid(), SIGKILL);
		}
		close(left_pipe[1]);                                    //close the left_pipe write in child
		execvp(myargs[0], myargs);                              //execute
		printf("execvp failed in first child");
		kill(getpid(), SIGKILL);
    }
    
    // parent
    close(left_pipe[1]);                    //close the left_pipe write in parent
    
    // Reset myargs
    for (i = 0; i < arg_limit; i++)
    {
        if (myargs[i] != NULL)
        {
          myargs[i] = NULL;
          free(myargs[i]);
        }
    }
    if (myargs != NULL)
    {
      myargs = NULL;
      free(myargs);
    }
    
    while (wait(NULL) >= 0);
    
    /******************************First Command Ends****************************/
    
    // For each command not including the first and last
    for (i = 0; i < total_cmds - 2; i++)
    {
        // tokenize next command               
        temptoken = strtok_r(user_input, "|", &user_input);  
        clean_string(&temptoken);
        
         // allocate memory for myargs
        myargs = (char**) malloc(arg_limit * sizeof(char*));
        myargs_size = space_tokenize(temptoken, myargs);
        
        //make a new pipe
        int right_pipe[2];
        if (pipe(right_pipe) == -1)
        {
            printf("Error! Right pipe failed (%d).", i);
            return;
        }
        
        pid_t cpid_middle = fork();             //fork a middle child
        
        if (cpid_middle == 0) // middle command child process
        {
            if (dup2(left_pipe[0], STDIN_FILENO) == -1){                //replace stdin with left_pipe read in child
    			printf("dup2 of stdin failed in middle child (%d)", i);
    			kill(getpid(), SIGKILL);
		    }   
		    
		    close(left_pipe[0]);                                        //close left_pipe read in child
		    
		    if (dup2(right_pipe[1], STDOUT_FILENO) == -1){              //replace stdout with right_pipe write in child
    			printf("dup2 of stdout failed in middle child (%d)", i);
    			kill(getpid(), SIGKILL);
		    }  
		    
		    //close right pipe entirely in child
		    close(right_pipe[1]);
		    close(right_pipe[0]);
		    
		    execvp(myargs[0], myargs);          //execute
    		printf("execvp failed in middle child (%d)", i);
    		kill(getpid(), SIGKILL);
        }
        
        close(right_pipe[1]);           //close right_pipe write in parent
        
        if (dup2(right_pipe[0], left_pipe[0]) == -1)            //make the new left pipe read become the previous right pipe read
        {
            printf("dup2 of left and right pipe failed in middle child (%d)", i);
            kill(getpid(), SIGKILL);
        }
	    
	    close(right_pipe[0]);           //close right_pipe read in parent
	    
	    //clear myargs
	    int j;
	    for (j = 0; j < arg_limit; j++)
        {
            if (myargs[j] != NULL)
            {
                myargs[j] = NULL;
                free(myargs[j]);
            }
        }
        if (myargs != NULL)
        {
            myargs = NULL;
            free(myargs);
        }
        //wait for the child
        while (wait(NULL) >= 0);
    }
    
    /******************************Last Command Begins*****************************/
    
    
    //get the last token
    temptoken = strtok_r(user_input, "|", &user_input);
    clean_string(&temptoken);
    
    //allocate memory for last child args
    myargs = (char**) malloc(arg_limit * sizeof(char*));
    myargs_size = space_tokenize(temptoken, myargs);
    
    if (myargs_size < 1)
    {
        printf("Error: Invalid command");
        return;            
    }
    
    pid_t cpidlast = fork();            //fork last child
    
    if (cpidlast == 0) // last child
    {
        if(dup2(left_pipe[0], STDIN_FILENO) == -1){         //replace stdin with left_pipe read in child
        	printf("dup2 in last child failed");
        	kill(getpid(), SIGKILL);
		}
		close(left_pipe[0]);                                //close left_pipe read in child
		execvp(myargs[0], myargs);                          //execute
		printf("execvp failed in last child");
		kill(getpid(), SIGKILL);
    }
    
    // parent
    close(left_pipe[0]);            //close left_pipe read in parent
    
    //clear myargs
    for (i = 0; i < arg_limit; i++)
    {
        if (myargs[i] != NULL)
        {
          myargs[i] = NULL;
          free(myargs[i]);
        }
    }
    if (myargs != NULL)
    {
      myargs = NULL;
      free(myargs);
    }
    
    while (wait(NULL) >= 0);
    return;   

}

//removes leading and/or trailing space
void clean_string(char **str) {
    if ((*str)[0] == ' ') {                      //remove leading space
        int j = 0;
        for (j = 0; j < strlen(*str); j++) {
            (*str)[j] = (*str)[j + 1];
        }
    }
    if ((*str)[strlen(*str) - 1] == ' ') {      //remove trailing space
        (*str)[strlen(*str) - 1] = '\0';
    }
}
