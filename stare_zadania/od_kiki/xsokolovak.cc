n20 n21 n22 n23 n24
        n20 n21 n22 n23 n24
// n15 n16 n17 n18 n19
n nn n11 n12 n13 n14
// n5 n6 n7 n8 n9
// n0 n1 n2 n3 n4

#include "ns3 / core-module.h"
#include "ns3 / network-module.h"
#include "ns3 / mobility-module.h"
#include "ns3 / config-store-module.h"
#include "ns3 / wifi-module.h"
#include "ns3 / energy-module.h"
#include "ns3 / internet-module.h"
#include "ns3 / olsr-helper.h"
#include "ns3 / netanim-module.h"
#include "ns3 / gnuplot.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>

pomocou namespace ns3;

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Gnuplot2dDataset data3;
long counter_x = 0;
float graphArray [4];

statický inline std :: string
PrintReceivedPacket (adresa & z)
{
InetSocketAddress iaddr = InetSocketAddress :: ConvertFrom (from);

std :: ostringstream oss;
oss << "- nReceived jeden paket! Socket:" << iaddr.GetIpv4 ()
<< "port:" << iaddr.GetPort ()
<< "at time =" << Simulátor :: Teraz () .GetSeconds ()
<< "n--";

return oss.str ();
}

/ **
param socket Pointer na zásuvku.
*
* Paket príjem drez.
* /
prázdno
ReceivePacket (zásuvka Ptr <Socket>)
{
    Paket Ptr <Packet>;
    Adresa z;
    zatiaľ čo ((packet = socket-> RecvFrom (od)))
    {
        if (paket-> GetSize ()> 0)
        {
            NS_LOG_UNCOND (PrintReceivedPacket (from));
            globalCounter + = paket-> GetSize ();
            globalCounter2 ++;
        }
    }
}

/ **
param socket Pointer na zásuvku.
veľkosť pktSize Veľkosť paketu.
* n n Ukazovateľ na uzol.
* Param pktCount Počet paketov, ktoré sa majú vygenerovať.
* Param pktInterval Interval odosielania paketov.
*
* Generátor prevádzky.
* /
statické neplatné
GenerateTraffic (Ptr <Socket> socket, uint32_t pktSize, Ptr <Node> n,
uint32_t pktCount, Čas pktInterval)
{
if (pktCount> 0)
{
socket-> Send (Vytvoriť <Packet> (pktSize));
Simulator :: Plán (pktInterval, & GenerateTraffic, socket, pktSize, n,
        pktCount - 1, pktInterval);
}
inak
{
socket-> Close ();
}
}

/// Funkcia sledovania zostávajúcej energie v uzle.
prázdno
        Zvyšná energia (dvojitá stará hodnota, dvojitá zvyšná energia)
{
std :: cout << Simulátor :: Teraz () .GetSeconds ()
<< "s Aktuálna zostávajúca energia =" << << zostávajúceEnergy << "J" << std :: endl;
if (counter_x% 3125 == 0)
{
data3.Add (Simulator :: Now () .GetSeconds (), zostávajúceEnergy);
}
counter_x ++;
}

/// Funkcia sledovania celkovej spotreby energie v uzle.
prázdno
TotalEnergy (double oldValue, double totalEnergy)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" << std :: endl;
}

/// Stopová funkcia pre energiu zberanú energetickým kombajnom.
prázdno
Zberač (double oldValue, double harvestedPower)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" << std :: endl;
}

/// Stopová funkcia celkovej energie zozbieranej uzlom.
prázdno
TotalEnergyHarvestované (double oldValue, double TotalEnergyHarvested)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková energia zozbieraná kombajnom ="
                << TotalEnergyHarvested << "J" << std :: endl;
}

void standardDeviation (float data [], float temp []) {
    float sum = 0,0, priemer, štandardDeviation = 0,0;
    int i;

    pre (i = 0; i <5; ++ i) {
        sum + = údaje [i];
    }
    mean = sum / 5;

    pre (i = 0; i <5; ++ i)
    standardDeviation + = pow (údaje [i] - priemer, 2);

    teplota [0] = stredná hodnota;
    temp [1] = sqrt (standardDeviation / 5);

}

static void goToNewPosition (údaje Ptr <Node> farmár, Gnuplot2dDataset *, Gnuplot2dDataset * data2, int k) {
    MobilitaPomoc mobility;
    int strana = rand ()% 4;
    int poloha = rand ()% 250;
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();

    if (strana == 0) {
        positionAlloc-> Add (Vector (0.0, poloha, 0.0));
    }
    else if (strana == 1) {
        positionAlloc-> Add (Vector (poloha, 0,0, 0,0));
    }
    else if (strana == 2) {
        positionAlloc-> Add (Vektor (250,0, poloha, 0,0));
    }
    else if (strana == 3) {
        positionAlloc-> Add (Vector (poloha, 250,0, 0,0));
    }

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ( "NS3 :: ConstantPositionMobilityModel");
    mobility.Install (poľnohospodár);

    graphArray [k] = globalCounter;
    globalCounter = 0;
    if (k == 4) {
        float tmpGraphArray [2];
        standardDeviation (graphArray, tmpGraphArray);
        if (tmpGraphArray [0]! = 0 && tmpGraphArray [1]! = 0)
        data-> Pridať (Simulator :: Now (). GetSeconds (), tmpGraphArray [0], 0.0, tmpGraphArray [1]);
    }
    data2-> Pridať (Simulator :: Now (). GetSeconds (), globalCounter2);

}

