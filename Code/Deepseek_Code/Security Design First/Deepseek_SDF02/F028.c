/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stop {
    char *name;
    char *note;
    struct stop *next;
} stop_t;

stop_t *head = NULL;
stop_t *tail = NULL;
int count = 0;

stop_t *find_stop(const char *name) {
    stop_t *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    stop_t *s = malloc(sizeof(stop_t));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
        return;
    }
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    count++;
}

void insert_stop(int idx, const char *name, const char *note) {
    if (idx < 0 || idx > count) return;
    if (find_stop(name)) return;
    stop_t *s = malloc(sizeof(stop_t));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
        return;
    }
    if (idx == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        stop_t *prev = head;
        for (int i = 0; i < idx - 1; i++) prev = prev->next;
        s->next = prev->next;
        prev->next = s;
        if (s->next == NULL) tail = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count || from == to) return;
    // remove from
    stop_t *prev = NULL, *s = head;
    for (int i = 0; i < from; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    if (s == tail) tail = prev;
    // insert at to
    if (to == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        stop_t *p = head;
        for (int i = 0; i < to - 1; i++) p = p->next;
        s->next = p->next;
        p->next = s;
        if (s->next == NULL) tail = s;
    }
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= count) return;
    stop_t *prev = NULL, *s = head;
    for (int i = 0; i < idx; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    if (s == tail) tail = prev;
    free(s->name);
    free(s->note);
    free(s);
    count--;
}

void find_stop_print(const char *name) {
    stop_t *s = find_stop(name);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->note);
}

void print_stops(void) {
    stop_t *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

void free_all(void) {
    stop_t *s = head;
    while (s) {
        stop_t *nxt = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        int idx1, idx2;
        if (sscanf(line, "%9s %d %d %255s", cmd, &idx1, &idx2, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s", a) == 1) {
                    const char *note = rest + strlen(a) + 1;
                    add_stop(a, note);
                }
            } else if (strcmp(cmd, "INSERT") == 0) {
                if (sscanf(line, "%*s %d %255s", &idx1, a) == 2) {
                    const char *note = rest + (rest - line - strlen(cmd) - 1) + 1;
                    while (*note == ' ') note++;
                    insert_stop(idx1, a, note);
                }
            } else if (strcmp(cmd, "MOVE") == 0) {
                if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2)
                    move_stop(idx1, idx2);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                if (sscanf(line, "%*s %d", &idx1) == 1)
                    remove_stop(idx1);
            } else if (strcmp(cmd, "FIND") == 0) {
                if (sscanf(line, "%*s %255s", a) == 1)
                    find_stop_print(a);
            } else if (strcmp(cmd, "PRINT") == 0) {
                print_stops();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}