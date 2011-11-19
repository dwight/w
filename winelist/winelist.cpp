// winelist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

/*
Bin,10/2011,Acker,Status,Varietal          ,Producer,Bottling,Region,Vintage,Bottle Price,Category,Format,Distributor,Cost/Case,Cost/BTL,Cost/Gls,Est. $,Est. Glass $,List Glass $,Comments,,
BTG,6      ,30   ,      ,Cabernet Sauvignon,Chad,    "Lake County, Red Hills",California,2009,,,,Petit Pois,,,,,,,,,

1.I.18.F,1,,,Riesling,F. Haag,Brauneberger Juffer Sonnenuhr Auslese #13 GK Auction 2006 Signed by F. Haag,Mosel,2005,$850,Sweet,1500,,,,,,,,,,
2.A.1.BMF,3,,,Furmint,Tokaj Classic ,Tokaji Aszu 6 Puttonyos,Hungary,1997,$153,Sweet ,500,,,,,,,,,,
2.A.10.BM,2,2,,Riesling,Selbach-Oster,Zeltinger Sonnenur Trockenbeerenauslese GK,Mosel,1994,$234,Sweet,375,J. Gilman,255,85,21.25,233.75,58.44,,Acker does not specify GK,,
2.A.10.F,1,2,,Riesling,Lingenfelder,Freinsheimer Goldberg,Pfaltz,1989,$168,Sweet,375,,,,,,,,,,
2.A.11.BM,2,5,,Rieslaner,K. Darting,Ungsteiner Bettelhaus Trockenbeerenauslese,Pfalz,1992,$165,Sweet,375,J. Gilman,180,60,15,165,41.25,,,,
2.A.11.F,1,11,,Riesling,R. Haart,Piesporter Goldtropfchen Auslese EL GK Auction ,Mosel,2009,$140,Sweet,375,,,,,,,,,,

Bin,
10/2011,
Acker,
Status,
Varietal,
Producer,
Bottling,
Region,
Vintage,
Bottle Price,
Category,
Format,
Distributor,Cost/Case,Cost/BTL,Cost/Gls,Est. $,Est. Glass $,List Glass $,Comments,,

1.I.18.F,
1,
,
,
Riesling,
F. Haag,
Brauneberger Juffer Sonnenuhr Auslese #13 GK Auction 2006 Signed by F. Haag,
Mosel,
2005,
$850,
Sweet,1500,,,,,,,,,,
*/

enum { varietal = 4, producer, bottling, region, vintage, price, cat, format };
const char *fnames[] = { "varietal", "producer", "bottling", "region", "vintage", "price", "cat", "format" };

int grouping[] = { format, cat, region, 0 } ;

vector<string> last(20);

vector<string> split(string s) {
    vector<string> v;
    const char *p = s.c_str();
    while( 1 ) {
        const char *q = strchr(p, ',');
        if( q == 0 ) {
            // we don't bother with the last one
            break;
        }
        string quoted;
        while( *p == '"' ) {
            // unquote
            const char *end = strchr(p+1,'"');
            if( end ) { 
                quoted += string(p,end);
                p = end+1;
                if( *p != ',' ) { 
                    return v;
                }
                p++;
                q = strchr(p, ',');
            }
            else {
                break;
            }
        }
        if( quoted.empty() ) {
            string s = string(p, q);
            v.push_back(s); 
        }
        else {
            v.push_back(s);
        }
        p = q + 1; 
    }
    return v;
}

struct wine { 
    string raw;
    vector<string> fields;
    wine(string s) : raw(s) {
        fields = split(s);
    }

    string f(int x) const { 
        return (unsigned) x < fields.size() ? fields[x] : "";
    }
    bool operator<(const wine& rhs) const { 
        int *g = grouping;
        while( *g ) { 
            string a = f(*g);
            string b = rhs.f(*g);
            if( a < b ) 
                return true;
            if( a > b )
                return false;
            ++g;
        }
        return false;
    }
    bool operator==(const wine& rhs) const { 
        int *g = grouping;
        while( *g ) { 
            string a = f(*g);
            string b = rhs.f(*g);
            if( a != b ) 
                return false;
            ++g;
        }
        return true;
    }
    //void operator=(const wine& rhs) { fields = rhs.fields; }
    string str() { 
        stringstream ss;
        ss << f(vintage) << ' ' << f(producer) << ' ' << f(bottling) << ' ' << f(price);
        return ss.str();
    }
    string formatted();
};

string newcategory(string s, int x) { 
    stringstream ss;
    ss << '\n';
    ss << "<h" << x << ">" << s << "</h" << x << ">\n";
    return ss.str();
}

string wine::formatted() {
    int *g = grouping;
    bool newcat = false;
    stringstream out;
    // DEBUG:
    //    cout << raw << "<br>" << endl;
    int i = 1;
    while( *g ) { 
        int x = *g++;
        string q = f(x);
        if( f(x).empty() )
            return "";
        if( newcat || f(x) != last[x] ) { 
            last[x] = f(x);
            newcat = true;
            out << newcategory(f(x), i);
        }
        i++;
    }
    if( newcat ) out << '\n';
    out << "<div class=\"lineitem\">";
    out << str();
    out << "</div>";
    return out.str();
}

vector<wine> wines;

void outputFormatted() {
    cout << "<html><head>"
//        "<link rel=\"stylesheet\" href=\"winelist.css\" />"
        "</head><body>\n";
    for( vector<wine>::iterator i = wines.begin(); i != wines.end(); i++ ) {        
        cout << i->formatted() << endl;
    }
    cout << "</body></html>\n";
}

void output() {
    for( vector<wine>::iterator i = wines.begin(); i != wines.end(); i++ ) {        
     //   cout << "raw:" << i->raw << endl;
        string s = i->str();
        cout << s << endl;
    }
}

void run() { 
    //output();
    sort(wines.begin(), wines.end());
    outputFormatted();
}

int _tmain(int argc, _TCHAR* argv[])
{
    ifstream f("winelist.csv");

    while( f ) { 
        string x;
        getline(f, x);
        //        cout << x << endl;
        wines.push_back( wine(x) );
    }
    run();
    cout << "fin" << endl;
	return 0;
}
