#ifndef AIRPORTNODE
#define AIRPORTNODE

#include<limits>
#include<string>

using namespace std;

struct airportnode
{
    int id;
    string name;
    string city;
    string country;
    string iata;
    string icao;
    double lat;
    double lon;
    double distance;
    int prev;
    string airline;

    airportnode(int ID, string n, string thecity, string thecountry,string IATA, string ICAO, double latitude, double longitude):\
        id(ID),name(n),city(thecity),country(thecountry),iata(IATA),icao(ICAO),lat(latitude),lon(longitude),distance(INT_MAX)
    {

    }


    airportnode(int ID):id(ID)
    {

    }

    friend
    bool operator<(const airportnode &a1, const airportnode &a2 )
    {
        return a1.id < a2.id;
    }

    friend
    bool operator==(const airportnode &a1, const airportnode &a2)
    {
        return a1.id == a2.id;
    }


    bool operator()(const airportnode& a1, const airportnode& a2)
    {
        return a1.id > a2.id;
    }


};

#endif // AIRPORTNODE

