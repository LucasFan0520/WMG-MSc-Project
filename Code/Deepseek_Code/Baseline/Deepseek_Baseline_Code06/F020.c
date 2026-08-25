/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;
    int body_lines;
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

void add_record(const char *title, char **body, int lines) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = malloc(lines * sizeof(char*));
    for (int i = 0; i < lines; i++) r->body[i] = strdup(body[i]);
    r->body_lines = lines;
    r->next = head;
    head = r;
}

void delete_record(const char *title) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->title);
            for (int i = 0; i < r->body_lines; i++) free(r->body[i]);
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
    if (!r) return;
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_lines; i++) printf("%s\n", r->body[i]);
}

void print_all_records() {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_lines; i++) printf("%s\n", r->body[i]);
        printf("--\n");
        r = r->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    char title[256];
    char **body = NULL;
    int lines = 0;
    int reading = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (reading) {
                // finish record
                add_record(title, body, lines);
                for (int i = 0; i < lines; i++) free(body[i]);
                free(body);
                body = NULL;
                lines = 0;
                reading = 0;
            }
            continue;
        }
        if (!reading) {
            strcpy(title, line);
            reading = 1;
            body = malloc(sizeof(char*));
        } else {
            body = realloc(body, (lines + 1) * sizeof(char*));
            body[lines] = strdup(line);
            lines++;
        }
    }
    if (reading) {
        add_record(title, body, lines);
        for (int i = 0; i < lines; i++) free(body[i]);
        free(body);
    }
    fclose(f);
}

void free_all() {
    while (head) {
        Record *tmp = head;
        head = head->next;
        free(tmp->title);
        for (int i = 0; i < tmp->body_lines; i++) free(tmp->body[i]);
        free(tmp->body);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char title[256];
        int n = sscanf(line, "%31s %255[^\n]", cmd, title);
        if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0 && n == 2) {
            // read body lines until "--"
            char **body = NULL;
            int lines = 0;
            char bline[4096];
            while (fgets(bline, sizeof(bline), stdin)) {
                bline[strcspn(bline, "\n")] = '\0';
                if (strcmp(bline, "--") == 0) break;
                body = realloc(body, (lines + 1) * sizeof(char*));
                body[lines] = strdup(bline);
                lines++;
            }
            add_record(title, body, lines);
            for (int i = 0; i < lines; i++) free(body[i]);
            free(body);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all_records();
        }
    }
    free_all();
    return 0;
}