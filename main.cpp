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

struct UserRecord {
    string username;
    string password;
};

// Backend operates as a high-speed matchmaker, passing raw strings to frontend APIs

// Global variables 
vector<DriverRecord> driverDatabase;
vector<BookingRecord> bookingDatabase;
vector<UserRecord> userDatabase;

// calculate days from date 44-04-2211
int calculateTotalDays(string dateString) {
    int year = stoi(dateString.substr(0, 4));
    int month = stoi(dateString.substr(5, 2));
    int day = stoi(dateString.substr(8, 2));
    

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    int totalDays = (year * 365) + day;
    for(int i = 1; i < month; i++) {
        totalDays = totalDays + daysInMonth[i];
    }
    return totalDays; 
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

void loadInitialData() {
    driverDatabase.push_back({"Madhvan", {"DELHI", "GURGAON", "JAIPUR", "AJMER"}, "2026-04-19", 800, 1200, "SUV"});
    driverDatabase.push_back({"Neha", {"CHANDIGARH", "PANIPAT", "DELHI", "NOIDA"}, "2026-04-20", 900, 1100, "Sedan"});
    driverDatabase.push_back({"Rahul", {"NOIDA", "AGRA", "KANPUR", "LUCKNOW"}, "2026-04-21", 700, 1000, "Hatchback"});
    driverDatabase.push_back({"Speedy John", {"JAIPUR", "AJMER", "UDAIPUR", "AHMEDABAD"}, "2026-04-19", 830, 1300, "Hatchback"});
    driverDatabase.push_back({"Cruise Sarah", {"GURGAON", "DELHI", "NOIDA", "AGRA"}, "2026-04-20", 1000, 1400, "Sedan"});
    driverDatabase.push_back({"Big Mike", {"MUMBAI", "PUNE", "LONAVALA"}, "2026-04-22", 600, 1500, "SUV"});
    driverDatabase.push_back({"Turbo Tom", {"DELHI", "MATHURA", "AGRA", "GWALIOR"}, "2026-04-21", 1100, 1600, "Sedan"});
    driverDatabase.push_back({"Drift Dave", {"LUCKNOW", "KANPUR", "AGRA", "NOIDA"}, "2026-04-23", 1400, 1800, "Hatchback"});
    driverDatabase.push_back({"Rapid Rachel", {"CHANDIGARH", "AMBALA", "DELHI", "GURGAON"}, "2026-04-19", 700, 1000, "SUV"});
    driverDatabase.push_back({"Lightning Luke", {"PUNE", "LONAVALA", "MUMBAI", "SURAT"}, "2026-04-24", 900, 1200, "Hatchback"});

    // default student
    userDatabase.push_back({"student", "password123"});
}


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
            int driverStartMinutes = (currentDriver.startTime / 100) * 60 + (currentDriver.startTime % 100);

            string sharedPathString = "";
            for(int x = pickupIndex; x <= dropoffIndex; x++) {
                sharedPathString = sharedPathString + currentDriver.driverRoute[x];
                if(x < dropoffIndex) {
                    sharedPathString = sharedPathString + "->";
                }
            }

            int extraStops = currentDriver.driverRoute.size() - (dropoffIndex - pickupIndex + 1);

            if (!isFirstItem) {
                resultJson = resultJson + ",";
            }
            
            resultJson = resultJson + "{";
            resultJson = resultJson + "\"driverName\":\"" + currentDriver.driverName + "\",";
            resultJson = resultJson + "\"sharedPath\":\"" + sharedPathString + "\",";
            resultJson = resultJson + "\"carType\":\"" + currentDriver.carType + "\",";
            resultJson = resultJson + "\"driverStartMinutes\":" + to_string(driverStartMinutes) + ",";
            resultJson = resultJson + "\"requestedMinutes\":" + to_string(requestedMinutes) + ",";
            resultJson = resultJson + "\"extraStops\":" + to_string(extraStops) + ",";
            resultJson = resultJson + "\"pickupIndex\":" + to_string(pickupIndex) + ",";
            resultJson = resultJson + "\"travelDate\":\"" + currentDriver.travelDate + "\",";
            resultJson = resultJson + "\"startNode\":\"" + currentDriver.driverRoute[0] + "\"";
            resultJson = resultJson + "}";
            
            isFirstItem = false;
        }
    }
    resultJson = resultJson + "]";
    return resultJson;
}

int main() {
    loadInitialData();
    httplib::Server localServer;

    localServer.Get("/api/signup", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string user = request.get_param_value("u");
        string pass = request.get_param_value("p");
        
        for (int i = 0; i < userDatabase.size(); i++) {
            if (userDatabase[i].username == user) {
                response.set_content("Exists", "text/plain");
                return;
            }
        }
        
        userDatabase.push_back({user, pass});
        response.set_content("Success", "text/plain");
    });

    localServer.Get("/api/login", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string user = request.get_param_value("u");
        string pass = request.get_param_value("p");
        
        for (int i = 0; i < userDatabase.size(); i++) {
            if (userDatabase[i].username == user && userDatabase[i].password == pass) {
                response.set_content("Success", "text/plain");
                return;
            }
        }
        
        response.set_content("Failed", "text/plain");
    });

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
        
        bookingDatabase.push_back({inputPassengerName, inputContactNumber, inputEmailAddress, inputTravelDate, inputPassengerCount, inputDriverName});
        
        response.set_content("Success", "text/plain");
    });

    cout << "C++ Web Server is running on port 8080.\n";
    cout << "You can now open the index.html file in your browser.\n";
    localServer.listen("localhost", 8080);
    return 0;
}