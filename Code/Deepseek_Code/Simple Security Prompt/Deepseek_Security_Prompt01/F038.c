// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *encoded; } RLE;
RLE *rles = NULL;
int rc = 0, rcap = 0;

int find_rle(const char *name) {
    for (int i = 0; i < rc; i++)
        if (strcmp(rles[i].name, name) == 0) return i;
    return -1;
}

void store_rle(const char *name, const char *encoded) {
    if (find_rle(name) >= 0) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; rles = realloc(rles, sizeof(RLE) * rcap); }
    rles[rc].name = strdup(name);
    rles[rc].encoded = strdup(encoded);
    rc++;
}

void decode_rle(const char *name) {
    int idx = find_rle(name);
    if (idx < 0) return;
    const char *s = rles[idx].encoded;
    while (*s) {
        char *end;
        long count = strtol(s, &end, 10);
        if (end == s) break; // no digits
        if (count < 0 || count > 1000000) { printf("INVALID\n"); return; }
        s = end;
        if (!*s) break;
        char ch = *s++;
        for (long i = 0; i < count; i++) putchar(ch);
    }
    putchar('\n');
}

void delete_rle(const char *name) {
    int idx = find_rle(name);
    if (idx < 0) return;
    free(rles[idx].name);
    free(rles[idx].encoded);
    for (int i = idx; i < rc - 1; i++) rles[i] = rles[i + 1];
    rc--;
}

void list_rles(void) {
    for (int i = 0; i < rc; i++) printf("%s\n", rles[i].name);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], enc[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, enc) == 2)
                store_rle(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) decode_rle(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_rle(name);
        } else if (strcmp(cmd, "LIST") == 0) list_rles();
    }
    for (int i = 0; i < rc; i++) { free(rles[i].name); free(rles[i].encoded); }
    free(rles);
    return 0;
}