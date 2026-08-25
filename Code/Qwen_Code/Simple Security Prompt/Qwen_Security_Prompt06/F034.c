// F034.c
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

typedef struct FNote {
    char *name;
    char *note;
    struct FNote *next;
} FNote;

FNote *fnotes = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    
    FNote *temp = NULL;
    char *line;
    int valid = 1;
    while ((line = read_line())) {
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        char name[256] = {0};
        sscanf(line, "%255s", name);
        char *note = get_rest(line, 1);
        if (!note) {
            valid = 0;
            free(line);
            break;
        }
        
        FNote *n = malloc(sizeof(FNote));
        n->name = strdup(name);
        n->note = strdup(note);
        n->next = temp;
        temp = n;
        free(line);
    }
    fclose(f);
    
    if (valid) {
        FNote *curr = fnotes;
        while (curr) {
            FNote *next = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            curr = next;
        }
        fnotes = temp;
    } else {
        FNote *curr = temp;
        while (curr) {
            FNote *next = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            curr = next;
        }
    }
}

void add_note(const char *name, const char *note) {
    FNote *n = malloc(sizeof(FNote));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = fnotes;
    fnotes = n;
}

void delete_note(const char *name) {
    FNote *prev = NULL, *curr = fnotes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else fnotes = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes() {
    for (FNote *n = fnotes; n; n = n->next) {
        printf("%s %s\n", n->name, n->note);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    char *path = argv[1];
    
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        sscanf(line, "%15s %255s", cmd, arg1);
        
        if (strcmp(cmd, "LOAD") == 0) load_file(path);
        else if (strcmp(cmd, "ADD") == 0) {
            char *note = get_rest(line, 2);
            if (note) add_note(arg1, note);
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_note(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_notes();
        
        free(line);
    }
    return 0;
}