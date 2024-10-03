
#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/buildings-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include <ns3/basic-energy-source-helper.h>
#include <ns3/basic-energy-source.h>
#include <ns3/device-energy-model-container.h>
#include <ns3/li-ion-energy-source-helper.h>
#include <ns3/li-ion-energy-source.h>
#include <cstdlib>

#include "ns3/flow-monitor-module.h"

using namespace ns3;



// Fonction qui récupere les metriques réseau à chaque seconde

void 
delayCalcul( Ptr<Ipv4FlowClassifier> classifier, Ptr<FlowMonitor> monitor){
    
    
    uint32_t SentPackets = 0;
    double ReceivedPackets = 0;
    double Throughput = 0;
    double timeInterval = 0.0;
    double Pdr = 0.0;
    double Reliab = 0;
    int j = 0;
    float PDC = 0.0;
    float PTD = 0;
    Time Delay;
    Time eedMs;
    double simTime = Simulator::Now().GetSeconds();
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        j = j + 1;
        SentPackets = i->second.txPackets;
        ReceivedPackets = i->second.rxPackets;
        if (SentPackets != 0) {
            Pdr = Pdr + (ReceivedPackets / SentPackets)*100;
            
            Reliab = ReceivedPackets / SentPackets; 
        }
        Delay = i->second.delaySum/60;
        if ((i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) != 0) {
            Throughput = Throughput + i->second.rxBytes * 8.0/(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024/1024; // en Kbps
        }
        /////PDC///////////////////////////////////////////////////////////////////////////////////////

        PDC = i->second.packetsDropped.size();

        /////PTD///////////////////////////////////////////////////////////////////////////////////////

        Time somme = i->second.delaySum;
        float somInSeconds = somme.GetSeconds();
        float somAsFloat = static_cast<float>(somInSeconds);

        float delayInSeconds = Delay.GetSeconds();
        float delayAsFloat = static_cast<float>(delayInSeconds);

        if (delayAsFloat != 0) {

            PTD = somAsFloat / delayAsFloat;

        } else {

            PTD = 0;
        }

        /////PFI///////////////////////////////////////////////////////////////////////////////////////

        uint32_t numPackets = i->second.txPackets;

        if ((numPackets - 1) > 0) {

            timeInterval = somAsFloat / (numPackets - 1);

        }

    }
    if (j != 0) {

        Pdr = Pdr / j;

    }

    float delayInSeconds = Delay.GetSeconds();
    float delayAsFloat = static_cast<float>(delayInSeconds); // EED en secondes

    std::ofstream out3("random_5g.csv", std::ios::app);      

    out3 << simTime << "," << Throughput << "," << delayAsFloat << "," << Pdr << std::endl;

    out3.close();


    Simulator::Schedule(Seconds(0.1), &delayCalcul, classifier, monitor);

}



int
main(int argc, char* argv[])
{
    uint16_t gNbNum = 2;
    uint16_t uavNumPergNb = 2;
  


// Lancement du code python qui genere la mobility 
  //  const char* command = "python ./mobility/Mc-aleatoire.py";
    //std::system(command);



    // Parametres du traffic
    uint32_t udpPacketSize = 15052; 
    uint32_t packet_interval = 5000;
    
    // Parametres de simulation
    
    Time simTime = Seconds(100);
    Time udpAppStartTime = MilliSeconds(0);
    uint16_t numerologyBwp1 = 4 ; //Numérologie ( 4 == le plus petit subcarrier spacing )
    double centralFrequencyBand1 = 28e9; //bande de frequence centrale
    double bandwidthBand1 = 100e6; // largeur de la bande passante 
    
      
    double totalTxPower = 10;
    std::string simTag = "default";
    std::string outputDir = "./";

    
    NS_ABORT_IF(centralFrequencyBand1 > 100e9);


    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    int64_t randomStream = 1;
   
    
    NodeContainer uavNodes;
    NodeContainer gNbNodes;
 


    gNbNodes.Create(gNbNum);
    uavNodes.Create(uavNumPergNb * gNbNum);
    
    // Configuration de la mobilité

    Ns2MobilityHelper ns2 = Ns2MobilityHelper ("random_mobility.txt");


   
    ns2.Install ();
    
    NodeContainer uavVideoContainer;

    

    for (uint32_t j = 0; j < uavNodes.GetN(); ++j)
    {
        Ptr<Node> uav = uavNodes.Get(j);   
        uavVideoContainer.Add(uav);
        
    }

    //creation du coeur 5g nsa (epchelper) et du beamforming
    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();

    // Installer le coeur et la technique de beamforming dans le réseau 
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);
    


    // configuration des bande de frequences 

    BandwidthPartInfoPtrVector allBwps;


    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // Nombre de carrier par bande 
    CcBwpCreator::SimpleOperationBandConf bandConf1(centralFrequencyBand1,
                                                    bandwidthBand1,
                                                    numCcPerBand,
                                                    BandwidthPartInfo::UMi_StreetCanyon);
    
    OperationBandInfo band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
    

    
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));


    nrHelper->InitializeOperationBand(&band1);

    double x = pow(10, totalTxPower / 10);
    double totalBandwidth = bandwidthBand1;


    allBwps = CcBwpCreator::GetAllBwps({band1}); //cc == component carrier
    

    Packet::EnableChecking();
    Packet::EnablePrinting();


    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));

    // Latence lien coeur (cas idéal )
    epcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Parametrage des antennes des UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Parametrage des antennes des 
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    uint32_t bwpIdForVideo = 0;
 
   
    // GNB routage entre le bearer et la bande passante

    // GBR_CONV_VIDEO 
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO",UintegerValue(bwpIdForVideo));
    
   
     // UAV routage entre le bearer et la bande passante    
    
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO", UintegerValue(bwpIdForVideo));
   
   //Installation des netdevice dans chaque type de noeuds


    NetDeviceContainer enbNetDev =
        nrHelper->InstallGnbDevice(gNbNodes, allBwps);

    NetDeviceContainer uavVideoNetDev = nrHelper->InstallUeDevice(uavVideoContainer, allBwps);
   

    randomStream += nrHelper->AssignStreams(enbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(uavVideoNetDev, randomStream);
    

    nrHelper->GetGnbPhy(enbNetDev.Get(0), 0)
        ->SetAttribute("Numerology", UintegerValue(numerologyBwp1));
    nrHelper->GetGnbPhy(enbNetDev.Get(0), 0)
        ->SetAttribute("TxPower", DoubleValue(10 * log10((bandwidthBand1 / totalBandwidth) * x)));


    
    for (auto it = enbNetDev.Begin(); it != enbNetDev.End(); ++it)
    {
        DynamicCast<NrGnbNetDevice>(*it)->UpdateConfig();
    }


    for (auto it = uavVideoNetDev.Begin(); it != uavVideoNetDev.End(); ++it)
    {
        DynamicCast<NrUeNetDevice>(*it)->UpdateConfig();
    }


// création du pgw et du noeuds distant qui est la station de controle 
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));

    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(uavNodes);

    Ipv4InterfaceContainer uavVideoIpIface =
        epcHelper->AssignUeIpv4Address(NetDeviceContainer(uavVideoNetDev));
    

      // Configurer la passerelle pour les uavs 
    for (uint32_t j = 0; j < uavNodes.GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> uavStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            uavNodes.Get(j)->GetObject<Ipv4>());
        uavStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }


    // attacher les Uavs au plus proche GNB
    nrHelper->AttachToClosestEnb(uavVideoNetDev, enbNetDev);
    

   uint16_t dlPortVideo = 1234;
    //création du serveur udp qui est installé dans la station de controle
    
    ApplicationContainer serverApps;

    // On le met listen au port 1234
    UdpServerHelper dlPacketSinkVideo(dlPortVideo);

   
    serverApps.Add(dlPacketSinkVideo.Install(remoteHost));
    
