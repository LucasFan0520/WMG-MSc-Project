// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *time; char *label; char *note; } Activity;
Activity *log = NULL;
int log_count = 0, log_cap = 0;

void add_activity(const char *time, const char *label, const char *note) {
    if (log_count == log_cap) {
        log_cap = log_cap ? log_cap * 2 : 4;
        log = realloc(log, sizeof(Activity) * log_cap);
    }
    log[log_count].time = strdup(time);
    log[log_count].label = strdup(label);
    log[log_count].note = strdup(note);
    log_count++;
}

void delete_activity(const char *time, const char *label) {
    int i = 0;
    while (i < log_count) {
        if (strcmp(log[i].time, time) == 0 && strcmp(log[i].label, label) == 0) {
            free(log[i].time);
            free(log[i].label);
            free(log[i].note);
            for (int j = i; j < log_count - 1; j++) log[j] = log[j + 1];
            log_count--;
        } else i++;
    }
}

void find_label(const char *label) {
    int best = -1;
    for (int i = 0; i < log_count; i++) {
        if (strcmp(log[i].label, label) == 0) {
            if (best == -1 || strcmp(log[i].time, log[best].time) < 0)
                best = i;
        }
    }
    if (best < 0) return;
    printf("%s %s %s\n", log[best].time, log[best].label, log[best].note);
}

int cmp_act(const void *a, const void *b) {
    const Activity *A = (const Activity *)a, *B = (const Activity *)b;
    int c = strcmp(A->time, B->time);
    if (c) return c;
    return strcmp(A->label, B->label);
}

void sort_log(void) {
    qsort(log, log_count, sizeof(Activity), cmp_act);
    for (int i = 0; i < log_count; i++)
        printf("%s %s %s\n", log[i].time, log[i].label, log[i].note);
}

void clear_log(void) {
    for (int i = 0; i < log_count; i++) { free(log[i].time); free(log[i].label); free(log[i].note); }
    log_count = 0;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, rest) == 3)
                add_activity(arg1, arg2, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                delete_activity(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_label(arg1);
        } else if (strcmp(cmd, "SORT") == 0) sort_log();
        else if (strcmp(cmd, "CLEAR") == 0) clear_log();
    }
    for (int i = 0; i < log_count; i++) { free(log[i].time); free(log[i].label); free(log[i].note); }
    free(log);
    return 0;
}