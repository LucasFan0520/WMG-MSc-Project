// F026.c
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

typedef struct Note {
    char *cat;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void add_note(const char *cat, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->cat = strdup(cat);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_cat(const char *cat) {
    Note *prev = NULL, *curr = notes;
    while (curr) {
        if (strcmp(curr->cat, cat) == 0) {
            Note *next = curr->next;
            if (prev) prev->next = next;
            else notes = next;
            free(curr->cat);
            free(curr->text);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void keep_cat(const char *cat) {
    Note *prev = NULL, *curr = notes;
    while (curr) {
        if (strcmp(curr->cat, cat) != 0) {
            Note *next = curr->next;
            if (prev) prev->next = next;
            else notes = next;
            free(curr->cat);
            free(curr->text);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(const char *word) {
    for (Note *n = notes; n; n = n->next) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
    }
}

void print_notes() {
    for (Note *n = notes; n; n = n->next) {
        printf("%s: %s\n", n->cat, n->text);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        sscanf(line, "%15s %255s", cmd, arg1);
        
        if (strcmp(cmd, "ADD") == 0) {
            char *text = get_rest(line, 2);
            if (text) add_note(arg1, text);
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_cat(arg1);
        else if (strcmp(cmd, "KEEP") == 0) keep_cat(arg1);
        else if (strcmp(cmd, "FIND") == 0) find_word(arg1);
        else if (strcmp(cmd, "PRINT") == 0) print_notes();
        
        free(line);
    }
    return 0;
}