// F023.c
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
    char *tag;
} Tag;
typedef struct {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    size_t t_size;
    size_t t_cap;
} Mail;
typedef struct {
    Mail *items;
    size_t size;
    size_t capacity;
} MList;

int main(void) {
    MList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *sender = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *subject = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Mail));
            }
            list.items[list.size].sender = safe_strdup(sender);
            list.items[list.size].subject = safe_strdup(subject);
            list.items[list.size].body = safe_strdup(p);
            list.items[list.size].tags = NULL;
            list.items[list.size].t_size = 0;
            list.items[list.size].t_cap = 0;
            list.size++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *subject = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *tag = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].subject, subject) == 0) {
                    if (list.items[i].t_size == list.items[i].t_cap) {
                        list.items[i].t_cap = list.items[i].t_cap ? list.items[i].t_cap * 2 : 4;
                        list.items[i].tags =
                            safe_realloc(list.items[i].tags, list.items[i].t_cap * sizeof(Tag));
                    }
                    list.items[i].tags[list.items[i].t_size++].tag = safe_strdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *p = line + 5;
            while (*p == ' ')
                p++;
            char *subject = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *tag = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].subject, subject) == 0) {
                    for (size_t j = 0; j < list.items[i].t_size; j++) {
                        if (strcmp(list.items[i].tags[j].tag, tag) == 0) {
                            free(list.items[i].tags[j].tag);
                            for (size_t k = j; k < list.items[i].t_size - 1; k++)
                                list.items[i].tags[k] = list.items[i].tags[k + 1];
                            list.items[i].t_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *subject = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].subject, subject) == 0) {
                    free(list.items[i].sender);
                    free(list.items[i].subject);
                    free(list.items[i].body);
                    for (size_t j = 0; j < list.items[i].t_size; j++)
                        free(list.items[i].tags[j].tag);
                    free(list.items[i].tags);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *tag = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                for (size_t j = 0; j < list.items[i].t_size; j++) {
                    if (strcmp(list.items[i].tags[j].tag, tag) == 0) {
                        printf("%s\n", list.items[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %s %s", list.items[i].sender, list.items[i].subject, list.items[i].body);
                for (size_t j = 0; j < list.items[i].t_size; j++)
                    printf(" [%s]", list.items[i].tags[j].tag);
                printf("\n");
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].sender);
        free(list.items[i].subject);
        free(list.items[i].body);
        for (size_t j = 0; j < list.items[i].t_size; j++)
            free(list.items[i].tags[j].tag);
        free(list.items[i].tags);
    }
    free(list.items);
    return 0;
}