// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
} PacketCache;

int main(void) {
    PacketCache *cache = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *hlen_s = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                hlen_s++;
            }
            while (*hlen_s == ' ') hlen_s++;
            char *hlen_end = hlen_s;
            while (*hlen_end && *hlen_end != ' ') hlen_end++;
            char *plen_s = hlen_end;
            if (*hlen_end != '\0') {
                *hlen_end = '\0';
                plen_s++;
            }
            while (*plen_s == ' ') plen_s++;
            char *plen_end = plen_s;
            while (*plen_end && *plen_end != ' ') plen_end++;
            char *hdr = plen_end;
            if (*plen_end != '\0') {
                *plen_end = '\0';
                hdr++;
            }
            while (*hdr == ' ') hdr++;
            char *hdr_end = hdr;
            while (*hdr_end && *hdr_end != ' ') hdr_end++;
            char *pay = hdr_end;
            if (*hdr_end != '\0') {
                *hdr_end = '\0';
                pay++;
            }
            while (*pay == ' ') pay++;
            char *pay_end = pay;
            while (*pay_end && *pay_end != ' ') pay_end++;
            *pay_end = '\0';
            int hlen = atoi(hlen_s);
            int plen = atoi(plen_s);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                PacketCache *new_c = realloc(cache, capacity * sizeof(PacketCache));
                if (new_c) cache = new_c;
            }
            cache[count].id = mystrdup(id);
            cache[count].header_len = hlen;
            cache[count].payload_len = plen;
            cache[count].header = mystrdup(hdr);
            cache[count].payload = mystrdup(pay);
            count++;
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
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
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total_payload = 0;
            for (size_t i = 0; i < count; i++) {
                total_payload += cache[i].payload_len;
            }
            printf("%lld\n", total_payload);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