int
main (int argc, char * argv [])
{

    uint32_t numNodes = 25; // 5x5
    dvojitá vzdialenosť = 62,5; // [m] vzdialenost medzi uzlami asi
    std :: string phyMode ("DsssRate1Mbps");
    double Prss = -20; // dBm
    uint32_t PpacketSize = 200; // bajty
    bool verbose = false;

    // simulačné parametre
    uint32_t numPackets = 10000; // počet paketov, ktoré sa majú odoslať
    dvojitý interval = 1; // sekúnd
    double startTime = 0,0; // sekúnd
    dvojitá vzdialenosťToRx = 10,0; // metre
    dvojitý posun = 81;

    // Premenné energetického kombajnu
    double harvestingUpdateInterval = 1; // sekúnd

    Gnuplot graf ("graf_pole.svg");
    graf.SetTerminal ( "SVG");
    graf.SetTitle ("Graf zavislosti suctu velkosti prijatých paketov od casu");
    graf.SetLegend ("Cas [s]", "SUM velkosti prijatych paketov [B]");
    Gnuplot2dDataset dáta;
    data.SetTitle ( "Vysledok");
    data.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    data.SetErrorBars (Gnuplot2dDataset :: XY);

    Gnuplot graf2 ("graf_pole2.svg");
    graf2.SetTerminal ( "SVG");
    graf2.SetTitle ("Graf zavislosti poctu prijatých paketov od casu");
    graf2.SetLegend ("Cas [s]", "Pocet přijatých paketov");
    Gnuplot2dDataset data2;
    data2.SetTitle ( "Vysledok");
    data2.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    //data2.SetErrorBars(Gnuplot2dDataset::XY);

    Gnuplot graf3 ("graf_pole3.svg");
    graf3.SetTerminal ( "SVG");
    graf3.SetTitle ("Graf zavislosti zostatku energie od casu.");
    graf3.SetLegend ("Cas [s]", "Zostavajuca energia");
    data3.SetTitle ( "Vysledok");
    data3.SetStyle (Gnuplot2dDataset :: LINES_POINTS);

    CommandLine cmd;
    cmd.AddValue ("phyMode", "režim Wifi Phy", phyMode);
    cmd.AddValue ("Prss", "Určený primárny RSS (dBm)", Prss);
    cmd.AddValue ("PpacketSize", "veľkosť odoslaného paketu aplikácií", PpacketSize);
    cmd.AddValue ("numPackets", "Celkový počet odoslaných paketov", numPackets);
    cmd.AddValue ("startTime", "Čas začiatku simulácie", startTime);
    cmd.AddValue ("distanceToRx", "vzdialenosť osi X medzi uzlami", distanceToRx);
    cmd.AddValue ("verbose", "Zapnúť všetky komponenty protokolu zariadenia", verbose);
    cmd.Parse (argc, argv);

    // Konverzia na objekt času
    Time interPacketInterval = sekundy (interval);

    // vypne fragmentáciu pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: FragmentationThreshold",
                          StringValue ("2200"));
    // vypnúť RTS / CTS pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: RtsCtsThreshold",
                          StringValue ("2200"));
    // Fix non-unicast dátovej rýchlosti, aby bola rovnaká ako unicast
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: NonUnicastMode",
                          StringValue (phyMode));

    // pre uzly> senzory
    NodeContainer senzory;
    senzors.Create (numNodes);

    // pre uzol> polnohospodar
    NodeContainer farmár;
    farmer.Create (1);

    // pre celú senzorickú sieť
    NodeContainer senzorNet;
    senzorNet.Add (poľnohospodár);
    senzorNet.Add (senzory);


    Nižšie uvedený súbor pomocníkov nám pomôže zostaviť WiFi NIC, ktoré chceme
    WifiHelper wifi;
    ak (podrobne)
    {
        wifi.EnableLogComponents ();
    }
    wifi.SetStandard (WIFI_PHY_STANDARD_80211g);

    / ** Wifi PHY ** /

              YansWifiPhyHelper wifiPhy = YansWifiPhyHelper :: Default ();
    wifiPhy.Set ("RxGain", DoubleValue (-30));
    wifiPhy.Set ("TxGain", DoubleValue (offset + Prss));
    wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-90,0));
    wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-61.8));

    / ** wifi kanál ** /
              YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3 :: ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3 :: JakesPropagationLossModel");
    // vytvoriť wifi kanál
    Ptr <YansWifiChannel> wifiChannelPtr = wifiChannel.Create ();
    wifiPhy.SetChannel (wifiChannelPtr);

    / ** Vrstva MAC ** /
                // Pridajte MAC a vypnite riadenie rýchlosti
                WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager ("ns3 :: ConstantRateWifiManager", "DataMode",
                                  StringValue (phyMode), "ControlMode",
                                  StringValue (phyMode));
    // Nastavte ho na režim ad-hoc
    wifiMac.SetType ("ns3 :: AdhocWifiMac");

    / ** nainštalovať PHY + MAC ** /
                            NetDeviceContainer senzorDevices = wifi.Install (wifiPhy, wifiMac, senzory);
    NetDeviceContainer farmerDevice = wifi.Install (wifiPhy, wifiMac, farmár);
    NetDeviceContainer allDevices = wifi.Install (wifiPhy, wifiMac, senzorNet);

    / ** mobilita ** /
      MobilitaPomoc mobility;
    // senzory
    mobility.SetPositionAllocator ("ns3 :: GridPositionAllocator",
                                   "MinX", DoubleValue (0.0), // Xova os zacina v 0
                                   "MinY", DoubleValue (0.0), // Yos zazina v 0
                                   "DeltaX", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Xovej osi
                                   "DeltaY", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Yovej osi
                                   "GridWidth", UintegerValue (5), // 5 uzlov v jednom riadku
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3 :: ConstantPositionMobilityModel");
    mobilita.Inštalovať (senzory);

    // poľnohospodár
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
    positionAlloc-> Add (Vektor (100,0, 250,0, 0,0));
    mobility.SetPositionAllocator (positionAlloc);
    // mobility.SetPositionAllocator ("ns3 :: RandomRectanglePositionAllocator",
    // "X", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 50.0]"), "Y", StringValue ("ns3 :: UniformRandomVariable [Min = -20.0 | Max = 20.0]"));
    // mobility.SetMobilityModel ("ns3 :: RandomRectanglePositionAllocator");
    // mobility.SetMobilityModel ("ns3 :: RandomWalk2dMobilityModel", "Bounds", RectangleValue (Obdĺžnik (-100, 250, -100, 250)));

    mobilita.


    / ** Energetický model ** /
                     / ************************************************* ************************** /
                     /* Zdroj energie */
                     BasicEnergySourceHelper basicSourceHelper;
    // konfigurácia zdroja energie
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (3.0));
    // nainštalovať zdroj
    EnergySourceContainer sources = basicSourceHelper.Install (senzory);
    / * energetický model zariadenia * /
                          WifiRadioEnergyModelHelper radioEnergyHelper;
    // konfigurovať model rádiovej energie
    radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0,0290));
    radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0,0080));
    // nainštalovať model zariadenia
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (senzory, zdroje);

    / * energetický kombajn * /
                    BasicEnergyHarvesterHelper basicHarvesterHelper;
    // konfigurácia zberača energie
    basicHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
    basicHarvesterHelper.Set ("HarvestablePower", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 0,1]"));
    // nainštalovať kombajn na všetky zdroje energie
    EnergyHarvesterContainer harvestory = basicHarvesterHelper.Install (zdroje);

    / ************************************************* ************************************************** ************** /
      / ** Internetový zásobník ** /
                       OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;

    Ipv4ListRoutingHelper zoznam;
    list.Add (staticRouting, 0);
    list.Add (olsr, 1);

    InternetStackHelper internet;
    internet.SetRoutingHelper (zoznam);
    internet.Install (senzorNet);

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO ("Priradenie adries IP.");
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer IpContainer = ipv4.Assign (allDevices);

    TypeId tid = TypeId :: LookupByName ("ns3 :: UdpSocketFactory");
    Ptr <Socket> recvSink = Socket :: CreateSocket (farmár.Get (0), tid); // uzol 0, Cieľ
    InetSocketAddress local = InetSocketAddress (Ipv4Address :: GetAny (), 80);
    recvSink-> Bind (lokálne);
    recvSink-> SetRecvCallback (MakeCallback (& ​​ReceivePacket));

    pre (int j = 0; j <25; j ++)
    {
        pre (int k = 0; k <5; k ++)
        {

            Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & goToNewPosition, farmár.Get (0), & dáta, & data2, k);

            pre (uint32_t i = 0; i <senzory.GetN (); i ++)
            {
                Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
                positionAlloc-> Add (Vektor (135,0 + i * 5, 220,0, 0,0));
                mobility.SetPositionAllocator (positionAlloc);

                Ptr <Socket> source = Socket :: CreateSocket (senzory.Get (i), tid); // node i, Zdroj
                InetSocketAddress remote = InetSocketAddress (Ipv4Address :: GetBroadcast (), 80);
                vstup-> SetAllowBroadcast (true);
                vstup-> Connect (diaľkové);

                / ** pripojiť zdroje sledovania ** /
                                     / ************************************************* ************************** /
                                     // všetky stopy sú pripojené k uzlu 1 (Cieľ)
                                     // Zdroj energie
                                     Ptr <BasicEnergySource> basicSourcePtr = DynamicCast <BasicEnergySource> (sources.Get (0));
                basicSourcePtr-> TraceConnectW WithoutContext ("RemainingEnergy", MakeCallback (& ​​RemainingEnergy));
                // energetický model zariadenia
                Ptr <DeviceEnergyModel> basicRadioModelPtr =
                basicSourcePtr-> FindDeviceEnergyModels ( "NS3 :: WifiRadioEnergyModel") Get (0).
                        NS_ASSERT (basicRadioModelPtr! = 0);
                basicRadioModelPtr-> TraceConnectW WithoutContext ("TotalEnergyConsumption", MakeCallback (& ​​TotalEnergy));
                // zberač energie
                Ptr <BasicEnergyHarvester> basicHarvesterPtr = DynamicCast <BasicEnergyHarvester> (kombajny.Get (1));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("HarvestedPower", MakeCallback (& ​​HarvestedPower));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("TotalEnergyHarvested", MakeCallback (& ​​TotalEnergyHarvested));
                / ************************************************* ************************** /

                  / ** nastavenie simulácie ** /
                                  // začať prevádzku

                                  Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & GenerateTraffic, zdroj, PpacketSize,
                                                         senzorNet.Get (0), numPackets, interPacketInterval);
            }

        }
    }
    Simulátor :: Stop (sekundy (10,0));

    AnimationInterface anim ("netanim_pole.xml");

    pre (int i = 0; i <25; i ++) {
        anim.UpdateNodeSize (i, 5, 5);
        anim.UpdateNodeColor (i, 0, 128, 0);
    }
    anim.UpdateNodeSize (25, 10, 10);
    anim.UpdateNodeColor (farmár.Get (0), 255, 0, 0);
    anim.UpdateNodeDescription (farmer.Get (0), "pol poľnohospodárodar");


    Simulátor :: Run ();
    Simulátor :: Destroy ();
