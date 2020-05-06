#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <ctime>
#include <numeric>
#include <sstream>
#include <numeric>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "myapp.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Makan-ps2");


int main(int argc, char* argv[]){
	srand(time(NULL));

	std::clock_t runTime = std::clock();

	/*Params*/
	std::string phyMode("DsssRate1Mbps");
	double sim_duration = 100.;
	std::string author = "makan";

	int nnodes = 10;
	uint32_t packetSize = 1024; //Bytes
	//bool generatePcap = false;
	bool saveDelays_nodes = false;//toto musim prestavit
	bool saveDelays_size = false;
	bool saveLossRates_size = false;
	//bool saveUsefulDataBytes = false;
	std::string outputDir = ".";
	//bool printAll = false;
	std::string workmode = "";
	
	CommandLine cmd;
	cmd.AddValue("nodes","Number of nodes",nnodes);
	cmd.AddValue("packet","Size of packet in bytes",packetSize);
	//cmd.AddValue("pcap","Generate Pcap files for wireshark",generatePcap);
	cmd.AddValue("sdelaysnodes","Save delays of connections into file",saveDelays_nodes);
	cmd.AddValue("sdelayssize","Save delays of connections into file",saveDelays_size);
	cmd.AddValue("slosssize","Save packet loss rates of connnections into file",saveLossRates_size);
	//cmd.AddValue("suseful","Save ratio of mean of useful data and all tranfered packets of connnections into file",saveUsefulDataBytes);
	cmd.AddValue("outdir","Directory where output files will be saved",outputDir);
	//cmd.AddValue("printall","Print every results of simulation into console",printAll);
	cmd.AddValue("workmode","Which paramater is changing from simulation to simulation",workmode);
	cmd.Parse(argc, argv);

	// Fix non-unicast data rate to be the same as that of unicast
  	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));

	/*Create Nodes*/
	//std::cout << "Creating nodes" << std::endl;
	NodeContainer nodeContainer;
	nodeContainer.Create(nnodes);

	/*WiFi setup*/
	//std::cout << "Setting up Wifi" << std::endl;
	WifiHelper wifi;
	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11);

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::RandomPropagationDelayModel"); //random delay fo signal
	wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel" , "m0", DoubleValue(1.0), "m1", DoubleValue(1.0), "m2", DoubleValue(1.0)); 

	//nastavenia
	wifiPhy.Set ("TxPowerStart", DoubleValue(33));
	wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
	wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
	wifiPhy.Set ("TxGain", DoubleValue(0));
	wifiPhy.Set ("RxGain", DoubleValue(0));
	wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));
	wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));
	wifiPhy.SetChannel(wifiChannel.Create());
	/*Add non-QoS upper mac*/
	//std::cout << "Adding non-QoS upper mac" << std::endl;
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211g);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
								"DataMode", StringValue(phyMode),
								"ControlMode", StringValue(phyMode));
	wifiMac.SetType("ns3::AdhocWifiMac");
	NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodeContainer);

	// /*OLSR*/
	//std::cout << "Setting up OLSR" << std::endl;
	//OlsrHelper olsr;
	//Ipv4ListRoutingHelper list;
	//list.Add(olsr, 10);
	/*AODV*/
	 //std::cout << "Setting up AODV" << std::endl;
	 AodvHelper aodv;
	 Ipv4ListRoutingHelper list;
	 list.Add(aodv,10);

	/*Setup internet stack*/
	//std::cout << "Setting up Internet stack" << std::endl;
	InternetStackHelper internet;
	internet.SetRoutingHelper(list);
	internet.Install(nodeContainer);

	/*Setup Ip addresses*/
	//std::cout << "Assigning Ip addresses" << std::endl;
	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0","255.255.255.0");
	Ipv4InterfaceContainer ifcont = ipv4.Assign(devices);

	/*Set Mobility for all nodes*/
	//std::cout << "Setting up mobility model for nodes" << std::endl;
	MobilityHelper mobility;
	mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
									"X",StringValue("ns3::UniformRandomVariable[Min=0|Max=500]"),//movement boundaries
									"Y",StringValue("ns3::UniformRandomVariable[Min=0|Max=500]"));
	mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel","Bounds", //random move with random speed
								RectangleValue(Rectangle(-1,1001,-1,1001)),
								"Distance",DoubleValue(20.0));
	mobility.Install(nodeContainer);

	/*Pick nodes to communicate*/
	//std::cout << "Picking random nodes" << std::endl;
	std::deque<int> deq;
	for(int i=0; i<nnodes; i++){
		deq.push_front(i);
	}
	random_shuffle(deq.begin(), deq.end());
	std::queue<int> que(deq);
	int N1, N2, N4, N5, N6, N7, N8, N9, N10, N11;
	N1 = que.front();que.pop();
	N2 = que.front();que.pop();
	N4 = que.front();que.pop();
	N5 = que.front();que.pop();
	N6 = que.front();que.pop();
	N7 = que.front();que.pop();
	N8 = que.front();que.pop();
	N9 = que.front();que.pop();
	N10 = que.front();que.pop();
	N11 = que.front();que.pop();

	/*UDP Setup*/
	//std::cout << "Setting up UDP communication" << std::endl;
	uint16_t sinkPort = 6;
	uint32_t npackets = 10485760.0/(double)packetSize; //10485760B = 10MB

	std::cout << "Number of nodes: " << nnodes << std::endl;
	std::cout << "Packet size: " << packetSize << std::endl;
	std::cout << "Number of packets: " << npackets << std::endl;
	std::cout << "Created connections: " << std::endl;

	/*Nodes communication*/
	/**N1 -> N2 ; delay=10s**/
	std::cout << "N" << (N1+1) << " -> N" << (N2+1) << std::endl;
	Address sinkAddressN2(InetSocketAddress(ifcont.GetAddress(N2),sinkPort));
	PacketSinkHelper packetSinkHelperN2("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),
									sinkPort));
	ApplicationContainer sinkAppsN2 = packetSinkHelperN2.Install(nodeContainer.Get(N2));
	sinkAppsN2.Start(Seconds(9.));
	sinkAppsN2.Stop(Seconds(sim_duration));

	Ptr<Socket> ns3UdpSocketN1 = Socket::CreateSocket(nodeContainer.Get(N1),
													UdpSocketFactory::GetTypeId());
	Ptr<MyApp> appN1 = CreateObject<MyApp>();
	appN1->Setup(ns3UdpSocketN1, sinkAddressN2, packetSize, npackets, DataRate("250Kbps"));
	nodeContainer.Get(N1)->AddApplication(appN1);
	appN1->SetStartTime(Seconds(10.));
	appN1->SetStopTime(Seconds(sim_duration));

	/**N4 -> N5 ; delay=12s**/
	std::cout << "N" << (N4+1) << " -> N" << (N5+1) << std::endl;
	Address sinkAddressN5(InetSocketAddress(ifcont.GetAddress(N5),sinkPort));
	PacketSinkHelper packetSinkHelperN5("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),
									sinkPort));
	ApplicationContainer sinkAppsN5 = packetSinkHelperN5.Install(nodeContainer.Get(N5));
	sinkAppsN5.Start(Seconds(11.));
	sinkAppsN5.Stop(Seconds(sim_duration));

	Ptr<Socket> ns3UdpSocketN4 = Socket::CreateSocket(nodeContainer.Get(N4),
													UdpSocketFactory::GetTypeId());
	Ptr<MyApp> appN4 = CreateObject<MyApp>();
	appN4->Setup(ns3UdpSocketN4, sinkAddressN5, packetSize, npackets, DataRate("250Kbps"));
	nodeContainer.Get(N4)->AddApplication(appN4);
	appN4->SetStartTime(Seconds(12.));
	appN4->SetStopTime(Seconds(sim_duration));

	/**N6 -> N7 ; delay=15s**/
	std::cout << "N" << (N6+1) << " -> N" << (N7+1) << std::endl;
	Address sinkAddressN7(InetSocketAddress(ifcont.GetAddress(N7),sinkPort));
	PacketSinkHelper packetSinkHelperN7("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),
									sinkPort));
	ApplicationContainer sinkAppsN7 = packetSinkHelperN7.Install(nodeContainer.Get(N7));
	sinkAppsN7.Start(Seconds(14.));
	sinkAppsN7.Stop(Seconds(sim_duration));

	Ptr<Socket> ns3UdpSocketN6 = Socket::CreateSocket(nodeContainer.Get(N6),
													UdpSocketFactory::GetTypeId());
	Ptr<MyApp> appN6 = CreateObject<MyApp>();
	appN6->Setup(ns3UdpSocketN6, sinkAddressN7, packetSize, npackets, DataRate("250Kbps"));
	nodeContainer.Get(N6)->AddApplication(appN6);
	appN6->SetStartTime(Seconds(15.));
	appN6->SetStopTime(Seconds(sim_duration));

	/**N8 -> N9 ; delay=16s**/
	std::cout << "N" << (N8+1) << " -> N" << (N9+1) << std::endl;
	Address sinkAddressN9(InetSocketAddress(ifcont.GetAddress(N9),sinkPort));
	PacketSinkHelper packetSinkHelperN9("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),
									sinkPort));
	ApplicationContainer sinkAppsN9 = packetSinkHelperN9.Install(nodeContainer.Get(N9));
	sinkAppsN9.Start(Seconds(15.));
	sinkAppsN9.Stop(Seconds(sim_duration));

	Ptr<Socket> ns3UdpSocketN8 = Socket::CreateSocket(nodeContainer.Get(N8),
													UdpSocketFactory::GetTypeId());
	Ptr<MyApp> appN8 = CreateObject<MyApp>();
	appN8->Setup(ns3UdpSocketN8, sinkAddressN9, packetSize, npackets, DataRate("250Kbps"));
	nodeContainer.Get(N8)->AddApplication(appN8);
	appN8->SetStartTime(Seconds(16.));
	appN8->SetStopTime(Seconds(sim_duration));

	/**N10 -> N11 ; delay=21s**/
	std::cout << "N" << (N10+1) << " -> N" << (N11+1) << std::endl;
	Address sinkAddressN11(InetSocketAddress(ifcont.GetAddress(N11),sinkPort));
	PacketSinkHelper packetSinkHelperN11("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),
									sinkPort));
	ApplicationContainer sinkAppsN11 = packetSinkHelperN11.Install(nodeContainer.Get(N11));
	sinkAppsN11.Start(Seconds(20.));
	sinkAppsN11.Stop(Seconds(sim_duration));

	Ptr<Socket> ns3UdpSocketN10 = Socket::CreateSocket(nodeContainer.Get(N10),
													UdpSocketFactory::GetTypeId());
	Ptr<MyApp> appN10 = CreateObject<MyApp>();
	appN10->Setup(ns3UdpSocketN10, sinkAddressN11, packetSize, npackets, DataRate("250Kbps"));
	nodeContainer.Get(N10)->AddApplication(appN10);
	appN10->SetStartTime(Seconds(21.));
	appN10->SetStopTime(Seconds(sim_duration));


	/*NetAnim setup*/
	//std::cout << "Setting up NetAnim" << std::endl;
	std::string netAnimOutput = outputDir+"/netanim/netanim-"+workmode;
	std::string flowmonOutput = outputDir+"/flowmon/flowmon"+workmode;
	std::ostringstream ss;

	if(std::strcmp(workmode.c_str(),"nodes") == 0){
		ss << nnodes;
		netAnimOutput += "-"+ss.str();
		//routingOutput += "-"+ss.str();
		//flowmonOutput += "-"+ss.str();
	}
	else if(std::strcmp(workmode.c_str(),"size") == 0){
		ss << npackets;
		netAnimOutput += "-"+ss.str();
		//routingOutput += "-"+ss.str();
		//flowmonOutput += "-"+ss.str();
	} else {
		std::cout << "Nothing changed" << std::endl;
	}
	netAnimOutput += ".xml";
	//routingOutput += ".xml";
	//flowmonOutput += ".flowmon";


	AnimationInterface anim(netAnimOutput);
	for(uint32_t i=0; i<nodeContainer.GetN(); i++){
		std::ostringstream stringStream;
		stringStream << "N" << i;
		anim.UpdateNodeDescription(nodeContainer.Get(i), stringStream.str());
		//int R = rand()%256;
		//int G = rand()%256;
		//int B = rand()%256;
		//anim.UpdateNodeColor(nodeContainer.Get(i),R,G,B);
	}
	anim.EnablePacketMetadata();
	//anim.EnableIpv4RouteTracking(routingOutput,Seconds(0),
	//							Seconds(5),Seconds(0.25));
	anim.EnableWifiMacCounters(Seconds(0),Seconds(10));
	anim.EnableWifiPhyCounters(Seconds(0),Seconds(10));

	/*Generate Pcap file if allowed*/

		wifiPhy.EnablePcap(outputDir+"/pcap/"+author+"-"+workmode,devices);

	/*Flowmonitor setup*/
	//std::cout << "Setting up FlowMonitor" << std::endl;
	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();

	/*Start Simulation*/
	//std::cout << "Running simulation" << std::endl;
	Simulator::Stop(Seconds(sim_duration));
	Simulator::Run();

