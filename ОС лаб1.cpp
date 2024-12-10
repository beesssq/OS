#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    while (true) {
        sleep(1);
        pthread_mutex_lock(&lock);

        if (ready == 1) {
            pthread_mutex_unlock(&lock);
            continue;
        }

        ready = 1;
        std::cout << "Поставка! Сообщение отправлено!\n";

        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

void* consumer(void* arg) {
    while (true) {
        pthread_mutex_lock(&lock);

        while (ready == 0) {
            pthread_cond_wait(&cond1, &lock);
            std::cout << "Потребитель получил сообщение!\n";
        }

        ready = 0;
        std::cout << "Потребитель также обработал сообщение!\n";

        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

int main() {
    pthread_t producerThread, consumerThread;

    pthread_create(&producerThread, nullptr, producer, nullptr);
    pthread_create(&consumerThread, nullptr, consumer, nullptr);

    pthread_join(producerThread, nullptr);
    pthread_join(consumerThread, nullptr);

    return 0;
}
