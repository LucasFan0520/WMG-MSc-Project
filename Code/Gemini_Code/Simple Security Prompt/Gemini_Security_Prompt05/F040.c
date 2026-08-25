// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
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
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = get_token(&p);
            char *hlen_s = get_token(&p);
            char *plen_s = get_token(&p);
            char *hdr = get_token(&p);
            char *pay = get_token(&p);
            if (id && hlen_s && plen_s && hdr && pay) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Packet *narr = realloc(arr, cap * sizeof(Packet));
                    if (narr) arr = narr;
                }
                arr[count].id = strdup(id);
                arr[count].hlen = atoi(hlen_s);
                arr[count].plen = atoi(plen_s);
                arr[count].header = strdup(hdr);
                arr[count].payload = strdup(pay);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = get_token(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        free(arr[i].id);
                        free(arr[i].header);
                        free(arr[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = get_token(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        printf("%s\n", arr[i].header);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = get_token(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        printf("%s\n", arr[i].payload);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total_bytes = 0;
            for (int i = 0; i < count; i++) {
                total_bytes += arr[i].plen;
            }
            printf("%lld\n", total_bytes);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].header);
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
