/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_LORAWAN_RTC_H
#define TDMA_LORAWAN_RTC_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/object.h"
#include "ns3/system-wall-clock-ms.h"

using namespace ns3;
// using namespace lorawan;

namespace ns3
{

  namespace lorawan
  {
    class TDMALorawanRTC : public Object
    {
    public:
      TDMALorawanRTC();
      ~TDMALorawanRTC();

      static TypeId GetTypeId(void);
      void StartRTC(void);
      int64_t GetTimestamp(void);
      void SetDelta(int64_t);

    private:
      SystemWallClockMs m_clock;
      int64_t m_delta;
    };

  } // namespace lorawan

} // namespace ns3

#endif /* TDMA_LORAWAN_RTC_H */
