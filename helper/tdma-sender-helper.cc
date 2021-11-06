/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "tdma-helper.h"
#include "ns3/tdma-sender-helper.h"
#include "ns3/tdma-sender.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/lora-net-device.h"

namespace ns3 {

namespace lorawan {

NS_LOG_COMPONENT_DEFINE("TDMASenderHelper");

TDMASenderHelper::TDMASenderHelper() {
	m_factory.SetTypeId("ns3::TDMASender");
}

TDMASenderHelper::~TDMASenderHelper() {
}

void TDMASenderHelper::SetTDMAParams(TDMAParams params) {
	tdma_params = params;
}

void TDMASenderHelper::SetAttribute(std::string name,
		const AttributeValue &value) {
	m_factory.Set(name, value);
}

ApplicationContainer TDMASenderHelper::Install(Ptr<Node> node) const {
	return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer TDMASenderHelper::Install(NodeContainer c) const {
	ApplicationContainer apps;
	uint16_t counter(0);
	for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
		counter++;
	}
	counter = 0;
	for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
		apps.Add(InstallPriv(*i));
		counter++;
	}

	return apps;
}

Ptr<Application> TDMASenderHelper::InstallPriv(Ptr<Node> node) const {

	Ptr<TDMASender> app = m_factory.Create<TDMASender>();
	app->SetTDMAParams(tdma_params);
	app->SetNode(node);
	node->AddApplication(app);

	//Receive Uplink packets from MAC Layer and deliver to Application
	Ptr<LoraNetDevice> loraNetDevice = node->GetDevice(0)->GetObject<LoraNetDevice> ();
	loraNetDevice->SetReceiveCallback (MakeCallback(&TDMASender::Receive, app));
	return app;
}

}

}

