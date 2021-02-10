package org.car2x.veins.subprojects.veins_hetvnet;

import inet.applications.contract.IUDPApp;



simple SimpleServerApp like IUDPApp 
{
	parameters:
		int localPort = default(4242);
		int packetSize = default(10);
        double period @unit("s") = default(0.02s);  //Pablo cambio a 1 s en el omnetpp.ini
        double startTime @unit("s") = default(0s);
        int send_N_packets = default(0);
        int DstAppCarPort = default(1000);
        string statistics = default("/root/Documents/EPN/msgs.txt");
        string DtsIpFwdCar = default("1.1.1.1");
	
	    gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
		
    
}
