// F003.c
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

typedef struct Alias {
    char *name;
    char *expansion;
    struct Alias *next;
} Alias;

Alias *aliases = NULL;

Alias *find_alias(const char *name) {
    for (Alias *a = aliases; a; a = a->next) {
        if (strcmp(a->name, name) == 0) return a;
    }
    return NULL;
}

void define_alias(const char *name, const char *exp) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(exp);
    } else {
        a = malloc(sizeof(Alias));
        a->name = strdup(name);
        a->expansion = strdup(exp);
        a->next = aliases;
        aliases = a;
    }
}

void rename_alias(const char *old_name, const char *new_name) {
    Alias *a = find_alias(old_name);
    if (a) {
        free(a->name);
        a->name = strdup(new_name);
    }
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *curr = aliases;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else aliases = curr->next;
            free(curr->name);
            free(curr->expansion);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
            char *text2 = strchr(text, ' ');
            if (text2) {
                text2++;
                while (*text2 == ' ') text2++;
                text = text2;
            } else {
                text = NULL;
            }
        }
        
        if (strcmp(cmd, "DEFINE") == 0 && text) {
            define_alias(arg1, text);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            Alias *a = find_alias(arg1);
            if (a) print_underscores(a->expansion);
        } else if (strcmp(cmd, "RENAME") == 0) {
            rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Alias *a = aliases; a; a = a->next) {
                printf("%s ", a->name);
                print_underscores(a->expansion);
            }
        }
        free(line);
    }
    
    Alias *a = aliases;
    while (a) {
        Alias *next = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = next;
    }
    return 0;
}