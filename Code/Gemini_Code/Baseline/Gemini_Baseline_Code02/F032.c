// F032.c
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
    char *val;
} Sett;

int main() {
    Sett *committed = NULL;
    int c_count = 0;
    Sett *staged = NULL;
    int s_count = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "BEGIN") == 0) {
                in_tx = 1;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key); free(staged[i].val);
                }
                free(staged); staged = NULL; s_count = 0;
            } else if (strcmp(cmd, "COMMIT") == 0) {
                if (in_tx) {
                    for (int i = 0; i < s_count; i++) {
                        int found = 0;
                        for (int j = 0; j < c_count; j++) {
                            if (strcmp(committed[j].key, staged[i].key) == 0) {
                                free(committed[j].val);
                                committed[j].val = strdup(staged[i].val);
                                found = 1; break;
                            }
                        }
                        if (!found) {
                            committed = realloc(committed, sizeof(Sett) * (c_count + 1));
                            committed[c_count].key = strdup(staged[i].key);
                            committed[c_count].val = strdup(staged[i].val);
                            c_count++;
                        }
                    }
                    for (int i = 0; i < s_count; i++) {
                        free(staged[i].key); free(staged[i].val);
                    }
                    free(staged); staged = NULL; s_count = 0;
                    in_tx = 0;
                }
            } else if (strcmp(cmd, "ABORT") == 0) {
                if (in_tx) {
                    for (int i = 0; i < s_count; i++) {
                        free(staged[i].key); free(staged[i].val);
                    }
                    free(staged); staged = NULL; s_count = 0;
                    in_tx = 0;
                }
            } else if (strcmp(cmd, "DUMP") == 0) {
                for (int i = 0; i < c_count; i++) {
                    printf("%s=%s\n", committed[i].key, committed[i].val);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int found = 0;
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, args) == 0) {
                        free(committed[j].val);
                        committed[j].val = strdup(p2 + 1);
                        found = 1; break;
                    }
                }
                if (!found) {
                    committed = realloc(committed, sizeof(Sett) * (c_count + 1));
                    committed[c_count].key = strdup(args);
                    committed[c_count].val = strdup(p2 + 1);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *p2 = strchr(args, ' ');
                if (p2) {
                    *p2 = '\0';
                    int found = 0;
                    for (int j = 0; j < s_count; j++) {
                        if (strcmp(staged[j].key, args) == 0) {
                            free(staged[j].val);
                            staged[j].val = strdup(p2 + 1);
                            found = 1; break;
                        }
                    }
                    if (!found) {
                        staged = realloc(staged, sizeof(Sett) * (s_count + 1));
                        staged[s_count].key = strdup(args);
                        staged[s_count].val = strdup(p2 + 1);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            int found = 0;
            if (in_tx) {
                for (int j = 0; j < s_count; j++) {
                    if (strcmp(staged[j].key, args) == 0) {
                        printf("%s\n", staged[j].val);
                        found = 1; break;
                    }
                }
            }
            if (!found) {
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, args) == 0) {
                        printf("%s\n", committed[j].val);
                        found = 1; break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key); free(committed[i].val);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key); free(staged[i].val);
    }
    free(staged);
    return 0;
}
