/*
 * F020.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *title;
    char *body;
    struct record *next;
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
    Record *prev = NULL, *r = records;
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

void find_record_cmd(const char *title) {
    Record *r = find_record(title);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n%s\n", r->title, r->body);
}

void print_all(void) {
    Record *r = records;
    while (r) {
        printf("%s\n%s\n--\n", r->title, r->body);
        r = r->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            char title[4096] = "";
            char body[8192] = "";
            int in_body = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    if (in_body) {
                        add_record(title, body);
                        title[0] = 0;
                        body[0] = 0;
                        in_body = 0;
                    }
                    continue;
                }
                if (!in_body) {
                    strcpy(title, line);
                    in_body = 1;
                } else {
                    if (strlen(body) > 0) strcat(body, "\n");
                    strcat(body, line);
                }
            }
            if (in_body) add_record(title, body);
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256];
        if (sscanf(line, "%31s %255s", cmd, title) < 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            find_record_cmd(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char body[8192] = "";
            char line2[4096];
            while (fgets(line2, sizeof(line2), stdin)) {
                line2[strcspn(line2, "\n")] = 0;
                if (strcmp(line2, "--") == 0) break;
                if (strlen(body) > 0) strcat(body, "\n");
                strcat(body, line2);
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    return 0;
}