//
// ping.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp03/icmp
//
#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/chrono.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/time_point.hpp>

#include <istream>
#include <iostream>
#include <ostream>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"

using boost::asio::ip::icmp;
using boost::asio::deadline_timer;

class pinger
{
public:
    pinger(boost::asio::io_service& io_context, const char* destination)
        : resolver_(io_context), socket_(io_context, icmp::v4()),
        timer_(io_context), sequence_number_(0), num_replies_(0)
    {
        //destination_ = *resolver_.resolve(icmp::v4(), destination, "").begin();
        boost::system::error_code ec;
        icmp::resolver::query query(icmp::v4(), destination, "");

        destination_ = *resolver_.resolve(query, ec);

        std::cout << "pinger constructor" << std::endl;
        start_send();
        start_receive();
    }

private:
    void start_send()
    {
        std::cout << "start send" << std::endl;
        std::string body("\"Hello!\" from Asio ping.");

        // Create an ICMP header for an echo request.
        icmp_header echo_request;
        echo_request.type(icmp_header::echo_request);
        echo_request.code(0);
        echo_request.identifier(get_identifier());
        echo_request.sequence_number(++sequence_number_);
        compute_checksum(echo_request, body.begin(), body.end());

        // Encode the request packet.
        boost::asio::streambuf request_buffer;
        std::ostream os(&request_buffer);
        os << echo_request << body;

        // Send the request.
        time_sent_ = boost::chrono::steady_clock::now();
        socket_.send_to(request_buffer.data(), destination_);

        // Wait up to five seconds for a reply.
        num_replies_ = 0;
        boost::system::error_code ec;
        timer_.expires_from_now(boost::posix_time::seconds(5));
        timer_.async_wait(boost::bind(&pinger::handle_timeout, this));
    }

    void handle_timeout()
    {
        if(num_replies_ == 0)
            std::cout << "Request timed out" << std::endl;

        // Requests must be sent no less than one second apart.
        boost::system::error_code ec;
        timer_.expires_from_now(boost::posix_time::seconds(1));
        timer_.async_wait(boost::bind(&pinger::start_send, this));
    }

    void start_receive()
    {
        // Discard any data already in the buffer.
        reply_buffer_.consume(reply_buffer_.size());

        // Wait for a reply. We prepare the buffer to receive up to 64KB.
        socket_.async_receive(reply_buffer_.prepare(65536),
            boost::bind(&pinger::handle_receive, this, _2));
    }

    void handle_receive(std::size_t length)
    {
        // The actual number of bytes received is committed to the buffer so that we
        // can extract it using a std::istream object.
        reply_buffer_.commit(length);

        // Decode the reply packet.
        std::istream is(&reply_buffer_);
        ipv4_header ipv4_hdr;
        icmp_header icmp_hdr;
        is >> ipv4_hdr >> icmp_hdr;

        // We can receive all ICMP packets received by the host, so we need to
        // filter out only the echo replies that match the our identifier and
        // expected sequence number.
        if(is && icmp_hdr.type() == icmp_header::echo_reply
            && icmp_hdr.identifier() == get_identifier()
            && icmp_hdr.sequence_number() == sequence_number_)
        {
            // If this is the first reply, interrupt the five second timeout.
            if(num_replies_++ == 0)
                timer_.cancel();

            // Print out some information about the reply packet.
            boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
            boost::chrono::steady_clock::duration elapsed = now - time_sent_;
            std::cout << length - ipv4_hdr.header_length()
                << " bytes from " << ipv4_hdr.source_address()
                << ": icmp_seq=" << icmp_hdr.sequence_number()
                << ", ttl=" << ipv4_hdr.time_to_live()
                << ", time="
                << boost::chrono::duration_cast<boost::chrono::milliseconds>(elapsed).count()
                << std::endl;
        }

        start_receive();
    }

    static unsigned short get_identifier()
    {
#if defined(ASIO_WINDOWS)
        return static_cast<unsigned short>(::GetCurrentProcessId());
#else
        return static_cast<unsigned short>(::getpid());
#endif
    }

    icmp::resolver resolver_;
    icmp::endpoint destination_;
    icmp::socket socket_;
    boost::asio::deadline_timer timer_;
    unsigned short sequence_number_;
    boost::chrono::steady_clock::time_point time_sent_;
    boost::asio::streambuf reply_buffer_;
    std::size_t num_replies_;
};
