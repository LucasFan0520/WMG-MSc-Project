// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

Person *q = NULL;
int nq = 0;

int find_person(const char *name) {
    for (int i = 0; i < nq; i++)
        if (strcmp(q[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 5 + strlen(arg1);
            if (*p == ' ') p++;
            q = realloc(q, sizeof(Person) * (nq + 1));
            q[nq].name = strdup(arg1);
            q[nq].note = strdup(p);
            nq++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (nq > 0) {
                printf("%s\n", q[0].name);
                free(q[0].name);
                free(q[0].note);
                for (int i = 0; i < nq - 1; i++) q[i] = q[i + 1];
                nq--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_person(arg1);
            if (i != -1) {
                free(q[i].name);
                free(q[i].note);
                for (int j = i; j < nq - 1; j++) q[j] = q[j + 1];
                nq--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_person(arg1);
            if (i != -1) printf("%s\n", q[i].note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < nq; i++) printf("%s %s\n", q[i].name, q[i].note);
        }
    }
    for (int i = 0; i < nq; i++) {
        free(q[i].name);
        free(q[i].note);
    }
    free(q);
    return 0;
}