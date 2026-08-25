// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
} PacketRecord;

int main(void) {
    PacketRecord *cache = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char id[256], hstr[1024], pstr[1024];
            int hl, pl;
            if (sscanf(line + 7, "%255s %d %d %1023s %1023s", id, &hl, &pl, hstr, pstr) == 5) {
                PacketRecord *tmp = realloc(cache, (count + 1) * sizeof(PacketRecord));
                if (tmp) {
                    cache = tmp;
                    cache[count].id = strdup(id);
                    cache[count].hlen = hl;
                    cache[count].plen = pl;
                    cache[count].header = strdup(hstr);
                    cache[count].payload = strdup(pstr);
                    count++;
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].plen;
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
