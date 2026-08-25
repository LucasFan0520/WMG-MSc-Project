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

Stop* get_stop(int index) {
    Stop *s = head;
    for (int i = 0; s && i < index; i++) s = s->next;
    return s;
}

void free_stop(Stop *s) {
    free(s->name);
    free(s->note);
    free(s);
}

void handle_add(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    Stop *new = malloc(sizeof(Stop));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Stop *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
    stop_count++;
}

void handle_insert(char *cmd) {
    char *idxstr = strtok(NULL, " ");
    if (!idxstr) return;
    int idx = atoi(idxstr);
    if (idx < 0) return;
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    if (idx > stop_count) return; // invalid, ignore
    Stop *new = malloc(sizeof(Stop));
    new->name = strdup(name);
    new->note = strdup(note);
    if (idx == 0) {
        new->next = head;
        head = new;
    } else {
        Stop *prev = get_stop(idx - 1);
        if (!prev) { free(new->name); free(new->note); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
    stop_count++;
}

void handle_move(char *cmd) {
    char *fromstr = strtok(NULL, " ");
    char *tostr = strtok(NULL, " ");
    if (!fromstr || !tostr) return;
    int from = atoi(fromstr);
    int to = atoi(tostr);
    if (from < 0 || to < 0 || from >= stop_count || to >= stop_count) return;
    if (from == to) return;
    Stop *prev_from = NULL, *from_node = head;
    for (int i = 0; i < from; i++) { prev_from = from_node; from_node = from_node->next; }
    Stop *prev_to = NULL, *to_node = head;
    for (int i = 0; i < to; i++) { prev_to = to_node; to_node = to_node->next; }
    // Remove from_node
    if (prev_from) prev_from->next = from_node->next;
    else head = from_node->next;
    // Insert before to_node
    if (to == 0) {
        from_node->next = head;
        head = from_node;
    } else {
        // If to > from, the target position may have shifted because we removed from_node.
        // But since we already removed, we need to find the new position.
        // Recompute prev_to and to_node after removal.
        if (to > from) {
            // The node at original 'to' is now at index 'to-1'
            // We need to insert after the node at index 'to-1'
            Stop *new_prev = get_stop(to - 1);
            if (!new_prev) { // should not happen
                // reinsert at end? but we can just put at head?
                from_node->next = head;
                head = from_node;
            } else {
                from_node->next = new_prev->next;
                new_prev->next = from_node;
            }
        } else { // to < from, original 'to' still valid
            Stop *new_prev = get_stop(to - 1);
            if (!new_prev) { // to == 0 handled
                from_node->next = head;
                head = from_node;
            } else {
                from_node->next = new_prev->next;
                new_prev->next = from_node;
            }
        }
    }
}

void handle_remove_index(char *cmd) {
    char *idxstr = strtok(NULL, " ");
    if (!idxstr) return;
    int idx = atoi(idxstr);
    if (idx < 0 || idx >= stop_count) return;
    Stop *s = head, *prev = NULL;
    for (int i = 0; i < idx; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    free_stop(s);
    stop_count--;
}

void handle_find(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void handle_print() {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "INSERT") == 0) handle_insert(cmd);
        else if (strcmp(cmd, "MOVE") == 0) handle_move(cmd);
        else if (strcmp(cmd, "REMOVE") == 0) handle_remove_index(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "PRINT") == 0) handle_print();
    }
    while (head) {
        Stop *tmp = head;
        head = head->next;
        free_stop(tmp);
    }
    return 0;
}