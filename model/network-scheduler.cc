#include "network-scheduler.h"
#include "ns3/tdma-rtc-trailer.h"
#include "ns3/lora-utils.h"

namespace ns3
{
  namespace lorawan
  {

    NS_LOG_COMPONENT_DEFINE("NetworkScheduler");

    NS_OBJECT_ENSURE_REGISTERED(NetworkScheduler);

    TypeId
    NetworkScheduler::GetTypeId(void)
    {
      static TypeId tid = TypeId("ns3::NetworkScheduler")
                              .SetParent<Object>()
                              .AddConstructor<NetworkScheduler>()
                              .AddTraceSource("ReceiveWindowOpened",
                                              "Trace source that is fired when a receive window opportunity happens.",
                                              MakeTraceSourceAccessor(&NetworkScheduler::m_receiveWindowOpened),
                                              "ns3::Packet::TracedCallback")
                              .SetGroupName("lorawan");
      return tid;
    }

    NetworkScheduler::NetworkScheduler()
    {
      // m_frame_pos={0,0,0,0,0,0};
      m_sync_win = 0;
    }

    NetworkScheduler::NetworkScheduler(Ptr<NetworkStatus> status,
                                       Ptr<NetworkController> controller) : m_status(status),
                                                                            m_controller(controller)
    {
      // m_frame_pos={};
      m_sync_win = 0;
    }

    NetworkScheduler::~NetworkScheduler()
    {
    }

    void
    NetworkScheduler::OnReceivedPacket(Ptr<const Packet> packet)
    {
      // NS_LOG_FUNCTION(packet);

      // Get the current packet's frame counter
      Ptr<Packet> packetCopy = packet->Copy();
      LorawanMacHeader receivedMacHdr;
      packetCopy->RemoveHeader(receivedMacHdr);
      LoraFrameHeader receivedFrameHdr;
      receivedFrameHdr.SetAsUplink();
      packetCopy->RemoveHeader(receivedFrameHdr);
      uint8_t currentFrameCounter = receivedFrameHdr.GetFCnt();

      TDMARTCTrailer rtc;
      packetCopy->RemoveTrailer(rtc);
      m_rtc = rtc.GetRTC();
      m_dev_id = rtc.GetId();
      m_sf = rtc.GetSF();
      uint64_t isData = rtc.GetFrameIsData();
      NS_LOG_INFO("Capture Spreading Factor(SF): " << m_dev_id << ";" << m_sf << ";" << isData);

      if (isData)
      {
        //assuming always atleast 1 data packet will reach NS
        for (int i = 0; i < 6; i++)
        {
          m_frame_pos[i] = 0;
        }
        NS_LOG_INFO("Cleared frame positons!");
        NS_LOG_INFO("Data Packet Received");
      }

      // Get the saved packet's frame counter
      Ptr<const Packet> savedPacket = m_status->GetEndDeviceStatus(packet)->GetLastReceivedPacketInfo().packet;
      if (savedPacket)
      {
        Ptr<Packet> savedPacketCopy = savedPacket->Copy();
        LorawanMacHeader savedMacHdr;
        savedPacketCopy->RemoveHeader(savedMacHdr);
        LoraFrameHeader savedFrameHdr;
        savedFrameHdr.SetAsUplink();
        savedPacketCopy->RemoveHeader(savedFrameHdr);
        uint8_t savedFrameCounter = savedFrameHdr.GetFCnt();

        if (currentFrameCounter == savedFrameCounter)
        {
          NS_LOG_DEBUG("Packet was already received by another gateway.");
          return;
        }
      }

      // It's possible that we already received the same packet from another
      // gateway.
      // - Extract the address
      LoraDeviceAddress deviceAddress = receivedFrameHdr.GetAddress();

      if (!isData)
      {
        // Schedule OnReceiveWindowOpportunity event
        Simulator::Schedule(Seconds(1), &NetworkScheduler::OnReceiveWindowOpportunity, this, deviceAddress, 1); // This will be the first receive window
      }
    }

