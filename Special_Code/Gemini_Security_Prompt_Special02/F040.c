// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    int payload_len;
} PacketCache;

int main() {
    PacketCache *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *id = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            while (*p == ' ') p++;
            char *hlen_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *plen_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *hdr = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *pay = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int hlen = atoi(hlen_s);
            int plen = atoi(plen_s);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(PacketCache));
            }
            items[count].id = mystrdup(id);
            char *h_buf = malloc(hlen + 1);
            strncpy(h_buf, hdr, hlen);
            h_buf[hlen] = '\0';
            items[count].header = h_buf;
            char *p_buf = malloc(plen + 1);
            strncpy(p_buf, pay, plen);
            p_buf[plen] = '\0';
            items[count].payload = p_buf;
            items[count].payload_len = plen;
            count++;
        } else if (strcmp(cmd, "DROP") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].header);
                    free(items[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (size_t i = 0; i < count; i++) {
                total += items[i].payload_len;
            }
            printf("%d\n", total);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].header);
        free(items[i].payload);
    }
    free(items);
    return 0;
}
