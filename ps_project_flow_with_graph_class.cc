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
#include "ns3/flow-monitor-module.h"
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

class Graph {
private:
    Gnuplot2dDataset data;
    Gnuplot graph;
    long counter_data_x = 0;
public:
    
    Graph(string fileName, string title, string xAxisTitle, string yAxisTitle, bool errBar=false) {
        Gnuplot graph(fileName);
        this->graph = graph;
        this->graph.SetTerminal("svg");
        this->graph.SetTitle(title);
        this->graph.SetLegend(xAxisTitle, yAxisTitle);
        this->data.SetTitle("Vysledok");
        this->data.SetStyle(Gnuplot2dDataset::LINES_POINTS);
        if (errBar)
            this->data.SetErrorBars(Gnuplot2dDataset::XY);
    };
    
    Gnuplot2dDataset getData() {
        return data;
    };
    
    long getCounter() {
        return counter_data_x;
    }
    
    void addCounter() {
        counter_data_x++;
    }
    
    void addData(double x, double y) {
        data.Add(x,y);
    }
    void addData(double x, double y, double errX, double errY) {
        data.Add(x,y, errX, errY);
    }
        
    void saveGraph(string fileName) {
        this->graph.AddDataset(this->data);
        ofstream plotFile(fileName);
        this->graph.GenerateOutput(plotFile);
        plotFile.close();
        system(("gnuplot " + fileName).c_str());
    };
};

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Graph graph1("graf_pole1.svg", "Graf zavislosti suctu velkosti prijatých paketov od casu", "Cas [s]",  "SUM velkosti prijatych paketov [B]", true);
Graph graph2("graf_pole2.svg", "Graf zavislosti poctu prijatých paketov od casu", "Cas [s]",  "Pocet přijatých paketov");
Graph graph3("graf_pole3.svg", "Graf zavislosti zostatku energie od casu.", "Cas [s]",  "Zostavajuca energia [J]");
Graph graph4("graf_pole4.svg", "Graf zavislosti celkovej spotreby energie od casu.", "Cas [s]",  "Celkova spotreba energie [J]");
Graph graph5("graf_pole5.svg", "Graf zavislosti zozbieranej energie od casu.", "Cas [s]",  "Zozbierana energia [W]");
Graph graph6("graf_pole6.svg", "Graf zavislosti celkovej zozbieranej energie od casu.", "Cas [s]",  "Celkova zozbierana energia [J]");
float graphArray[4];

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

void RemainingEnergy(double oldValue, double remainingEnergy)
{
    /*cout << Simulator::Now().GetSeconds()
        << "s Aktuálna zostávajúca energia =" << remainingEnergy << "J" <<  endl;*/
    if (graph3.getCounter() % 3125 == 0)
    {
        graph3.addData(Simulator::Now().GetSeconds(), remainingEnergy);
    }
    graph3.addCounter();
}

void TotalEnergy(double oldValue, double totalEnergy)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" <<  endl;*/
    if (graph4.getCounter() % 3125 == 0)
    {
        graph4.addData(Simulator::Now().GetSeconds(), totalEnergy);
    }
    graph4.addCounter();
}

void HarvestedPower(double oldValue, double harvestedPower)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" <<  endl;*/
    if (graph5.getCounter() % 3125 == 0)
    {
        graph5.addData(Simulator::Now().GetSeconds(), harvestedPower);
    }
    graph5.addCounter();
}

void TotalEnergyHarvested(double oldValue, double TotalEnergyHarvested)
{
     /*cout << Simulator::Now().GetSeconds()
                << "s Celková energia zozbieraná ="
                << TotalEnergyHarvested << "J" <<  endl;*/
    if (graph6.getCounter() % 3125 == 0)
    {
        graph6.addData(Simulator::Now().GetSeconds(), TotalEnergyHarvested);
    }
    graph6.addCounter();
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

static void goToNewPosition(Ptr<Node> hosp, int k) {
    MobilityHelper mobility;
//     int strana = rand() % 4;
//     int poloha = rand() % 250;// pouzit random z NS3
    Ptr<UniformRandomVariable> stranaRandom = CreateObject<UniformRandomVariable>();
    stranaRandom->SetAttribute("Min", DoubleValue(0));
    stranaRandom->SetAttribute("Max", DoubleValue(4));
    Ptr<UniformRandomVariable> polohaRandom = CreateObject<UniformRandomVariable>();
    polohaRandom->SetAttribute("Min", DoubleValue(0));
    polohaRandom->SetAttribute("Max", DoubleValue(250));
    int strana = stranaRandom->GetValue();
    int poloha = polohaRandom->GetValue();
    
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
            
        //data->Add(Simulator::Now().GetSeconds(), tmpGraphArray[0], 0.0, tmpGraphArray[1]);
            graph1.addData(Simulator::Now().GetSeconds(), tmpGraphArray[0], 0.0, tmpGraphArray[1]);
    }
    graph2.addData(Simulator::Now().GetSeconds(), globalCounter2);

}

