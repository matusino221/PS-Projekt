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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Gnuplot2dDataset data3;
long counter_x = 0;
float graphArray [4];

inline string PrintReceivedPacket(const Address &from)
{
    InetSocketAddress iaddr = InetSocketAddress::ConvertFrom(from);

    ostringstream oss;
    oss << "- nReceived jeden paket! Socket:" << iaddr.GetIpv4() << "port:" << iaddr.GetPort () << "at time =" << Simulator::Now().GetSeconds() << "n--";
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
            NS_LOG_UNCOND(PrintReceivedPacket(from));
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
    cout << Simulator::Now().GetSeconds()
        << "s Aktuálna zostávajúca energia =" << remainingEnergy << "J" <<  endl;
    if (counter_x % 3125 == 0)
    {
        data3.Add(Simulator::Now().GetSeconds(), remainingEnergy);
    }
    counter_x ++;
}

///Funkcia sledovania celkovej spotreby energie v uzle.
void TotalEnergy(double oldValue, double totalEnergy)
{
     cout << Simulator::Now().GetSeconds()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" <<  endl;
}

///Stopová funkcia pre energiu zberanú energetickým kombajnom.
void HarvestedPower(double oldValue, double harvestedPower)
{
     cout << Simulator::Now().GetSeconds()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" <<  endl;
}

///Stopová funkcia celkovej energie zozbieranej uzlom.
void TotalEnergyHarvested(double oldValue, double TotalEnergyHarvested)
{
     cout << Simulator::Now().GetSeconds()
                << "s Celková energia zozbieraná kombajnom ="
                << TotalEnergyHarvested << "J" <<  endl;
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
    graf3.SetLegend("Cas [s]", "Zostavajuca energia");
    data3.SetTitle("Vysledok");
    data3.SetStyle(Gnuplot2dDataset :: LINES_POINTS);

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
    // mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
    // "X", StringValue ("ns3::UniformRandomVariable [Min = 0,0 | Max = 50.0]"), "Y", StringValue ("ns3::UniformRandomVariable [Min = -20.0 | Max = 20.0]"));
    // mobility.SetMobilityModel("ns3::RandomRectanglePositionAllocator");
    // mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Obdĺžnik (-100, 250, -100, 250)));

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
//                Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels "NS3::WifiRadioEnergyModel").Get(0);
//                NS_ASSERT(basicRadioModelPtr != 0);
//                basicRadioModelPtr->TraceConnectWithoutContext("TotalEnergyConsumption", MakeCallback(&​​TotalEnergy));
//                // zberač energie
//                Ptr<BasicEnergyHarvester> basicHarvesterPtr = DynamicCast<BasicEnergyHarvester>(harvesters.Get(1));
//                basicHarvesterPtr->TraceConnectWithoutContext("HarvestedPower", MakeCallback(&​​HarvestedPower));
//                basicHarvesterPtr->TraceConnectWithoutContext("TotalEnergyHarvested", MakeCallback (& ​​TotalEnergyHarvested));
                /************************************************* **************************/

                /** nastavenie simulácie **/
                // začať prevádzku

                Simulator::Schedule(Seconds(startTime + j * 0.7 + k * 0.4), &GenerateTraffic, source, PpacketSize,
                                    senzorNet.Get (0), numPackets, interPacketInterval);
            }

        }
    }
    Simulator::Stop(Seconds(10.0));

    AnimationInterface anim ("netanim_pole.xml");

    for (int i = 0; i <25; i ++) {
        anim.UpdateNodeSize(i, 5, 5);
        anim.UpdateNodeColor(i, 0, 128, 0);
    }
    anim.UpdateNodeSize(25, 10, 10);
    anim.UpdateNodeColor(hosp.Get(0), 255, 0, 0);
    anim.UpdateNodeDescription(hosp.Get(0), "pol poľnohospodárodar");


    Simulator::Run();
    Simulator::Destroy();

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

    return 0;
} 
