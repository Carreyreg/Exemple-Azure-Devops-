#include <string>
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/netsimulyzer-module.h>
#include "ns3/ns2-mobility-helper.h"
#include <iostream>
#include <ns3/color.h>
#include "ns3/color-palette.h"

using namespace ns3;

Ptr<netsimulyzer::LogStream> eventLog;

void
CourseChanged (Ptr<const MobilityModel> model)
{
  const auto nodeId = model->GetObject<Node> ()->GetId ();
  const auto position = model->GetPosition ();
  const auto velocity = model->GetVelocity ();

  *eventLog << Simulator::Now ().GetMilliSeconds () << ": Node [" << nodeId
            << "] Course Change Position: [" << position.x << ", " << position.y << ", "
            << position.z << "] "
            << "Velocity [" << velocity.x << ", " << velocity.y << ", " << velocity.z << "]\n";
}

int
main (int argc, char *argv[])
{

  
  // Définir la zone de recherche des drones 
  double minNodePosition = -50;
  double maxNodePosition = 50;

  double duration = 5;
  std::string outputFileName = "netsimulyzer-uavs-simulation.json";
  std::string droneModelPath = netsimulyzer::models::QUADCOPTER_UAV;

//création des drones 
  NodeContainer drones;
  drones.Create (4);

  Ns2MobilityHelper ns2 = Ns2MobilityHelper ("mobility_netsimulyzer.txt");
  ns2.Install ();

  
  for (auto iter = NodeList::Begin (); iter != NodeList::End (); iter++)
    {
      auto m = (*iter)->GetObject<MobilityModel> ();
      if (!m)
        continue;
      m->TraceConnectWithoutContext ("CourseChange", MakeCallback (&CourseChanged));
    }


  // ----  configuration de l'orchestrateur NetSimulyzer ----
  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> (outputFileName);



//Dans tous ce qui suit il y a l'intégration des objets 3D et des décorations
  auto decoration1 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration1->SetAttribute ("Model", StringValue("models/house_destroyed.obj"));
  decoration1->SetPosition ({20.0, 35.0, 0.0});
  //decoration->SetOrientation ({0.5,0.0,0.0});
  decoration1->SetAttribute ("Scale" , DoubleValue (0.9));




auto targetLocations1 = CreateObject<netsimulyzer::RectangularArea> (
      orchestrator, Rectangle{ 18, 26, 32, 38});

  // Identification de la zone
  targetLocations1->SetAttribute ("Name", StringValue ("Possible Node Locations"));

  // Marquer la target en rouge
  targetLocations1->SetAttribute ("BorderColor", netsimulyzer::Color3Value(ns3::netsimulyzer::RED));




auto decoration2 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration2->SetAttribute ("Model", StringValue("models/cellphone_tower.obj"));
  decoration2->SetPosition ({50.0, 50.0, 0.0});

 decoration2->SetAttribute ("Scale" , DoubleValue (0.2));




auto decoration3 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration3->SetAttribute ("Model", StringValue("models/house_destroyed.obj"));
  decoration3->SetPosition ({-30.0, 15.0, 0.0});
  //decoration->SetOrientation ({0.5,0.0,0.0});
  decoration3->SetAttribute ("Scale" , DoubleValue (0.9));


auto targetLocations2 = CreateObject<netsimulyzer::RectangularArea> (
      orchestrator, Rectangle{ -33, -24, 17, 11});

  // Identify the area
  targetLocations2->SetAttribute ("Name", StringValue ("Possible Node Locations"));

  // Mark with a light green color
  targetLocations2->SetAttribute ("BorderColor", netsimulyzer::Color3Value(ns3::netsimulyzer::RED));




auto decoration4 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration4->SetAttribute ("Model", StringValue("models/cellphone_tower.obj"));
  decoration4->SetPosition ({-50.0, -50.0, 0.0});
 decoration4->SetAttribute ("Scale" , DoubleValue (0.2));





auto decoration5 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration5->SetAttribute ("Model", StringValue("models/ambulance.obj"));
  decoration5->SetPosition ({-7.0, -63.0, 1.0});

 decoration5->SetAttribute ("Scale" , DoubleValue (1.5));



auto decoration6 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration6->SetAttribute ("Model", StringValue("models/tree.obj"));
  decoration6->SetPosition ({15.0, -35.0, 0.0});

 decoration6->SetAttribute ("Scale" , DoubleValue (0.1));

auto decoration6_2 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration6_2->SetAttribute ("Model", StringValue("models/tree.obj"));
  decoration6_2->SetPosition ({-15.0, 35.0, 0.0});

 decoration6_2->SetAttribute ("Scale" , DoubleValue (0.1));

auto decoration6_3 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration6_3->SetAttribute ("Model", StringValue("models/tree.obj"));
  decoration6_3->SetPosition ({41.0, 27.0, 0.0});

 decoration6_3->SetAttribute ("Scale" , DoubleValue (0.1));

auto decoration6_4 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration6_4->SetAttribute ("Model", StringValue("models/tree.obj"));
  decoration6_4->SetPosition ({35.0, -42.0, 0.0});

 decoration6_4->SetAttribute ("Scale" , DoubleValue (0.1));

auto decoration6_5 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration6_5->SetAttribute ("Model", StringValue("models/tree.obj"));
  decoration6_5->SetPosition ({22.0, -5.0, 0.0});

 decoration6_5->SetAttribute ("Scale" , DoubleValue (0.1));




auto decoration7 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration7->SetAttribute ("Model", StringValue("models/route.obj"));
  decoration7->SetPosition ({10.0, -62.0, 0.0});
  decoration7->SetOrientation ({0.0, 0.0, 90.0});
  decoration7->SetAttribute ("Scale" , DoubleValue (0.40));


auto decoration8 = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration8->SetAttribute ("Model", StringValue("models/logo.obj"));
  decoration8->SetPosition ({-50.0, -50.0, 12.0});
  decoration8->SetAttribute ("Scale" , DoubleValue (5));



  // Mark possible Node locations
  auto possibleNodeLocations = CreateObject<netsimulyzer::RectangularArea> (
      orchestrator, Rectangle{minNodePosition, maxNodePosition, minNodePosition, maxNodePosition});

  possibleNodeLocations->SetAttribute ("Name", StringValue ("Possible Node Locations"));

  // Marquer les cibles en rouge 
  possibleNodeLocations->SetAttribute ("FillColor", netsimulyzer::Color3Value{245U, 61U, 0U});

  auto infoLog = CreateObject<netsimulyzer::LogStream> (orchestrator);
  eventLog = CreateObject<netsimulyzer::LogStream> (orchestrator);


  
  netsimulyzer::NodeConfigurationHelper nodeConfigHelper (orchestrator);
  nodeConfigHelper.Set("EnableMotionTrail", BooleanValue(true));

  


  nodeConfigHelper.Set ("Model", StringValue (droneModelPath));
  nodeConfigHelper.Install (drones);

 
  

  Simulator::Stop (Seconds (duration));
  Simulator::Run ();

  *infoLog << "Scenario Finished\n";
  Simulator::Destroy ();
}
