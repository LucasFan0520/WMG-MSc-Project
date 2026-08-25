// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessageItem;

int main() {
    MessageItem *items = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(NULL, " ");
            char *msg = strtok(NULL, "");
            if (tag && msg) {
                items = realloc(items, (count + 1) * sizeof(MessageItem));
                items[count].tag = strdup(tag);
                items[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(NULL, " ");
            char *tag2 = strtok(NULL, " ");
            char *newtag = strtok(NULL, " ");
            if (tag1 && tag2 && newtag) {
                char *msg1 = NULL;
                char *msg2 = NULL;
                for (int i = 0; i < count; i++) {
                    if (!msg1 && strcmp(items[i].tag, tag1) == 0) {
                        msg1 = items[i].message;
                    }
                    if (!msg2 && strcmp(items[i].tag, tag2) == 0) {
                        msg2 = items[i].message;
                    }
                }
                if (msg1 && msg2) {
                    int len = strlen(msg1) + 1 + strlen(msg2) + 1;
                    char *new_msg = malloc(len);
                    sprintf(new_msg, "%s %s", msg1, msg2);
                    items = realloc(items, (count + 1) * sizeof(MessageItem));
                    items[count].tag = strdup(newtag);
                    items[count].message = new_msg;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].tag, tag) == 0) {
                        free(items[i].tag);
                        free(items[i].message);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(items);
                    items = NULL;
                } else {
                    items = realloc(items, count * sizeof(MessageItem));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].tag, tag) == 0) {
                        char *m = items[i].message;
                        for (int j = 0; m[j]; j++) {
                            if (m[j] == ' ') {
                                putchar('_');
                            } else {
                                putchar(m[j]);
                            }
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].message);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].tag);
        free(items[i].message);
    }
    free(items);
    return 0;
}
