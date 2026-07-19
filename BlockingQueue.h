#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <concepts>

template<typename T>
class BlockingQueue
{
public:
	template <typename V>
	requires std::constructible_from<T, V&&>
	bool push(V&& value)
	{
		{
			std::lock_guard lock(m_);
			if (stop_)
			{
				return false;
			}
			data_.emplace(std::forward<V>(value));
		}
		cv_.notify_one();
		return true;
	}

	std::optional<T> pop()
	{
		std::unique_lock lock(m_);
		cv_.wait(lock, [this]() {
			return stop_ || !data_.empty();
		});
		
		if (data_.empty())
		{
			return std::nullopt;
		}
		
		T item = std::move(data_.front());
		data_.pop();
		return { std::move(item) };
	}
	
	void shutdown()
	{
		{
			std::lock_guard lock(m_);
			stop_ = true;
		}
		cv_.notify_all();
	}
    
private:
	std::mutex m_;
	std::condition_variable cv_;
	std::queue<T> data_;
	bool stop_ = false;
};