//
    graf.AddDataset (dáta);
    std :: ofstream plotFile ("graf_pole.plt");
    graf.GenerateOutput (plotFile);
    plotFile.close ();
    systém ("gnuplot graf_pole.plt");

    graf2.AddDataset (Data2);
    std :: ofstream plotFile2 ("graf_pole2.plt");
    graf2.GenerateOutput (plotFile2);
    plotFile2.close ();
    systém ("gnuplot graf_pole2.plt");

    graf3.AddDataset (data3);
    std :: ofstream plotFile3 ("graf_pole3.plt");
    graf3.GenerateOutput (plotFile3);
    plotFile3.close ();
    systém ("gnuplot graf_pole3.plt");

    návrat 0;
} n20 n21 n22 n23 n24
        n20 n21 n22 n23 n24
// n15 n16 n17 n18 n19
n nn n11 n12 n13 n14
// n5 n6 n7 n8 n9
// n0 n1 n2 n3 n4

#include "ns3 / core-module.h"
#include "ns3 / network-module.h"
#include "ns3 / mobility-module.h"
#include "ns3 / config-store-module.h"
#include "ns3 / wifi-module.h"
#include "ns3 / energy-module.h"
#include "ns3 / internet-module.h"
#include "ns3 / olsr-helper.h"
#include "ns3 / netanim-module.h"
#include "ns3 / gnuplot.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>

pomocou namespace ns3;

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Gnuplot2dDataset data3;
long counter_x = 0;
float graphArray [4];

statický inline std :: string
PrintReceivedPacket (adresa & z)
{
InetSocketAddress iaddr = InetSocketAddress :: ConvertFrom (from);

std :: ostringstream oss;
oss << "- nReceived jeden paket! Socket:" << iaddr.GetIpv4 ()
<< "port:" << iaddr.GetPort ()
<< "at time =" << Simulátor :: Teraz () .GetSeconds ()
<< "n--";

return oss.str ();
}

/ **
param socket Pointer na zásuvku.
*
* Paket príjem drez.
* /
prázdno
ReceivePacket (zásuvka Ptr <Socket>)
{
    Paket Ptr <Packet>;
    Adresa z;
    zatiaľ čo ((packet = socket-> RecvFrom (od)))
    {
        if (paket-> GetSize ()> 0)
        {
            NS_LOG_UNCOND (PrintReceivedPacket (from));
            globalCounter + = paket-> GetSize ();
            globalCounter2 ++;
        }
    }
}

/ **
param socket Pointer na zásuvku.
veľkosť pktSize Veľkosť paketu.
* n n Ukazovateľ na uzol.
* Param pktCount Počet paketov, ktoré sa majú vygenerovať.
* Param pktInterval Interval odosielania paketov.
*
* Generátor prevádzky.
* /
statické neplatné
GenerateTraffic (Ptr <Socket> socket, uint32_t pktSize, Ptr <Node> n,
uint32_t pktCount, Čas pktInterval)
{
if (pktCount> 0)
{
socket-> Send (Vytvoriť <Packet> (pktSize));
Simulator :: Plán (pktInterval, & GenerateTraffic, socket, pktSize, n,
        pktCount - 1, pktInterval);
}
inak
{
socket-> Close ();
}
}

