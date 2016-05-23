#include <QtCore/QtCore>
#include <QtXml>
#include <QDebug>
#include <iostream>
#include <map>
#include <cstdlib>
#include <cmath>
#include <limits.h>
#include <queue>
#include "airportnode.h"
#include "edgeroute.h"

#define RADIUS 3959
#define PI 3.14159



using namespace std;

struct node
{
    int id;
    string name;
    string city;
    string airline;
    string IATA;
    double distance;
    int prevnode;

    node(int ID, string n, string c,string al,string iata,double d, int p):id(ID),name(n),city(c),airline(al),IATA(iata),distance(d),prevnode(p)
    {

    }
};


struct lessThan
{
    bool operator()(const airportnode& a1, const airportnode& a2)
    {
        return a1.distance > a2.distance;
    }
};

//reads file of airports and creates keys to empty edge vectors

void initializeVertices(map<airportnode,vector<edgeroute>> &themap, map<string,int> &iataMap);

//reads file of routes and adds to adjacency list of edge vectors
void addEdges(map<airportnode,vector<edgeroute>> &themap);



//Great Circle Distance
double greatCircleDistance(double lat1, double long1, double lat2, double long2);



bool getInput(string &source, string &destination);

void getPath(map<airportnode, vector< edgeroute >> graph, int src, int dest);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    map <airportnode, vector< edgeroute > > airportGraph;
    map <string, int> iataMap;


    cout << "Loading data..." << endl;
    initializeVertices(airportGraph,iataMap);
    cout<<"Finished vert"<<endl;

    addEdges(airportGraph);

    cout << "Loaded!";

    string srce,destin;
    int source, destination;
    while(getInput(srce,destin))
    {



        transform(srce.begin(),srce.end(),srce.begin(),::toupper);
        transform(destin.begin(),destin.end(),destin.begin(),::toupper);

        if(iataMap.count(srce) != 1 || iataMap.count(destin) != 1){
            cout<<"Unrecognized Airportcode.  Please enter another."<<endl;
        }
        else
        {
            source = iataMap[srce];
            destination = iataMap[destin];
            getPath(airportGraph,source,destination);
        }

    }



    qDebug() << "Finished";

    return a.exec();
}




bool getInput(string &source, string &destination)
{
    cout<<"\n\nEnter src airport code: ";
    getline(cin, source);
    cout<<"Enter dst airport code: ";
    getline(cin,destination);
    return !source.empty() && !destination.empty();
}

void getPath(map<airportnode, vector<edgeroute> > graph, int src, int dest)
{
    //dijkstra
    //initialize heap with vertices//
    vector<airportnode> Queue;
    //priority_queue<airportnode> Queue;

    for(map<airportnode,vector<edgeroute>>::iterator it = graph.begin();it!=graph.end();++it)
    {
        Queue.push_back(airportnode(it->first.id,it->first.name,it->first.city,it->first.country,it->first.iata,it->first.icao,it->first.lat,it->first.lon));
    }


    for(unsigned int i = 0;i<Queue.size();i++)
    {
        if(Queue[i].id == src )
        {
            Queue[i].distance = 0;
        }
    }

    vector<node> visited;

    //get smallest element
    make_heap(Queue.begin(),Queue.end(),lessThan());
    airportnode shortest = Queue.front();

    //add to visited
    visited.push_back(node(shortest.id,shortest.name,shortest.city,"N/A",shortest.iata,shortest.distance,0));
    //pop from queue
    Queue.erase(Queue.begin());



    while(shortest.id != dest)
    {
        vector<edgeroute> edgeSet = graph.at(shortest.id);


        for(unsigned int i = 0;i<edgeSet.size();i++)
        {
            for(unsigned int j = 0;j<Queue.size();j++)
            {
                if(edgeSet[i].destID == Queue[j].id)
                {

                    double newDist = edgeSet[i].weight + shortest.distance;
                    if(newDist < Queue[j].distance)
                    {
                        Queue[j].airline = edgeSet[i].airline;
                        Queue[j].distance =  newDist;
                        Queue[j].prev = shortest.id;
                        break;

                    }
                }
            }
        }
        make_heap(Queue.begin(),Queue.end(),lessThan());
        shortest = Queue.front();
        visited.push_back(node(shortest.id,shortest.name,shortest.city,shortest.airline,shortest.iata,shortest.distance,shortest.prev));
        Queue.erase(Queue.begin());
    }




        vector<node> stack;
        int i = visited.size()-1;
        bool found = false;
        while(!found)
        {
            for(unsigned int j = 0;j < visited.size(); j++)
            {
                if(visited[j].id == visited[i].prevnode)
                {
                    stack.push_back(visited[i]);
                    i = j;
                }
                if(visited[0].prevnode == i)
                {
                    stack.push_back(visited[i]);
                    found = true;
                    break;
                }
            }


        }


        cout<<"Leave "<<stack[stack.size()-1].name<<"("<<stack[stack.size()-1].city<<")"<<" on "<<stack[stack.size()-2].airline<<" after flying "<<stack[stack.size()-2].distance-stack[stack.size()-1].distance<<" miles."<<endl;

        for(int i = stack.size()-2;i>=2;i--)
        {
            cout<<"Change planes at "<<stack[i].name<<"("<<stack[i].city<<")"<<" to "<<stack[i-1].airline<<" and fly "<<stack[i-2].distance - stack[i-1].distance<<" miles."<<endl;
        }
        cout<<"Arrive at "<<stack[0].name<< "." <<endl;



}

