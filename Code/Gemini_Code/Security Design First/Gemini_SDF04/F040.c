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
} Packet;

Packet *cache = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *id = space1 + 1;
            char *space2 = strchr(id, ' ');
            if (strcmp(cmd, "PACKET") == 0) {
                if (space2) {
                    *space2 = '\0';
                    char *hl_str = space2 + 1;
                    char *space3 = strchr(hl_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *pl_str = space3 + 1;
                        char *space4 = strchr(pl_str, ' ');
                        if (space4) {
                            *space4 = '\0';
                            char *hdr = space4 + 1;
                            char *space5 = strchr(hdr, ' ');
                            if (space5) {
                                *space5 = '\0';
                                char *pay = space5 + 1;
                                int hl = atoi(hl_str);
                                int pl = atoi(pl_str);
                                if (hl >= 0 && pl >= 0 && strlen(hdr) == (size_t)hl && strlen(pay) == (size_t)pl) {
                                    if (count >= capacity) {
                                        capacity = capacity == 0 ? 4 : capacity * 2;
                                        Packet *tmp = realloc(cache, capacity * sizeof(Packet));
                                        if (!tmp) {
                                            free(line);
                                            return 1;
                                        }
                                        cache = tmp;
                                    }
                                    cache[count].id = strdup(id);
                                    cache[count].header = strdup(hdr);
                                    cache[count].payload = strdup(pay);
                                    cache[count].payload_len = pl;
                                    if (cache[count].id && cache[count].header && cache[count].payload) {
                                        count++;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DROP") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(cache[i].id, id) == 0) {
                        free(cache[i].id);
                        free(cache[i].header);
                        free(cache[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            cache[j] = cache[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "HEADER") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].header);
                        break;
                    }
                }
            } else if (strcmp(cmd, "PAYLOAD") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cache[i].id, id) == 0) {
                        printf("%s\n", cache[i].payload);
                        break;
                    }
                }
            }
        } else {
            if (strcmp(cmd, "STATS") == 0) {
                int total = 0;
                for (int i = 0; i < count; i++) {
                    total += cache[i].payload_len;
                }
                printf("%d\n", total);
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
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