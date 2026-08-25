// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Stu;

Stu stus[1000];
int sc = 0;

int find_name(char *name) {
    for (int i = 0; i < sc; i++) if (strcmp(stus[i].name, name) == 0) return i;
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
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char name[1000];
            int score;
            sscanf(rest, "%s %d", name, &score);
            char *p = rest;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            int idx = find_name(name);
            if (idx == -1) {
                stus[sc].name = strdup(name);
                stus[sc].score = score;
                stus[sc].note = strdup(p);
                sc++;
            } else {
                stus[idx].score = score;
                free(stus[idx].note);
                stus[idx].note = strdup(p);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(stus[idx].name);
                free(stus[idx].note);
                for (int i = idx; i < sc - 1; i++) stus[i] = stus[i+1];
                sc--;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (sc == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (int i = 0; i < sc; i++) sum += stus[i].score;
                printf("%d\n", sum / sc);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < sc; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
    }
    for (int i = 0; i < sc; i++) {
        free(stus[i].name);
        free(stus[i].note);
    }
    return 0;
}