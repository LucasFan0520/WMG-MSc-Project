// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Person;
Person *q = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i=0; i<n; i++) if (!strcmp(q[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], name[256], rest[1700];
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "JOIN") || !strcmp(cmd, "REJOIN")) sscanf(line, "%*s %s %[^\n]", name, rest);
        else sscanf(line, "%*s %s", name);

        if (!strcmp(cmd, "JOIN") || !strcmp(cmd, "REJOIN")) {
            if (n == cap) { cap = cap ? cap*2 : 4; q = realloc(q, cap*sizeof(Person)); }
            q[n].name = strdup(name); q[n].note = strdup(rest); n++;
        } else if (!strcmp(cmd, "SERVE")) {
            if (n > 0) {
                printf("%s\n", q[0].name); free(q[0].name); free(q[0].note);
                for (int i=0; i<n-1; i++) q[i] = q[i+1]; n--;
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            int i = find(name);
            if (i >= 0) {
                free(q[i].name); free(q[i].note);
                for (int j=i; j<n-1; j++) q[j] = q[j+1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(name);
            if (i >= 0) puts(q[i].note);
        } else if (!strcmp(cmd, "QUEUE")) {
            for (int i=0; i<n; i++) puts(q[i].name);
        }
    }
    return 0;
}