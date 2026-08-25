// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size, cap;
} Buffer;

static Buffer *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Buffer *t = realloc(arr, nc * sizeof(Buffer));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int buf_grow(Buffer *b, size_t needed) {
    if (needed <= b->cap) return 0;
    size_t nc = b->cap ? b->cap : 16;
    while (nc < needed) {
        if (nc > (size_t)-1 / 2) return -1;
        nc *= 2;
    }
    unsigned char *t = realloc(b->bytes, nc);
    if (!t) return -1;
    b->bytes = t; b->cap = nc;
    return 0;
}

static int find_buffer(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_buffer(Buffer *b) {
    free(b->name); free(b->bytes);
    b->name = NULL; b->bytes = NULL; b->size = 0; b->cap = 0;
}

static int parse_hex(const char *hex, unsigned char **out, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return -1;
    size_t byte_len = len / 2;
    unsigned char *bytes = malloc(byte_len);
    if (!bytes) return -1;
    for (size_t i = 0; i < byte_len; i++) {
        char h = hex[i * 2];
        char l = hex[i * 2 + 1];
        if (!isxdigit((unsigned char)h) || !isxdigit((unsigned char)l)) {
            free(bytes); return -1;
        }
        unsigned char val = 0;
        if (h >= '0' && h <= '9') val = (unsigned char)(h - '0');
        else if (h >= 'a' && h <= 'f') val = (unsigned char)(h - 'a' + 10);
        else val = (unsigned char)(h - 'A' + 10);
        val <<= 4;
        if (l >= '0' && l <= '9') val |= (unsigned char)(l - '0');
        else if (l >= 'a' && l <= 'f') val |= (unsigned char)(l - 'a' + 10);
        else val |= (unsigned char)(l - 'A' + 10);
        bytes[i] = val;
    }
    *out = bytes;
    *out_len = byte_len;
    return 0;
}

static void cmd_new(char *name, char *hex) {
    if (find_buffer(name) >= 0) return;
    unsigned char *bytes = NULL; size_t byte_len = 0;
    if (parse_hex(hex, &bytes, &byte_len) < 0) return;
    if (grow() < 0) { free(bytes); return; }
    arr[cnt].name = safe_strdup(name);
    arr[cnt].bytes = bytes;
    arr[cnt].size = byte_len;
    arr[cnt].cap = byte_len;
    if (!arr[cnt].name) { free(bytes); return; }
    cnt++;
}

static void cmd_append(char *name, char *hex) {
    int idx = find_buffer(name);
    if (idx < 0) return;
    unsigned char *bytes = NULL; size_t byte_len = 0;
    if (parse_hex(hex, &bytes, &byte_len) < 0) return;
    Buffer *b = &arr[idx];
    if (buf_grow(b, b->size + byte_len) < 0) { free(bytes); return; }
    memcpy(b->bytes + b->size, bytes, byte_len);
    b->size += byte_len;
    free(bytes);
}

static void cmd_slice(char *name, size_t offset, size_t length) {
    int idx = find_buffer(name);
    if (idx < 0) return;
    Buffer *b = &arr[idx];
    if (offset > b->size) return;
    if (length > b->size - offset) length = b->size - offset;
    for (size_t i = 0; i < length; i++)
        printf("%02x", b->bytes[offset + i]);
    putchar('\n');
}

static void cmd_delete(char *name) {
    int idx = find_buffer(name);
    if (idx < 0) return;
    free_buffer(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %zu\n", arr[i].name, arr[i].size);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_buffer(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *hex = rest + n2;
            while (*hex == ' ') hex++;
            if (strcmp(cmd, "NEW") == 0) cmd_new(name, hex);
            else cmd_append(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256] = {0}; size_t offset = 0, length = 0;
            if (sscanf(rest, "%255s %zu %zu", name, &offset, &length) < 3) continue;
            cmd_slice(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_delete(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}