
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/log.h"
#include "ns3/system-thread.h"
#include <chrono>
#include <thread>
#include "ns3/simulator.h"
#include "ns3/tdma-rtc-trailer.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"

namespace ns3
{
  namespace lorawan
  {
    NS_LOG_COMPONENT_DEFINE("TDMARTCTrailer");

    NS_OBJECT_ENSURE_REGISTERED(TDMARTCTrailer);

    TDMARTCTrailer::TDMARTCTrailer()
    {
    }
    TDMARTCTrailer::~TDMARTCTrailer()
    {
    }
    TypeId TDMARTCTrailer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::TDMARTCTrailer")
        .SetParent<Trailer> ()
        .SetGroupName ("tdma")
        .AddConstructor<TDMARTCTrailer> ();
      return tid;
    }

    TypeId TDMARTCTrailer::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    void TDMARTCTrailer::Print (std::ostream &os) const
    {
        os << "RTC: " << std::dec << m_rtc;
    }
    void TDMARTCTrailer::SetRTC (uint64_t rtc)
    {
        m_rtc=rtc;
    }
    uint64_t TDMARTCTrailer::GetRTC (void)
    {
        return m_rtc;
    }
    void TDMARTCTrailer::SetFrameSlot (uint64_t slot)
    {
        m_frame_slot=slot;
    }
    uint64_t TDMARTCTrailer::GetFrameSlot (void)
    {
        return m_frame_slot;
    }
    void TDMARTCTrailer::SetSF (uint64_t sf)
    {
        m_sf=sf;
    }
    uint64_t TDMARTCTrailer::GetSF (void)
    {
        return m_sf;
    }
     void TDMARTCTrailer::SetFrameIsData (uint64_t is_data)
    {
        m_frame_is_data=is_data;
    }
    uint64_t TDMARTCTrailer::GetFrameIsData (void)
    {
        return m_frame_is_data;
    }
    void TDMARTCTrailer::SetId (uint64_t dev_id)
    {
        m_dev_id=dev_id;
    }
    uint64_t TDMARTCTrailer::GetId (void)
    {
        return m_dev_id;
    }
    void TDMARTCTrailer::SetSyncWindow (uint64_t win)
    {
        m_sync_window = win;
    }
    uint64_t TDMARTCTrailer::GetSyncWindow (void)
    {
        return m_sync_window;
    }

    uint32_t TDMARTCTrailer::GetSerializedSize (void) const
    {
      return TDMA_LORAWAN_RTC_LENGTH;
    }

    void TDMARTCTrailer::Serialize (Buffer::Iterator start) const
    {
      NS_LOG_INFO("Serialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_RTC_LENGTH);
      i.WriteU64(m_rtc);
      i.WriteU64(m_dev_id);
      i.WriteU64(m_frame_slot);
      i.WriteU64(m_sf);
      i.WriteU64(m_frame_is_data);
      i.WriteU64(m_sync_window);
      NS_LOG_INFO("Dev ID: " << m_dev_id);
    }

    uint32_t TDMARTCTrailer::Deserialize (Buffer::Iterator start)
    {
      NS_LOG_INFO("Deserialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_RTC_LENGTH);
      m_rtc = i.ReadU64();
      m_dev_id = i.ReadU64();
      m_frame_slot = i.ReadU64();
      m_sf = i.ReadU64();
      m_frame_is_data = i.ReadU64();
      m_sync_window = i.ReadU64();
      return TDMA_LORAWAN_RTC_LENGTH;
    }
  } // namespace lorawan

} // namespace ns3
