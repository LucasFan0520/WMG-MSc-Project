/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body;
    struct Record *next;
} Record;

Record *head = NULL;
Record *tail = NULL;

Record *find_record(char *title) {
    for (Record *r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void add_record(char *title, char *body) {
    Record *r = find_record(title);
    if (r) {
        free(r->body);
        r->body = strdup(body);
        return;
    }
    Record *new = malloc(sizeof(Record));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = NULL;
    if (!tail) head = tail = new;
    else { tail->next = new; tail = new; }
}

void delete_record(char *title) {
    Record *prev = NULL;
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
    }
}

void print_record(char *title) {
    Record *r = find_record(title);
    if (r) printf("%s\n%s\n", r->title, r->body);
}

void print_all(void) {
    for (Record *r = head; r; r = r->next)
        printf("%s\n%s\n", r->title, r->body);
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <catalogfile>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    // read records separated by "--"
    while (1) {
        // read title
        if (getline(&line, &len, f) == -1) break;
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) continue; // skip empty?
        char *title = strdup(line);
        // read body lines until "--" or EOF
        size_t body_len = 0;
        char *body = malloc(1);
        body[0] = '\0';
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "--") == 0) break;
            body = realloc(body, body_len + strlen(line) + 2);
            if (body_len > 0) {
                strcat(body, "\n");
                body_len++;
            }
            strcat(body, line);
            body_len += strlen(line);
        }
        // add record
        Record *new = malloc(sizeof(Record));
        new->title = title;
        new->body = body;
        new->next = NULL;
        if (!tail) head = tail = new;
        else { tail->next = new; tail = new; }
    }
    free(line);
    fclose(f);

    // commands from stdin
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256];
        int n = sscanf(line, "%15s %255s", cmd, arg1);
        if (n < 1) continue;
        if (strcmp(cmd, "FIND") == 0 && n == 2) {
            print_record(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "ADD") == 0 && n == 2) {
            // read body lines until "--"
            char *title = strdup(arg1);
            size_t body_len = 0;
            char *body = malloc(1);
            body[0] = '\0';
            while (getline(&line, &len, stdin) != -1) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) break;
                body = realloc(body, body_len + strlen(line) + 2);
                if (body_len > 0) {
                    strcat(body, "\n");
                    body_len++;
                }
                strcat(body, line);
                body_len += strlen(line);
            }
            add_record(title, body);
            free(title);
            free(body);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all();
        }
    }
    free(line);
    free_all();
    return 0;
}