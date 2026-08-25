/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *title;
    char **body;
    int body_count;
    struct record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Record *new_record(const char *title) {
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = NULL;
    r->body_count = 0;
    r->next = head;
    head = r;
    return r;
}

void add_body_line(Record *r, const char *line) {
    r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
    r->body[r->body_count++] = strdup(line);
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[1024];
    Record *current = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            current = NULL;
            continue;
        }
        if (!current) {
            if (strlen(line) > 0) {
                current = new_record(line);
            }
        } else {
            add_body_line(current, line);
        }
    }
    fclose(f);
}

void add_record_from_stdin(const char *title) {
    if (find_record(title)) return; // or overwrite? spec says ADD title, likely create new, maybe ignore if exists? We'll replace.
    Record *r = new_record(title);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        add_body_line(r, line);
    }
}

void delete_record(const char *title) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->title);
            for (int i = 0; i < r->body_count; i++) free(r->body[i]);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_count; i++) {
        for (char *c = r->body[i]; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

void print_all() {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_count; i++) {
            for (char *c = r->body[i]; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
        }
        r = r->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], title[256];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_all(); continue; }
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) add_record_from_stdin(title);
        }
    }
    return 0;
}