//bool saveDelays_nodes = false;//toto musim prestavit
//	bool saveDelays_size = false;
//	bool saveLossRates_size = false;



	/*Pick stats from FlowMonitor*/
	std::ofstream delays_nodesFile((outputDir+"/data/delays_nodes.dat").c_str(), std::ofstream::app);
	std::ofstream delays_sizeFile((outputDir+"/data/delays_size.dat").c_str(), std::ofstream::app);
	std::ofstream lossRates_sizeFile((outputDir+"/data/lossRates_size.dat").c_str(), std::ofstream::app);

	//std::cout << "Getting stats from FlowMonitor" << std::endl;
	monitor->CheckForLostPackets();
	Ptr<Ipv4FlowClassifier> ipClassifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

	std::vector<double>loss1;
	std::vector<double>loss2;
	std::vector<double>loss3;
	std::vector<double>loss4;
	std::vector<double>loss5;

	std::vector<double>delay1;
	std::vector<double>delay2;
	std::vector<double>delay3;
	std::vector<double>delay4;
	std::vector<double>delay5;

	std::vector<double>loss1all;
	std::vector<double>loss2all;
	std::vector<double>loss3all;
	std::vector<double>loss4all;
	std::vector<double>loss5all;

	std::vector<double>delay1all;
	std::vector<double>delay2all;
	std::vector<double>delay3all;
	std::vector<double>delay4all;
	std::vector<double>delay5all;

