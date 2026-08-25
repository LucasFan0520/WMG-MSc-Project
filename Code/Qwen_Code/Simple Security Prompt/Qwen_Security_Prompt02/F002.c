// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *msg;
} Message;

Message *msgs = NULL;
int count = 0;

int find_tag(const char *tag) {
    for (int i = 0; i < count; i++) {
        if (strcmp(msgs[i].tag, tag) == 0) {
            return i;
        }
    }
    return -1;
}

void show_msg(const char *m) {
    for (int i = 0; m[i]; i++) {
        if (m[i] == ' ') {
            putchar('_');
        } else {
            putchar(m[i]);
        }
    }
    putchar('\n');
}

int main() {
    char cmd[20];
    char t1[1000];
    char t2[1000];
    char t3[1000];
    char msg[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", t1);
            fgets(msg, sizeof(msg), stdin);
            if (msg[0] == ' ') {
                memmove(msg, msg + 1, strlen(msg));
            }
            msg[strcspn(msg, "\n")] = 0;
            int idx = find_tag(t1);
            if (idx != -1) {
                char *nm = malloc(strlen(msgs[idx].msg) + strlen(msg) + 2);
                sprintf(nm, "%s %s", msgs[idx].msg, msg);
                free(msgs[idx].msg);
                msgs[idx].msg = nm;
            } else {
                msgs = realloc(msgs, sizeof(Message) * (count + 1));
                msgs[count].tag = strdup(t1);
                msgs[count].msg = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            scanf("%s %s %s", t1, t2, t3);
            int i1 = find_tag(t1);
            int i2 = find_tag(t2);
            if (i1 != -1 && i2 != -1) {
                char *nm = malloc(strlen(msgs[i1].msg) + strlen(msgs[i2].msg) + 2);
                sprintf(nm, "%s %s", msgs[i1].msg, msgs[i2].msg);
                int i3 = find_tag(t3);
                if (i3 != -1) {
                    free(msgs[i3].msg);
                    msgs[i3].msg = nm;
                } else {
                    msgs = realloc(msgs, sizeof(Message) * (count + 1));
                    msgs[count].tag = strdup(t3);
                    msgs[count].msg = nm;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", t1);
            int idx = find_tag(t1);
            if (idx != -1) {
                free(msgs[idx].tag);
                free(msgs[idx].msg);
                for (int i = idx; i < count - 1; i++) {
                    msgs[i] = msgs[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", t1);
            int idx = find_tag(t1);
            if (idx != -1) {
                show_msg(msgs[idx].msg);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", msgs[i].tag, msgs[i].msg);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(msgs[i].tag);
        free(msgs[i].msg);
    }
    free(msgs);

    return 0;
}