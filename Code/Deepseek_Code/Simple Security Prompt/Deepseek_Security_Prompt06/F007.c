// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = malloc(strlen(time) + 1);
    strcpy(a->time, time);
    a->label = malloc(strlen(label) + 1);
    strcpy(a->label, label);
    a->note = malloc(strlen(note) + 1);
    strcpy(a->note, note);
    a->next = head;
    head = a;
}

void delete_activities(const char *time, const char *label) {
    Activity *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->time, time) == 0 && strcmp(p->label, label) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->time);
            free(p->label);
            free(p->note);
            free(p);
            p = prev ? prev->next : head;
            continue;
        }
        prev = p;
        p = p->next;
    }
}

void find_activity(const char *label) {
    Activity *best = NULL;
    Activity *p = head;
    while (p) {
        if (strcmp(p->label, label) == 0) {
            if (!best || strcmp(p->time, best->time) < 0) best = p;
        }
        p = p->next;
    }
    if (best) printf("%s %s %s\n", best->time, best->label, best->note);
}

int cmp_act(const void *a, const void *b) {
    Activity *const *pa = (Activity *const *)a;
    Activity *const *pb = (Activity *const *)b;
    int cmp = strcmp((*pa)->time, (*pb)->time);
    if (cmp == 0) cmp = strcmp((*pa)->label, (*pb)->label);
    return cmp;
}

void sort_activities(void) {
    int n = 0;
    Activity *p = head;
    while (p) { n++; p = p->next; }
    if (n == 0) return;
    Activity **arr = malloc(n * sizeof(Activity *));
    p = head;
    for (int i = 0; i < n; i++) { arr[i] = p; p = p->next; }
    qsort(arr, n, sizeof(Activity *), cmp_act);
    for (int i = 0; i < n; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_all(void) {
    Activity *p = head;
    while (p) {
        Activity *tmp = p->next;
        free(p->time);
        free(p->label);
        free(p->note);
        free(p);
        p = tmp;
    }
    head = NULL;
}

void free_all(void) {
    clear_all();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) {
                const char *note = rest + strlen(a) + 1 + strlen(b) + 1;
                while (*note == ' ') note++;
                if (*note) add_activity(a, b, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) delete_activities(a, b);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", a) == 1) find_activity(a);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    return 0;
}