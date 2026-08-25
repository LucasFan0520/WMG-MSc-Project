// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

int hex_to_bytes(const char *hex, unsigned char **res) {
    int len = strlen(hex);
    int count = len / 2;
    *res = malloc(count);
    if (count > 0 && !*res) return 0;
    for (int i = 0; i < count; i++) {
        unsigned int b;
        sscanf(hex + 2 * i, "%2x", &b);
        (*res)[i] = (unsigned char)b;
    }
    return count;
}

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "NEW") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *hex = "";
            if (space2) {
                *space2 = '\0';
                hex = space2 + 1;
                while (*hex == ' ') hex++;
            }
            unsigned char *res = NULL;
            int b_size = hex_to_bytes(hex, &res);
            buffers = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
            if (!buffers) return 1;
            buffers[count].name = strdup(name);
            buffers[count].bytes = res;
            buffers[count].size = b_size;
            count++;
        } else if (strcmp(cmd, "APPEND") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *hex = "";
            if (space2) {
                *space2 = '\0';
                hex = space2 + 1;
                while (*hex == ' ') hex++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    unsigned char *res = NULL;
                    int b_size = hex_to_bytes(hex, &res);
                    if (b_size > 0) {
                        buffers[i].bytes = realloc(buffers[i].bytes, buffers[i].size + b_size);
                        if (!buffers[i].bytes) return 1;
                        memcpy(buffers[i].bytes + buffers[i].size, res, b_size);
                        buffers[i].size += b_size;
                    }
                    free(res);
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int offset = atoi(space2 + 1);
                char *space3 = strchr(space2 + 1, ' ');
                if (space3) {
                    int slen = atoi(space3 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(buffers[i].name, name) == 0) {
                            if (offset >= 0 && offset < buffers[i].size && slen > 0 && offset + slen <= buffers[i].size) {
                                for (int j = 0; j < slen; j++) {
                                    printf("%02x", buffers[i].bytes[offset + j]);
                                }
                                putchar('\n');
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, args) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (int j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    buffers = realloc(buffers, count * sizeof(ByteBuffer));
                    if (count > 0 && !buffers) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
