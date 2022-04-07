#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <stdlib.h>
using namespace std;
#define exp 2.71828
#define left 0
#define right 1
#define up 2
#define down 3
static double lenda = (double)1/(double)2;
double poissan(){
	double n = lenda * 0.001;
        double p = 1 - (1 / pow(exp,n));
        return p;
}
class car {
	public:
		int id;
		int x,y;
		int dir;
		int baseStation;
		bool justIn; 

		car(){
			justIn=false;
		}
		void move(void) {
			//move car
			if(dir==left) {
				x-=10;
			} else if(dir==right) {
				x+=10;
			} else if(dir==up) {
				y+=10;
			} else if(dir==down) {
				y-=10;
			}
			/*change dir*/
			if(x%100==0&&y%100==0&&justIn==true) {
				int d = rand()%(5)+1;
				if(dir==left) {
					if(d<4) {dir=left;}
					else if(d==4) {dir=up;}
					else if(d==5) {dir=down;}
				} else if(dir==right) {
					if(d<4) {dir=right;}
					else if(d==4) {dir=up;}
					else if(d==5) {dir=down;}
				} else if(dir==up) {
					if(d<4) {dir=up;}
					else if(d==4) {dir=left;}
					else if(d==5) {dir=right;}
				} else if(dir==down) {
					if(d<4) {dir=down;}
					else if(d==4) {dir=left;}
					else if(d==5) {dir=right;}
				}
			}
		}

		bool out(void) {
			//arrive edge
			if(x%1000==0||y%1000==0) {
				if(justIn) return true;
				else justIn=true;
			}
			return false;
		}
};

class port {
	public:
		int x, y;
		int dir;
		int carEnter(void) {
			double pa = poissan();
			int arrival = 0;
			for(int i=0;i<1000;i++) {
				double p = ((double) rand() / (RAND_MAX));
				if(p<=pa) arrival++;
			}
			return arrival;
		}
};

class bs {
	public:
		int x, y;
};

double dbm(car c, bs b) {
	double distance = 0;
	distance = sqrt(pow(abs(c.x-b.x), 2) + pow(abs(c.y-b.y), 2));
	double power = 100 - 20*log10(distance) - 33;
	return power;
}

