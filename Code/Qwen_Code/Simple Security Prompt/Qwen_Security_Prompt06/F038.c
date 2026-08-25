// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef struct RLE {
    char *name;
    char *enc;
    struct RLE *next;
} RLE;

RLE *rles = NULL;

void store_rle(const char *name, const char *enc) {
    RLE *r = malloc(sizeof(RLE));
    r->name = strdup(name);
    r->enc = strdup(enc);
    r->next = rles;
    rles = r;
}

void decode_rle(const char *name) {
    RLE *r = NULL;
    for (RLE *curr = rles; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) { r = curr; break; }
    }
    if (!r) return;
    
    char *p = r->enc;
    while (*p) {
        if (!isdigit(*p)) return;
        long count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) return;
            p++;
        }
        if (!*p) return;
        char c = *p++;
        for (long i = 0; i < count; i++) {
            putchar(c == ' ' ? '_' : c);
        }
    }
    putchar('\n');
}

void delete_rle(const char *name) {
    RLE *prev = NULL, *curr = rles;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else rles = curr->next;
            free(curr->name);
            free(curr->enc);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_rles() {
    for (RLE *r = rles; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[1024] = {0};
        sscanf(line, "%15s %255s %1023s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "STORE") == 0) store_rle(arg1, arg2);
        else if (strcmp(cmd, "DECODE") == 0) decode_rle(arg1);
        else if (strcmp(cmd, "DELETE") == 0) delete_rle(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_rles();
        
        free(line);
    }
    return 0;
}