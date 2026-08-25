// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // concatenated body lines with newlines?
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

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->body = malloc(strlen(body) + 1);
    strcpy(r->body, body);
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
    // body lines are separated by newline? We stored body with newlines.
    // Print as stored.
    printf("%s\n", r->body);
}

void print_all(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        printf("%s\n", r->body);
        r = r->next;
    }
}

void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    char title[4096];
    char body[4096*10] = "";
    int have_title = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            // end of record
            if (have_title) {
                add_record(title, body);
                have_title = 0;
                body[0] = 0;
            }
            continue;
        }
        if (!have_title) {
            strcpy(title, line);
            have_title = 1;
        } else {
            if (strlen(body) > 0) strcat(body, "\n");
            strcat(body, line);
        }
    }
    // if trailing record without --? ignore? The spec says records separated by --, so last might not have --? We'll treat as record if have_title.
    if (have_title) {
        add_record(title, body);
    }
    fclose(f);
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            print_record(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "ADD") == 0) {
            // reads following body lines until -- from stdin
            char title[4096];
            char *p = line + 4;
            while (*p == ' ') p++;
            strcpy(title, p);
            char body[4096*10] = "";
            int first = 1;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (!first) strcat(body, "\n");
                strcat(body, line);
                first = 0;
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}