//création du client udp qui est installé dans les uavs et qui envoie du traffic vidéo à la station de controle 
    UdpClientHelper dlClientVideo;
    
    dlClientVideo.SetAttribute("RemotePort", UintegerValue(dlPortVideo));
    dlClientVideo.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientVideo.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    dlClientVideo.SetAttribute("Interval", TimeValue(Seconds(1.0 / packet_interval)));

    // Le bearer qui va porter le traffic vidéo 
    EpsBearer videoBearer(EpsBearer::GBR_CONV_VIDEO); // video conversationnel 


    // Filtre pour le traffic vidéo
    Ptr<EpcTft> videoTft = Create<EpcTft>();
    EpcTft::PacketFilter dlpfVideo;
    dlpfVideo.localPortStart = dlPortVideo;
    dlpfVideo.localPortEnd = dlPortVideo;
    
    videoTft->Add(dlpfVideo);
    
    
    // Installation de l'application cliente 

    ApplicationContainer clientApps;

    for (uint32_t i = 0; i < uavVideoContainer.GetN(); ++i) 
    {
        // on prend un noeud
        Ptr<Node> uav = uavVideoContainer.Get(i);
        // on prend le device de celui ci 
        Ptr<NetDevice> uavDevice = uavVideoNetDev.Get(i); 
        // on prend ensuite son adresse ip 
        Address uavAddress = uavVideoIpIface.GetAddress(i);

        //définir la destination de l'envoi du trafic vidéo à la station de controle 
        dlClientVideo.SetAttribute("RemoteAddress", AddressValue(internetIpIfaces.GetAddress(1)));
        clientApps.Add(dlClientVideo.Install(uav));
        // Activer le bearer
        nrHelper->ActivateDedicatedEpsBearer(uavDevice, videoBearer, videoTft);
    
    
    }

    
    // Lancement de l'application de streaming
    serverApps.Start(udpAppStartTime);
    clientApps.Start(udpAppStartTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);

    

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    
    endpointNodes.Add(uavNodes);
    endpointNodes.Add(remoteHost);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    Ptr<Ipv4FlowClassifier> classifier1 = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));


    
    
    Simulator::Stop(simTime);
    
  


   delayCalcul(classifier1,monitor);
    Simulator::Run();

    delayCalcul(classifier1,monitor);


/* Decommenter ça si vous voulez avoir des statistiques sur les performances globale du réseau par flot
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream outFile;
    std::string filename = outputDir + "/" + simTag;
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    double flowDuration = (simTime - udpAppStartTime).GetSeconds();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                << t.destinationAddress << ":" << t.destinationPort << ") proto "
                << protoStream.str() << "\n";
        outFile << "  Tx Packets: " << i->second.txPackets << "\n";
        outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
        outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / flowDuration / 1000.0 / 1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            averageFlowThroughput += i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            outFile << "  Throughput: " << i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "  Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            // outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << "
            // Mbps \n";
            outFile << "  Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / stats.size() << "\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    } */

    Simulator::Destroy();
    return 0;
}
