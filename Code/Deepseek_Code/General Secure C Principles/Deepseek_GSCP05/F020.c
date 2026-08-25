// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body;   // may contain multiple lines, but we store as single string with newlines
    struct Record *next;
} Record;

Record *catalog = NULL;

Record *find_record(const char *title) {
    Record *r = catalog;
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
    r->next = catalog;
    catalog = r;
}

void delete_record(const char *title) {
    Record *prev = NULL, *cur = catalog;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else catalog = cur->next;
            free(cur->title);
            free(cur->body);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    if (r->body && strlen(r->body) > 0) printf("%s\n", r->body);
}

void print_catalog(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n", r->title);
        if (r->body && strlen(r->body) > 0) printf("%s\n", r->body);
        printf("--\n");
        r = r->next;
    }
}

void free_all(void) {
    Record *r = catalog;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char line[4096];
    char current_title[4096] = {0};
    char current_body[8192] = {0};
    int in_record = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (in_record && strlen(current_title) > 0) {
                add_record(current_title, current_body);
            }
            in_record = 0;
            current_title[0] = '\0';
            current_body[0] = '\0';
            continue;
        }
        if (!in_record) {
            strcpy(current_title, line);
            in_record = 1;
        } else {
            if (strlen(current_body) > 0) strcat(current_body, "\n");
            strcat(current_body, line);
        }
    }
    if (in_record && strlen(current_title) > 0) {
        add_record(current_title, current_body);
    }
    fclose(f);

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            // read body lines until "--"
            char body[8192] = {0};
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "--") == 0) break;
                if (strlen(body) > 0) strcat(body, "\n");
                strcat(body, line);
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    free_all();
    return 0;
}