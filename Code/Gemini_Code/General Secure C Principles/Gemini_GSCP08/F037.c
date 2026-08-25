// F037.c
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
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

int main(void) {
    ByteBuffer *arr = NULL;
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
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %zu\n", arr[i].name, arr[i].size);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *hexbytes = p2 + 1;
                size_t hlen = strlen(hexbytes);
                size_t blen = hlen / 2;
                unsigned char *bytes = malloc(blen == 0 ? 1 : blen);
                if (bytes) {
                    for (size_t i = 0; i < blen; i++) {
                        char hex[3] = { hexbytes[2*i], hexbytes[2*i+1], '\0' };
                        bytes[i] = (unsigned char)strtol(hex, NULL, 16);
                    }
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        ByteBuffer *narr = realloc(arr, ncap * sizeof(ByteBuffer));
                        if (narr) { arr = narr; cap = ncap; }
                    }
                    if (count < cap) {
                        arr[count].name = strdup(name);
                        arr[count].bytes = bytes;
                        arr[count].size = blen;
                        count++;
                    } else {
                        free(bytes);
                    }
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *hexbytes = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        size_t hlen = strlen(hexbytes);
                        size_t blen = hlen / 2;
                        if (blen > 0) {
                            unsigned char *nbytes = realloc(arr[i].bytes, arr[i].size + blen);
                            if (nbytes) {
                                arr[i].bytes = nbytes;
                                for (size_t j = 0; j < blen; j++) {
                                    char hex[3] = { hexbytes[2*j], hexbytes[2*j+1], '\0' };
                                    arr[i].bytes[arr[i].size + j] = (unsigned char)strtol(hex, NULL, 16);
                                }
                                arr[i].size += blen;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *off_s = p2 + 1;
                char *p3 = strchr(off_s, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *len_s = p3 + 1;
                    long offset = strtol(off_s, NULL, 10);
                    long length = strtol(len_s, NULL, 10);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (offset >= 0 && length >= 0 && (size_t)offset + (size_t)length <= arr[i].size) {
                                for (size_t j = 0; j < (size_t)length; j++) {
                                    printf("%02X", arr[i].bytes[(size_t)offset + j]);
                                }
                                printf("\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].bytes);
    }
    free(arr);
    return 0;
}
