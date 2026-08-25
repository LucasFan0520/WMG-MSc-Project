/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *stop;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
int stop_count = 0;

Stop *get_stop_at(int index) {
    Stop *s = head;
    int i = 0;
    while (s && i < index) {
        s = s->next;
        i++;
    }
    return s;
}

void add_stop(const char *stop, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->stop = strdup(stop);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) head = s;
    else {
        Stop *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *stop, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    s->stop = strdup(stop);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *prev_from = NULL, *from_node = head;
    for (int i = 0; i < from; i++) {
        prev_from = from_node;
        from_node = from_node->next;
    }
    if (prev_from) prev_from->next = from_node->next;
    else head = from_node->next;
    Stop *prev_to = NULL, *to_node = head;
    for (int i = 0; i < to; i++) {
        prev_to = to_node;
        to_node = to_node->next;
    }
    if (prev_to) {
        from_node->next = prev_to->next;
        prev_to->next = from_node;
    } else {
        from_node->next = head;
        head = from_node;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *s = head, *prev = NULL;
    for (int i = 0; i < index; i++) {
        prev = s;
        s = s->next;
    }
    if (prev) prev->next = s->next;
    else head = s->next;
    free(s->stop);
    free(s->note);
    free(s);
    stop_count--;
}

void find_stop(const char *stop) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->stop, stop) == 0) {
            for (char *c = s->note; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            return;
        }
        s = s->next;
    }
    printf("NOT_FOUND\n");
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s ", s->stop);
        for (char *c = s->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->stop);
        free(s->note);
        free(s);
        s = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            char *p = line + 4;
            if (sscanf(p, "%255s", stop) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_stop(stop, p);
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char stop[256];
            char *p = line + 7;
            if (sscanf(p, "%d %255s", &idx, stop) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            insert_stop(idx, stop, p);
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(line + 5, "%d %d", &from, &to) == 2)
                move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1)
                remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_stop(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    free(line);
    return 0;
}