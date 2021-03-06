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

package org.car2x.veins.subprojects.veins_hetvnet;

import inet.applications.contract.IUDPApp;

simple HetVNetDemoApp like IUDPApp
{
    parameters:
        int localPortLte = default(-1);
        int localPortWlan = default(-1);
        string destAddressLte;
        string destAddressWlan;
        int packetSize = default(10);
        double period @unit("s") = default(0.02s);  //Pablo cambio a 1 s en el omnetpp.ini
        double startTime @unit("s") = default(0s);
        string interfaceTableModule;
        string statistics = default("/root/Documents/EPN/msgs.txt");    
        int send_N_packets = default(1);  // Pablo add number of msg to send
        int Forwarding_nodeId = default(0); //Node that forward the packet to LTE multicas group
        int DstAppServerPort = default(1000); //use the same port configured for the udpapp server in omnet.ini
        
        @signal[demoPktSent];
        @statistic[demoPktSent](title="Demo packets sent"; unit=""; source="demoPktSent"; record=sum,vector);
        @signal[demoPktRcvDelay];
        @statistic[demoPktRcvDelay](title="Demo packet receive delay"; unit="s"; source="demoPktRcvDelay"; record=mean,vector);    
        @signal[demoPktRcvd]; 
        @statistic[demoPktRcvd](title="Demo packets received"; unit="s"; source="demoPktRcvd"; record=sum,vector);         

        @display("i=block/source");
    gates:
        output udpOut;
        input udpIn;
}
