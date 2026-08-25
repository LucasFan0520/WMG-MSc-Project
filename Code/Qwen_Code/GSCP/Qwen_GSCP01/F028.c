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
int stop_count = 0;

Stop *get_stop_at(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    if (!s) exit(EXIT_FAILURE);
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) {
        head = s;
    } else {
        Stop *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) exit(EXIT_FAILURE);
    s->name = strdup(name);
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
    Stop *curr = head;
    Stop *prev = NULL;
    for (int i = 0; i < from; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    
    curr->next = NULL;
    if (to == 0) {
        curr->next = head;
        head = curr;
    } else {
        Stop *prev_to = get_stop_at(to - 1);
        curr->next = prev_to->next;
        prev_to->next = curr;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *to_delete;
    if (index == 0) {
        to_delete = head;
        head = head->next;
    } else {
        Stop *prev = get_stop_at(index - 1);
        to_delete = prev->next;
        prev->next = to_delete->next;
    }
    free(to_delete->name);
    free(to_delete->note);
    free(to_delete);
    stop_count--;
}

void find_stop(const char *name) {
    Stop *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            printf("%s\n", curr->note);
            return;
        }
        curr = curr->next;
    }
}

void print_stops() {
    Stop *curr = head;
    while (curr) {
        printf("%s %s\n", curr->name, curr->note);
        curr = curr->next;
    }
}

void free_all() {
    Stop *curr = head;
    while (curr) {
        Stop *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        int idx1 = 0, idx2 = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (matched >= 2) {
            sscanf(arg1, "%d", &idx1);
        }
        if (matched >= 3) {
            sscanf(arg2, "%d", &idx2);
        }
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 2) {
            add_stop(arg1, arg2);
        } else if (strcmp(cmd, "INSERT") == 0 && matched >= 3) {
            insert_stop(idx1, arg2, arg3);
        } else if (strcmp(cmd, "MOVE") == 0 && matched >= 2) {
            move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}