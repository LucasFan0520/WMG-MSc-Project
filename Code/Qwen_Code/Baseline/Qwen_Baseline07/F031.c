// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name, *note;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

int main() {
    char cmd[20], name[256], note[1024];
    int n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BATCH") == 0) {
            scanf("%d", &n);
            char names[100][256], notes[100][1024];
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(names[i], sizeof(names[i]), stdin)) { valid = 0; break; }
                size_t l = strlen(names[i]);
                if (l > 0 && names[i][l-1] == '\n') names[i][l-1] = '\0';
                if (!fgets(notes[i], sizeof(notes[i]), stdin)) { valid = 0; break; }
                l = strlen(notes[i]);
                if (l > 0 && notes[i][l-1] == '\n') notes[i][l-1] = '\0';
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    Rec *r = malloc(sizeof(Rec));
                    r->name = strdup(names[i]); r->note = strdup(notes[i]); r->next = head; head = r;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            for (Rec *r = head; r; r = r->next) if (strcmp(r->name, name) == 0) { printf("%s %s\n", r->name, r->note); break; }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) printf("%s %s\n", r->name, r->note);
        }
    }
    return 0;
}