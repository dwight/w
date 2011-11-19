// winelist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <assert.h>

using namespace std;

/*
2.H.FL.1,
4,
6,
,
Red,
375ml,
2002,
Pinot Noir,
Faiveley,
"Gevrey-Chambertin 1er ""La Combe aux Moines""",
Burgundy,
,
$2,,,,,,,,$0
*/

enum { cat=4,format,vintage,varietal,producer,bottling,region,blank,price };
const char *fnames[] = { "cat","format","vintage","varietal","producer","bottling","region","blank","price" };

//int grouping[] = { format, cat, region, 0 } ;
int grouping[] = { cat, varietal, region, 0 } ;

vector<string> last(20);

vector<string> split(string in) {
    vector<string> v;
    const char *p = in.c_str();
    int state = 0;
    const char *s = p;
    while( *p ) {
        if( state > 0 ) {
            if( *p == '"' ) { 
                state--;
            }
        }
        else { 
            if( *p == '"' && 
                p == s /* not handling double quote escaping yet */
                ) { 
                assert( p == s );
                s++;
                state++;
            }
            else if( *p == ',' ) {
                string str(s, p);
                if( !str.empty() && str.back() == '"' )
                    str.pop_back();
                v.push_back(str);
                s = p+1;
            }
        }
        p++;
    }
    return v;
}

struct wine { 
    string raw;
    vector<string> fields;
    wine(string s) : raw(s) {
        fields = split(s);
        static int n;
        if( n++ == 0 ) {
#if 0
            int q = 0;
            vector<string>& v = fields;
            cout << "FIRST" << endl;
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << v[q++] << '\n';
            cout << "FIRST\n";
            cout << f(format) << '\n';
            cout << f(cat) << '\n';
            cout << f(region) << endl;
#endif

            // data check: 
            if( f(format) != "Format"  ||
                f(region) != "Region" ) { 
                    cout << "error header row doesn't look right.\n" << endl;
                    cout << raw << endl;
                    exit(1);
            }
        }
    }

    string f(int x) const { 
        string s = (unsigned) x < fields.size() ? fields[x] : "";
        if( s == "375" || s == "375ml" )
            return "Half bottle";
        if( s == "1500" )
            return "Magnum";
        return s;
    }
    bool weirdFormat(string s) const { 
        return s != "750" && s != "750ml" && 
            s != "Half bottles" && 
            s != "Magnum" && 
            s != "";
    }
    string fnorm(int x) const {
        string s = f(x);
        if( x == format && weirdFormat(s) ) { 
            return "";
        }
        return s;
    }
    bool operator<(const wine& rhs) const { 
        int *g = grouping;
        while( *g ) { 
            string a = fnorm(*g);
            string b = rhs.fnorm(*g);
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
            string a = fnorm(*g);
            string b = rhs.fnorm(*g);
            if( a != b ) 
                return false;
            ++g;
        }
        return true;
    }
    //void operator=(const wine& rhs) { fields = rhs.fields; }
    string str() { 
        stringstream ss;
        ss << f(vintage) << ' ' << f(producer) << ' ' << f(bottling);
        string fmt = f(format);
        if( weirdFormat(fmt) ) { 
            ss << " (" << fmt << ")";
        }
        ss <<" \t" << f(price);
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
    string s = str();
    if( !s.empty() ) {
        out << "<div class=\"lineitem\">";
        out << str();
        out << "</div>\n";
    }
    return out.str();
}

vector<wine> wines;

void outputFormatted() {
    cout << "<html><head>"
//        "<link rel=\"stylesheet\" href=\"winelist.css\" />"
        "</head><body>\n";
    for( vector<wine>::iterator i = wines.begin(); i != wines.end(); i++ ) {        
        cout << i->formatted();
    }
    cout << "</body></html>\n" << endl;
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


void debug(string s) { 
    cout << s << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
    ifstream f("winelist.csv");
    int i = 0;
    while( f ) { 
        string x;
        getline(f, x);
        if( 0 && strstr(x.c_str(), "Saluce") ){ 
            debug(x);
        }
        //        cout << x << endl;
        wine w = wine(x);
        if( i++ > 0 ) // skip header
            wines.push_back(w);
    }
    run();
    cout << "fin" << endl;
	return 0;
}
