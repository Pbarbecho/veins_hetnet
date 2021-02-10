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
// Based on an adapted version of the SimuLTE `cars` simulation
//

package veins_hetvnet.simulations;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.eNodeB;
import lte.epc.PgwStandardSimplified;
import lte.world.radio.LteChannelControl;
import org.car2x.veins.subprojects.veins_inet.VeinsInetManager;

network Highway
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)

        @display("i=block/network2;bgb=842,1260;bgi=background/centro1");
        @display("bgb=2595.964,3027.976");
    submodules:
        routingRecorder: RoutingTableRecorder {
            @display("p=2471.4202,521.528;is=s");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=2475.312,720.02");
            config = xmldoc("demo.xml");
        }
        veinsManager: VeinsInetManager {
            @display("p=2475.312,883.4841;is=s");
        }
        car[0]: veins_hetvnet.simulations.Car;
        channelControl: LteChannelControl {
            @display("p=2471.4202,346.38803;is=s");
        }
        binder: LteBinder {
            @display("p=2471.4202,1284.3601;is=s");
        }
        server: StandardHost {
            @display("p=2475.312,128.436;is=n;i=device/server");
        }
        router: Router {
            @display("p=2265.144,124.54401;i=device/smallrouter");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=2008.2721,128.436;is=l");
        }
        eNodeB1: eNodeB {
            @display("p=174.56999,301.53;is=vl");
        }
        eNodeB2: eNodeB {
            @display("p=210.16801,2650.4521;is=vl");
        }
        //eNodeB3: eNodeB {
        //    @display("p=498,782;is=vl");
        //}
 		//eNodeB4: eNodeB {
        //    @display("p=370,1016;is=vl");
        //}
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=2471.4202,1043.056");
        }


    connections allowunconnected:
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNodeB1.ppp;
        pgw.pppg++ <--> Eth10G <--> eNodeB2.ppp;
        //pgw.pppg++ <--> Eth10G <--> eNodeB3.ppp;
        //pgw.pppg++ <--> Eth10G <--> eNodeB4.ppp;

        eNodeB1.x2++ <--> Eth10G <--> eNodeB2.x2++;
        //eNodeB1.x2++ <--> Eth10G <--> eNodeB3.x2++;
        //eNodeB1.x2++ <--> Eth10G <--> eNodeB4.x2++;

        //eNodeB2.x2++ <--> Eth10G <--> eNodeB3.x2++;
        //eNodeB2.x2++ <--> Eth10G <--> eNodeB4.x2++;

        //eNodeB3.x2++ <--> Eth10G <--> eNodeB4.x2++;
}

