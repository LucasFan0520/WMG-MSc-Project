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

Packet *cache = NULL;
int count = 0;
int capacity = 0;

int find_packet(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(cache[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p1 = line + 7;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *p3 = sp2 + 1;
                    char *sp3 = strchr(p3, ' ');
                    if (sp3) {
                        *sp3 = 0;
                        char *p4 = sp3 + 1;
                        char *sp4 = strchr(p4, ' ');
                        if (sp4) {
                            *sp4 = 0;
                            char *payload = sp4 + 1;
                            int h_len = atoi(p2);
                            int p_len = atoi(p3);
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                cache = realloc(cache, capacity * sizeof(Packet));
                            }
                            cache[count].id = strdup(p1);
                            cache[count].h_len = h_len;
                            cache[count].p_len = p_len;
                            cache[count].header = strdup(p4);
                            cache[count].payload = strdup(payload);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            int idx = find_packet(id);
            if (idx != -1) {
                free(cache[idx].id);
                free(cache[idx].header);
                free(cache[idx].payload);
                for (int i = idx; i < count - 1; i++) {
                    cache[i] = cache[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            int idx = find_packet(id);
            if (idx != -1) {
                printf("%s\n", cache[idx].header);
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            int idx = find_packet(id);
            if (idx != -1) {
                printf("%s\n", cache[idx].payload);
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total_payload = 0;
            for (int i = 0; i < count; i++) {
                total_payload += cache[i].p_len;
            }
            printf("%lld\n", total_payload);
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
