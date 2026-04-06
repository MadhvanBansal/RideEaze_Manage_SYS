#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include "httplib.h"

using namespace std;

// Student struct using full, clear names
struct DriverRecord { 
    string driverName; 
    vector<string> driverRoute; 
    string travelDate; 
    int startTime; 
    int endTime; 
    string carType; 
};

struct BookingRecord {
    string passengerName;
    string contactNumber;
    string emailAddress;
    string travelDate;
    string passengerCount;
    string driverName;
};

// Global variables with clear names
map<string, map<string, int>> cityTravelGraph;
vector<DriverRecord> driverDatabase;
vector<BookingRecord> bookingDatabase;

// calculate days from date 44-04-2211
int calculateTotalDays(string dateString) {
    int year = stoi(dateString.substr(0, 4));
    int month = stoi(dateString.substr(5, 2));
    int day = stoi(dateString.substr(8, 2));
    
    // simple array of days in each month
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    int totalDays = (year * 365) + day;
    for(int i = 1; i < month; i++) {
        totalDays = totalDays + daysInMonth[i];
    }
    return totalDays; // ignoring leap years to keep logic simple
}

// find difference between 
int getDaysDifference(string requestedDate, string driverDate) {
    int reqDays = calculateTotalDays(requestedDate);
    int driverDays = calculateTotalDays(driverDate);
    return driverDays - reqDays;
}

// Format 1430 into ampm form
string formatTimeAMPM(int timeInMinutes) {
    int hours = timeInMinutes / 100; 
    int minutes = timeInMinutes % 100;
    string ampmIndicator = "AM";
    
    if (hours >= 12) {
        ampmIndicator = "PM";
    }
    
    hours = hours % 12; 
    if (hours == 0) {
        hours = 12;
    }
    
    string minuteString = to_string(minutes);
    if (minutes < 10) {
        minuteString = "0" + minuteString;
    }
    
    return to_string(hours) + ":" + minuteString + " " + ampmIndicator;
}

// Load starting data
void loadInitialData() {
    cityTravelGraph["A"]["B"] = 15; cityTravelGraph["A"]["F"] = 10; cityTravelGraph["A"]["C"] = 30;
    cityTravelGraph["B"]["C"] = 25; cityTravelGraph["B"]["H"] = 15; cityTravelGraph["B"]["D"] = 20;
    cityTravelGraph["C"]["D"] = 10; cityTravelGraph["C"]["Z"] = 40; cityTravelGraph["C"]["E"] = 15;
    cityTravelGraph["D"]["E"] = 20; cityTravelGraph["D"]["H"] = 10; cityTravelGraph["D"]["F"] = 25;

    driverDatabase.push_back({"Madhvan", {"A", "B", "C", "D", "E"}, "2026-03-24", 800, 1200, "SUV"});
    driverDatabase.push_back({"Neha", {"X", "Y", "B", "C", "Z"}, "2026-03-24", 900, 1100, "Sedan"});
    driverDatabase.push_back({"Rahul", {"A", "B", "C", "D"}, "2026-03-26", 700, 1000, "Hatchback"});
}

// Create a JSON string to send all drivers to the HTML page
string buildDriversJSON() {
    string finalJson = "[";
    for(int i = 0; i < driverDatabase.size(); i++) {
        if(i > 0) {
            finalJson = finalJson + ",";
        }
        
        finalJson = finalJson + "{\"driverName\":\"" + driverDatabase[i].driverName + "\", ";
        finalJson = finalJson + "\"travelDate\":\"" + driverDatabase[i].travelDate + "\", ";
        finalJson = finalJson + "\"startTime\":\"" + formatTimeAMPM(driverDatabase[i].startTime) + "\", ";
        finalJson = finalJson + "\"carType\":\"" + driverDatabase[i].carType + "\", ";
        finalJson = finalJson + "\"driverRoute\":\"";
        
        for(int j = 0; j < driverDatabase[i].driverRoute.size(); j++) {
            finalJson = finalJson + driverDatabase[i].driverRoute[j];
            if(j < driverDatabase[i].driverRoute.size() - 1) {
                finalJson = finalJson + "->";
            }
        }
        finalJson = finalJson + "\"}";
    }
    finalJson = finalJson + "]";
    return finalJson;
}

