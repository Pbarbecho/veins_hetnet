package org.car2x.veins.subprojects.veins_hetvnet.server;

import inet.applications.contract.IUDPApp;

simple SimpleServerApp like IUDPApp {
	parameters:
		int localPort = default(4242);
		bool debug = default(false);
		bool infoLogging = default(true);
	
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
