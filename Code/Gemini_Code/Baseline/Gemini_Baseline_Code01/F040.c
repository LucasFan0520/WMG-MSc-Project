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

int main() {
    Packet *cache = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256], header[1024], payload[4096];
            int h_len = 0, p_len = 0;
            if (sscanf(args, "%255s %d %d %1023s %4095s", id, &h_len, &p_len, header, payload) == 5) {
                cache = realloc(cache, (count + 1) * sizeof(Packet));
                cache[count].id = strdup(id);
                cache[count].h_len = h_len;
                cache[count].p_len = p_len;
                cache[count].header = strdup(header);
                cache[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        free(cache[i].id);
                        free(cache[i].header);
                        free(cache[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            cache[j] = cache[j + 1];
                        }
                        count--;
                        cache = realloc(cache, count * sizeof(Packet));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].header);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].payload);
                        break;
                    }
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
