// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    int payload_len;
} PacketRecord;

int main() {
    PacketRecord *cache = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "PACKET") == 0 && args) {
            char *id = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int h_len = atoi(space2 + 1);
                char *space3 = strchr(space2 + 1, ' ');
                if (space3) {
                    int p_len = atoi(space3 + 1);
                    char *space4 = strchr(space3 + 1, ' ');
                    if (space4) {
                        *space4 = '\0';
                        char *hdr = space4 + 1;
                        char *space5 = strchr(hdr, ' ');
                        if (space5) {
                            *space5 = '\0';
                            char *pay = space5 + 1;

                            cache = realloc(cache, (count + 1) * sizeof(PacketRecord));
                            if (!cache) return 1;
                            cache[count].id = strdup(id);
                            cache[count].header = strdup(hdr);
                            cache[count].payload = strdup(pay);
                            cache[count].payload_len = p_len;
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    cache = realloc(cache, count * sizeof(PacketRecord));
                    if (count > 0 && !cache) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, args) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].payload_len;
            }
            printf("%lld\n", total);
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
