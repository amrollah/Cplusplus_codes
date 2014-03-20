//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 *
 * Place code of "AP"s in this file.
 */

#include "sm.h"

#include "interface.h"
#include "frame.h"

#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <map>

using namespace std;

u_short checksum(u_short *,int);
void pars_infos(string, uint32*, byte*);
void print_ip(uint32);
char * hextodec(char);
void make_ip_packet(byte*, uint32, uint32, byte*,  byte* , byte);
byte * mac2array(uint64);
void print_mac(byte*);
uint64 ext_mac(byte *);
uint32 ext_IP(byte *);
byte * ip2array(uint32);
byte ahtob(char *);
uint32 getIP(void);
void remove_ip(uint32);
uint64 timer;
map<uint64, uint64> timemp;
map<uint32, uint64> mp;
map<uint32, uint32> ipmp;
uint32 ips[50];
byte gateway[6];
uint32 ipp;
uint32 ap2p;


SimulatedMachine::SimulatedMachine (const ClientFramework *cf, int count) :
	Machine (cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
}

SimulatedMachine::~SimulatedMachine () {
	// destructor...
}

void SimulatedMachine::initialize () {
	timer = 0;
	// TODO: Initialize your program here; interfaces are valid now.
	string cinfo = getCustomInformation();
	pars_infos(cinfo,ips,gateway);
}
void pars_infos(string s, uint32* ips, byte* gateway){
	cout << s<<endl;
	char *mac;
	char *ip;
	char* ii[50];
	unsigned int i = 0;
	unsigned int j = 0;
	mac = strtok((char*) s.c_str(), "\n");
	cout <<"mac is : " <<mac<<endl;
	ip = strtok(NULL, "\n");
	while(ip != NULL){
		cout <<"ip is : " <<ip<<endl;
		ii[j++] = ip;
		ip = strtok(NULL, "\n");
	}


	char* t1;
	t1 = strtok(mac, ":");
	while(t1 != NULL){
		cout << t1 << "--";
		gateway[i++] = ahtob(t1);
		t1 = strtok(NULL, ":");
	}
	cout <<endl;
	print_mac(gateway);
	string ss = "192.168.125.72";
	char * ap2ip =(char*) ss.c_str();
	ap2p = 0;
	cout << "ap2 ip is : ";
	char* temp;
	temp = strtok(ap2ip, ".");
	int ct = 0;
	while(temp != NULL){
		cout << temp <<"--";
		ap2p += (byte)atoi(temp);
		if(ct<3)
			ap2p = ap2p << 8;
		ct++;
		temp = strtok(NULL, ".");
	}
	cout <<endl;

	for (unsigned int i=0;i<j;i++){
		char* temp;
		temp = strtok(ii[i], ".");
		int ct = 0;
		while(temp != NULL){
			cout << temp <<"--";
			ips[i] += (byte)atoi(temp);
			if(ct<3)
				ips[i] = ips[i] << 8;
			ct++;
			temp = strtok(NULL, ".");
		}
		cout << "a new ip : "<< ips[i] <<endl;
	}
}
byte ahtob(char * s){
	byte r = 0;
	r = (byte)atoi(hextodec(s[0]));
	r = r << 4;
	r += (byte)atoi(hextodec(s[1]));
	return r;
}
char* hextodec(char c){
	switch (c) {
		case 'A':
			return "10";
		case 'B':
			return "11";
		case 'C':
			return "12";
		case 'D':
			return "13";
		case 'E':
			return "14";
		case 'F':
			return "15";
		case '0':
			return "0";
		case '1':
			return "1";
		case '2':
			return "2";
		case '3':
			return "3";
		case '4':
			return "4";
		case '5':
			return "5";
		case '6':
			return "6";
		case '7':
			return "7";
		case '8':
			return "8";
		case '9':
			return "9";
	}
}
/**
 * This method is called from the main thread.
 * Also ownership of the data of the frame is not with you.
 * If you need it, make a copy for yourself.
 *
 * You can also send frames using:
 * <code>
 *     bool synchronized sendFrame (Frame frame, int ifaceIndex) const;
 * </code>
 * which accepts one frame and the interface index (counting from 0) and
 * sends the frame on that interface.
 * The Frame class used here, encapsulates any kind of network frame.
 * <code>
 *     class Frame {
 *     public:
 *       uint32 length;
 *       byte *data;
 *
 *       Frame (uint32 _length, byte *_data);
 *       virtual ~Frame ();
 *     };
 * </code>
 */