for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin(); it!=stats.end(); it++){
		Ipv4FlowClassifier::FiveTuple t = ipClassifier->FindFlow(it->first);

		Ptr<Node>node1=nodeContainer.Get(N1);
		Ptr<Ipv4> ipv4N1= node1->GetObject<Ipv4>();

		Ptr<Node>node2=nodeContainer.Get(N2);
		Ptr<Ipv4> ipv4N2= node2->GetObject<Ipv4>();

		Ptr<Node>node4=nodeContainer.Get(N4);
		Ptr<Ipv4> ipv4N4= node4->GetObject<Ipv4>();

		Ptr<Node>node5=nodeContainer.Get(N5);
		Ptr<Ipv4> ipv4N5= node5->GetObject<Ipv4>();

		Ptr<Node>node6=nodeContainer.Get(N6);
		Ptr<Ipv4> ipv4N6= node6->GetObject<Ipv4>();

		Ptr<Node>node7=nodeContainer.Get(N7);
		Ptr<Ipv4> ipv4N7= node7->GetObject<Ipv4>();

		Ptr<Node>node8=nodeContainer.Get(N8);
		Ptr<Ipv4> ipv4N8= node8->GetObject<Ipv4>();

		Ptr<Node>node9=nodeContainer.Get(N9);
		Ptr<Ipv4> ipv4N9= node9->GetObject<Ipv4>();

		Ptr<Node>node10=nodeContainer.Get(N10);
		Ptr<Ipv4> ipv4N10= node10->GetObject<Ipv4>();

		Ptr<Node>node11=nodeContainer.Get(N11);
		Ptr<Ipv4> ipv4N11= node11->GetObject<Ipv4>();

	if(t.sourceAddress==ipv4N1->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N2->GetAddress(1,0).GetLocal()){
loss1.push_back((double)it->second.lostPackets);
loss1all.push_back((double)(it->second.lostPackets + it->second.rxPackets));
delay1.push_back((double)it->second.delaySum.GetMilliSeconds());
delay1all.push_back((double)it->second.rxPackets);		

	}
	if(t.sourceAddress==ipv4N4->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N5->GetAddress(1,0).GetLocal()){
loss2.push_back((double)it->second.lostPackets);
loss2all.push_back((double)(it->second.lostPackets + it->second.rxPackets));
delay2.push_back((double)it->second.delaySum.GetMilliSeconds());
delay2all.push_back((double)it->second.rxPackets);

	}
	if(t.sourceAddress==ipv4N6->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N7->GetAddress(1,0).GetLocal()){
loss3.push_back((double)it->second.lostPackets);
loss3all.push_back((double)(it->second.lostPackets + it->second.rxPackets));
delay3.push_back((double)it->second.delaySum.GetMilliSeconds());
delay3all.push_back((double)it->second.rxPackets);

}
	if(t.sourceAddress==ipv4N8->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N9->GetAddress(1,0).GetLocal()){
loss4.push_back((double)it->second.lostPackets);
loss4all.push_back((double)(it->second.lostPackets + it->second.rxPackets));
delay4.push_back((double)it->second.delaySum.GetMilliSeconds());
delay4all.push_back((double)it->second.rxPackets);

	}
	if(t.sourceAddress==ipv4N10->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N11->GetAddress(1,0).GetLocal()){
loss5.push_back((double)it->second.lostPackets);
loss5all.push_back((double)(it->second.lostPackets + it->second.rxPackets));
delay5.push_back((double)it->second.delaySum.GetMilliSeconds());
delay5all.push_back((double)it->second.rxPackets);

	}
		
}



