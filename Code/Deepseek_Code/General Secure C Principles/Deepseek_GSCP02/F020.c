// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;
    int body_count;
    struct Record *next;
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

void add_record(Record *r) {
    r->next = head;
    head = r;
}

void delete_record(const char *title) {
    Record *prev = NULL;
    Record *r = head;
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

void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_count; i++) {
        printf("%s\n", r->body[i]);
    }
}

void print_all(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_count; i++) {
            printf("%s\n", r->body[i]);
        }
        r = r->next;
    }
}

Record *read_record_from_file(FILE *f) {
    char line[1024];
    if (!fgets(line, sizeof(line), f)) return NULL;
    line[strcspn(line, "\n")] = '\0';
    if (strcmp(line, "--") == 0) return NULL;
    Record *r = malloc(sizeof(Record));
    if (!r) return NULL;
    r->title = strdup(line);
    r->body = NULL;
    r->body_count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
        r->body[r->body_count] = strdup(line);
        r->body_count++;
    }
    return r;
}

Record *read_record_from_stdin(const char *title) {
    Record *r = malloc(sizeof(Record));
    if (!r) return NULL;
    r->title = strdup(title);
    r->body = NULL;
    r->body_count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
        r->body[r->body_count] = strdup(line);
        r->body_count++;
    }
    free(line);
    return r;
}

void free_all(void) {
    while (head) {
        Record *next = head->next;
        free(head->title);
        for (int i = 0; i < head->body_count; i++) free(head->body[i]);
        free(head->body);
        free(head);
        head = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                Record *r = read_record_from_file(f);
                if (!r) break;
                if (!find_record(r->title)) {
                    add_record(r);
                } else {
                    free(r->title);
                    for (int i = 0; i < r->body_count; i++) free(r->body[i]);
                    free(r->body);
                    free(r);
                }
            }
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                print_record(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                delete_record(title);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                if (find_record(title)) {
                    // discard incoming body
                    char *tmp = NULL;
                    size_t tlen = 0;
                    while (getline(&tmp, &tlen, stdin) != -1) {
                        tmp[strcspn(tmp, "\n")] = '\0';
                        if (strcmp(tmp, "--") == 0) break;
                    }
                    free(tmp);
                } else {
                    Record *r = read_record_from_stdin(title);
                    if (r) add_record(r);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }

    free(line);
    free_all();
    return 0;
}