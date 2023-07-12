#include "remotereader.hpp"

void RemoteReader::Run(const char *host, const char *port, const char *target, int version)
{
    printf("RUN\n");
    req_.version(version);
    req_.method(boost::beast::http::verb::get);
    req_.target(target);
    req_.set(boost::beast::http::field::host, host);
    req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    host_ = host;
    port_ = port;
    target_ = target;
    DoResolve();
}

void RemoteReader::DoResolve()
{
    printf("RESOLVING\n");
    resolver_.async_resolve(
        host_,port_,[this](const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::iterator epiter)
        {
            if(!ec)
            {
                printf("RESOLVED!\n");
                for(; epiter != boost::asio::ip::tcp::resolver::iterator(); epiter++)
                    endpoints_.push_back(*epiter);
                curEp_ = endpoints_.begin();
                DoConnect();
            }
        }
    );
}

void RemoteReader::DoConnect()
{
    stream_.expires_after(std::chrono::seconds(3));
    stream_.async_connect
    (
        *curEp_,
        [this](const boost::beast::error_code& ec)
        {
            if(!ec)
            {
                printf("CONNECTED!\n");
                DoWrite();
            }
        }
    );
}

void RemoteReader::DoWrite()
{
    boost::beast::http::async_write
    (
        stream_, req_, 
        [this](const boost::beast::error_code& ec, std::size_t b)
        {
            if(!ec)
            {
                printf("WROTE %lu\n",b);
                DoRead();
            }
        }
    );
}

void RemoteReader::DoRead()
{
    boost::beast::http::async_read
    (
        stream_, buffer_, res_,
        [this](const boost::beast::error_code& ec, std::size_t b)
        {
            if(!ec)
            {
                printf("READ %lu\n",b);
                const char *data = static_cast<const char*>(buffer_.data().data());
                if(content_!= res_.body())
                    content_ = std::move(res_.body());
                buffer_.clear();
                res_.clear();
                req_.clear();
                host_.clear();
                port_.clear();
                target_.clear();
                stream_.cancel();
                stream_.close();
            }
        }
    );
}
