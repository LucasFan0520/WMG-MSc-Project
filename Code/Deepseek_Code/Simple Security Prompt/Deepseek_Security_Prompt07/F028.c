// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (tail) {
        tail->next = s;
        tail = s;
    } else {
        head = tail = s;
    }
    count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > count) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
        if (!s->next) tail = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count) return;
    if (from == to) return;
    Stop *prev_from = NULL;
    Stop *s = head;
    for (int i = 0; i < from; i++) { prev_from = s; s = s->next; }
    if (prev_from) prev_from->next = s->next;
    else head = s->next;
    if (!s->next) tail = prev_from;
    count--;
    if (to > from) to--;
    if (to == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev_to = head;
        for (int i = 0; i < to - 1; i++) prev_to = prev_to->next;
        s->next = prev_to->next;
        prev_to->next = s;
        if (!s->next) tail = s;
    }
    count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= count) return;
    Stop *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        Stop *prev = get_stop(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (!prev->next) tail = prev;
    }
    free(to_del->name);
    free(to_del->note);
    free(to_del);
    count--;
}

void find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            for (char *p = s->note; *p; p++) putchar(*p == ' ' ? '_' : *p);
            putchar('\n');
            return;
        }
        s = s->next;
    }
    printf("NOT_FOUND\n");
}

void print_stops() {
    Stop *s = head;
    while (s) {
        printf("%s:", s->name);
        for (char *p = s->note; *p; p++) putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
        s = s->next;
    }
}

void free_all() {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                add_stop(name, note);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char name[256], note[4096];
            if (sscanf(rest, "%d %255s %[^\n]", &idx, name, note) == 3) {
                insert_stop(idx, name, note);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(rest, "%d %d", &from, &to) == 2) {
                move_stop(from, to);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) == 1) {
                remove_stop(idx);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            find_stop(rest);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}