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

Record *head = NULL, *tail = NULL;

Record *find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(Record *r) {
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Record *current = NULL;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "--") == 0) {
            if (current) {
                add_record(current);
                current = NULL;
            }
            continue;
        }
        if (!current) {
            current = malloc(sizeof(Record));
            current->title = malloc(strlen(line) + 1);
            strcpy(current->title, line);
            current->body = NULL;
            current->body_count = 0;
            current->next = NULL;
        } else {
            current->body = realloc(current->body, (current->body_count + 1) * sizeof(char *));
            current->body[current->body_count] = malloc(strlen(line) + 1);
            strcpy(current->body[current->body_count], line);
            current->body_count++;
        }
    }
    if (current) add_record(current);
    free(line);
    fclose(f);
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_count; i++) {
        printf("%s\n", r->body[i]);
    }
}

void delete_record(const char *title) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->title);
            for (int i = 0; i < curr->body_count; i++) free(curr->body[i]);
            free(curr->body);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void add_record_interactive(const char *title) {
    Record *r = malloc(sizeof(Record));
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->body = NULL;
    r->body_count = 0;
    r->next = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
        r->body[r->body_count] = malloc(strlen(line) + 1);
        strcpy(r->body[r->body_count], line);
        r->body_count++;
    }
    free(line);
    add_record(r);
}

void print_all_records(void) {
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

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->title);
        for (int i = 0; i < r->body_count; i++) free(r->body[i]);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) add_record_interactive(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }
    free(line);
    free_all();
    return 0;
}