/// Funkcia sledovania zostávajúcej energie v uzle.
prázdno
        Zvyšná energia (dvojitá stará hodnota, dvojitá zvyšná energia)
{
std :: cout << Simulátor :: Teraz () .GetSeconds ()
<< "s Aktuálna zostávajúca energia =" << << zostávajúceEnergy << "J" << std :: endl;
if (counter_x% 3125 == 0)
{
data3.Add (Simulator :: Now () .GetSeconds (), zostávajúceEnergy);
}
counter_x ++;
}

/// Funkcia sledovania celkovej spotreby energie v uzle.
prázdno
TotalEnergy (double oldValue, double totalEnergy)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" << std :: endl;
}

/// Stopová funkcia pre energiu zberanú energetickým kombajnom.
prázdno
Zberač (double oldValue, double harvestedPower)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" << std :: endl;
}

/// Stopová funkcia celkovej energie zozbieranej uzlom.
prázdno
TotalEnergyHarvestované (double oldValue, double TotalEnergyHarvested)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková energia zozbieraná kombajnom ="
                << TotalEnergyHarvested << "J" << std :: endl;
}

void standardDeviation (float data [], float temp []) {
    float sum = 0,0, priemer, štandardDeviation = 0,0;
    int i;

    pre (i = 0; i <5; ++ i) {
        sum + = údaje [i];
    }
    mean = sum / 5;

    pre (i = 0; i <5; ++ i)
    standardDeviation + = pow (údaje [i] - priemer, 2);

    teplota [0] = stredná hodnota;
    temp [1] = sqrt (standardDeviation / 5);

}

static void goToNewPosition (údaje Ptr <Node> farmár, Gnuplot2dDataset *, Gnuplot2dDataset * data2, int k) {
    MobilitaPomoc mobility;
    int strana = rand ()% 4;
    int poloha = rand ()% 250;
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();

    if (strana == 0) {
        positionAlloc-> Add (Vector (0.0, poloha, 0.0));
    }
    else if (strana == 1) {
        positionAlloc-> Add (Vector (poloha, 0,0, 0,0));
    }
    else if (strana == 2) {
        positionAlloc-> Add (Vektor (250,0, poloha, 0,0));
    }
    else if (strana == 3) {
        positionAlloc-> Add (Vector (poloha, 250,0, 0,0));
    }

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ( "NS3 :: ConstantPositionMobilityModel");
    mobility.Install (poľnohospodár);

    graphArray [k] = globalCounter;
    globalCounter = 0;
    if (k == 4) {
        float tmpGraphArray [2];
        standardDeviation (graphArray, tmpGraphArray);
        if (tmpGraphArray [0]! = 0 && tmpGraphArray [1]! = 0)
        data-> Pridať (Simulator :: Now (). GetSeconds (), tmpGraphArray [0], 0.0, tmpGraphArray [1]);
    }
    data2-> Pridať (Simulator :: Now (). GetSeconds (), globalCounter2);

}

