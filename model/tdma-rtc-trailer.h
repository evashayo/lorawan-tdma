/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_RTC_TRAILER_H
#define TDMA_RTC_TRAILER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/object.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/trailer.h"

static const uint16_t TDMA_LORAWAN_RTC_LENGTH = 60;

using namespace ns3;
// using namespace lorawan;

namespace ns3
{

  namespace lorawan
  {
    class TDMARTCTrailer : public Trailer
    {
    public:
      TDMARTCTrailer();
      ~TDMARTCTrailer();

      static TypeId GetTypeId (void);
      TypeId GetInstanceTypeId (void) const;
      void Print (std::ostream &os) const;
      uint32_t GetSerializedSize (void) const;
      void Serialize (Buffer::Iterator start) const;
      uint32_t Deserialize (Buffer::Iterator start);
      void SetRTC(uint64_t rtc);
      uint64_t GetRTC(void);
      void SetFrameSlot(uint64_t slot);
      uint64_t GetFrameSlot(void);
      void SetSF(uint64_t sf);
      uint64_t GetSF(void);
      void SetId(uint64_t dev_id);
      uint64_t GetId(void);
      void SetSyncWindow(uint64_t win);
      uint64_t GetSyncWindow(void);
      void SetFrameIsData(uint64_t win);
      uint64_t GetFrameIsData(void);
      private:
        int64_t m_rtc;
        int64_t m_dev_id;
        uint64_t m_frame_slot;
        uint64_t m_sf;
        uint64_t m_frame_is_data;
        uint64_t m_sync_window;
    };
  } // namespace lorawan

} // namespace ns3

#endif /* TDMA_RTC_TRAILER_H */
