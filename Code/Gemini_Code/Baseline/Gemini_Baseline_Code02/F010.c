// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *key;
    char **vals;
    int val_count;
} KeyList;

int main() {
    KeyList *lists = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "KEYS") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", lists[i].key);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = args;
                char *val = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(lists[i].key, key) == 0) { found = i; break; }
                }
                if (found == -1) {
                    lists = realloc(lists, sizeof(KeyList) * (count + 1));
                    lists[count].key = strdup(key);
                    lists[count].vals = malloc(sizeof(char*));
                    lists[count].vals[0] = strdup(val);
                    lists[count].val_count = 1;
                    count++;
                } else {
                    int vc = lists[found].val_count;
                    lists[found].vals = realloc(lists[found].vals, sizeof(char*) * (vc + 1));
                    lists[found].vals[vc] = strdup(val);
                    lists[found].val_count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = args;
                char *val = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(lists[i].key, key) == 0) {
                        for (int j = 0; j < lists[i].val_count; j++) {
                            if (strcmp(lists[i].vals[j], val) == 0) {
                                free(lists[i].vals[j]);
                                for (int k = j; k < lists[i].val_count - 1; k++) {
                                    lists[i].vals[k] = lists[i].vals[k + 1];
                                }
                                lists[i].val_count--;
                                if (lists[i].val_count == 0) {
                                    free(lists[i].vals);
                                    lists[i].vals = NULL;
                                } else {
                                    lists[i].vals = realloc(lists[i].vals, sizeof(char*) * lists[i].val_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(lists[i].key, key) == 0) {
                    free(lists[i].key);
                    for (int j = 0; j < lists[i].val_count; j++) free(lists[i].vals[j]);
                    free(lists[i].vals);
                    for (int j = i; j < count - 1; j++) {
                        lists[j] = lists[j + 1];
                    }
                    count--;
                    if (count == 0) { free(lists); lists = NULL; }
                    else { lists = realloc(lists, sizeof(KeyList) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(lists[i].key, key) == 0) {
                    for (int j = 0; j < lists[i].val_count; j++) {
                        printf("%s%s", lists[i].vals[j], (j == lists[i].val_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(lists[i].key);
        for (int j = 0; j < lists[i].val_count; j++) free(lists[i].vals[j]);
        free(lists[i].vals);
    }
    free(lists);
    return 0;
}