int
main (int argc, char * argv [])
{

    uint32_t numNodes = 25; // 5x5
    dvojitá vzdialenosť = 62,5; // [m] vzdialenost medzi uzlami asi
    std :: string phyMode ("DsssRate1Mbps");
    double Prss = -20; // dBm
    uint32_t PpacketSize = 200; // bajty
    bool verbose = false;

    // simulačné parametre
    uint32_t numPackets = 10000; // počet paketov, ktoré sa majú odoslať
    dvojitý interval = 1; // sekúnd
    double startTime = 0,0; // sekúnd
    dvojitá vzdialenosťToRx = 10,0; // metre
    dvojitý posun = 81;

    // Premenné energetického kombajnu
    double harvestingUpdateInterval = 1; // sekúnd

    Gnuplot graf ("graf_pole.svg");
    graf.SetTerminal ( "SVG");
    graf.SetTitle ("Graf zavislosti suctu velkosti prijatých paketov od casu");
    graf.SetLegend ("Cas [s]", "SUM velkosti prijatych paketov [B]");
    Gnuplot2dDataset dáta;
    data.SetTitle ( "Vysledok");
    data.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    data.SetErrorBars (Gnuplot2dDataset :: XY);

    Gnuplot graf2 ("graf_pole2.svg");
    graf2.SetTerminal ( "SVG");
    graf2.SetTitle ("Graf zavislosti poctu prijatých paketov od casu");
    graf2.SetLegend ("Cas [s]", "Pocet přijatých paketov");
    Gnuplot2dDataset data2;
    data2.SetTitle ( "Vysledok");
    data2.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    //data2.SetErrorBars(Gnuplot2dDataset::XY);

    Gnuplot graf3 ("graf_pole3.svg");
    graf3.SetTerminal ( "SVG");
    graf3.SetTitle ("Graf zavislosti zostatku energie od casu.");
    graf3.SetLegend ("Cas [s]", "Zostavajuca energia");
    data3.SetTitle ( "Vysledok");
    data3.SetStyle (Gnuplot2dDataset :: LINES_POINTS);

    CommandLine cmd;
    cmd.AddValue ("phyMode", "režim Wifi Phy", phyMode);
    cmd.AddValue ("Prss", "Určený primárny RSS (dBm)", Prss);
    cmd.AddValue ("PpacketSize", "veľkosť odoslaného paketu aplikácií", PpacketSize);
    cmd.AddValue ("numPackets", "Celkový počet odoslaných paketov", numPackets);
    cmd.AddValue ("startTime", "Čas začiatku simulácie", startTime);
    cmd.AddValue ("distanceToRx", "vzdialenosť osi X medzi uzlami", distanceToRx);
    cmd.AddValue ("verbose", "Zapnúť všetky komponenty protokolu zariadenia", verbose);
    cmd.Parse (argc, argv);

    // Konverzia na objekt času
    Time interPacketInterval = sekundy (interval);

    // vypne fragmentáciu pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: FragmentationThreshold",
                          StringValue ("2200"));
    // vypnúť RTS / CTS pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: RtsCtsThreshold",
                          StringValue ("2200"));
    // Fix non-unicast dátovej rýchlosti, aby bola rovnaká ako unicast
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: NonUnicastMode",
                          StringValue (phyMode));

    // pre uzly> senzory
    NodeContainer senzory;
    senzors.Create (numNodes);

    // pre uzol> polnohospodar
    NodeContainer farmár;
    farmer.Create (1);

    // pre celú senzorickú sieť
    NodeContainer senzorNet;
    senzorNet.Add (poľnohospodár);
    senzorNet.Add (senzory);


    Nižšie uvedený súbor pomocníkov nám pomôže zostaviť WiFi NIC, ktoré chceme
    WifiHelper wifi;
    ak (podrobne)
    {
        wifi.EnableLogComponents ();
    }
    wifi.SetStandard (WIFI_PHY_STANDARD_80211g);

    / ** Wifi PHY ** /

              YansWifiPhyHelper wifiPhy = YansWifiPhyHelper :: Default ();
    wifiPhy.Set ("RxGain", DoubleValue (-30));
    wifiPhy.Set ("TxGain", DoubleValue (offset + Prss));
    wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-90,0));
    wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-61.8));

    / ** wifi kanál ** /
              YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3 :: ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3 :: JakesPropagationLossModel");
    // vytvoriť wifi kanál
    Ptr <YansWifiChannel> wifiChannelPtr = wifiChannel.Create ();
    wifiPhy.SetChannel (wifiChannelPtr);

    / ** Vrstva MAC ** /
                // Pridajte MAC a vypnite riadenie rýchlosti
                WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager ("ns3 :: ConstantRateWifiManager", "DataMode",
                                  StringValue (phyMode), "ControlMode",
                                  StringValue (phyMode));
    // Nastavte ho na režim ad-hoc
    wifiMac.SetType ("ns3 :: AdhocWifiMac");

    / ** nainštalovať PHY + MAC ** /
                            NetDeviceContainer senzorDevices = wifi.Install (wifiPhy, wifiMac, senzory);
    NetDeviceContainer farmerDevice = wifi.Install (wifiPhy, wifiMac, farmár);
    NetDeviceContainer allDevices = wifi.Install (wifiPhy, wifiMac, senzorNet);

    / ** mobilita ** /
      MobilitaPomoc mobility;
    // senzory
    mobility.SetPositionAllocator ("ns3 :: GridPositionAllocator",
                                   "MinX", DoubleValue (0.0), // Xova os zacina v 0
                                   "MinY", DoubleValue (0.0), // Yos zazina v 0
                                   "DeltaX", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Xovej osi
                                   "DeltaY", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Yovej osi
                                   "GridWidth", UintegerValue (5), // 5 uzlov v jednom riadku
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3 :: ConstantPositionMobilityModel");
    mobilita.Inštalovať (senzory);

    // poľnohospodár
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
    positionAlloc-> Add (Vektor (100,0, 250,0, 0,0));
    mobility.SetPositionAllocator (positionAlloc);
    // mobility.SetPositionAllocator ("ns3 :: RandomRectanglePositionAllocator",
    // "X", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 50.0]"), "Y", StringValue ("ns3 :: UniformRandomVariable [Min = -20.0 | Max = 20.0]"));
    // mobility.SetMobilityModel ("ns3 :: RandomRectanglePositionAllocator");
    // mobility.SetMobilityModel ("ns3 :: RandomWalk2dMobilityModel", "Bounds", RectangleValue (Obdĺžnik (-100, 250, -100, 250)));

    mobilita.


    / ** Energetický model ** /
                     / ************************************************* ************************** /
                     /* Zdroj energie */
                     BasicEnergySourceHelper basicSourceHelper;
    // konfigurácia zdroja energie
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (3.0));
    // nainštalovať zdroj
    EnergySourceContainer sources = basicSourceHelper.Install (senzory);
    / * energetický model zariadenia * /
                          WifiRadioEnergyModelHelper radioEnergyHelper;
    // konfigurovať model rádiovej energie
    radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0,0290));
    radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0,0080));
    // nainštalovať model zariadenia
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (senzory, zdroje);

    / * energetický kombajn * /
                    BasicEnergyHarvesterHelper basicHarvesterHelper;
    // konfigurácia zberača energie
    basicHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
    basicHarvesterHelper.Set ("HarvestablePower", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 0,1]"));
    // nainštalovať kombajn na všetky zdroje energie
    EnergyHarvesterContainer harvestory = basicHarvesterHelper.Install (zdroje);

    / ************************************************* ************************************************** ************** /
      / ** Internetový zásobník ** /
                       OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;

    Ipv4ListRoutingHelper zoznam;
    list.Add (staticRouting, 0);
    list.Add (olsr, 1);

    InternetStackHelper internet;
    internet.SetRoutingHelper (zoznam);
    internet.Install (senzorNet);

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO ("Priradenie adries IP.");
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer IpContainer = ipv4.Assign (allDevices);

    TypeId tid = TypeId :: LookupByName ("ns3 :: UdpSocketFactory");
    Ptr <Socket> recvSink = Socket :: CreateSocket (farmár.Get (0), tid); // uzol 0, Cieľ
    InetSocketAddress local = InetSocketAddress (Ipv4Address :: GetAny (), 80);
    recvSink-> Bind (lokálne);
    recvSink-> SetRecvCallback (MakeCallback (& ​​ReceivePacket));

    pre (int j = 0; j <25; j ++)
    {
        pre (int k = 0; k <5; k ++)
        {

            Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & goToNewPosition, farmár.Get (0), & dáta, & data2, k);

            pre (uint32_t i = 0; i <senzory.GetN (); i ++)
            {
                Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
                positionAlloc-> Add (Vektor (135,0 + i * 5, 220,0, 0,0));
                mobility.SetPositionAllocator (positionAlloc);

                Ptr <Socket> source = Socket :: CreateSocket (senzory.Get (i), tid); // node i, Zdroj
                InetSocketAddress remote = InetSocketAddress (Ipv4Address :: GetBroadcast (), 80);
                vstup-> SetAllowBroadcast (true);
                vstup-> Connect (diaľkové);

                / ** pripojiť zdroje sledovania ** /
                                     / ************************************************* ************************** /
                                     // všetky stopy sú pripojené k uzlu 1 (Cieľ)
                                     // Zdroj energie
                                     Ptr <BasicEnergySource> basicSourcePtr = DynamicCast <BasicEnergySource> (sources.Get (0));
                basicSourcePtr-> TraceConnectW WithoutContext ("RemainingEnergy", MakeCallback (& ​​RemainingEnergy));
                // energetický model zariadenia
                Ptr <DeviceEnergyModel> basicRadioModelPtr =
                basicSourcePtr-> FindDeviceEnergyModels ( "NS3 :: WifiRadioEnergyModel") Get (0).
                        NS_ASSERT (basicRadioModelPtr! = 0);
                basicRadioModelPtr-> TraceConnectW WithoutContext ("TotalEnergyConsumption", MakeCallback (& ​​TotalEnergy));
                // zberač energie
                Ptr <BasicEnergyHarvester> basicHarvesterPtr = DynamicCast <BasicEnergyHarvester> (kombajny.Get (1));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("HarvestedPower", MakeCallback (& ​​HarvestedPower));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("TotalEnergyHarvested", MakeCallback (& ​​TotalEnergyHarvested));
                / ************************************************* ************************** /

                  / ** nastavenie simulácie ** /
                                  // začať prevádzku

                                  Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & GenerateTraffic, zdroj, PpacketSize,
                                                         senzorNet.Get (0), numPackets, interPacketInterval);
            }

        }
    }
    Simulátor :: Stop (sekundy (10,0));

    AnimationInterface anim ("netanim_pole.xml");

    pre (int i = 0; i <25; i ++) {
        anim.UpdateNodeSize (i, 5, 5);
        anim.UpdateNodeColor (i, 0, 128, 0);
    }
    anim.UpdateNodeSize (25, 10, 10);
    anim.UpdateNodeColor (farmár.Get (0), 255, 0, 0);
    anim.UpdateNodeDescription (farmer.Get (0), "pol poľnohospodárodar");


    Simulátor :: Run ();
    Simulátor :: Destroy ();
