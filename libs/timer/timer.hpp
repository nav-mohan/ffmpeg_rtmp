#if !defined(TIMER_HPP)
#define TIMER_HPP

#include <chrono>
#include <boost/asio.hpp>

struct Timer
{
    boost::asio::io_context& ioContext_; // for timer or even http(s) request
    boost::asio::high_resolution_timer refreshTimer_;
    int refreshDurSec_ = -1;

    Timer(boost::asio::io_context& ioc, int refreshDurSec) : 
    ioContext_(ioc), refreshTimer_(ioc), refreshDurSec_(refreshDurSec)
    {
    }

    void StartTimer();
    std::function<void()> OnTimeout;


};

#endif // TIMER_HPP
