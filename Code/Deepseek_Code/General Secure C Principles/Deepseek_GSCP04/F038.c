/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPAND 1000000

typedef struct Stored {
    char *name;
    char *encoded;
} Stored;

static Stored *stored = NULL;
static size_t scount = 0, scap = 0;

static Stored *find_stored(const char *name) {
    for (size_t i = 0; i < scount; i++) {
        if (strcmp(stored[i].name, name) == 0)
            return &stored[i];
    }
    return NULL;
}

static void store_encoded(const char *name, const char *encoded) {
    if (find_stored(name)) return;
    if (scount == scap) {
        scap = scap ? scap * 2 : 4;
        stored = realloc(stored, scap * sizeof(Stored));
        if (!stored) { perror("realloc"); exit(1); }
    }
    stored[scount].name = strdup(name);
    stored[scount].encoded = strdup(encoded);
    scount++;
}

static void delete_stored(const char *name) {
    for (size_t i = 0; i < scount; i++) {
        if (strcmp(stored[i].name, name) == 0) {
            free(stored[i].name);
            free(stored[i].encoded);
            for (size_t j = i; j < scount - 1; j++)
                stored[j] = stored[j + 1];
            scount--;
            return;
        }
    }
}

static void decode_stored(const char *name) {
    Stored *s = find_stored(name);
    if (!s) return;
    const char *p = s->encoded;
    char *out = malloc(MAX_EXPAND + 1);
    if (!out) { perror("malloc"); exit(1); }
    size_t outpos = 0;
    int valid = 1;
    while (*p) {
        if (!isdigit((unsigned char)*p)) { valid = 0; break; }
        int count = 0;
        while (isdigit((unsigned char)*p)) {
            count = count * 10 + (*p - '0');
            if (count > MAX_EXPAND) { valid = 0; break; }
            p++;
        }
        if (!valid) break;
        if (!*p) { valid = 0; break; }
        char ch = *p++;
        for (int i = 0; i < count; i++) {
            if (outpos >= MAX_EXPAND) { valid = 0; break; }
            out[outpos++] = ch;
        }
        if (!valid) break;
    }
    out[outpos] = '\0';
    if (valid) {
        for (char *c = out; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
    free(out);
}

static void list_stored(void) {
    for (size_t i = 0; i < scount; i++) {
        printf("%s\n", stored[i].name);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *enc = strtok(NULL, "");
            if (!enc) enc = "";
            store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) decode_stored(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_stored(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_stored();
        }
    }
    for (size_t i = 0; i < scount; i++) {
        free(stored[i].name);
        free(stored[i].encoded);
    }
    free(stored);
    return 0;
}