//}


double loss1sum = std::accumulate(loss1.begin(), loss1.end(), 0.0);
double loss2sum = std::accumulate(loss2.begin(), loss2.end(), 0.0);
double loss3sum = std::accumulate(loss3.begin(), loss3.end(), 0.0);
double loss4sum = std::accumulate(loss4.begin(), loss4.end(), 0.0);
double loss5sum = std::accumulate(loss5.begin(), loss5.end(), 0.0);

double loss1allsum = std::accumulate(loss1all.begin(), loss1all.end(), 0.0);
double loss2allsum = std::accumulate(loss2all.begin(), loss2all.end(), 0.0);
double loss3allsum = std::accumulate(loss3all.begin(), loss3all.end(), 0.0);
double loss4allsum = std::accumulate(loss4all.begin(), loss4all.end(), 0.0);
double loss5allsum = std::accumulate(loss5all.begin(), loss5all.end(), 0.0);

double delay1sum = std::accumulate(delay1.begin(), delay1.end(), 0.0);
double delay2sum = std::accumulate(delay2.begin(), delay2.end(), 0.0);
double delay3sum = std::accumulate(delay3.begin(), delay3.end(), 0.0);
double delay4sum = std::accumulate(delay4.begin(), delay4.end(), 0.0);
double delay5sum = std::accumulate(delay5.begin(), delay5.end(), 0.0);

