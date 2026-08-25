// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct record {
    char *title;
    char **body;
    int body_count;
    struct record *next;
};

struct record *records = NULL;

struct record *find_record(char *title) {
    struct record *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(char *title, char **body, int body_count) {
    struct record *r = find_record(title);
    if (r) {
        // replace body
        for (int i = 0; i < r->body_count; i++) free(r->body[i]);
        free(r->body);
        r->body_count = body_count;
        r->body = malloc(body_count * sizeof(char *));
        for (int i = 0; i < body_count; i++) {
            r->body[i] = malloc(strlen(body[i]) + 1);
            strcpy(r->body[i], body[i]);
        }
    } else {
        r = malloc(sizeof(struct record));
        r->title = malloc(strlen(title) + 1);
        strcpy(r->title, title);
        r->body_count = body_count;
        r->body = malloc(body_count * sizeof(char *));
        for (int i = 0; i < body_count; i++) {
            r->body[i] = malloc(strlen(body[i]) + 1);
            strcpy(r->body[i], body[i]);
        }
        r->next = records;
        records = r;
    }
}

void delete_record(char *title) {
    struct record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
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

void find_record_print(char *title) {
    struct record *r = find_record(title);
    if (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_count; i++) {
            printf("%s\n", r->body[i]);
        }
    }
}

void print_records() {
    struct record *r = records;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_count; i++) {
            printf("%s\n", r->body[i]);
        }
        printf("--\n");
        r = r->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        char *title = NULL;
        char **body = NULL;
        int body_count = 0;
        int in_record = 0;
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "--") == 0) {
                if (in_record && title) {
                    add_record(title, body, body_count);
                    free(title);
                    for (int i = 0; i < body_count; i++) free(body[i]);
                    free(body);
                    title = NULL;
                    body = NULL;
                    body_count = 0;
                    in_record = 0;
                }
                continue;
            }
            if (!in_record) {
                title = malloc(strlen(line) + 1);
                strcpy(title, line);
                in_record = 1;
                body = NULL;
                body_count = 0;
            } else {
                body = realloc(body, (body_count + 1) * sizeof(char *));
                body[body_count] = malloc(strlen(line) + 1);
                strcpy(body[body_count], line);
                body_count++;
            }
        }
        if (in_record && title) {
            add_record(title, body, body_count);
            free(title);
            for (int i = 0; i < body_count; i++) free(body[i]);
            free(body);
        }
        fclose(f);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256];
        if (sscanf(line, "%31s %255s", cmd, title) >= 2) {
            if (strcmp(cmd, "FIND") == 0) {
                find_record_print(title);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_record(title);
            } else if (strcmp(cmd, "ADD") == 0) {
                // read body lines until --
                char **body = NULL;
                int body_count = 0;
                char buf[4096];
                while (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    if (strcmp(buf, "--") == 0) break;
                    body = realloc(body, (body_count + 1) * sizeof(char *));
                    body[body_count] = malloc(strlen(buf) + 1);
                    strcpy(body[body_count], buf);
                    body_count++;
                }
                add_record(title, body, body_count);
                for (int i = 0; i < body_count; i++) free(body[i]);
                free(body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_records();
        }
    }
    return 0;
}