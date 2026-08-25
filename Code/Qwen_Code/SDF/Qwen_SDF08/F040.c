// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp)
        exit(EXIT_FAILURE);
    return temp;
}
static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;
    char *ptr = strdup(s);
    if (!ptr)
        exit(EXIT_FAILURE);
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
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return NULL;
            }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0)
        return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Pkt;
typedef struct {
    Pkt *items;
    size_t size;
    size_t capacity;
} PList;

int main(void) {
    PList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *id = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int h_len = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int p_len = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *header = p;
            size_t h_actual = strlen(header);
            if (h_len >= 0 && (size_t)h_len <= h_actual) {
                char *payload = header + h_len;
                size_t p_actual = strlen(payload);
                if (p_len >= 0 && (size_t)p_len <= p_actual) {
                    if (list.size == list.capacity) {
                        list.capacity = list.capacity ? list.capacity * 2 : 4;
                        list.items = safe_realloc(list.items, list.capacity * sizeof(Pkt));
                    }
                    list.items[list.size].id = safe_strdup(id);
                    list.items[list.size].h_len = h_len;
                    list.items[list.size].p_len = p_len;
                    list.items[list.size].header = safe_malloc(h_len + 1);
                    memcpy(list.items[list.size].header, header, h_len);
                    list.items[list.size].header[h_len] = '\0';
                    list.items[list.size].payload = safe_malloc(p_len + 1);
                    memcpy(list.items[list.size].payload, payload, p_len);
                    list.items[list.size].payload[p_len] = '\0';
                    list.size++;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *id = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id) == 0) {
                    free(list.items[i].id);
                    free(list.items[i].header);
                    free(list.items[i].payload);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *id = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id) == 0) {
                    printf("%s\n", list.items[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 7;
            while (*p == ' ')
                p++;
            char *id = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id) == 0) {
                    printf("%s\n", list.items[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (size_t i = 0; i < list.size; i++)
                total += list.items[i].p_len;
            printf("%ld\n", total);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].id);
        free(list.items[i].header);
        free(list.items[i].payload);
    }
    free(list.items);
    return 0;
}