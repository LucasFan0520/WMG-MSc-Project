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
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *message = p;
            if (*tag == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *new_records = realloc(records, new_cap * sizeof(Record));
                if (!new_records) continue;
                records = new_records;
                capacity = new_cap;
            }
            char *t = strdup(tag);
            char *m = strdup(message);
            if (t && m) {
                records[count].tag = t;
                records[count].message = m;
                count++;
            } else {
                free(t);
                free(m);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag1 = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *tag2 = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *newtag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*tag1 == '\0' || *tag2 == '\0' || *newtag == '\0') continue;

            int idx1 = -1, idx2 = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag1) == 0) {
                    idx1 = (int)i;
                    break;
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag2) == 0) {
                    idx2 = (int)i;
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
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *new_records = realloc(records, new_cap * sizeof(Record));
                        if (new_records) {
                            records = new_records;
                            capacity = new_cap;
                        } else {
                            free(new_msg);
                            continue;
                        }
                    }
                    char *nt = strdup(newtag);
                    if (nt) {
                        records[count].tag = nt;
                        records[count].message = new_msg;
                        count++;
                    } else {
                        free(new_msg);
                        free(nt);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*tag == '\0') continue;

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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*tag == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    for (size_t j = 0; records[i].message[j] != '\0'; j++) {
                        if (records[i].message[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(records[i].message[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    free(line);
    return 0;
}
