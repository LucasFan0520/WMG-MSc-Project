// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    int score;
} Stu;

Stu *stus = NULL;
int count = 0;

int find_stu(const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(stus[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        int sc;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            sscanf(line, "%*s %s %d", arg1, &sc);
            char *p = line;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            int i = find_stu(arg1);
            if (i != -1) {
                stus[i].score = sc;
                free(stus[i].note);
                stus[i].note = strdup(p);
            } else {
                stus = realloc(stus, sizeof(Stu) * (count + 1));
                stus[count].name = strdup(arg1);
                stus[count].score = sc;
                stus[count].note = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_stu(arg1);
            if (i != -1) {
                free(stus[i].name);
                free(stus[i].note);
                for (int j = i; j < count - 1; j++) stus[j] = stus[j + 1];
                count--;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0;
            for (int i = 0; i < count; i++) sum += stus[i].score;
            printf("%d\n", count ? sum / count : 0);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
    }
    for (int i = 0; i < count; i++) {
        free(stus[i].name);
        free(stus[i].note);
    }
    free(stus);
    return 0;
}