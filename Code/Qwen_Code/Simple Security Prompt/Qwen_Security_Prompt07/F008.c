// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; int score; } Stu;
Stu *stus = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i=0; i<n; i++) if (!strcmp(stus[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], name[256], rest[1700]; int score;
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD") || !strcmp(cmd, "UPDATE")) sscanf(line, "%*s %s %d %[^\n]", name, &score, rest);
        else sscanf(line, "%*s %s", name);

        if (!strcmp(cmd, "ADD")) {
            if (find(name) < 0) {
                if (n == cap) { cap = cap ? cap*2 : 4; stus = realloc(stus, cap*sizeof(Stu)); }
                stus[n].name = strdup(name); stus[n].score = score; stus[n].note = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "UPDATE")) {
            int i = find(name);
            if (i >= 0) { stus[i].score = score; free(stus[i].note); stus[i].note = strdup(rest); }
        } else if (!strcmp(cmd, "DROP")) {
            int i = find(name);
            if (i >= 0) {
                free(stus[i].name); free(stus[i].note);
                for (int j=i; j<n-1; j++) stus[j] = stus[j+1]; n--;
            }
        } else if (!strcmp(cmd, "AVERAGE")) {
            int sum = 0;
            for (int i=0; i<n; i++) sum += stus[i].score;
            printf("%d\n", n ? sum/n : 0);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i=0; i<n; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
    }
    return 0;
}