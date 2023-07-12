#if !defined(REMOTEREADER_HPP)
#define REMOTEREADER_HPP


#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>

struct RemoteReader
{
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::empty_body> req_;
    boost::beast::http::response<boost::beast::http::string_body> res_;
    std::string host_;
    std::string port_;
    std::string target_;
    std::vector<boost::asio::ip::tcp::endpoint> endpoints_;
    std::vector<boost::asio::ip::tcp::endpoint>::iterator curEp_;
    std::string content_;

    RemoteReader(boost::asio::io_context& ioc)
    : resolver_(ioc), stream_(ioc){}
    void Run(const char *host, const char *port, const char *target, int version);
    void DoResolve();
    void DoConnect();
    void DoWrite();
    void DoRead();
};


#endif // REMOTEREADER_HPP
