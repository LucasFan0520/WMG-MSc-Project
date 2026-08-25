// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Record {
    char *title;
    char *body;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *title) {
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->title, title) == 0) return r;
    }
    return NULL;
}

void add_record(const char *title, const char *body) {
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->body = strdup(body);
    r->next = records;
    records = r;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "--") == 0) {
            free(line);
            continue;
        }
        char *title = strdup(line);
        free(line);
        
        size_t body_cap = 256, body_len = 0;
        char *body = malloc(body_cap);
        body[0] = '\0';
        
        while ((line = read_line())) {
            if (strcmp(line, "--") == 0) {
                free(line);
                break;
            }
            size_t line_len = strlen(line);
            while (body_len + line_len + 2 > body_cap) {
                body_cap *= 2;
                body = realloc(body, body_cap);
            }
            if (body_len > 0) {
                body[body_len++] = '\n';
            }
            strcpy(body + body_len, line);
            body_len += line_len;
            free(line);
        }
        add_record(title, body);
        free(title);
        free(body);
    }
    fclose(f);
}

void delete_record(const char *title) {
    Record *prev = NULL, *curr = records;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free(curr->title);
            free(curr->body);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_print_record(const char *title) {
    Record *r = find_record(title);
    if (r) {
        printf("%s\n%s\n", r->title, r->body);
    }
}

void print_records() {
    for (Record *r = records; r; r = r->next) {
        printf("%s\n%s\n--\n", r->title, r->body);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, title[256] = {0};
        sscanf(line, "%15s %255s", cmd, title);
        
        if (strcmp(cmd, "FIND") == 0) {
            find_print_record(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            size_t body_cap = 256, body_len = 0;
            char *body = malloc(body_cap);
            body[0] = '\0';
            
            while ((line = read_line())) {
                if (strcmp(line, "--") == 0) {
                    free(line);
                    break;
                }
                size_t line_len = strlen(line);
                while (body_len + line_len + 2 > body_cap) {
                    body_cap *= 2;
                    body = realloc(body, body_cap);
                }
                if (body_len > 0) {
                    body[body_len++] = '\n';
                }
                strcpy(body + body_len, line);
                body_len += line_len;
                free(line);
            }
            add_record(title, body);
            free(body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_records();
        }
        free(line);
    }
    
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
    return 0;
}