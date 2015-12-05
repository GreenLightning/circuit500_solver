#ifndef STUPID_QUEUE
#define STUPID_QUEUE

#include <cstddef>

/* An array-based queue of type T, that allows a maximum number of N pushes.
 * 
 * N specifies the maximum number of total pushes performed on the queue,
 * not the maximum number of elements it can contain at a given time.
 * Corrupts memory if push is called more than N times.
 * 
 * Does not perform any checks. Use with care.
 */

template <class T, std::size_t N>
class Stupid_Queue {

public:
	Stupid_Queue() : first(0), last(0) {}
	
	void push(T value) {
		values[last++] = value;
	}
	
	T pop() {
		return values[first++];
	}

	bool empty() {
		return first == last;
	}

private:
	T values[N];
	std::size_t first;
	std::size_t last;
};

#endif