// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAGS 1000
#define MAX_LEN 1000

typedef struct {
    char tag[100];
    char msg[MAX_LEN];
} TagMsg;

TagMsg tags[MAX_TAGS];
int count = 0;

int find_tag(const char *tag) {
    for (int i = 0; i < count; i++) if (strcmp(tags[i].tag, tag) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], t1[100], t2[100], t3[100], msg[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", t1);
            fgets(msg, MAX_LEN, stdin);
            msg[strcspn(msg, "\n")] = 0;
            if (msg[0] == ' ') memmove(msg, msg + 1, strlen(msg));
            int idx = find_tag(t1);
            if (idx == -1 && count < MAX_TAGS) {
                strcpy(tags[count].tag, t1);
                strcpy(tags[count].msg, msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            scanf("%s %s %s", t1, t2, t3);
            int i1 = find_tag(t1), i2 = find_tag(t2);
            if (i1 != -1 && i2 != -1 && count < MAX_TAGS) {
                strcpy(tags[count].tag, t3);
                snprintf(tags[count].msg, MAX_LEN, "%s %s", tags[i1].msg, tags[i2].msg);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", t1);
            int idx = find_tag(t1);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) tags[i] = tags[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", t1);
            int idx = find_tag(t1);
            if (idx != -1) {
                for (char *p = tags[idx].msg; *p; p++) putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) printf("%s %s\n", tags[i].tag, tags[i].msg);
        }
    }
    return 0;
}