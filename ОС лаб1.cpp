#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;  //�������� ����������, ������� ������������ ��� ������������� �������. 
//�����-����������� ������� ������� �� ������-����������, ����� ������ ��������� �������.

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //�������(����������), �������������� ������ � ����������� ���������� ready ����� ��������.
int ready = 0;//����, ������������ ���������� �������
//0 ��������, ��� ������� ���.
//1 ��������, ��� ������� �������.

void* producer(void* arg) {
    while (true) {
        sleep(1);   //�������� �� 1 �������, ����� ������������ ����� �������� �������.
        pthread_mutex_lock(&lock);   //����������� ������� ��� ����������� ��������� ����� ready.

        if (ready == 1) {                  //���������, �� ����������� �� ��� ������� (if (ready == 1)):
                                            //���� ������� ��� ������, ����� ����������� ������� � ���������� ����.
            pthread_mutex_unlock(&lock);
            continue;
        }

        ready = 1;          //������������� ready = 1 (������� ������).
        std::cout << "��������! ��������� ����������!\n";

        pthread_cond_signal(&cond1);           //���������� �����-����������� �� ��������� ��������� � �������
        pthread_mutex_unlock(&lock);            //����������� �������
    }
    return nullptr;
}

void* consumer(void* arg) {
    while (true) {
        pthread_mutex_lock(&lock);           //����������� �������, ����� ��������� ������ �������� ready.

        while (ready == 0) {
            pthread_cond_wait(&cond1, &lock);          //�������, ���� ready ������ ������ 1. ��� ������� �������� ����������� ������� � ��������� ����� �� ��������� �������.
            std::cout << "����������� ������� ���������!\n";
        }

        ready = 0;            //������������� ready = 0, ����� �������, ��� ������� ����������
        std::cout << "����������� ����� ��������� ���������!\n";

        pthread_mutex_unlock(&lock);         //����������� �������
    }
    return nullptr;
}

int main() {
    pthread_t producerThread, consumerThread;

    pthread_create(&producerThread, nullptr, producer, nullptr); ����� - ���������
    pthread_create(&consumerThread, nullptr, consumer, nullptr); ����� - ����������

    pthread_join(producerThread, nullptr);         //��������� ���������� �������� ��������� �� ���������� ����� �������
    pthread_join(consumerThread, nullptr);

    return 0;
}