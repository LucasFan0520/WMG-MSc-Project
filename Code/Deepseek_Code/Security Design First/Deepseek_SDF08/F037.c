// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t len;
} Buffer;

typedef struct {
    Buffer *items;
    size_t count;
    size_t capacity;
} Store;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void buf_free(Buffer *b) {
    free(b->name);
    free(b->bytes);
    b->name = NULL;
    b->bytes = NULL;
    b->len = 0;
}

static void store_init(Store *s) {
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static void store_free(Store *s) {
    for (size_t i = 0; i < s->count; i++)
        buf_free(&s->items[i]);
    free(s->items);
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static int store_find(Store *s, const char *name) {
    for (size_t i = 0; i < s->count; i++) {
        if (strcmp(s->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static unsigned char hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0xFF;
}

static unsigned char *parse_hex(const char *s, size_t *outlen) {
    size_t len = strlen(s);
    if (len % 2 != 0) { *outlen = 0; return NULL; }
    unsigned char *bytes = malloc(len / 2);
    if (!bytes) { *outlen = 0; return NULL; }
    for (size_t i = 0; i < len; i += 2) {
        unsigned char hi = hexval(s[i]);
        unsigned char lo = hexval(s[i+1]);
        if (hi == 0xFF || lo == 0xFF) { free(bytes); *outlen = 0; return NULL; }
        bytes[i/2] = (hi << 4) | lo;
    }
    *outlen = len / 2;
    return bytes;
}

static void store_new(Store *s, const char *name, const char *hex) {
    if (store_find(s, name) >= 0) return;
    size_t len;
    unsigned char *bytes = parse_hex(hex, &len);
    if (!bytes) return;
    if (s->count == s->capacity) {
        size_t newcap = s->capacity ? s->capacity * 2 : 4;
        Buffer *tmp = realloc(s->items, newcap * sizeof(Buffer));
        if (!tmp) { free(bytes); return; }
        s->items = tmp;
        s->capacity = newcap;
    }
    s->items[s->count].name = safe_strdup(name);
    s->items[s->count].bytes = bytes;
    s->items[s->count].len = len;
    if (s->items[s->count].name)
        s->count++;
    else {
        free(s->items[s->count].name);
        free(bytes);
    }
}

static void store_append(Store *s, const char *name, const char *hex) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    size_t addlen;
    unsigned char *add = parse_hex(hex, &addlen);
    if (!add) return;
    Buffer *b = &s->items[idx];
    unsigned char *newbytes = realloc(b->bytes, b->len + addlen);
    if (!newbytes) { free(add); return; }
    memcpy(newbytes + b->len, add, addlen);
    b->bytes = newbytes;
    b->len += addlen;
    free(add);
}

static void store_slice(Store *s, const char *name, int offset, int length) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    Buffer *b = &s->items[idx];
    if (offset < 0 || offset >= (int)b->len || length <= 0) return;
    if (offset + length > (int)b->len) length = (int)b->len - offset;
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    putchar('\n');
}

static void store_delete(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    buf_free(&s->items[idx]);
    for (size_t i = idx; i < s->count - 1; i++)
        s->items[i] = s->items[i+1];
    s->count--;
}

static void store_list(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        printf("%s %zu\n", s->items[i].name, s->items[i].len);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Store s;
    store_init(&s);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096], a3[64];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "NEW") == 0) {
            store_new(&s, a1, a2);
        } else if (strcmp(cmd, "APPEND") == 0) {
            store_append(&s, a1, a2);
        } else if (strcmp(cmd, "SLICE") == 0) {
            int off = atoi(a2);
            int len = atoi(a3);
            store_slice(&s, a1, off, len);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            store_list(&s);
        }
    }
    store_free(&s);
    return 0;
}