    void
    NetworkScheduler::OnReceiveWindowOpportunity(LoraDeviceAddress deviceAddress, int window)
    {
      // NS_LOG_FUNCTION(deviceAddress);
      NS_LOG_DEBUG("Opening receive window nubmer " << window << " for device " << deviceAddress << ", DevID: " << m_dev_id);

      // Check whether we can send a reply to the device, again by using
      // NetworkStatus
      Address gwAddress = m_status->GetBestGatewayForDevice(deviceAddress, window);

      // NS_LOG_DEBUG("Found available gateway with address: " << gwAddress <<  "; DevID: " << m_dev_id);
      if (gwAddress == Address() && window == 1)
      {
        NS_LOG_DEBUG("No suitable gateway found. DevID: " << m_dev_id);
        // No suitable GW was found
        // Schedule OnReceiveWindowOpportunity event
        Simulator::Schedule(Seconds(1), &NetworkScheduler::OnReceiveWindowOpportunity, this, deviceAddress, 2); // This will be the second receive window
      }
      else if (gwAddress == Address() && window == 2)
      {
        // No suitable GW was found
        // Simply give up.
        NS_LOG_INFO("Giving up on reply: no suitable gateway was found "
                    << "on the second receive window. DevID: " << m_dev_id);

        // Reset the reply
        // XXX Should we reset it here or keep it for the next opportunity?
        m_status->GetEndDeviceStatus(deviceAddress)->InitializeReply();
      }
      else
      {
        // A gateway was found
        m_controller->BeforeSendingReply(m_status->GetEndDeviceStatus(deviceAddress));

        // Check whether this device needs a response by querying m_status
        bool needsReply = m_status->NeedsReply(deviceAddress);

        if (needsReply)
        {
          NS_LOG_INFO("A reply is needed. DevID: " << m_dev_id);

          // Send the reply through that gateway
          Ptr<Packet> ackPacket = Create<Packet>((const uint8_t *)"ACK", 3);

          //Sync Window(Ref) = f(n) + T_buffer + 1st_Gw_Time
          // int sf_num=ExtractInt(m_sf);
          int sf_idx = m_sf - 7;
          NS_LOG_INFO("Frame Pos: " << m_frame_pos[sf_idx] << ", For SF: " << m_sf);
          uint8_t slot_size_sync = 20; //1 second as slot size
          //TODONS3
          //Formula for slot_size_sync for sync transmission

          uint8_t slot_size_data = 10; //1 second as slot size
          //TODONS3
          //Formula for slot_size_data for data transmission

          // Calculate Sync Window
          bool calculated = 0;
          for (int i = 0; i < 6; i++)
          {
            if (m_frame_pos[i] != 0)
            {
              calculated = 1;
              NS_LOG_INFO("Initiatilzed At: " << i << ", Val: " << m_frame_pos[i]);
              break;
            }
          }
          if (!calculated)
          { // The first
            m_sync_win = n_devices * slot_size_sync;
          }

          m_frame_pos[sf_idx] = m_frame_pos[sf_idx] + 1; // Newly synched dev

          TDMARTCTrailer tsPart;
          tsPart.SetRTC(m_rtc);
          tsPart.SetId(m_dev_id);
          tsPart.SetFrameSlot(m_frame_pos[sf_idx] * slot_size_data + m_sync_win);
          tsPart.SetSF(m_sf);
          tsPart.SetSyncWindow(m_sync_win);
          ackPacket->AddTrailer(tsPart);

          //Dev trans time = m_sync_win + (m_frame_pos * tx_interval)

          m_status->GetEndDeviceStatus(deviceAddress)->SetReplyPayload(ackPacket);
          m_status->SendThroughGateway(m_status->GetReplyForDevice(deviceAddress, window), gwAddress);

          // Reset the reply
          m_status->GetEndDeviceStatus(deviceAddress)->InitializeReply();
        }
      }
    }
    void
    NetworkScheduler::SetNumberOfDevices(uint32_t n)
    {
      NS_LOG_INFO("Devices: " << n);
      n_devices = n;
    }
  } // namespace lorawan
} // namespace ns3
