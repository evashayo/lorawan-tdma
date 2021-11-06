/*
 * This script simulates a simple network in which one end device sends one
 * packet to the gateway.
 */

#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/node-container.h"
#include "ns3/position-allocator.h"
#include "ns3/tdma-sender-helper.h"
#include "ns3/command-line.h"
#include <algorithm>
#include <ctime>
#include "ns3/packet-sink.h"

#include "ns3/forwarder-helper.h"
#include "ns3/network-server-helper.h"
#include "ns3/tdma-rtc-trailer.h"

// Lora Energy Model
#include "ns3/basic-energy-source-helper.h"
#include "ns3/lora-radio-energy-model-helper.h"
#include "ns3/file-helper.h"
#include "ns3/names.h"

using namespace ns3;
using namespace lorawan;
//using namespace tdma;

NS_LOG_COMPONENT_DEFINE("EvaNetworkTDMAv4");

int main(int argc, char *argv[])
{
	uint16_t nDevices = 2;
	// uint16_t nPeriods = 20;
	uint16_t nMinutes = 2;
	uint16_t interval = 60 * 30; //30 minutes by default
	uint64_t nRngRun = 2;
	uint16_t mSyncByDevId = 1; //0 = Sync simultaneously; 1 = Sync by device id sloting 

	CommandLine cmd;
	cmd.AddValue("nDevices", "Number of end devices", nDevices);
	cmd.AddValue("nRngRun", "Random Generator Number", nRngRun);
	cmd.AddValue("interval", "Trigger sending interval in seconds", interval);
	cmd.AddValue("nMinutes", "Simulation in minutes", nMinutes);
	cmd.AddValue("mSyncByDevId", "Make RTC sync using Device Id for slotting", mSyncByDevId);
	cmd.Parse(argc, argv);

	

	// GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

	// Set up logging
	LogComponentEnable("EvaNetworkTDMAv4", LOG_LEVEL_ALL);

	LogComponentEnable("ClassAEndDeviceLorawanMac", LOG_LEVEL_ALL);
	// LogComponentEnable("Config", LOG_LEVEL_ALL);
	LogComponentEnable("EndDeviceLoraPhy", LOG_LEVEL_ALL);
	// LogComponentEnable("EndDeviceLorawanMac", LOG_LEVEL_ALL);
	// LogComponentEnable("EndDeviceStatus", LOG_LEVEL_ALL);
	// LogComponentEnable("FileHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("Forwarder", LOG_LEVEL_INFO);
	// LogComponentEnable("ForwarderHelper", LOG_LEVEL_INFO);
	// LogComponentEnable("GatewayLoraPhy", LOG_LEVEL_ALL);
	LogComponentEnable("GatewayStatus", LOG_LEVEL_ALL);
	LogComponentEnable("GatewayLorawanMac", LOG_LEVEL_ALL);
	// LogComponentEnable("LogicalLoraChannel", LOG_LEVEL_ALL);
	// LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraInterferenceHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraNetDevice", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraPacketTracker", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraPhy", LOG_LEVEL_ALL);
	// LogComponentEnable("LoraPhyHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("LorawanMac", LOG_LEVEL_ALL);
	// LogComponentEnable("LorawanMacHeader", LOG_LEVEL_ALL);
	// LogComponentEnable("LorawanMacHelper", LOG_LEVEL_ALL);
	// LogComponentEnable("Names", LOG_LEVEL_ALL);
	// LogComponentEnable("NetworkControllerComponent", LOG_LEVEL_ALL);
	// LogComponentEnable("NetworkScheduler", LOG_LEVEL_ALL);
	// LogComponentEnable("NetworkServer", LOG_LEVEL_ALL);
	LogComponentEnable("NetworkStatus", LOG_LEVEL_ALL);
	// LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
	// LogComponentEnable("SimpleEndDeviceLoraPhy", LOG_LEVEL_ALL);
	// LogComponentEnable("SimpleGatewayLoraPhy", LOG_LEVEL_ALL);
	// LogComponentEnable("SystemWallClockMs", LOG_LEVEL_ALL);
	LogComponentEnable("Simulator", LOG_LEVEL_ALL);
	// LogComponentEnable("SubBand", LOG_LEVEL_ALL);
	// LogComponentEnable("TDMALorawanRTC", LOG_LEVEL_ALL);
	// LogComponentEnable("TDMARTCTrailer", LOG_LEVEL_INFO);
	LogComponentEnable("TDMASender", LOG_LEVEL_ALL);
	// LogComponentEnable("TDMASenderHelper", LOG_LEVEL_ALL);

	LogComponentEnableAll(LOG_PREFIX_FUNC);
	LogComponentEnableAll(LOG_PREFIX_NODE);
	LogComponentEnableAll(LOG_PREFIX_TIME);

	NS_LOG_INFO("Params: " << nDevices << " devices, " << interval << " seconds interval, " << nMinutes << " minutes run time" << ", Sloted: " << mSyncByDevId);

	/***********************
	 * Initial setup
	 **********************/
	SeedManager::SetRun(nRngRun);
	Packet::EnablePrinting();
	Packet::EnableChecking();

	/************************
	 *  Create the channel  *
	 ************************/

	NS_LOG_INFO("Creating the channel...");

	// Create the lora channel object
	Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel>();
	loss->SetPathLossExponent(3.76);
	loss->SetReference(1, 7.7);

	Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();

	Ptr<LoraChannel> channel = CreateObject<LoraChannel>(loss, delay);

	/************************
	 *  Create the helpers  *
	 ************************/

	NS_LOG_INFO("Setting up helpers...");

	MobilityHelper mobility;
	// Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
	// u_int16_t dist=1; //5 metres apart
	// for (int i = 1; i <= nDevices; i++) 
	// {
	// 	allocator->Add(Vector(i * dist, 0, 0));
	// }
	// allocator->Add(Vector(0, 0, 0)); // Gateway
	// mobility.SetPositionAllocator(allocator);
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue(-100.0),
                                 "MinY", DoubleValue(-100.0),
                                 "DeltaX", DoubleValue(5.0),
                                 "DeltaY", DoubleValue(20.0),
                                 "GridWidth", UintegerValue(20),
                                 "LayoutType", StringValue("RowFirst"));
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	// Create the LoraPhyHelper
	LoraPhyHelper phyHelper = LoraPhyHelper();
	phyHelper.SetChannel(channel);

	// Create the LorawanMacHelper
	LorawanMacHelper macHelper = LorawanMacHelper();

	// Create the LoraHelper
	LoraHelper helper = LoraHelper();
	helper.EnablePacketTracking();

	/************************
	 *  Create End Devices  *
	 ************************/

	NS_LOG_INFO("Creating the end device...");

	// Create a set of nodes
	NodeContainer endDevices;
	endDevices.Create(nDevices);

	// Assign a mobility model to the node
	mobility.Install(endDevices);

	// Create a LoraDeviceAddressGenerator
	uint8_t nwkId = 54;
	uint32_t nwkAddr = 1864;
	Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (nwkId,nwkAddr);

	// Create the LoraNetDevices of the end devices
	phyHelper.SetDeviceType(LoraPhyHelper::ED);
	macHelper.SetDeviceType(LorawanMacHelper::ED_A);
	// macHelper.SetRetransMax(0); //0=Disable
	macHelper.SetAddressGenerator (addrGen);
  	macHelper.SetRegion (LorawanMacHelper::EU);
	NetDeviceContainer endDevicesNetDevices = helper.Install(phyHelper, macHelper, endDevices);

	/*********************************************
	 *  Install applications on the end devices  *
	 *********************************************/
	TDMAParams params;
	params.interval = interval; // 3 minutes
	params.deviceType = 1;
	params.mSyncByDevId = mSyncByDevId;
	TDMASenderHelper senderHelper;
	senderHelper.SetTDMAParams(params);
	senderHelper.Install(endDevices);

	/*********************
	 *  Create Gateways  *
	 *********************/

	NS_LOG_INFO("Creating the gateway...");
	NodeContainer gateways;
	gateways.Create(1);
	mobility.Install(gateways);

	// Create a netdevice for each gateway
	phyHelper.SetDeviceType(LoraPhyHelper::GW);
	macHelper.SetDeviceType(LorawanMacHelper::GW);
	helper.Install(phyHelper, macHelper, gateways);

	// params.deviceType = 0;
	// senderHelper.SetTDMAParams(params);
	// senderHelper.Install(gateways);

	/******************
	 * Set Data Rates *
	 ******************/
	std::vector<int> sfQuantity(6);
	sfQuantity = macHelper.SetSpreadingFactorsUp(endDevices, gateways, channel);

	////////////
	// Create NS
	////////////

	NodeContainer networkServers;
	networkServers.Create(1);

	// Install the NetworkServer application on the network server
	NetworkServerHelper networkServerHelper;
	networkServerHelper.SetGateways(gateways);
	networkServerHelper.SetEndDevices(endDevices);
	networkServerHelper.Install(networkServers);

	// Install the Forwarder application on the gateways
	ForwarderHelper forwarderHelper;
	forwarderHelper.Install(gateways);



	// // Activate printing of ED MAC parameters
	// Time stateSamplePeriod = Seconds(0.040);
	// helper.EnablePeriodicDeviceStatusPrinting(endDevices, gateways, "nodeData.txt", stateSamplePeriod);
	// helper.EnablePeriodicPhyPerformancePrinting(gateways, "phyPerformance.txt", stateSamplePeriod);
	// helper.EnablePeriodicGlobalPerformancePrinting("globalPerformance.txt", stateSamplePeriod);

	// LoraPacketTracker &tracker = helper.GetPacketTracker();

	// /************************
	//  * Install Energy Model *
	//  ************************/

	BasicEnergySourceHelper basicSourceHelper;
	LoraRadioEnergyModelHelper radioEnergyHelper;

	// configure energy source
	basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(10000)); // Energy in J
	basicSourceHelper.Set("BasicEnergySupplyVoltageV", DoubleValue(3.3));

	radioEnergyHelper.Set("StandbyCurrentA", DoubleValue(0.0014));
	radioEnergyHelper.Set("TxCurrentA", DoubleValue(0.028));
	radioEnergyHelper.Set("SleepCurrentA", DoubleValue(0.0000015));
	radioEnergyHelper.Set("RxCurrentA", DoubleValue(0.0112));

	radioEnergyHelper.SetTxCurrentModel("ns3::ConstantLoraTxCurrentModel", "TxCurrent", DoubleValue(0.028));

	// install source on EDs' nodes
	EnergySourceContainer sources = basicSourceHelper.Install(endDevices);
	for (uint16_t i = 0; i < nDevices; i++)
	{
		char name[40];
		sprintf(name, "/Names/EnergySourceTDMA%04d", (i + 1));
		Names::Add(name, sources.Get(i));
	}

	// install device model
	DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(endDevicesNetDevices, sources);

	// /**************
	//  * Get output *
	//  **************/

	FileHelper fileHelpers[nDevices];
	for (uint16_t i = 0; i < nDevices; i++)
	{
		char name[60];
		char fname[40];
		sprintf(name, "/Names/EnergySourceTDMA%04d/RemainingEnergy", (i + 1));
		sprintf(fname, "battery-level-%04d", (i + 1));
		fileHelpers[i].ConfigureFile(fname, FileAggregator::SPACE_SEPARATED);
		fileHelpers[i].WriteProbe("ns3::DoubleProbe", name, "Output");
	}

	/****************
	 *  Simulation  *
	 ****************/
	Simulator::Stop(Minutes(nMinutes));

	Simulator::Run();

	Simulator::Destroy();

	// std::cout << tracker.CountMacPacketsGlobally(Seconds(1200 * (nPeriods - 2)), Seconds(1200 * (nPeriods - 1)))<< std::endl;

	return 0;
}