int main()
{
	int id = 0;
	int handoff = 0;
	int handoffpersecond = 0;
	int policy = 1; //policy: 0:best; 1:entropy; 2:survival; 3:mypolicy; 4:minimum

	long long int systemcarSum = 0;
	double systempowerSum = 0;
	double bestpow = 21.9094;
	double minimalpow = 18.9257;
	double powerBound = 20.42;

	double threshold = 15; //12: 19.4243; 12.5: 19.5872; 13: 19.7638; 13.5: 19.9079; 14: 20.1336; 14.5: 20.3205; 15: 20.502
	double pmin = 10;
	double entropy = 13.5; //12: 20.6162; 12.5: 20.5625; 13: 20.5221; 13.5: 20.4663; 14:20.4049

	int htbcount = 0;
	int ltbcount = 0;

	fstream file;
	file.open("best_1.txt", ios::out);

	srand( time(NULL) );
	vector<car> carVector;
	vector<int> remove;

	//port initial
	vector<port> portVector;
	port p;
	for(int i=0;i<9;i++) {
		p.x = 0;
		p.y = 100+100*i;
		p.dir = right;
		portVector.push_back(p);
	}
	for(int i=0;i<9;i++) {
		p.x = 1000;
		p.y = 100+100*i;
		p.dir = left;
		portVector.push_back(p);
	}
	for(int i=0;i<9;i++) {
		p.x = 100+100*i;
		p.y = 0;
		p.dir = up;
		portVector.push_back(p);
	}
	for(int i=0;i<9;i++) {
		p.x = 100+100*i;
		p.y = 1000;
		p.dir = down;
		portVector.push_back(p);
	}

	//base initial
	vector<bs> baseVector;
	bs bt;
	bt.x = 330;
	bt.y = 350;
	baseVector.push_back(bt);
	bt.x = 360;
	bt.y = 680;
	baseVector.push_back(bt);
	bt.x = 640;
	bt.y = 310;
	baseVector.push_back(bt);
	bt.x = 660;
	bt.y = 650;
	baseVector.push_back(bt);

	/*run*/
	int sec = 86400;
	for(int i=0;i<sec;i++) {
		handoffpersecond = handoff;
		/*new car*/
		for(int j=0;j<36;j++) {
			int arrival = portVector[j].carEnter();
			for(int k=0;k<arrival;k++) {
				car c;
				c.x = portVector[j].x;
				c.y = portVector[j].y;
				c.dir = portVector[j].dir;
				c.id = id;
				c.baseStation = -1;
				id++;
				carVector.push_back(c);
			}
			//cout<<arrival<<'\n';
		}

		//move car
		for(int j=0;j<carVector.size();j++) {
			if(carVector[j].justIn==true) carVector[j].move();
		}

		//best
		for(int j=0;j<carVector.size();j++) {
			if(carVector[j].baseStation==-1) {
				carVector[j].baseStation = 0;
				for(int k=0;k<4;k++) {
					if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
				}	
			} else {
				//policy: 1:best; 2:entropy; 3:survival; 4:mypolicy; 5:minimum
				switch(policy) {
					case 1: //best
						{
							int nowbs =  carVector[j].baseStation;
							for(int k=0;k<4;k++) {
								if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
							}
							if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
								handoff++;
							}
						}
						break;
					case 2: //entropy
						if(dbm(carVector[j], baseVector[carVector[j].baseStation])>=pmin) {
							int nowbs =  carVector[j].baseStation;
							double minusResult = 0;
							double temp = 0;
							for(int k=0;k<4;k++) {
								temp = minusResult;
								minusResult = dbm(carVector[j], baseVector[k]) - dbm(carVector[j], baseVector[nowbs]);
								if(dbm(carVector[j], baseVector[nowbs])<dbm(carVector[j], baseVector[k])&&minusResult>entropy) {
									if(minusResult>temp) carVector[j].baseStation = k;
								}
							}
							if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
								handoff++;
							}
						} else {
							int nowbs =  carVector[j].baseStation;
							for(int k=0;k<4;k++) {
								if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
							}
							if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
								handoff++;
							}
						}
						break;
					case 3: //survival
						{
							if(dbm(carVector[j], baseVector[carVector[j].baseStation])<threshold) {
								int nowbs =  carVector[j].baseStation;
								for(int k=0;k<4;k++) {
									if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
								}
								if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
									handoff++;
								}
							}
						}
						break;
					case 4: //mypolicy
						{
							//double nowpower = (double)systempowerSum/systemcarSum;
							if(dbm(carVector[j], baseVector[carVector[j].baseStation])>=pmin) {
                                                        int nowbs =  carVector[j].baseStation;
                                                        double minusResult = 0;
                                                        double temp = 0;
                                                        for(int k=0;k<4;k++) {
                                                                temp = minusResult;
                                                                minusResult = dbm(carVector[j], baseVector[k]) - dbm(carVector[j], baseVector[nowbs]);
                                                                if(dbm(carVector[j], baseVector[nowbs])<dbm(carVector[j], baseVector[k])&&minusResult>entropy) {
                                                                        if(minusResult>temp) carVector[j].baseStation = k;
                                                                }
                                                        }
                                                        if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
                                                                handoff++;
                                                        }
                                                }

							



							/*if(nowpower>powerBound) {
								htbcount++;
								ltbcount = 0;
								entropy = 13.5;
								entropy+=(double)htbcount/10.0;
								//entropy
								if(dbm(carVector[j], baseVector[carVector[j].baseStation])>=pmin) {
									int nowbs =  carVector[j].baseStation;
									double minusResult = 0;
									double temp = 0;
									for(int k=0;k<4;k++) {
										temp = minusResult;
										minusResult = dbm(carVector[j], baseVector[k]) - dbm(carVector[j], baseVector[nowbs]);
										if(dbm(carVector[j], baseVector[nowbs])<dbm(carVector[j], baseVector[k])&&minusResult>entropy) {
											if(minusResult>temp) carVector[j].baseStation = k;
										}
									}
									if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
										handoff++;
									}
								} else {
									int nowbs =  carVector[j].baseStation;
									for(int k=0;k<4;k++) {
										if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
									}
									if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
										handoff++;
									}
								}
							} else if(nowpower<powerBound) {
								ltbcount++;
								htbcount = 0;
								if(ltbcount<100) {
									entropy = 13.5;
								} else {
									entropy = 13;
								}
								if(dbm(carVector[j], baseVector[carVector[j].baseStation])>=pmin) {
									int nowbs =  carVector[j].baseStation;
									double minusResult = 0;
									double temp = 0;
									for(int k=0;k<4;k++) {
										temp = minusResult;
										minusResult = dbm(carVector[j], baseVector[k]) - dbm(carVector[j], baseVector[nowbs]);
										if(dbm(carVector[j], baseVector[nowbs])<dbm(carVector[j], baseVector[k])&&minusResult>entropy) {
											if(minusResult>temp) carVector[j].baseStation = k;
										}
									}
									if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
										handoff++;
									}
								} else {
									int nowbs =  carVector[j].baseStation;
									for(int k=0;k<4;k++) {
										if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
									}
									if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
										handoff++;
									}
								}
							}*/
						}
						break;
					case 5: //minimum
						{
							if(dbm(carVector[j], baseVector[carVector[j].baseStation])<10) {
								int nowbs =  carVector[j].baseStation;
								for(int k=0;k<4;k++) {
									if(dbm(carVector[j], baseVector[carVector[j].baseStation])<dbm(carVector[j], baseVector[k])) carVector[j].baseStation = k;
								}
								if(nowbs!=carVector[j].baseStation&&!carVector[j].out()) {
									handoff++;
								}
							}
						}
						break;	
				}
			}
		}

		/*remove car*/
		for(int j=0;j<carVector.size();j++) {
			if(carVector[j].out()) remove.push_back(carVector[j].id);
		}
		for(int j=0;j<remove.size();j++) {
			for(int k=0;k<carVector.size();k++) {
				if(remove[j]==carVector[k].id) {
					carVector.erase(carVector.begin()+k);
					break;
				}
			} 
		}
		remove.clear();
		systemcarSum+=carVector.size();
		for(int j=0;j<carVector.size();j++) {
			//cout<<"dbm: "<<dbm(carVector[j], baseVector[carVector[j].baseStation])<<'\n';
			systempowerSum+=dbm(carVector[j], baseVector[carVector[j].baseStation]);
		}
		//caculate handoffpersecond
		handoffpersecond = handoff - handoffpersecond;
		file<<handoffpersecond<<'\n';
	}

	// for(int i=0;i<carVector.size();i++) {
	// 	cout<<i<<' '<<carVector[i].id<<' '<<carVector[i].x<<' '<<carVector[i].y<<' '<<carVector[i].dir<<'\n';
	// }
	cout<<handoff<<'\n';
	cout<<systemcarSum<<'\n';
	cout<<systempowerSum<<'\n';
	cout<<(double)systempowerSum/systemcarSum<<'\n';
    return 0;
}
