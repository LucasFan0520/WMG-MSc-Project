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

void free_list() {
    while (head) {
        Stop *s = head; head = head->next;
        free(s->name); free(s->note); free(s);
    }
    stop_count = 0;
}

Stop *get_stop(int index) {
    Stop *s = head;
    for (int i = 0; i < index && s; i++) s = s->next;
    return s;
}

Stop *find_stop(const char *name) {
    for (Stop *s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], note[4096] = "";
        int idx1, idx2;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) < 1) continue;
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name); s->note = strdup(note); s->next = NULL;
            if (!head) head = s;
            else {
                Stop *tail = head;
                while (tail->next) tail = tail->next;
                tail->next = s;
            }
            stop_count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &idx1, name, note) < 2) continue;
            if (idx1 < 0 || idx1 > stop_count) continue;
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name); s->note = strdup(note);
            if (idx1 == 0) {
                s->next = head; head = s;
            } else {
                Stop *prev = get_stop(idx1 - 1);
                s->next = prev->next; prev->next = s;
            }
            stop_count++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) != 2) continue;
            if (idx1 < 0 || idx1 >= stop_count || idx2 < 0 || idx2 >= stop_count) continue;
            if (idx1 == idx2) continue;
            Stop *prev1 = NULL, *curr1 = head;
            for (int i = 0; i < idx1; i++) { prev1 = curr1; curr1 = curr1->next; }
            if (prev1) prev1->next = curr1->next; else head = curr1->next;
            
            Stop *prev2 = NULL, *curr2 = head;
            for (int i = 0; i < idx2; i++) { prev2 = curr2; curr2 = curr2->next; }
            if (prev2) {
                curr1->next = prev2->next;
                prev2->next = curr1;
            } else {
                curr1->next = head;
                head = curr1;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) != 1) continue;
            if (idx1 < 0 || idx1 >= stop_count) continue;
            Stop *prev = NULL, *curr = head;
            for (int i = 0; i < idx1; i++) { prev = curr; curr = curr->next; }
            if (prev) prev->next = curr->next; else head = curr->next;
            free(curr->name); free(curr->note); free(curr);
            stop_count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Stop *s = find_stop(name);
            if (s) printf("%s\n", s->note);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Stop *s = head; s; s = s->next) {
                printf("%s %s\n", s->name, s->note);
            }
        }
    }
    free_list();
    return 0;
}