// filename: F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Encoded {
    char *name;
    char *decoded;
    struct Encoded *next;
} Encoded;

static Encoded *encodings = NULL;

static void cleanup(void) {
    while (encodings) {
        Encoded *tmp = encodings;
        encodings = encodings->next;
        free(tmp->name);
        free(tmp->decoded);
        free(tmp);
    }
}

static Encoded *find_encoding(const char *name) {
    for (Encoded *e = encodings; e; e = e->next)
        if (strcmp(e->name, name) == 0)
            return e;
    return NULL;
}

static char *decode_rle(const char *encoded) {
    // encoded format: digits followed by char, e.g., "3a2b"
    size_t len = strlen(encoded);
    char *result = malloc(1);
    if (!result) return NULL;
    size_t res_len = 0;
    size_t i = 0;
    while (i < len) {
        // read digits
        if (!(encoded[i] >= '0' && encoded[i] <= '9')) {
            free(result);
            return NULL;
        }
        long count = 0;
        while (i < len && encoded[i] >= '0' && encoded[i] <= '9') {
            count = count * 10 + (encoded[i] - '0');
            if (count > 1000000) { free(result); return NULL; } // arbitrary large limit
            ++i;
        }
        if (i >= len) { free(result); return NULL; }
        char ch = encoded[i++];
        // append ch count times
        size_t new_len = res_len + count;
        char *new_res = realloc(result, new_len + 1);
        if (!new_res) { free(result); return NULL; }
        result = new_res;
        for (long j = 0; j < count; ++j) {
            result[res_len++] = ch;
        }
    }
    result[res_len] = '\0';
    return result;
}

static void store_encoding(const char *name, const char *encoded) {
    if (find_encoding(name)) return;
    char *decoded = decode_rle(encoded);
    if (!decoded) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) { free(decoded); return; }
    e->name = strdup(name);
    e->decoded = decoded;
    if (!e->name) {
        free(decoded);
        free(e);
        return;
    }
    e->next = encodings;
    encodings = e;
}

static void decode_print(const char *name) {
    Encoded *e = find_encoding(name);
    if (!e) return;
    for (char *p = e->decoded; *p; ++p)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void delete_encoding(const char *name) {
    Encoded *prev = NULL;
    for (Encoded *e = encodings; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else encodings = e->next;
            free(e->name);
            free(e->decoded);
            free(e);
            return;
        }
        prev = e;
    }
}

static void list_encodings(void) {
    for (Encoded *e = encodings; e; e = e->next)
        printf("%s\n", e->name);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "STORE") == 0) {
            if (*arg1 && *arg2) store_encoding(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (*arg1) decode_print(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_encoding(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encodings();
        }
    }
    free(line);
    return 0;
}