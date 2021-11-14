#include <iostream>
#include <string>
#include <future>


using namespace std;

int main()
{
	auto two_nums_sum = [](int a, int b) -> int {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		cout << "worker thread id: " << this_thread::get_id() << endl;
		return a + b;
	};

	packaged_task<int(int, int)> task(two_nums_sum);
	auto f = task.get_future();
	thread worker(std::move(task), 7, 9);
	cout << "a work is finished. sum: " << f.get() << endl;
	cout << "main thread id: " << this_thread::get_id() << endl;
	worker.join();
}