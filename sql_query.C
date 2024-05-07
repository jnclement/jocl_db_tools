#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include <iostream>
using namespace std;

void sql_query() {
    TSQLServer *db = TSQLServer::Connect("pgsql://10.20.32.23:5432/daq","",""); // IP of database server
    TSQLResult *res;
    TSQLRow * row;
    
    if (db) printf("Server info: %s\n", db->ServerInfo());
    else return "no data";

    const char * query = "sql query here";
    const char * sql = query.c_str();
    res = db->Query(sql);
    int nrows = res->GetRowCount();
    int nfields = res->GetFieldCount();
    for (int ifield = 0; ifield < nfields; ifield++) {
        cout << res->GetFieldName(ifield) << endl;
    }
    for (int irow = 0; irow < nrows; irow++) {
        row = res->Next();
        for (int ifield = 0; ifield < nfields; ifield++) {
            cout << row->GetField(ifield) << " ";
        }
        cout << endl;
        delete row;
    }

    
    delete res;
    delete db;
    
    return;
}
