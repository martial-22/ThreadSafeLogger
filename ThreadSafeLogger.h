#include <string>
#include <fstream>

#include "BlockingQueue.h"

namespace std
{

class Logger
{
public:
    explicit Logger(const filesystem::path& file)
	    : ofs_(file)
    {
	    auto write = [this]()
	    {
		    while (auto msg = queue_.pop())
		    {
			    ofs_ << *msg;
		    }
	    };
	    
	    writer_ = thread(move(write));
    }

    ~Logger()
    {
	    queue_.shutdown();
	    writer_.join();
    }

    bool log(string message)
    {
	    return queue_.push(move(message));
    }

private:
	ofstream ofs_;
	BlockingQueue<string> queue_;
	thread writer_;
};

}