// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Record {
    char *title;
    char *body;  // may contain newlines
    struct Record *next;
} Record;

Record *catalog = NULL;

Record *find_record(const char *title) {
    Record *r = catalog;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->title = safe_strdup(title);
    r->body = safe_strdup(body);
    r->next = catalog;
    catalog = r;
}

void delete_record(const char *title) {
    Record *prev = NULL, *cur = catalog;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else catalog = cur->next;
            free(cur->title);
            free(cur->body);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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

void print_catalog(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n%s\n--\n", r->title, r->body);
        r = r->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    char title[MAX_LINE];
    char body[MAX_LINE * 10]; // dynamic would be better, but using large fixed for simplicity
    body[0] = '\0';
    int have_title = 0;
    int in_body = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (have_title) {
                add_record(title, body);
                body[0] = '\0';
                have_title = 0;
                in_body = 0;
            }
            continue;
        }
        if (!have_title) {
            strncpy(title, line, sizeof(title) - 1);
            title[sizeof(title)-1] = '\0';
            have_title = 1;
            in_body = 0;
            body[0] = '\0';
        } else {
            if (in_body) {
                strncat(body, "\n", sizeof(body) - strlen(body) - 1);
            }
            strncat(body, line, sizeof(body) - strlen(body) - 1);
            in_body = 1;
        }
    }
    if (have_title) {
        add_record(title, body);
    }
    fclose(f);
}

void add_interactive(void) {
    char line[MAX_LINE];
    char title[MAX_LINE];
    char body[MAX_LINE * 10];
    if (!fgets(line, sizeof(line), stdin)) return;
    line[strcspn(line, "\n")] = '\0';
    strncpy(title, line, sizeof(title) - 1);
    title[sizeof(title)-1] = '\0';
    body[0] = '\0';
    int in_body = 0;
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        if (in_body) strncat(body, "\n", sizeof(body) - strlen(body) - 1);
        strncat(body, line, sizeof(body) - strlen(body) - 1);
        in_body = 1;
    }
    add_record(title, body);
}

void free_all(void) {
    Record *r = catalog;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) load_file(argv[1]);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_record_cmd(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_interactive();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    free_all();
    return 0;
}