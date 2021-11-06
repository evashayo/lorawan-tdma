/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "tdma.h"
#include "ns3/tdma-sender.h"
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/lora-net-device.h"
#include "ns3/simulator.h"
#include "string.h"
#include "ns3/lora-tag.h"
#include "ns3/simple-end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/system-thread.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/packet.h"
#include "ns3/tdma-rtc-trailer.h"
#include "ns3/tdma-dev-trailer.h"

using namespace std;

namespace ns3
{

	namespace lorawan
	{
		NS_LOG_COMPONENT_DEFINE("TDMASender");

		NS_OBJECT_ENSURE_REGISTERED(TDMASender);

		TypeId TDMASender::GetTypeId(void)
		{
			static TypeId tid = TypeId("ns3::TDMASender").SetParent<Application>().AddConstructor<TDMASender>().SetGroupName("tdma");
			return tid;
		}

		TDMASender::TDMASender()
			: m_clock()
		{
			m_rate = 500;
			m_packet = 0;
			m_send_window = 0;
			time_t t_time(time(NULL));
			startTime = *localtime(&t_time);
			m_interval = 30 * 1;
			m_tracker = 0;
			deviceType = 0;
			// m_rtc = Create<TDMALorawanRTC>();
		}

		TDMASender::~TDMASender()
		{
			NS_LOG_FUNCTION_NOARGS();
		}

		void TDMASender::SetTDMAParams(TDMAParams params)
		{
			m_interval = params.interval;
			deviceType = params.deviceType;
			m_syncByDevId = params.mSyncByDevId;
		}

		void TDMASender::ScheduleReach(void)
		{
			int64_t ts = m_mac->GetTimestamp();
			tm ltm = startTime;
			double sim_tm = Simulator::Now().GetSeconds();
			ltm.tm_sec += sim_tm;
			char now_str[40];
			fmt_date_tm(&ltm, now_str, 0);
			NS_LOG_INFO("Sync Ready: " << m_syncReady);

			Ptr<Packet> packet;
			if (m_syncReady)
			{
				packet = Create<Packet>((const uint8_t *)"My Data", 5);
				TDMADevTrailer devPart;
				devPart.SetId(m_node->GetId());
				devPart.SetFrameIsData(1);
				packet->AddTrailer(devPart);
				NS_LOG_INFO("DATA: ............ RTC: " << ts << ", Sent: " << this->m_packet << ", At: " << now_str << ", The packet: " << packet);
				if (deviceType == 1) //ED_A
				{
					m_mac->GetObject<ClassAEndDeviceLorawanMac>()->SetMType(LorawanMacHeader::UNCONFIRMED_DATA_UP);
				}
				m_mac->Send(packet);
				NS_LOG_INFO("Sent data packet");
			}
			else
			{
				packet = Create<Packet>((const uint8_t *)"Hello", 5);

				TDMADevTrailer devPart;
				devPart.SetId(m_node->GetId());
				devPart.SetFrameIsData(0);
				packet->AddTrailer(devPart);
				NS_LOG_INFO("SYNC: .............. RTC: " << ts << ", Sent: " << this->m_packet << ", At: " << now_str << ", The packet: " << packet);
				if (deviceType == 1) //ED_A
				{
					m_mac->GetObject<ClassAEndDeviceLorawanMac>()->SetMType(LorawanMacHeader::CONFIRMED_DATA_UP);
				}
				m_mac->Send(packet);
			}
			Simulator::Cancel(m_sendEvent);
		}

		void TDMASender::SendPacket(void)
		{
			//  NS_LOG_FUNCTION_NOARGS ();
			if (m_tracker == 0)
			{
				Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable>("Min", DoubleValue(0), "Max", DoubleValue(5));
				m_tracker = rv->GetInteger();
			}

			double t_slot = 0;
			if (m_syncByDevId == 1)
			{
				t_slot = node_id * 20; // 3 minutes; Tx(t) + 1(1st W) + 1(2nd W)
				//TODONS3
				//1. Formula to calculate t_slot more accurately
			}
			NS_LOG_INFO("New window; Slot: " << t_slot << ", Tracker: " << m_tracker);

			Simulator::Cancel(m_sendEvent);
			m_send_window = delta(1);
			m_sendEvent = Simulator::Schedule(Seconds(t_slot), &TDMASender::ScheduleReach, this);

			if (t_slot!=0)
			{
				Ptr<ClassAEndDeviceLorawanMac> edMac = m_mac->GetObject<ClassAEndDeviceLorawanMac> ();
				edMac->ForceDeviceSleep();
			}
		}

