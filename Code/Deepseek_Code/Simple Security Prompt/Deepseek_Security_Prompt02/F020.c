/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct BodyLine {
    char *text;
    struct BodyLine *next;
} BodyLine;

typedef struct Record {
    char *title;
    BodyLine *body;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *title) {
    Record *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_body(BodyLine *b) {
    while (b) {
        BodyLine *tmp = b;
        b = b->next;
        free(tmp->text);
        free(tmp);
    }
}

void free_record(Record *r) {
    free(r->title);
    free_body(r->body);
    free(r);
}

void add_record(const char *title) {
    Record *r = find_record(title);
    if (r) {
        free_body(r->body);
        r->body = NULL;
        return;
    }
    r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = NULL;
    r->next = records;
    records = r;
}

void add_body_line(Record *r, const char *line) {
    BodyLine *b = malloc(sizeof(BodyLine));
    b->text = strdup(line);
    b->next = r->body;
    r->body = b;
}

void delete_record(const char *title) {
    Record *prev = NULL, *curr = records;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free_record(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->title);
    BodyLine *b = r->body;
    while (b) {
        printf("%s\n", b->text);
        b = b->next;
    }
}

void print_records(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->title);
        BodyLine *b = r->body;
        while (b) {
            printf("%s\n", b->text);
            b = b->next;
        }
        printf("--\n");
        r = r->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    Record *current = NULL;
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        if (strcmp(line, "--") == 0) {
            current = NULL;
            continue;
        }
        if (!current) {
            current = malloc(sizeof(Record));
            current->title = strdup(line);
            current->body = NULL;
            current->next = records;
            records = current;
        } else {
            add_body_line(current, line);
        }
    }
    fclose(f);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    char cmd[32];
    char title[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) != 1) continue;
            add_record(title);
            Record *r = find_record(title);
            if (r) {
                while (fgets(line, sizeof(line), stdin)) {
                    trim_newline(line);
                    if (strcmp(line, "--") == 0) break;
                    add_body_line(r, line);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_records();
        }
    }

    while (records) {
        Record *tmp = records;
        records = records->next;
        free_record(tmp);
    }
    return 0;
}