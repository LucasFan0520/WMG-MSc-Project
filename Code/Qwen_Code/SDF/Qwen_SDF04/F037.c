// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; unsigned char* data; size_t size, cap; } Buffer;

Buffer* buffers = NULL;
size_t b_count = 0, b_cap = 0;

void cleanup() {
    for (size_t i = 0; i < b_count; i++) { free(buffers[i].name); free(buffers[i].data); }
    free(buffers);
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    return -1;
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "NEW") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* hex = ptr;
            size_t hlen = strlen(hex);
            if (hlen % 2 != 0) continue;
            size_t bytes = hlen / 2;
            
            unsigned char* data = malloc(bytes);
            if (!data) { cleanup(); free(line); return 1; }
            int valid = 1;
            for (size_t i = 0; i < bytes; i++) {
                int h = hex_val(hex[2*i]);
                int l = hex_val(hex[2*i+1]);
                if (h < 0 || l < 0) { valid = 0; break; }
                data[i] = (h << 4) | l;
            }
            if (!valid) { free(data); continue; }

            if (b_count == b_cap) {
                size_t new_cap = b_cap == 0 ? 4 : b_cap * 2;
                Buffer* temp = realloc(buffers, new_cap * sizeof(Buffer));
                if (!temp) { free(data); cleanup(); free(line); return 1; }
                buffers = temp; b_cap = new_cap;
            }
            buffers[b_count].name = strdup(name);
            buffers[b_count].data = data;
            buffers[b_count].size = bytes;
            buffers[b_count].cap = bytes;
            if (!buffers[b_count].name) { free(data); cleanup(); free(line); return 1; }
            b_count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* hex = ptr;
            size_t hlen = strlen(hex);
            if (hlen % 2 != 0) continue;
            size_t bytes = hlen / 2;

            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    size_t needed = buffers[i].size + bytes;
                    if (needed > buffers[i].cap) {
                        size_t new_cap = needed * 2;
                        unsigned char* temp = realloc(buffers[i].data, new_cap);
                        if (!temp) { cleanup(); free(line); return 1; }
                        buffers[i].data = temp;
                        buffers[i].cap = new_cap;
                    }
                    int valid = 1;
                    for (size_t j = 0; j < bytes; j++) {
                        int h = hex_val(hex[2*j]);
                        int l = hex_val(hex[2*j+1]);
                        if (h < 0 || l < 0) { valid = 0; break; }
                        buffers[i].data[buffers[i].size + j] = (h << 4) | l;
                    }
                    if (valid) buffers[i].size += bytes;
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* off_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* len_str = ptr;

            long off = strtol(off_str, NULL, 10);
            long slen = strtol(len_str, NULL, 10);

            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (off < 0 || slen < 0 || (size_t)(off + slen) > buffers[i].size) break;
                    for (long j = 0; j < slen; j++) {
                        printf("%02x", buffers[i].data[off + j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(buffers[i].name, ptr) == 0) {
                    free(buffers[i].name); free(buffers[i].data);
                    for (size_t j = i; j < b_count - 1; j++) buffers[j] = buffers[j + 1];
                    b_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                printf("%s %zu\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}