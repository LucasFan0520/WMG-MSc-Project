// F037.c
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
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuf;

int main() {
    ByteBuf *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            size_t hl = strlen(hex);
            while (hl > 0 && (hex[hl-1] == '\r' || hex[hl-1] == '\n')) {
                hex[hl-1] = '\0';
                hl--;
            }
            int b_cnt = hl / 2;
            unsigned char *b_arr = malloc(b_cnt > 0 ? b_cnt : 1);
            if (b_arr) {
                for (int i = 0; i < b_cnt; i++) {
                    char tmp[3] = {hex[i*2], hex[i*2+1], '\0'};
                    b_arr[i] = (unsigned char)strtol(tmp, NULL, 16);
                }
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].bytes);
                        arr[i].bytes = b_arr;
                        arr[i].size = b_cnt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        ByteBuf *narr = realloc(arr, cap * sizeof(ByteBuf));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].name = mystrdup(name);
                        arr[count].bytes = b_arr;
                        arr[count].size = b_cnt;
                        count++;
                    } else {
                        free(b_arr);
                    }
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            size_t hl = strlen(hex);
            while (hl > 0 && (hex[hl-1] == '\r' || hex[hl-1] == '\n')) {
                hex[hl-1] = '\0';
                hl--;
            }
            int b_cnt = hl / 2;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    unsigned char *nb = realloc(arr[i].bytes, arr[i].size + b_cnt);
                    if (nb) {
                        arr[i].bytes = nb;
                        for (int j = 0; j < b_cnt; j++) {
                            char tmp[3] = {hex[j*2], hex[j*2+1], '\0'};
                            arr[i].bytes[arr[i].size + j] = (unsigned char)strtol(tmp, NULL, 16);
                        }
                        arr[i].size += b_cnt;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *off_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *len_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int offset = atoi(off_s);
            int length = atoi(len_s);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (offset >= 0 && offset < arr[i].size) {
                        int end = offset + length;
                        if (end > arr[i].size) end = arr[i].size;
                        for (int j = offset; j < end; j++) {
                            printf("%02x", arr[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
