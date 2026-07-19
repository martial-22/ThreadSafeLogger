#include <string>
#include <fstream>
#include <filesystem>

#include "BlockingQueue.h"

class Logger
{
public:
	explicit Logger(const std::filesystem::path& file)
		: ofs_(file)
	{
		if (!ofs_)
		{
    throw std::runtime_error("Cannot open log file");
		}
		
		auto write = [this]() {
			while (auto msg = queue_.pop())
			{
				ofs_ << *msg << '\n';
			}
		};
		writer_ = std::thread(std::move(write));
	}
	
	~Logger()
	{
		queue_.shutdown();
		writer_.join();
	}
	
	bool log(std::string message)
	{
		return queue_.push(std::move(message));
	}

private:
	std::ofstream ofs_;
	BlockingQueue<std::string> queue_;
	std::thread writer_;
};
