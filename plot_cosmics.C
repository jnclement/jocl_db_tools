#include "TH1D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
using namespace std;

string get_run_date(string runnum) {
    if (runnum == "") return "";
    TSQLServer *db = TSQLServer::Connect("pgsql://10.20.32.23:5432/daq","","");
    if (db) printf("Server info: %s\n", db->ServerInfo());
    else return "no data";
    TSQLResult *res;

    string query = "SELECT ertimestamp FROM run WHERE runnumber = " + runnum;
    const char * sql = query.c_str();
    res = db->Query(sql);
    string date = res->Next()->GetField(0);
    
    delete res;
    delete db;
    return date;
}


void plot_cosmics() {
    
    ifstream f("cosmics.txt");
    string line;
    
    TGraph * g_cosmics_single = new TGraph();
    TGraph * g_cosmics_coin = new TGraph();
    g_cosmics_single->SetMarkerColor(kBlue);
    g_cosmics_coin->SetMarkerColor(kRed);
    while (getline(f,line)) {
        long long int data[100];
        int index = 0;
        istringstream words(line);
        string word;
        while (getline(words,word,' ')) {
            try {
                data[index++] = stoll(word);
            }
            catch(exception &err) {
                cout << "Missing data value" << endl;
                continue;
            }
        }
        if (data[1] == 2 || data[1] == 3) {
            int ntriggers = data[2];
            float runtime = data[15];
            float rate = ntriggers / runtime;
            if (data[1] == 2) g_cosmics_single->SetPoint(g_cosmics_single->GetN(),data[0],rate);
            
            if (data[1] == 3) g_cosmics_coin->SetPoint(g_cosmics_coin->GetN(),data[0],rate);
        }
    }
    
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
   
    cout << g_cosmics_single->GetXaxis()->GetNdivisions();
    
    int lowestrun = g_cosmics_single->GetXaxis()->GetXmin();
    int highestrun = g_cosmics_single->GetXaxis()->GetXmax();
    cout << lowestrun << " " << highestrun << endl;
    for (int i = 0; i < 10; i++) {
        //g_cosmics_single->GetXaxis()->ChangeLabel(i,-1,-1,-1,-1,-1,
    } 

    TCanvas * c = new TCanvas("c","",1500,500);
    TLegend * leg = new TLegend(.8,.7,.9,.9);
    leg->AddEntry(g_cosmics_single,"singles triggers");
    leg->AddEntry(g_cosmics_coin,"coincidence triggers");
    g_cosmics_single->SetMarkerStyle(20);
    g_cosmics_coin->SetMarkerStyle(20);
    g_cosmics_single->Draw("ap");
    g_cosmics_coin->Draw("p same");
    leg->Draw();
}

