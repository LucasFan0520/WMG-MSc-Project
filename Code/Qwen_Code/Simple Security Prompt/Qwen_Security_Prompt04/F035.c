// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job jobs[10000];
int jc = 0;

int find_name(char *name) {
    for (int i = 0; i < jc; i++) if (strcmp(jobs[i].name, name) == 0) return i;
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
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *payload = rest + strlen(name);
            while (*payload == ' ') payload++;
            if (find_name(name) == -1) {
                jobs[jc].name = strdup(name);
                jobs[jc].payload = strdup(payload);
                jc++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old_name[1000], new_name[1000];
            sscanf(rest, "%s %s", old_name, new_name);
            int idx = find_name(old_name);
            if (idx != -1 && find_name(new_name) == -1) {
                jobs[jc].name = strdup(new_name);
                jobs[jc].payload = strdup(jobs[idx].payload);
                jc++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < jc - 1; i++) jobs[i] = jobs[i+1];
                jc--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                printf("%s\n", jobs[idx].payload);
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < jc - 1; i++) jobs[i] = jobs[i+1];
                jc--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < jc; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < jc; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    return 0;
}