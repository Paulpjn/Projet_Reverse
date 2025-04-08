#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 256

void afficher_menu() {
    printf("=== Liste des commandes ===\n");
    printf("LED1 ON / LED1 OFF\n");
    printf("LED2 ON / LED2 OFF\n");
    printf("LED3 ON / LED3 OFF\n");
    printf("CHENILLARD1 ON / OFF\n");
    printf("CHENILLARD2 ON / OFF\n");
    printf("CHENILLARD3 ON / OFF\n");
    printf("CHENILLARD FREQUENCE<1|2|3>\n");
    printf("help / h : Affiche ce menu\n");
    printf("clear / c : Nettoie le terminal\n");
    printf("quit / q : Quitte le programme\n");
    printf("===========================\n");
}

int setup_serial_port(const char *port_name) {
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("Unable to open serial port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    options.c_iflag = IGNBRK;
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

int write_serial_port(int fd, const char *data) {
    return write(fd, data, strlen(data));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <serial_port>\n", argv[0]);
        return 1;
    }

    int serial_fd = setup_serial_port(argv[1]);
    if (serial_fd == -1) return 1;

    struct pollfd fds;
    fds.fd = serial_fd;
    fds.events = POLLIN;

    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    int bytes_read;

    afficher_menu();

    while (1) {
        // Vérifier s’il y a une réponse côté STM32
        int ret = poll(&fds, 1, 100); // 100ms timeout
        if (ret > 0 && (fds.revents & POLLIN)) {
            if (ioctl(serial_fd, FIONREAD, &bytes_read) != -1) {
                if (bytes_read > 0) {
                    read(serial_fd, buffer, bytes_read);
                    buffer[bytes_read] = '\0';
                    printf("[STM32] %s\n", buffer);
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        }


        printf("> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0; 

        // Traitement local
        if (strcasecmp(input, "quit") == 0 || strcasecmp(input, "q") == 0) {
            printf("Fermeture du programme.\n");
            break;
        } else if (strcasecmp(input, "help") == 0 || strcasecmp(input, "h") == 0) {
            afficher_menu();
        } else if (strcasecmp(input, "clear") == 0 || strcasecmp(input, "c") == 0) {
            system("clear");
        } else if (strlen(input) == 0) {
            continue;
        } else {
            strcat(input, "\n"); 
            if (write_serial_port(serial_fd, input) < 0) {
                perror("Erreur d'envoi série");
            }
        }
    }

    close(serial_fd);
    return 0;
}
