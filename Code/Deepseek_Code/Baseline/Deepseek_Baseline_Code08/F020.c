// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // all body lines concatenated with newlines? According to spec: body lines are zero or more lines, we store as a single string with newlines.
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_record(const char *title) {
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *new = malloc(sizeof(Record));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_record(const char *title) {
    Record *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->title);
            free(cur->body);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n%s\n", r->title, r->body);
}

void print_all() {
    Record *cur = head;
    while (cur) {
        printf("%s\n%s\n--\n", cur->title, cur->body);
        cur = cur->next;
    }
}

void free_all() {
    Record *cur = head;
    while (cur) {
        Record *tmp = cur;
        cur = cur->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            char title[4096] = "";
            char body[4096] = "";
            int reading_body = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    if (reading_body) {
                        // end of record
                        if (strlen(title) > 0) {
                            add_record(title, body);
                        }
                        title[0] = 0;
                        body[0] = 0;
                        reading_body = 0;
                    } else {
                        // start of record? Actually spec: records separated by line containing only "--".
                        // So if we see "--" and not reading body, it might be empty record? We'll treat as separator.
                        reading_body = 0;
                    }
                } else {
                    if (!reading_body) {
                        strcpy(title, line);
                        reading_body = 1;
                        body[0] = 0;
                    } else {
                        if (strlen(body) > 0) strcat(body, "\n");
                        strcat(body, line);
                    }
                }
            }
            // if file ends without trailing --, still add last record
            if (reading_body && strlen(title) > 0) {
                add_record(title, body);
            }
            fclose(f);
        }
    }
    char cmdline[4096];
    while (fgets(cmdline, sizeof(cmdline), stdin)) {
        cmdline[strcspn(cmdline, "\n")] = 0;
        if (strcmp(cmdline, "END") == 0) break;
        char *cmd = strtok(cmdline, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) print_record(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char body[4096] = "";
            char line[4096];
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (strlen(body) > 0) strcat(body, "\n");
                strcat(body, line);
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}