
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/log.h"
#include "ns3/system-thread.h"
#include <chrono>
#include <thread>
#include "ns3/simulator.h"
#include "ns3/tdma-lorawan-rtc.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"


namespace ns3
{
  namespace lorawan
  {
    NS_LOG_COMPONENT_DEFINE("TDMALorawanRTC");

    NS_OBJECT_ENSURE_REGISTERED(TDMALorawanRTC);

    TypeId TDMALorawanRTC::GetTypeId(void)
    {
      static TypeId tid = TypeId("ns3::TDMALorawanRTC").SetParent<Object>().AddConstructor<TDMALorawanRTC>().SetGroupName("tdma");
      return tid;
    }

    TDMALorawanRTC::TDMALorawanRTC() : m_clock()
    {
      // Ptr<UniformRandomVariable> rnd = CreateObject<UniformRandomVariable>();
      // rnd->SetAttribute("Min", DoubleValue(0));
      // rnd->SetAttribute("Max", DoubleValue(0));
      // m_delta = rnd->GetValue();
      
      m_delta=0;
      NS_LOG_INFO("Initial Delta: " << m_delta);
      // StartRTC();
      m_clock.Start();
    }
    TDMALorawanRTC::~TDMALorawanRTC()
    {
      uint64_t ts = m_clock.End();
      NS_LOG_INFO("Destroyed at RTC: " << ts);
    }
    void TDMALorawanRTC::StartRTC(void)
    {
      // m_clock.Start();
      uint64_t ts = GetTimestamp();
      NS_LOG_INFO("OnStart: " << ts);
    }
    int64_t TDMALorawanRTC::GetTimestamp(void)
    {
      int64_t ts = Simulator::Now().GetMilliSeconds();
      m_clock.End();
      // uint64_t e_sys = m_clock.GetElapsedSystem();
      // uint64_t e_usr = m_clock.GetElapsedUser();
      // uint64_t d_usr = e_end-e_usr;
      // uint64_t d_sys = e_end-e_sys;
      // uint64_t d_end = e_end - ts;
      // uint64_t cum_sys = e_sys * 100;
      
      // NS_LOG_INFO("RTC Variations: " << e_end << ", " << e_sys << ", " << e_usr << ", " << d_sys << ", " << d_usr << "; "<< ts << ", " << d_end << " >> " << cum_sys);
      // return (e_sys * 100) + m_delta;
      return ts + m_delta;
      // return e_sys + m_delta;
    }
    void TDMALorawanRTC::SetDelta(int64_t delta)
    {
      // int64_t ts1 = this->GetTimestamp();
      m_delta += delta;
      // int64_t ts2 = this->GetTimestamp();
      // NS_LOG_INFO("RTC DELTA: " << delta << "->" << ts1 << " >> " << ts2);
    }
  } // namespace lorawan

} // namespace ns3
