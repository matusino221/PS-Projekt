#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/energy-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-radio-energy-model-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <numeric>

using namespace ns3;
using namespace std;
//https://www.nsnam.org/doxygen/energy-model-with-harvesting-example_8cc.html

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
long counter_data3_x = 0;
long counter_data4_x = 0;
long counter_data5_x = 0;
long counter_data6_x = 0;
float graphArray [4];

/*Cal mean from all dropped bytes*/
uint64_t meanDroppedBytes(std::vector<uint64_t> dropped){
	if(dropped.size() == 0) return 0;
	uint64_t sum = accumulate(dropped.begin(),dropped.end(),0);
	return (uint64_t)((double)sum/(double)dropped.size());
}

inline string PrintReceivedPacket(const Address &from)
{
    InetSocketAddress iaddr = InetSocketAddress::ConvertFrom(from);

    ostringstream oss;
    oss << "- nReceived jeden paket! Socket: " << iaddr.GetIpv4() << " port: " << iaddr.GetPort () << " at time = " << Simulator::Now().GetSeconds() << "n--";
    return oss.str();
}

/**
param socket Pointer na zásuvku.
*
* Paket príjem drez.
*/
void ReceivePacket(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    while((packet = socket->RecvFrom(from)))
    {
        if (packet->GetSize() > 0)
        {
            //NS_LOG_UNCOND(PrintReceivedPacket(from));
            globalCounter += packet->GetSize();
            globalCounter2++;
        }
    }
}

/**
param socket Pointer na zásuvku.
veľkosť pktSize Veľkosť paketu.
* n n Ukazovateľ na uzol.
* Param pktCount Počet paketov, ktoré sa majú vygenerovať.
* Param pktInterval Interval odosielania paketov.
*
* Generátor prevádzky.
*/
static void GenerateTraffic (Ptr <Socket> socket, uint32_t pktSize, Ptr <Node> n,uint32_t pktCount, Time pktInterval)
{
    if (pktCount> 0)
    {
        socket->Send(Create <Packet> (pktSize));
        Simulator::Schedule(pktInterval, &GenerateTraffic, socket, pktSize, n,
                pktCount - 1, pktInterval);
    }
    else
    {
        socket->Close();
    }
}

///Funkcia sledovania zostávajúcej energie v uzle.
void RemainingEnergy(double oldValue, double remainingEnergy)
{
    /*cout << Simulator::Now().GetSeconds()
        << "s Aktuálna zostávajúca energia =" << remainingEnergy << "J" <<  endl;*/
    if (counter_data3_x % 3125 == 0)
    {
        data3.Add(Simulator::Now().GetSeconds(), remainingEnergy);
    }
    counter_data3_x ++;
}

///Funkcia sledovania celkovej spotreby energie v uzle.
void TotalEnergy(double oldValue, double totalEnergy)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" <<  endl;*/
    if (counter_data4_x % 3125 == 0)
    {
        data4.Add(Simulator::Now().GetSeconds(), totalEnergy);
    }
    counter_data4_x ++;
}

///Stopová funkcia pre energiu zberanú energetickým kombajnom.
void HarvestedPower(double oldValue, double harvestedPower)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" <<  endl;*/
    if (counter_data5_x % 3125 == 0)
    {
        data5.Add(Simulator::Now().GetSeconds(), harvestedPower);
    }
    counter_data5_x ++;
}

///Stopová funkcia celkovej energie zozbieranej uzlom.
void TotalEnergyHarvested(double oldValue, double TotalEnergyHarvested)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Celková energia zozbieraná ="
                << TotalEnergyHarvested << "J" <<  endl;*/
    if (counter_data6_x % 3125 == 0)
    {
        data6.Add(Simulator::Now().GetSeconds(), TotalEnergyHarvested);
    }
    counter_data6_x ++;
}

void standardDeviation(float data[], float temp[]) {
    double sum = 0.0, mean, standardDeviation = 0.0;
    int i;

    for (i = 0; i < 5; ++ i) {
        sum += data[i];
    }
    mean = sum / 5;

    for (i = 0; i <5; ++ i)
        standardDeviation += pow(data[i] - mean, 2);

    temp[0] = mean;//stredná hodnota;
    temp[1] = sqrt(standardDeviation / 5);

}

