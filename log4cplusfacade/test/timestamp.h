#ifndef _APPSERVER_BASE_TIMESTAMP_H
#define _APPSERVER_BASE_TIMESTAMP_H

#include <stdint.h>
#include <string>

namespace appserver
{
namespace base
{

class Timestamp
{
public:
    Timestamp()
    : m_microSecondsSinceEpoch(0)
    {
    }

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
    : m_microSecondsSinceEpoch(microSecondsSinceEpochArg)
    {
    }

    void swap(Timestamp& that)
    {
        std::swap(m_microSecondsSinceEpoch, that.m_microSecondsSinceEpoch);
    }

    //default copy/assignment/dtor are Okay

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const { return m_microSecondsSinceEpoch > 0; }

    // for internal usage.
    int64_t microSecondsSinceEpoch() const { return m_microSecondsSinceEpoch; }

    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(m_microSecondsSinceEpoch / kMicroSecondsPerSecond);
    }

    inline bool operator<(const Timestamp& rhs) const
    {
        return this->m_microSecondsSinceEpoch < rhs.microSecondsSinceEpoch();
    }

    inline bool operator>(const Timestamp& rhs) const
    {
        return this->m_microSecondsSinceEpoch > rhs.microSecondsSinceEpoch();
    }

    inline bool operator==(const Timestamp& rhs) const
    {
        return this->m_microSecondsSinceEpoch == rhs.microSecondsSinceEpoch();
    }

    static Timestamp now();

    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t m_microSecondsSinceEpoch;
};

inline int64_t timeDiffInMicroSec(Timestamp high, Timestamp low)
{
    return high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
}

inline double timeDiffInSec(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}
}

#endif

