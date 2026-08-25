/* F040.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    int p_len;
} PacketRec;

int main(void) {
    PacketRec *cache = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "PACKET") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *h_len_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *p_len_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *header = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *payload = p;

            int h_len = atoi(h_len_str);
            int p_len = atoi(p_len_str);

            if (h_len >= 0 && p_len >= 0 && strlen(header) >= (size_t)h_len && strlen(payload) >= (size_t)p_len) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    PacketRec *temp = realloc(cache, new_cap * sizeof(PacketRec));
                    if (!temp) break;
                    cache = temp;
                    capacity = new_cap;
                }
                cache[count].id = strdup(id);
                cache[count].header = malloc((size_t)h_len + 1);
                cache[count].payload = malloc((size_t)p_len + 1);
                if (cache[count].id && cache[count].header && cache[count].payload) {
                    memcpy(cache[count].header, header, (size_t)h_len);
                    cache[count].header[h_len] = '\0';
                    memcpy(cache[count].payload, payload, (size_t)p_len);
                    cache[count].payload[p_len] = '\0';
                    cache[count].p_len = p_len;
                    count++;
                } else {
                    free(cache[count].id);
                    free(cache[count].header);
                    free(cache[count].payload);
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = p;
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
            char *id = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (size_t i = 0; i < count; i++) {
                total += cache[i].p_len;
            }
            printf("%lld\n", total);
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
