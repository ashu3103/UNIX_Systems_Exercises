#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "log.h"

typedef struct log_list {
    data_t* data;
    int data_size;
    struct log_list* next;
} log_list;

static log_list *head = NULL;
static log_list *tail = NULL;
static int log_list_size = 0;

void clearlog()
{
    // free all the memory
    while(head){
        log_list* tmp = head;
        head = head->next;
        free(tmp->data->string);
        free(tmp->data);
        free(tmp);
    }

    // reset the head and tail pointer
    head = NULL;
    tail = NULL;
}

int addmsg(data_t data)
{
    char* data_str;
    data_t* new_data;

    if((data_str = malloc(strlen(data.string)+1)) == NULL) return 1;
    memset(data_str, '\0', strlen(data.string)+1);

    if((new_data = malloc(sizeof(data_t))) == NULL) return 1;

    // Create new data
    new_data->time = data.time;
    new_data->string = data_str;
    strncpy(new_data->string, data.string, strlen(data.string));

    log_list* newNode =  malloc(sizeof(log_list));
    if(!newNode) return 1;

    newNode->data = new_data;
    newNode->data_size = strlen(new_data->string) + strlen(ctime(&(new_data->time))) + 5;
    newNode->next = NULL;

    // Add the new node in the list
    if(!head) {
        head = newNode;
        tail  = head;
    } else {
        tail->next = newNode;
        tail = tail->next;
    }

    log_list_size++;
    return 0;
}

char* getlog()
{
    char* result;
    log_list* tmp_head = head;
    int total_size = 0;
    while(tmp_head){
        total_size += tmp_head->data_size;
        tmp_head = tmp_head->next;
    }
    tmp_head = head;

    result = malloc(total_size*sizeof(char));
    memset(result, '\0', total_size*sizeof(char));

    int offset = 0;
    while(tmp_head){
        int string_len = (int)strlen(tmp_head->data->string);
        int anon_len = 4;
        int time_len = (int)strlen(ctime(&(tmp_head->data->time)));

        memcpy(result+offset, tmp_head->data->string, string_len);
        offset+=string_len;
        memcpy(result+offset, " -- ", 4);
        offset+=4;
        memcpy(result+offset, ctime(&(tmp_head->data->time)), time_len);
        offset+=time_len;

        tmp_head = tmp_head->next;
    }

    return result;
}

void getCurrentTime(time_t* tm)
{
    time(tm);
}

int savelog(char* filename)
{
    int flags = O_RDWR | O_CREAT | O_TRUNC; // flags for read/write, create
    // mode_t mode = S_IRWXU; // mode 

    // open a file in CREATE mode
    int fd = open(filename, flags, S_IRWXU | S_IRWXG);
    char* res = getlog();

    // write to the file
    int total_bytes = strlen(res);
    int offset = 0;
    while(total_bytes)
    {
        int bytes_write = write(fd, res+offset, total_bytes);

        if(bytes_write < 0){
            return 1;
        }

        offset+=bytes_write;
        total_bytes-=bytes_write;
    }

    return 0;
}
