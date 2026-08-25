// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    long payload_len;
} PacketRecord;

int main(void) {
    PacketRecord *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "STATS") == 0) {
                long total = 0;
                for (size_t i = 0; i < count; i++) {
                    total += arr[i].payload_len;
                }
                printf("%ld\n", total);
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = args;
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *hl_s = p2 + 1;
                char *p3 = strchr(hl_s, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *pl_s = p3 + 1;
                    char *p4 = strchr(pl_s, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *hdr = p4 + 1;
                        char *p5 = strchr(hdr, ' ');
                        if (p5) {
                            *p5 = '\0';
                            char *pay = p5 + 1;
                            long hl = strtol(hl_s, NULL, 10);
                            long pl = strtol(pl_s, NULL, 10);
                            if (hl >= 0 && pl >= 0 && (size_t)hl == strlen(hdr) && (size_t)pl == strlen(pay)) {
                                if (count >= cap) {
                                    size_t ncap = cap == 0 ? 4 : cap * 2;
                                    PacketRecord *narr = realloc(arr, ncap * sizeof(PacketRecord));
                                    if (narr) { arr = narr; cap = ncap; }
                                }
                                if (count < cap) {
                                    arr[count].id = strdup(id);
                                    arr[count].header = strdup(hdr);
                                    arr[count].payload = strdup(pay);
                                    arr[count].payload_len = pl;
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].header);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].payload);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].header);
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
