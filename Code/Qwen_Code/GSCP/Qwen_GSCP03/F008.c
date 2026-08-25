// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stud {
    char *name, *note;
    int score;
    struct Stud *next;
} Stud;

Stud *head = NULL;

void free_list() {
    while (head) {
        Stud *t = head; head = head->next;
        free(t->name); free(t->note); free(t);
    }
}

Stud *find_stud(const char *name) {
    for (Stud *s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], note[4096] = "";
        int score;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", name, &score, note) < 2) continue;
            if (find_stud(name)) continue;
            Stud *s = malloc(sizeof(Stud));
            s->name = strdup(name); s->score = score; s->note = strdup(note);
            s->next = head; head = s;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", name, &score, note) < 2) continue;
            Stud *s = find_stud(name);
            if (s) {
                s->score = score;
                free(s->note);
                s->note = strdup(note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Stud *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Stud *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0, c = 0;
            for (Stud *s = head; s; s = s->next) { sum += s->score; c++; }
            printf("%d\n", c ? sum / c : 0);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Stud *s = head; s; s = s->next) printf("%s %d %s\n", s->name, s->score, s->note);
        }
    }
    free_list();
    return 0;
}