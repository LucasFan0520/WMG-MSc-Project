// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *tag = p;
                char *msg = space + 1;
                Record *tmp = realloc(records, sizeof(Record) * (count + 1));
                if (tmp) {
                    records = tmp;
                    records[count].tag = strdup(tag);
                    records[count].message = strdup(msg);
                    if (records[count].tag && records[count].message) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *tag1 = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *tag2 = p2;
                    char *newtag = s2 + 1;
                    int idx1 = -1, idx2 = -1;
                    for (int i = 0; i < count; i++) {
                        if (idx1 == -1 && strcmp(records[i].tag, tag1) == 0) idx1 = i;
                        if (idx2 == -1 && strcmp(records[i].tag, tag2) == 0) idx2 = i;
                    }
                    if (idx1 != -1 && idx2 != -1) {
                        size_t l1 = strlen(records[idx1].message);
                        size_t l2 = strlen(records[idx2].message);
                        char *new_msg = malloc(l1 + l2 + 2);
                        if (new_msg) {
                            strcpy(new_msg, records[idx1].message);
                            strcat(new_msg, " ");
                            strcat(new_msg, records[idx2].message);
                            Record *tmp = realloc(records, sizeof(Record) * (count + 1));
                            if (tmp) {
                                records = tmp;
                                records[count].tag = strdup(newtag);
                                records[count].message = new_msg;
                                if (records[count].tag) {
                                    count++;
                                } else {
                                    free(new_msg);
                                }
                            } else {
                                free(new_msg);
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *t = records[found].message;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    free(line);
    return 0;
}
