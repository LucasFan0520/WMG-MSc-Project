// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;      // array of body lines
    int body_count;
    struct Record *next;
} Record;

Record *head = NULL;
Record *tail = NULL;

Record *find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record_from_file(FILE *f) {
    char line[1024];
    if (!fgets(line, sizeof(line), f)) return;
    line[strcspn(line, "\n")] = 0;
    if (strcmp(line, "--") == 0) return;
    char *title = strdup(line);
    char **body = NULL;
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) break;
        body = realloc(body, (count+1)*sizeof(char*));
        body[count] = strdup(line);
        count++;
    }
    Record *r = malloc(sizeof(Record));
    r->title = title;
    r->body = body;
    r->body_count = count;
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    // Clear existing records
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->title);
        for (int i=0; i<r->body_count; i++) free(r->body[i]);
        free(r->body);
        free(r);
        r = next;
    }
    head = tail = NULL;
    // Read records
    while (!feof(f)) {
        add_record_from_file(f);
    }
    fclose(f);
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (int i=0; i<r->body_count; i++) {
        printf("%s\n", r->body[i]);
    }
}

void delete_record(const char *title) {
    Record *prev = NULL;
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            for (int i=0; i<r->body_count; i++) free(r->body[i]);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void add_record_interactive(const char *title) {
    if (find_record(title)) return;
    char **body = NULL;
    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) break;
        body = realloc(body, (count+1)*sizeof(char*));
        body[count] = strdup(line);
        count++;
    }
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = body;
    r->body_count = count;
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void print_all_records(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        for (int i=0; i<r->body_count; i++) {
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
        for (int i=0; i<r->body_count; i++) free(r->body[i]);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(rest, " ");
            if (title) find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(rest, " ");
            if (title) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(rest, " ");
            if (title) add_record_interactive(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }
    free_all();
    return 0;
}