		void TDMASender::StartApplication(void)
		{
			NS_LOG_FUNCTION(this);
			//Schedule next Cycle and "m_interval"
			Simulator::Cancel(m_intervalSyncEvent);
			// m_intervalSyncEvent = Simulator::Schedule(Seconds(m_interval), &TDMASender::StartApplication, this);

			m_syncReady = false; //Start with Sync
			m_syncSlot = 0;
			m_syncWindow = 0;
			node_id = m_node->GetId();
			Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice(0)->GetObject<LoraNetDevice>();

			// Make sure we have a MAC layer
			if (m_mac == 0)
			{
				// Assumes there's only one device
				m_mac = loraNetDevice->GetMac();
				NS_ASSERT(m_mac != 0);
				m_mac->SetTxFinishedCallback(MakeCallback(&TDMASender::TxFinished, this));
				if (m_syncByDevId == 1)
				{
					m_mac->GetObject<EndDeviceLorawanMac>()->SetMaxNumberOfTransmissions(1);
				}
			}

			// Schedule the next SendPacket event
			Simulator::Cancel(m_sendEvent);
			m_sendEvent = Simulator::Schedule(Seconds(0), &TDMASender::SendPacket, this);
			m_syncStart = m_mac->GetTimestamp()/1000;
		}

		void TDMASender::StopApplication(void)
		{
			NS_LOG_FUNCTION_NOARGS();
			NS_LOG_INFO("Stopping Application");
			Simulator::Cancel(m_sendEvent);
			Simulator::Cancel(m_intervalSyncEvent);
		}

		bool TDMASender::Receive(Ptr<NetDevice> loraNetDevice, Ptr<const Packet> packet, uint16_t protocol, const Address &sender)
		{
			int64_t ts = m_mac->GetTimestamp();
			Ptr<Packet> copy = packet->Copy();
			LoraTag s_tag;
			copy->RemovePacketTag(s_tag);

			TDMARTCTrailer rtc;
			copy->RemoveTrailer(rtc);

			uint64_t sync_Tx = m_Tx;
			uint64_t sync_Rx = ts;
			uint64_t sync_TGw = rtc.GetRTC();
			int64_t new_delta = sync_TGw - sync_Tx;
			u_int64_t dev_id = rtc.GetId();

			NS_LOG_INFO("Sync Details >> "
						<< "TX: " << sync_Tx << ", RX: " << sync_Rx << ", TG: " << sync_TGw << ", Delta: " << new_delta << ", Dev Id: " << dev_id);

			Simulator::Cancel(m_sendEvent);
			if (new_delta != 0)
			{
				NS_LOG_INFO("Update ED_A RTC to sync with Gw: " << new_delta);
				m_mac->SetDelta(new_delta);
				NS_LOG_INFO("Interval(seconds): " << m_interval << "\n\n");
				NS_LOG_INFO("ED_A RTC is in sync with Gw::Adjusted");
				OnSyncReady(rtc);
			}
			else
			{
				NS_LOG_INFO("ED_A RTC is in sync with Gw");
				//Schedule next trans
				OnSyncReady(rtc);
			}
			return true;
		}

		void
		TDMASender::TxFinished(Ptr<const Packet> packet)
		{
			Ptr<Packet> copy = packet->Copy();
			TDMADevTrailer devHdr;
			copy->RemoveTrailer(devHdr);

			m_Tx = m_mac->GetTimestamp();
			NS_LOG_INFO("Finished TX @: " << m_Tx << ", DevID: " << devHdr.GetId());
		}

		void TDMASender::OnSyncReady(TDMARTCTrailer rtc)
		{
			int64_t ts = m_mac->GetTimestamp()/1000;
			int64_t syncTimeDelta = ts - m_syncStart;
			m_syncReady = true;
			m_syncWindow = rtc.GetSyncWindow();
			m_syncSlot = rtc.GetFrameSlot(); // sync_win + data_t_slot (When to transmit)
			if (m_syncByDevId == 0){ //Non slotted
				m_syncSlot = m_syncSlot - m_syncWindow;
				m_syncWindow = 1280; // 160 sec * 8 retransmissions
				m_syncSlot = m_syncSlot + m_syncWindow;
			}
			
			// int64_t dataTxTime = m_syncSlot - syncTimeDelta;
			int64_t dataTxTime = (m_syncStart + m_syncSlot) - ts;
			NS_LOG_INFO("Sync Window......................................" << m_syncWindow);
			NS_LOG_INFO("TS, Start TS, Slot, Delta: ......................" << m_syncStart << "; " << ts << "; " << m_syncSlot << "; " << syncTimeDelta);
			NS_LOG_INFO("Data Tx Time....................................." << dataTxTime << " >> " << (ts + dataTxTime));
			Simulator::Cancel(m_sendEvent);
			// Schedule data Transmission
			//m_sendEvent = Simulator::Schedule(Seconds(dataTxTime), &TDMASender::ScheduleReach, this);
			Ptr<ClassAEndDeviceLorawanMac> edMac = m_mac->GetObject<ClassAEndDeviceLorawanMac> ();
			edMac->ForceDeviceSleep();
		}

	} // namespace lorawan

} // namespace ns3
