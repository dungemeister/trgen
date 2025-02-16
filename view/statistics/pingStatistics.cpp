#include "pingStatistics.hpp"
#include <iomanip>

void PingStatistics::showStatistics(std::string hostname) {
    std::cout << "--- " << hostname << " --- ping statistics" << "\n";
    std::cout << m_sendedPackets << " packets transmitted, "
              << m_receivedPackets << " packets received, "
              << static_cast<int>( (m_receivedPackets > 0) ?(1 - m_sendedPackets  / m_receivedPackets) * 100: 100 ) << "\% lost, "
              << "time " << m_execTime << "s\n";
    std::cout << std::setprecision(3) << "min_rtt " << m_min_rtt << " avg_rtt " <<
                 m_avg_rtt << " max_rtt " << m_max_rtt << "\n";

}