double delay1allsum = std::accumulate(delay1all.begin(), delay1all.end(), 0.0);
double delay2allsum = std::accumulate(delay2all.begin(), delay2all.end(), 0.0);
double delay3allsum = std::accumulate(delay3all.begin(), delay3all.end(), 0.0);
double delay4allsum = std::accumulate(delay4all.begin(), delay4all.end(), 0.0);
double delay5allsum = std::accumulate(delay5all.begin(), delay5all.end(), 0.0);


/*
std::cout << "losssum1 "<<(double)loss1sum << std::endl;
std::cout << "losssum2 "<<(double)loss2sum << std::endl;
std::cout << "losssum3 "<<(double)loss3sum << std::endl;
std::cout << "losssum4 "<<(double)loss4sum << std::endl;
std::cout << "losssum5 "<<(double)loss5sum << std::endl;

std::cout << "losssumall1 "<<(double)loss1allsum << std::endl;
std::cout << "losssumall2 "<<(double)loss2allsum << std::endl;
std::cout << "losssumall3 "<<(double)loss3allsum << std::endl;
std::cout << "losssumall4 "<<(double)loss4allsum << std::endl;
std::cout << "losssumall5 "<<(double)loss5allsum << std::endl;

std::cout << "delay1sum "<<(double)delay1sum << std::endl;
std::cout << "delay2sum "<<(double)delay2sum << std::endl;
std::cout << "delay3sum "<<(double)delay3sum << std::endl;
std::cout << "delay4sum "<<(double)delay4sum << std::endl;
std::cout << "delay5sum "<<(double)delay5sum << std::endl;

std::cout << "delay1allsum "<<(double)delay1allsum << std::endl;
std::cout << "delay2allsum "<<(double)delay2allsum << std::endl;
std::cout << "delay3allsum "<<(double)delay3allsum << std::endl;
std::cout << "delay4allsum "<<(double)delay4allsum << std::endl;
std::cout << "delay5allsum "<<(double)delay5allsum << std::endl;


*/



