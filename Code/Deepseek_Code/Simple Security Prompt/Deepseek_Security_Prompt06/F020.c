// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // concatenated body lines with newlines
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *title) {
    Record *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *title, const char *body) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->body = malloc(strlen(body) + 1);
    strcpy(r->body, body);
    r->next = records;
    records = r;
}

void delete_record(const char *title) {
    Record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->title);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n%s\n", r->title, r->body);
}

void print_all(void) {
    Record *r = records;
    while (r) {
        printf("%s\n%s\n--\n", r->title, r->body);
        r = r->next;
    }
}

void load_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    char line[4096];
    char title[4096];
    char body[4096*10]; // large enough
    body[0] = '\0';
    int in_record = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (in_record) {
                add_record(title, body);
                body[0] = '\0';
                in_record = 0;
            }
            continue;
        }
        if (!in_record) {
            strcpy(title, line);
            in_record = 1;
        } else {
            if (strlen(body) > 0) strcat(body, "\n");
            strcat(body, line);
        }
    }
    if (in_record) add_record(title, body);
    fclose(fp);
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = tmp;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "FIND") == 0) {
            if (*rest) print_record(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*rest) delete_record(rest);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (*rest) {
                char body[4096*10] = "";
                strcpy(title, rest);
                while (fgets(line, sizeof(line), stdin)) {
                    line[strcspn(line, "\n")] = '\0';
                    if (strcmp(line, "--") == 0) break;
                    if (strlen(body) > 0) strcat(body, "\n");
                    strcat(body, line);
                }
                add_record(title, body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}