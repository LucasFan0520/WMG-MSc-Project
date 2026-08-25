// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop { char *name; char *note; struct Stop *next; } Stop;
Stop *head = NULL, *tail = NULL;

int count_stops(void) {
    int c = 0;
    for (Stop *s = head; s; s = s->next) c++;
    return c;
}

Stop *find_stop(const char *name) {
    for (Stop *s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s; else head = s;
    tail = s;
}

void insert_stop(int index, const char *name, const char *note) {
    int len = count_stops();
    if (index < 0 || index > len) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev = head;
        for (int i = 0; i < index - 1; i++) prev = prev->next;
        s->next = prev->next;
        prev->next = s;
        if (s->next == NULL) tail = s;
    }
}

void move_stop(int from, int to) {
    int len = count_stops();
    if (from < 0 || from >= len || to < 0 || to >= len || from == to) return;
    // remove from 'from'
    Stop *prev = NULL, *cur = head;
    for (int i = 0; i < from; i++) { prev = cur; cur = cur->next; }
    if (prev) prev->next = cur->next; else head = cur->next;
    if (cur == tail) tail = prev;
    // insert at 'to'
    if (to == 0) {
        cur->next = head;
        head = cur;
        if (!tail) tail = cur;
    } else {
        Stop *p = head;
        for (int i = 0; i < to - 1; i++) p = p->next;
        cur->next = p->next;
        p->next = cur;
        if (cur->next == NULL) tail = cur;
    }
}

void remove_stop(int index) {
    int len = count_stops();
    if (index < 0 || index >= len) return;
    Stop *prev = NULL, *cur = head;
    for (int i = 0; i < index; i++) { prev = cur; cur = cur->next; }
    if (prev) prev->next = cur->next; else head = cur->next;
    if (cur == tail) tail = prev;
    free(cur->name); free(cur->note); free(cur);
}

void find_stop_print(const char *name) {
    Stop *s = find_stop(name);
    if (!s) { printf("NOT_FOUND\n"); return; }
    for (char *c = s->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_stops(void) {
    for (Stop *s = head; s; s = s->next)
        printf("%s\n", s->name);
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
            char name[256], note[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *n = rest + strlen(name);
                while (*n == ' ') n++;
                strcpy(note, n);
                add_stop(name, note);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int index; char name[256], note[4096];
            if (sscanf(rest, "%d %255s", &index, name) == 2) {
                char *n = rest;
                while (*n == ' ') n++;
                while (*n >= '0' && *n <= '9') n++;
                while (*n == ' ') n++;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                strcpy(note, n);
                insert_stop(index, name, note);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(rest, "%d %d", &from, &to) == 2) move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int index;
            if (sscanf(rest, "%d", &index) == 1) remove_stop(index);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_stop_print(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    while (head) { Stop *s = head; head = s->next; free(s->name); free(s->note); free(s); }
    return 0;
}