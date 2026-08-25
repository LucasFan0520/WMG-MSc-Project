// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    int payload_len;
} Packet;

int main() {
    Packet *cache = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(NULL, " ");
            char *h_len_str = strtok(NULL, " ");
            char *p_len_str = strtok(NULL, " ");
            char *header = strtok(NULL, " ");
            char *payload = strtok(NULL, " ");
            if (id && h_len_str && p_len_str && header && payload) {
                int h_len = atoi(h_len_str);
                int p_len = atoi(p_len_str);
                if ((int)strlen(header) == h_len && (int)strlen(payload) == p_len) {
                    cache = realloc(cache, (count + 1) * sizeof(Packet));
                    cache[count].id = strdup(id);
                    cache[count].header = strdup(header);
                    cache[count].payload = strdup(payload);
                    cache[count].payload_len = p_len;
                    count++;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        free(cache[i].id);
                        free(cache[i].header);
                        free(cache[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            cache[j] = cache[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(cache);
                    cache = NULL;
                } else {
                    cache = realloc(cache, count * sizeof(Packet));
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].header);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].payload);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].payload_len;
            }
            printf("%d\n", total);
        }
    }
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
