#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

class ProducerConsumer {
private:
    queue<int> buffer;
    pthread_mutex_t mutexLock;
    pthread_cond_t condFull, condEmpty;

public:
    ProducerConsumer() {
        pthread_mutex_init(&mutexLock, nullptr);
        pthread_cond_init(&condFull, nullptr);
        pthread_cond_init(&condEmpty, nullptr);
        logMessage("[Main] Synchronization objects initialized.");
    }

    ~ProducerConsumer() {
        pthread_mutex_destroy(&mutexLock);
        pthread_cond_destroy(&condFull);
        pthread_cond_destroy(&condEmpty);
        logMessage("[Main] Cleanup completed.");
    }

    static void logMessage(const string& message) {
        cout << "[LOG] " << message << endl;
    }

    static void* produce(void* arg) {
        ProducerConsumer* pc = (ProducerConsumer*)arg;
        srand(time(0));
        for (int i = 1; i <= NUM_ITEMS; i++) {
            usleep((rand() % 500 + 200) * 1000);
            pthread_mutex_lock(&pc->mutexLock);
            while (pc->buffer.size() == BUFFER_SIZE) {
                logMessage("[Producer] Buffer is full, waiting...");
                pthread_cond_wait(&pc->condFull, &pc->mutexLock);
            }
            int item = rand() % 100 + 1;
            pc->buffer.push(item);
            logMessage("[Producer] Created item " + to_string(item));
            pthread_cond_signal(&pc->condEmpty);
            pthread_mutex_unlock(&pc->mutexLock);
        }
        return nullptr;
    }

    static void* consume(void* arg) {
        ProducerConsumer* pc = (ProducerConsumer*)arg;
        srand(time(0));
        for (int i = 1; i <= NUM_ITEMS; i++) {
            usleep((rand() % 700 + 300) * 1000);
            pthread_mutex_lock(&pc->mutexLock);
            while (pc->buffer.empty()) {
                logMessage("[Consumer] Buffer is empty, waiting...");
                pthread_cond_wait(&pc->condEmpty, &pc->mutexLock);
            }
            int item = pc->buffer.front();
            pc->buffer.pop();
            logMessage("[Consumer] Processed item " + to_string(item));
            pthread_cond_signal(&pc->condFull);
            pthread_mutex_unlock(&pc->mutexLock);
        }
        return nullptr;
    }

    void startSimulation() {
        pthread_t producerThread, consumerThread;
        logMessage("[Main] Starting Producer-Consumer simulation...");

        pthread_create(&producerThread, nullptr, produce, this);
        pthread_create(&consumerThread, nullptr, consume, this);

        pthread_join(producerThread, nullptr);
        pthread_join(consumerThread, nullptr);

        logMessage("[Main] Producer-Consumer simulation completed successfully!");
    }
};

int main() {
    ProducerConsumer pc;
    pc.startSimulation();
    return 0;
}

