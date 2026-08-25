// F015.c
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

static char *safe_strcat(const char *s1, const char *s2) {
    size_t len1 = s1 ? strlen(s1) : 0;
    size_t len2 = s2 ? strlen(s2) : 0;
    if (len1 > SIZE_MAX - len2 - 1) exit(EXIT_FAILURE);
    char *res = safe_malloc(len1 + len2 + 1);
    if (s1) memcpy(res, s1, len1);
    if (s2) memcpy(res + len1, s2, len2 + 1);
    else res[len1] = '\0';
    return res;
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
    char *name;
    char *email;
    char *phone;
    char *note;
} Con;

typedef struct {
    Con *items;
    size_t size;
    size_t capacity;
} CList;

int main(void) {
    CList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *e_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *ph_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Con));
            }
            list.items[list.size].name = safe_strdup(n_start);
            list.items[list.size].email = safe_strdup(e_start);
            list.items[list.size].phone = safe_strdup(ph_start);
            list.items[list.size].note = safe_strdup("");
            list.size++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    free(list.items[i].note);
                    list.items[i].note = safe_strdup(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *rem = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            Con *k_con = NULL, *r_con = NULL;
            size_t k_idx = 0, r_idx = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, keep) == 0) { k_con = &list.items[i]; k_idx = i; }
                if (strcmp(list.items[i].name, rem) == 0) { r_con = &list.items[i]; r_idx = i; }
            }
            if (k_con && r_con) {
                char *new_note = safe_strcat(k_con->note, " ");
                char *final_note = safe_strcat(new_note, r_con->note);
                free(new_note);
                free(k_con->note);
                k_con->note = final_note;
                
                free(r_con->name);
                free(r_con->email);
                free(r_con->phone);
                free(r_con->note);
                for (size_t i = r_idx; i < list.size - 1; i++) {
                    list.items[i] = list.items[i + 1];
                }
                list.size--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    free(list.items[i].name);
                    free(list.items[i].email);
                    free(list.items[i].phone);
                    free(list.items[i].note);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    printf("%s %s %s %s\n", list.items[i].name, list.items[i].email, list.items[i].phone, list.items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %s %s %s\n", list.items[i].name, list.items[i].email, list.items[i].phone, list.items[i].note);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        free(list.items[i].email);
        free(list.items[i].phone);
        free(list.items[i].note);
    }
    free(list.items);
    return 0;
}