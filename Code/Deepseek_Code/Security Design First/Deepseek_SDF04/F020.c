// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // concatenated body lines with newline separators
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
    if (!r) return;
    r->title = strdup(title);
    r->body = strdup(body);
    r->next = NULL;
    if (!r->title || !r->body) {
        free(r->title); free(r->body); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void delete_record(const char *title) {
    Record *r = head, *prev = NULL;
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

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (r) printf("%s\n%s\n", r->title, r->body);
}

void print_all_records(void) {
    Record *r = head;
    while (r) {
        printf("%s\n%s\n--\n", r->title, r->body);
        r = r->next;
    }
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
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            char title[256] = "";
            char body[4096] = "";
            int in_record = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) {
                    if (in_record && strlen(title) > 0) {
                        add_record(title, body);
                        title[0] = '\0';
                        body[0] = '\0';
                        in_record = 0;
                    }
                    continue;
                }
                if (!in_record) {
                    strncpy(title, line, sizeof(title)-1);
                    title[sizeof(title)-1] = '\0';
                    in_record = 1;
                    body[0] = '\0';
                } else {
                    if (strlen(body) > 0) strcat(body, "\n");
                    strncat(body, line, sizeof(body)-strlen(body)-1);
                }
            }
            if (in_record && strlen(title) > 0) {
                add_record(title, body);
            }
            free(line);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], title[256];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                // read body lines until --
                char body[4096] = "";
                char *bline = NULL;
                size_t blen = 0;
                while (getline(&bline, &blen, stdin) != -1) {
                    bline[strcspn(bline, "\n")] = '\0';
                    if (strcmp(bline, "--") == 0) break;
                    if (strlen(body) > 0) strcat(body, "\n");
                    strncat(body, bline, sizeof(body)-strlen(body)-1);
                }
                free(bline);
                add_record(title, body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }
    free(line);
    free_all();
    return 0;
}