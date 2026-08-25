/* F002.c */
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
    int capacity = 0;
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
            char *tag = NULL;
            char *msg = NULL;
            if (space) {
                *space = '\0';
                tag = strdup(args);
                msg = strdup(space + 1);
                *space = ' ';
            } else {
                tag = strdup(args);
                msg = strdup("");
            }
            if (!tag || !msg) {
                free(tag);
                free(msg);
                continue;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *tmp = realloc(records, new_cap * sizeof(Record));
                if (!tmp) {
                    free(tag);
                    free(msg);
                    continue;
                }
                records = tmp;
                capacity = new_cap;
            }
            records[count].tag = tag;
            records[count].message = msg;
            count++;
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *args = line + 7;
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *tag1 = args;
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            if (!space2) {
                *space1 = ' ';
                continue;
            }
            *space2 = '\0';
            char *tag2 = rest;
            char *newtag = space2 + 1;

            char *msg1 = NULL;
            char *msg2 = NULL;
            for (int i = 0; i < count; i++) {
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
                char *newmsg = malloc(l1 + l2 + 2);
                char *nt = strdup(newtag);
                if (newmsg && nt) {
                    strcpy(newmsg, msg1);
                    strcat(newmsg, " ");
                    strcat(newmsg, msg2);
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *tmp = realloc(records, new_cap * sizeof(Record));
                        if (tmp) {
                            records = tmp;
                            capacity = new_cap;
                        } else {
                            free(newmsg);
                            free(nt);
                            *space1 = ' ';
                            *space2 = ' ';
                            continue;
                        }
                    }
                    records[count].tag = nt;
                    records[count].message = newmsg;
                    count++;
                } else {
                    free(newmsg);
                    free(nt);
                }
            }
            *space1 = ' ';
            *space2 = ' ';
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
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    char *p = records[i].message;
                    while (*p) {
                        if (*p == ' ') putchar('_');
                        else putchar(*p);
                        p++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
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
