// F036.c
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

typedef struct LRec {
    char *name;
    char *data;
    struct LRec *next;
} LRec;

LRec *lrecs = NULL;

void add_rec(const char *name, const char *raw_data) {
    char *colon = strchr(raw_data, ':');
    if (!colon) return;
    int length = atoi(raw_data);
    if (length < 0) return;
    char *data_start = colon + 1;
    if ((int)strlen(data_start) < length) return;
    
    LRec *r = malloc(sizeof(LRec));
    r->name = strdup(name);
    r->data = malloc(length + 1);
    strncpy(r->data, data_start, length);
    r->data[length] = '\0';
    r->next = lrecs;
    lrecs = r;
}

void delete_rec(const char *name) {
    LRec *prev = NULL, *curr = lrecs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else lrecs = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_rec(const char *name) {
    LRec *r = NULL;
    for (LRec *curr = lrecs; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) { r = curr; break; }
    }
    if (!r) return;
    for (char *p = r->data; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void report_recs() {
    for (LRec *r = lrecs; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        sscanf(line, "%15s %255s", cmd, arg1);
        
        if (strcmp(cmd, "ADD") == 0) {
            char *raw = strchr(line, ' ');
            if (raw) {
                raw++;
                while (*raw == ' ') raw++;
                raw = strchr(raw, ' ');
                if (raw) {
                    raw++;
                    while (*raw == ' ') raw++;
                    add_rec(arg1, raw);
                }
            }
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_rec(arg1);
        else if (strcmp(cmd, "SHOW") == 0) show_rec(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_recs();
        
        free(line);
    }
    return 0;
}