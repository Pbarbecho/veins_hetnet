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

    {
    socket.setOutputGate(gate("udpOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    }

    //schedule server message to starttime at omnet.ini
    sendPacket = new cMessage("sendPacket");
    scheduleAt(simTime()+startTime, sendPacket);


}


void SimpleServerApp::handleMessage(cMessage *msg){
    // Send Server message
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
   if (pkt == 0) {throw cRuntimeError("Unknown packet type");}

   std::cerr<<pkt->getSender()<<"  "<<simTime()<<endl;

   delete msg;
}


void SimpleServerApp::sendHetVNetDemoPacket()
{
    if (nextSequenceNumber<send_N_packets){ //limita el number de msgs enviados por el nodo[0]
        //Create server demo packet
        HetVNetDemoPacket *server_msg = new HetVNetDemoPacket("server");
        inet::L3Address destAddressWlan = inet::L3AddressResolver().resolve("10.0.1.86");
        //update packet fields
        server_msg->setIsWlan(0);
        server_msg->setCreationTime(simTime());
        server_msg->setByteLength(packetSizeBytes);
        server_msg->setSender(getParentModule()->getId());
        server_msg->setDsttype("car");
        // send mesg
        socket.sendTo(server_msg, destAddressWlan, 1000);
    }
    //send packet and schedule the next server message as specified in omnetini packetinterval
    nextSequenceNumber++;
    scheduleAt(simTime() + packetInterval, sendPacket);

}