void SimulatedMachine::processFrame (Frame frame, int ifaceIndex) {
	// TODO: process the raw frame; frame.data points to the frame's byte stream
	cout << "=========================="<<endl;
	cout << "Frame received at iface " << ifaceIndex <<
		" with length " << frame.length << endl;
//	for (unsigned int i = 0; i < frame.length; ++i) {
//		cout << (byte) frame.data[i] << " ";
//		}
	cout << endl;

	if(ifaceIndex == 0){
	byte node_mac[6];
	uint64  me;
	memcpy ((void *) node_mac, frame.data+6, 6);
//	memcpy (&me, node_mac, 6);
	me = ext_mac(node_mac);
	cout << "node mac is: ";
	print_mac(node_mac);
//	delete frame.data;
	timemp.insert(pair<uint64 , uint64> (me, timer));
	if((byte)frame.data[14] == 0){ // node has no IP
		cout << "node has no IP"<<endl;

		Frame Oframe (40, new byte[40]);
		cout << "0";
		memcpy (Oframe.data, node_mac, 6); // set destination MAC address
		cout << "00";
		memcpy (Oframe.data + 6, iface[0].mac, 6); // set source MAC address
		cout << "1";
		uint16 *type = reinterpret_cast<uint16 *> (&Oframe.data[2*6]);
		*type = htons (0x0500); // random type; should be changed...
		cout << "2";
		static const char O = 0x10;
		Oframe.data[14] = static_cast<byte>(O);
		cout << "3";
		memcpy (Oframe.data + 15, iface[0].mac, 6);
		cout << "4";
		memcpy (Oframe.data + 21, ip2array(iface[1].ip), 4);
		cout << "5";
		memcpy (Oframe.data + 25, ip2array(iface[0].ip), 4);
		cout << "6";
		uint32 ip_offer = getIP();
		memcpy (Oframe.data + 29, ip2array(ip_offer), 4);
		cout << " ip offer is : ";
		print_ip(ip_offer);
		cout << "7";
		sendFrame (Oframe, 0);

	}else if((byte)frame.data[14] == 0x01){ //This is an ACK for IP offer
		cout << "This is an ACK for IP offer"<<endl;
		uint32 ip = ext_IP(frame.data + 29);
		uint64 mac_u64;
		memcpy (&mac_u64, node_mac, 6);
		mp.insert(pair<uint32 , uint64> (ip, mac_u64));
		print_ip(ip);
		cout << "--> ";
		print_mac(node_mac);
		cout <<"map size : " << mp.size() << endl;
		remove_ip(ip);
	}else if((byte)frame.data[14] == 0xFF){ // node has IP and home as bellow
		cout << "node has IP and home as bellow"<<endl;
		uint32 h_ip,ip;
		h_ip = ext_IP(frame.data+15);
		cout << "home ip is: ";
		print_ip(h_ip);
		ip = ext_IP(frame.data+19);
		ipp = ip;
		cout << "node ip is: ";
		print_ip(ip);

		if(h_ip == iface[1].ip){
			cout << "i am your home" << endl;
			int t = ipmp.erase(ip);
			cout << ((t>0) ? "one item is removed from ipmp. new map size is: " : "ipmp map size: ") << ipmp.size()<<endl;
		}else{// should inform home
			cout <<"i am your foreign"<<endl;
			Frame APframe (200 , new byte(200));
			byte protocol = (byte)(0x60);
			make_ip_packet(APframe.data,h_ip,iface[1].ip,iface[1].mac,gateway,protocol);
			memcpy (APframe.data+34, &ip, 4);
//			cout << ((sendFrame(APframe,0)==0)? "Fail : " : "OK ") << "   ))))(((((   ";
			cout << ((sendFrame(APframe,1) == 0)? "Fail" : "OK  ");
			u_short ck = checksum((u_short*)(APframe.data+34),4);
			memcpy (APframe.data+38, &ck, 2);
			cout << "AP message sent for AP"<<endl;

		}
	}else if(frame.data[23] == (byte)0x01){//ICMP reply packet
		cout << "ICMP reply packet from node"<<endl;
		memcpy (frame.data, gateway, 6);
		memcpy (frame.data + 6, iface[1].mac, 6);
		sendFrame(frame,1);
		cout << "reply packet sent to gateway"<<endl;
	}
	}else{//Packet from Net
		if(checksum((u_short*)(frame.data+14),20) == 0){
		if(frame.data[23] == (byte)0x01){//ICMP packet
			cout << "ICMP packet from Net"<<endl;
			uint32 ip = ext_IP(frame.data + 30);
			cout << "icmp dst ip is : ";
			print_ip(ip);
//			byte mac[6];
//			memcpy ((void *) mac, frame.data, 6);
//			uint32 dst = 0;
			map<uint32, uint32>::iterator itr = ipmp.find(ip);
			uint32 dst = (*itr).second;
//			dst = ipmp[ip];
			cout << "ip map size is : "<< ipmp.size()<<endl;
			uint64 dst2 = mp[ip];
			if(itr != ipmp.end()){ //I should Tunnel
				cout << "Tunneling ... "<<endl;
				Frame APframe (200 , new byte(200));
				byte protocol = (byte)(0x65);
				make_ip_packet(APframe.data,dst,iface[1].ip,iface[1].mac,gateway,protocol);
				memcpy (frame.data, &dst2, 6);
//				frame.data[10+14] = (byte)(0x00);
//				frame.data[11+14] = (byte)(0x00);
				u_short r = checksum((u_short*)(frame.data),frame.length);
				memcpy (APframe.data+34, &r, 2);
				memcpy (APframe.data+36, frame.data, 98);
				cout << ((sendFrame(APframe,1) == 0)? "Fail" : "OK  ");
				cout <<"cksum is: " << checksum((u_short*)(APframe.data+14),20) <<endl;
				cout << "AP message sent for AP"<<endl;
			}else if(dst2 != NULL){ // I should normally send this
				cout << "packet has sent for this node: "<<endl;
				byte mm[6];
				memcpy (mm, &dst2, 6);
				print_mac(mm);
				memcpy (frame.data, &dst2, 6); // set destination MAC address
				memcpy (frame.data + 6, iface[0].mac, 6);
				sendFrame(frame,0);
			}

		}else if(frame.data[9+14] == (byte)0x60){ //Message from foreign AP
			cout << "Message from foreign AP" <<endl;
//			if(checksum((u_short*)(frame.data+34),6) == 0){
			uint32 ip,foreign_ip;
			memcpy (&ip,frame.data+34, 4);
			memcpy (&foreign_ip,frame.data+26, 4);
			print_ip(ip);
			foreign_ip = htonl(foreign_ip);
			print_ip(foreign_ip);
			ipmp.insert(pair<uint32 , uint32> (ip, foreign_ip));
			cout <<"ip map size : " << ipmp.size() << endl;
//			}else{
//				cout <<"checksum error. packet dropped."<<endl;
//			}

		}else if(frame.data[9+14] == (byte)0x65){ //ICMP Tunnel packet from home AP
			cout << "ICMP Tunnel packet from home AP"<<endl;
//			frame.data[34+14+10] = (byte)(0x00);
//			frame.data[34+14+11] = (byte)(0x00);
//			u_short r = checksum((u_short*)(frame.data+14),20);
//			memcpy (frame.data+34+14+10, &r, 2);
			if(checksum((u_short*)(frame.data+34),100) == 0){
			Frame icmp (160, new byte(160));
			memcpy (icmp.data, frame.data+36, 160);
			memcpy (frame.data+6, iface[0].mac, 6);
			cout << ((sendFrame(icmp,0) == 0)? "Fail" : "OK  ");
			}else{
				cout <<"ICMP checksum error. packet dropped."<<endl;
			}
		}
		}else{
			cout <<"IP checksum error. packet dropped."<<endl;
		}
	}
}

