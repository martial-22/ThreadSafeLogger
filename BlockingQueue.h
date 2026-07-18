#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace std
{

template<typename T>
class BlockingQueue
{
public:
	bool push(T value)
	{
		{
			lock_guard<mutex> lock(m_);
			if (stop_)
			{
				return false;
			}
			data_.push(move(value));
		}
		cv_.notify_one();
		return true;
	}

	optional<T> pop()
	{
		unique_lock<mutex> lock(m_);
		cv_.wait(lock, [&]() {
			return stop_ || !data_.empty();
		});
		
		if (!data_.empty())
		{
			T item = move(data_.front());
			data_.pop();
			return item;
		}
		return nullopt;
	}
	
	void shutdown()
	{
		{
			lock_guard<mutex> lock(m_);
			stop_ = true;
		}
		cv_.notify_all();
	}
    
private:
	mutex m_;
	condition_variable cv_;
	queue<T> data_;
	bool stop_ = false;
};

}