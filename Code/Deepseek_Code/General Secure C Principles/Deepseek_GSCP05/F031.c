// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *name) {
    Record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    char *s = r->note;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s: ", r->name);
        char *s = r->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &n) != 1) continue;
            // read n lines, store temporarily
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            if (!names || !notes) {
                free(names); free(notes);
                // discard batch
                for (int i = 0; i < n; i++) {
                    fgets(line, sizeof(line), stdin);
                }
                continue;
            }
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = '\0';
                char *space = strchr(line, ' ');
                if (!space) {
                    valid = 0;
                    break;
                }
                *space = '\0';
                names[i] = strdup(line);
                notes[i] = strdup(space + 1);
                if (!names[i] || !notes[i]) { valid = 0; break; }
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
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}