// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;
    size_t count, cap;
    struct Record *next;
} Record;

Record *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Record *find_record(const char *title) {
    for (Record *r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void add_body(Record *r, const char *line) {
    if (r->count == r->cap) {
        size_t new_cap = r->cap ? r->cap * 2 : 4;
        char **tmp = realloc(r->body, new_cap * sizeof(char *));
        if (!tmp) return;
        r->body = tmp; r->cap = new_cap;
    }
    r->body[r->count++] = strdup(line);
}

void parse_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    Record *curr = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            curr = NULL;
        } else if (!curr) {
            curr = malloc(sizeof(Record));
            if (curr) {
                curr->title = strdup(line);
                curr->body = NULL;
                curr->count = curr->cap = 0;
                curr->next = head;
                head = curr;
            }
        } else {
            add_body(curr, line);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) parse_file(argv[1]);
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Record *r = find_record(title);
                if (r) {
                    printf("%s\n", r->title);
                    for (size_t i = 0; i < r->count; i++) printf("%s\n", r->body[i]);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Record *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        for (size_t i = 0; i < curr->count; i++) free(curr->body[i]);
                        free(curr->body); free(curr->title); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = get_token(&ptr);
            if (title && !find_record(title)) {
                Record *r = malloc(sizeof(Record));
                if (r) {
                    r->title = strdup(title);
                    r->body = NULL;
                    r->count = r->cap = 0;
                    r->next = head;
                    head = r;
                    while (fgets(line, sizeof(line), stdin)) {
                        line[strcspn(line, "\n")] = '\0';
                        if (strcmp(line, "--") == 0) break;
                        add_body(r, line);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Record *r = head; r; r = r->next) {
                printf("%s\n", r->title);
                for (size_t i = 0; i < r->count; i++) printf("%s\n", r->body[i]);
                printf("--\n");
            }
        }
    }
    while (head) {
        Record *next = head->next;
        for (size_t i = 0; i < head->count; i++) free(head->body[i]);
        free(head->body); free(head->title); free(head);
        head = next;
    }
    return 0;
}