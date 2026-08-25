// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
    struct Stop *prev;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int count = 0;

Stop *find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

Stop *get_stop_at(int index) {
    if (index < 0 || index >= count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    s->prev = tail;
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
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->prev = NULL;
        s->next = head;
        if (head) head->prev = s;
        head = s;
        if (!tail) tail = s;
    } else if (index == count) {
        s->prev = tail;
        s->next = NULL;
        if (tail) tail->next = s;
        tail = s;
        if (!head) head = s;
    } else {
        Stop *after = get_stop_at(index);
        Stop *before = after->prev;
        s->prev = before;
        s->next = after;
        before->next = s;
        after->prev = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count || from == to) return;
    Stop *s = get_stop_at(from);
    // remove from current position
    if (s->prev) s->prev->next = s->next;
    else head = s->next;
    if (s->next) s->next->prev = s->prev;
    else tail = s->prev;
    count--;
    // insert at new position
    if (to == 0) {
        s->prev = NULL;
        s->next = head;
        if (head) head->prev = s;
        head = s;
        if (!tail) tail = s;
    } else if (to == count) {
        s->prev = tail;
        s->next = NULL;
        if (tail) tail->next = s;
        tail = s;
        if (!head) head = s;
    } else {
        Stop *after = get_stop_at(to);
        Stop *before = after->prev;
        s->prev = before;
        s->next = after;
        before->next = s;
        after->prev = s;
    }
    count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= count) return;
    Stop *s = get_stop_at(index);
    if (s->prev) s->prev->next = s->next;
    else head = s->next;
    if (s->next) s->next->prev = s->prev;
    else tail = s->prev;
    free(s->name);
    free(s->note);
    free(s);
    count--;
}

void find_stop_print(const char *name) {
    Stop *s = find_stop(name);
    if (!s) return;
    for (char *p = s->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s ", s->name);
        for (char *p = s->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    while (head) {
        Stop *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
    tail = NULL;
    count = 0;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, note) == 2) {
                add_stop(name, note);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int index;
            char name[256], note[1024];
            if (sscanf(line, "%*s %d %255s %1023[^\n]", &index, name, note) == 3) {
                insert_stop(index, name, note);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(line, "%*s %d %d", &from, &to) == 2) {
                move_stop(from, to);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int index;
            if (sscanf(line, "%*s %d", &index) == 1) {
                remove_stop(index);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                find_stop_print(name);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }

    free(line);
    free_all();
    return 0;
}