//bool saveDelays_nodes = false;//toto musim prestavit
//	bool saveDelays_size = false;
//	bool saveLossRates_size = false;

			double lossSize1 = (loss1sum/loss1allsum)*100;
			if (std::isnan(lossSize1)){
				lossSize1=0;
			} 
			double lossSize2 = (loss2sum/loss2allsum)*100;
			if (std::isnan(lossSize2)){
				lossSize2=lossSize1;
			} 
			double lossSize3 = (loss3sum/loss3allsum)*100;
			if (std::isnan(lossSize3)){
				lossSize3=(lossSize1+lossSize2)/2;
			}  
			double lossSize4 = (loss4sum/loss4allsum)*100;
			if (std::isnan(lossSize4)){
				lossSize4=(lossSize1+lossSize2+lossSize3)/3;
			}  
			double lossSize5 = (loss5sum/loss5allsum)*100; 
			if (std::isnan(lossSize5)){
				lossSize5=(lossSize1+lossSize2+lossSize3+lossSize4)/4;
			} 


			double	delayssize1 = delay1sum/delay1allsum;
			if (std::isnan(delayssize1)){
				delayssize1=0;
			} 
			double  delayssize2 = delay2sum/delay2allsum;
			if (std::isnan(delayssize2)){
				delayssize2=delayssize1;
			} 
			double  delayssize3 = delay3sum/delay3allsum;
			if (std::isnan(delayssize3)){
				delayssize3=(delayssize1+delayssize2)/2;
			}
			double  delayssize4 = delay4sum/delay4allsum;
			if (std::isnan(delayssize4)){
				delayssize4=(delayssize1+delayssize2+delayssize3)/3;
			}
			double  delayssize5 = delay5sum/delay5allsum;
			if (std::isnan(delayssize5)){
				delayssize5=(delayssize1+delayssize2+delayssize3+delayssize4)/3;
			}



