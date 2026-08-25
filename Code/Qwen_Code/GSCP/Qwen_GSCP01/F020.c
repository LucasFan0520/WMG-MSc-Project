// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BodyLine {
    char *line;
    struct BodyLine *next;
} BodyLine;

typedef struct Record {
    char *title;
    BodyLine *body;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *title) {
    Record *curr = records;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_record(const char *title) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) exit(EXIT_FAILURE);
    r->title = strdup(title);
    r->body = NULL;
    r->next = records;
    records = r;
}

void append_body(const char *title, const char *line) {
    Record *r = find_record(title);
    if (!r) return;
    BodyLine *b = malloc(sizeof(BodyLine));
    if (!b) exit(EXIT_FAILURE);
    b->line = strdup(line);
    b->next = NULL;
    if (!r->body) {
        r->body = b;
    } else {
        BodyLine *curr = r->body;
        while (curr->next) curr = curr->next;
        curr->next = b;
    }
}

void delete_record(const char *title) {
    Record *curr = records;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            BodyLine *b = curr->body;
            while (b) {
                BodyLine *next_b = b->next;
                free(b->line);
                free(b);
                b = next_b;
            }
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    BodyLine *b = r->body;
    while (b) {
        printf("%s\n", b->line);
        b = b->next;
    }
}

void print_records() {
    Record *curr = records;
    while (curr) {
        find_record_print(curr->title);
        curr = curr->next;
    }
}

void free_all() {
    Record *curr = records;
    while (curr) {
        Record *next_r = curr->next;
        BodyLine *b = curr->body;
        while (b) {
            BodyLine *next_b = b->next;
            free(b->line);
            free(b);
            b = next_b;
        }
        free(curr->title);
        free(curr);
        curr = next_r;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    char current_title[256] = {0};
    int in_record = 0;
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            in_record = 0;
            current_title[0] = '\0';
        } else if (!in_record && strlen(line) > 0) {
            strncpy(current_title, line, sizeof(current_title) - 1);
            add_record(current_title);
            in_record = 1;
        } else if (in_record && current_title[0] != '\0') {
            append_body(current_title, line);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[4096];
    char current_add_title[256] = {0};
    int is_adding_body = 0;
    
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        
        if (is_adding_body) {
            if (strcmp(line, "--") == 0) {
                is_adding_body = 0;
                current_add_title[0] = '\0';
            } else {
                append_body(current_add_title, line);
            }
            continue;
        }
        
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char title[256] = {0};
        
        sscanf(line, "%31s %255s", cmd, title);
        
        if (strcmp(cmd, "FIND") == 0) {
            find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record(title);
            strncpy(current_add_title, title, sizeof(current_add_title) - 1);
            is_adding_body = 1;
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_records();
        }
    }
    free_all();
    return 0;
}