// Create a JSON string to send all bookings to the HTML page
string buildBookingsJSON() {
    string finalJson = "[";
    for(int i = 0; i < bookingDatabase.size(); i++) {
        if(i > 0) {
            finalJson = finalJson + ",";
        }
        
        finalJson = finalJson + "{\"passengerName\":\"" + bookingDatabase[i].passengerName + "\", ";
        finalJson = finalJson + "\"contactNumber\":\"" + bookingDatabase[i].contactNumber + "\", ";
        finalJson = finalJson + "\"emailAddress\":\"" + bookingDatabase[i].emailAddress + "\", ";
        finalJson = finalJson + "\"travelDate\":\"" + bookingDatabase[i].travelDate + "\", ";
        finalJson = finalJson + "\"passengerCount\":\"" + bookingDatabase[i].passengerCount + "\", ";
        finalJson = finalJson + "\"driverName\":\"" + bookingDatabase[i].driverName + "\"}";
    }
    finalJson = finalJson + "]";
    return finalJson;
}

// algorithm
string executeSearchAlgorithm(string pickupNode, string dropoffNode, string requestedDate, int requestedTimeFormat) {
    int requestedMinutes = (requestedTimeFormat / 100) * 60 + (requestedTimeFormat % 100);
    string resultJson = "[";
    bool isFirstItem = true;

    for (int i = 0; i < driverDatabase.size(); i++) {
        DriverRecord currentDriver = driverDatabase[i];
        
        int pickupIndex = -1;
        int dropoffIndex = -1;
        
        // Find if the pickup and dropoff nodes exist in this driver's route
        for (int j = 0; j < currentDriver.driverRoute.size(); j++) {
            if (currentDriver.driverRoute[j] == pickupNode) {
                pickupIndex = j;
            }
            if (currentDriver.driverRoute[j] == dropoffNode) {
                dropoffIndex = j;
            }
        }

        // If both nodes are found and pickup comes before dropoff
        if (pickupIndex != -1 && dropoffIndex != -1 && pickupIndex < dropoffIndex) {
            int differenceInDays = getDaysDifference(requestedDate, currentDriver.travelDate);

            //  1 day early and 2 days late
            if (differenceInDays >= -1 && differenceInDays <= 2) {
                
                //  Calculate time to reach pickup location
                int travelToPickupMinutes = 0;
                for (int k = 0; k < pickupIndex; k++) {
                    string currentNode = currentDriver.driverRoute[k];
                    string nextNode = currentDriver.driverRoute[k+1];
                    if (cityTravelGraph[currentNode][nextNode]) {
                        travelToPickupMinutes = travelToPickupMinutes + cityTravelGraph[currentNode][nextNode];
                    } else {
                        travelToPickupMinutes = travelToPickupMinutes + 20; // default backup weight
                    }
                }
                
                int driverStartMinutes = (currentDriver.startTime / 100) * 60 + (currentDriver.startTime % 100);
                int calculatedEtaMinutes = driverStartMinutes + travelToPickupMinutes;
                int timeDifference = abs(calculatedEtaMinutes - requestedMinutes);

                // 2. Calculate shared travel time to destination
                int sharedTravelMinutes = 0;
                for (int j = pickupIndex; j < dropoffIndex; j++) {
                    string currentNode = currentDriver.driverRoute[j];
                    string nextNode = currentDriver.driverRoute[j+1];
                    if (cityTravelGraph[currentNode][nextNode]) {
                        sharedTravelMinutes = sharedTravelMinutes + cityTravelGraph[currentNode][nextNode];
                    } else {
                        sharedTravelMinutes = sharedTravelMinutes + 20;
                    }
                }

                // pricing choice
                int pricePerMinute = 5; 
                if (currentDriver.carType == "Hatchback") {
                    pricePerMinute = 4;
                } else if (currentDriver.carType == "SUV") {
                    pricePerMinute = 7;
                }
                int calculatedPrice = 50 + (sharedTravelMinutes * pricePerMinute); 
                
                // Exact time reach the dropoff node
                int dropoffEtaMinutes = calculatedEtaMinutes + sharedTravelMinutes;

                // Penalty scoring engine
                int extraStops = currentDriver.driverRoute.size() - (dropoffIndex - pickupIndex + 1);
                int compatibilityScore = 100 - (extraStops * 5) - (timeDifference / 5) - (abs(differenceInDays) * 25);
                
                if (compatibilityScore < 0) {
                    compatibilityScore = 0; // prevent negative scores
                }

                string sharedPathString = "";
                for(int x = pickupIndex; x <= dropoffIndex; x++) {
                    sharedPathString = sharedPathString + currentDriver.driverRoute[x];
                    if(x < dropoffIndex) {
                        sharedPathString = sharedPathString + "->";
                    }
                }

                int printEtaFormat = (calculatedEtaMinutes / 60) * 100 + (calculatedEtaMinutes % 60);
                int printDropoffFormat = (dropoffEtaMinutes / 60) * 100 + (dropoffEtaMinutes % 60);

                // Add to JSON output
                if (!isFirstItem) {
                    resultJson = resultJson + ",";
                }
                
                resultJson = resultJson + "{";
                resultJson = resultJson + "\"driverName\":\"" + currentDriver.driverName + "\",";
                resultJson = resultJson + "\"sharedPath\":\"" + sharedPathString + "\",";
                resultJson = resultJson + "\"matchScore\":" + to_string(compatibilityScore) + ",";
                resultJson = resultJson + "\"totalPrice\":" + to_string(calculatedPrice) + ",";
                resultJson = resultJson + "\"daysDifference\":" + to_string(differenceInDays) + ",";
                resultJson = resultJson + "\"carType\":\"" + currentDriver.carType + "\",";
                resultJson = resultJson + "\"pickupEta\":\"" + formatTimeAMPM(printEtaFormat) + "\",";
                resultJson = resultJson + "\"dropoffEta\":\"" + formatTimeAMPM(printDropoffFormat) + "\"";
                resultJson = resultJson + "}";
                
                isFirstItem = false;
            }
        }
    }
    resultJson = resultJson + "]";
    return resultJson;
}

