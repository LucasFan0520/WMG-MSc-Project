/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t len;
} Buffer;

static Buffer *buffers = NULL;
static int bcount = 0;
static int bcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_buffer(const char *name) {
    for (int i = 0; i < bcount; i++)
        if (strcmp(buffers[i].name, name) == 0) return i;
    return -1;
}

static int parse_hex(const char *hex, unsigned char **out, size_t *out_len) {
    size_t hlen = strlen(hex);
    if (hlen % 2 != 0) return -1;
    size_t blen = hlen / 2;
    unsigned char *buf = malloc(blen);
    if (!buf) return -1;
    for (size_t i = 0; i < blen; i++) {
        char byte_str[3] = {hex[2*i], hex[2*i+1], '\0'};
        char *end;
        long val = strtol(byte_str, &end, 16);
        if (*end != '\0' || val < 0 || val > 255) {
            free(buf);
            return -1;
        }
        buf[i] = (unsigned char)val;
    }
    *out = buf;
    *out_len = blen;
    return 0;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *hex = line + offset + off2;
            while (*hex == ' ') hex++;
            unsigned char *bytes = NULL;
            size_t blen = 0;
            if (parse_hex(hex, &bytes, &blen) != 0) continue;
            if (bcount >= bcap) {
                bcap = bcap == 0 ? 8 : bcap * 2;
                buffers = realloc(buffers, sizeof(Buffer) * bcap);
            }
            buffers[bcount].name = dup_str(name);
            buffers[bcount].bytes = bytes;
            buffers[bcount].len = blen;
            bcount++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *hex = line + offset + off2;
            while (*hex == ' ') hex++;
            unsigned char *bytes = NULL;
            size_t blen = 0;
            if (parse_hex(hex, &bytes, &blen) != 0) continue;
            int idx = find_buffer(name);
            if (idx < 0) { free(bytes); continue; }
            buffers[idx].bytes = realloc(buffers[idx].bytes, buffers[idx].len + blen);
            if (!buffers[idx].bytes) { free(bytes); continue; }
            memcpy(buffers[idx].bytes + buffers[idx].len, bytes, blen);
            buffers[idx].len += blen;
            free(bytes);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256] = {0};
            int off_val = 0, len_val = 0;
            if (sscanf(line + offset, "%255s %d %d", name, &off_val, &len_val) != 3) continue;
            int idx = find_buffer(name);
            if (idx < 0 || off_val < 0 || len_val < 0 || (size_t)(off_val + len_val) > buffers[idx].len) continue;
            for (int i = 0; i < len_val; i++) {
                printf("%02X", buffers[idx].bytes[off_val + i]);
            }
            printf("\n");
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_buffer(name);
            if (idx < 0) continue;
            free(buffers[idx].name);
            free(buffers[idx].bytes);
            for (int i = idx; i < bcount - 1; i++) buffers[i] = buffers[i + 1];
            bcount--;
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) {
                printf("%s %zu\n", buffers[i].name, buffers[i].len);
            }
        }
    }
    for (int i = 0; i < bcount; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}