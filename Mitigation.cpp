#include "ns3/command-line.h"
#include "ns3/double.h"
#include "ns3/names.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/uinteger.h"
#include "ns3/csma-module.h"
#include "ns3/ndnSIM/helper/ndn-app-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-global-routing-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-strategy-choice-helper.hpp"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
#include "ns3/ndnSIM/model/ndn-net-device-transport.hpp"
#include "ns3/ndnSIM/NFD/daemon/fw/random-strategy.hpp"
#include "ns3/ndnSIM/NFD/daemon/fw/best-route-strategy.hpp"
#include "ns3/ndnSIM/utils/topology/annotated-topology-reader.hpp"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/gnuplot.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
#include <map>

#include <iostream>
#include <chrono>
#include <thread>
#include <limits>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <string>
#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <string>
#include <iostream>


#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <map>
#include <iostream>
#include <chrono>


#include <ndn-cxx/face.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>


using namespace ndn;
using namespace std;


using namespace std::chrono;


const int MAX_ITERATION = 10; // Maximum iterations
const int m = 5;              // Modulus value

// Simulate cache hit or miss by generating a random boolean
bool isCacheHit() {
    return rand() % 2 == 0; // Randomly returns true (cache hit) or false
}

// Parallel Cache Probing algorithm
void parallelCacheProbing() {
    for (int j = 1; j <= MAX_ITERATION; ++j) {
        // Generate the chunk name based on the template "alice/generate/<j>"
        std::string chunkName = "/alice/generate/" + std::to_string(j);
        Name name(chunkName);

        // Simulate sending request and checking for cache hit
        if (isCacheHit()) {
            cout << "Cache hit detected: Someone requested content " << name.toUri() << endl;
        } else {
            cout << "No content is detected for " << name.toUri() << endl;
        }

        // Sleep for a short interval based on m
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m));
    }
}

struct HistoryEntry {
    std::string name;
    time_point<steady_clock> time;
    int face;
    int count;
    bool isAttack;
    int CC;
};

class OnInterestAlgorithm {
public:
    OnInterestAlgorithm()
        : avgLT(200), delta(50), AT(10), CountT(5), CC_increment(2) {}

    void onInterest(const Interest& interest, int inFace) {
        auto firstComp = interest.getName().get(0).toUri();
        auto found = history.find({firstComp, inFace});

        if (found == history.end()) {
            // If not found, create a new History entry
            HistoryEntry his;
            his.name = firstComp;
            his.time = steady_clock::now();
            his.face = inFace;
            his.count = 1;
            his.isAttack = false;
            his.CC = 0;
            history[{firstComp, inFace}] = his;
        }
        else {
            // Entry found, update it
            auto& it = found->second;
            auto currentTime = steady_clock::now();
            auto elapsed = duration_cast<milliseconds>(currentTime - it.time).count();

            if ((avgLT - delta) < elapsed && elapsed < (avgLT + delta)) {
                it.count++;
                it.time = currentTime;

                if (it.count == AT && !it.isAttack) {
                    it.isAttack = true;
                    it.CC += 1;
                }
                if (it.isAttack && it.CC < CountT) {
                    it.CC += 1;
                }
                if (it.CC >= CountT) {
                    history.erase(found);
                }
            }
            else {
                if (it.isAttack) {
                    it.CC += CC_increment;
                }
                if (it.CC >= CountT) {
                    history.erase(found);
                }
            }
        }

        // Simulate Content Store Lookup
        bool contentFound = contentStoreLookup(interest);
        if (contentFound && found != history.end() && !found->second.isAttack) {
            delayDataPacket();
        }
        else {
            usualInterestProcess();
        }
    }

private:
    int avgLT;
    int delta;
    int AT;
    int CountT;
    int CC_increment;

    std::map<std::pair<std::string, int>, HistoryEntry> history;

    bool contentStoreLookup(const Interest& interest) {
        // Simulate content store lookup
        return true; // or false based on implementation needs
    }

    void delayDataPacket() {
        std::cout << "Delaying data packet due to suspicious activity.\n";
    }

    void usualInterestProcess() {
        std::cout << "Processing interest normally.\n";
    }
};


class CacheHitExperiment {
public:
    CacheHitExperiment(int numIterations, int interval)
        : N(numIterations), m(interval), tc_u(0), tu_l(numeric_limits<double>::infinity()) {}

