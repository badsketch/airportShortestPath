#ifndef EDGEROUTE
#define EDGEROUTE


#include <string>
using namespace std;

struct edgeroute
{
    string airline;
    int airline_ID;
    string src;
    int srcID;
    string dest;
    int destID;
    double weight;

    edgeroute(string al,int ID,string source,int sID,string destination,int dID, double theWeight): \
        airline(al),airline_ID(ID),src(source),srcID(sID),dest(destination),destID(dID),weight(theWeight)
    {

    }

    friend
    bool operator<(const edgeroute &e1, const edgeroute &e2 )
    {
        return e1.weight > e2.weight;
    }
};

#endif // EDGEROUTE

