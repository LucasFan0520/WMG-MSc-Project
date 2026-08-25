// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *head = NULL;

void free_list() {
    while (head) {
        Note *n = head; head = head->next;
        free(n->name); free(n->note); free(n);
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_head = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], note[4096] = "";
        if (sscanf(line, "%255s %4095[^\n]", name, note) < 1) { valid = 0; break; }
        Note *n = malloc(sizeof(Note));
        n->name = strdup(name); n->note = strdup(note);
        n->next = new_head; new_head = n;
    }
    fclose(f);
    if (valid) {
        free_list();
        head = new_head;
    } else {
        while (new_head) {
            Note *tmp = new_head; new_head = new_head->next;
            free(tmp->name); free(tmp->note); free(tmp);
        }
    }
}

int main(int argc, char *argv[]) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) < 1) continue;
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name); n->note = strdup(note);
            n->next = head; head = n;
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Note *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->name, n->note);
            }
        }
    }
    free_list();
    return 0;
}