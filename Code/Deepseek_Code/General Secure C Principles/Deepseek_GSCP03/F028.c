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
Stop *tail = NULL;
int count = 0;

Stop *get_stop_at(int index) {
    if (index < 0 || index >= count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *stop, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->stop = strdup(stop);
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    count++;
}

void insert_stop(int index, const char *stop, const char *note) {
    if (index < 0 || index > count) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->stop = strdup(stop);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else if (index == count) {
        s->next = NULL;
        if (tail) tail->next = s;
        else head = s;
        tail = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        if (prev) {
            s->next = prev->next;
            prev->next = s;
        } else {
            free(s->stop);
            free(s->note);
            free(s);
            return;
        }
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count || from == to) return;
    Stop *prev_from = NULL, *curr_from = head;
    for (int i = 0; i < from; i++) {
        prev_from = curr_from;
        curr_from = curr_from->next;
    }
    // remove from current
    if (prev_from) prev_from->next = curr_from->next;
    else head = curr_from->next;
    if (curr_from == tail) tail = prev_from;
    count--;
    // insert at 'to'
    if (to == 0) {
        curr_from->next = head;
        head = curr_from;
    } else {
        Stop *prev_to = get_stop_at(to - 1);
        if (prev_to) {
            curr_from->next = prev_to->next;
            prev_to->next = curr_from;
        } else {
            // should not happen
            curr_from->next = head;
            head = curr_from;
        }
    }
    if (!curr_from->next) tail = curr_from;
    count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= count) return;
    Stop *prev = NULL, *curr = head;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    if (curr == tail) tail = prev;
    free(curr->stop);
    free(curr->note);
    free(curr);
    count--;
}

void find_stop(const char *stop) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->stop, stop) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
    printf("NOT_FOUND\n");
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->stop, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *tmp = s;
        s = s->next;
        free(tmp->stop);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", stop) == 1) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    add_stop(stop, note);
                }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char stop[256];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &idx, stop) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    insert_stop(idx, stop, note);
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(line + 5, "%d %d", &from, &to) == 2) {
                move_stop(from, to);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1) {
                remove_stop(idx);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_stop(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}