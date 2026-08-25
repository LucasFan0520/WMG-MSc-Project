// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    long long payload_len;
} Packet;

int main(void) {
    Packet *cache = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hlen_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *plen_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hdr = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *pay = p;

            if (*id == '\0' || *hlen_str == '\0' || *plen_str == '\0') continue;
            long long h_len = strtoll(hlen_str, NULL, 10);
            long long p_len = strtoll(plen_str, NULL, 10);

            if (h_len < 0 || p_len < 0 || (size_t)h_len != strlen(hdr) || (size_t)p_len != strlen(pay)) continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Packet *new_cache = realloc(cache, new_cap * sizeof(Packet));
                if (!new_cache) continue;
                cache = new_cache;
                capacity = new_cap;
            }

            char *p_id = strdup(id);
            char *p_hdr = strdup(hdr);
            char *p_pay = strdup(pay);
            if (p_id && p_hdr && p_pay) {
                cache[count].id = p_id;
                cache[count].header = p_hdr;
                cache[count].payload = p_pay;
                cache[count].payload_len = p_len;
                count++;
            } else {
                free(p_id);
                free(p_hdr);
                free(p_pay);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

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
        } else if (strcmp(cmd, "HEADER") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total_bytes = 0;
            for (size_t i = 0; i < count; i++) {
                total_bytes += cache[i].payload_len;
            }
            printf("%lld\n", total_bytes);
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    free(line);
    return 0;
}
