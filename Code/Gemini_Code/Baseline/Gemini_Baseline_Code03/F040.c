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
} PacketCache;

int main() {
    PacketCache *cache = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256], head[1024], pay[1024];
            int h_len = 0, p_len = 0;
            if (sscanf(ptr, "%255s %d %d %1023s %1023s", id, &h_len, &p_len, head, pay) == 5) {
                cache = realloc(cache, (count + 1) * sizeof(PacketCache));
                cache[count].id = strdup(id);
                cache[count].h_len = h_len;
                cache[count].p_len = p_len;
                cache[count].header = strdup(head);
                cache[count].payload = strdup(pay);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
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
                    if (count > 0) {
                        cache = realloc(cache, count * sizeof(PacketCache));
                    } else {
                        free(cache);
                        cache = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", cache[found].header);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", cache[found].payload);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].p_len;
            }
            printf("%ld\n", total);
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