static void goToNewPosition(Ptr<Node> hosp, Gnuplot2dDataset* data, Gnuplot2dDataset* data2, int k) {
    MobilityHelper mobility;
    int strana = rand() % 4;
    int poloha = rand() % 250;
    Ptr <ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    if (strana == 0) {
        positionAlloc->Add(Vector(0.0, poloha, 0.0));
    }
    else if (strana == 1) {
        positionAlloc->Add(Vector(poloha, 0.0, 0.0));
    }
    else if (strana == 2) {
        positionAlloc->Add(Vector(250.0, poloha, 0.0));
    }
    else if (strana == 3) {
        positionAlloc->Add(Vector(poloha, 250.0, 0.0));
    }

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(hosp);

    graphArray [k] = globalCounter;
    globalCounter = 0;
    if (k == 4) {
        float tmpGraphArray[2];
        standardDeviation(graphArray, tmpGraphArray);
        if (tmpGraphArray[0] != 0 && tmpGraphArray[1] != 0)
        data->Add(Simulator::Now().GetSeconds(), tmpGraphArray[0], 0.0, tmpGraphArray[1]);
    }
    data2->Add(Simulator::Now().GetSeconds(), globalCounter2);

}

int main(int argc, char * argv [])
{
    uint32_t numNodes = 25; // 5x5
    double distance = 62.5; // [m] vzdialenost medzi uzlami asi
    string phyMode("DsssRate1Mbps");
    double Prss = -20; // dBm
    uint32_t PpacketSize = 200; // bajty
    bool verbose = false; //false

    // simulačné parametre
    uint32_t numPackets = 10000; // počet paketov, ktoré sa majú odoslať
    double interval = 1.0; // sekúnd
    double startTime = 0.0; // sekúnd
    double distanceToRx = 10.0; // metre
    double offset = 81.0;

    // Premenné energetického kombajnu
    double harvestingUpdateInterval = 1; // sekúnd

    Gnuplot graf("graf_pole.svg");
    graf.SetTerminal( "svg");
    graf.SetTitle("Graf zavislosti suctu velkosti prijatých paketov od casu");
    graf.SetLegend("Cas [s]", "SUM velkosti prijatych paketov [B]");
    Gnuplot2dDataset data;
    data.SetTitle("Vysledok");
    data.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    data.SetErrorBars(Gnuplot2dDataset::XY);

    Gnuplot graf2("graf_pole2.svg");
    graf2.SetTerminal("svg");
    graf2.SetTitle("Graf zavislosti poctu prijatých paketov od casu");
    graf2.SetLegend("Cas [s]", "Pocet přijatých paketov");
    Gnuplot2dDataset data2;
    data2.SetTitle("Vysledok");
    data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    //data2.SetErrorBars(Gnuplot2dDataset::XY);

    Gnuplot graf3("graf_pole3.svg");
    graf3.SetTerminal("svg");
    graf3.SetTitle("Graf zavislosti zostatku energie od casu.");
    graf3.SetLegend("Cas [s]", "Zostavajuca energia [J]");
    data3.SetTitle("Vysledok");
    data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

    Gnuplot graf4("graf_pole4.svg");
    graf4.SetTerminal("svg");
    graf4.SetTitle("Graf zavislosti celkovej spotreby energie od casu.");
    graf4.SetLegend("Cas [s]", "Celkova spotreba energie [J]");
    data4.SetTitle("Vysledok");
    data4.SetStyle(Gnuplot2dDataset::LINES_POINTS);

    Gnuplot graf5("graf_pole5.svg");
    graf5.SetTerminal("svg");
    graf5.SetTitle("Graf zavislosti zozbieranej energie od casu.");
    graf5.SetLegend("Cas [s]", "Zozbierana energia [W]");
    data5.SetTitle("Vysledok");
    data5.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    
    Gnuplot graf6("graf_pole6.svg");
    graf6.SetTerminal("svg");
    graf6.SetTitle("Graf zavislosti celkovej zozbieranej energie od casu.");
    graf6.SetLegend("Cas [s]", "Celkova zozbierana energia [J]");
    data6.SetTitle("Vysledok");
    data6.SetStyle(Gnuplot2dDataset :: LINES_POINTS);
    
    Gnuplot graf7("graf_pole7.svg");
    graf7.SetTerminal("svg");
    graf7.SetTitle("Graf zavislosti Throughput od node.");
    graf7.SetLegend("Node", "Throughput");
    Gnuplot2dDataset data7;
    data7.SetTitle("Vysledok");
    data7.SetStyle(Gnuplot2dDataset :: LINES_POINTS);
    
    Gnuplot graf8("graf_pole8.svg");
    graf8.SetTerminal("svg");
    graf8.SetTitle("Graf zavislosti sumy oneskorenia paketov od node.");
    graf8.SetLegend("Node", "suma oneskorenia paketov [s]");
    Gnuplot2dDataset data8;
    data8.SetTitle("Vysledok");
    data8.SetStyle(Gnuplot2dDataset :: LINES_POINTS);
    
    Gnuplot graf9("graf_pole9.svg");
    graf9.SetTerminal("svg");
    graf9.SetTitle("Graf zavislosti priemerneho oneskorenia od node.");
    graf9.SetLegend("Node", "priemerneho oneskorenia [ms]");
    Gnuplot2dDataset data9;
    data9.SetTitle("Vysledok");
    data9.SetStyle(Gnuplot2dDataset :: LINES_POINTS);
    
    Gnuplot graf10("graf_pole10.svg");
    graf10.SetTerminal("svg");
    graf10.SetTitle("Graf zavislosti jitter Sum od node.");
    graf10.SetLegend("Node", "jitter Sum [ms]");
    Gnuplot2dDataset data10;
    data10.SetTitle("Vysledok");
    data10.SetStyle(Gnuplot2dDataset :: LINES_POINTS);

    CommandLine cmd;
    cmd.AddValue("phyMode", "režim Wifi Phy", phyMode);
    cmd.AddValue("Prss", "Určený primárny RSS (dBm)", Prss);
    cmd.AddValue("PpacketSize", "veľkosť odoslaného paketu aplikácií", PpacketSize);
    cmd.AddValue("numPackets", "Celkový počet odoslaných paketov", numPackets);
    cmd.AddValue("startTime", "Čas začiatku simulácie", startTime);
    cmd.AddValue("distanceToRx", "vzdialenosť osi X medzi uzlami", distanceToRx);
    cmd.AddValue("verbose", "Zapnúť všetky komponenty protokolu zariadenia", verbose);
    cmd.Parse(argc, argv);

    // Konverzia na objekt času
    Time interPacketInterval = Seconds(interval);

    // vypne fragmentáciu pre snímky pod 2200 bajtov
    Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold",
                          StringValue("2200"));
    // vypnúť RTS / CTS pre snímky pod 2200 bajtov
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                          StringValue("2200"));
    // Fix non-unicast dátovej rýchlosti, aby bola rovnaká ako unicast
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",
                          StringValue(phyMode));

    // pre uzly> senzors
    NodeContainer senzors;
    senzors.Create(numNodes);

    // pre uzol> polnohospodar
    NodeContainer hosp;
    hosp.Create(1);

    // pre celú senzorickú sieť
    NodeContainer senzorNet;
    senzorNet.Add(hosp);
    senzorNet.Add(senzors);


    //Nižšie uvedený súbor pomocníkov nám pomôže zostaviť WiFi NIC, ktoré chceme
    WifiHelper wifi;
    if(verbose)
    {
        wifi.EnableLogComponents();
    }
    wifi.SetStandard(WIFI_PHY_STANDARD_80211g);

    /** Wifi PHY **/

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    wifiPhy.Set("RxGain", DoubleValue(-30.0));
    wifiPhy.Set("TxGain", DoubleValue(offset + Prss));
    wifiPhy.Set("EnergyDetectionThreshold", DoubleValue(-90.0));
