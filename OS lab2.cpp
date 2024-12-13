#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>

constexpr int PORT = 12345;
constexpr int BUFFER_SIZE = 1024;

volatile sig_atomic_t signal_received = 0;

void signal_handler(int signum) {
    signal_received = signum;
}

int main() {
    // Установка обработчика сигнала
    struct sigaction sa {};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }

    // Создание TCP-сокета
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        return 1;
    }

    // Установка параметров сокета
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_socket);
        return 1;
    }

    // Привязка сокета к адресу
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        return 1;
    }

    // Перевод сокета в режим прослушивания
    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("listen");
        close(server_socket);
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // Главный цикл обработки
    int client_socket = -1;
    fd_set readfds;
    sigset_t mask, orig_mask;

    // Блокировка сигналов во время pselect()
    sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) == -1) {
        perror("sigprocmask");
        close(server_socket);
        return 1;
    }

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        if (client_socket != -1) {
            FD_SET(client_socket, &readfds);
        }

        int nfds = std::max(server_socket, client_socket) + 1;

        int ready = pselect(nfds, &readfds, nullptr, nullptr, nullptr, &orig_mask);
        if (ready == -1) {
            if (errno == EINTR) {
                if (signal_received == SIGHUP) {
                    std::cout << "Received SIGHUP signal" << std::endl;
                    signal_received = 0;
                }
                continue;
            }
            perror("pselect");
            break;
        }

        // Обработка нового подключения
        if (FD_ISSET(server_socket, &readfds)) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int new_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
            if (new_socket == -1) {
                perror("accept");
                continue;
            }

            std::cout << "New connection accepted" << std::endl;

            if (client_socket == -1) {
                client_socket = new_socket;
            }
            else {
                std::cout << "Closing additional connection" << std::endl;
                close(new_socket);
            }
        }

        // Обработка данных от клиента
        if (client_socket != -1 && FD_ISSET(client_socket, &readfds)) {
            char buffer[BUFFER_SIZE];
            ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received > 0) {
                std::cout << "Received " << bytes_received << " bytes" << std::endl;
            }
            else {
                if (bytes_received == 0) {
                    std::cout << "Client disconnected" << std::endl;
                }
                else {
                    perror("recv");
                }
                close(client_socket);
                client_socket = -1;
            }
        }
    }

    close(server_socket);
    if (client_socket != -1) {
        close(client_socket);
    }

    return 0;
}