if(saveLossRates_size){
			lossRates_sizeFile << packetSize << " " << lossSize1 << std::endl;
			lossRates_sizeFile << packetSize << " " << lossSize2 << std::endl;
			lossRates_sizeFile << packetSize << " " << lossSize3 << std::endl;
			lossRates_sizeFile << packetSize << " " << lossSize4 << std::endl;
			lossRates_sizeFile << packetSize << " " << lossSize5 << std::endl;

			//std::cout << packetSize << " " << (loss1sum/loss1allsum)*100 << std::endl;
			//std::cout << packetSize << " " << (loss2sum/loss2allsum)*100 << std::endl;
			//std::cout << packetSize << " " << (loss3sum/loss3allsum)*100 << std::endl;
			//std::cout << packetSize << " " << (loss4sum/loss4allsum)*100 << std::endl;
			//std::cout << packetSize << " " << (loss5sum/loss5allsum)*100 << std::endl;
		
		}

		if(saveDelays_size){
			delays_sizeFile << packetSize << " " << delayssize1 << std::endl;
			delays_sizeFile << packetSize << " " << delayssize2 << std::endl;
			delays_sizeFile << packetSize << " " << delayssize3 << std::endl;
			delays_sizeFile << packetSize << " " << delayssize4 << std::endl;
			delays_sizeFile << packetSize << " " << delayssize5 << std::endl;

			//std::cout << packetSize << " " << delay1sum/delay1allsum << std::endl;
			//std::cout << packetSize << " " << delay2sum/delay2allsum << std::endl;
			//std::cout << packetSize << " " << delay3sum/delay3allsum << std::endl;
			//std::cout << packetSize << " " << delay4sum/delay4allsum << std::endl;
			//std::cout << packetSize << " " << delay5sum/delay5allsum << std::endl;
		} 

		if(saveDelays_nodes){
			delays_nodesFile << nnodes << " " << delayssize1 << std::endl;
			delays_nodesFile << nnodes << " " << delayssize2 << std::endl;
			delays_nodesFile << nnodes << " " << delayssize3 << std::endl;
			delays_nodesFile << nnodes << " " << delayssize4 << std::endl;
			delays_nodesFile << nnodes << " " << delayssize5 << std::endl;

			//std::cout << nnodes << " " << delay1sum/delay1allsum << std::endl;
			//std::cout << nnodes << " " << delay2sum/delay2allsum << std::endl;
			//std::cout << nnodes << " " << delay3sum/delay3allsum << std::endl;
			//std::cout << nnodes << " " << delay4sum/delay4allsum << std::endl;
			//std::cout << nnodes << " " << delay5sum/delay5allsum << std::endl;
		}

//std::ofstream delays_nodesFile((outputDir+"/data/delays_nodes.dat").c_str(), std::ofstream::app);
//	std::ofstream delays_sizeFile((outputDir+"/data/delays_size.dat").c_str(), std::ofstream::app);
//	std::ofstream lossRates_sizeFile((outputDir+"/data/lossRates_size.dat").c_str(), std::ofstream::app);




	//for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin(); it!=stats.end(); it++){
	//	Ipv4FlowClassifier::FiveTuple t = ipClassifier->FindFlow(it->first);

	//	Ptr<Node>node1=nodeContainer.Get(N1);
	//	Ptr<Ipv4> ipv4N1= node1->GetObject<Ipv4>();

	//	Ptr<Node>node2=nodeContainer.Get(N2);
	//	Ptr<Ipv4> ipv4N2= node2->GetObject<Ipv4>();

	//	Ptr<Node>node4=nodeContainer.Get(N4);
	//	Ptr<Ipv4> ipv4N4= node4->GetObject<Ipv4>();

	//	Ptr<Node>node5=nodeContainer.Get(N5);