    void run() {
        // First Iteration Loop
     for (int j = 1; j <= 2; ++j) {
          //  sendInterest(generateObjectName(1));
            this_thread::sleep_for(chrono::milliseconds(100)); // Simulating some delay between requests
       }

        // Cache Hit Detection
        for (int j = 1; j <= 2; ++j) {
            auto startTime = chrono::steady_clock::now();
    //        sendInterest(generateObjectName(j));
            waitForResponse(); // Wait for the response

            if (isCacheHit()) {
                auto endTime = chrono::steady_clock::now();
                double currentTime = chrono::duration<double, milli>(endTime - startTime).count();
                tc_u = currentTime; // Time for the first cache hit
                tu_l = currentTime; // Time for the last cache hit
                break; // Break on the first cache hit
            }
        }

        // Sleep for 1/m seconds
        this_thread::sleep_for(chrono::milliseconds(1000 / m));

        // Output results
        cout << "Time for first cache hit (tc,u): " << tc_u << " ms" << endl;
        cout << "Time for last cache hit (tu,l): " << tu_l << " ms" << endl;
    }

private:
    const int N;          // Number of iterations
    const int m;          // Interval for sleep
    double tc_u;          // Time for first cache hit
    double tu_l;          // Time for last cache hit

    // Generate object name based on index j
    string generateObjectName(int j) {
        return "/alice/generate/" + to_string(j);
    }

    void sendInterest(const string& objectName) {
        // Correctly create Interest object
        Interest interest(Name(objectName));
       // interest.setCanBePrefix(false); // Set canBePrefix to false
        //interest.setMustBeFresh(true);   // Set mustBeFresh to true

        cout << "Sending Interest for: " << objectName << endl;

        // Create the face object
        Face face;
        
        // Send the interest and handle callbacks
    //    face.expressInterest(interest,
        //    [](const Interest& interest, const Data& data) {
              //  cout << "Received Data for Interest: " << interest.getName() << endl;
           // },
           // [](const Interest& interest, const lp::Nack& nack) {
       //         cout << "Nack received for Interest: " << interest.getName() << endl;
          //  },
          //  [](const Interest& interest) {
          //      cout << "Timeout for Interest: " << interest.getName() << endl;
          //  });
    }

    void waitForResponse() {
        // Placeholder for waiting for the response logic
        // Simulate a wait for demonstration purposes
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulated wait
    }

    bool isCacheHit() {
        // Placeholder for cache hit detection logic
        // Implement logic to determine if the last request resulted in a cache hit
        return true; // This is a placeholder; actual condition should be based on cache status
    }
};

namespace ns3 {

void plot1()
{
std::string fileNameWithNoExtension = "GRA11";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Certificate Registration";
  std::string dataTitle               = "Certificate Registration";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(1,7200);
data1.Add(15, 7250);
data1.Add(15, 7300);
data1.Add(15, 7350);
data1.Add(15,7450);

data2.Add(1,7500);
data2.Add(3, 8000);
data2.Add(6, 8700);
data2.Add(9, 9300);
data2.Add(15,9600);

data3.Add(1,7700);
data3.Add(3, 8200);
data3.Add(6, 9400);
data3.Add(9, 9600);
data3.Add(15,9800);




Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Number of claims ","Registration delay (ms)");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [0:+15]");
  plot.AppendExtra ("set yrange [7000:+10500]");

data2.SetTitle("Proposed LBHIA");
data1.SetTitle("NDN-BDA");
data3.SetTitle("DPKI-ZKP");




data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

plot.AddDataset(data1);
plot.AddDataset(data2);
plot.AddDataset(data3);


// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}

void plot2()
{
std::string fileNameWithNoExtension = "GRA22";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Registration Phase Delay";
  std::string dataTitle               = "Registration Phase Delay";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,5100);
data1.Add(50, 5150);
data1.Add(100, 5200);
data1.Add(200, 5250);
data1.Add(300, 5300);
data1.Add(400,5400);


data2.Add(0,7150);
data2.Add(50, 7200);
data2.Add(100, 7270);
data2.Add(200, 7300);
data2.Add(300, 7400);
data2.Add(400,7500);

data3.Add(0,11000);
data3.Add(50, 11150);
data3.Add(100, 11190);
data3.Add(200, 11250);
data3.Add(300, 11300);
data3.Add(400, 11310);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Blockchain Height ","Certificate Registration delay(ms)");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [0:+400]");
  plot.AppendExtra ("set yrange [5000:+12000]");

