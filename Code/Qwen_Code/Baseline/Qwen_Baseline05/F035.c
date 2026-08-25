// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_PAYLOAD 1000

typedef struct {
    char name[MAX_NAME];
    char payload[MAX_PAYLOAD];
} Job;

Job queue[10000];
int q_count = 0;

int find_job(const char *name) {
    for (int i = 0; i < q_count; i++) if (strcmp(queue[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], payload[MAX_PAYLOAD], oldName[MAX_NAME], newName[MAX_NAME];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            scanf("%s", name);
            fgets(payload, MAX_PAYLOAD, stdin);
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            if (payload[strlen(payload) - 1] == '\n') payload[strlen(payload) - 1] = '\0';
            strcpy(queue[q_count].name, name);
            strcpy(queue[q_count].payload, payload);
            q_count++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            scanf("%s %s", oldName, newName);
            int idx = find_job(oldName);
            if (idx != -1) {
                strcpy(queue[q_count].name, newName);
                strcpy(queue[q_count].payload, queue[idx].payload);
                q_count++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                for (int i = idx; i < q_count - 1; i++) queue[i] = queue[i + 1];
                q_count--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                printf("%s\n", queue[idx].payload);
                for (int i = idx; i < q_count - 1; i++) queue[i] = queue[i + 1];
                q_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < q_count; i++) printf("%s %s\n", queue[i].name, queue[i].payload);
        }
    }
    return 0;
}