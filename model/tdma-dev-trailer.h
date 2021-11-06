/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_DEV_TRAILER_H
#define TDMA_DEV_TRAILER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/object.h"
#include "ns3/trailer.h"

static const uint16_t TDMA_LORAWAN_DEV_ID_LENGTH = 20;
using namespace ns3;
// using namespace lorawan;

namespace ns3
{

  namespace lorawan
  {
    class TDMADevTrailer : public Trailer
    {
    public:
      TDMADevTrailer();
      ~TDMADevTrailer();

      static TypeId GetTypeId (void);
      TypeId GetInstanceTypeId (void) const;
      void Print (std::ostream &os) const;
      uint32_t GetSerializedSize (void) const;
      void Serialize (Buffer::Iterator start) const;
      uint32_t Deserialize (Buffer::Iterator start);
      void SetId(uint64_t rtc);
      uint64_t GetId(void);
      void SetFrameIsData(uint64_t win);
      uint64_t GetFrameIsData(void);
      private:
        int64_t m_dev_id;
        uint64_t m_frame_is_data;
    };
  } // namespace lorawan

} // namespace ns3

#endif /* TDMA_DEV_TRAILER_H */
