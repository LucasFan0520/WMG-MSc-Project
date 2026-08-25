/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct stored {
    char *name;
    char *decoded;
    struct stored *next;
} Stored;

Stored *head = NULL;

Stored *find_stored(const char *name) {
    Stored *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

char *decode_rle(const char *encoded) {
    char *result = malloc(1);
    result[0] = '\0';
    int result_len = 0;
    const char *p = encoded;
    while (*p) {
        if (!isdigit(*p)) { free(result); return NULL; }
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            p++;
        }
        if (count > 100000) { free(result); return NULL; } // treat as invalid
        if (*p == '\0') { free(result); return NULL; }
        char ch = *p++;
        result = realloc(result, result_len + count + 1);
        for (int i = 0; i < count; i++) result[result_len + i] = ch;
        result_len += count;
        result[result_len] = '\0';
    }
    return result;
}

void store_rle(const char *name, const char *encoded) {
    char *decoded = decode_rle(encoded);
    if (!decoded) return;
    Stored *s = find_stored(name);
    if (s) {
        free(s->decoded);
        s->decoded = decoded;
        return;
    }
    Stored *new = malloc(sizeof(Stored));
    new->name = strdup(name);
    new->decoded = decoded;
    new->next = head;
    head = new;
}

void decode_print(const char *name) {
    Stored *s = find_stored(name);
    if (!s) { printf("NOT_FOUND\n"); return; }
    for (char *c = s->decoded; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void delete_stored(const char *name) {
    Stored *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->decoded);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void list_names() {
    Stored *s = head;
    while (s) {
        printf("%s\n", s->name);
        s = s->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], encoded[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_names(); continue; }
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            store_rle(name, rest);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_stored(name);
        }
    }
    return 0;
}