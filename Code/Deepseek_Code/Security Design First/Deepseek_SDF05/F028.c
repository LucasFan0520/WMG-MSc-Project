// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int stop_count = 0;

Stop *get_stop_at(int index) {
    Stop *cur = head;
    for (int i = 0; i < index && cur; i++) cur = cur->next;
    return cur;
}

Stop *find_stop(const char *name) {
    Stop *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void append_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = safe_strdup(name);
    s->note = safe_strdup(note);
    s->next = NULL;
    if (tail) {
        tail->next = s;
        tail = s;
    } else {
        head = tail = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = safe_strdup(name);
    s->note = safe_strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        if (!prev) { free(s->name); free(s->note); free(s); return; }
        s->next = prev->next;
        prev->next = s;
        if (s->next == NULL) tail = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count) return;
    if (from == to) return;
    Stop *prev_from = NULL, *cur = head;
    for (int i = 0; i < from; i++) { prev_from = cur; cur = cur->next; }
    // remove cur
    if (prev_from) prev_from->next = cur->next;
    else head = cur->next;
    if (cur == tail) tail = prev_from;
    // adjust to if needed
    if (to > from) to--;
    // insert at to
    if (to == 0) {
        cur->next = head;
        head = cur;
        if (!head->next) tail = head;
    } else {
        Stop *prev_to = get_stop_at(to - 1);
        if (!prev_to) { /* should not happen */ return; }
        cur->next = prev_to->next;
        prev_to->next = cur;
        if (cur->next == NULL) tail = cur;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *prev = NULL, *cur = head;
    for (int i = 0; i < index; i++) { prev = cur; cur = cur->next; }
    if (prev) prev->next = cur->next;
    else head = cur->next;
    if (cur == tail) tail = prev;
    free(cur->name);
    free(cur->note);
    free(cur);
    stop_count--;
}

void find_stop_cmd(const char *name) {
    Stop *s = find_stop(name);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->note);
}

void print_stops(void) {
    Stop *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void free_all(void) {
    Stop *cur = head;
    while (cur) {
        Stop *next = cur->next;
        free(cur->name);
        free(cur->note);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            append_stop(arg1, p);
        } else if (strcmp(cmd, "INSERT") == 0 && n >= 3) {
            int index = atoi(arg1);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            insert_stop(index, arg2, p);
        } else if (strcmp(cmd, "MOVE") == 0 && n >= 3) {
            int from = atoi(arg1);
            int to = atoi(arg2);
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            int index = atoi(arg1);
            remove_stop(index);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_stop_cmd(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}