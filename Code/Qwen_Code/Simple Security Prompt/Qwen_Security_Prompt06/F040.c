// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Pkt {
    char *id;
    char *header;
    char *payload;
    int hlen;
    int plen;
    struct Pkt *next;
} Pkt;

Pkt *pkts = NULL;

void add_pkt(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if ((int)strlen(header) < hlen || (int)strlen(payload) < plen) return;
    Pkt *p = malloc(sizeof(Pkt));
    p->id = strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = malloc(hlen + 1);
    strncpy(p->header, header, hlen);
    p->header[hlen] = '\0';
    p->payload = malloc(plen + 1);
    strncpy(p->payload, payload, plen);
    p->payload[plen] = '\0';
    p->next = pkts;
    pkts = p;
}

void drop_pkt(const char *id) {
    Pkt *prev = NULL, *curr = pkts;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else pkts = curr->next;
            free(curr->id);
            free(curr->header);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_header(const char *id) {
    for (Pkt *p = pkts; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->header);
            return;
        }
    }
}

void print_payload(const char *id) {
    for (Pkt *p = pkts; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->payload);
            return;
        }
    }
}

void print_stats() {
    int total = 0;
    for (Pkt *p = pkts; p; p = p->next) {
        total += p->plen;
    }
    printf("%d\n", total);
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, id[256] = {0};
        int hlen = 0, plen = 0;
        sscanf(line, "%15s %255s %d %d", cmd, id, &hlen, &plen);
        
        if (strcmp(cmd, "PACKET") == 0) {
            char *header = get_rest(line, 4);
            if (header) {
                char *payload = get_rest(header, 1);
                if (payload) add_pkt(id, hlen, plen, header, payload);
            }
        }
        else if (strcmp(cmd, "DROP") == 0) drop_pkt(id);
        else if (strcmp(cmd, "HEADER") == 0) print_header(id);
        else if (strcmp(cmd, "PAYLOAD") == 0) print_payload(id);
        else if (strcmp(cmd, "STATS") == 0) print_stats();
        
        free(line);
    }
    return 0;
}