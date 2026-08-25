// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Act { char *time; char *label; char *note; struct Act *next; } Act;
Act *acts = NULL;

void add_act(const char *time, const char *label, const char *note) {
    Act *a = malloc(sizeof(Act));
    if (!a) return;
    a->time = strdup(time); a->label = strdup(label); a->note = strdup(note);
    a->next = acts;
    acts = a;
}

void delete_act(const char *time, const char *label) {
    Act *prev = NULL;
    for (Act *a = acts; a; prev = a, a = a->next) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next; else acts = a->next;
            free(a->time); free(a->label); free(a->note); free(a);
            a = prev ? prev : acts; // continue
        }
    }
}

void find_label(const char *label) {
    Act *best = NULL;
    for (Act *a = acts; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0) best = a;
        }
    }
    if (!best) { printf("NOT_FOUND\n"); return; }
    printf("%s %s ", best->time, best->label);
    for (char *c = best->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

int cmp_act(const void *a, const void *b) {
    Act *A = *(Act **)a, *B = *(Act **)b;
    int t = strcmp(A->time, B->time);
    if (t) return t;
    return strcmp(A->label, B->label);
}

void sort_acts(void) {
    int count = 0;
    for (Act *a = acts; a; a = a->next) count++;
    if (count < 2) { print_acts(); return; }
    Act **arr = malloc(count * sizeof(Act *));
    if (!arr) return;
    int i = 0;
    for (Act *a = acts; a; a = a->next) arr[i++] = a;
    qsort(arr, count, sizeof(Act *), cmp_act);
    for (i = 0; i < count; i++) {
        printf("%s %s ", arr[i]->time, arr[i]->label);
        for (char *c = arr[i]->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
    free(arr);
}

void print_acts(void) {
    for (Act *a = acts; a; a = a->next) {
        printf("%s %s ", a->time, a->label);
        for (char *c = a->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

void clear_acts(void) {
    while (acts) { Act *a = acts; acts = a->next; free(a->time); free(a->label); free(a->note); free(a); }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char time[256], label[256], note[4096];
            if (sscanf(rest, "%255s %255s", time, label) == 2) {
                char *n = rest + strlen(time) + 1 + strlen(label);
                while (*n == ' ') n++;
                strcpy(note, n);
                add_act(time, label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[256], label[256];
            if (sscanf(rest, "%255s %255s", time, label) == 2) delete_act(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256];
            if (sscanf(rest, "%255s", label) == 1) find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_acts();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_acts();
        }
    }
    clear_acts();
    return 0;
}