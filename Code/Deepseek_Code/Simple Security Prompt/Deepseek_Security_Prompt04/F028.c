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
    Stop *cur = head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur;
}

void add_stop(char *stop, char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->stop = strdup(stop);
    s->note = strdup(note);
    s->next = NULL;
    if (!tail) head = tail = s;
    else { tail->next = s; tail = s; }
    count++;
}

void insert_stop(int index, char *stop, char *note) {
    if (index < 0 || index > count) return;
    Stop *s = malloc(sizeof(Stop));
    s->stop = strdup(stop);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else if (index == count) {
        s->next = NULL;
        tail->next = s;
        tail = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count) return;
    if (from == to) return;
    Stop *prev_from = NULL;
    Stop *cur_from = head;
    for (int i = 0; i < from; i++) { prev_from = cur_from; cur_from = cur_from->next; }
    Stop *prev_to = NULL;
    Stop *cur_to = head;
    for (int i = 0; i < to; i++) { prev_to = cur_to; cur_to = cur_to->next; }
    // remove from 'from'
    if (prev_from) prev_from->next = cur_from->next;
    else head = cur_from->next;
    if (cur_from == tail) tail = prev_from;
    // insert before 'to'
    if (to == 0) {
        cur_from->next = head;
        head = cur_from;
    } else {
        if (to > from) to--; // because we removed one
        Stop *p = get_stop_at(to - 1);
        cur_from->next = p->next;
        p->next = cur_from;
        if (cur_from->next == NULL) tail = cur_from;
    }
    // count unchanged
}

void remove_stop(int index) {
    if (index < 0 || index >= count) return;
    Stop *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        Stop *prev = get_stop_at(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (to_del == tail) tail = prev;
    }
    free(to_del->stop);
    free(to_del->note);
    free(to_del);
    count--;
}

void find_stop(char *stop) {
    for (Stop *s = head; s; s = s->next) {
        if (strcmp(s->stop, stop) == 0) {
            printf("%s\n", s->note);
            return;
        }
    }
}

void print_stops(void) {
    for (Stop *s = head; s; s = s->next)
        printf("%s %s\n", s->stop, s->note);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_stop(arg1, arg2);
        } else if (strcmp(cmd, "INSERT") == 0 && n == 4) {
            insert_stop(atoi(arg1), arg2, arg3);
        } else if (strcmp(cmd, "MOVE") == 0 && n == 3) {
            move_stop(atoi(arg1), atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_stop(atoi(arg1));
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_stops();
        }
    }
    free(line);
    free_all();
    return 0;
}