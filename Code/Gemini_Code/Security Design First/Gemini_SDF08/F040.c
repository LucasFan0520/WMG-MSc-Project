// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    long p_len;
} Packet;

int main(void) {
    Packet *cache = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *id = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    long h_len = strtol(p2, NULL, 10);
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        long p_len = strtol(p3, NULL, 10);
                        char *p4 = s3 + 1;
                        char *s4 = strchr(p4, ' ');
                        if (s4) {
                            *s4 = '\0';
                            char *header = p4;
                            char *payload = s4 + 1;
                            if ((long)strlen(header) == h_len && (long)strlen(payload) == p_len) {
                                Packet *tmp = realloc(cache, sizeof(Packet) * (count + 1));
                                if (tmp) {
                                    cache = tmp;
                                    cache[count].id = strdup(id);
                                    cache[count].header = strdup(header);
                                    cache[count].payload = strdup(payload);
                                    cache[count].p_len = p_len;
                                    if (cache[count].id && cache[count].header && cache[count].payload) {
                                        count++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    idx = i;
                    break;
                }
            }
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
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].p_len;
            }
            printf("%lld\n", total);
        }
    }
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    free(line);
    return 0;
}
