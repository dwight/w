// winelist.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <assert.h>

using namespace std;

enum { cat=4,format,vintage,varietal,producer,bottling,region,blank,
    price=11,
    Group1=22,
    Desc=Group1+5
};
const char *fnames[] = { "cat","format","vintage","varietal","producer","bottling","region","blank","price" };

vector<string> last(40);

vector<string> split(string in) {
    vector<string> v;
    const char *p = in.c_str();
    int state = 0;
    const char *s = p;
    while( 1 ) {
        if( *p == 0 ) { 
            v.push_back(s);
            break;
        }
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
                if( !str.empty() && str[ str.size()-1 ] == '"' ) {
				  // remove trailing quote
				  //str.pop_back();
				  str = string(str, 0, str.size()-1);
				}
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
        //cout << x << ' ' << fields.size() << endl;

        string s = (unsigned) x < fields.size() ? fields[x] : "";
        /*if( s == "375" || s == "375ml" )
            return "Half bottle";
        if( s == "1500" )
            return "Magnum";*/
        return s;
    }
    bool weirdFormat(string s) const { 
        return s != "750" && s != "750ml" && 
/*            s != "Half bottles" && 
            s != "Magnum" &&  */
            s != "";
    }
    string fnorm(int x) const {
        string s = f(x);
        /*
        if( x == format && weirdFormat(s) ) { 
            return "";
        }*/
        return s;
    }
    bool operator<(const wine& rhs) const { 
        for( int i = 0; i < 5; i++ ) {
            int column = Group1+i;
            string a = fnorm(column);
            string b = rhs.fnorm(column);
            if( a < b ) 
                return true;
            if( a > b )
                return false;
        }
        return false;
    }
    bool operator==(const wine& rhs) const { 
        for( int i = 0; i < 5; i++ ) {
            int column = Group1+i;
            string a = fnorm(column);
            string b = rhs.fnorm(column);
            if( a != b ) 
                return false;
        }
        return true;
    }
    //void operator=(const wine& rhs) { fields = rhs.fields; }
    string str() { 
        stringstream ss;
        string d = f(Desc);
        const char *desc = d.c_str();
        ss << d << "   ";
        string p = f(price);
        if( p.empty() )
            ss << "$0";
        else
            ss << p;
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
    bool newcat = false;
    stringstream out;
    // DEBUG:
    //    cout << raw << "<br>" << endl;

    static int z;
    if(0&& ++z < 10 ) {
        cout << "g1=" << f(Group1) << endl;
        cout << f(price-1) << endl;
        cout << "p:" << f(price) << endl;
        cout << f(price+1) << endl;
    }

    int level = 0;
    for( int i = 0; i < 5; i++ ) {
        int x = i+Group1;
        string q = f(x);
        if( f(x).empty() )
            continue;
        level++;
        if( newcat || f(x) != last[x] ) { 
            if( level == 1 ) { 
                out << "\n\n<!-- g1=" << f(Group1) << " g2=" << f(Group1+1) << " g3=" << f(Group1+2) << "-->\n";
            }
            last[x] = f(x);
            newcat = true;
            out << newcategory(f(x), level);
        }
    }
    if( newcat ) out << '\n';
    string s = str();
    if( !s.empty() ) {
        out << "<h6>"; //div class=\"lineitem\">";
        out << str();
        out << "</h6>\n";
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

    //    sort(wines.begin(), wines.end());
    outputFormatted();
}


void debug(string s) { 
    cout << s << endl;
}

int main(int argc, char* argv[])
{
    ifstream f("winelist.csv");
    int i = 0;
    while( f ) { 
        string x;
        getline(f, x);
        if( 0 && strstr(x.c_str(), "Saluce") ){ 
            debug(x);
        }
                //cout << x << endl;
        wine w = wine(x);
        if( i++ > 0 ) // skip header
            wines.push_back(w);
    }
    run();
    cout << "fin" << endl;
	return 0;
}
