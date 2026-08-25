// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char **values;
    int val_count;
} KeyList;

int main() {
    KeyList *index = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char k[128], v[128];
            if (sscanf(line + 4, "%127s %127s", k, v) == 2) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        index[i].values = realloc(index[i].values, (index[i].val_count + 1) * sizeof(char *));
                        index[i].values[index[i].val_count] = strdup(v);
                        index[i].val_count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    index = realloc(index, (count + 1) * sizeof(KeyList));
                    index[count].key = strdup(k);
                    index[count].values = malloc(sizeof(char *));
                    index[count].values[0] = strdup(v);
                    index[count].val_count = 1;
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char k[128], v[128];
            if (sscanf(line + 7, "%127s %127s", k, v) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        for (int j = 0; j < index[i].val_count; j++) {
                            if (strcmp(index[i].values[j], v) == 0) {
                                free(index[i].values[j]);
                                for (int m = j; m < index[i].val_count - 1; m++) {
                                    index[i].values[m] = index[i].values[m + 1];
                                }
                                index[i].val_count--;
                                index[i].values = realloc(index[i].values, index[i].val_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *k = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    free(index[i].key);
                    for (int j = 0; j < index[i].val_count; j++) {
                        free(index[i].values[j]);
                    }
                    free(index[i].values);
                    for (int j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                    index = realloc(index, count * sizeof(KeyList));
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *k = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    for (int j = 0; j < index[i].val_count; j++) {
                        printf("%s", index[i].values[j]);
                        if (j < index[i].val_count - 1) printf(" ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(index[i].key);
        for (int j = 0; j < index[i].val_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
