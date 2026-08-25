/* F031.c */
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
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *r = records, *prev = NULL;
    while (r && strcmp(r->name, name) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else records = r->next;
    free(r->name);
    free(r->note);
    free(r);
}

void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s ", r->name);
    for (char *c = r->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
    Record *r = records;
    while (r) {
        printf("%s ", r->name);
        for (char *c = r->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line + 6, "%d", &n) != 1) continue;
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (getline(&line, &len, stdin) == -1) { valid = 0; break; }
                line[strcspn(line, "\n")] = '\0';
                char name[256];
                char *p = line;
                if (sscanf(p, "%255s", name) != 1) { valid = 0; break; }
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                names[i] = strdup(name);
                notes[i] = strdup(p);
                if (find_record(name)) { valid = 0; }
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
            delete_record(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_record_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}