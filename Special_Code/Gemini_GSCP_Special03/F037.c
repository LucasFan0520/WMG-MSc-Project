// F037.c
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
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

unsigned char *parse_hex(const char *hex, size_t *out_size) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    unsigned char *res = malloc(count == 0 ? 1 : count);
    if (!res) return NULL;
    for (size_t i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + i * 2, "%2x", &val);
        res[i] = (unsigned char)val;
    }
    *out_size = count;
    return res;
}

int main(void) {
    ByteBuffer *items = NULL;
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
        if (strcmp(cmd, "NEW") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *hex = p;
            size_t hlen = strlen(hex);
            while (hlen > 0 && (hex[hlen-1] == '\n' || hex[hlen-1] == '\n')) {
                hex[hlen-1] = '\0';
                hlen--;
            }
            size_t b_size = 0;
            unsigned char *b_data = parse_hex(hex, &b_size);
            if (b_data) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    ByteBuffer *nitems = realloc(items, cap * sizeof(ByteBuffer));
                    if (nitems) items = nitems;
                }
                items[count].name = mystrdup(name);
                items[count].bytes = b_data;
                items[count].size = b_size;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *hex = p;
            size_t hlen = strlen(hex);
            while (hlen > 0 && (hex[hlen-1] == '\n' || hex[hlen-1] == '\n')) {
                hex[hlen-1] = '\0';
                hlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    size_t add_size = 0;
                    unsigned char *add_data = parse_hex(hex, &add_size);
                    if (add_data) {
                        unsigned char *nb = realloc(items[i].bytes, items[i].size + add_size);
                        if (nb) {
                            items[i].bytes = nb;
                            memcpy(items[i].bytes + items[i].size, add_data, add_size);
                            items[i].size += add_size;
                        }
                        free(add_data);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *off_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *len_s = p;
            size_t lnlen = strlen(len_s);
            while (lnlen > 0 && (len_s[lnlen-1] == '\n' || len_s[lnlen-1] == '\n')) {
                len_s[lnlen-1] = '\0';
                lnlen--;
            }
            int offset = atoi(off_s);
            int length = atoi(len_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < items[i].size && length > 0) {
                        size_t end = (size_t)offset + (size_t)length;
                        if (end > items[i].size) end = items[i].size;
                        for (size_t j = (size_t)offset; j < end; j++) {
                            printf("%02x", items[i].bytes[j]);
                        }
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", items[i].name, items[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].bytes);
    }
    free(items);
    return 0;
}
