#include "sm.h"
#include "interface.h"
#include "frame.h"
#include "hvnsdef.h"
#include <string.h>
#include <map>
using namespace std;
void print_mac(uint64);
void parsframe(byte*, uint64*, uint64*);
int str_compare(string, string);
char* parsinfo(string);
int make_hello(byte*, uint32);
char get_type(byte*);
void parsHello(byte*, string*, uint16*, string*, uint32*);
void print_dsg_interfaces(int);
void print_states(int);

uint64 timer = 0;
const uint16 MAX_AGE = 240;
const uint16 MAX_STAY = 230;
const uint16 MAX_WAIT_FOR_HELLO = MAX_AGE;
const uint16 HELLO_TIME = 60;

struct timer_int {
	int findex;
	uint64 time;
};

struct link {
	string lid;
	uint16 ldist;
	string lrootID;
};

enum state {
	forwarding, backup, pre_forwarding, pre_backup
};
map<uint64, timer_int> m;
string rootID;
string id;
uint16 dist;
uint32 age;
int *dsg_interfaces;
string parent = "None";
int parent_index = -1;
uint16 LastHelloIsent;
uint16 noHelloTimer;
struct link* links;
state* states;
SimulatedMachine::SimulatedMachine(const ClientFramework *cf, int count) :
	Machine(cf, count) {
}
void SimulatedMachine::initialize() {
	dist = 0;
	age = 0;
	noHelloTimer = timer;
	id = parsinfo(getCustomInformation());
	cout << "the macid is : " << id << endl;
	rootID = id;
	dsg_interfaces = new int[countOfInterfaces];
	links = new struct link[countOfInterfaces];
	for (int i=0;i<countOfInterfaces;i++)
		links[i].lrootID = "zzzzzzzzzzzzzzzzzzzz";
	states = new state[countOfInterfaces];
	for (int i = 0; i < countOfInterfaces; i++)
		states[i] = forwarding;
	print_states(countOfInterfaces);
	byte hello_frame[100];
	int size = make_hello(hello_frame, age);
	Frame fr(size, hello_frame);
	for (int i = 0; i < countOfInterfaces; i++) {
		cout << "Hello  ==> sending frame to " << i << endl;
		sendFrame(fr, i);
	}
	LastHelloIsent = timer;
}
void SimulatedMachine::processFrame(Frame frame, int ifaceIndex) {
	uint64 dst = 0;
	uint64 src = 0;
	cout << "a packet received with length : " << frame.length << endl;
	char type = get_type(frame.data);
	if (type == 'H') { //for learning distance and forming TSP tree.
		cout << "i got a hello" << endl;
		string rootID_h; // _h means data from a hello message.
		string id_h;
		uint16 dist_h = 0;
		uint32 age_h = 0;
		parsHello(frame.data, &rootID_h, &dist_h, &id_h, &age_h);
		cout << "the hello infos : " << rootID_h << " , " << dist_h << " , "
				<< id_h << " , " << age_h << endl;
		if (age_h <= MAX_AGE) {
			cout << "hello is age valid." << endl;
			links[ifaceIndex].lid = id_h;
			links[ifaceIndex].lrootID = rootID_h;
			links[ifaceIndex].ldist = dist_h;
			if ((str_compare(rootID_h, rootID) < 0) || ((dist_h < (dist - 1))
					&& (str_compare(rootID_h, rootID) == 0))) {
				cout << "updating bridge status" << endl;
				rootID = rootID_h; //update status
				dist = dist_h + 1;
				age = age_h;
				if(str_compare(parent,id_h) != 0)
					states[parent_index] = backup;
				parent = id_h;
				parent_index = ifaceIndex;
				//find designated links
				dsg_interfaces[parent_index] = 1;
//				for (int i = 0; i < countOfInterfaces; i++) {
//					if ((str_compare(links[i].lrootID, rootID) < 0)) { //never happen
//						dsg_interfaces[i] = 1;
//						states[i] = backup;
//					} else if ((str_compare(links[i].lrootID, rootID) == 0) //never happen
//							&& (links[i].ldist < dist)) {
//						dsg_interfaces[i] = 1;
//						states[i] = backup;
//					} else if ((str_compare(links[i].lrootID, rootID) == 0)
//							&& (links[i].ldist == dist) && (str_compare(
//							links[i].lid, id) < 0)) {
//						dsg_interfaces[i] = 1;
//						states[i] = backup;
//					} else if (i == parent_index) {
//						dsg_interfaces[i] = 1;
//					} else {
//						dsg_interfaces[i] = 0;
//					}
//				}
				print_states(countOfInterfaces);
				print_dsg_interfaces(countOfInterfaces);

				byte hello_frame[100];
				int size = make_hello(hello_frame, age);
				Frame fr(size, hello_frame);
				//send hello to all designated links
				for (int i = 0; i < countOfInterfaces; i++) {
					if (!dsg_interfaces[i]) {
						cout << "Hello  ==> sending frame to " << i << endl;
						sendFrame(fr, i);
					}
				}
			}else if((str_compare(rootID_h, rootID) == 0) && (dist_h == dist)){
				cout << "fight for designating a lan"<<endl;
				if(str_compare(id_h,id) < 0){
					cout << "Hey, i lose."<<endl;
					dsg_interfaces[ifaceIndex] = 1;
					states[ifaceIndex] = backup;
					print_states(countOfInterfaces);
					print_dsg_interfaces(countOfInterfaces);
				}
			}else if ((str_compare(rootID_h, rootID) > 0) || ((str_compare(
					rootID_h, rootID) == 0) && (dist_h > (dist - 1)))) {
				cout << "notify khosh khial" << endl;
				byte hello_frame[100];
				int size = make_hello(hello_frame, age);
				Frame fr(size, hello_frame);
				sendFrame(fr, ifaceIndex);
			}else {
				cout << "a hello from the current root (for refresh) " << endl;
				age = age_h;
				byte hello_frame[100];
				int size = make_hello(hello_frame, age + 1);
				Frame fr(size, hello_frame);
				for (int i = 0; i < countOfInterfaces; i++) {
					if (!dsg_interfaces[i]) {
						cout << "Hello  ==> sending frame to " << i << endl;
						sendFrame(fr, i);
					}
				}
			}

			noHelloTimer = timer;
		}
	} else if (states[ifaceIndex] != backup) { // for forwarding data and learning topology.
		cout << "there is a data from a valid link." << endl;
		parsframe(frame.data, &dst, &src);
		print_mac(dst);
		print_mac(src);
		timer_int ind;
		ind.findex = ifaceIndex;
		ind.time = timer;
		m.insert(pair<uint64, timer_int> (src, ind));
		cout << m.size() << endl;
		map<uint64, timer_int>::iterator itr = m.find(dst);
		timer_int dst_iface = (*itr).second;

		if (itr == m.end()) {
			for (int i = 0; i < countOfInterfaces; i++) {
				if (!dsg_interfaces[i] && i!=ifaceIndex) {
					cout << "broadcast ==> sending frame to " << i
							<< " :result  " << (sendFrame(frame, i) ? "OK"
							: "Fail") << endl;
				}
			}
		} else if (dst_iface.findex != ifaceIndex) {
			cout << "i know the dst ==> sending frame to dst "
					<< dst_iface.findex << " :result  " << (sendFrame(frame,
					dst_iface.findex) ? "OK" : "Fail") << endl;
		} else {
			cout << "frame was to dest of sending LAN." << endl;
		}
	}
}
void SimulatedMachine::run() {
	for (;;) {
		timer++;
		sleep(1);
		map<uint64, timer_int>::iterator itr;
		for (itr = m.begin(); itr != m.end(); itr++) {
			uint64 temp = ((*itr).second).time;
			if ((timer - temp) > MAX_STAY) {
				m.erase(itr);
				m.size();
				cout << "one item removed from forwarding map." << endl;
			}

		}
		if (dist == 0) { // bridge is current root
			if ((timer - LastHelloIsent) > HELLO_TIME) {
				cout << "i was root and HELLO_TIME expired" << endl;
				byte hello_frame[100];
				int size = make_hello(hello_frame, age);
				Frame fr(size, hello_frame);
				for (int i = 0; i < countOfInterfaces; i++) {
					cout << "Hello  ==> sending frame to " << i << endl;
					sendFrame(fr, i);
				}
				LastHelloIsent = timer;
			}
		} else if ((timer - noHelloTimer) > MAX_WAIT_FOR_HELLO) { //claim being root and send hello to all links
			cout << "i am root" << endl;
			rootID = id;
			for (int i = 0; i < countOfInterfaces; i++)
				dsg_interfaces[i] = 0;
			byte hello_frame[100];
			int size = make_hello(hello_frame,age);
			Frame fr(size, hello_frame);
			for (int i = 0; i < countOfInterfaces; i++) {
				cout << "Hello  ==> sending frame to " << i << endl;
				sendFrame(fr, i);
			}
			noHelloTimer = timer;
		}
	}

}
void print_dsg_interfaces(int size) {
	cout << "interfaces :  ";
	for (int i = 0; i < size; i++) {
		if (!dsg_interfaces[i])
			cout << i << "  ";
	}
	cout << endl;
}
void print_states(int size) {
	cout << "states :  ";
	for (int i = 0; i < size; i++) {
		cout << ((states[i] == 0) ? "forwarding" : "backup") << "   ";
	}
	cout << endl;
	cout << "parent is " << parent << " # root is " << rootID << " # dist is "
			<< dist << " # age is " << age << endl;
}
void print_mac(uint64 d) {
	int nums[6];
	for (int i = 0; i < 6; i++) {
		nums[i] = (int) (d % 256);
		d = d >> 8;
	}
	for (int i = 5; i >= 0; i--)
		cout << nums[i] << ((i != 0) ? ":" : "");
	cout << endl;
}
void parsframe(byte* d, uint64* dst, uint64* src) {
	int i;
	for (i = 0; i < 6; i++) {
		(*dst) += d[i];
		if (i != 5)
			*dst = (*dst) << 8;
	}
	for (int j = 0; j < 6; j++) {
		(*src) += d[i];
		i++;
		if (j != 5)
			(*src) = (*src) << 8;
	}
}
////
char* parsinfo(string s) {
	char *mac;
	char *id;

	mac = strtok((char*) s.c_str(), "\n");
	id = strtok(NULL, "\n");

	cout << mac << "   " << id << endl;
	int j = 0;
	unsigned int size = strlen(mac);
	for (unsigned int i = 0; i < size; i++) {
		if (s[i] != ':') {
			mac[j] = mac[i];
			j++;
		}
	}
	mac[j] = NULL;
	char* macid = strcat(id, mac);
	return macid;
}
char get_type(byte* d) {
	return (char) d[12];
}
int make_hello(byte* h, uint32 age) {
	int i;
	for (i = 0; i < 12; i++) {
		h[i] = NULL;
	}
	h[i++] = 'H';
	for (unsigned int j = 0; j < rootID.length(); j++) {
		h[i++] = rootID[j];
	}
	h[i++] = ' ';
	uint16 dist_t = dist;
	for (unsigned int j = 0; j < 2; j++) {
		h[i++] = (byte) (dist_t % 256);
		dist_t = dist_t >> 8;
	}
	h[i++] = ' ';

	for (unsigned int j = 0; j < id.length(); j++) {
		h[i++] = id[j];
	}
	h[i++] = ' ';

	uint64 age_t = age;
	cout << "age that is written to hello is  " << age_t << endl;
	for (int j = 0; j < 4; j++) {
		h[i++] = (byte) (age_t % 256);
		age_t = age_t >> 8;
	}
	return i;
}
void parsHello(byte* h, string* rootID_h, uint16* dist_h, string* id_h,
		uint32* age_h) {
	h += 13;
	string rootID_t(strtok((char*) h, " "));
	*rootID_h = rootID_t;
	h = h + 1 + rootID_t.length();

	*dist_h += (byte) (*(h + 1));
	*dist_h = *dist_h << 8;
	*dist_h += (byte) (*h);
	h += 3;

	string id_t(strtok((char*) h, " "));
	*id_h = id_t;
	h = h + 1 + id_t.length();

	for (int i = 3; i >= 0; i--) {
		*age_h += (byte) (*(h + i));
		if (i != 0)
			*age_h = *age_h << 8;
	}
}
int str_compare(string s1, string s2) {
	int size_diff = s1.size() - s2.size();
	if (size_diff == 0) {
		return s1.compare(s2);
	}
	return size_diff;
}
