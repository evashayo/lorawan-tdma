/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_SENDER_HELPER_H
#define TDMA_SENDER_HELPER_H

#include "ns3/tdma.h"
#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/tdma-sender.h"
#include <stdint.h>
#include <string>

namespace ns3 {

namespace lorawan {

/**
 * This class can be used to install OneShotSender applications on multiple
 * nodes at once.
 */
class TDMASenderHelper
{
public:
  TDMASenderHelper ();

  ~TDMASenderHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c) const;

  ApplicationContainer Install (Ptr<Node> node) const;

  void SetTDMAParams(TDMAParams params);

private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory;

  TDMAParams tdma_params;
};


}

}

#endif /* TDMA_HELPER_H */

