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

void add_record(const char *name, const char *note) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    if (!r->name || !r->note) { free(r->name); free(r->note); free(r); return; }
    r->next = head;
    head = r;
}

void process_batch(int n) {
    char **names = malloc(n * sizeof(char*));
    char **notes = malloc(n * sizeof(char*));
    if (!names || !notes) { free(names); free(notes); return; }
    
    int valid = 1;
    for (int i = 0; i < n; i++) {
        names[i] = NULL;
        notes[i] = NULL;
        char line[8192];
        if (!fgets(line, sizeof(line), stdin)) {
            valid = 0;
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        char name[1024] = {0};
        char note[4096] = {0};
        if (sscanf(line, "%1023s %4095[^\n]", name, note) == 2) {
            names[i] = strdup(name);
            notes[i] = strdup(note);
            if (!names[i] || !notes[i]) {
                free(names[i]); free(notes[i]);
                valid = 0;
                break;
            }
        } else {
            valid = 0;
            break;
        }
    }
    
    if (valid) {
        for (int i = 0; i < n; i++) {
            add_record(names[i], notes[i]);
            free(names[i]);
            free(notes[i]);
        }
    } else {
        for (int i = 0; i < n; i++) {
            free(names[i]);
            free(notes[i]);
        }
    }
    free(names);
    free(notes);
}

void delete_record(const char *name) {
    Record *curr = head;
    Record *prev = NULL;
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

void find_record(const char *name) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            printf("%s %s\n", r->name, r->note);
            return;
        }
    }
}

void report() {
    for (Record *r = head; r; r = r->next) {
        printf("%s %s\n", r->name, r->note);
    }
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        int n = 0;
        
        int parsed = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (parsed < 1) continue;
        
        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1) {
                process_batch(n);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && parsed >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && parsed >= 2) {
            find_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}