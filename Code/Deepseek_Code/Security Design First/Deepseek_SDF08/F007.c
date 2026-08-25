// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct activity {
    char *time;
    char *label;
    char *note;
    struct activity *next;
} Activity;

Activity *head = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) { fprintf(stderr, "Memory error\n"); exit(1); }
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    if (!a->time || !a->label || !a->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    a->next = head;
    head = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            a = prev ? prev->next : head;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void find_label(const char *label) {
    Activity *a = head, *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) earliest = a;
        }
        a = a->next;
    }
    if (earliest) {
        for (char *s = earliest->note; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
    }
}

int cmp_activity(const void *p1, const void *p2) {
    Activity *a1 = *(Activity**)p1;
    Activity *a2 = *(Activity**)p2;
    int cmp = strcmp(a1->time, a2->time);
    if (cmp != 0) return cmp;
    return strcmp(a1->label, a2->label);
}

void sort_activities(void) {
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity*));
    if (!arr) { fprintf(stderr, "Memory error\n"); exit(1); }
    a = head;
    for (int i = 0; i < count; i++) { arr[i] = a; a = a->next; }
    qsort(arr, count, sizeof(Activity*), cmp_activity);
    for (int i = 0; i < count; i++) {
        printf("%s %s ", arr[i]->time, arr[i]->label);
        for (char *s = arr[i]->note; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
    }
    free(arr);
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
    head = NULL;
}

void free_all(void) {
    clear_all();
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "SORT") == 0) { sort_activities(); continue; }
        if (strcmp(cmd, "CLEAR") == 0) { clear_all(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char *time = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *label = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            add_activity(time, label, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *label = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_label(rest);
        }
    }
    free_all();
    return 0;
}