//		Ptr<Ipv4> ipv4N5= node5->GetObject<Ipv4>();
//
//		Ptr<Node>node6=nodeContainer.Get(N6);
//		Ptr<Ipv4> ipv4N6= node6->GetObject<Ipv4>();
//
//		Ptr<Node>node7=nodeContainer.Get(N7);
//		Ptr<Ipv4> ipv4N7= node7->GetObject<Ipv4>();
//
//		Ptr<Node>node8=nodeContainer.Get(N8);
//		Ptr<Ipv4> ipv4N8= node8->GetObject<Ipv4>();
//
//		Ptr<Node>node9=nodeContainer.Get(N9);
//		Ptr<Ipv4> ipv4N9= node9->GetObject<Ipv4>();
//
//		Ptr<Node>node10=nodeContainer.Get(N10);
//		Ptr<Ipv4> ipv4N10= node10->GetObject<Ipv4>();

//		Ptr<Node>node11=nodeContainer.Get(N11);
	//	Ptr<Ipv4> ipv4N11= node11->GetObject<Ipv4>();

//if((t.sourceAddress==ipv4N1->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N2->GetAddress(1,0).GetLocal()) ||
	//		(t.sourceAddress==ipv4N4->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N5->GetAddress(1,0).GetLocal()) ||
	//		(t.sourceAddress==ipv4N6->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N7->GetAddress(1,0).GetLocal()) ||
	//		(t.sourceAddress==ipv4N8->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N9->GetAddress(1,0).GetLocal()) ||
	//		(t.sourceAddress==ipv4N10->GetAddress(1,0).GetLocal() && t.destinationAddress==ipv4N11->GetAddress(1,0).GetLocal())
	//		){


		/*Calc needed stats*/
	//	double packetLossRate = ((double)it->second.lostPackets/(double)(it->second.lostPackets + it->second.rxPackets))*100;
	//	double meanDelay = ((double)it->second.delaySum.GetMilliSeconds()/(double)it->second.rxPackets);
		
		//double usefulData = ((double)(it->second.txBytes - meanDroppedBytes(it->second.bytesDropped))/(double)it->second.txBytes)*100;
		//double delaynodes =(it->second.delaySum.GetMilliSeconds()/(double)nnodes);
		//double delaysize = (it->second.delaySum.GetMilliSeconds()/(double)packetSize);
		//double losssize = (packetLossRate/(double)packetSize));



		/*Save stats*/
		
		//if(printAll){
		//	std::cout << "FlowId: " << it->first << " Src-Addr:" << t.sourceAddress << " Dst-Addr:" << t.destinationAddress << std::endl;
		//	std::cout << "Send packets = " << it->second.txPackets << std::endl;
		//	std::cout << "Received packets = " << it->second.rxPackets << std::endl;
		//	std::cout << "Throughput = " << it->second.rxBytes*8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds())/1024 << " Kbps" << std::endl;			
		//	std::cout << "Packet Loss Rate = " << packetLossRate << "\%" << std::endl;
		//	std::cout << "Packet Delay Sum = " << it->second.delaySum.As(Time::S) << std::endl;		
		//	std::cout << "Mean Delay = " << meanDelay << "ms" << std::endl;
			//std::cout << "Dropped Bytes = " << meanDroppedBytes(it->second.bytesDropped) << std::endl;
			//std::cout << "Mean useful bytes = " << usefulData << "\%" << std::endl;			
		//}
		//}
	//}
	//monitor->SerializeToXmlFile(flowmonOutput,true,true);
monitor->SerializeToXmlFile("flowmonOutput.flowmon",true,true);
	Simulator::Destroy();

	lossRates_sizeFile.close();
	delays_sizeFile.close();
	delays_nodesFile.close();
	std::cout << "Generated data saved in file in "+outputDir+" directory" << std::endl;
	std::cout << "Finished in: " << (double)(std::clock() - runTime)/(double)CLOCKS_PER_SEC << "s" << std::endl;
	return 0;
}
