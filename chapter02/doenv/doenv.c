// include headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define EQUAL_DELIM "="
#define MAX_COMMAND_SIZE 1024

extern char** environ;

/**
 * Usage function
*/
void usage()
{
    printf("usage: doenv [-i] [name=value] ... [utility [argument ...]]\n");
}

/**
 * Function to print all the current environment variables
*/
void getenv_custom()
{
    int i;
    printf("The environment list follows:\n");
    for(i = 0; environ[i] != NULL; i++)
    printf("environ[%d]: %s\n", i, environ[i]);
}

/**
 * Check if the following string is a pair of form "NAME=VALUE"
*/
int checkPairs(char* pair)
{
    char* tmp_pair = malloc(strlen(pair));
    memcpy(tmp_pair, pair, strlen(pair));

    char* name = NULL;
    char* value = NULL;

    name = strtok(tmp_pair, EQUAL_DELIM);
    if(!name) return 1;

    value = strtok(NULL, EQUAL_DELIM);
    if (!value) return 1;

    return 0;
}

/**
 * Function to print all the current environment variables
*/
int create_command(char* argv[], char** cmd, int start, int end)
{
    // find length of command
    int cmd_length = 0;
    for(int i=start;i<end;i++){
        cmd_length+=strlen(argv[i]);
        cmd_length++;
    }
    cmd_length--; // Remove 1 whitespace

    char* res = malloc(cmd_length);
    if(!res) return 1;

    int offset = 0;
    for(int i=start;i<end;i++){
        memcpy(res+offset, argv[i], strlen(argv[i]));
        offset+=(strlen(argv[i]));
        if(i==end-1){
            break;
        }
        memcpy(res+offset, " ", 1);
        offset+=1;
    }

    *cmd = res;
    return 0;
}


int main(int argc, char* argv[])
{
    if(argc == 1) // no flags provided
    { 
        getenv_custom();
    }
    else if(argc >= 2)
    {
        // temporary environment variable buffer
        char** tmp_environ;

        // Get the total number of env variables
        int total_envs = 0;
        for(int i=0;environ[i] != NULL; i++) total_envs++;

        // Get the arguments till utility
        int pair_index = argc;
        int utility = 0;
        for (int j=argc-1;j>=0;j--) {
            if(!strcmp(argv[j], "utility")) {
                pair_index = j;
                utility = 1;
                break;
            }
        }

        char* command;
        create_command(argv, &command, pair_index+1, argc);

        if (!strcmp(argv[1], "-i")) {
            tmp_environ = malloc((pair_index-1)*sizeof(char*));
            int i;
            for (i=2;i<pair_index;i++) {
                if(!checkPairs(argv[i])){
                    tmp_environ[i-2] = argv[i];
                }else{
                    goto error;
                }
            }
            tmp_environ[i] = NULL;

        } else {
            tmp_environ = malloc((pair_index+total_envs)*sizeof(char*));

            // copy the previous environment variables
            int i;
            for(i=0;environ[i] != NULL; i++) tmp_environ[i] = environ[i];

            for (i=1;i<pair_index;i++) {
                if(!checkPairs(argv[i])){
                    tmp_environ[total_envs+i-1] = argv[i];
                }else{
                    goto error;
                }
            }

            tmp_environ[i+total_envs] = NULL;
        }

        environ = tmp_environ;
        if(utility){
            system(command);
        } else {
            getenv_custom();
        }
        
    } else {
        goto error;
    }

    return 0;

error:
    usage();
    return 1;
}