uint64 ext_mac(byte * d){
	uint64 mac = 0;
	for(unsigned int i=0;i<6;i++){
		mac += (byte)d[i];
		if (i < 5)
			mac = mac << 8;
	}
return mac;
}
uint32 ext_IP(byte * d){
	uint32 ip = 0;
	for(unsigned int i=0;i<4;i++){
		ip += (byte)d[i];
		if (i < 3)
			ip = ip << 8;
	}
return ip;
}
byte * ip2array(uint32 ip){
	byte p[4];
	for(unsigned int i=0;i<4;i++){
		p[3-i] = ip % 256;
		ip = ip >> 8;
	}
	return p;
}
byte * mac2array(uint64 mac){
	byte p[6];
	for(unsigned int i=0;i<6;i++){
		p[5-i] = mac % 256;
		mac = mac >> 8;
	}
	return p;
}
void print_mac(byte* mac){
	char str[3 * 6];
	for (int j = 0; j < 6; ++j) {
		int b1 = (mac[j] >> 4) & 0x0F;
		int b2 = (mac[j]) & 0x0F;
		str[3 * j] = TO_HEX (b1);
		str[3 * j + 1] = TO_HEX (b2);
		str[3 * j + 2] = ':';
	}
	str[3 * 5 + 2] = '\0';
	PRINT ("-- MAC address: " << str);
}
void print_ip(uint32 ip){
	cout <<"   ";
	int nums[4];
	for (unsigned int i = 0; i < 4; i++) {
		nums[i] = (byte) (ip % 256);
		ip = ip >> 8;
	}
	for (int i = 3; i >= 0; i--)
		cout << nums[i] << ((i != 0) ? "." : "");
	cout << endl;
}
uint32 getIP(void){
	uint32 ip = 0;
	for(unsigned int i=0;i<50;i++){
		if(ips[i] != NULL){
			ip = ips[i];
			for (unsigned int j=i;j<50;j++){
				if(ips[j] == NULL){
					uint32 t = ips[j-1];
					ips[j-1]=ips[i];
					ips[i] = t;
				}
			}
			return ip;
		}
	}
	return NULL;
}
void remove_ip(uint32 ip){
	for(unsigned int i=0;i<50;i++){
		if(ips[i] == ip){
			ips[i] = NULL;
			return;
		}
	}
}