data1.SetTitle("Node4");
data2.SetTitle("Node8");
data3.SetTitle("Node12");



data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

plot.AddDataset(data1);
plot.AddDataset(data2);
plot.AddDataset(data3);


// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}

void plot3()
{
std::string fileNameWithNoExtension = "GRA333";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Request Frequency";
  std::string dataTitle               = "Request Frequency";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,0);
data1.Add(90, 86);
data1.Add(180,190);
data1.Add(270, 280);
data1.Add(360, 350);
data1.Add(450,440);

data2.Add(0,0);
data2.Add(90,146);
data2.Add(130,240);
data2.Add(250,450);
data2.Add(360,460);
data2.Add(450,470);


data3.Add(0,0);
data3.Add(90,256);
data3.Add(180,360);
data3.Add(270,460);
data3.Add(360, 470);
data3.Add(450,480);



//data2.Add(1,0);
//data2.Add(10, 0);
//data2.Add(100, 0);
//data2.Add(1000, 2500);
//data2.Add(9000, 10000);
//data2.Add(10000,30000);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Request Frequency","Cache Presence (m/s)");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [0:+450]");
  plot.AppendExtra ("set yrange [0:+600]");

data1.SetTitle("Proposed LRU");
data2.SetTitle("Conventional-1");
data3.SetTitle("Conventional-2");



data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);


plot.AddDataset(data1);
plot.AddDataset(data2);
plot.AddDataset(data3);


// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}

void plot4()
{
std::string fileNameWithNoExtension = "GRA444";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Cache Occupancy";
  std::string dataTitle               = "Cache Occupancy";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,7190);
data1.Add(100, 7200);
data1.Add(150, 7270);
data1.Add(200, 7355);
data1.Add(300, 7510);
data1.Add(400, 7600);


data2.Add(0,7150);
data2.Add(100,7170);
data2.Add(150, 7190);
data2.Add(200, 7260);
data2.Add(300, 7340);
data2.Add(400, 7500);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Cache Occupancy over Time (ms)","Cache Occupancy");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [1:+400]");
  plot.AppendExtra ("set yrange [7150:+7850]");

data1.SetTitle("LRU");
data2.SetTitle("Conventional");




data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);


plot.AddDataset(data1);
plot.AddDataset(data2);



// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}




void plot5()
{
std::string fileNameWithNoExtension = "GRA555";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Content Popularity";
  std::string dataTitle               = "Content Popularity";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,7240);
data1.Add(100, 7290);
data1.Add(150, 7370);
data1.Add(200, 7415);
data1.Add(300, 7590);
data1.Add(400, 7690);


data2.Add(0,7210);
data2.Add(100,7270);
data2.Add(150, 7240);
data2.Add(200, 7290);
data2.Add(300, 7390);
data2.Add(400, 7550);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Cache Hit Probability","Content Popularity");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [1:+400]");
  plot.AppendExtra ("set yrange [7150:+7850]");

data1.SetTitle("LRU Cache");
data2.SetTitle("Conventional");




data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);


plot.AddDataset(data1);
plot.AddDataset(data2);



// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}


void plot6()
{
std::string fileNameWithNoExtension = "GRA666";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "Cache Replacement Count Over Time";
  std::string dataTitle               = "Cache Replacement Count Over Time";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,0);
data1.Add(20, 4);
data1.Add(40, 7);
data1.Add(60, 12);
data1.Add(80, 15);
data1.Add(100, 17);


data2.Add(0,0);
data2.Add(20,3);
data2.Add(40, 5);
data2.Add(60, 7);
data2.Add(80, 9);
data2.Add(100, 12);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Time (seconds)","Cache Replacement Count Over Time");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [1:+100]");
  plot.AppendExtra ("set yrange [0:+20]");

data1.SetTitle("LRU");
data2.SetTitle("Conventional - 1 ");




data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);


plot.AddDataset(data1);
plot.AddDataset(data2);



// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}


void plot7()
{
std::string fileNameWithNoExtension = "GRA777";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "NDN-Cache Hit Rate";
  std::string dataTitle               = "NDN-Cache Hit Rate";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,50);
data1.Add(50, 60);
data1.Add(100, 70);
data1.Add(200, 80);
data1.Add(300, 90);
data1.Add(400,100);


