#include <iostream>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

class Barrier {
public:
    Barrier(int size, std::function<void()> callback) {
        _size = size;
        _current = size;
        _callback = callback;
    }
   void Push() {
       std::unique_lock<std::mutex> lock(_mutex);
       _current--;

       if (_current == 0) {
           _callback();
           _current = _size;
           _cv.notify_all();
       }
       else {
           _cv.wait(lock);
       }
    }

private:
    int _size;
    int _current;

    std::function<void()> _callback;
    std::mutex _mutex;
    std::condition_variable _cv;
};

int main()
{
	const int size = 3;
	int time[size] = { 4000, 2000, 7000 };

	Barrier barrier(size, [] {
		std::cout << "barrier passed!" << std::endl;
		});

	std::thread threadArr[size];
	for (int i = 0; i < size; i++) {
		threadArr[i] = std::thread([ms = time[i], &barrier]() mutable {
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		std::cout << std::this_thread::get_id() << " passed time" << std::endl;
		barrier.Push();
			});
	}
    for (int i = 0; i < size; i++) {
        threadArr[i].join();
    }
}
