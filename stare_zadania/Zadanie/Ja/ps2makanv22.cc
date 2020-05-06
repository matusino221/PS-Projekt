#include <queue>#include <iostream>#include <stdio.h>#include <vector>#include <ctime>#include <fstream>#include "ns3/core-module.h"#include "ns3/network-module.h"#include "ns3/internet-module.h"#include "ns3/applications-module.h"#include "ns3/mobility-module.h"#include "ns3/wifi-module.h"#include "ns3/aodv-module.h"#include "ns3/flow-monitor-module.h"#include "ns3/mobility-module.h"#include "myapp.h"#include "ns3/netanim-module.h"#include "ns3/propagation-loss-model.h"//#include "ns3/nakagami-propagation-loss-model.h"using namespace ns3;NS_LOG_COMPONENT_DEFINE ("makan_ps2");int main(int argc, char* argv[]) {	srand(time(NULL));	int nnodes = 16;	uint32_t packetSize = 1024; //Bytes	std::string phyMode("DsssRate1Mbps");	bool generatePcap = false;	double sim_duration = 100.;	CommandLine cmd;	cmd.AddValue("nodes","Number of nodes",nnodes);	cmd.AddValue("packet","Size of packet in bytes",packetSize);	cmd.AddValue("pcap","Generate Pcap files for wireshark",generatePcap);	cmd.Parse(argc, argv);	NS_LOG_INFO ("Create nodes.");	NodeContainer nodeContainer;	nodeContainer.Create(nnodes);//vytvorenie poctu nodov	// Set up WiFi	WifiHelper wifi;	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);	YansWifiChannelHelper wifiChannel;	wifiChannel.SetPropagationDelay ("ns3::RandomPropagationDelayModel"); // n�hodn� oneskorenia sign�lu	wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel" , "m0", DoubleValue(1.0), "m1", DoubleValue(1.0), "m2", DoubleValue(1.0)); 	wifiPhy.Set ("TxPowerStart", DoubleValue(33));	wifiPhy.Set ("TxPowerEnd", DoubleValue(33));	wifiPhy.Set ("TxPowerLevels", UintegerValue(1));	wifiPhy.Set ("TxGain", DoubleValue(0));	wifiPhy.Set ("RxGain", DoubleValue(0));	wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));	wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));	wifiPhy.SetChannel (wifiChannel.Create ());		// Add a non-QoS upper mac	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();	wifiMac.SetType ("ns3::AdhocWifiMac");	wifi.SetStandard (WIFI_PHY_STANDARD_80211g);	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",StringValue(phyMode), "ControlMode",StringValue(phyMode));	NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodeContainer);	// AODV	AodvHelper aodv;	Ipv4ListRoutingHelper list;	list.Add (aodv, 10);	// Set up internet stack	InternetStackHelper internet;	internet.SetRoutingHelper (list);	internet.Install (nodeContainer);	// Set up Addresses	Ipv4AddressHelper ipv4;	 NS_LOG_INFO ("Assign IP Addresses.");	ipv4.SetBase ("10.1.1.0", "255.255.255.0");	Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);	NS_LOG_INFO ("Create Applications.");		std::deque<int> d;	for (int i = 0; i < nnodes; i++)		d.push_front(i);	std::random_shuffle(d.begin(), d.end());	std::queue<int> q(d);	int N1, N2, N4, N5, N6, N7, N8, N9, N10, N11;	N1 = q.front();q.pop();	N2 = q.front();q.pop();	N4 = q.front();q.pop();	N5 = q.front();q.pop();	N6 = q.front();q.pop();	N7 = q.front();q.pop();	N8 = q.front();q.pop();	N9 = q.front();q.pop();	N10 = q.front();q.pop();	N11 = q.front();q.pop();	std::cout << "Creating application layer" << std::endl;		uint32_t npackets = 10485760.0/(double)packetSize; //10485760B = 10MB	uint16_t sinkPort = 6;			std::cout << "Sending from N" << (N1+1) << " to N" << (N2+1) << std::endl;	Address sinkAddressN2(InetSocketAddress(ifcont.GetAddress(N2),sinkPort));	PacketSinkHelper packetSinkHelperN2("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));	ApplicationContainer sinkAppsN2 = packetSinkHelperN2.Install(nodeContainer.Get(N2));	sinkAppsN2.Start(Seconds(0.));	sinkAppsN2.Stop(Seconds(sim_duration));	Ptr<Socket> ns3UdpSocketN1 = Socket::CreateSocket(nodeContainer.Get(N1), UdpSocketFactory::GetTypeId());	Ptr<MyApp> appN1 = CreateObject<MyApp>();	appN1->Setup(ns3UdpSocketN1, sinkAddressN2, packetSize, npackets, DataRate("250Kbps"));	nodeContainer.Get(N1)->AddApplication(appN1);	appN1->SetStartTime(Seconds(10.));	appN1->SetStopTime(Seconds(sim_duration));		std::cout << "Sending from N" << (N4+1) << " to N" << (N5+1) << std::endl;	Address sinkAddressN5(InetSocketAddress(ifcont.GetAddress(N5),sinkPort));	PacketSinkHelper packetSinkHelperN5("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));	ApplicationContainer sinkAppsN5 = packetSinkHelperN5.Install(nodeContainer.Get(N5));	sinkAppsN5.Start(Seconds(0.));	sinkAppsN5.Stop(Seconds(sim_duration));	Ptr<Socket> ns3UdpSocketN4 = Socket::CreateSocket(nodeContainer.Get(N4), UdpSocketFactory::GetTypeId());	Ptr<MyApp> appN4 = CreateObject<MyApp>();	appN4->Setup(ns3UdpSocketN4, sinkAddressN5, packetSize, npackets, DataRate("250Kbps"));	nodeContainer.Get(N4)->AddApplication(appN4);	appN4->SetStartTime(Seconds(12.));	appN4->SetStopTime(Seconds(sim_duration));		std::cout << "Sending from N" << (N6+1) << " to N" << (N7+1) << std::endl;	Address sinkAddressN7(InetSocketAddress(ifcont.GetAddress(N7),sinkPort));	PacketSinkHelper packetSinkHelperN7("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));	ApplicationContainer sinkAppsN7 = packetSinkHelperN7.Install(nodeContainer.Get(N7));	sinkAppsN7.Start(Seconds(0.));	sinkAppsN7.Stop(Seconds(sim_duration));	Ptr<Socket> ns3UdpSocketN6 = Socket::CreateSocket(nodeContainer.Get(N6), UdpSocketFactory::GetTypeId());	Ptr<MyApp> appN6 = CreateObject<MyApp>();	appN6->Setup(ns3UdpSocketN6, sinkAddressN7, packetSize, npackets, DataRate("250Kbps"));	nodeContainer.Get(N6)->AddApplication(appN6);	appN6->SetStartTime(Seconds(15.));	appN6->SetStopTime(Seconds(sim_duration));	std::cout << "Sending from N" << (N8+1) << " to N" << (N9+1) << std::endl;	Address sinkAddressN9(InetSocketAddress(ifcont.GetAddress(N9),sinkPort));	PacketSinkHelper packetSinkHelperN9("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));	ApplicationContainer sinkAppsN9 = packetSinkHelperN9.Install(nodeContainer.Get(N9));	sinkAppsN9.Start(Seconds(0.));	sinkAppsN9.Stop(Seconds(sim_duration));	Ptr<Socket> ns3UdpSocketN8 = Socket::CreateSocket(nodeContainer.Get(N8),	UdpSocketFactory::GetTypeId());	Ptr<MyApp> appN8 = CreateObject<MyApp>();	appN8->Setup(ns3UdpSocketN8, sinkAddressN9, packetSize, npackets, DataRate("250Kbps"));	nodeContainer.Get(N8)->AddApplication(appN8);	appN8->SetStartTime(Seconds(16.));	appN8->SetStopTime(Seconds(sim_duration));		std::cout << "Sending from N" << (N10+1) << " to N" << (N11+1) << std::endl;	Address sinkAddressN11(InetSocketAddress(ifcont.GetAddress(N11),sinkPort));	PacketSinkHelper packetSinkHelperN11("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));	ApplicationContainer sinkAppsN11 = packetSinkHelperN11.Install(nodeContainer.Get(N11));	sinkAppsN11.Start(Seconds(0.));	sinkAppsN11.Stop(Seconds(sim_duration));	Ptr<Socket> ns3UdpSocketN10 = Socket::CreateSocket(nodeContainer.Get(N10), UdpSocketFactory::GetTypeId());	Ptr<MyApp> appN10 = CreateObject<MyApp>();	appN10->Setup(ns3UdpSocketN10, sinkAddressN11, packetSize, npackets, DataRate("250Kbps"));	nodeContainer.Get(N10)->AddApplication(appN10);	appN10->SetStartTime(Seconds(21.));	appN10->SetStopTime(Seconds(sim_duration));	std::cout << "Creating mobility" << std::endl;	// Set Mobility for all nodes	MobilityHelper mobility;	mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",                                 "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=500]"),                                 "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=500]"));	mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Rectangle (-1, 1001, -1, 1001)), "Distance", DoubleValue(20.0)); 	mobility.Install(nodeContainer);	std::cout << "Simulation stop" << std::endl;	Simulator::Stop (Seconds(100.0));	std::cout << "Creating visualization" << std::endl;	AnimationInterface anim ("makan.xml"); // Mandatory	for (uint32_t i = 0; i < nodeContainer.GetN(); ++i){		std::ostringstream stringStream;		stringStream << "N" << i;		anim.UpdateNodeDescription (nodeContainer.Get (i), stringStream.str()); // Optional		anim.UpdateNodeColor (nodeContainer.Get (i), 255, 0, 0); // Optional	}	anim.EnablePacketMetadata (); // Optional	anim.EnableIpv4RouteTracking ("routingtable-wireless.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional	anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional	anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optionalif(generatePcap){	wifiPhy.EnablePcap ("makan-dev", devices);}	NS_LOG_INFO ("Run Simulation.");std::cout << "Setting up FlowMonitor" << std::endl;	FlowMonitorHelper flowmon;	Ptr<FlowMonitor> monitor = flowmon.InstallAll();	/*Start Simulation*/	std::cout << "Running simulation" << std::endl;	Simulator::Stop(Seconds(sim_duration));	Simulator::Run();	/*Pick stats from FlowMonitor*/	std::cout << "Getting stats from FlowMonitor" << std::endl;	monitor->CheckForLostPackets();	Ptr<Ipv4FlowClassifier> ipClassifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();	for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin(); it!=stats.end(); it++){		Ipv4FlowClassifier::FiveTuple t = ipClassifier->FindFlow(it->first);		std::cout << "FlowId: " << it->first << " Src-Addr:" << t.sourceAddress << " Dst-Addr:" << t.destinationAddress << std::endl;		std::cout << "Send packets = " << it->second.txPackets << std::endl;		std::cout << "Received packets = " << it->second.rxPackets << std::endl;		std::cout << "Throughput = " << it->second.rxBytes*8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds())/1024 << " Kbps" << std::endl;		std::cout << "Packet Loss Rate = " << ((it->second.txPackets - it->second.rxPackets)*1.0)/it->second.txPackets << "\%" << std::endl;		std::cout << "Packet Delay Sum = " << it->second.delaySum.As(Time::S) << std::endl;	}	monitor->SerializeToXmlFile("makan-ps2.flowmon",true,true);	Simulator::Destroy();	std::cout << "End" << std::endl;	return 0;}