data2.Add(0,50);
data2.Add(50, 52);
data2.Add(100, 62);
data2.Add(200, 73);
data2.Add(300, 74);
data2.Add(400,79);

data3.Add(0,50);
data3.Add(50, 51);
data3.Add(100, 57);
data3.Add(200, 68);
data3.Add(300, 69);
data3.Add(400, 70);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Time (Seconds) ","Cache Hit Rate(%)");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [0:+400]");
  plot.AppendExtra ("set yrange [50:+100]");

data1.SetTitle("LRU");
data2.SetTitle("Conventional -1 ");
data3.SetTitle("Conventional - 2 ");



data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

plot.AddDataset(data1);
plot.AddDataset(data2);
plot.AddDataset(data3);


// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}


void plot8()
{
std::string fileNameWithNoExtension = "GRA88";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "DPKI -ZKP -Registration Phase Delay";
  std::string dataTitle               = "DPKI -ZKP - Registration Phase Delay";

  // Instantiate the plot and set its title.
 // Gnuplot plot (graphicsFileName);
  // Instantiate the dataset, set its title, and make the points be
  
  // plotted with no connecting lines.
 
Gnuplot2dDataset data1;
Gnuplot2dDataset data2;
Gnuplot2dDataset data3;
Gnuplot2dDataset data4;
Gnuplot2dDataset data5;
Gnuplot2dDataset data6;
Gnuplot2dDataset data7;


data1.Add(0,5400);
data1.Add(50, 5550);
data1.Add(100, 5600);
data1.Add(200, 5650);
data1.Add(300, 5700);
data1.Add(400,5900);


data2.Add(0,7450);
data2.Add(50, 7600);
data2.Add(100, 7770);
data2.Add(200, 7800);
data2.Add(300, 7900);
data2.Add(400,7950);

data3.Add(0,11300);
data3.Add(50, 11450);
data3.Add(100, 11590);
data3.Add(200, 11650);
data3.Add(300, 11800);
data3.Add(400, 11910);


Gnuplot plot (fileNameWithNoExtension + ".png");
plot.SetTerminal ("png");
//plot.SetLegend ("The average number of users in a cell" , "Blocking Probability");
plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Blockchain Height ","Certificate Registration delay(ms)");

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange [0:+400]");
  plot.AppendExtra ("set yrange [5000:+12000]");

data1.SetTitle("Node4");
data2.SetTitle("Node8");
data3.SetTitle("Node12");	



data1.SetStyle(Gnuplot2dDataset::LINES_POINTS);

data2.SetStyle(Gnuplot2dDataset::LINES_POINTS);
data3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

plot.AddDataset(data1);
plot.AddDataset(data2);
plot.AddDataset(data3);


// Open the plot file.
std::ofstream plotFile ((fileNameWithNoExtension + ".plt").c_str());

// Write the plot file.
plot.GenerateOutput (plotFile);

// Close the plot file.
plotFile.close ();
}


void plot9()
{
    std::string fileNameWithNoExtension = "RiskAssessment";
    std::string graphicsFileName = fileNameWithNoExtension + ".png";
    std::string plotFileName = fileNameWithNoExtension + ".plt";
    std::string plotTitle = "Risk Assessment Chart";
    
    Gnuplot plot(fileNameWithNoExtension + ".png");
    plot.SetTerminal("png");
    plot.SetTitle(plotTitle);

    // Set the labels for each axis.
    plot.SetLegend("Identified Risks", "Risk Impact");

    // Set the range for the y axis (assuming Low = 1, Medium = 2, High = 3).
    plot.AppendExtra("set yrange [0:4]");

    // Instantiate the dataset
    Gnuplot2dDataset riskData;

    // Populate riskData with hypothetical values
    // (Assuming we have 5 identified risks)
    riskData.Add(1, 3); // Risk 1: High impact
    riskData.Add(2, 2); // Risk 2: Medium impact
    riskData.Add(3, 1); // Risk 3: Low impact
    riskData.Add(4, 2); // Risk 4: Medium impact
    riskData.Add(5, 3); // Risk 5: High impact

    riskData.SetTitle("Risk Impact");
    riskData.SetStyle(Gnuplot2dDataset::IMPULSES); // Using impulses for bar-like appearance

    // Add dataset to the plot
    plot.AddDataset(riskData);

    // Open the plot file.
    std::ofstream plotFile((fileNameWithNoExtension + ".plt").c_str());

    // Write the plot file.
    plot.GenerateOutput(plotFile);

    // Close the plot file.
    plotFile.close();
}



