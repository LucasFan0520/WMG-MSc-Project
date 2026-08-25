// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *payload; } Job;
Job *queue = NULL;
int qc = 0, qcap = 0;

int find_job(const char *name) {
    for (int i = 0; i < qc; i++)
        if (strcmp(queue[i].name, name) == 0) return i;
    return -1;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name) >= 0) return;
    if (qc == qcap) { qcap = qcap ? qcap * 2 : 4; queue = realloc(queue, sizeof(Job) * qcap); }
    queue[qc].name = strdup(name);
    queue[qc].payload = strdup(payload);
    qc++;
}

void clone_job(const char *old, const char *new) {
    int idx = find_job(old);
    if (idx < 0 || find_job(new) >= 0) return;
    enqueue(new, queue[idx].payload);
}

void cancel_job(const char *name) {
    int idx = find_job(name);
    if (idx < 0) return;
    free(queue[idx].name);
    free(queue[idx].payload);
    for (int i = idx; i < qc - 1; i++) queue[i] = queue[i + 1];
    qc--;
}

void run_job(const char *name) {
    int idx = find_job(name);
    if (idx < 0) return;
    printf("%s\n", queue[idx].payload);
    cancel_job(name);
}

void list_jobs(void) {
    for (int i = 0; i < qc; i++)
        printf("%s %s\n", queue[i].name, queue[i].payload);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                enqueue(arg1, arg2);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) list_jobs();
    }
    for (int i = 0; i < qc; i++) { free(queue[i].name); free(queue[i].payload); }
    free(queue);
    return 0;
}