void initializeVertices(map<airportnode,vector<edgeroute>> &themap, map<string, int> &iataMap)
{
    QDomDocument document;
    QFile file("airports.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
        //return -1;
    }


    if(!document.setContent(&file))
    {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        //return -1;
    }
    //get root element
    QDomElement root = document.firstChildElement();
    QDomElement airport = root.firstChildElement();
    //QDomNodeList items = airport.childNodes();


    while(airport.hasChildNodes())
    {
        QDomNodeList items = airport.childNodes();
        int id = items.at(0).toElement().text().toInt();
        string name = items.at(1).toElement().text().toStdString();
        string city = items.at(2).toElement().text().toStdString();
        string country = items.at(3).toElement().text().toStdString();
        string iata = items.at(4).toElement().text().toStdString();
        string icao = items.at(5).toElement().text().toStdString();
        double lat = items.at(6).toElement().text().toDouble();
        double lon = items.at(7).toElement().text().toDouble();


        if(!iata.empty())
        {
            themap[airportnode(id,name,city,country,iata,icao,lat,lon)] = vector<edgeroute>();
            iataMap[iata] = id;
        }
        airport = airport.nextSiblingElement();
    }
}

void addEdges(map<airportnode,vector<edgeroute>> &themap)
{
    QDomDocument document;
    QFile file("routes.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
    }


    if(!document.setContent(&file))
    {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
    }
    QDomElement root = document.firstChildElement();
    QDomElement route = root.firstChildElement();
    QDomNodeList items = route.childNodes();



    int counter = 0;
    while(route.hasChildNodes())
    {
        QDomNodeList items = route.childNodes();
        string airline = items.at(0).toElement().text().toStdString();
        int airlineID = items.at(1).toElement().text().toInt();
        string src = items.at(2).toElement().text().toStdString();
        int srcID = items.at(3).toElement().text().toInt();
        string dest =items.at(4).toElement().text().toStdString();
        int destID = items.at(5).toElement().text().toInt();


        if(srcID)
        {
            double latitude1, longitude1,latitude2,longitude2;
            bool source = 0, destination = 0;


            //find better way
            for(map<airportnode,vector<edgeroute>>::iterator it = themap.begin();it!=themap.end();++it)
            {

                if(srcID == it->first.id)
                {
                    latitude1 = it->first.lat;
                    longitude1 =it->first.lon;
                    source = 1;

                }

                if(destID == it->first.id)
                {
                    latitude2 = it->first.lat;
                    longitude2 = it->first.lon;
                    destination = 1;
                }
                if (source && destination)
                    break;


            }
            double weight = greatCircleDistance(latitude1,longitude1,latitude2,longitude2);
            themap.at(srcID).push_back(edgeroute(airline,airlineID,src,srcID,dest,destID,weight));


        }


        route = route.nextSiblingElement();
        counter++;

    }


}

double greatCircleDistance(double lat1, double long1, double lat2, double long2)
{

    lat1 = lat1 * PI/180;
    long1 = long1 * PI/180;
    lat2 = lat2 * PI/180;
    long2 = long2 * PI/180;
    //double centralAngle = acos(sin(lat1)*sin(lat2)+cos(long1)*cos(long2)*cos(abs((long1temp-long2temp))));

    double first = pow(sin(abs(lat1-lat2)/2),2);
    double second = cos(lat1) * cos(lat2) * pow(sin(abs(long1-long2)/2),2);
    double centralAngle = 2 * asin(sqrt(first + second));

    double distance = RADIUS * centralAngle;

    return distance;
}