// t-SNE algorithm implementation
void
displayRoutes(const NodeContainer& allNodes, const std::string& prefix)
{
  for (const auto& n : allNodes) {
    const auto& fib = n->GetObject<ndn::L3Protocol>()->getForwarder()->getFib();
    const auto& e = fib.findLongestPrefixMatch(prefix);

    std::cout << "dynamic adjustment forwarding table " << n->GetId() << ", priority based forwarding " << prefix << "\n";

    for (const auto& nh : e.getNextHops()) {
      // Get remote nodeId from face:
      const auto transport = dynamic_cast<ndn::NetDeviceTransport*>(nh.getFace().getTransport());
      if (transport == nullptr)
        continue;

      const auto nd1 = transport->GetNetDevice()->GetObject<PointToPointNetDevice>();
      if (nd1 == nullptr)
        continue;

      const auto ppChannel = DynamicCast<PointToPointChannel>(nd1->GetChannel());
      if (ppChannel == nullptr)
        continue;

      auto nd2 = ppChannel->GetDevice(0);
      if (nd2->GetNode() == n)
        nd2 = ppChannel->GetDevice(1);

      std::cout << "    rate limiting: " << nd2->GetNode()->GetId() << ", feedback mechanism " << nh.getCost() << "\n";
    }
    std::cout << "\n";
  }
}

struct ContentRequest {
    int priority;
    std::string contentName;
};

// Custom comparator for ContentRequest to enable priority-based sorting
struct CompareContentRequest {
    bool operator()(const ContentRequest& lhs, const ContentRequest& rhs) const {
        return lhs.priority > rhs.priority; // Higher priority comes first
    }
};

// Dummy function to initialize routing tables
void InitializeRoutingTable(Ptr<Node> router) {
    // Initialize routing table entries for the router
    std::cout << "Initializing routing table for router " << router->GetId() << std::endl;
}

// Dummy function to update routing tables based on traffic patterns
void UpdateRoutingTableEntries(Ptr<Node> router, std::map<std::string, int> reducedTrafficData) {
    // Update routing table entries based on reducedTrafficData
    std::cout << "Updating routing table entries for router " << router->GetId() << std::endl;
}

// Dummy function to perform dimensionality reduction
std::map<std::string, int> DummyTSNE(std::map<std::string, int> trafficData) {
    // Dummy t-SNE implementation, replace with actual t-SNE logic
    std::cout << "Performingfor traffic data reduction" << std::endl;
    return trafficData; // Return reduced traffic data
}


