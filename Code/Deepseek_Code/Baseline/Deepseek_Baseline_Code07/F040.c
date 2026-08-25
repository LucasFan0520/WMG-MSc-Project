// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;
Packet *tail = NULL;

Packet *find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = malloc(strlen(id)+1);
    strcpy(p->id, id);
    p->header = malloc(strlen(header)+1);
    strcpy(p->header, header);
    p->payload = malloc(strlen(payload)+1);
    strcpy(p->payload, payload);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL;
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void stats_total(void) {
    int total = 0;
    Packet *p = head;
    while (p) {
        total += strlen(p->payload);
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = head;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(rest, " ");
            if (!id) continue;
            char *hlen_str = strtok(NULL, " ");
            if (!hlen_str) continue;
            char *plen_str = strtok(NULL, " ");
            if (!plen_str) continue;
            int hlen = atoi(hlen_str);
            int plen = atoi(plen_str);
            char *header = strtok(NULL, " ");
            if (!header) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(rest, " ");
            if (id) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(rest, " ");
            if (id) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(rest, " ");
            if (id) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_total();
        }
    }
    free_all();
    return 0;
}