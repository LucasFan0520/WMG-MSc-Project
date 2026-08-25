// F040.c
#define _GNU_SOURCE
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

int main(void) {
    Packet *cache = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *args = line + 7;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *id = args;
                char *rest1 = sp1 + 1;
                char *sp2 = strchr(rest1, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int h_len = atoi(rest1);
                    char *rest2 = sp2 + 1;
                    char *sp3 = strchr(rest2, ' ');
                    if (sp3) {
                        *sp3 = '\0';
                        int p_len = atoi(rest2);
                        char *rest3 = sp3 + 1;
                        char *sp4 = strchr(rest3, ' ');
                        if (sp4) {
                            *sp4 = '\0';
                            char *header = rest3;
                            char *payload = sp4 + 1;
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Packet *next = realloc(cache, capacity * sizeof(Packet));
                                if (!next) break;
                                cache = next;
                            }
                            cache[count].id = strdup(id);
                            cache[count].h_len = h_len;
                            cache[count].p_len = p_len;
                            cache[count].header = strdup(header);
                            cache[count].payload = strdup(payload);
                            count++;
                        }
                    }
                }
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
            long long total_payload = 0;
            for (size_t i = 0; i < count; i++) {
                total_payload += cache[i].p_len;
            }
            printf("%lld\n", total_payload);
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