int main(int argc, char * argv [])
{
    uint32_t numNodes = 25; // 5x5
    double distance = 62.5; // [m] vzdialenost medzi uzlami asi
    string phyMode("DsssRate1Mbps");
    double Prss = -20; // dBm
    uint32_t PpacketSize = 200; // bajty
    bool verbose = false; //false

    //simulacne parametre
    uint32_t numPackets = 10000; // pocet paketov ktore sa maju odoslat
    double interval = 1.0; // sec
    double startTime = 0.0; // sec
    double distanceToRx = 10.0; // metre
    double offset = 81.0;

    double harvestingUpdateInterval = 1; // sec
    
    SeedManager::SetSeed(3);

    Graph graph7("graf_pole7.svg", "Graf zavislosti priepustnosti od node.", "Node",  "Priepustnost");
    Graph graph8("graf_pole8.svg", "Graf zavislosti sumy oneskorenia paketov od node.", "Node",  "Suma oneskorenia paketov [s]");
    Graph graph9("graf_pole9.svg", "Graf zavislosti priemerneho oneskorenia od node.", "Node", "Priemerneho oneskorenia [ms]");
    Graph graph10("graf_pole10.svg", "Graf zavislosti jitter Sum od node.", "Node", "jitter Sum [ms]");

    CommandLine cmd;
    cmd.AddValue("phyMode", "režim Wifi Phy", phyMode);
    cmd.AddValue("Prss", "Určený primárny RSS (dBm)", Prss);
    cmd.AddValue("PpacketSize", "veľkosť odoslaného paketu aplikácií", PpacketSize);
    cmd.AddValue("numPackets", "Celkový počet odoslaných paketov", numPackets);
    cmd.AddValue("startTime", "Čas začiatku simulácie", startTime);
    cmd.AddValue("distanceToRx", "vzdialenosť osi X medzi uzlami", distanceToRx);
    cmd.AddValue("verbose", "Zapnúť všetky komponenty protokolu zariadenia", verbose);
    cmd.Parse(argc, argv);
    // Convert to time object
    Time interPacketInterval = Seconds(interval);
    // disable fragmentation for frames below 2200 bytes
    Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold",
                          StringValue("2200"));
    // turn off RTS/CTS for frames below 2200 bytes
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                          StringValue("2200"));
    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",
                          StringValue(phyMode));

    // pre uzly senzors
    NodeContainer senzors;
    senzors.Create(numNodes);

    // pre uzol hospodar
    NodeContainer hosp;
    hosp.Create(1);

    NodeContainer senzorNet;
    senzorNet.Add(hosp);
    senzorNet.Add(senzors);

    // The below set of helpers will help us to put together the wifi NICs we want
    WifiHelper wifi;
    if(verbose)
    {
        wifi.EnableLogComponents();
    }
    wifi.SetStandard(WIFI_PHY_STANDARD_80211g);

    /** Wifi PHY **/

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    wifiPhy.Set("RxGain", DoubleValue(-30.0)); //zapis
    wifiPhy.Set("TxGain", DoubleValue(offset + Prss)); //citanie
    wifiPhy.Set("EnergyDetectionThreshold", DoubleValue(-90.0));
    //wifiPhy.Set("CcaMode1Threshold", DoubleValue(-61.8));

    /** wifi Channel **/
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    // create wifi channel
    Ptr<YansWifiChannel> wifiChannelPtr = wifiChannel.Create();
    wifiPhy.SetChannel(wifiChannelPtr);

    // Add a MAC and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                                  StringValue(phyMode), "ControlMode",
                                  StringValue(phyMode));
    // Set it to ad-hoc mode
    wifiMac.SetType("ns3::AdhocWifiMac");

    /** install PHY + MAC **/
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

    // hospodar
    Ptr <ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(100.0, 250.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(hosp);


    /***************************************************************************/
    /* energy source */
    BasicEnergySourceHelper basicSourceHelper;
    // configure energy source
    basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue (3.0));
    // install source
    EnergySourceContainer sources = basicSourceHelper.Install(senzors);
    /* device energy model */
    WifiRadioEnergyModelHelper radioEnergyHelper;
    // configure radio energy model
    radioEnergyHelper.Set("TxCurrentA", DoubleValue(0.0290)); //citanie
    radioEnergyHelper.Set("RxCurrentA", DoubleValue(0.0080)); //zapis
    // install device model
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(senzorDevices, sources);

    /* energy harvester */
    BasicEnergyHarvesterHelper basicHarvesterHelper;
    // configure energy harvester
    basicHarvesterHelper.Set("PeriodicHarvestedPowerUpdateInterval", TimeValue(Seconds(harvestingUpdateInterval)));
    basicHarvesterHelper.Set("HarvestablePower", StringValue("ns3::UniformRandomVariable [Min = 0,0 | Max = 0,1]"));
    // install harvester on all energy sources
    EnergyHarvesterContainer harvesters = basicHarvesterHelper.Install(sources);

    /*****************************************************************************************************************/
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
    Ptr<Socket> recvSink = Socket::CreateSocket(hosp.Get(0), tid);
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
    recvSink->Bind(local);
    recvSink->SetRecvCallback(MakeCallback(&ReceivePacket));

    for(int j = 0; j <25; j ++)
    {
        for(int k = 0; k <5; k ++)
        {
            Simulator::Schedule(Seconds(startTime + j * 0.7 + k * 0.4), &goToNewPosition, hosp.Get(0), k);

            for (uint32_t i = 0; i < senzors.GetN(); i ++)
            {
                Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
                positionAlloc->Add(Vector(135.0 + i * 5, 220.0, 0.0));
                mobility.SetPositionAllocator(positionAlloc);

                Ptr<Socket> source = Socket::CreateSocket(senzors.Get(i), tid); // node i, Zdroj
                InetSocketAddress remote = InetSocketAddress(Ipv4Address::GetBroadcast(), 80);
                source->SetAllowBroadcast(true);
                source->Connect(remote);

                /***************************************************************************/
                // all traces are connected to node 1 (Destination)
                // energy source
                Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource>(sources.Get(0));
                basicSourcePtr->TraceConnectWithoutContext("RemainingEnergy", MakeCallback(&RemainingEnergy));
                // device energy model
                Ptr<DeviceEnergyModel> basicRadioModelPtr =
                  basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
                NS_ASSERT (basicRadioModelPtr != 0);
                basicRadioModelPtr->TraceConnectWithoutContext("TotalEnergyConsumption", MakeCallback(&TotalEnergy));
                // energy harvester
                Ptr<BasicEnergyHarvester> basicHarvesterPtr = DynamicCast<BasicEnergyHarvester>(harvesters.Get (1));
                basicHarvesterPtr->TraceConnectWithoutContext("HarvestedPower", MakeCallback (&HarvestedPower));
                basicHarvesterPtr->TraceConnectWithoutContext("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));
                /************************************************* **************************/

                /** nastavenie simulácie **/
                // start traffic

                Simulator::Schedule(Seconds(startTime + j * 0.7 + k * 0.4), &GenerateTraffic, source, PpacketSize,
                                    senzorNet.Get(0), numPackets, interPacketInterval); //v čase simulácie zmena v modelu L2-L5 3 body
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
   //onOffHelper.SetConstantRate(DataRate("448kb/s"));
   onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate("448kb/s")));
   onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(startTime)));
    ApplicationContainer cbrApps;
    for (uint32_t i = 0; i < senzors.GetN(); i++){
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
    anim.UpdateNodeDescription(hosp.Get(0), "Hospodarodar");

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
		graph7.addData((double)it->first, (double)ThroughputData);
                std::cout << "Packet Loss Rate = " << packetLossRate << "\%" << std::endl;
		std::cout << "Packet Delay Sum = " << it->second.delaySum.As(Time::S) << std::endl;
                std::cout << "Packet Delay Sum = " << it->second.delaySum.GetMilliSeconds() << std::endl;
                graph8.addData((double)it->first, (double)it->second.delaySum.GetMilliSeconds()/1000);
		std::cout << "Mean Delay = " << meanDelay << "ms" << std::endl;
                if (isnan(meanDelay)) {
                    meanDelay=0.0;
                }
                graph9.addData((double)it->first, (double)meanDelay);
		std::cout << "Dropped Bytes = " << meanDroppedBytes(it->second.bytesDropped) << std::endl;
		std::cout << "Mean useful bytes = " << usefulData << "\%" << std::endl;		
                
                std::cout << "Packet Loss = " << it->second.lostPackets << std::endl;
                std::cout << "delay Histogram = " << it->second.delayHistogram.GetNBins() << std::endl;
                std::cout << "jitter Histogram = " << it->second.jitterHistogram.GetNBins() << std::endl;
                std::cout << "jitter Sum = " << it->second.jitterSum.GetMilliSeconds() << std::endl;
                graph10.addData((double)it->first, (double)it->second.jitterSum.GetMilliSeconds());
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
   
    graph1.saveGraph("graf_pole1.plt");
    graph2.saveGraph("graf_pole2.plt");
    graph3.saveGraph("graf_pole3.plt");
    graph4.saveGraph("graf_pole4.plt");
    graph5.saveGraph("graf_pole5.plt");
    graph6.saveGraph("graf_pole6.plt");
    graph7.saveGraph("graf_pole7.plt");
    graph8.saveGraph("graf_pole8.plt");
    graph9.saveGraph("graf_pole9.plt");
    graph10.saveGraph("graf_pole10.plt");

    return 0;
} 
