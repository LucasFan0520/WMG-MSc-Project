// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessageRecord;

int main() {
    MessageRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *tag = args;
            char *space2 = strchr(args, ' ');
            char *message = "";
            if (space2) {
                *space2 = '\0';
                message = space2 + 1;
                while (*message == ' ') message++;
            }
            records = realloc(records, (count + 1) * sizeof(MessageRecord));
            if (!records) return 1;
            records[count].tag = strdup(tag);
            records[count].message = strdup(message);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0 && args) {
            char *tag1 = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *tag2 = space2 + 1;
                while (*tag2 == ' ') tag2++;
                char *space3 = strchr(tag2, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *newtag = space3 + 1;
                    while (*newtag == ' ') newtag++;

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
                        size_t new_len = strlen(msg1) + strlen(msg2) + 2;
                        char *new_msg = malloc(new_len);
                        if (!new_msg) return 1;
                        strcpy(new_msg, msg1);
                        strcat(new_msg, " ");
                        strcat(new_msg, msg2);

                        records = realloc(records, (count + 1) * sizeof(MessageRecord));
                        if (!records) return 1;
                        records[count].tag = strdup(newtag);
                        records[count].message = new_msg;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            int idx = 0;
            while (idx < count) {
                if (strcmp(records[idx].tag, args) == 0) {
                    free(records[idx].tag);
                    free(records[idx].message);
                    for (int j = idx; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            records = realloc(records, count * sizeof(MessageRecord));
            if (count > 0 && !records) return 1;
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, args) == 0) {
                    for (int j = 0; records[i].message[j] != '\0'; j++) {
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
