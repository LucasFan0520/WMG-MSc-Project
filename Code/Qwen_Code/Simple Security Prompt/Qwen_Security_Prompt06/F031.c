// F031.c
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

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct BRec {
    char *name;
    char *note;
    struct BRec *next;
} BRec;

BRec *brecs = NULL;

void add_rec(const char *name, const char *note) {
    BRec *r = malloc(sizeof(BRec));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = brecs;
    brecs = r;
}

void delete_rec(const char *name) {
    BRec *prev = NULL, *curr = brecs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else brecs = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_rec(const char *name) {
    for (BRec *r = brecs; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            printf("%s %s\n", r->name, r->note);
            return;
        }
    }
}

void report_recs() {
    for (BRec *r = brecs; r; r = r->next) {
        printf("%s %s\n", r->name, r->note);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        int n = 0;
        sscanf(line, "%15s %255s %d", cmd, arg1, &n);
        
        if (strcmp(cmd, "BATCH") == 0) {
            BRec *temp = NULL;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *bline = read_line();
                if (!bline) { valid = 0; break; }
                char name[256] = {0};
                sscanf(bline, "%255s", name);
                char *note = get_rest(bline, 1);
                if (!note) { valid = 0; free(bline); break; }
                
                BRec *r = malloc(sizeof(BRec));
                r->name = strdup(name);
                r->note = strdup(note);
                r->next = temp;
                temp = r;
                free(bline);
            }
            if (valid) {
                while (temp) {
                    BRec *next = temp->next;
                    temp->next = brecs;
                    brecs = temp;
                    temp = next;
                }
            } else {
                while (temp) {
                    BRec *next = temp->next;
                    free(temp->name);
                    free(temp->note);
                    free(temp);
                    temp = next;
                }
            }
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_rec(arg1);
        else if (strcmp(cmd, "FIND") == 0) find_rec(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_recs();
        
        free(line);
    }
    return 0;
}