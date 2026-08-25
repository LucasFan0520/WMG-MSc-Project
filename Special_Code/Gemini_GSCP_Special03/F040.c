// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    long long payload_len;
} PacketRecord;

int main(void) {
    PacketRecord *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *hlen_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *plen_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *header = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *payload = p;
            size_t pylen = strlen(payload);
            while (pylen > 0 && (payload[pylen-1] == '\n' || payload[pylen-1] == '\n')) {
                payload[pylen-1] = '\0';
                pylen--;
            }
            long long h_len = atoll(hlen_s);
            long long p_len = atoll(plen_s);
            if (h_len >= 0 && p_len >= 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    PacketRecord *nitems = realloc(items, cap * sizeof(PacketRecord));
                    if (nitems) items = nitems;
                }
                items[count].id = mystrdup(id);
                items[count].header = mystrdup(header);
                items[count].payload = mystrdup(payload);
                items[count].payload_len = p_len;
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = p;
            size_t idlen = strlen(id);
            while (idlen > 0 && (id[idlen-1] == '\n' || id[idlen-1] == '\n')) {
                id[idlen-1] = '\0';
                idlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].header);
                    free(items[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = p;
            size_t idlen = strlen(id);
            while (idlen > 0 && (id[idlen-1] == '\n' || id[idlen-1] == '\n')) {
                id[idlen-1] = '\0';
                idlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = p;
            size_t idlen = strlen(id);
            while (idlen > 0 && (id[idlen-1] == '\n' || id[idlen-1] == '\n')) {
                id[idlen-1] = '\0';
                idlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (size_t i = 0; i < count; i++) {
                total += items[i].payload_len;
            }
            printf("%lld\n", total);
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