//
    graf.AddDataset (dáta);
    std :: ofstream plotFile ("graf_pole.plt");
    graf.GenerateOutput (plotFile);
    plotFile.close ();
    systém ("gnuplot graf_pole.plt");

    graf2.AddDataset (Data2);
    std :: ofstream plotFile2 ("graf_pole2.plt");
    graf2.GenerateOutput (plotFile2);
    plotFile2.close ();
    systém ("gnuplot graf_pole2.plt");

    graf3.AddDataset (data3);
    std :: ofstream plotFile3 ("graf_pole3.plt");
    graf3.GenerateOutput (plotFile3);
    plotFile3.close ();
    systém ("gnuplot graf_pole3.plt");

    návrat 0;
} n20 n21 n22 n23 n24
        n20 n21 n22 n23 n24
// n15 n16 n17 n18 n19
n nn n11 n12 n13 n14
// n5 n6 n7 n8 n9
// n0 n1 n2 n3 n4

#include "ns3 / core-module.h"
#include "ns3 / network-module.h"
#include "ns3 / mobility-module.h"
#include "ns3 / config-store-module.h"
#include "ns3 / wifi-module.h"
#include "ns3 / energy-module.h"
#include "ns3 / internet-module.h"
#include "ns3 / olsr-helper.h"
#include "ns3 / netanim-module.h"
#include "ns3 / gnuplot.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>

pomocou namespace ns3;

NS_LOG_COMPONENT_DEFINE ("EnergyWithHarvestingExample");

long globalCounter = 0;
long globalCounter2 = 0;
Gnuplot2dDataset data3;
long counter_x = 0;
float graphArray [4];

statický inline std :: string
PrintReceivedPacket (adresa & z)
{
InetSocketAddress iaddr = InetSocketAddress :: ConvertFrom (from);

std :: ostringstream oss;
oss << "- nReceived jeden paket! Socket:" << iaddr.GetIpv4 ()
<< "port:" << iaddr.GetPort ()
<< "at time =" << Simulátor :: Teraz () .GetSeconds ()
<< "n--";

return oss.str ();
}

/ **
param socket Pointer na zásuvku.
*
* Paket príjem drez.
* /
prázdno
ReceivePacket (zásuvka Ptr <Socket>)
{
    Paket Ptr <Packet>;
    Adresa z;
    zatiaľ čo ((packet = socket-> RecvFrom (od)))
    {
        if (paket-> GetSize ()> 0)
        {
            NS_LOG_UNCOND (PrintReceivedPacket (from));
            globalCounter + = paket-> GetSize ();
            globalCounter2 ++;
        }
    }
}

/ **
param socket Pointer na zásuvku.
veľkosť pktSize Veľkosť paketu.
* n n Ukazovateľ na uzol.
* Param pktCount Počet paketov, ktoré sa majú vygenerovať.
* Param pktInterval Interval odosielania paketov.
*
* Generátor prevádzky.
* /
statické neplatné
GenerateTraffic (Ptr <Socket> socket, uint32_t pktSize, Ptr <Node> n,
uint32_t pktCount, Čas pktInterval)
{
if (pktCount> 0)
{
socket-> Send (Vytvoriť <Packet> (pktSize));
Simulator :: Plán (pktInterval, & GenerateTraffic, socket, pktSize, n,
        pktCount - 1, pktInterval);
}
inak
{
socket-> Close ();
}
}

/// Funkcia sledovania zostávajúcej energie v uzle.
prázdno
        Zvyšná energia (dvojitá stará hodnota, dvojitá zvyšná energia)
{
std :: cout << Simulátor :: Teraz () .GetSeconds ()
<< "s Aktuálna zostávajúca energia =" << << zostávajúceEnergy << "J" << std :: endl;
if (counter_x% 3125 == 0)
{
data3.Add (Simulator :: Now () .GetSeconds (), zostávajúceEnergy);
}
counter_x ++;
}

/// Funkcia sledovania celkovej spotreby energie v uzle.
prázdno
TotalEnergy (double oldValue, double totalEnergy)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková spotreba energie rádia =" << totalEnergy << "J" << std :: endl;
}

/// Stopová funkcia pre energiu zberanú energetickým kombajnom.
prázdno
Zberač (double oldValue, double harvestedPower)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Aktuálny zber výkonu =" << harvestedPower << "W" << std :: endl;
}

/// Stopová funkcia celkovej energie zozbieranej uzlom.
prázdno
TotalEnergyHarvestované (double oldValue, double TotalEnergyHarvested)
{
    std :: cout << Simulátor :: Teraz () .GetSeconds ()
                << "s Celková energia zozbieraná kombajnom ="
                << TotalEnergyHarvested << "J" << std :: endl;
}

void standardDeviation (float data [], float temp []) {
    float sum = 0,0, priemer, štandardDeviation = 0,0;
    int i;

    pre (i = 0; i <5; ++ i) {
        sum + = údaje [i];
    }
    mean = sum / 5;

    pre (i = 0; i <5; ++ i)
    standardDeviation + = pow (údaje [i] - priemer, 2);

    teplota [0] = stredná hodnota;
    temp [1] = sqrt (standardDeviation / 5);

}

