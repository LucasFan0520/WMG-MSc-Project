// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *records = NULL;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) != 1 || n <= 0) continue;
            Record *batch = NULL;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char name[256];
                int len = 0;
                if (sscanf(line, "%255s%n", name, &len) != 1) { valid = 0; break; }
                char *note = line + len;
                while (*note == ' ') note++;
                Record *r = malloc(sizeof(Record));
                if (!r) { valid = 0; break; }
                r->name = strdup(name);
                r->note = strdup(note);
                if (!r->name || !r->note) { free(r->name); free(r->note); free(r); valid = 0; break; }
                r->next = batch;
                batch = r;
            }
            if (valid) {
                while (batch) {
                    Record *r = batch;
                    batch = batch->next;
                    r->next = records;
                    records = r;
                }
            } else {
                while (batch) {
                    Record *r = batch;
                    batch = batch->next;
                    free(r->name);
                    free(r->note);
                    free(r);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Record *prev = NULL;
            for (Record *r = records; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else records = r->next;
                    free(r->name);
                    free(r->note);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            for (Record *r = records; r; r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    printf("%s ", r->name);
                    print_text(r->note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Record *r = records; r; r = r->next) {
                printf("%s ", r->name);
                print_text(r->note);
            }
        }
    }
    while (records) {
        Record *r = records;
        records = records->next;
        free(r->name);
        free(r->note);
        free(r);
    }
    return 0;
}