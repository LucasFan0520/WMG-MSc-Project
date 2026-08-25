/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

Stop *find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_stop(Stop *s) {
    free(s->name);
    free(s->note);
    free(s);
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) {
        head = s;
    } else {
        Stop *last = head;
        while (last->next) last = last->next;
        last->next = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *prev_from = NULL, *curr_from = head;
    for (int i = 0; i < from; i++) { prev_from = curr_from; curr_from = curr_from->next; }
    Stop *prev_to = NULL, *curr_to = head;
    for (int i = 0; i < to; i++) { prev_to = curr_to; curr_to = curr_to->next; }
    if (prev_from) prev_from->next = curr_from->next;
    else head = curr_from->next;
    curr_from->next = curr_to;
    if (prev_to) prev_to->next = curr_from;
    else head = curr_from;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *to_delete;
    if (index == 0) {
        to_delete = head;
        head = head->next;
    } else {
        Stop *prev = get_stop(index - 1);
        to_delete = prev->next;
        prev->next = to_delete->next;
    }
    free_stop(to_delete);
    stop_count--;
}

void find_stop_print(const char *name) {
    Stop *s = find_stop(name);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->note);
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    int index, from, to;
    char name[256], note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            add_stop(name, p);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &index, name) != 2) continue;
            p += (index >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            p += strlen(name);
            while (*p == ' ') p++;
            insert_stop(index, name, p);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %d", &from, &to) == 2) move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) == 1) remove_stop(index);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_stop_print(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }

    while (head) {
        Stop *tmp = head;
        head = head->next;
        free_stop(tmp);
    }
    return 0;
}