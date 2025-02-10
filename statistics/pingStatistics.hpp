#ifndef TRGEN_PING_STATISTICS_HPP
#define TRGEN_PING_STATISTICS_HPP

#include <string>
#include <iostream>


class PingStatistics {
public:
    PingStatistics() {}
    ~PingStatistics() {}

    void showStatistics(std::string hostname);
    void increaseSendedPackets() { m_sendedPackets++; }
    void increaseReceivedPackets() { m_receivedPackets++; }
    void setMinRtt(double rtt) { if(rtt < m_min_rtt || m_min_rtt == 0) m_min_rtt = rtt; }
    void setMaxRtt(double rtt) { if(rtt > m_max_rtt) m_max_rtt = rtt; }
    void setExecTime(double et) { m_execTime = et; }
private:
    double m_max_rtt = 0;
    double m_min_rtt = 0;
    double m_avg_rtt = 0;
    int m_sendedPackets = 0;
    int m_receivedPackets = 0;
    double m_execTime = 0.0;
};

#endif //TRGEN_PING_STATISTICS_HPP