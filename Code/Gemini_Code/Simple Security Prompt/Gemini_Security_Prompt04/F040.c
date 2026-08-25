// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    int p_len;
} Packet;

int main(void) {
    Packet *cache = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long h_len = strtol(p + 1, &endptr, 10);
                long p_len = strtol(endptr, &endptr, 10);
                while (*endptr == ' ') endptr++;
                char *hdr = endptr;
                if ((long)strlen(hdr) >= h_len) {
                    char *pay = hdr + h_len;
                    while (*pay == ' ') pay++;
                    if ((long)strlen(pay) >= p_len) {
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            cache = realloc(cache, cap * sizeof(Packet));
                        }
                        cache[count].id = strdup(id);
                        cache[count].header = malloc(h_len + 1);
                        strncpy(cache[count].header, hdr, h_len);
                        cache[count].header[h_len] = '\0';
                        cache[count].payload = malloc(p_len + 1);
                        strncpy(cache[count].payload, pay, p_len);
                        cache[count].payload[p_len] = '\0';
                        cache[count].p_len = (int)p_len;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(cache[found].id);
                free(cache[found].header);
                free(cache[found].payload);
                for (int i = found; i < count - 1; i++) {
                    cache[i] = cache[i + 1];
                }
                count--;
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
            long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].p_len;
            }
            printf("%ld\n", total);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
