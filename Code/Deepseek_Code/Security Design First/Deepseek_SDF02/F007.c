/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct act {
    char *time;
    char *label;
    char *note;
    struct act *next;
} act_t;

act_t *head = NULL;

void add_act(const char *time, const char *label, const char *note) {
    act_t *a = malloc(sizeof(act_t));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = head;
    head = a;
    if (!a->time || !a->label || !a->note) {
        free(a->time); free(a->label); free(a->note); free(a);
    }
}

void delete_act(const char *time, const char *label) {
    act_t *prev = NULL, *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            a = prev ? prev->next : head;
            continue;
        }
        prev = a;
        a = a->next;
    }
}

void find_earliest(const char *label) {
    act_t *best = NULL;
    act_t *a = head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0 ||
                (strcmp(a->time, best->time) == 0 && strcmp(a->label, best->label) < 0))
                best = a;
        }
        a = a->next;
    }
    if (!best) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", best->time, best->label, best->note);
}

int cmp_act(const void *a, const void *b) {
    const act_t *aa = *(const act_t**)a;
    const act_t *bb = *(const act_t**)b;
    int t = strcmp(aa->time, bb->time);
    if (t) return t;
    return strcmp(aa->label, bb->label);
}

void sort_print(void) {
    int n = 0;
    act_t *a = head;
    while (a) { n++; a = a->next; }
    if (n == 0) return;
    act_t **arr = malloc(sizeof(act_t*) * n);
    if (!arr) return;
    int i = 0;
    a = head;
    while (a) { arr[i++] = a; a = a->next; }
    qsort(arr, n, sizeof(act_t*), cmp_act);
    for (i = 0; i < n; i++)
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    free(arr);
}

void clear_all(void) {
    act_t *a = head;
    while (a) {
        act_t *nxt = a->next;
        free(a->time);
        free(a->label);
        free(a->note);
        free(a);
        a = nxt;
    }
    head = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(a) + 1 + strlen(b) + 1;
                    add_act(a, b, note);
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    delete_act(a, b);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_earliest(a);
            } else if (strcmp(cmd, "SORT") == 0) {
                sort_print();
            } else if (strcmp(cmd, "CLEAR") == 0) {
                clear_all();
            }
        }
    }
    free(line);
    clear_all();
    return 0;
}