//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 *
 * Place code of "Node"s in this file.
 */

#include "sm.h"

#include "interface.h"
#include "frame.h"

#include <netinet/in.h>

using namespace std;

uint32 home_ip1;
uint32 home_ip0;
byte home_mac1[6];
byte home_mac0[6];

bool is_my_mac(byte*,byte*);
bool is_broadcast(byte*);
uint32 ext_IP(byte *);
u_short checksum(u_short *,int);
void print_mac(byte*);
void print_ip(uint32);
byte * ip2array(uint32);

uint32 ext_IP(byte * d){
	uint32 ip = 0;
	for(unsigned int i=0;i<4;i++){
		ip += (byte)d[i];
		if (i < 3)
			ip = ip << 8;
	}
return ip;
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
	int nums[4];
	for (unsigned int i = 0; i < 4; i++) {
		nums[i] = (byte) (ip % 256);
		ip = ip >> 8;
	}
	for (int i = 3; i >= 0; i--)
		cout << nums[i] << ((i != 0) ? "." : "");
	cout << endl;
}
byte * ip2array(uint32 ip){
	byte p[4];
	for(unsigned int i=0;i<4;i++){
		p[3-i] = ip % 256;
		ip = ip >> 8;
	}
	return p;
}

SimulatedMachine::SimulatedMachine (const ClientFramework *cf, int count) :
	Machine (cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
}

SimulatedMachine::~SimulatedMachine () {
	// destructor...
}

void SimulatedMachine::initialize () {
	// TODO: Initialize your program here; interfaces are valid now.
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
	cout << endl<<"=========================="<<endl;
	cout << "Frame received at iface " << ifaceIndex <<
		" with length " << frame.length << endl;
//	for (unsigned int i = 0; i < frame.length; ++i) {
//		cout << (byte) frame.data[i] << " ";
//		}
	cout << endl;
	byte dst_mac[6];
	memcpy ((void *) dst_mac, frame.data, 6);
	if(is_my_mac(dst_mac,iface[0].mac) || is_broadcast(dst_mac)){ //Packet is for me
		if(frame.data[14] == (byte) 0x10){ //This is IP offer packet
			cout << "This is IP offer packet"<<endl;
			//Set information received from AP
			memcpy ((void*) home_mac0, frame.data+15, 6);
			print_mac(home_mac0);
			home_ip1 = ext_IP(frame.data + 21);
			home_ip0 = ext_IP(frame.data + 25);
			iface[0].ip = ext_IP(frame.data + 29);
			//preparing and sending ACK packet
			for (unsigned int i=0; i < 6; ++i){
				byte t = frame.data[i+6];
				frame.data[i+6] = frame.data[i];
				frame.data[i] = t;
			}
			static const char M = 0x01;
			frame.data[14] = static_cast<byte>(M);
			sendFrame (frame, 0);
			cout << "my IP is : ";
			print_ip(iface[0].ip);
		}else if(frame.data[23] == (byte)0x01){ //This is ICMP packet
			cout << "I was pingged in this agent :" <<endl;
			print_mac(frame.data+6);
			//check check sum
			u_short cksum = checksum((u_short*)(frame.data+14),20);
			cout <<"check sum is : " << cksum <<endl;
			for (unsigned int i=0; i < 6; ++i){
				byte t = frame.data[i+6];
				frame.data[i+6] = frame.data[i];
				frame.data[i] = t;
			}
			for (unsigned int i=26; i < 30; ++i){
				byte t = frame.data[i+4];
				frame.data[i+4] = frame.data[i];
				frame.data[i] = t;
			}
			frame.data[34]=0x00;
//			frame.data[36]=0x00;
//			frame.data[37]=0x00;
			sendFrame (frame, 0);
		}//for debugging (must removed finally)
		else if((frame.data[12] == (byte)0x08) && (frame.data[13] == (byte)0x00)){
			cout << "Message from foreign AP" <<endl;
			uint32 ip,foreign_ip;
			memcpy (&ip,frame.data+35, 4);
			memcpy (&foreign_ip,frame.data+26, 4);
			print_ip(ip);
			print_ip(foreign_ip);
		}
	}
}

bool is_my_mac(byte* mac,byte* imac){
	for(unsigned int i=0; i<6; i++){
		if (mac[i] != imac[i])
			return false;
	}
	return true;
}
bool is_broadcast(byte* mac){
	for(unsigned int i=0; i<6; i++){
		if (mac[i] != (byte) 0xFF)
			return false;
	}
	return true;
}

/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run () {
	cout << "00";
	// preparing frame....
	while(1){
		static const char broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		cout << "0";
		Frame hframe (40, new byte[40]);
		cout << "1";
		memcpy (hframe.data, broadcast, 6); // set destination MAC address
		cout << "2";
		memcpy (hframe.data + 6, iface[0].mac, 6); // set source MAC address
		cout << "3";
		uint16 *type = reinterpret_cast<uint16 *> (&hframe.data[2*6]);
		*type = htons (0x0500); // random type; should be changed...
		cout << "4";
		if(iface[0].ip == 0){  // Data
			static const char Z = 0x00;
			hframe.data[14] = static_cast<byte>(Z);
			cout << "5";
		}else {
			static const char F = 0xFF;
			cout << "7";
 			hframe.data[14] = static_cast<byte>(F);
			cout << "8";
			memcpy (hframe.data + 15, ip2array(home_ip1), 4);
			cout << "9";
			memcpy (hframe.data + 19, ip2array(iface[0].ip), 4);
			cout << "10";
		}
	//	for (int i = 2*6 + 2; i < 200; ++i) {
	//		frame.data[i] = static_cast<byte> (i); // fill with some data...
	//	}
		sendFrame (hframe, 0);
		cout << "Sent"<< endl;
		delete hframe.data; // free the frame...
		sleep(4);
	//	for (int i = 0; i < 60; ++i) { // sending 60 frames...
	//		sendFrame (frame, 0);
	//		cout << "Sent " << i << endl;
	//		for (int j = 0; j < 400l * 1000 * 1000; ++j) {
	//		  // wait about a second...
	//		}
	//	}
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
