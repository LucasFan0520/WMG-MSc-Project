// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *stops = NULL;

Stop *find_stop(const char *name) {
    Stop *s = stops;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

int stop_count(void) {
    int cnt = 0;
    Stop *s = stops;
    while (s) { cnt++; s = s->next; }
    return cnt;
}

Stop *get_stop_at(int index) {
    int i = 0;
    Stop *s = stops;
    while (s) {
        if (i == index) return s;
        i++;
        s = s->next;
    }
    return NULL;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!stops) {
        stops = s;
    } else {
        Stop *cur = stops;
        while (cur->next) cur = cur->next;
        cur->next = s;
    }
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count()) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = stops;
        stops = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
}

void move_stop(int from, int to) {
    int cnt = stop_count();
    if (from < 0 || from >= cnt || to < 0 || to >= cnt) return;
    if (from == to) return;
    Stop *prev_from = NULL, *cur_from = stops;
    for (int i = 0; i < from; i++) {
        prev_from = cur_from;
        cur_from = cur_from->next;
    }
    // remove from
    if (prev_from) prev_from->next = cur_from->next;
    else stops = cur_from->next;
    // insert at to
    if (to == 0) {
        cur_from->next = stops;
        stops = cur_from;
    } else {
        Stop *prev_to = get_stop_at(to - 1);
        cur_from->next = prev_to->next;
        prev_to->next = cur_from;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count()) return;
    Stop *prev = NULL, *cur = stops;
    for (int i = 0; i < index; i++) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) prev->next = cur->next;
    else stops = cur->next;
    free(cur->name);
    free(cur->note);
    free(cur);
}

void find_stop_print(const char *name) {
    Stop *s = find_stop(name);
    if (!s) return;
    char *n = s->note;
    for (; *n; n++) putchar(*n == ' ' ? '_' : *n);
    putchar('\n');
}

void print_stops(void) {
    Stop *s = stops;
    while (s) {
        printf("%s: ", s->name);
        char *n = s->note;
        for (; *n; n++) putchar(*n == ' ' ? '_' : *n);
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = stops;
    while (s) {
        Stop *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            int index;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) != 1) continue;
            char *name_start = strchr(p, ' ');
            if (!name_start) continue;
            while (*name_start == ' ') name_start++;
            char *name = name_start;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            insert_stop(index, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %d", &from, &to) != 2) continue;
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int index;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) == 1) remove_stop(index);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            find_stop_print(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}