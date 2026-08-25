/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = head;
    head = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = r->data; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = head;
    int first = 1;
    while (r) {
        if (!first) printf(" ");
        printf("%s", r->name);
        first = 0;
        r = r->next;
    }
    printf("\n");
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *colon = strchr(p, ':');
                if (colon) {
                    *colon = '\0';
                    char *len_str = colon + 1;
                    char *data = len_str;
                }
                char *colon_pos = strchr(p, ':');
                if (colon_pos) {
                    char *length_start = p + strlen(name);
                    while (*length_start == ' ') length_start++;
                    char *colon_in = strchr(length_start, ':');
                    if (colon_in) {
                        *colon_in = '\0';
                        int len = atoi(length_start);
                        char *data = colon_in + 1;
                        if (len >= 0 && (int)strlen(data) == len) {
                            add_record(name, data);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            show_record(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    return 0;
}