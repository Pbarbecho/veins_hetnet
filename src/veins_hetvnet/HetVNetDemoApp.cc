//
// Copyright (C) 2012 Antonio Virdis, Daniele Migliorini, Matteo Maria Andreozzi, Giovanni Accongiagioco, Generoso Pagano, Vincenzo Pii.
// Copyright (C) 2019 Christoph Sommer <sommer@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: LGPL-3.0-only
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

//
// Based on an adapted version of the SimuLTE `AlertSender` and `AlertReceiver` simulation modules
//

#include <cmath>

#include "HetVNetDemoApp.h"

#include "inet/common/ModuleAccess.h"

Define_Module(HetVNetDemoApp);

HetVNetDemoApp::HetVNetDemoApp()
{
    sendPacket = nullptr;
}

HetVNetDemoApp::~HetVNetDemoApp()
{
    cancelAndDelete(sendPacket);
}

void HetVNetDemoApp::initialize(int stage)
{
    EV_TRACE << "HetVNetDemoApp::initialize - stage " << stage << std::endl;
    cSimpleModule::initialize(stage);

    if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

    packetSizeBytes = par("packetSize");
    packetInterval = par("period");
    localPortLte = par("localPortLte");
    localPortWlan = par("localPortWlan");
    destAddressLte_ = inet::L3AddressResolver().resolve(par("destAddressLte").stringValue());
    destAddressWlan = inet::L3AddressResolver().resolve(par("destAddressWlan").stringValue());
    simtime_t startTime = par("startTime");

    sigDemoPktSent = registerSignal("demoPktSent");
    sigDemoPktRcvDelay = registerSignal("demoPktRcvDelay");
    sigDemoPktRcvd = registerSignal("demoPktRcvd");

    nextSequenceNumber = 0;


    // Capture message statistics
    statistics = par("statistics").stringValue();
    // Number of message to send
    send_N_packets = par("send_N_packets").intValue();
    // forwarding node index
    Forwarding_nodeId = par("Forwarding_nodeId").intValue();

    {
        socketLte.setOutputGate(gate("udpOut"));
        socketLte.bind(localPortLte);

        inet::IInterfaceTable* ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
        inet::MulticastGroupList mgl = ift->collectMulticastGroups();
        socketLte.joinLocalMulticastGroups(mgl);
        inet::InterfaceEntry* ie = ift->getInterfaceByName("wlan");
        if (!ie) throw cRuntimeError("Wrong multicastInterface setting: no interface named wlan");
        socketLte.setMulticastOutputInterface(ie->getInterfaceId());
    }

    {
        socketWlan.setOutputGate(gate("udpOut"));
        socketWlan.bind(localPortWlan);

        inet::IInterfaceTable* ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
        inet::MulticastGroupList mgl = ift->collectMulticastGroups();
        socketWlan.joinLocalMulticastGroups(mgl);
        inet::InterfaceEntry* ie = ift->getInterfaceByName("wlan0");
        if (!ie) throw cRuntimeError("Wrong multicastInterface setting: no interface named wlan0");
        socketWlan.setMulticastOutputInterface(ie->getInterfaceId());
    }

    // TO DO Cambio para sincronizar el envio de packetes con el packet interval
    if (startTime > 0) {
        sendPacket = new cMessage("sendPacket");
        scheduleAt(simTime(), sendPacket);
    }
}

void HetVNetDemoApp::handleMessage(cMessage* msg)
{
    //=================== All NODES ==================

    // Send created message
    if (msg->isSelfMessage()) {
        if (msg == sendPacket) {
            sendHetVNetDemoPacket();
            return;
        }
        else {
            throw cRuntimeError("Unknown self message");
        }
    }

    HetVNetDemoPacket* pkt = check_and_cast<HetVNetDemoPacket*>(msg);
    if (pkt == 0) {
        throw cRuntimeError("Unknown packet type");
    }
    //Compute delay at reception
    simtime_t delay = simTime() - pkt->getCreationTime();

    //emit(sigDemoPktRcvDelay, delay);
    //emit(sigDemoPktRcvd, (long) 1);
    //EV_TRACE << "Packet received from " << (pkt->getIsWlan() ? "WLAN" : "LTE!") << " interface: SeqNo[" << pkt->getSequenceNo() << "] Delay[" << delay << "]" << std::endl;
    //=================================================

    //Capture received message statistics
    CaptureMSG("car", "rx", pkt);

    // FORWARD MSG just in case the multicast msg is received by the forward node
    if (getParentModule()->getIndex() == Forwarding_nodeId){
        forwardHetVNetDemoPacket(pkt);
    }

    delete msg;
}


void HetVNetDemoApp::forwardHetVNetDemoPacket(HetVNetDemoPacket* pkt){
    // El forwarding node (set in omnetpp.ini)
    // reenvia el packete recibido por WLAN a la IP LTE multicast

    HetVNetDemoPacket* packet = new HetVNetDemoPacket("LTE");
    packet->setIsWlan(0);
    packet->setSequenceNo(0);
    packet->setCreationTime(simTime());
    packet->setByteLength(packetSizeBytes);
    packet->setSender(pkt->getSender());
    packet->setForward(getParentModule()->getId());

    //Capture forwarded message statistics
    CaptureMSG("car", "fd", packet);

    //inet::L3Address server = inet::L3AddressResolver().resolve("192.168.0.1");
    socketLte.sendTo(packet, destAddressLte_, localPortLte);
}



void HetVNetDemoApp::sendHetVNetDemoPacket()
{
    //=============== SE   EJECUTA SOLO EN EL NODO[0] +=======================
    //================== SEND WLAN MSG FROM NODE[0] ==========================
    // El origen del mensaje siempre es el mismo node[o] specified in omentpp.ini

    if (nextSequenceNumber<send_N_packets){ //limita el number de msgs enviados por el nodo[0]
        HetVNetDemoPacket* packet = new HetVNetDemoPacket("WLAN");
        packet->setIsWlan(1);
        packet->setSequenceNo(nextSequenceNumber);
        packet->setCreationTime(simTime());
        packet->setByteLength(packetSizeBytes);
        packet->setSender(getParentModule()->getId());

        // Capture send message statistics
        CaptureMSG("car", "tx", packet);

        // Send to lower layers UDP
        socketWlan.sendTo(packet, destAddressWlan, localPortWlan);
        //emit(sigDemoPktSent, (long) 1);
    }

    nextSequenceNumber++;  // que hace ?????
    // Schedule new message each 1s in omnet.ini  period parameter
    scheduleAt(simTime() + packetInterval, sendPacket);
}


void HetVNetDemoApp::CaptureMSG(std::string node, std::string state, HetVNetDemoPacket* packet)
{
    //Captured DATA from packet
    int nodeid = getParentModule()->getId();                //current node
    int type = packet->getIsWlan();                         // wlan(1) or lte(0) message
    int source = packet->getSender();                       //sender node
    int destination = packet->getByteLength();              // bytes at omnet.ini (used for channel computations)
    int msgID = packet->getTreeId();                        // message unique identifier
    simtime_t creationtime = packet->getCreationTime(); // time of message creation in origin

    //Save DATA to external .csv file
    MSG_file.open(statistics, std::ios::out | std::ios::app);                                    //para leer datos ios::in
    if (MSG_file.is_open()){
        MSG_file<<node<<","<<state<<","<<nodeid<<","<<type<<"," << source <<","<<destination<<","<<msgID<<","<< creationtime<<","<< simTime()<<'\n';
        MSG_file.close();
    }
    else std::cerr << "ERROR NO SE PUEDE ABRIR EL ARCHIVO  "<<statistics<< endl;
}
