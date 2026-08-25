// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

int main() {
    Record *records = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256];
            int tag_bytes = 0;
            if (sscanf(args, "%255s%n", tag, &tag_bytes) > 0) {
                char *msg = args + tag_bytes;
                while (*msg == ' ') msg++;
                records = realloc(records, (count + 1) * sizeof(Record));
                records[count].tag = strdup(tag);
                records[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char tag1[256], tag2[256], newtag[256];
            if (sscanf(args, "%255s %255s %255s", tag1, tag2, newtag) == 3) {
                char *msg1 = NULL;
                char *msg2 = NULL;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, tag1) == 0) {
                        msg1 = records[i].message;
                        break;
                    }
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, tag2) == 0) {
                        msg2 = records[i].message;
                        break;
                    }
                }
                if (msg1 && msg2) {
                    int len1 = strlen(msg1);
                    int len2 = strlen(msg2);
                    char *new_msg = malloc(len1 + len2 + 2);
                    strcpy(new_msg, msg1);
                    strcat(new_msg, " ");
                    strcat(new_msg, msg2);
                    records = realloc(records, (count + 1) * sizeof(Record));
                    records[count].tag = strdup(newtag);
                    records[count].message = new_msg;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(args, "%255s", tag) == 1) {
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
                records = realloc(records, count * sizeof(Record));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(args, "%255s", tag) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, tag) == 0) {
                        for (int j = 0; records[i].message[j] != '\0'; j++) {
                            if (records[i].message[j] == ' ') printf("_");
                            else printf("%c", records[i].message[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
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
    return 0;
}
