/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include "ns3/forwarder.h"
#include "ns3/log.h"
#include "ns3/lorawan-mac-header.h"
#include "ns3/lora-frame-header.h"
#include "ns3/tdma-rtc-trailer.h"
#include "ns3/lora-tag.h"

namespace ns3
{
  namespace lorawan
  {

    NS_LOG_COMPONENT_DEFINE("Forwarder");

    NS_OBJECT_ENSURE_REGISTERED(Forwarder);

    TypeId
    Forwarder::GetTypeId(void)
    {
      static TypeId tid = TypeId("ns3::Forwarder")
                              .SetParent<Application>()
                              .AddConstructor<Forwarder>()
                              .SetGroupName("lorawan");
      return tid;
    }

    Forwarder::Forwarder()
    {
      NS_LOG_FUNCTION_NOARGS();
      NS_LOG_INFO("Forwarder!!! - New");
    }

    Forwarder::~Forwarder()
    {
      NS_LOG_FUNCTION_NOARGS();
    }

    void
    Forwarder::SetPointToPointNetDevice(Ptr<PointToPointNetDevice>
                                            pointToPointNetDevice)
    {
      NS_LOG_FUNCTION(this << pointToPointNetDevice);

      m_pointToPointNetDevice = pointToPointNetDevice;
    }

    void
    Forwarder::SetLoraNetDevice(Ptr<LoraNetDevice> loraNetDevice)
    {
      NS_LOG_FUNCTION(this << loraNetDevice);

      m_loraNetDevice = loraNetDevice;
    }

    bool
    Forwarder::ReceiveFromLora(Ptr<NetDevice> loraNetDevice, Ptr<const Packet> packet, uint16_t protocol, const Address &sender)
    {
      NS_LOG_FUNCTION(this << packet << protocol << sender);

      Ptr<Packet> packetCopy = packet->Copy();

      NS_LOG_INFO("Forwarder!!!");
      // Ptr<Packet> copy = packet->Copy();
      // LorawanMacHeader mHdr;
      // copy->RemoveHeader(mHdr);
      // LoraFrameHeader fHdr;
      // // fHdr.SetAsDownlink();
      // copy->RemoveHeader(fHdr);
      // const uint32_t size = copy->GetSize();
      // NS_LOG_INFO("Packet size: " << size);
      // uint8_t buffer[size] = {};
      // copy->CopyData(buffer, size);
      // buffer[size] = 0;
      // NS_LOG_INFO("Data: " << buffer);
      LoraTag tag;
      packetCopy->PeekPacketTag(tag);
      NS_LOG_INFO("Fordwarder SF: " << unsigned(tag.GetSpreadingFactor()));

      TDMARTCTrailer tsPart;
      packetCopy->RemoveTrailer(tsPart);
      tsPart.SetSF(tag.GetSpreadingFactor());
      packetCopy->AddTrailer(tsPart);

      m_pointToPointNetDevice->Send(packetCopy, m_pointToPointNetDevice->GetBroadcast(), 0x800);

      return true;
    }

    bool
    Forwarder::ReceiveFromPointToPoint(Ptr<NetDevice> pointToPointNetDevice,
                                       Ptr<const Packet> packet, uint16_t protocol,
                                       const Address &sender)
    {
      NS_LOG_FUNCTION(this << packet << protocol << sender);

      Ptr<Packet> packetCopy = packet->Copy();

      m_loraNetDevice->Send(packetCopy);

      return true;
    }

    void
    Forwarder::StartApplication(void)
    {
      NS_LOG_FUNCTION(this);

      // TODO Make sure we are connected to both needed devices
    }

    void
    Forwarder::StopApplication(void)
    {
      NS_LOG_FUNCTION_NOARGS();

      // TODO Get rid of callbacks
    }

  } // namespace lorawan
} // namespace ns3
