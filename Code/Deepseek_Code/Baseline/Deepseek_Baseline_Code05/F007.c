/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct act {
    char *time;
    char *label;
    char *note;
    struct act *next;
} Act;

Act *head = NULL;

void add_act(const char *time, const char *label, const char *note) {
    Act *a = malloc(sizeof(Act));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = head;
    head = a;
}

void delete_act(const char *time, const char *label) {
    Act *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->time); free(a->label); free(a->note); free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void find_act(const char *label) {
    Act *best = NULL;
    Act *a = head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0 ||
                (strcmp(a->time, best->time) == 0 && strcmp(a->label, best->label) < 0))
                best = a;
        }
        a = a->next;
    }
    if (!best) { printf("NOT_FOUND\n"); return; }
    printf("%s %s ", best->time, best->label);
    for (char *c = best->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

int cmp_act(const void *p1, const void *p2) {
    Act *a = *(Act **)p1, *b = *(Act **)p2;
    int t = strcmp(a->time, b->time);
    if (t) return t;
    return strcmp(a->label, b->label);
}

void sort_print() {
    int count = 0;
    Act *a = head;
    while (a) { count++; a = a->next; }
    if (!count) return;
    Act **arr = malloc(count * sizeof(Act *));
    a = head;
    for (int i = 0; i < count; i++) { arr[i] = a; a = a->next; }
    qsort(arr, count, sizeof(Act *), cmp_act);
    for (int i = 0; i < count; i++) {
        printf("%s %s ", arr[i]->time, arr[i]->label);
        for (char *c = arr[i]->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
    free(arr);
}

void clear_all() {
    Act *a = head;
    while (a) { Act *tmp = a; a = a->next; free(tmp->time); free(tmp->label); free(tmp->note); free(tmp); }
    head = NULL;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], time[256], label[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CLEAR") == 0) { clear_all(); continue; }
        if (strcmp(cmd, "SORT") == 0) { sort_print(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", time, label) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(time) + 1 + strlen(label) + 1;
            if (*rest == ' ') rest++;
            add_act(time, label, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s %255s", time, label) == 2) delete_act(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", label) == 1) find_act(label);
        }
    }
    return 0;
}