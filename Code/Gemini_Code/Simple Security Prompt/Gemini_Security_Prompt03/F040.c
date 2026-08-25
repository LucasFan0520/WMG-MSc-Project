// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Packet;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int main(void) {
    Packet *cache = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hl_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *pl_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *header = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *payload = p;
            cache = realloc(cache, (count + 1) * sizeof(Packet));
            cache[count].id = strdup(id);
            cache[count].h_len = atoi(hl_s);
            cache[count].p_len = atoi(pl_s);
            cache[count].header = strdup(header);
            cache[count].payload = strdup(payload);
            count++;
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (cache[i].id && strcmp(cache[i].id, id) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    cache[i].id = NULL;
                    cache[i].header = NULL;
                    cache[i].payload = NULL;
                    cache[i].h_len = 0;
                    cache[i].p_len = 0;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (cache[i].id && strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (cache[i].id && strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total_bytes = 0;
            for (int i = 0; i < count; i++) {
                if (cache[i].id) {
                    total_bytes += cache[i].p_len;
                }
            }
            printf("%d\n", total_bytes);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (cache[i].id) {
            free(cache[i].id);
            free(cache[i].header);
            free(cache[i].payload);
        }
    }
    free(cache);
    return 0;
}
