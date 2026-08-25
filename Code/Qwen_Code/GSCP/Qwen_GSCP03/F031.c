// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *note;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void free_list() {
    while (head) {
        Rec *r = head; head = head->next;
        free(r->name); free(r->note); free(r);
    }
}

Rec *find_rec(const char *name) {
    for (Rec *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], note[4096] = "";
        int n;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %d", &n) != 1) continue;
            Rec **batch = malloc(n * sizeof(Rec*));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                if (sscanf(line, "%255s %4095[^\n]", name, note) < 1) { valid = 0; break; }
                batch[i] = malloc(sizeof(Rec));
                batch[i]->name = strdup(name);
                batch[i]->note = strdup(note);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    batch[i]->next = head; head = batch[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch[i]) { free(batch[i]->name); free(batch[i]->note); free(batch[i]); }
                }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Rec *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Rec *r = find_rec(name);
            if (r) printf("%s %s\n", r->name, r->note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->name, r->note);
            }
        }
    }
    free_list();
    return 0;
}