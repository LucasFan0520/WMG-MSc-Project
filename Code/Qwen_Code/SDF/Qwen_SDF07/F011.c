// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *title; char *body; } Draft;
Draft *drafts = NULL;
size_t dc = 0, dcap = 0;

Draft *discarded = NULL;
size_t disc_c = 0, disc_cap = 0;

void add_draft(const char *title, const char *body) {
    for (size_t i = 0; i < dc; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            char *tmp = strdup(body);
            if (!tmp) { perror("strdup"); exit(1); }
            free(drafts[i].body);
            drafts[i].body = tmp;
            return;
        }
    }
    if (dc == dcap) {
        size_t nc = dcap ? dcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Draft)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Draft *tmp = realloc(drafts, nc * sizeof(Draft));
        if (!tmp) { perror("realloc"); exit(1); }
        drafts = tmp;
        dcap = nc;
    }
    drafts[dc].title = strdup(title);
    drafts[dc].body = strdup(body);
    if (!drafts[dc].title || !drafts[dc].body) { perror("strdup"); exit(1); }
    dc++;
}

void discard_draft(const char *title) {
    for (size_t i = 0; i < dc; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            if (disc_c == disc_cap) {
                size_t nc = disc_cap ? disc_cap * 2 : 4;
                if (nc > SIZE_MAX / sizeof(Draft)) { fprintf(stderr, "Overflow\n"); exit(1); }
                Draft *tmp = realloc(discarded, nc * sizeof(Draft));
                if (!tmp) { perror("realloc"); exit(1); }
                discarded = tmp;
                disc_cap = nc;
            }
            discarded[disc_c++] = drafts[i];
            drafts[i] = drafts[--dc];
            return;
        }
    }
}

void restore_draft() {
    if (disc_c == 0) return;
    Draft d = discarded[--disc_c];
    if (dc == dcap) {
        size_t nc = dcap ? dcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Draft)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Draft *tmp = realloc(drafts, nc * sizeof(Draft));
        if (!tmp) { perror("realloc"); exit(1); }
        drafts = tmp;
        dcap = nc;
    }
    drafts[dc++] = d;
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) != 1) { free(line); continue; }
            char *body = strchr(line, ' ');
            if (body) { body = strchr(body + 1, ' '); if (body) body++; }
            if (!body) body = "";
            add_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                for (size_t i = 0; i < dc; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        printf("%s\n", drafts[i].body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < dc; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
        free(line);
    }
    for (size_t i = 0; i < dc; i++) { free(drafts[i].title); free(drafts[i].body); }
    free(drafts);
    for (size_t i = 0; i < disc_c; i++) { free(discarded[i].title); free(discarded[i].body); }
    free(discarded);
    return 0;
}