// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0) {
            char name[256];
            int name_len = 0;
            if (sscanf(ptr, "%255s%n", name, &name_len) > 0) {
                char *hex = ptr + name_len;
                while (*hex == ' ') hex++;
                int hex_len = strlen(hex);
                int byte_count = hex_len / 2;
                unsigned char *new_bytes = malloc(byte_count);
                for (int i = 0; i < byte_count; i++) {
                    unsigned int bval = 0;
                    sscanf(hex + 2 * i, "%2x", &bval);
                    new_bytes[i] = (unsigned char)bval;
                }
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "NEW") == 0) {
                    if (found != -1) {
                        free(buffers[found].bytes);
                        buffers[found].bytes = new_bytes;
                        buffers[found].size = byte_count;
                    } else {
                        buffers = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
                        buffers[count].name = strdup(name);
                        buffers[count].bytes = new_bytes;
                        buffers[count].size = byte_count;
                        count++;
                    }
                } else {
                    if (found != -1) {
                        int old_size = buffers[found].size;
                        buffers[found].bytes = realloc(buffers[found].bytes, old_size + byte_count);
                        memcpy(buffers[found].bytes + old_size, new_bytes, byte_count);
                        buffers[found].size += byte_count;
                        free(new_bytes);
                    } else {
                        free(new_bytes);
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset = 0, length = 0;
            if (sscanf(ptr, "%255s %d %d", name, &offset, &length) == 3) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    if (offset >= 0 && length >= 0 && offset + length <= buffers[found].size) {
                        for (int j = 0; j < length; j++) {
                            printf("%02x", buffers[found].bytes[offset + j]);
                        }
                        printf("\n");
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(buffers[found].name);
                    free(buffers[found].bytes);
                    for (int i = found; i < count - 1; i++) {
                        buffers[i] = buffers[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        buffers = realloc(buffers, count * sizeof(ByteBuffer));
                    } else {
                        free(buffers);
                        buffers = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
