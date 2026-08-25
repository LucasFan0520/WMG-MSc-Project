// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *head = NULL;

void add_record(char *name, char *note) {
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (!head) {
        head = r;
    } else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void delete_record(char *name) {
    Record *prev = NULL;
    Record *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record(char *name) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            printf("%s %s\n", r->name, r->note);
            return;
        }
    }
}

void report_records() {
    for (Record *r = head; r; r = r->next) {
        printf("%s %s\n", r->name, r->note);
    }
}

int main() {
    char cmd[20], name[100], note[1000];
    int n;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BATCH") == 0) {
            scanf("%d", &n);
            char **names = malloc(sizeof(char*) * n);
            char **notes = malloc(sizeof(char*) * n);
            int valid = 1;
            for (int i = 0; i < n; i++) {
                names[i] = malloc(100);
                notes[i] = malloc(1000);
                if (scanf("%99s", names[i]) != 1) {
                    valid = 0;
                    fgets(notes[i], 1000, stdin);
                } else {
                    fgets(notes[i], 1000, stdin);
                    notes[i][strcspn(notes[i], "\n")] = 0;
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    add_record(names[i], notes[i]);
                }
            }
            for (int i = 0; i < n; i++) {
                free(names[i]);
                free(notes[i]);
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", name);
            delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", name);
            find_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}