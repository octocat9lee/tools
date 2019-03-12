//#include "ping.h"
//#include "Log4.h"
//
//#include <thread>
//#include <chrono>
//#include <boost/xpressive/xpressive_dynamic.hpp>
//
//bool check_ip(const char* ip)
//{
//    boost::xpressive::cregex reg_ip = boost::xpressive::cregex::compile("(25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])");
//    return boost::xpressive::regex_match(ip, reg_ip);
//}
//
//void pinger::receive_with_timeout(boost::posix_time::seconds sec)
//{
//    // Discard any data already in the buffer.
//    boost::system::error_code ec;
//
//    socket_.async_receive(boost::asio::buffer(buffer_),
//        strand_.wrap(boost::bind(&pinger::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
//
//    // Wait up to five seconds for a reply.
//    timer_.expires_from_now(sec, ec);
//    if(ec)
//    {
//        LOG4_ERROR("init deadline timer failed");
//        return;
//    }
//
//    timer_.async_wait(strand_.wrap(boost::bind(&pinger::handle_timeout, this, boost::asio::placeholders::error)));
//}
//
//int pinger::start_send(const time_t outtime)
//{
//    std::string body("Hello!");
//
//    // Create an ICMP header for an echo request.
//    icmp_header echo_request;
//    echo_request.type(icmp_header::echo_request);
//    echo_request.code(0);
//    echo_request.identifier(get_identifier());
//    echo_request.sequence_number(++sequence_number_);
//    compute_checksum(echo_request, body.begin(), body.end());
//
//    // Encode the request packet.
//    boost::asio::streambuf request_buffer;
//    std::ostream os(&request_buffer);
//    os << echo_request << body;
//
//    boost::system::error_code ec;
//
//    // Send the request.
//    socket_.send_to(request_buffer.data(), destination_, 0, ec);
//    if(ec)
//    {
//        return -1;
//    }
//
//    strand_.post(boost::bind(&pinger::receive_with_timeout, this, boost::posix_time::seconds(3)));
//
//    return 0;
//}
//
//pinger::pinger(boost::asio::io_service &io_service)
//    : resolver_(io_service), socket_(io_service, icmp::v4()),
//    timer_(io_service), sequence_number_(0), strand_(io_service)
//{
//}
//
//pinger::~pinger()
//{
//    boost::system::error_code err;
//    socket_.cancel(err);
//    timer_.cancel(err);
//}
//
//bool pinger::ping(const char *destination, const time_t outtime)
//{
//    if(!check_ip(destination))
//    {
//        LOG4_ERROR("pinger, invalid destination: " << destination);
//        return false;
//    }
//
//    boost::system::error_code ec;
//    icmp::resolver::query query(icmp::v4(), destination, "");
//
//    destination_ = *resolver_.resolve(query, ec);
//    if(ec)
//    {
//        LOG4_ERROR("pinger, invalid destination: " << destination);
//        return false;
//    }
//
//    socket_.cancel(ec);
//    timer_.cancel(ec);
//
//    time_sent_ = time(NULL);
//    sequence_number_ = 0;
//    replies_ = false;
//    recv_timeout_ = false;
//
//    // only ping once
//    start_send(outtime);
//    do
//    {
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    } while(!replies_ && !recv_timeout_ && (time(NULL) - time_sent_ < 5));
//
//    return replies_;
//}
//
//void pinger::handle_read(const boost::system::error_code& ec, std::size_t bytes)
//{
//    if(ec)
//    {
//        LOG4_ERROR("pinger handler read error: " << ec.message());
//        return;
//    }
//    replies_ = true;
//    boost::system::error_code error;
//    timer_.cancel(error);
//}
//
//void pinger::handle_timeout(boost::system::error_code ec)
//{
//    if(!ec)
//    {
//        socket_.cancel(ec);
//    }
//    recv_timeout_ = true;
//}
