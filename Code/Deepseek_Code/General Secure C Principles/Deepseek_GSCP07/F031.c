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

Record* find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_record(Record *r) {
    free(r->name);
    free(r->note);
    free(r);
}

int is_valid_line(const char *line) {
    // line must have at least name and note? We'll just check if we can parse name and note.
    // We'll split at first space.
    const char *space = strchr(line, ' ');
    if (!space) return 0;
    return 1;
}

void parse_line(const char *line, char **name, char **note) {
    const char *space = strchr(line, ' ');
    if (!space) { *name = NULL; *note = NULL; return; }
    *name = strndup(line, space - line);
    *note = strdup(space + 1);
}

void handle_batch(char *cmd) {
    char *nstr = strtok(NULL, " ");
    if (!nstr) return;
    int n = atoi(nstr);
    if (n <= 0) return;
    // Read n lines and validate
    char **lines = malloc(n * sizeof(char*));
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char buf[4096];
        if (!fgets(buf, sizeof(buf), stdin)) { valid = 0; break; }
        buf[strcspn(buf, "\n")] = '\0';
        lines[i] = strdup(buf);
        if (!is_valid_line(buf)) valid = 0;
    }
    if (!valid) {
        // discard all
        for (int i = 0; i < n; i++) free(lines[i]);
        free(lines);
        return;
    }
    // All valid, append
    for (int i = 0; i < n; i++) {
        char *name, *note;
        parse_line(lines[i], &name, &note);
        if (name && note && !find_record(name)) {
            Record *new = malloc(sizeof(Record));
            new->name = name;
            new->note = note;
            new->next = NULL;
            if (!head) head = new;
            else {
                Record *last = head;
                while (last->next) last = last->next;
                last->next = new;
            }
        } else {
            free(name);
            free(note);
        }
        free(lines[i]);
    }
    free(lines);
}

void handle_delete(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_record(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_find(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Record *r = find_record(name);
    if (r) printf("%s\n", r->note);
}

void handle_report() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BATCH") == 0) handle_batch(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "REPORT") == 0) handle_report();
    }
    while (head) {
        Record *tmp = head;
        head = head->next;
        free_record(tmp);
    }
    return 0;
}