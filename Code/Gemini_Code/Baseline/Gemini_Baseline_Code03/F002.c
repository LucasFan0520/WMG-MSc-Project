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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256];
            int tag_len = 0;
            if (sscanf(ptr, "%255s%n", tag, &tag_len) > 0) {
                char *msg = ptr + tag_len;
                while (*msg == ' ') msg++;
                records = realloc(records, (count + 1) * sizeof(Record));
                records[count].tag = strdup(tag);
                records[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char tag1[256], tag2[256], newtag[256];
            if (sscanf(ptr, "%255s %255s %255s", tag1, tag2, newtag) == 3) {
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
                    int new_len = strlen(msg1) + 1 + strlen(msg2) + 1;
                    char *new_msg = malloc(new_len);
                    sprintf(new_msg, "%s %s", msg1, msg2);
                    records = realloc(records, (count + 1) * sizeof(Record));
                    records[count].tag = strdup(newtag);
                    records[count].message = new_msg;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(ptr, "%255s", tag) > 0) {
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
                if (count > 0) {
                    records = realloc(records, count * sizeof(Record));
                } else {
                    free(records);
                    records = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(ptr, "%255s", tag) > 0) {
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
                } else {
                    printf("NOT_FOUND\n");
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
