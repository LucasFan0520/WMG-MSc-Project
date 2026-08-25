// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Packet;

int main() {
    Packet *cache = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "STATS") == 0) {
            int total_bytes = 0;
            for (int i = 0; i < count; i++) {
                total_bytes += cache[i].p_len;
            }
            printf("%d\n", total_bytes);
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256], header[512], payload[512];
            int h_len = 0, p_len = 0;
            if (sscanf(p, "%255s %d %d %511s %511s", id, &h_len, &p_len, header, payload) == 5) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    cache = realloc(cache, capacity * sizeof(Packet));
                }
                cache[count].id = strdup(id);
                cache[count].h_len = h_len;
                cache[count].p_len = p_len;
                cache[count].header = strdup(header);
                cache[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(p, "%255s", id) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(cache[found].id);
                    free(cache[found].header);
                    free(cache[found].payload);
                    for (int i = found; i < count - 1; i++) {
                        cache[i] = cache[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(p, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].header);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(p, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].payload);
                        break;
                    }
                }
            }
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
