// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct act {
    char *time;
    char *label;
    char *note;
    struct act *next;
} Act;

Act *log = NULL;

void add_act(const char *time, const char *label, const char *note) {
    Act *a = malloc(sizeof(Act));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = log;
    log = a;
}

void delete_act(const char *time, const char *label) {
    Act *a = log, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else log = a->next;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void find_act(const char *label) {
    Act *a = log, *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0)
                earliest = a;
        }
        a = a->next;
    }
    if (earliest) printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
}

int act_cmp(const void *x, const void *y) {
    Act *a = *(Act**)x, *b = *(Act**)y;
    int t = strcmp(a->time, b->time);
    if (t != 0) return t;
    return strcmp(a->label, b->label);
}

void sort_act(void) {
    int n = 0;
    Act *a = log;
    while (a) { n++; a = a->next; }
    if (n == 0) return;
    Act **arr = malloc(n * sizeof(Act*));
    a = log;
    for (int i = 0; i < n; i++, a = a->next) arr[i] = a;
    qsort(arr, n, sizeof(Act*), act_cmp);
    for (int i = 0; i < n; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_log(void) {
    Act *a = log;
    while (a) {
        Act *tmp = a;
        a = a->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
    log = NULL;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %[^\n]", arg1, arg2, arg3) == 3)
                add_act(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                delete_act(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                find_act(arg1);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_act();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    return 0;
}