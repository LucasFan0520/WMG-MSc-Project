// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    unsigned char *data;
    int size;
} Buffer;

Buffer *buffers = NULL;
int num_buffers = 0;

int find_buffer(const char *name) {
    for (int i = 0; i < num_buffers; i++) {
        if (strcmp(buffers[i].name, name) == 0) return i;
    }
    return -1;
}

int parse_hex(const char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    int bytes = len / 2;
    *out = malloc(bytes);
    for (int i = 0; i < bytes; i++) {
        char byte_str[3] = {hex[2*i], hex[2*i+1], 0};
        char *end;
        long val = strtol(byte_str, &end, 16);
        if (*end != 0 || !isxdigit(hex[2*i]) || !isxdigit(hex[2*i+1])) {
            free(*out);
            return 0;
        }
        (*out)[i] = (unsigned char)val;
    }
    return bytes;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[4096];
            sscanf(line, "%*s %s %s", name, hex);
            unsigned char *data;
            int bytes = parse_hex(hex, &data);
            if (bytes > 0) {
                buffers = realloc(buffers, (num_buffers + 1) * sizeof(Buffer));
                buffers[num_buffers].name = strdup(name);
                buffers[num_buffers].data = data;
                buffers[num_buffers].size = bytes;
                num_buffers++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[4096];
            sscanf(line, "%*s %s %s", name, hex);
            int idx = find_buffer(name);
            if (idx >= 0) {
                unsigned char *data;
                int bytes = parse_hex(hex, &data);
                if (bytes > 0) {
                    buffers[idx].data = realloc(buffers[idx].data, buffers[idx].size + bytes);
                    memcpy(buffers[idx].data + buffers[idx].size, data, bytes);
                    buffers[idx].size += bytes;
                    free(data);
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            sscanf(line, "%*s %s %d %d", name, &offset, &length);
            int idx = find_buffer(name);
            if (idx >= 0 && offset >= 0 && length >= 0 && offset + length <= buffers[idx].size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", buffers[idx].data[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_buffer(name);
            if (idx >= 0) {
                free(buffers[idx].name);
                free(buffers[idx].data);
                for (int i = idx; i < num_buffers - 1; i++) {
                    buffers[i] = buffers[i + 1];
                }
                num_buffers--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_buffers; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    
    for (int i = 0; i < num_buffers; i++) {
        free(buffers[i].name);
        free(buffers[i].data);
    }
    free(buffers);
    
    return 0;
}