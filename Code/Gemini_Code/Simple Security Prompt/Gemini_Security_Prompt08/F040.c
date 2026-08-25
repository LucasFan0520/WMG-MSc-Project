// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
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

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Packet;

int main(void) {
    Packet *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hlen_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *plen_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *header = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *payload = p;
            int h_len = atoi(hlen_str);
            int p_len = atoi(plen_str);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Packet *nb = realloc(list, cap * sizeof(Packet));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].id = strdup(id);
                list[count].h_len = h_len;
                list[count].p_len = p_len;
                list[count].header = strdup(header);
                list[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, arg1) == 0) {
                    free(list[i].id);
                    free(list[i].header);
                    free(list[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, arg1) == 0) {
                    printf("%s\n", list[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, arg1) == 0) {
                    printf("%s\n", list[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total_payload = 0;
            for (size_t i = 0; i < count; i++) {
                total_payload += list[i].p_len;
            }
            printf("%lld\n", total_payload);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].header);
        free(list[i].payload);
    }
    free(list);
    return 0;
}
