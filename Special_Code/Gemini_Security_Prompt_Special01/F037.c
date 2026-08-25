// F037.c
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
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

unsigned char hex_to_byte(char h, char l) {
    unsigned char b = 0;
    if (h >= '0' && h <= '9') b += (h - '0') * 16;
    else if (h >= 'a' && h <= 'f') b += (h - 'a' + 10) * 16;
    else if (h >= 'A' && h <= 'F') b += (h - 'A' + 10) * 16;
    if (l >= '0' && l <= '9') b += (l - '0');
    else if (l >= 'a' && l <= 'f') b += (l - 'a' + 10);
    else if (l >= 'A' && l <= 'F') b += (l - 'A' + 10);
    return b;
}

int main(void) {
    ByteBuffer *buffers = NULL;
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
        if (strcmp(cmd, "NEW") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *hex = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                hex++;
            }
            while (*hex == ' ') hex++;
            char *hex_end = hex;
            while (*hex_end && *hex_end != ' ') hex_end++;
            *hex_end = '\0';
            size_t hex_len = strlen(hex);
            size_t bytes_len = hex_len / 2;
            unsigned char *b_buf = malloc(bytes_len > 0 ? bytes_len : 1);
            if (b_buf) {
                for (size_t i = 0; i < bytes_len; i++) {
                    b_buf[i] = hex_to_byte(hex[i * 2], hex[i * 2 + 1]);
                }
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ByteBuffer *new_b = realloc(buffers, capacity * sizeof(ByteBuffer));
                    if (new_b) buffers = new_b;
                }
                buffers[count].name = mystrdup(name);
                buffers[count].bytes = b_buf;
                buffers[count].size = bytes_len;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *hex = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                hex++;
            }
            while (*hex == ' ') hex++;
            char *hex_end = hex;
            while (*hex_end && *hex_end != ' ') hex_end++;
            *hex_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    size_t hex_len = strlen(hex);
                    size_t add_len = hex_len / 2;
                    if (add_len > 0) {
                        unsigned char *new_bytes = realloc(buffers[i].bytes, buffers[i].size + add_len);
                        if (new_bytes) {
                            buffers[i].bytes = new_bytes;
                            for (size_t j = 0; j < add_len; j++) {
                                buffers[i].bytes[buffers[i].size + j] = hex_to_byte(hex[j * 2], hex[j * 2 + 1]);
                            }
                            buffers[i].size += add_len;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *off_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                off_s++;
            }
            while (*off_s == ' ') off_s++;
            char *off_end = off_s;
            while (*off_end && *off_end != ' ') off_end++;
            char *len_s = off_end;
            if (*off_end != '\0') {
                *off_end = '\0';
                len_s++;
            }
            while (*len_s == ' ') len_s++;
            char *len_end = len_s;
            while (*len_end && *len_end != ' ') len_end++;
            *len_end = '\0';
            int offset = atoi(off_s);
            int length = atoi(len_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < buffers[i].size && length > 0) {
                        size_t lim = (size_t)offset + (size_t)length;
                        if (lim > buffers[i].size) lim = buffers[i].size;
                        for (size_t j = (size_t)offset; j < lim; j++) {
                            printf("%02x", buffers[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", buffers[i].name, buffers[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
