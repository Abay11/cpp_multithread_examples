#include <iostream>
#include <string>
#include <future>


using namespace std;
int main()
{
	future<int> f = async([](int a, int b, int c) {
			cout << "worker thread id: " << this_thread::get_id() << endl;
			cout << "args: " << a << " " << b << " " << c << endl;
			this_thread::sleep_for(chrono::seconds(5));
			return a + b + c;
		}, 1, 2, 3);
	
	cout << "a work is finished. sum: " << f.get() << endl;
	cout << "main thread id: " << this_thread::get_id() << endl;
}
