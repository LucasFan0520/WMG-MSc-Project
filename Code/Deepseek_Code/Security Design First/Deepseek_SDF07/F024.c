// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *title;
    char **authors;
    size_t acount;
    size_t acap;
    int year;
} Book;

typedef struct {
    Book *items;
    size_t count;
    size_t capacity;
} Library;

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

static void book_free(Book *b) {
    free(b->title);
    for (size_t i = 0; i < b->acount; i++)
        free(b->authors[i]);
    free(b->authors);
    b->title = NULL;
    b->authors = NULL;
    b->acount = 0;
    b->acap = 0;
}

static void lib_init(Library *lib) {
    lib->items = NULL;
    lib->count = 0;
    lib->capacity = 0;
}

static void lib_free(Library *lib) {
    for (size_t i = 0; i < lib->count; i++)
        book_free(&lib->items[i]);
    free(lib->items);
    lib->items = NULL;
    lib->count = 0;
    lib->capacity = 0;
}

static int lib_find(Library *lib, const char *title) {
    for (size_t i = 0; i < lib->count; i++) {
        if (strcmp(lib->items[i].title, title) == 0)
            return (int)i;
    }
    return -1;
}

static void lib_book(Library *lib, const char *title, int year) {
    if (lib_find(lib, title) >= 0) return;
    if (lib->count == lib->capacity) {
        size_t newcap = lib->capacity ? lib->capacity * 2 : 4;
        Book *tmp = realloc(lib->items, newcap * sizeof(Book));
        if (!tmp) return;
        lib->items = tmp;
        lib->capacity = newcap;
    }
    Book *b = &lib->items[lib->count];
    b->title = safe_strdup(title);
    b->year = year;
    b->authors = NULL;
    b->acount = 0;
    b->acap = 0;
    if (b->title)
        lib->count++;
    else
        free(b->title);
}

static void lib_author(Library *lib, const char *title, const char *name) {
    int idx = lib_find(lib, title);
    if (idx < 0) return;
    Book *b = &lib->items[idx];
    for (size_t i = 0; i < b->acount; i++) {
        if (strcmp(b->authors[i], name) == 0)
            return;
    }
    if (b->acount == b->acap) {
        size_t newcap = b->acap ? b->acap * 2 : 4;
        char **tmp = realloc(b->authors, newcap * sizeof(char *));
        if (!tmp) return;
        b->authors = tmp;
        b->acap = newcap;
    }
    b->authors[b->acount] = safe_strdup(name);
    if (b->authors[b->acount])
        b->acount++;
}

static void lib_removeauthor(Library *lib, const char *title, const char *name) {
    int idx = lib_find(lib, title);
    if (idx < 0) return;
    Book *b = &lib->items[idx];
    size_t write = 0;
    for (size_t i = 0; i < b->acount; i++) {
        if (strcmp(b->authors[i], name) != 0) {
            if (write != i) b->authors[write] = b->authors[i];
            write++;
        } else {
            free(b->authors[i]);
        }
    }
    b->acount = write;
}

static void lib_delete(Library *lib, const char *title) {
    int idx = lib_find(lib, title);
    if (idx < 0) return;
    book_free(&lib->items[idx]);
    for (size_t i = idx; i < lib->count - 1; i++)
        lib->items[i] = lib->items[i+1];
    lib->count--;
}

static void lib_find_print(Library *lib, const char *title) {
    int idx = lib_find(lib, title);
    if (idx < 0) return;
    Book *b = &lib->items[idx];
    printf("%d", b->year);
    for (size_t i = 0; i < b->acount; i++)
        printf(" %s", b->authors[i]);
    putchar('\n');
}

static void lib_list(Library *lib) {
    for (size_t i = 0; i < lib->count; i++) {
        printf("%s\n", lib->items[i].title);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
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
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    Library lib;
    lib_init(&lib);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "BOOK") == 0) {
            int year = atoi(a2);
            lib_book(&lib, a1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            lib_author(&lib, a1, a2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            lib_removeauthor(&lib, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            lib_delete(&lib, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            lib_find_print(&lib, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            lib_list(&lib);
        }
    }
    lib_free(&lib);
    return 0;
}