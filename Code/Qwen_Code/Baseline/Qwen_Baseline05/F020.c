// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_BODY 10000

typedef struct {
    char title[MAX_TITLE];
    char body[MAX_BODY];
} Record;

Record records[1000];
int record_count = 0;

int find_record(const char *title) {
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].title, title) == 0) return i;
    }
    return -1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[1000];
    int reading_body = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        if (strcmp(line, "--") == 0) {
            if (reading_body) {
                reading_body = 0;
            } else {
                reading_body = 1;
            }
        } else {
            if (!reading_body && record_count == 0) {
                strcpy(records[record_count].title, line);
                records[record_count].body[0] = '\0';
                reading_body = 1;
            } else if (!reading_body) {
                record_count++;
                strcpy(records[record_count].title, line);
                records[record_count].body[0] = '\0';
                reading_body = 1;
            } else {
                if (records[record_count].body[0] != '\0') {
                    strcat(records[record_count].body, "\n");
                }
                strcat(records[record_count].body, line);
            }
        }
    }
    
    if (record_count >= 0 && records[record_count].title[0] != '\0') {
        record_count++;
    }
    
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char cmd[20], title[MAX_TITLE], line[1000];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_record(title);
            if (idx != -1) {
                printf("%s\n", records[idx].title);
                if (records[idx].body[0] != '\0') {
                    printf("%s\n", records[idx].body);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            int idx = find_record(title);
            if (idx != -1) {
                for (int i = idx; i < record_count - 1; i++) {
                    records[i] = records[i + 1];
                }
                record_count--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", title);
            strcpy(records[record_count].title, title);
            records[record_count].body[0] = '\0';
            
            fgets(line, sizeof(line), stdin);
            if (line[0] == '\n') {
                while (fgets(line, sizeof(line), stdin)) {
                    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
                    if (strcmp(line, "--") == 0) break;
                    
                    if (records[record_count].body[0] != '\0') {
                        strcat(records[record_count].body, "\n");
                    }
                    strcat(records[record_count].body, line);
                }
            }
            record_count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < record_count; i++) {
                printf("%s\n", records[i].title);
                if (records[i].body[0] != '\0') {
                    printf("%s\n", records[i].body);
                }
                printf("--\n");
            }
        }
    }
    
    return 0;
}