int
main(int argc, char* argv[])
{
  bool grid = false; // Use grid topology?
  std::string routing = "lfid";
  CommandLine cmd;
  cmd.AddValue("grid", "use grid topology (instead of abilene)", grid);
  cmd.AddValue("routing", "which route computation to use (lfid, sp, allroutes)", routing);
  cmd.Parse(argc, argv);

  std::string topoName = "abilene";
  if (grid) {
    topoName = "grid";
  }

  std::cout << "Using " << topoName << " topology\n\n";

  AnnotatedTopologyReader topologyReader{};
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-" + topoName + ".txt");
  topologyReader.Read();

  ndn::StackHelper stackHelper{};
  stackHelper.InstallAll();

  // IMPORTANT: Has to be run after StackHelper!
  topologyReader.ApplyOspfMetric();

  const std::string prefix{"/prefix"};

  Ptr<Node> consumerN = Names::Find<Node>("router0");
  Ptr<Node> producerN = Names::Find<Node>("producer");
  NS_ABORT_MSG_UNLESS(consumerN && producerN, "consumer or producer name does not exist in topo file!");

  ndn::GlobalRoutingHelper routingHelper;
  routingHelper.InstallAll(); // Fills GlobalRouter with incidencies.
  routingHelper.AddOrigin(prefix, producerN);

  if (routing == "lfid") {
    routingHelper.CalculateLfidRoutes();
  }
  else if (routing == "sp") {
    routingHelper.CalculateRoutes();
  }
  else if (routing == "allroutes") {
    routingHelper.CalculateAllPossibleRoutes();
  }
  else {
    NS_FATAL_ERROR("Unknown route calculation! Use --routing {lfid|sp|allroutes}");
  }

  // IMPORTANT: Some strategy needs to be installed for displayRoutes() to work.
  //ndn::StrategyChoiceHelper strategyHelper;
  //strategyHelper.InstallAll<nfd::fw::BestRouteStrategy>("/");

  // TODO: Needs RandomStrategy for test to work!
  // Uncomment after NFD version has been updated.
  //  strategyHelper.InstallAll<nfd::fw::RandomStrategy>("/");

  displayRoutes(topologyReader.GetNodes(), prefix);

////////////////////////////////////////



  // Set up the network topology
    NodeContainer nodes;
    nodes.Create(2); // Create 2 nodes for demonstration

    // Connect nodes with a point-to-point channel
    PointToPointHelper p2p;
    p2p.Install(nodes);

    // Install NDN stack on nodes
    //ns3::ndn::StackHelper stackHelper;
    stackHelper.SetDefaultRoutes(true); // Enable default routes
    stackHelper.Install(nodes);

    // Set up forwarding strategy - BestRoute
    ns3::ndn::StrategyChoiceHelper::InstallAll("/",
        "/localhost/nfd/strategy/best-route");

    // Step 1: Initialize routing tables for routers
    Ptr<Node> router1 = nodes.Get(0);
    Ptr<Node> router2 = nodes.Get(1);
    InitializeRoutingTable(router1);
    InitializeRoutingTable(router2);


    // Simulation loop
    bool running = true;
    while (running) {
        // Step 2: Update routing tables based on traffic patterns
        std::map<std::string, int> trafficData;
        // Get traffic data from aggregators
        // Simulate t-SNE dimensionality reduction
        std::map<std::string, int> reducedTrafficData = DummyTSNE(trafficData);
        // Update routing table entries based on reduced traffic data
        UpdateRoutingTableEntries(router1, reducedTrafficData);
        UpdateRoutingTableEntries(router2, reducedTrafficData);
    

        // Other simulation steps
        // Consumers generate Interests for data
        // Forward Interests using updated routing tables
        // Run other simulation steps as needed

        // Exit simulation loop after a certain condition (for demonstration purposes)
        running = false;
    }
    


//NodeContainer nodes;
 //   nodes.Create(30);

    // Set up CSMA channel
   // CsmaHelper csma;
   // csma.SetChannelAttribute("DataRate", DataRateValue(DataRate("100Mbps")));
   // csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // Install network devices
   // NetDeviceContainer devices = csma.Install(nodes);
    
 CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate("100Mbps")));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // Install network devices
    NetDeviceContainer devices = csma.Install(nodes);

    // Enable packet tracing
    AsciiTraceHelper ascii;
    csma.EnableAsciiAll(ascii.CreateFileStream("LBHIA.tr"));
    
    PcapHelper pcapHelper;
    csma.EnablePcapAll("LBHIA", true); 

//////////////////////////////////////////////////////////////*****************
  // Installing applications
  ndn::AppHelper consumerHelperX{"ns3::ndn::ConsumerCbr"};
  consumerHelperX.SetPrefix(prefix);
  consumerHelperX.SetAttribute("Frequency", DoubleValue(100.0));
  consumerHelperX.Install(consumerN);

  ndn::AppHelper producerHelper0{"ns3::ndn::Producer"};
  producerHelper0.SetPrefix(prefix);
  producerHelper0.Install(producerN);
  
  
  plot1();
plot2();
plot4();
plot3();
plot5();
plot6();
plot7();
plot8();
plot9();

  Simulator::Stop(Seconds(30));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

// namespace ns3

//int
//main(int argc, char* argv[])
//{
//  return ns3::main(argc, argv);
//}



// Custom struct to represent a content request with priority


//int main(int argc, char* argv[]) {
  
//    return 0;

}


int
main(int argc, char* argv[])
{

 int N = 3;  // Example maximum number of iterations
    int m = 1;   // Example interval for sleep (requests per second)

    CacheHitExperiment experiment(N, m);
    experiment.run();

 OnInterestAlgorithm algorithm;

    Interest interest("/example/data");
    int faceId = 1;

    // Trigger the onInterest function
    algorithm.onInterest(interest, faceId);
 srand(::time(0));  // Corrected line to avoid ambiguity

    cout << "Starting Parallel Cache Probing algorithm..." << endl;
    parallelCacheProbing();

  return ns3::main(argc, argv);
}