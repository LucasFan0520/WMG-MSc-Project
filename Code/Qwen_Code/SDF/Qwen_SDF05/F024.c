// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char *title;
    long year;
    char **authors;
    size_t acnt, acap;
} Book;

static Book *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Book *t = realloc(arr, nc * sizeof(Book));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int auth_grow(Book *b) {
    if (b->acnt < b->acap) return 0;
    size_t nc = b->acap ? b->acap * 2 : 4;
    if (nc < b->acap) return -1;
    char **t = realloc(b->authors, nc * sizeof(char *));
    if (!t) return -1;
    b->authors = t; b->acap = nc;
    return 0;
}

static int find_book(const char *title) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].title, title) == 0) return (int)i;
    return -1;
}

static void free_book(Book *b) {
    free(b->title);
    for (size_t i = 0; i < b->acnt; i++) free(b->authors[i]);
    free(b->authors);
    b->title = NULL; b->authors = NULL; b->acnt = 0; b->acap = 0;
}

static void cmd_book(char *title, long year) {
    if (find_book(title) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].title = safe_strdup(title);
    arr[cnt].year = year;
    arr[cnt].authors = NULL; arr[cnt].acnt = 0; arr[cnt].acap = 0;
    if (!arr[cnt].title) return;
    cnt++;
}

static void cmd_author(char *title, char *name) {
    int idx = find_book(title);
    if (idx < 0) return;
    Book *b = &arr[idx];
    for (size_t i = 0; i < b->acnt; i++)
        if (strcmp(b->authors[i], name) == 0) return;
    if (auth_grow(b) < 0) return;
    b->authors[b->acnt] = safe_strdup(name);
    if (b->authors[b->acnt]) b->acnt++;
}

static void cmd_removeauthor(char *title, char *name) {
    int idx = find_book(title);
    if (idx < 0) return;
    Book *b = &arr[idx];
    for (size_t i = 0; i < b->acnt; i++) {
        if (strcmp(b->authors[i], name) == 0) {
            free(b->authors[i]);
            for (size_t j = i; j + 1 < b->acnt; j++) b->authors[j] = b->authors[j + 1];
            b->acnt--;
            return;
        }
    }
}

static void cmd_delete(char *title) {
    int idx = find_book(title);
    if (idx < 0) return;
    free_book(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *title) {
    int idx = find_book(title);
    if (idx < 0) return;
    printf("%s %ld", arr[idx].title, arr[idx].year);
    for (size_t i = 0; i < arr[idx].acnt; i++) printf(" %s", arr[idx].authors[i]);
    putchar('\n');
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s %ld", arr[i].title, arr[i].year);
        for (size_t j = 0; j < arr[i].acnt; j++) printf(" %s", arr[i].authors[j]);
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_book(&arr[i]);
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
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256] = {0}; long year = 0;
            if (sscanf(rest, "%255s %ld", title, &year) < 2) continue;
            cmd_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 || strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256] = {0}, name[256] = {0};
            if (sscanf(rest, "%255s %255s", title, name) < 2) continue;
            if (strcmp(cmd, "AUTHOR") == 0) cmd_author(title, name);
            else cmd_removeauthor(title, name);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(rest, "%255s", title) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(title);
            else cmd_find(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}