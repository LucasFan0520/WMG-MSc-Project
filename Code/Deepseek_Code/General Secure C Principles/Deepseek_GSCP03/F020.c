/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;
    int body_count;
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

void add_record(const char *title) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->title = strdup(title);
    r->body = NULL;
    r->body_count = 0;
    r->next = head;
    head = r;
}

void delete_record(const char *title) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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

void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->title);
    for (int i = 0; i < r->body_count; i++) {
        printf("%s\n", r->body[i]);
    }
}

void print_all_records(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->body_count; i++) {
            printf("%s\n", r->body[i]);
        }
        r = r->next;
    }
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->title);
        for (int i = 0; i < tmp->body_count; i++) free(tmp->body[i]);
        free(tmp->body);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            Record *current = NULL;
            int in_body = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) {
                    in_body = 0;
                    current = NULL;
                    continue;
                }
                if (!current) {
                    current = malloc(sizeof(Record));
                    if (current) {
                        current->title = strdup(line);
                        current->body = NULL;
                        current->body_count = 0;
                        current->next = head;
                        head = current;
                        in_body = 1;
                    }
                } else if (in_body) {
                    current->body = realloc(current->body, (current->body_count + 1) * sizeof(char *));
                    if (current->body) {
                        current->body[current->body_count] = strdup(line);
                        current->body_count++;
                    }
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
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
            char *p = line + 4;
            while (*p == ' ') p++;
            if (*p) {
                add_record(p);
                Record *rec = find_record(p);
                if (rec) {
                    while (fgets(line, sizeof(line), stdin)) {
                        line[strcspn(line, "\n")] = '\0';
                        if (strcmp(line, "--") == 0) break;
                        if (strcmp(line, "END") == 0) {
                        }
                        // Add body line
                        rec->body = realloc(rec->body, (rec->body_count + 1) * sizeof(char *));
                        if (rec->body) {
                            rec->body[rec->body_count] = strdup(line);
                            rec->body_count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }
    free_all();
    return 0;
}