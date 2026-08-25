// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

unsigned char hex_to_byte(char h1, char h2) {
    unsigned char b = 0;
    if (h1 >= '0' && h1 <= '9') b += (h1 - '0') * 16;
    else if (h1 >= 'a' && h1 <= 'f') b += (h1 - 'a' + 10) * 16;
    else if (h1 >= 'A' && h1 <= 'F') b += (h1 - 'A' + 10) * 16;
    if (h2 >= '0' && h2 <= '9') b += (h2 - '0');
    else if (h2 >= 'a' && h2 <= 'f') b += (h2 - 'a' + 10);
    else if (h2 >= 'A' && h2 <= 'F') b += (h2 - 'A' + 10);
    return b;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

int main() {
    Buffer *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            char *name = get_token(&p);
            char *hex = get_token(&p);
            if (name && hex) {
                int hlen = strlen(hex);
                int blen = hlen / 2;
                unsigned char *buf = malloc(blen > 0 ? blen : 1);
                for (int i = 0; i < blen; i++) {
                    buf[i] = hex_to_byte(hex[2 * i], hex[2 * i + 1]);
                }
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Buffer *narr = realloc(arr, cap * sizeof(Buffer));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].bytes = buf;
                arr[count].size = blen;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = get_token(&p);
            char *hex = get_token(&p);
            if (name && hex) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        int hlen = strlen(hex);
                        int blen = hlen / 2;
                        if (blen > 0) {
                            unsigned char *nb = realloc(arr[i].bytes, arr[i].size + blen);
                            if (nb) {
                                arr[i].bytes = nb;
                                for (int j = 0; j < blen; j++) {
                                    arr[i].bytes[arr[i].size + j] = hex_to_byte(hex[2 * j], hex[2 * j + 1]);
                                }
                                arr[i].size += blen;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = get_token(&p);
            char *off_s = get_token(&p);
            char *len_s = get_token(&p);
            if (name && off_s && len_s) {
                int offset = atoi(off_s);
                int length = atoi(len_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        if (offset >= 0 && offset + length <= arr[i].size) {
                            for (int j = 0; j < length; j++) {
                                printf("%02x", arr[i].bytes[offset + j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].bytes);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].name, arr[i].size);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].bytes);
    }
    free(arr);
    return 0;
}
