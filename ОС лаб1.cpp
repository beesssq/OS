#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;  //Условная переменная, которая используется для синхронизации потоков. 
//Поток-потребитель ожидает сигнала от потока-поставщика, чтобы начать обработку события.

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //Мьютекс(блокировка), обеспечивающий доступ к разделяемой переменной ready между потоками.
int ready = 0;//Флаг, обозначающий готовность события
//0 означает, что события нет.
//1 означает, что событие создано.

void* producer(void* arg) {
    while (true) {
        sleep(1);   //Задержка на 1 секунду, чтобы симулировать время создания события.
        pthread_mutex_lock(&lock);   //Захватывает мьютекс для безопасного изменения флага ready.

        if (ready == 1) {                  //Проверяет, не установлено ли уже событие (if (ready == 1)):
                                            //Если событие уже готово, поток освобождает мьютекс и продолжает цикл.
            pthread_mutex_unlock(&lock);
            continue;
        }

        ready = 1;          //Устанавливает ready = 1 (событие готово).
        std::cout << "Поставка! Сообщение отправлено!\n";

        pthread_cond_signal(&cond1);           //Уведомляет поток-потребитель об изменении состояния с помощью
        pthread_mutex_unlock(&lock);            //Освобождает мьютекс
    }
    return nullptr;
}

void* consumer(void* arg) {
    while (true) {
        pthread_mutex_lock(&lock);           //Захватывает мьютекс, чтобы безопасно читать значение ready.

        while (ready == 0) {
            pthread_cond_wait(&cond1, &lock);          //Ожидает, пока ready станет равным 1. Эта функция временно освобождает мьютекс и блокирует поток до получения сигнала.
            std::cout << "Потребитель получил сообщение!\n";
        }

        ready = 0;            //Устанавливает ready = 0, чтобы указать, что событие обработано
        std::cout << "Потребитель также обработал сообщение!\n";

        pthread_mutex_unlock(&lock);         //Освобождает мьютекс
    }
    return nullptr;
}

int main() {
    pthread_t producerThread, consumerThread;

    pthread_create(&producerThread, nullptr, producer, nullptr); Поток - поставщик
    pthread_create(&consumerThread, nullptr, consumer, nullptr); Поток - потребител

    pthread_join(producerThread, nullptr);         //блокирует выполнение основной программы до завершения обоих потоков
    pthread_join(consumerThread, nullptr);

    return 0;
}