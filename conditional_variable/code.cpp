#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <memory>
#include <mutex>

std::mutex mutex_;
std::queue<int> data_;
std::condition_variable cv_;
int sum_;

void data_provider()
{
	std::cout << "The producer thread is starting...\n";
	for (int i = 0; i < 20; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "Push a new data\n";
		std::lock_guard<std::mutex> lk(mutex_);
		data_.push(1);
		cv_.notify_one();
	}

	std::cout << "The producer thread is stopping...\n";
}

void data_processor()
{
	std::cout << "The processor thread is starting...\n";

	for (int i = 0; i < 20; ++i)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		std::cout << "Waiting for a new data\n";
		cv_.wait(lk, []() { return !data_.empty(); });
		std::cout << "A new data received\n";
		sum_ += data_.front();
		data_.pop();
		lk.unlock();
	}

	std::cout << "The processor thread is stopping...\n";
}

using namespace std;
int main()
{
	sum_ = 0;

	thread t0(&data_provider);
	thread t1(&data_processor);


	t0.join();
	t1.join();

	cout << "Threads joined. Resulting sum: " << sum_ << endl;
}
