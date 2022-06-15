/* -- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"              // yo s mero

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SixthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//
int count_pckt =0,count_pckt1 =0,count_pckt2 =0,count_pckt3 =0;
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  if(Simulator::Now ().GetSeconds ()>=1. && Simulator::Now ().GetSeconds ()<=20.){
  	count_pckt1++;
  }else if(Simulator::Now ().GetSeconds ()>=5. && Simulator::Now ().GetSeconds ()<=25.){
  	count_pckt2++;
  }else if(Simulator::Now ().GetSeconds ()>=15. && Simulator::Now ().GetSeconds ()<=30.){
  	count_pckt3++;
  }
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
  count_pckt++;
}

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  std::cout<<"Enter the config. number:  ";
  std::string my_protocol = "1";
  std::cin>>my_protocol;
   
	NodeContainer nodes;
	nodes.Create (3);

	PointToPointHelper pointToPointfor_Node1;
	pointToPointfor_Node1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
	pointToPointfor_Node1.SetChannelAttribute ("Delay", StringValue ("3ms"));

	NetDeviceContainer connection1;
	connection1 = pointToPointfor_Node1.Install (nodes.Get(0),nodes.Get(1));
	NetDeviceContainer connection2;
	connection2 = pointToPointfor_Node1.Install (nodes.Get(0),nodes.Get(1));
	
	Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
	em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
	connection1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
	connection2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

	PointToPointHelper pointToPointfor_Node2;
	pointToPointfor_Node2.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
	pointToPointfor_Node2.SetChannelAttribute ("Delay", StringValue ("3ms"));

	NetDeviceContainer connection3;
	connection3 = pointToPointfor_Node2.Install (nodes.Get(2),nodes.Get(1));

	connection3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

	InternetStackHelper stack;
	stack.Install (nodes);

	Ipv4AddressHelper addressfor_connection1;
	addressfor_connection1.SetBase ("10.1.1.0", "255.255.255.252");
	Ipv4InterfaceContainer interfacesfor_connection1 = addressfor_connection1.Assign (connection1);
	
   	uint16_t Portfor_connection1 = 8080;
	Address sinkAddressfor_connection1 (InetSocketAddress (interfacesfor_connection1.GetAddress (1), Portfor_connection1));
	PacketSinkHelper packetSinkHelperfor_connection1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), Portfor_connection1));
	ApplicationContainer sinkAppsfor_connection1 = packetSinkHelperfor_connection1.Install (nodes.Get (1));
	sinkAppsfor_connection1.Start (Seconds (1.));
	sinkAppsfor_connection1.Stop (Seconds (20.));
	
	Ptr<Socket> Socketfor_connection1;
	if(my_protocol.compare("3")==0){
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	}else{
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewReno");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	}
	Ptr<MyApp> appfor_connection1 = CreateObject<MyApp> ();
	appfor_connection1->Setup (Socketfor_connection1, sinkAddressfor_connection1, 3000, 15000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (appfor_connection1);
	appfor_connection1->SetStartTime (Seconds (1.));
	appfor_connection1->SetStopTime (Seconds (20.));

	Ipv4AddressHelper addressfor_connection2;
	addressfor_connection2.SetBase ("10.1.2.0", "255.255.255.252");
	Ipv4InterfaceContainer interfacesfor_connection2 = addressfor_connection2.Assign (connection2);
	
   	uint16_t Portfor_connection2 = 8081;
	Address sinkAddressfor_connection2 (InetSocketAddress (interfacesfor_connection2.GetAddress (1), Portfor_connection2));
	PacketSinkHelper packetSinkHelperfor_connection2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), Portfor_connection2));
	ApplicationContainer sinkAppsfor_connection2 = packetSinkHelperfor_connection2.Install (nodes.Get (1));
	sinkAppsfor_connection2.Start (Seconds (5.));
	sinkAppsfor_connection2.Stop (Seconds (25.));

	Ptr<Socket> Socketfor_connection2;
	if(my_protocol.compare("3")==0){
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	}else{
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewReno");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
	}
	Ptr<MyApp> appfor_connection2 = CreateObject<MyApp> ();
	appfor_connection2->Setup (Socketfor_connection2, sinkAddressfor_connection2, 3000, 15000, DataRate ("1.5Mbps"));
	nodes.Get (0)->AddApplication (appfor_connection2);
	appfor_connection2->SetStartTime (Seconds (5.));
	appfor_connection2->SetStopTime (Seconds (25.));
	
	Ipv4AddressHelper addressfor_connection3;
	addressfor_connection3.SetBase ("10.1.3.0", "255.255.255.252");
	Ipv4InterfaceContainer interfacesfor_connection3 = addressfor_connection3.Assign (connection3);

   	uint16_t Portfor_connection3 = 8082;
	Address sinkAddressfor_connection3 (InetSocketAddress (interfacesfor_connection3.GetAddress (1), Portfor_connection3));
	PacketSinkHelper packetSinkHelperfor_connection3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), Portfor_connection3));
	ApplicationContainer sinkAppsfor_connection3 = packetSinkHelperfor_connection3.Install (nodes.Get (1));
	sinkAppsfor_connection3.Start (Seconds (15.));
	sinkAppsfor_connection3.Stop (Seconds (30.));

	Ptr<Socket> Socketfor_connection3;
	if(my_protocol.compare("1")==0){
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewReno");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection3 = Socket::CreateSocket (nodes.Get (2), TcpSocketFactory::GetTypeId ());
	}else{
		TypeId tid = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
		Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid));
		Socketfor_connection3 = Socket::CreateSocket (nodes.Get (2), TcpSocketFactory::GetTypeId ());
	}
	Ptr<MyApp> appfor_connection3 = CreateObject<MyApp> ();
	appfor_connection3->Setup (Socketfor_connection3, sinkAddressfor_connection3, 3000, 15000, DataRate ("1.5Mbps"));
	nodes.Get (2)->AddApplication (appfor_connection3);
	appfor_connection3->SetStartTime (Seconds (15.));
	appfor_connection3->SetStopTime (Seconds (30.));

	std::string cwndfile = "",pcapfile = "";
	if(my_protocol.compare("1")==0){
		cwndfile += "Configuration"+my_protocol+"connection";
		pcapfile += "Configuration"+my_protocol+"connection";
	}else if(my_protocol.compare("2")==0){
		cwndfile += "Configuration"+my_protocol+"connection";
		pcapfile += "Configuration"+my_protocol+"connection";
	}else{
		cwndfile += "Configuration"+my_protocol+"connection";
		pcapfile += "Configuration"+my_protocol+"connection";
	}
	AsciiTraceHelper asciiTraceHelper;
	Ptr<OutputStreamWrapper> streamfor_connection1 = asciiTraceHelper.CreateFileStream (cwndfile+"1.cwnd");
	Ptr<OutputStreamWrapper> streamfor_connection2 = asciiTraceHelper.CreateFileStream (cwndfile+"2.cwnd");
	Ptr<OutputStreamWrapper> streamfor_connection3 = asciiTraceHelper.CreateFileStream (cwndfile+"3.cwnd");
	Socketfor_connection1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,streamfor_connection1));
	Socketfor_connection2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,streamfor_connection2));
	Socketfor_connection3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,streamfor_connection3));

	PcapHelper pcapHelper;
	Ptr<PcapFileWrapper> pcapfilefor_connection1 = pcapHelper.CreateFile (pcapfile+"1.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> pcapfilefor_connection2 = pcapHelper.CreateFile (pcapfile+"2.pcap", std::ios::out, PcapHelper::DLT_PPP);
	Ptr<PcapFileWrapper> pcapfilefor_connection3 = pcapHelper.CreateFile (pcapfile+"3.pcap", std::ios::out, PcapHelper::DLT_PPP);
	connection1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,pcapfilefor_connection1));
	connection2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,pcapfilefor_connection2));
	connection3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop,pcapfilefor_connection3));

	Simulator::Stop (Seconds (30));
	Simulator::Run ();
	std::cout<<"The number of packets dropped are: "<<count_pckt<<"\n";
	std::cout<<"The number of packets dropped from sender 1 are: "<<count_pckt1<<"\n";
	std::cout<<"The number of packets dropped from sender 2 are: "<<count_pckt2<<"\n";
	std::cout<<"The number of packets dropped from sender 3are: "<<count_pckt3<<"\n";
	Simulator::Destroy ();

	return 0;
}
