// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr) exit(EXIT_FAILURE);
    return ptr;
}

static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp) exit(EXIT_FAILURE);
    return temp;
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    char *ptr = strdup(s);
    if (!ptr) exit(EXIT_FAILURE);
    return ptr;
}

static char *read_line(FILE *stream) {
    size_t capacity = 256, length = 0;
    char *buffer = safe_malloc(capacity);
    while (fgets(buffer + length, capacity - length, stream)) {
        length += strlen(buffer + length);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
            return buffer;
        }
        if (length == capacity - 1) {
            if (capacity > SIZE_MAX / 2) { free(buffer); return NULL; }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0) return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char *key;
    char *value;
} Var;

typedef struct {
    Var *items;
    size_t size;
    size_t capacity;
} VList;

int main(void) {
    VList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *key_start = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, key_start) == 0) {
                    free(list.items[i].value);
                    list.items[i].value = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Var));
                }
                list.items[list.size].key = safe_strdup(key_start);
                list.items[list.size].value = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, key_start) == 0) {
                    free(list.items[i].key);
                    free(list.items[i].value);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *text = p;
            
            size_t out_cap = strlen(text) * 2 + 1;
            char *out = safe_malloc(out_cap);
            size_t out_len = 0;
            
            while (*text) {
                if (text[0] == '{') {
                    char *end = strchr(text + 1, '}');
                    if (end) {
                        size_t klen = end - (text + 1);
                        char *key = safe_malloc(klen + 1);
                        memcpy(key, text + 1, klen);
                        key[klen] = '\0';
                        
                        char *val = NULL;
                        for (size_t i = 0; i < list.size; i++) {
                            if (strcmp(list.items[i].key, key) == 0) {
                                val = list.items[i].value;
                                break;
                            }
                        }
                        free(key);
                        
                        if (val) {
                            size_t vlen = strlen(val);
                            if (out_len + vlen >= out_cap) {
                                out_cap = out_len + vlen + 1;
                                out = safe_realloc(out, out_cap);
                            }
                            memcpy(out + out_len, val, vlen);
                            out_len += vlen;
                            text = end + 1;
                            continue;
                        }
                    }
                }
                if (out_len + 1 >= out_cap) {
                    out_cap *= 2;
                    out = safe_realloc(out, out_cap);
                }
                out[out_len++] = *text++;
            }
            out[out_len] = '\0';
            printf("%s\n", out);
            free(out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s=%s\n", list.items[i].key, list.items[i].value);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].key);
        free(list.items[i].value);
    }
    free(list.items);
    return 0;
}