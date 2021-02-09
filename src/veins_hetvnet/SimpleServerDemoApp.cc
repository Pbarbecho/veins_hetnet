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

    {
    socket.setOutputGate(gate("udpOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    }

    sendPacket = new cMessage("sendPacket");
    scheduleAt(simTime(), sendPacket);
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

   delete msg;
}


void SimpleServerApp::sendHetVNetDemoPacket()
{
    HetVNetDemoPacket *reply = new HetVNetDemoPacket("Server Reply");
    inet::L3Address destAddressWlan = inet::L3AddressResolver().resolve("10.0.8.25");
    socket.sendTo(reply, destAddressWlan, 1000);
    scheduleAt(simTime() + 1, sendPacket);
}
