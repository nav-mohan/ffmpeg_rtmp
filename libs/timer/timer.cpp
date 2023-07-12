#include "timer.hpp"

void Timer::StartTimer()
{
    refreshTimer_.expires_after(std::chrono::seconds(refreshDurSec_));
    refreshTimer_.async_wait
    (
        [this](const boost::system::error_code ec)
        {
            if(!ec){
                OnTimeout();
                StartTimer();
            }
        }
    );
}