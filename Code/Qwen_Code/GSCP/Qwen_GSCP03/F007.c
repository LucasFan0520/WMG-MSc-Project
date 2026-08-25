// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Act {
    int time;
    char *label, *note;
    struct Act *next;
} Act;

Act *head = NULL;

void free_list() {
    while (head) {
        Act *t = head; head = head->next;
        free(t->label); free(t->note); free(t);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], label[256], note[4096] = "";
        int time;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &time, label, note) < 2) continue;
            Act *a = malloc(sizeof(Act));
            a->time = time; a->label = strdup(label); a->note = strdup(note);
            a->next = head; head = a;
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d %255s", &time, label) != 2) continue;
            Act *prev = NULL, *curr = head;
            while (curr) {
                if (curr->time == time && strcmp(curr->label, label) == 0) {
                    Act *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->label); free(tmp->note); free(tmp);
                } else { prev = curr; curr = curr->next; }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", label) != 1) continue;
            Act *earliest = NULL;
            for (Act *a = head; a; a = a->next) {
                if (strcmp(a->label, label) == 0) {
                    if (!earliest || a->time < earliest->time) earliest = a;
                }
            }
            if (earliest) printf("%d %s %s\n", earliest->time, earliest->label, earliest->note);
        } else if (strcmp(cmd, "SORT") == 0) {
            int swapped = 1;
            while (swapped) {
                swapped = 0;
                Act *curr = head, *prev = NULL;
                while (curr && curr->next) {
                    Act *next = curr->next;
                    if (curr->time > next->time || (curr->time == next->time && strcmp(curr->label, next->label) > 0)) {
                        curr->next = next->next;
                        next->next = curr;
                        if (prev) prev->next = next; else head = next;
                        swapped = 1;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
            for (Act *a = head; a; a = a->next) printf("%d %s %s\n", a->time, a->label, a->note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            free_list();
            head = NULL;
        }
    }
    free_list();
    return 0;
}