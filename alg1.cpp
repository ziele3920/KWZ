#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>
 
using namespace std;
 
 struct Task {
 	int id;
 	int duration;
 	int minDuration;
 	int shortenCost;
 	int ES, EF, LS, LF;
 	vector<int> predecessors;
 };
 
     string nazwa;
    int liczba_zadan, liczba_polaczen, path = 0;
    char tmp;
    Task *zadania;
    int *liczba_poprzednikow;
    vector<pair<int, int> > connections;
    vector<int> kolejnosc;
    int budget, baseBudget;
    vector<int> topologyOrder;	
	int maxEF = 0; 
	vector<int>  criticalPath;
	int totalCost = 0;
	int *shortenList;
 
 void Wczytaj() {
 	         ///////////////////////////////////////////////// czytanie ///////////////////////////////
    cout <<"podaj nazwe pliku: " << endl;
    cin >> nazwa;
    fstream plik;
    plik.open( nazwa.c_str());
    plik >> liczba_zadan;
    plik >> liczba_polaczen;
    plik >> budget;
    baseBudget = budget;
    zadania = new Task[liczba_zadan];
    liczba_poprzednikow = new int[liczba_zadan];
    shortenList = new int[liczba_zadan];
     
    for(int i =0; i < liczba_zadan; ++i) {  // czytanie d³ugoœci zadañ
    	zadania[i].id = i+1;
        plik >> zadania[i].duration;
        plik >> zadania[i].minDuration;
        plik >> zadania[i].shortenCost;
        liczba_poprzednikow[i] = 0;
    }
     
    for(int i =0; i < liczba_polaczen; ++i) {
        pair<int, int> tmp;
        plik >> tmp.first;
        plik >> tmp.second;
        tmp.first--;
        tmp.second--;
        connections.push_back(tmp);
    }
 }
 
 void LiczPoprzednikow() {
 	    //////////////////////////////////////////////////////// liczenie poprzednikwo ////////////////////////////////////
    
	for(int i = 0; i < liczba_polaczen; ++i) {
		zadania[connections[i].second].predecessors.push_back(connections[i].first);
        liczba_poprzednikow[connections[i].second]++;
    }
     
    for(int i = 0; i < liczba_zadan; ++i)  {
        if(liczba_poprzednikow[i] == 0) {
            kolejnosc.push_back(i);
    	}
    }
 }
 
 void SortujTopologicznie() {
 	    ///////////////////////////////////////////////// sortowanie topologiczne ///////////////////////////
	  

	 while(kolejnosc.size()>0) {
	 	int currTask = kolejnosc[0];
	 	kolejnosc.erase(kolejnosc.begin());
	 	topologyOrder.push_back(currTask);
	 	for(int i = 0; i < connections.size(); ++i) {
	 		if(connections[i].first == currTask) {
	 			int newTask = connections[i].second;
	 			liczba_poprzednikow[newTask] -=1;
	 			if(liczba_poprzednikow[newTask] == 0)
	 			kolejnosc.push_back(newTask);
			 }
		 }
	 }
 }
 
 void LiczEfEs() {
 		////////////////////////////////////////////////////////// liczenie ES EF ///////////////
	
	for(int i = 0; i < liczba_zadan; ++i) {
		int currentTask = topologyOrder[i], newES = 0;
		for(int j = 0; j < zadania[currentTask].predecessors.size(); ++j) {
			if(newES < zadania[zadania[currentTask].predecessors[j]].EF)
				newES = zadania[zadania[currentTask].predecessors[j]].EF;
		}
		zadania[currentTask].ES = newES;
		zadania[currentTask].EF = newES + zadania[currentTask].duration;

	}
 }
 
 int LiczMaxEf() {
 		//////////////////////////////// liczenie maxEF /////////////////////////////////

	for(int i = 0; i < liczba_zadan; ++i) {
		if(zadania[i].EF > maxEF)
			maxEF = zadania[i].EF;
	}
	return maxEF;
 }
 
 void LiczLsLf() {
 		/////////////////// liczenie LS, LF /////////////
	for(int i = 0; i < liczba_zadan; ++i) {
		zadania[i].LF = maxEF;
		zadania[i].LS = zadania[i].LF - zadania[i].duration;
	}
	
    for (int k = 0; k < liczba_zadan; k++) {
        for (int j = 0; j < liczba_polaczen; ++j) {
            int z1 = connections[j].second; int z2 = connections[j].first;
            
            if (zadania[z2].LF > zadania[z1].LS) {
               zadania[z2].LF = zadania[z1].LS;
               zadania[z2].LS = zadania[z2 ].LF - zadania[z2].duration;
            }
        }
    }
	
 }
 
 void WyznaczSciezkeKrytyczna() {
 		// œcie¿ka krytyczna/////////////////////////////////////////
	//cout << endl << "critical path" << endl;
	while (path<maxEF) {
		for (int i = 0; i<liczba_zadan; i++) {
			if (zadania[i].ES == zadania[i].LS && zadania[i].LS == path) {
				criticalPath.push_back(i);
				//cout << i + 1 << " " << zadania[i].ES << " " << zadania[i].EF << endl;
				path = zadania[i].EF;
			}
		}
	}
 }
 
int main(int argc, char *argv[])
{
 	Wczytaj();
	LiczPoprzednikow();
	SortujTopologicznie();
 
 	LiczEfEs();
 	cout << endl;
 	cout << "no reduction scheduler time:  " << endl <<  LiczMaxEf() << endl;
	LiczLsLf();
	WyznaczSciezkeKrytyczna();	
	
	
		for(int i = 0; i < liczba_zadan; ++i)
			shortenList[i] = 0;
	
	for(int i = criticalPath.size()-1; i > -1 ; --i) {
		int newDuration = zadania[criticalPath[i]].minDuration;
		int oldDuration = zadania[criticalPath[i]].duration;
		int cost = zadania[criticalPath[i]].shortenCost;
		while((oldDuration -  newDuration) * cost > budget) 
			++newDuration;
		zadania[criticalPath[i]].duration = newDuration;
		budget -= (oldDuration -  newDuration) * cost;
		shortenList[criticalPath[i]] = oldDuration -  newDuration;
	}


	maxEF = 0;
	LiczEfEs();
 	cout << "reduction scheduler time:" << endl << LiczMaxEf() << endl;
 	cout << "reduction cost: " << endl << baseBudget-budget << endl;
 	cout << "reduction:" << endl;
 		for(int i = 0; i < liczba_zadan; ++i)
		cout << shortenList[i] << " ";
		
 	
}