static void goToNewPosition (údaje Ptr <Node> farmár, Gnuplot2dDataset *, Gnuplot2dDataset * data2, int k) {
    MobilitaPomoc mobility;
    int strana = rand ()% 4;
    int poloha = rand ()% 250;
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();

    if (strana == 0) {
        positionAlloc-> Add (Vector (0.0, poloha, 0.0));
    }
    else if (strana == 1) {
        positionAlloc-> Add (Vector (poloha, 0,0, 0,0));
    }
    else if (strana == 2) {
        positionAlloc-> Add (Vektor (250,0, poloha, 0,0));
    }
    else if (strana == 3) {
        positionAlloc-> Add (Vector (poloha, 250,0, 0,0));
    }

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ( "NS3 :: ConstantPositionMobilityModel");
    mobility.Install (poľnohospodár);

    graphArray [k] = globalCounter;
    globalCounter = 0;
    if (k == 4) {
        float tmpGraphArray [2];
        standardDeviation (graphArray, tmpGraphArray);
        if (tmpGraphArray [0]! = 0 && tmpGraphArray [1]! = 0)
        data-> Pridať (Simulator :: Now (). GetSeconds (), tmpGraphArray [0], 0.0, tmpGraphArray [1]);
    }
    data2-> Pridať (Simulator :: Now (). GetSeconds (), globalCounter2);

}

