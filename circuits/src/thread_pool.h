#pragma once

#include <functional>
#include <thread>
#include <vector>


class ThreadPool {
private:
	std::vector<std::jthread> threads;

public:
	ThreadPool(int pool_size);

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	ThreadPool(const ThreadPool &&other);
	ThreadPool &operator=(const ThreadPool &&other);

	~ThreadPool();


	template <class Func>
	void dispatch(const std::function<Func> &f) {

	}
};