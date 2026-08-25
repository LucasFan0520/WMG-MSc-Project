// F021.c
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
    char *ingredient;
    char *note;
} Ing;

typedef struct {
    char *name;
    Ing *ings;
    size_t ing_size;
    size_t ing_cap;
} Recipe;

typedef struct {
    Recipe *items;
    size_t size;
    size_t capacity;
} RList;

int main(void) {
    RList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) { found = 1; break; }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Recipe));
                }
                list.items[list.size].name = safe_strdup(name);
                list.items[list.size].ings = NULL;
                list.items[list.size].ing_size = 0;
                list.items[list.size].ing_cap = 0;
                list.size++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    if (list.items[i].ing_size == list.items[i].ing_cap) {
                        list.items[i].ing_cap = list.items[i].ing_cap ? list.items[i].ing_cap * 2 : 4;
                        list.items[i].ings = safe_realloc(list.items[i].ings, list.items[i].ing_cap * sizeof(Ing));
                    }
                    list.items[i].ings[list.items[i].ing_size].ingredient = safe_strdup(ing);
                    list.items[i].ings[list.items[i].ing_size].note = safe_strdup(p);
                    list.items[i].ing_size++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *ing = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    for (size_t j = 0; j < list.items[i].ing_size; j++) {
                        if (strcmp(list.items[i].ings[j].ingredient, ing) == 0) {
                            free(list.items[i].ings[j].ingredient);
                            free(list.items[i].ings[j].note);
                            for (size_t k = j; k < list.items[i].ing_size - 1; k++) {
                                list.items[i].ings[k] = list.items[i].ings[k + 1];
                            }
                            list.items[i].ing_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    free(list.items[i].name);
                    for (size_t j = 0; j < list.items[i].ing_size; j++) {
                        free(list.items[i].ings[j].ingredient);
                        free(list.items[i].ings[j].note);
                    }
                    free(list.items[i].ings);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    printf("%s\n", list.items[i].name);
                    for (size_t j = 0; j < list.items[i].ing_size; j++) {
                        printf("%s %s\n", list.items[i].ings[j].ingredient, list.items[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n", list.items[i].name);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        for (size_t j = 0; j < list.items[i].ing_size; j++) {
            free(list.items[i].ings[j].ingredient);
            free(list.items[i].ings[j].note);
        }
        free(list.items[i].ings);
    }
    free(list.items);
    return 0;
}