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

// Liste des commandes liées au microcontrôleur
const char *valid_commands[] = {
    "LED1 ON", "LED1 OFF",
    "LED2 ON", "LED2 OFF",
    "LED3 ON", "LED3 OFF",
    "CHENILLARD1 ON", "CHENILLARD1 OFF",
    "CHENILLARD2 ON", "CHENILLARD2 OFF",
    "CHENILLARD3 ON", "CHENILLARD3 OFF",
    "FREQUENCE1", "FREQUENCE2", "FREQUENCE3"
};
#define NUM_VALID_COMMANDS (sizeof(valid_commands) / sizeof(valid_commands[0]))

// Fonction pour afficher le menu d'aide
void afficher_menu() {
    printf("=== Liste des commandes ===\n");
    printf("LED1 ON / LED1 OFF\n");
    printf("LED2 ON / LED2 OFF\n");
    printf("LED3 ON / LED3 OFF\n");
    printf("CHENILLARD1 ON / OFF\n");
    printf("CHENILLARD2 ON / OFF\n");
    printf("CHENILLARD3 ON / OFF\n");
    printf("FREQUENCE<1|2|3>\n");
    printf("help / h : Affiche ce menu\n");
    printf("clear / c : Nettoie le terminal\n");
    printf("quit / q : Quitte le programme\n");
    printf("===========================\n");
}

// Fonction pour configurer le port série
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



// Fonction pour vérifier si la commande est valide (seulement les commandes du microcontrôleur)
int is_microcontroller_command_valid(const char *input) {
    for (int i = 0; i < NUM_VALID_COMMANDS; i++) {
        if (strcmp(input, valid_commands[i]) == 0) {
            return 1;  // Commande valide
        }
    }
    return 0;  // Commande invalide
}

// Fonction principale
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

    afficher_menu(); // Afficher le menu d'aide initial

    while (1) {
        // Vérifier s’il y a une réponse côté STM32
        int ret = poll(&fds, 1, 100); // 100ms timeout
        if (ret > 0 && (fds.revents & POLLIN)) {
            if (ioctl(serial_fd, FIONREAD, &bytes_read) != -1) {
                if (bytes_read > 0) {
                    read(serial_fd, buffer, bytes_read);
                    buffer[bytes_read] = '\0';
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        }

        // Récupérer l'entrée de l'utilisateur
        printf("> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Enlever le '\n' et '\r' à la fin
        input[strcspn(input, "\n")] = 0;
        input[strcspn(input, "\r")] = 0;
        input[strlen(input)] = '\0';

        // Vérifier si l'utilisateur veut quitter
        if (strcasecmp(input, "quit") == 0 || strcasecmp(input, "q") == 0) {
            printf("Fermeture du programme.\n");
            break;
        }
        // Afficher le menu d'aide
        else if (strcasecmp(input, "help") == 0 || strcasecmp(input, "h") == 0) {
            afficher_menu();
        }
        // Nettoyer le terminal
        else if (strcasecmp(input, "clear") == 0 || strcasecmp(input, "c") == 0) {
            system("clear");
        }
        // Vérifier la validité de la commande du microcontrôleur
        else if (is_microcontroller_command_valid(input)) {
            if (write(serial_fd, input, strlen(input)+1) < 0) {
                perror("Erreur d'envoi série");
            }
        } else {
            printf("Commande invalide.\n");
        }
    }

    close(serial_fd);
    return 0;
}
