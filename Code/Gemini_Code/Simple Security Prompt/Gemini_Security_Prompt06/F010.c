// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int v_count;
} IndexPair;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    IndexPair *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "KEYS") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].key);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REMOVE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *val = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "ADD") == 0) {
                    if (found == -1) {
                        list = realloc(list, (count + 1) * sizeof(IndexPair));
                        list[count].key = strdup(key);
                        list[count].values = malloc(sizeof(char *));
                        list[count].values[0] = strdup(val);
                        list[count].v_count = 1;
                        count++;
                    } else {
                        int vc = list[found].v_count;
                        list[found].values = realloc(list[found].values, (vc + 1) * sizeof(char *));
                        list[found].values[vc] = strdup(val);
                        list[found].v_count++;
                    }
                } else {
                    if (found != -1) {
                        int v_found = -1;
                        for (int i = 0; i < list[found].v_count; i++) {
                            if (strcmp(list[found].values[i], val) == 0) {
                                v_found = i;
                                break;
                            }
                        }
                        if (v_found != -1) {
                            free(list[found].values[v_found]);
                            for (int i = v_found; i < list[found].v_count - 1; i++) {
                                list[found].values[i] = list[found].values[i + 1];
                            }
                            list[found].v_count--;
                            if (list[found].v_count == 0) {
                                free(list[found].values);
                                list[found].values = NULL;
                            } else {
                                list[found].values = realloc(list[found].values, list[found].v_count * sizeof(char *));
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].key);
                for (int i = 0; i < list[found].v_count; i++) {
                    free(list[found].values[i]);
                }
                free(list[found].values);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(IndexPair));
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int i = 0; i < list[found].v_count; i++) {
                    printf("%s%s", list[found].values[i], (i == list[found].v_count - 1) ? "" : " ");
                }
                printf("\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        for (int j = 0; j < list[i].v_count; j++) {
            free(list[i].values[j]);
        }
        free(list[i].values);
    }
    free(list);
    return 0;
}