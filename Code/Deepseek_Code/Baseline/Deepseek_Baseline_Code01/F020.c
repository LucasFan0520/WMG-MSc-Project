// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *title;
    char **body;
    int body_lines;
    struct record *next;
} Record;

Record *head = NULL;
Record *tail = NULL;

Record *find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(Record *r) {
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *title) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            for (int i = 0; i < r->body_lines; i++) free(r->body[i]);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void print_record(Record *r) {
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_lines; i++)
        printf("%s\n", r->body[i]);
    printf("--\n");
}

void print_all(void) {
    Record *r = head;
    while (r) {
        print_record(r);
        r = r->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    Record *r = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (r) {
                add_record(r);
                r = NULL;
            }
            continue;
        }
        if (!r) {
            r = malloc(sizeof(Record));
            r->title = strdup(line);
            r->body = NULL;
            r->body_lines = 0;
        } else {
            r->body = realloc(r->body, (r->body_lines + 1) * sizeof(char*));
            r->body[r->body_lines++] = strdup(line);
        }
    }
    if (r) add_record(r);
    fclose(f);
}

void add_command(void) {
    char line[4096];
    if (!fgets(line, sizeof(line), stdin)) return;
    line[strcspn(line, "\n")] = '\0';
    if (find_record(line)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(line);
    r->body = NULL;
    r->body_lines = 0;
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->body = realloc(r->body, (r->body_lines + 1) * sizeof(char*));
        r->body[r->body_lines++] = strdup(line);
    }
    add_record(r);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                Record *r = find_record(title);
                if (r) print_record(r);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_command();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    return 0;
}