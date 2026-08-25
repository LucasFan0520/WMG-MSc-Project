// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    long long payload_len;
} Packet;

int main(void) {
    Packet *cache = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long h_len = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    long p_len = strtol(p, &endptr, 10);
                    if (endptr != p) {
                        p = endptr;
                        while (*p == ' ') p++;
                        char *header = p;
                        while (*p && *p != ' ') p++;
                        if (*p == ' ') {
                            *p = '\0';
                            char *payload = p + 1;
                            while (*payload == ' ') payload++;
                            if ((long)strlen(header) == h_len && (long)strlen(payload) == p_len) {
                                if (count >= capacity) {
                                    capacity = capacity == 0 ? 4 : capacity * 2;
                                    Packet *new_c = realloc(cache, capacity * sizeof(Packet));
                                    if (new_c) cache = new_c;
                                }
                                if (count < capacity) {
                                    cache[count].id = strdup(id);
                                    cache[count].header = strdup(header);
                                    cache[count].payload = strdup(payload);
                                    cache[count].payload_len = p_len;
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].payload_len;
            }
            printf("%lld\n", total);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
