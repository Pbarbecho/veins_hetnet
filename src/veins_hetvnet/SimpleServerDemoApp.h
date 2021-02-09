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

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/applications/base/ApplicationBase.h"

#include "inet/common/INETDefs.h"
#include "HetVNetDemoPacket_m.h"

#include <fstream>

#include "veins/modules/mobility/traci/TraCIScenarioManager.h"



class SimpleServerApp: public cSimpleModule {

      inet::UDPSocket socket;
      cMessage* sendPacket;
      int packetSizeBytes;
      simtime_t packetInterval;
      int send_N_packets;
      int nextSequenceNumber;
      void sendHetVNetDemoPacket();

public:
      ~SimpleServerApp();
      SimpleServerApp();

protected:
    virtual int numInitStages() const
    {
        return inet::NUM_INIT_STAGES;
    }
    void initialize(int stage);
    void handleMessage(cMessage* msg);

};
