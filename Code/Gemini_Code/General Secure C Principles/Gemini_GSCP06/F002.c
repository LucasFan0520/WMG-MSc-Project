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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *tag = args;
                char *message = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *next = realloc(records, capacity * sizeof(Record));
                    if (!next) break;
                    records = next;
                }
                records[count].tag = strdup(tag);
                records[count].message = strdup(message);
                count++;
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *args = line + 7;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *tag1 = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *tag2 = rest;
                    char *newtag = sp2 + 1;
                    char *msg1 = NULL;
                    char *msg2 = NULL;
                    for (size_t i = 0; i < count; i++) {
                        if (!msg1 && strcmp(records[i].tag, tag1) == 0) {
                            msg1 = records[i].message;
                        }
                        if (!msg2 && strcmp(records[i].tag, tag2) == 0) {
                            msg2 = records[i].message;
                        }
                    }
                    if (msg1 && msg2) {
                        size_t l1 = strlen(msg1);
                        size_t l2 = strlen(msg2);
                        char *new_msg = malloc(l1 + l2 + 2);
                        if (new_msg) {
                            strcpy(new_msg, msg1);
                            new_msg[l1] = ' ';
                            strcpy(new_msg + l1 + 1, msg2);
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Record *next = realloc(records, capacity * sizeof(Record));
                                if (!next) {
                                    free(new_msg);
                                    break;
                                }
                                records = next;
                            }
                            records[count].tag = strdup(newtag);
                            records[count].message = new_msg;
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    char *m = records[i].message;
                    for (size_t k = 0; m[k] != '\0'; k++) {
                        if (m[k] == ' ') putchar('_');
                        else putchar(m[k]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}
