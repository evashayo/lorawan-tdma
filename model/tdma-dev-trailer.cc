
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/log.h"
#include "ns3/system-thread.h"
#include <chrono>
#include <thread>
#include "ns3/simulator.h"
#include "ns3/tdma-dev-trailer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"

namespace ns3
{
  namespace lorawan
  {
    NS_LOG_COMPONENT_DEFINE("TDMADevTrailer");

    NS_OBJECT_ENSURE_REGISTERED(TDMADevTrailer);

    TDMADevTrailer::TDMADevTrailer()
    {
    }
    TDMADevTrailer::~TDMADevTrailer()
    {
    }
    TypeId TDMADevTrailer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::TDMADevTrailer")
        .SetParent<Trailer> ()
        .SetGroupName ("tdma")
        .AddConstructor<TDMADevTrailer> ();
      return tid;
    }

    TypeId TDMADevTrailer::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    void TDMADevTrailer::Print (std::ostream &os) const
    {
        os << "Dev ID: " << std::dec << m_dev_id;
    }
    void TDMADevTrailer::SetId (uint64_t dev_id)
    {
        m_dev_id = dev_id;
    }
    uint64_t TDMADevTrailer::GetId (void)
    {
        return m_dev_id;
    }

    void TDMADevTrailer::SetFrameIsData (uint64_t is_data)
    {
        m_frame_is_data=is_data;
    }
    uint64_t TDMADevTrailer::GetFrameIsData (void)
    {
        return m_frame_is_data;
    }

    uint32_t TDMADevTrailer::GetSerializedSize (void) const
    {
      return TDMA_LORAWAN_DEV_ID_LENGTH;
    }

    void TDMADevTrailer::Serialize (Buffer::Iterator start) const
    {
      NS_LOG_INFO("Serialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_DEV_ID_LENGTH);
      i.WriteU64(m_dev_id);
      i.WriteU64(m_frame_is_data);
    }

    uint32_t TDMADevTrailer::Deserialize (Buffer::Iterator start)
    {
      NS_LOG_INFO("Deserialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_DEV_ID_LENGTH);
      m_dev_id=i.ReadU64();
      m_frame_is_data = i.ReadU64();
      return TDMA_LORAWAN_DEV_ID_LENGTH;
    }
  } // namespace lorawan

} // namespace ns3
