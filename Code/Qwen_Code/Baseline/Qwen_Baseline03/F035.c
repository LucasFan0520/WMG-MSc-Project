// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char payload[1000];
} Job;

Job jobs[1000];
int jn = 0;

int findJob(char *name) {
    for (int i = 0; i < jn; i++) {
        if (!strcmp(jobs[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], old[100], new[100], payload[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ENQUEUE")) {
            scanf("%s", name);
            fgets(payload, 1000, stdin);
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            payload[strcspn(payload, "\n")] = 0;
            strcpy(jobs[jn].name, name);
            strcpy(jobs[jn].payload, payload);
            jn++;
        } else if (!strcmp(cmd, "CLONE")) {
            scanf("%s %s", old, new);
            int i = findJob(old);
            if (i != -1) {
                strcpy(jobs[jn].name, new);
                strcpy(jobs[jn].payload, jobs[i].payload);
                jn++;
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            scanf("%s", name);
            int i = findJob(name);
            if (i != -1) {
                for (int j = i; j < jn - 1; j++) {
                    jobs[j] = jobs[j + 1];
                }
                jn--;
            }
        } else if (!strcmp(cmd, "RUN")) {
            scanf("%s", name);
            int i = findJob(name);
            if (i != -1) {
                printf("%s\n", jobs[i].payload);
                for (int j = i; j < jn - 1; j++) {
                    jobs[j] = jobs[j + 1];
                }
                jn--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < jn; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    return 0;
}