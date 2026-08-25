// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_Q 10000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char payload[MAX_LEN];
} Job;

Job q[MAX_Q];
int qcount = 0;

int find_job(const char *name) {
    for (int i = 0; i < qcount; i++) if (strcmp(q[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], old_name[100], new_name[100], payload[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ENQUEUE") == 0) {
            scanf("%s", name);
            fgets(payload, MAX_LEN, stdin);
            payload[strcspn(payload, "\n")] = 0;
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            if (find_job(name) == -1 && qcount < MAX_Q) {
                strcpy(q[qcount].name, name);
                strcpy(q[qcount].payload, payload);
                qcount++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            scanf("%s %s", old_name, new_name);
            int idx = find_job(old_name);
            if (idx != -1 && find_job(new_name) == -1 && qcount < MAX_Q) {
                strcpy(q[qcount].name, new_name);
                strcpy(q[qcount].payload, q[idx].payload);
                qcount++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                for (int i = idx; i < qcount - 1; i++) q[i] = q[i + 1];
                qcount--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                printf("%s\n", q[idx].payload);
                for (int i = idx; i < qcount - 1; i++) q[i] = q[i + 1];
                qcount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < qcount; i++) {
                printf("%s %s\n", q[i].name, q[i].payload);
            }
        }
    }
    return 0;
}