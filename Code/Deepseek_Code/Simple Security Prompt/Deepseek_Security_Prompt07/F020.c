// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // all body lines concatenated with newline? Or separate?
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

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = strdup(body);
    r->next = records;
    records = r;
}

void delete_record(const char *title) {
    Record *prev = NULL;
    Record *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->title);
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
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->title);
    printf("%s\n", r->body);
}

void print_all_records() {
    Record *r = records;
    while (r) {
        printf("%s\n", r->title);
        printf("%s\n", r->body);
        printf("--\n");
        r = r->next;
    }
}

void free_all() {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            char title[4096] = "";
            char body[4096] = "";
            int in_body = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) {
                    if (in_body) {
                        add_record(title, body);
                        body[0] = '\0';
                        in_body = 0;
                    }
                    continue;
                }
                if (!in_body) {
                    strcpy(title, line);
                    in_body = 1;
                } else {
                    if (body[0] != '\0') strcat(body, "\n");
                    strcat(body, line);
                }
            }
            if (in_body) {
                add_record(title, body);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "FIND") == 0) {
            find_record_print(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(rest);
        } else if (strcmp(cmd, "ADD") == 0) {
            // read body lines until --
            char title[4096];
            strcpy(title, rest);
            char body[4096] = "";
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) break;
                if (body[0] != '\0') strcat(body, "\n");
                strcat(body, line);
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }
    free_all();
    return 0;
}