// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAG 100
#define MAX_MSG 10000

typedef struct {
    char tag[MAX_TAG];
    char message[MAX_MSG];
} Record;

Record records[1000];
int count = 0;

int find_first(const char *tag) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].tag, tag) == 0) return i;
    }
    return -1;
}

void print_with_underscores(const char *text) {
    for (int i = 0; text[i]; i++) {
        if (text[i] == ' ') putchar('_');
        else putchar(text[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], tag1[MAX_TAG], tag2[MAX_TAG], newtag[MAX_TAG], msg[MAX_MSG];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", tag1);
            fgets(msg, MAX_MSG, stdin);
            if (msg[0] == ' ') memmove(msg, msg + 1, strlen(msg));
            if (msg[strlen(msg) - 1] == '\n') msg[strlen(msg) - 1] = '\0';
            
            strcpy(records[count].tag, tag1);
            strcpy(records[count].message, msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            scanf("%s %s %s", tag1, tag2, newtag);
            int idx1 = find_first(tag1);
            int idx2 = find_first(tag2);
            
            if (idx1 != -1 && idx2 != -1) {
                strcpy(records[count].tag, newtag);
                strcpy(records[count].message, records[idx1].message);
                strcat(records[count].message, " ");
                strcat(records[count].message, records[idx2].message);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", tag1);
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag1) == 0) {
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", tag1);
            int idx = find_first(tag1);
            if (idx != -1) {
                print_with_underscores(records[idx].message);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    
    return 0;
}