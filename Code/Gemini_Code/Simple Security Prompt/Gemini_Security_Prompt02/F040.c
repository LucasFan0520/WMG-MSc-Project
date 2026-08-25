// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
} Packet;

int main() {
    Packet *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *id = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int hlen = atoi(p2);
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        int plen = atoi(p3);
                        char *p4 = s3 + 1;
                        char *s4 = strchr(p4, ' ');
                        if (s4) {
                            *s4 = '\0';
                            char *header = p4;
                            char *payload = s4 + 1;
                            if (count >= cap) {
                                cap = cap == 0 ? 4 : cap * 2;
                                arr = realloc(arr, cap * sizeof(Packet));
                            }
                            arr[count].id = strdup(id);
                            arr[count].hlen = hlen;
                            arr[count].plen = plen;
                            arr[count].header = strdup(header);
                            arr[count].payload = strdup(payload);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].header);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total = 0;
            for (size_t i = 0; i < count; i++) {
                total += arr[i].plen;
            }
            printf("%lld\n", total);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].header);
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
