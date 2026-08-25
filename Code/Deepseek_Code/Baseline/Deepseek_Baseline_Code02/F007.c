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
Activity *tail = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = malloc(strlen(time) + 1);
    strcpy(a->time, time);
    a->label = malloc(strlen(label) + 1);
    strcpy(a->label, label);
    a->note = malloc(strlen(note) + 1);
    strcpy(a->note, note);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            Activity *next = a->next;
            if (prev) prev->next = next;
            else head = next;
            if (tail == a) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            a = next;
            continue;
        }
        prev = a;
        a = a->next;
    }
}

void find_label(const char *label) {
    Activity *a = head;
    Activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) {
                earliest = a;
            }
        }
        a = a->next;
    }
    if (!earliest) return;
    printf("%s %s ", earliest->time, earliest->label);
    for (char *c = earliest->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

int compare_activities(const void *a, const void *b) {
    Activity *aa = *(Activity**)a;
    Activity *bb = *(Activity**)b;
    int cmp = strcmp(aa->time, bb->time);
    if (cmp != 0) return cmp;
    return strcmp(aa->label, bb->label);
}

void sort_activities(void) {
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count < 2) {
        print_activities();
        return;
    }
    Activity **arr = malloc(count * sizeof(Activity*));
    int i = 0;
    a = head;
    while (a) { arr[i++] = a; a = a->next; }
    qsort(arr, count, sizeof(Activity*), compare_activities);
    for (i = 0; i < count; i++) {
        Activity *p = arr[i];
        printf("%s %s ", p->time, p->label);
        for (char *c = p->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
    free(arr);
}

void print_activities(void) {
    Activity *a = head;
    while (a) {
        printf("%s %s ", a->time, a->label);
        for (char *c = a->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        a = a->next;
    }
}

void clear_all(void) {
    Activity *a = head;
    while (a) {
        Activity *next = a->next;
        free(a->time);
        free(a->label);
        free(a->note);
        free(a);
        a = next;
    }
    head = tail = NULL;
}

void free_all(void) {
    clear_all();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *time = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_activity(time, label, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *time = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            delete_activity(time, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_label(p);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    return 0;
}