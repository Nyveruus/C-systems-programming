#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <regex.h>

#define PORT 8080
#define IP "127.0.0.1"
#define BACKLOG 10
#define BUFFER_SIZE 8192

static volatile sig_atomic_t keep_running = 1;

int create(int socket_fd, struct sockaddr_in *server);
void *handler(void *arg);
char *get_filename (char *file);
char *get_extension(char *url_file);
void build_http(char *file, char *extension, char *response, size_t *response_len);
char *get_mime(char *extension);

void signal_handler(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    struct sigaction act = {
        .sa_handler = signal_handler,
        .sa_flags = 0,
    };
    sigaction(SIGINT, &act, NULL);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        return 1;
    }
    struct sockaddr_in server;
    if (create(socket_fd, &server) == 1) {
        close(socket_fd);
        return 1;
    }
    while (keep_running) {
        struct sockaddr_in client;
        socklen_t clientlen = sizeof(client);
        int *client_fd = malloc(sizeof(int));
        if (!client_fd) {
            perror("malloc");
            break;
        }
        *client_fd = accept(socket_fd, (struct sockaddr *)&client, &clientlen);
        if (*client_fd < 0) {
            perror("accept");
            free(client_fd);
            if (errno == EINTR) break;
            continue;
        }
        pthread_t tid;
        if (pthread_create(&tid, NULL, handler, (void *)client_fd) != 0) {
            perror("pthread_create");
            close(*client_fd);
            free(client_fd);
            continue;
        }
        pthread_detach(tid);
    }
    close(socket_fd);
    return 0;
}

int create(int socket_fd, struct sockaddr_in *server) {
    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = inet_addr(IP);
    socklen_t addrlen = sizeof(struct sockaddr);
    if (bind(socket_fd, (struct sockaddr *)server, addrlen) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(socket_fd, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }
    printf("Listening on port %d\n", PORT);
    return 0;
}

void *handler(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);

    char *buffer = malloc(BUFFER_SIZE);
    ssize_t received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (received > 0) {
        regex_t comp;
        //capture 0: the full match 2: group (filename)
        regmatch_t matches[2];
        regcomp(&comp, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        if (regexec(&comp, buffer, 2, matches, 0) == 0) {
            buffer[matches[1].rm_eo] = '\0';
            char *url_file = buffer + matches[1].rm_so;
            char *file = get_filename(url_file);

            char extension[32];
            strcpy(extension, get_extension(url_file));

            //buld response
            char *response = malloc(BUFFER_SIZE * 2);
            size_t response_len;
            build_http(file, extension, response, &response_len);
            //send to client
            send(client_fd, response, response_len, 0);
            //cleanup
            free(response);
            free(file);
        }
        regfree(&comp);
    }

    free(buffer);
    close(client_fd);
    return NULL;
}

char *get_filename (char *file) {
    size_t len = strlen(file);
    char *result = malloc(len + 1);
    size_t result_len = 0;

    for (size_t i = 0; i < len; i++) {
        if (file[i] == '%' && i + 2 < len) {
            int hex;
            sscanf(file + i + 1, "%2x", &hex);
            result[result_len++] = hex;
            i += 2;
        } else {
            result[result_len++] = file[i];
        }
    }
    result[result_len] = '\0';
    return result;
}

char *get_extension(char *url_file) {
    char *point = strrchr(url_file, '.');
    if (!point || point == url_file) {
        return "";
    }
    return point + 1;
}

void build_http(char *file, char *extension, char *response, size_t *response_len) {
    char *mime_type = get_mime(extension);
    char *header = malloc(BUFFER_SIZE);

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        snprintf(response, BUFFER_SIZE, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        *response_len = strlen(response);
        return;
    }
    snprintf(header, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", mime_type);

    *response_len = 0;

    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    ssize_t red;
    while((red = read(fd, response + *response_len, BUFFER_SIZE - *response_len)) > 0)
        *response_len += red;

    free(header);
    close(fd);
}

char *get_mime(char *extension) {
    if (strcasecmp(extension, "html") == 0)
        return "text/html";
    else if (strcasecmp(extension, "txt") == 0)
        return "text/plain";
    else if (strcasecmp(extension, "jpg") == 0 || strcasecmp(extension, "jpeg") == 0)
        return "image/jpeg";
    else if (strcasecmp(extension, "png") == 0)
        return "image/png";
    else
        return "application/octet-stream";
}
