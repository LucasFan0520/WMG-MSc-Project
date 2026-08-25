// F005.c
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

void print_underscores(const char *s) {
    while (*s) {
        putchar(*s == ' ' ? '_' : *s);
        s++;
    }
    putchar('\n');
}

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int line_count = 0;

Line *get_line_at(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *n = malloc(sizeof(Line));
    n->text = strdup(text);
    n->next = NULL;
    if (index == 0) {
        n->next = head;
        head = n;
    } else {
        Line *prev = get_line_at(index - 1);
        n->next = prev->next;
        prev->next = n;
    }
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    Line *del;
    if (index == 0) {
        del = head;
        head = head->next;
    } else {
        Line *prev = get_line_at(index - 1);
        del = prev->next;
        prev->next = del->next;
    }
    free(del->text);
    free(del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line_at(index);
    if (l) {
        free(l->text);
        l->text = strdup(text);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0};
        int index = 0;
        sscanf(line, "%15s %d", cmd, &index);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
            if (strcmp(cmd, "INSERT") == 0 || strcmp(cmd, "PATCH") == 0) {
                char *t2 = strchr(text, ' ');
                if (t2) {
                    t2++;
                    while (*t2 == ' ') t2++;
                    text = t2;
                } else {
                    text = NULL;
                }
            }
        }
        
        if (strcmp(cmd, "INSERT") == 0 && text) {
            insert_line(index, text);
        } else if (strcmp(cmd, "APPEND") == 0 && text) {
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0 && text) {
            patch_line(index, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Line *l = head; l; l = l->next) {
                print_underscores(l->text);
            }
        }
        free(line);
    }
    
    Line *l = head;
    while (l) {
        Line *next = l->next;
        free(l->text);
        free(l);
        l = next;
    }
    return 0;
}