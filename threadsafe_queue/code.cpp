#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <memory>
#include <mutex>

template <typename T>
class threadsafe_queue
{
public:
	threadsafe_queue() {}
	threadsafe_queue& operator=(const threadsafe_queue&) = delete;
	threadsafe_queue(const threadsafe_queue& right)
	{
		std::lock_guard<std::mutex> lg(right.mutex_);
		queue_ = right.queue_;
	}

	void push(T value)
	{
		{
			std::lock_guard<std::mutex> lg(mutex_);
			queue_.push(value);
		}

		cv_.notify_one();
	}

	bool try_pop(T& out)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (queue_.empty()) return false;

		out = queue_.front();
		queue_.pop();
		return true;
	}
	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (queue_.empty()) return std::shared_ptr<T>();

		auto res = std::make_shared<T>(queue_.front());
		queue_.pop();
		return res;
	}

	void wait_and_pop(T& out)
	{
		std::unique_lock<std::mutex> ul(mutex_);
		cv_.wait(ul, [this]() { return !queue_.empty(); });
		out = queue_.front();
		queue_.pop();
	}
	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> ul(mutex_);
		cv_.wait(ul, [this]() { return !queue_.empty(); });
		auto res = std::make_shared<T>(queue_.front());
		queue_.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lg;
		return queue_.empty();
	}

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<T> queue_;
};

int sum_;
threadsafe_queue<int> myqueue_;

void data_provider()
{
	std::cout << "The producer thread is starting...\n";
	for (int i = 0; i < 20; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "Push a new data\n";
		myqueue_.push(1);
	}

	std::cout << "The producer thread is stopping...\n";
}

void data_processor()
{
	std::cout << "The processor thread is starting...\n";

	for (int i = 0; i < 20; ++i)
	{
		int out;
		std::cout << "Waiting for a new data\n";
		myqueue_.wait_and_pop(out);
		std::cout << "A new data received\n";
		sum_ += out;
	}

	std::cout << "The processor thread is stopping...\n";
}


using namespace std;
int main()
{
	cout << "The main thread is running...\n";

	sum_ = 0;

	thread t0(&data_provider);
	thread t1(&data_processor);


	t0.join();
	t1.join();

	cout << "Threads joined. Resulting sum: " << sum_ << endl;
}