int main() {
    loadInitialData();
    httplib::Server localServer;

    // Search Endpoint
    localServer.Get("/api/search", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*"); 
        
        string pickupInput = request.get_param_value("pickupLocation");
        string dropoffInput = request.get_param_value("dropoffLocation");
        string dateInput = request.get_param_value("travelDate");
        
        // read HTML time string "HH:MM" using simple substr instead of sscanf
        string timeString = request.get_param_value("preferredTime");
        int hours = stoi(timeString.substr(0, 2));
        int minutes = stoi(timeString.substr(3, 2));
        int totalTimeFormat = (hours * 100) + minutes;

        string finalResult = executeSearchAlgorithm(pickupInput, dropoffInput, dateInput, totalTimeFormat);
        response.set_content(finalResult, "application/json");
    });

    // Get Database Endpoint
    localServer.Get("/api/drivers", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(buildDriversJSON(), "application/json");
    });

    // Add Driver Endpoint
    localServer.Get("/api/add", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        
        string newName = request.get_param_value("nameInput");
        string newRoute = request.get_param_value("routeInput");
        string newDate = request.get_param_value("dateInput");
        string newCar = request.get_param_value("carInput"); 
        
        // parse HTML time string manually
        string timeString = request.get_param_value("startInput");
        int hours = stoi(timeString.substr(0, 2));
        int minutes = stoi(timeString.substr(3, 2));
        int newStartTime = (hours * 100) + minutes;
        
        // manual split of the comma separated string
        vector<string> finalRouteArray;
        string temporaryString = "";
        
        for(int i = 0; i < newRoute.length(); i++) {
            char currentChar = newRoute[i];
            if(currentChar == ',') { 
                finalRouteArray.push_back(temporaryString); 
                temporaryString = ""; 
            }
            else if(currentChar != ' ') { 
                temporaryString = temporaryString + (char)toupper(currentChar); 
            }
        }
        if(temporaryString != "") {
            finalRouteArray.push_back(temporaryString);
        }

        // Add to global vector
        driverDatabase.push_back({newName, finalRouteArray, newDate, newStartTime, 0, newCar});
        response.set_content("Success", "text/plain");
    });

    // Get Bookings Endpoint
    localServer.Get("/api/bookings", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(buildBookingsJSON(), "application/json");
    });

    // Add Booking Endpoint
    localServer.Get("/api/book", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        
        string inputPassengerName = request.get_param_value("passengerName");
        string inputContactNumber = request.get_param_value("contactNumber");
        string inputEmailAddress = request.get_param_value("emailAddress");
        string inputTravelDate = request.get_param_value("travelDate");
        string inputPassengerCount = request.get_param_value("passengerCount");
        string inputDriverName = request.get_param_value("driverName");
        
        // Add to global vector
        bookingDatabase.push_back({inputPassengerName, inputContactNumber, inputEmailAddress, inputTravelDate, inputPassengerCount, inputDriverName});
        
        response.set_content("Success", "text/plain");
    });

    cout << "C++ Web Server is running on port 8080.\n";
    cout << "You can now open the index.html file in your browser.\n";
    localServer.listen("localhost", 8080);
    return 0;
}