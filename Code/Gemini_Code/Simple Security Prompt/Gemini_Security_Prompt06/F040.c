// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} PacketCache;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    PacketCache *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "STATS") == 0) {
                long total = 0;
                for (int i = 0; i < count; i++) {
                    total += list[i].p_len;
                }
                printf("%ld\n", total);
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "PACKET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *id = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int h_len = atoi(rest2);
                    char *rest3 = p3 + 1;
                    char *p4 = strchr(rest3, ' ');
                    if (p4) {
                        *p4 = '\0';
                        int p_len = atoi(rest3);
                        char *rest4 = p4 + 1;
                        char *p5 = strchr(rest4, ' ');
                        if (p5) {
                            *p5 = '\0';
                            char *header = rest4;
                            char *payload = p5 + 1;
                            if ((int)strlen(header) >= h_len && (int)strlen(payload) >= p_len) {
                                list = realloc(list, (count + 1) * sizeof(PacketCache));
                                list[count].id = strdup(id);
                                list[count].h_len = h_len;
                                list[count].p_len = p_len;
                                list[count].header = malloc(h_len + 1);
                                strncpy(list[count].header, header, h_len);
                                list[count].header[h_len] = '\0';
                                list[count].payload = malloc(p_len + 1);
                                strncpy(list[count].payload, payload, p_len);
                                list[count].payload[p_len] = '\0';
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].id);
                free(list[found].header);
                free(list[found].payload);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(PacketCache));
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    printf("%s\n", list[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    printf("%s\n", list[i].payload);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].header);
        free(list[i].payload);
    }
    free(list);
    return 0;
}