#include <iostream>
#include <string>
#include <future>
#include <exception>


using namespace std;

int main()
{
	cout << "main thread id: " << this_thread::get_id() << endl;

	promise<int> promise_;
	auto future_0 = promise_.get_future();

	thread worker_0([&promise_](int a, int b) {
			cout << "worker_0 is running. thread id: "
				<< this_thread::get_id() << endl;
			this_thread::sleep_for(chrono::seconds(1));
			promise_.set_value(a + b);
		}, 77, 11);

	cout << "worker_0 sum result: " << future_0.get() << endl;

	promise<int> promise_1;
	auto future_1 = promise_1.get_future();
	thread worker_1([&promise_1](int a, int b) {
			cout << "worker_1 is running. thread id: "
				<< this_thread::get_id() << endl;
			this_thread::sleep_for(chrono::seconds(1));
			try
			{
				//this could work this way and in catch block exception will be saved
				//throw std::runtime_error("myerror");
				//promise_1.set_value(a + b);

				// or I could use std::make_exception_ptr() to raise my own exception
				promise_1.set_exception(std::make_exception_ptr(std::runtime_error("MyError")));
			}
			catch (...)
			{
				promise_1.set_exception(std::current_exception());
			}
		}, 3.0, 0.0);

	future_1.wait();

	try {
		// this is won't print cause exception 
		cout << "worker_1 sum result: " << future_1.get() << endl;
	}
	catch (const std::runtime_error& e)
	{
		cout << "worker_1 finished with an exception: " << e.what() << endl;
	}

	std::promise<int> promise_for_two_threads;
	auto sf_0 = promise_for_two_threads.get_future().share();
	auto sf_1 = sf_0;
	std::thread processor_0([sf_0]() {
			cout << "processor_0 is running and waiting. thread id: "
				<< this_thread::get_id() << endl;
			sf_0.wait();
			cout << "processor_0 is get value: " << sf_0.get() << endl;
		});

	std::thread processor_1([sf_1]() {
			cout << "processor_1 is running and waiting. thread id: " 
				<< this_thread::get_id() << endl;

			sf_1.wait();
			cout << "processor_1 is get value: " << sf_1.get() << endl;
		});
	
	std::this_thread::sleep_for(std::chrono::seconds(1));

	promise_for_two_threads.set_value(371);

	worker_0.join();
	worker_1.join();
	processor_0.join();
	processor_1.join();

}