// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *id;
    long long hlen;
    long long plen;
    char *header;
    char *payload;
} Packet;

int main(void) {
    Packet *cache = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *hlen_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *plen_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *header = p;
            while (*p && *p != ' ') p++;
            char *payload = "";
            if (*p == ' ') {
                *p = '\0';
                payload = p + 1;
            }
            long long hl = strtoll(hlen_str, NULL, 10);
            long long pl = strtoll(plen_str, NULL, 10);
            if (hl >= 0 && pl >= 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Packet *nb = realloc(cache, cap * sizeof(Packet));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    cache = nb;
                }
                cache[count].id = safe_dup(id);
                cache[count].hlen = hl;
                cache[count].plen = pl;
                cache[count].header = safe_dup(header);
                cache[count].payload = safe_dup(payload);
                count++;
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total_bytes = 0;
            for (size_t i = 0; i < count; i++) {
                total_bytes += cache[i].plen;
            }
            printf("%lld\n", total_bytes);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
