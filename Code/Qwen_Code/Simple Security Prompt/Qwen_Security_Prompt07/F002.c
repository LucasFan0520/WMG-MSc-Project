// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag, *msg; } Msg;
Msg *msgs = 0; int n = 0, cap = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256], rest[1700];
        a[0] = b[0] = c[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else if (!strcmp(cmd, "CONCAT")) sscanf(line, "%*s %s %s %s", a, b, c);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap*2 : 4; msgs = realloc(msgs, cap*sizeof(Msg)); }
            msgs[n].tag = strdup(a); msgs[n].msg = strdup(rest); n++;
        } else if (!strcmp(cmd, "CONCAT")) {
            char *m1 = 0, *m2 = 0;
            for (int i=0; i<n; i++) {
                if (!strcmp(msgs[i].tag, a) && !m1) m1 = msgs[i].msg;
                if (!strcmp(msgs[i].tag, b) && !m2) m2 = msgs[i].msg;
            }
            if (m1 && m2) {
                char *t = malloc(strlen(m1) + strlen(m2) + 2);
                sprintf(t, "%s %s", m1, m2);
                if (n == cap) { cap = cap ? cap*2 : 4; msgs = realloc(msgs, cap*sizeof(Msg)); }
                msgs[n].tag = strdup(c); msgs[n].msg = t; n++;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            for (int i=0; i<n; ) {
                if (!strcmp(msgs[i].tag, a)) {
                    free(msgs[i].tag); free(msgs[i].msg);
                    for (int j=i; j<n-1; j++) msgs[j] = msgs[j+1]; n--;
                } else i++;
            }
        } else if (!strcmp(cmd, "FIND")) {
            for (int i=0; i<n; i++) {
                if (!strcmp(msgs[i].tag, a)) {
                    for (char *x = msgs[i].msg; *x; x++) putchar(*x == ' ' ? '_' : *x);
                    putchar('\n'); break;
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<n; i++) printf("%s %s\n", msgs[i].tag, msgs[i].msg);
        }
    }
    return 0;
}