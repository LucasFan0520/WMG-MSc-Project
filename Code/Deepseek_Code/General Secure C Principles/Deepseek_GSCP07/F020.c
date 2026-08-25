// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body; // array of lines
    int body_count;
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_record(Record *r) {
    free(r->title);
    for (int i = 0; i < r->body_count; i++) free(r->body[i]);
    free(r->body);
    free(r);
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    Record *current = NULL;
    int reading_body = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (current) {
                // finish current record
                // link to list
                if (!head) head = current;
                else {
                    Record *last = head;
                    while (last->next) last = last->next;
                    last->next = current;
                }
                current = NULL;
            }
            reading_body = 0;
            continue;
        }
        if (!current) {
            // title line
            current = malloc(sizeof(Record));
            current->title = strdup(line);
            current->body = NULL;
            current->body_count = 0;
            current->next = NULL;
            reading_body = 1;
        } else if (reading_body) {
            // body line
            current->body = realloc(current->body, (current->body_count + 1) * sizeof(char*));
            current->body[current->body_count] = strdup(line);
            current->body_count++;
        }
    }
    if (current) {
        // incomplete record - discard?
        free_record(current);
    }
    fclose(f);
}

void find_cmd(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_count; i++) {
        printf("%s\n", r->body[i]);
    }
}

void delete_cmd(const char *title) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_record(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void add_cmd(const char *title) {
    if (find_record(title)) return;
    Record *new = malloc(sizeof(Record));
    new->title = strdup(title);
    new->body = NULL;
    new->body_count = 0;
    new->next = NULL;
    // Read body lines until --
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        new->body = realloc(new->body, (new->body_count + 1) * sizeof(char*));
        new->body[new->body_count] = strdup(line);
        new->body_count++;
    }
    // Append to list
    if (!head) head = new;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void print_all() {
    Record *r = head;
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
    if (argc == 2) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_cmd(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_cmd(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) add_cmd(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    while (head) {
        Record *tmp = head;
        head = head->next;
        free_record(tmp);
    }
    return 0;
}