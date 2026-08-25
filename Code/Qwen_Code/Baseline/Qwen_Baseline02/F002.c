// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag; char *msg; } Msg;
Msg m[1000]; int nm = 0;

int main() {
    char cmd[20], t1[100], t2[100], nt[100], msg[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", t1); fgets(msg, sizeof(msg), stdin);
            msg[strcspn(msg, "\n")] = 0;
            if (msg[0] == ' ') memmove(msg, msg + 1, strlen(msg));
            m[nm].tag = strdup(t1); m[nm++].msg = strdup(msg);
        } else if (!strcmp(cmd, "CONCAT")) {
            scanf("%s %s %s", t1, t2, nt);
            char *m1 = NULL, *m2 = NULL;
            for (int i = 0; i < nm; i++) {
                if (!strcmp(m[i].tag, t1) && !m1) m1 = m[i].msg;
                if (!strcmp(m[i].tag, t2) && !m2) m2 = m[i].msg;
            }
            if (m1 && m2) {
                char *nm_msg = malloc(strlen(m1) + strlen(m2) + 2);
                sprintf(nm_msg, "%s %s", m1, m2);
                m[nm].tag = strdup(nt); m[nm++].msg = nm_msg;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", t1);
            for (int i = 0; i < nm; i++) {
                if (!strcmp(m[i].tag, t1)) {
                    free(m[i].tag); free(m[i].msg);
                    for (int j = i; j < nm - 1; j++) m[j] = m[j + 1];
                    nm--; i--;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", t1);
            for (int i = 0; i < nm; i++) {
                if (!strcmp(m[i].tag, t1)) {
                    for (char *c = m[i].msg; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                    printf("\n"); break;
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nm; i++) printf("%s %s\n", m[i].tag, m[i].msg);
        }
    }
    for (int i = 0; i < nm; i++) { free(m[i].tag); free(m[i].msg); }
    return 0;
}