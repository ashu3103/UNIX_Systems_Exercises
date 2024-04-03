#ifndef __LOG__
#define __LOG__

#include <time.h>

typedef struct data_struct {
    char *string;
    time_t time;
} data_t;

int addmsg(data_t data);

void clearlog(void);

char* getlog(void);

int savelog(char *filename);

#endif