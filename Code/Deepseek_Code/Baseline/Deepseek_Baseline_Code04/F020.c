/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body;   // concatenated body lines with newlines?
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

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = strdup(body);
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

void print_record(Record *r) {
    printf("%s\n", r->title);
    printf("%s\n", r->body);
}

void print_catalog(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n", r->title);
        r = r->next;
    }
}

void free_catalog(void) {
    while (catalog) {
        Record *tmp = catalog;
        catalog = catalog->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
}

char *read_body_until_sep(void) {
    char line[4096];
    char *body = strdup("");
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        char *newbody = malloc(strlen(body) + strlen(line) + 2);
        sprintf(newbody, "%s%s\n", body, line);
        free(body);
        body = newbody;
    }
    return body;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) == 0) continue;
            if (strcmp(line, "--") == 0) continue;
            // It's a title
            char title[4096];
            strcpy(title, line);
            // Read body until "--"
            char body[8192] = "";
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) break;
                strcat(body, line);
                strcat(body, "\n");
            }
            add_record(title, body);
        }
        fclose(f);
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char title[4096];
        int n = sscanf(line, "%s %[^\n]", cmd, title);
        if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Record *r = find_record(title);
            if (r) print_record(r);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            // read body until --
            char *body = read_body_until_sep();
            add_record(title, body);
            free(body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    free_catalog();
    return 0;
}