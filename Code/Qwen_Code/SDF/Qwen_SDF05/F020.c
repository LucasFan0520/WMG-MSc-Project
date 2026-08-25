// F020.c
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
    char **body_lines;
    size_t bcnt, bcap;
} Record;

static Record *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Record *t = realloc(arr, nc * sizeof(Record));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int body_grow(Record *r) {
    if (r->bcnt < r->bcap) return 0;
    size_t nc = r->bcap ? r->bcap * 2 : 4;
    if (nc < r->bcap) return -1;
    char **t = realloc(r->body_lines, nc * sizeof(char *));
    if (!t) return -1;
    r->body_lines = t; r->bcap = nc;
    return 0;
}

static int find_record(const char *title) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].title, title) == 0) return (int)i;
    return -1;
}

static void free_record(Record *r) {
    free(r->title);
    for (size_t i = 0; i < r->bcnt; i++) free(r->body_lines[i]);
    free(r->body_lines);
    r->title = NULL; r->body_lines = NULL; r->bcnt = 0; r->bcap = 0;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    int in_record = 0;
    Record cur;
    memset(&cur, 0, sizeof(cur));
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (in_record && cur.title) {
                if (grow() >= 0) {
                    arr[cnt] = cur;
                    cnt++;
                } else {
                    free_record(&cur);
                }
            } else if (in_record) {
                free_record(&cur);
            }
            memset(&cur, 0, sizeof(cur));
            in_record = 0;
        } else {
            if (!in_record) {
                cur.title = safe_strdup(line);
                if (!cur.title) continue;
                in_record = 1;
            } else {
                if (body_grow(&cur) >= 0) {
                    cur.body_lines[cur.bcnt] = safe_strdup(line);
                    if (cur.body_lines[cur.bcnt]) cur.bcnt++;
                }
            }
        }
    }
    if (in_record && cur.title) {
        if (grow() >= 0) { arr[cnt] = cur; cnt++; }
        else free_record(&cur);
    } else if (in_record) {
        free_record(&cur);
    }
    fclose(f);
}

static void cmd_find(char *title) {
    int idx = find_record(title);
    if (idx < 0) return;
    printf("%s\n", arr[idx].title);
    for (size_t i = 0; i < arr[idx].bcnt; i++)
        printf("%s\n", arr[idx].body_lines[i]);
    printf("--\n");
}

static void cmd_delete(char *title) {
    int idx = find_record(title);
    if (idx < 0) return;
    free_record(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_add(void) {
    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), stdin)) return;
    line[strcspn(line, "\n")] = '\0';
    char *title = line;
    if (find_record(title) >= 0) {
        while (fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "--") == 0) break;
        }
        return;
    }
    if (grow() < 0) {
        while (fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "--") == 0) break;
        }
        return;
    }
    arr[cnt].title = safe_strdup(title);
    arr[cnt].body_lines = NULL;
    arr[cnt].bcnt = 0;
    arr[cnt].bcap = 0;
    if (!arr[cnt].title) {
        while (fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "--") == 0) break;
        }
        return;
    }
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        if (body_grow(&arr[cnt]) >= 0) {
            arr[cnt].body_lines[arr[cnt].bcnt] = safe_strdup(line);
            if (arr[cnt].body_lines[arr[cnt].bcnt]) arr[cnt].bcnt++;
        }
    }
    cnt++;
}

static void cmd_print(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s\n", arr[i].title);
        for (size_t j = 0; j < arr[i].bcnt; j++)
            printf("%s\n", arr[i].body_lines[j]);
        printf("--\n");
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_record(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strcmp(line, "FIND") == 0 || strcmp(line, "DELETE") == 0) {
            char rest[MAX_LINE];
            if (!fgets(rest, sizeof(rest), stdin)) break;
            rest[strcspn(rest, "\n")] = '\0';
            if (strcmp(line, "FIND") == 0) cmd_find(rest);
            else cmd_delete(rest);
        } else if (strcmp(line, "ADD") == 0) {
            cmd_add();
        } else if (strcmp(line, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}