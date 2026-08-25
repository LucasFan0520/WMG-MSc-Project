// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stored {
    char *name;
    char *encoded;
    struct Stored *next;
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

void store_encoded(const char *name, const char *enc) {
    if (find_stored(name)) return;
    Stored *s = malloc(sizeof(Stored));
    s->name = malloc(strlen(name) + 1);
    strcpy(s->name, name);
    s->encoded = malloc(strlen(enc) + 1);
    strcpy(s->encoded, enc);
    s->next = head;
    head = s;
}

void decode_and_print(const char *name) {
    Stored *s = find_stored(name);
    if (!s) return;
    const char *p = s->encoded;
    while (*p) {
        char *end;
        long count = strtol(p, &end, 10);
        if (end == p) break; // no number
        if (count < 0 || count > 1000000) { // treat very large as invalid
            break;
        }
        p = end;
        if (*p == '\0') break;
        char ch = *p;
        p++;
        for (long i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
    }
    putchar('\n');
}

void delete_stored(const char *name) {
    Stored *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->encoded);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void list_stored(void) {
    Stored *s = head;
    while (s) {
        printf("%s\n", s->name);
        s = s->next;
    }
}

void free_all(void) {
    Stored *s = head;
    while (s) {
        Stored *tmp = s->next;
        free(s->name);
        free(s->encoded);
        free(s);
        s = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], enc[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *e = rest + strlen(name) + 1;
                while (*e == ' ') e++;
                if (*e) store_encoded(name, e);
                else store_encoded(name, "");
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) decode_and_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_stored(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_stored();
        }
    }
    free_all();
    return 0;
}