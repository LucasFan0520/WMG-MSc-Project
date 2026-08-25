// F040.c
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
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Pkt;

int main() {
    Pkt *cache = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "STATS") == 0) {
                long total = 0;
                for (int i = 0; i < count; i++) total += cache[i].p_len;
                printf("%ld\n", total);
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = args;
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *hlen_str = p2 + 1;
                char *p3 = strchr(hlen_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *plen_str = p3 + 1;
                    char *p4 = strchr(plen_str, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *header = p4 + 1;
                        char *p5 = strchr(header, ' ');
                        if (p5) {
                            *p5 = '\0';
                            char *payload = p5 + 1;
                            cache = realloc(cache, sizeof(Pkt) * (count + 1));
                            cache[count].id = strdup(id);
                            cache[count].h_len = atoi(hlen_str);
                            cache[count].p_len = atoi(plen_str);
                            cache[count].header = strdup(header);
                            cache[count].payload = strdup(payload);
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    free(cache[i].id); free(cache[i].header); free(cache[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    if (count == 0) { free(cache); cache = NULL; }
                    else { cache = realloc(cache, sizeof(Pkt) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cache[i].id); free(cache[i].header); free(cache[i].payload);
    }
    free(cache);
    return 0;
}