int
main (int argc, char * argv [])
{

    uint32_t numNodes = 25; // 5x5
    dvojitá vzdialenosť = 62,5; // [m] vzdialenost medzi uzlami asi
    std :: string phyMode ("DsssRate1Mbps");
    double Prss = -20; // dBm
    uint32_t PpacketSize = 200; // bajty
    bool verbose = false;

    // simulačné parametre
    uint32_t numPackets = 10000; // počet paketov, ktoré sa majú odoslať
    dvojitý interval = 1; // sekúnd
    double startTime = 0,0; // sekúnd
    dvojitá vzdialenosťToRx = 10,0; // metre
    dvojitý posun = 81;

    // Premenné energetického kombajnu
    double harvestingUpdateInterval = 1; // sekúnd

    Gnuplot graf ("graf_pole.svg");
    graf.SetTerminal ( "SVG");
    graf.SetTitle ("Graf zavislosti suctu velkosti prijatých paketov od casu");
    graf.SetLegend ("Cas [s]", "SUM velkosti prijatych paketov [B]");
    Gnuplot2dDataset dáta;
    data.SetTitle ( "Vysledok");
    data.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    data.SetErrorBars (Gnuplot2dDataset :: XY);

    Gnuplot graf2 ("graf_pole2.svg");
    graf2.SetTerminal ( "SVG");
    graf2.SetTitle ("Graf zavislosti poctu prijatých paketov od casu");
    graf2.SetLegend ("Cas [s]", "Pocet přijatých paketov");
    Gnuplot2dDataset data2;
    data2.SetTitle ( "Vysledok");
    data2.SetStyle (Gnuplot2dDataset :: LINES_POINTS);
    //data2.SetErrorBars(Gnuplot2dDataset::XY);

    Gnuplot graf3 ("graf_pole3.svg");
    graf3.SetTerminal ( "SVG");
    graf3.SetTitle ("Graf zavislosti zostatku energie od casu.");
    graf3.SetLegend ("Cas [s]", "Zostavajuca energia");
    data3.SetTitle ( "Vysledok");
    data3.SetStyle (Gnuplot2dDataset :: LINES_POINTS);

    CommandLine cmd;
    cmd.AddValue ("phyMode", "režim Wifi Phy", phyMode);
    cmd.AddValue ("Prss", "Určený primárny RSS (dBm)", Prss);
    cmd.AddValue ("PpacketSize", "veľkosť odoslaného paketu aplikácií", PpacketSize);
    cmd.AddValue ("numPackets", "Celkový počet odoslaných paketov", numPackets);
    cmd.AddValue ("startTime", "Čas začiatku simulácie", startTime);
    cmd.AddValue ("distanceToRx", "vzdialenosť osi X medzi uzlami", distanceToRx);
    cmd.AddValue ("verbose", "Zapnúť všetky komponenty protokolu zariadenia", verbose);
    cmd.Parse (argc, argv);

    // Konverzia na objekt času
    Time interPacketInterval = sekundy (interval);

    // vypne fragmentáciu pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: FragmentationThreshold",
                          StringValue ("2200"));
    // vypnúť RTS / CTS pre snímky pod 2200 bajtov
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: RtsCtsThreshold",
                          StringValue ("2200"));
    // Fix non-unicast dátovej rýchlosti, aby bola rovnaká ako unicast
    Config :: SetDefault ("ns3 :: WifiRemoteStationManager :: NonUnicastMode",
                          StringValue (phyMode));

    // pre uzly> senzory
    NodeContainer senzory;
    senzors.Create (numNodes);

    // pre uzol> polnohospodar
    NodeContainer farmár;
    farmer.Create (1);

    // pre celú senzorickú sieť
    NodeContainer senzorNet;
    senzorNet.Add (poľnohospodár);
    senzorNet.Add (senzory);


    Nižšie uvedený súbor pomocníkov nám pomôže zostaviť WiFi NIC, ktoré chceme
    WifiHelper wifi;
    ak (podrobne)
    {
        wifi.EnableLogComponents ();
    }
    wifi.SetStandard (WIFI_PHY_STANDARD_80211g);

    / ** Wifi PHY ** /

              YansWifiPhyHelper wifiPhy = YansWifiPhyHelper :: Default ();
    wifiPhy.Set ("RxGain", DoubleValue (-30));
    wifiPhy.Set ("TxGain", DoubleValue (offset + Prss));
    wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-90,0));
    wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-61.8));

    / ** wifi kanál ** /
              YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3 :: ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3 :: JakesPropagationLossModel");
    // vytvoriť wifi kanál
    Ptr <YansWifiChannel> wifiChannelPtr = wifiChannel.Create ();
    wifiPhy.SetChannel (wifiChannelPtr);

    / ** Vrstva MAC ** /
                // Pridajte MAC a vypnite riadenie rýchlosti
                WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager ("ns3 :: ConstantRateWifiManager", "DataMode",
                                  StringValue (phyMode), "ControlMode",
                                  StringValue (phyMode));
    // Nastavte ho na režim ad-hoc
    wifiMac.SetType ("ns3 :: AdhocWifiMac");

    / ** nainštalovať PHY + MAC ** /
                            NetDeviceContainer senzorDevices = wifi.Install (wifiPhy, wifiMac, senzory);
    NetDeviceContainer farmerDevice = wifi.Install (wifiPhy, wifiMac, farmár);
    NetDeviceContainer allDevices = wifi.Install (wifiPhy, wifiMac, senzorNet);

    / ** mobilita ** /
      MobilitaPomoc mobility;
    // senzory
    mobility.SetPositionAllocator ("ns3 :: GridPositionAllocator",
                                   "MinX", DoubleValue (0.0), // Xova os zacina v 0
                                   "MinY", DoubleValue (0.0), // Yos zazina v 0
                                   "DeltaX", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Xovej osi
                                   "DeltaY", DoubleValue (vzdialenosť), // vzdialenost medzi uzlami po Yovej osi
                                   "GridWidth", UintegerValue (5), // 5 uzlov v jednom riadku
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3 :: ConstantPositionMobilityModel");
    mobilita.Inštalovať (senzory);

    // poľnohospodár
    Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
    positionAlloc-> Add (Vektor (100,0, 250,0, 0,0));
    mobility.SetPositionAllocator (positionAlloc);
    // mobility.SetPositionAllocator ("ns3 :: RandomRectanglePositionAllocator",
    // "X", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 50.0]"), "Y", StringValue ("ns3 :: UniformRandomVariable [Min = -20.0 | Max = 20.0]"));
    // mobility.SetMobilityModel ("ns3 :: RandomRectanglePositionAllocator");
    // mobility.SetMobilityModel ("ns3 :: RandomWalk2dMobilityModel", "Bounds", RectangleValue (Obdĺžnik (-100, 250, -100, 250)));

    mobilita.


    / ** Energetický model ** /
                     / ************************************************* ************************** /
                     /* Zdroj energie */
                     BasicEnergySourceHelper basicSourceHelper;
    // konfigurácia zdroja energie
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (3.0));
    // nainštalovať zdroj
    EnergySourceContainer sources = basicSourceHelper.Install (senzory);
    / * energetický model zariadenia * /
                          WifiRadioEnergyModelHelper radioEnergyHelper;
    // konfigurovať model rádiovej energie
    radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0,0290));
    radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0,0080));
    // nainštalovať model zariadenia
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (senzory, zdroje);

    / * energetický kombajn * /
                    BasicEnergyHarvesterHelper basicHarvesterHelper;
    // konfigurácia zberača energie
    basicHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
    basicHarvesterHelper.Set ("HarvestablePower", StringValue ("ns3 :: UniformRandomVariable [Min = 0,0 | Max = 0,1]"));
    // nainštalovať kombajn na všetky zdroje energie
    EnergyHarvesterContainer harvestory = basicHarvesterHelper.Install (zdroje);

    / ************************************************* ************************************************** ************** /
      / ** Internetový zásobník ** /
                       OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;

    Ipv4ListRoutingHelper zoznam;
    list.Add (staticRouting, 0);
    list.Add (olsr, 1);

    InternetStackHelper internet;
    internet.SetRoutingHelper (zoznam);
    internet.Install (senzorNet);

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO ("Priradenie adries IP.");
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer IpContainer = ipv4.Assign (allDevices);

    TypeId tid = TypeId :: LookupByName ("ns3 :: UdpSocketFactory");
    Ptr <Socket> recvSink = Socket :: CreateSocket (farmár.Get (0), tid); // uzol 0, Cieľ
    InetSocketAddress local = InetSocketAddress (Ipv4Address :: GetAny (), 80);
    recvSink-> Bind (lokálne);
    recvSink-> SetRecvCallback (MakeCallback (& ​​ReceivePacket));

    pre (int j = 0; j <25; j ++)
    {
        pre (int k = 0; k <5; k ++)
        {

            Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & goToNewPosition, farmár.Get (0), & dáta, & data2, k);

            pre (uint32_t i = 0; i <senzory.GetN (); i ++)
            {
                Ptr <ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator> ();
                positionAlloc-> Add (Vektor (135,0 + i * 5, 220,0, 0,0));
                mobility.SetPositionAllocator (positionAlloc);

                Ptr <Socket> source = Socket :: CreateSocket (senzory.Get (i), tid); // node i, Zdroj
                InetSocketAddress remote = InetSocketAddress (Ipv4Address :: GetBroadcast (), 80);
                vstup-> SetAllowBroadcast (true);
                vstup-> Connect (diaľkové);

                / ** pripojiť zdroje sledovania ** /
                                     / ************************************************* ************************** /
                                     // všetky stopy sú pripojené k uzlu 1 (Cieľ)
                                     // Zdroj energie
                                     Ptr <BasicEnergySource> basicSourcePtr = DynamicCast <BasicEnergySource> (sources.Get (0));
                basicSourcePtr-> TraceConnectW WithoutContext ("RemainingEnergy", MakeCallback (& ​​RemainingEnergy));
                // energetický model zariadenia
                Ptr <DeviceEnergyModel> basicRadioModelPtr =
                basicSourcePtr-> FindDeviceEnergyModels ( "NS3 :: WifiRadioEnergyModel") Get (0).
                        NS_ASSERT (basicRadioModelPtr! = 0);
                basicRadioModelPtr-> TraceConnectW WithoutContext ("TotalEnergyConsumption", MakeCallback (& ​​TotalEnergy));
                // zberač energie
                Ptr <BasicEnergyHarvester> basicHarvesterPtr = DynamicCast <BasicEnergyHarvester> (kombajny.Get (1));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("HarvestedPower", MakeCallback (& ​​HarvestedPower));
                basicHarvesterPtr-> TraceConnectW WithoutContext ("TotalEnergyHarvested", MakeCallback (& ​​TotalEnergyHarvested));
                / ************************************************* ************************** /

                  / ** nastavenie simulácie ** /
                                  // začať prevádzku

                                  Simulator :: Schedule (Seconds (startTime + j * 0.7 + k * 0.4), & GenerateTraffic, zdroj, PpacketSize,
                                                         senzorNet.Get (0), numPackets, interPacketInterval);
            }

        }
    }
    Simulátor :: Stop (sekundy (10,0));

    AnimationInterface anim ("netanim_pole.xml");

    pre (int i = 0; i <25; i ++) {
        anim.UpdateNodeSize (i, 5, 5);
        anim.UpdateNodeColor (i, 0, 128, 0);
    }
    anim.UpdateNodeSize (25, 10, 10);
    anim.UpdateNodeColor (farmár.Get (0), 255, 0, 0);
    anim.UpdateNodeDescription (farmer.Get (0), "pol poľnohospodárodar");


    Simulátor :: Run ();
    Simulátor :: Destroy ();
//
    graf.AddDataset (dáta);
    std :: ofstream plotFile ("graf_pole.plt");
    graf.GenerateOutput (plotFile);
    plotFile.close ();
    systém ("gnuplot graf_pole.plt");

    graf2.AddDataset (Data2);
    std :: ofstream plotFile2 ("graf_pole2.plt");
    graf2.GenerateOutput (plotFile2);
    plotFile2.close ();
    systém ("gnuplot graf_pole2.plt");

    graf3.AddDataset (data3);
    std :: ofstream plotFile3 ("graf_pole3.plt");
    graf3.GenerateOutput (plotFile3);
    plotFile3.close ();
    systém ("gnuplot graf_pole3.plt");

    návrat 0;
}