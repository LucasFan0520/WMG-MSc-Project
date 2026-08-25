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
int size = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= size) return NULL;
    Stop *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void add_stop(char *name, char *note) {
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
    size++;
}

void insert_stop(int index, char *name, char *note) {
    if (index < 0 || index > size) return;
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
    size++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= size || to < 0 || to >= size || from == to) return;
    Stop *prev_from = NULL;
    Stop *curr = head;
    for (int i = 0; i < from; i++) {
        prev_from = curr;
        curr = curr->next;
    }
    if (prev_from) prev_from->next = curr->next;
    else head = curr->next;
    
    if (to == 0) {
        curr->next = head;
        head = curr;
    } else {
        Stop *prev_to = head;
        for (int i = 0; i < to - 1; i++) prev_to = prev_to->next;
        curr->next = prev_to->next;
        prev_to->next = curr;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= size) return;
    Stop *prev = NULL;
    Stop *curr = head;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    free(curr->name);
    free(curr->note);
    free(curr);
    size--;
}

void find_stop(char *name) {
    for (Stop *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
    }
}

void print_stops() {
    for (Stop *s = head; s; s = s->next) {
        printf("%s %s\n", s->name, s->note);
    }
}

int main() {
    char cmd[20], a[100], note[1000];
    int i, j;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_stop(a, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d%99s", &i, a);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            insert_stop(i, a, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            scanf("%d%d", &i, &j);
            move_stop(i, j);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%d", &i);
            remove_stop(i);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_stop(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    return 0;
}