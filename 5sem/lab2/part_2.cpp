#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>

typedef std::chrono::high_resolution_clock Clock;

const int tasks_count = 1024 * 1024 * 4;
std::mutex mtx;
int consumer_count = 4;
int producer_count = 4;
std::atomic<int> global_counter{0};
std::condition_variable queue_conditional;

class Queue {
public:
    virtual void push(uint8_t val) = 0;

    virtual bool pop(uint8_t &val) = 0;
};

class ThreadSafeQueue : public Queue {
private:
    std::queue<uint8_t> queue;
public:
    void push(uint8_t val) override {
        mtx.lock();
        queue.push(val);
        mtx.unlock();
    }

    bool pop(uint8_t &val) override {
        mtx.lock();
        if (queue.empty()) {
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            mtx.lock();
            if (!queue.empty()) {
                val = queue.front();
                queue.pop();
                mtx.unlock();
                return true;
            } else {
                mtx.unlock();
                return false;
            }
        } else {
            val = queue.front();
            queue.pop();
            mtx.unlock();
            return true;
        }
    }
};

class FixedSizeQueue : public Queue {
private:
    std::deque<uint8_t> deque;
    int size;
public:
    explicit FixedSizeQueue(int size) {
        this->size = size;
    }

    void push(uint8_t val) override {
        std::unique_lock<std::mutex> unique_lock(mtx);
        queue_conditional.wait(unique_lock, [=]() { return deque.size() < size; });
        deque.push_back(val);
        unique_lock.unlock();
        queue_conditional.notify_one();
    }

    bool pop(uint8_t &val) override {
        std::unique_lock<std::mutex> unique_lock(mtx);
        if (queue_conditional.wait_for(unique_lock, std::chrono::milliseconds(10), [=]() { return !deque.empty(); })) {
            val = deque.front();
            deque.pop_front();
            unique_lock.unlock();
            queue_conditional.notify_one();
            return true;
        } else return false;
    }
};

void producer(Queue &queue);

void consumer(Queue &queue);

void start_threads(Queue &queue);

void print_if_equal();

int main() {
    srand(time(nullptr));

    producer_count = rand() % 4 + 1;
    consumer_count = rand() % 4 + 1;

    std::cout << "Producers count: " << producer_count << std::endl;
    std::cout << "Consumers count: " << consumer_count << std::endl;

    auto queue = ThreadSafeQueue();
    start_threads(queue);
    auto fixed_queue = FixedSizeQueue(16);
    start_threads(fixed_queue);
    return 0;
}

void start_threads(Queue &queue) {
    global_counter = 0;
    std::vector<std::thread> producer_threads;
    std::vector<std::thread> consumer_threads;

    auto clock_start = Clock::now();
    for (auto i = 0; i < producer_count; ++i) {
        std::thread p_thread(producer, std::ref(queue));
        producer_threads.emplace_back(std::move(p_thread));
    }

    for (int i = 0; i < consumer_count; ++i) {
        std::thread c_thread(consumer, std::ref(queue));
        consumer_threads.emplace_back(std::move(c_thread));
    }

    for (int i = 0; i < consumer_count; ++i) {
        consumer_threads[i].join();
    }

    for (int i = 0; i < producer_count; ++i) {
        producer_threads[i].join();
    }
    auto clock_end = Clock::now();

    auto func_time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();

    std::cout << "Function time: " << func_time << " milliseconds" << std::endl;
    print_if_equal();
}

void consumer(Queue &queue) {
    auto clock_start = Clock::now();
    int counter = 0;
    uint8_t val;
    while (true) {
        if (queue.pop(val)) {
            counter += val;
        } else break;
    }
    global_counter.fetch_add(counter, std::memory_order_seq_cst);
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    mtx.lock();
    std::cout << "\tConsumer ID: " << std::this_thread::get_id() << "; time: " << time << std::endl;
    mtx.unlock();
}

void producer(Queue &queue) {
    auto clock_start = Clock::now();
    for (int i = 0; i < tasks_count; ++i) {
        queue.push(1);
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    mtx.lock();
    std::cout << "\tProducer ID: " << std::this_thread::get_id() << "; time: " << time << std::endl;
    mtx.unlock();
}

void print_if_equal() {
    std::cout << std::endl;
    std::cout << "Global counter: " << global_counter << std::endl;
    std::cout << "Tasks * producers: " << tasks_count * producer_count << std::endl;

    if (global_counter == tasks_count * producer_count) {
        std::cout << "Equal" << std::endl;
    } else {
        std::cout << "Not equal" << std::endl;
    }
}
