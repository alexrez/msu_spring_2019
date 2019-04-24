// Два потока по очереди выводят в консоль сообщение. Первый выводит ping, второй выводит pong.
// Количество ping и pong - по 500 000 каждый.

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

std::mutex mutex;
std::condition_variable condition;
std::atomic<bool> order {true};

void pingPong(const size_t stop, const bool threadOrder, const std::string &str){
    size_t i = 0;
    while(i < stop){
        ++i;
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (order != threadOrder)
                condition.wait(lock);
            std::cout << str;
            order.store(!threadOrder);
        }
        condition.notify_one();
    }
}


int main(){
    const size_t stop = 500000;
    const bool first = true, second = false;

    std::thread ping(pingPong, stop, first, "ping\n");
    std::thread pong(pingPong, stop, second, "pong\n");

    ping.join();
    pong.join();

    return 0;
}