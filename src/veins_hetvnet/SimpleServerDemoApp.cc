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

#include "SimpleServerDemoApp.h"



Define_Module(SimpleServerApp);

SimpleServerApp::SimpleServerApp()
{
    sendPacket = nullptr;
}

SimpleServerApp::~SimpleServerApp()
{
    cancelAndDelete(sendPacket);
}


void SimpleServerApp::initialize(int stage)
{
    EV_TRACE << "HetVNetDemoApp::initialize - stage " << stage << std::endl;
    cSimpleModule::initialize(stage);

    if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

    packetSizeBytes = par("packetSize");
    packetInterval = par("period");
    simtime_t startTime = par("startTime");
    send_N_packets = par("send_N_packets");
    nextSequenceNumber= 0;
    DstAppCarPort = par("DstAppCarPort");
    // Destination IP of the forward car
    DtsIpFwdCar = inet::L3AddressResolver().resolve(par("DtsIpFwdCar").stringValue());
    // Capture message statistics
    statistics = par("statistics").stringValue();

    {
    socket.setOutputGate(gate("udpOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    }

    if (send_N_packets!=0){
        //schedule server message to starttime at omnet.ini
        sendPacket = new cMessage("sendPacket");
        scheduleAt(simTime()+startTime, sendPacket);
    }

}


void SimpleServerApp::handleMessage(cMessage *msg){
    // Process self and received messages
    // Process SELFMSG
    if (msg->isSelfMessage()) {
       if (msg == sendPacket) {
           sendHetVNetDemoPacket();
           return;
       }
       else {
           throw cRuntimeError("Unknown self message");
       }
   }

   // Process RX MSGS
   HetVNetDemoPacket* pkt = check_and_cast<HetVNetDemoPacket*>(msg);
   if (pkt == 0) {throw cRuntimeError("Unknown packet type");}

   // TO DO REPLY to sender car

   //Capture received message statistics
   CaptureMSG("server", "rx", pkt);

   delete msg;
}


void SimpleServerApp::sendHetVNetDemoPacket()
{
    if (nextSequenceNumber<send_N_packets){ //limita el number de msgs enviados por el nodo[0]
        //Create server demo packet
        HetVNetDemoPacket *server_msg = new HetVNetDemoPacket("server");

        //update packet fields
        server_msg->setIsWlan(0);
        server_msg->setCreationTime(simTime());
        server_msg->setByteLength(packetSizeBytes);
        server_msg->setSender(getParentModule()->getId());
        server_msg->setDsttype("car");
        // send mesg
        socket.sendTo(server_msg, DtsIpFwdCar, DstAppCarPort); // IP and Port of the destination CAR to forward the packet
    }
    //send packet and schedule the next server message as specified in omnetini packetinterval
    nextSequenceNumber++;
    scheduleAt(simTime() + packetInterval, sendPacket);

}



void SimpleServerApp::CaptureMSG(std::string cur_node_type, std::string state, HetVNetDemoPacket* packet)
{
    //Captured DATA from packet
    std::string dsttype = packet->getDsttype();             //destination server | car
    int nodeid = getParentModule()->getId();                //current node
    int type = packet->getIsWlan();                         // wlan(1) or lte(0) message
    int source = packet->getSender();                       //sender node
    int destination = packet->getByteLength();              // bytes at omnet.ini (used for channel computations)
    int msgID = packet->getSrcTreeId();                     // message unique identifier
    simtime_t creationtime = packet->getCreationTime(); // time of message creation in origin

    //Save DATA to external .csv file
    MSG_file.open(statistics, std::ios::out | std::ios::app);                                    //para leer datos ios::in
    if (MSG_file.is_open()){
        MSG_file<<cur_node_type<<","<<getParentModule()->getIndex()<<","<<dsttype<<","<<state<<","<<nodeid<<","<<type<<"," << source <<","<<destination<<","<<msgID<<","<< creationtime<<","<< simTime()<<'\n';
        MSG_file.close();
    }
    else std::cerr << "ERROR NO SE PUEDE ABRIR EL ARCHIVO  "<<statistics<< endl;
}
