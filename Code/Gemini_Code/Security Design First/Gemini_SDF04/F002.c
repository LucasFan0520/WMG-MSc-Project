// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

Record *records = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *msg = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Record *tmp = realloc(records, capacity * sizeof(Record));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        records = tmp;
                    }
                    records[count].tag = strdup(arg1);
                    records[count].message = strdup(msg);
                    if (records[count].tag && records[count].message) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "CONCAT") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *arg2 = space2 + 1;
                    char *space3 = strchr(arg2, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *arg3 = space3 + 1;
                        int idx1 = -1, idx2 = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(records[i].tag, arg1) == 0) {
                                idx1 = i;
                                break;
                            }
                        }
                        for (int i = 0; i < count; i++) {
                            if (strcmp(records[i].tag, arg2) == 0) {
                                idx2 = i;
                                break;
                            }
                        }
                        if (idx1 != -1 && idx2 != -1) {
                            size_t len1 = strlen(records[idx1].message);
                            size_t len2 = strlen(records[idx2].message);
                            char *new_msg = malloc(len1 + len2 + 2);
                            if (new_msg) {
                                strcpy(new_msg, records[idx1].message);
                                strcat(new_msg, " ");
                                strcat(new_msg, records[idx2].message);
                                if (count >= capacity) {
                                    capacity = capacity == 0 ? 4 : capacity * 2;
                                    Record *tmp = realloc(records, capacity * sizeof(Record));
                                    if (!tmp) {
                                        free(new_msg);
                                        free(line);
                                        return 1;
                                    }
                                    records = tmp;
                                }
                                records[count].tag = strdup(arg3);
                                records[count].message = new_msg;
                                if (records[count].tag) {
                                    count++;
                                } else {
                                    free(new_msg);
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(records[i].tag, arg1) == 0) {
                        free(records[i].tag);
                        free(records[i].message);
                        for (int j = i; j < count - 1; j++) {
                            records[j] = records[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, arg1) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *p = records[found].message;
                    for (size_t i = 0; p[i] != '\0'; i++) {
                        if (p[i] == ' ') {
                            putchar('_');
                        } else {
                            putchar(p[i]);
                        }
                    }
                    putchar('\n');
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", records[i].tag, records[i].message);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}