/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run () {
	// TODO: Not implemented yet...
	while(1){
		sleep(1);
		map<uint64, uint64>::iterator iter;
		for (iter = timemp.begin() ; iter != timemp.end(); iter++){
			uint64 tim = (*iter).second;
			if((timer - tim) > 6){
			uint64 mac = (*iter).first;
			cout <<"the node with this MAC exited : ";
			print_mac(mac2array(mac));
			timemp.erase(iter);
			}
		}
		timer++;
	}
}

u_short checksum(u_short *buffer,int size)
 {
     unsigned long check=0;
     while(size>1){
	 check+=*buffer++;
	 size -=sizeof(u_short);
    }
     if(size){
        check += *(u_short*)buffer;
     }
     check = (check >>16) + (check & 0xffff);
     check += (check >>16);
     return (u_short)(~check);
 }

void make_ip_packet(byte* data, uint32 h_ip, uint32 src_ip, byte* src_mac, byte* gate, byte protocol){
	memcpy (data, gate, 6); // set destination MAC address
	print_mac(gate);
	memcpy (data + 6, src_mac, 6); // set source MAC address
	uint16 *type = reinterpret_cast<uint16 *> (&data[2*6]);
	*type = htons(0x0800);
	unsigned int i = 14;
	data[0+i] = (byte)(0x45);
	data[1+i] = (byte)(0x00);
	//total length
	data[2+i] = (byte)(0x00);
	//data[3+i] = (byte)(0x56);
	data[3+i] = (byte)(0xBA);

	data[4+i] = (byte)(0x00);
	data[5+i] = (byte)(0x00);
	data[6+i] = (byte)(0x00);
	data[7+i] = (byte)(0x00);
	data[8+i] = (byte)(0xF0);
	data[9+i] = protocol;
	//cksum
	data[10+i] = (byte)(0x00);
	data[11+i] = (byte)(0x00);
	uint32 t = htonl(src_ip);
	memcpy (data + i + 12, &t, 4);
	print_ip(src_ip);
	uint32 t1 = htonl(h_ip);
	memcpy (data + i + 16, &t1, 4);
	print_ip(h_ip);
	u_short cksum = checksum((u_short*)(data+14),20);
	memcpy (data + i + 10, &cksum, 2);
}
