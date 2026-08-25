// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char **vals;
    size_t vcount;
    size_t vcap;
} KeyIndex;

int main() {
    KeyIndex *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *key_start = line + 4;
            char *space = strchr(key_start, ' ');
            if (space) {
                *space = '\0';
                char *val_start = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key_start) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(KeyIndex));
                    }
                    arr[count].key = strdup(key_start);
                    arr[count].vals = NULL;
                    arr[count].vcount = 0;
                    arr[count].vcap = 0;
                    found = (int)count;
                    count++;
                }
                KeyIndex *ki = &arr[found];
                if (ki->vcount >= ki->vcap) {
                    ki->vcap = ki->vcap == 0 ? 4 : ki->vcap * 2;
                    ki->vals = realloc(ki->vals, ki->vcap * sizeof(char *));
                }
                ki->vals[ki->vcount++] = strdup(val_start);
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *key_start = line + 7;
            char *space = strchr(key_start, ' ');
            if (space) {
                *space = '\0';
                char *val_start = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key_start) == 0) {
                        for (size_t j = 0; j < arr[i].vcount; j++) {
                            if (strcmp(arr[i].vals[j], val_start) == 0) {
                                free(arr[i].vals[j]);
                                for (size_t k = j; k < arr[i].vcount - 1; k++) {
                                    arr[i].vals[k] = arr[i].vals[k + 1];
                                }
                                arr[i].vcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key_start = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key_start) == 0) {
                    free(arr[i].key);
                    for (size_t j = 0; j < arr[i].vcount; j++) {
                        free(arr[i].vals[j]);
                    }
                    free(arr[i].vals);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key_start) == 0) {
                    for (size_t j = 0; j < arr[i].vcount; j++) {
                        printf("%s\n", arr[i].vals[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].key);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        for (size_t j = 0; j < arr[i].vcount; j++) {
            free(arr[i].vals[j]);
        }
        free(arr[i].vals);
    }
    free(arr);
    return 0;
}