//    wifiPhy.Set("CcaMode1Threshold", DoubleValue(-61.8));

    /** wifi kanál **/
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    //vytvoriť wifi kanál
    Ptr<YansWifiChannel> wifiChannelPtr = wifiChannel.Create();
    wifiPhy.SetChannel(wifiChannelPtr);

    /** Vrstva MAC **/
    // Pridajte MAC a vypnite riadenie rýchlosti
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                                  StringValue(phyMode), "ControlMode",
                                  StringValue(phyMode));
    // Nastavte ho na režim ad-hoc
    wifiMac.SetType("ns3::AdhocWifiMac");

    /** nainštalovať PHY + MAC **/
    NetDeviceContainer senzorDevices = wifi.Install(wifiPhy, wifiMac, senzors);
    NetDeviceContainer hospDevice = wifi.Install(wifiPhy, wifiMac, hosp);
    NetDeviceContainer allDevices = wifi.Install(wifiPhy, wifiMac, senzorNet);

    /** mobilita **/
    MobilityHelper mobility;
    // senzors
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0), // Xova os zacina v 0
                                   "MinY", DoubleValue (0.0), // Yos zazina v 0
                                   "DeltaX", DoubleValue (distance), // vzdialenost medzi uzlami po Xovej osi
                                   "DeltaY", DoubleValue (distance), // vzdialenost medzi uzlami po Yovej osi
                                   "GridWidth", UintegerValue (5), // 5 uzlov v jednom riadku
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(senzors);

    // poľnohospodár
    Ptr <ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(100.0, 250.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(hosp);


    /** Energetický model **/
    /************************************************* **************************/
    /* Zdroj energie */
    BasicEnergySourceHelper basicSourceHelper;
    // konfigurácia zdroja energie
    basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue (3.0));
    // nainštalovať zdroj
    EnergySourceContainer sources = basicSourceHelper.Install(senzors);
    /* energetický model zariadenia */
    WifiRadioEnergyModelHelper radioEnergyHelper;
    // konfigurovať model rádiovej energie
    radioEnergyHelper.Set("TxCurrentA", DoubleValue(0.0290));
    radioEnergyHelper.Set("RxCurrentA", DoubleValue(0.0080));
    // nainštalovať model zariadenia
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(senzorDevices, sources);

    /* energetický kombajn */
    BasicEnergyHarvesterHelper basicHarvesterHelper;
    // konfigurácia zberača energie
    basicHarvesterHelper.Set("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
    basicHarvesterHelper.Set("HarvestablePower", StringValue("ns3::UniformRandomVariable [Min = 0,0 | Max = 0,1]"));
    // nainštalovať kombajn na všetky sources energie
    EnergyHarvesterContainer harvesters = basicHarvesterHelper.Install(sources);

    /************************************************* ************************************************** **************/
    /** Internetový zásobník **/
    OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;

    Ipv4ListRoutingHelper list;
    list.Add(staticRouting, 0);
    list.Add(olsr, 1);

    InternetStackHelper internet;
    internet.SetRoutingHelper(list);
    internet.Install(senzorNet);

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO("Priradenie adries IP.");
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer IpContainer = ipv4.Assign(allDevices);

    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> recvSink = Socket::CreateSocket(hosp.Get(0), tid); // uzol 0, Cieľ
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
    recvSink->Bind(local);
    recvSink->SetRecvCallback(MakeCallback(&ReceivePacket));

    for(int j = 0; j <25; j ++)
    {
        for(int k = 0; k <5; k ++)
        {

            Simulator::Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & goToNewPosition, hosp.Get(0), &data, & data2, k);

            for (uint32_t i = 0; i <senzors.GetN(); i ++)
            {
                Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
                positionAlloc->Add(Vector(135.0 + i * 5, 220.0, 0.0));
                mobility.SetPositionAllocator(positionAlloc);

                Ptr <Socket> source = Socket::CreateSocket(senzors.Get(i), tid); // node i, Zdroj
                InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast(), 80);
                source->SetAllowBroadcast(true);
                source->Connect(remote);

                /** pripojiť sources sledovania **/
                /************************************************* **************************/
                // všetky stopy sú pripojené k uzlu 1 (Cieľ)
                // Zdroj energie
                Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource>(sources.Get(0));
                basicSourcePtr->TraceConnectWithoutContext("RemainingEnergy", MakeCallback(&RemainingEnergy));
                // energetický model zariadenia
                // device energy model
                Ptr<DeviceEnergyModel> basicRadioModelPtr =
                  basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
                NS_ASSERT (basicRadioModelPtr != 0);
                basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
                // energy harvester
                Ptr<BasicEnergyHarvester> basicHarvesterPtr = DynamicCast<BasicEnergyHarvester> (harvesters.Get (1));
                basicHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
                basicHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));
                /************************************************* **************************/

                /** nastavenie simulácie **/
                // začať prevádzku

                Simulator::Schedule(Seconds(startTime + j * 0.7 + k * 0.4), &GenerateTraffic, source, PpacketSize,
                                    senzorNet.Get(0), numPackets, interPacketInterval);
            }

        }
    }
    
    //https://www.nsnam.org/doxygen/wifi-hidden-terminal_8cc_source.html
    // 7. Install applications: two CBR streams each saturating the channel
   uint16_t cbrPort = 12345;
   Ipv4Address hospIpv4 = hosp.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
   OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (hospIpv4, cbrPort));
   onOffHelper.SetAttribute("PacketSize", UintegerValue(PpacketSize));
   onOffHelper.SetAttribute("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1000]"));
   onOffHelper.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  // onOffHelper.SetConstantRate(DataRate("448kb/s"));
   onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate("448kb/s")));
   onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(startTime)));
    ApplicationContainer cbrApps;
    for (int i = 0; i < senzors.GetN(); i++){
	cbrApps.Add(onOffHelper.Install(senzors.Get(i)));
    }
    // 8. Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
 
    Simulator::Stop(Seconds(10.0));

    AnimationInterface anim ("netanim_pole.xml");

    for (int i = 0; i <25; i ++) {
        anim.UpdateNodeSize(i, 5, 5);
        anim.UpdateNodeColor(i, 0, 128, 0);
    }
    anim.UpdateNodeSize(25, 10, 10);
    anim.UpdateNodeColor(hosp.Get(0), 255, 0, 0);
    anim.UpdateNodeDescription(hosp.Get(0), "poľnohospodárodar");


    Simulator::Run();
    Simulator::Destroy();

    /*Pick stats from FlowMonitor*/
    std::ofstream lossFile("loss-rates.dat", std::ofstream::app);
    std::ofstream delayFile("delays.dat", std::ofstream::app);
    std::ofstream usefulFile("useful-data.dat", std::ofstream::app);

    // 10. Print per flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    std::cout << "Print per flow statistics "<< stats.size() << std::endl;
    for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin(); it != stats.end(); it++)
      {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(it->first);
/*Calc needed stats*/
		double packetLossRate = ((double)it->second.lostPackets/(double)(it->second.lostPackets + it->second.rxPackets))*100;
		double meanDelay = (it->second.delaySum.GetMilliSeconds()/(double)it->second.rxPackets);
		double usefulData = ((double)(it->second.txBytes - meanDroppedBytes(it->second.bytesDropped))/(double)it->second.txBytes)*100;
                double ThroughputData = it->second.rxBytes*8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds())/1024;

		/*Save stats*/
		lossFile << numNodes << " " << packetLossRate << std::endl;
		delayFile << numNodes << " " << meanDelay << std::endl;
		usefulFile << PpacketSize << " " << usefulData << std::endl;

                std::cout << "FlowId: " << it->first << " Src-Addr:" << t.sourceAddress << " Dst-Addr:" << t.destinationAddress << std::endl;
		std::cout << "Send packets = " << it->second.txPackets << std::endl;
		std::cout << "Received packets = " << it->second.rxPackets << std::endl;
		std::cout << "Throughput = " << ThroughputData << " Kbps" << std::endl;
		data7.Add((double)it->first, (double)ThroughputData);
                std::cout << "Packet Loss Rate = " << packetLossRate << "\%" << std::endl;
		std::cout << "Packet Delay Sum = " << it->second.delaySum.As(Time::S) << std::endl;
                std::cout << "Packet Delay Sum = " << it->second.delaySum.GetMilliSeconds() << std::endl;
                data8.Add((double)it->first, (double)it->second.delaySum.GetMilliSeconds()/1000);
		std::cout << "Mean Delay = " << meanDelay << "ms" << std::endl;
                if (isnan(meanDelay)) {
                    meanDelay=0.0;
                }
                data9.Add((double)it->first, (double)meanDelay);
		std::cout << "Dropped Bytes = " << meanDroppedBytes(it->second.bytesDropped) << std::endl;
		std::cout << "Mean useful bytes = " << usefulData << "\%" << std::endl;		
                
                std::cout << "Packet Loss = " << it->second.lostPackets << std::endl;
                std::cout << "delay Histogram = " << it->second.delayHistogram.GetNBins() << std::endl;
                std::cout << "jitter Histogram = " << it->second.jitterHistogram.GetNBins() << std::endl;
                std::cout << "jitter Sum = " << it->second.jitterSum.GetMilliSeconds() << std::endl;
                data10.Add((double)it->first, (double)it->second.jitterSum.GetMilliSeconds());
                std::cout << "packet Size Histogram= " << it->second.packetSizeHistogram.GetNBins() << std::endl;
                std::cout << "packets Dropped= ";
                for (auto i = it->second.packetsDropped.begin(); i != it->second.packetsDropped.end(); ++i)
                    std::cout << *i << ' ';
                cout << endl;
                std::cout << "times Forwarded= " << it->second.timesForwarded << std::endl;
      }
    monitor->SerializeToXmlFile("NameOfFile.xml", true, true);

    lossFile.close();
    delayFile.close();
    usefulFile.close();

    graf.AddDataset(data);
    ofstream plotFile("graf_pole.plt");
    graf.GenerateOutput(plotFile);
    plotFile.close();
    system("gnuplot graf_pole.plt");

    graf2.AddDataset (data2);
    ofstream plotFile2 ("graf_pole2.plt");
    graf2.GenerateOutput(plotFile2);
    plotFile2.close();
    system("gnuplot graf_pole2.plt");
    

    graf3.AddDataset(data3);
    ofstream plotFile3 ("graf_pole3.plt");
    graf3.GenerateOutput(plotFile3);
    plotFile3.close();
    system("gnuplot graf_pole3.plt");

    graf4.AddDataset(data4);
    ofstream plotFile4 ("graf_pole4.plt");
    graf4.GenerateOutput(plotFile4);
    plotFile4.close();
    system("gnuplot graf_pole4.plt");

    graf5.AddDataset(data5);
    ofstream plotFile5 ("graf_pole5.plt");
    graf5.GenerateOutput(plotFile5);
    plotFile5.close();
    system("gnuplot graf_pole5.plt");

    graf6.AddDataset(data6);
    ofstream plotFile6 ("graf_pole6.plt");
    graf6.GenerateOutput(plotFile6);
    plotFile6.close();
    system("gnuplot graf_pole6.plt");
    
    graf7.AddDataset(data7);
    ofstream plotFile7 ("graf_pole7.plt");
    graf7.GenerateOutput(plotFile7);
    plotFile7.close();
    system("gnuplot graf_pole7.plt");
        
    graf8.AddDataset(data8);
    ofstream plotFile8 ("graf_pole8.plt");
    graf8.GenerateOutput(plotFile8);
    plotFile8.close();
    system("gnuplot graf_pole8.plt");
    
    graf9.AddDataset(data9);
    ofstream plotFile9 ("graf_pole9.plt");
    graf9.GenerateOutput(plotFile9);
    plotFile9.close();
    system("gnuplot graf_pole9.plt");
    
    graf10.AddDataset(data10);
    ofstream plotFile10 ("graf_pole10.plt");
    graf10.GenerateOutput(plotFile10);
    plotFile10.close();
    system("gnuplot graf_pole10.plt");


    return 0;
} 
