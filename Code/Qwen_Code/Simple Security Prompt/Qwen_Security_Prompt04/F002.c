// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *msg;
} TagMsg;

TagMsg tm[1000];
int tc = 0;

int find_tag(char *tag) {
    for (int i = 0; i < tc; i++) if (strcmp(tm[i].tag, tag) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char tag[1000];
            sscanf(rest, "%s", tag);
            char *msg = rest + strlen(tag);
            while (*msg == ' ') msg++;
            int idx = find_tag(tag);
            if (idx == -1) {
                tm[tc].tag = strdup(tag);
                tm[tc].msg = strdup(msg);
                tc++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[1000], t2[1000], nt[1000];
            sscanf(rest, "%s %s %s", t1, t2, nt);
            int i1 = find_tag(t1), i2 = find_tag(t2);
            if (i1 != -1 && i2 != -1) {
                char *nmsg = malloc(strlen(tm[i1].msg) + strlen(tm[i2].msg) + 2);
                sprintf(nmsg, "%s %s", tm[i1].msg, tm[i2].msg);
                int idx = find_tag(nt);
                if (idx == -1) {
                    tm[tc].tag = strdup(nt);
                    tm[tc].msg = nmsg;
                    tc++;
                } else {
                    free(tm[idx].msg);
                    tm[idx].msg = nmsg;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[1000];
            sscanf(rest, "%s", tag);
            int idx = find_tag(tag);
            if (idx != -1) {
                free(tm[idx].tag);
                free(tm[idx].msg);
                for (int i = idx; i < tc - 1; i++) tm[i] = tm[i+1];
                tc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[1000];
            sscanf(rest, "%s", tag);
            int idx = find_tag(tag);
            if (idx != -1) {
                for (char *p = tm[idx].msg; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < tc; i++) printf("%s %s\n", tm[i].tag, tm[i].msg);
        }
    }
    for (int i = 0; i < tc; i++) {
        free(tm[i].tag);
        free(tm[i].msg);
    }
    return 0;
}