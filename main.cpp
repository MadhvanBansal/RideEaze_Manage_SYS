#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <fstream>
#include <sstream>
#include "httplib.h"

using namespace std;

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
    string accountUsername;
};

struct UserRecord {
    string username;
    string password;
    string accountType;
};

vector<DriverRecord> driverDatabase;
vector<BookingRecord> bookingDatabase;
vector<UserRecord> userDatabase;

struct DriverStats {
    float totalScore;
    int ratingCount;
    float averageRating;
    int ridesAccepted;
};

map<string, DriverStats> driverProfileDB;
map<string, vector<pair<string, int>>> cityGraph;

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

int getDaysDifference(string requestedDate, string driverDate) {
    int reqDays = calculateTotalDays(requestedDate);
    int driverDays = calculateTotalDays(driverDate);
    return driverDays - reqDays;
}

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

void buildCityGraph() {
    cityGraph["DELHI"].push_back({"GURGAON", 30});
    cityGraph["GURGAON"].push_back({"DELHI", 30});
    
    cityGraph["DELHI"].push_back({"NOIDA", 25});
    cityGraph["NOIDA"].push_back({"DELHI", 25});

    cityGraph["DELHI"].push_back({"PANIPAT", 85});
    cityGraph["PANIPAT"].push_back({"DELHI", 85});

    cityGraph["DELHI"].push_back({"CHANDIGARH", 240});
    cityGraph["CHANDIGARH"].push_back({"DELHI", 240});

    cityGraph["PANIPAT"].push_back({"AMBALA", 110});
    cityGraph["AMBALA"].push_back({"PANIPAT", 110});

    cityGraph["AMBALA"].push_back({"CHANDIGARH", 50});
    cityGraph["CHANDIGARH"].push_back({"AMBALA", 50});

    cityGraph["GURGAON"].push_back({"JAIPUR", 240});
    cityGraph["JAIPUR"].push_back({"GURGAON", 240});

    cityGraph["JAIPUR"].push_back({"AJMER", 135});
    cityGraph["AJMER"].push_back({"JAIPUR", 135});

    cityGraph["AJMER"].push_back({"UDAIPUR", 265});
    cityGraph["UDAIPUR"].push_back({"AJMER", 265});

    cityGraph["UDAIPUR"].push_back({"AHMEDABAD", 260});
    cityGraph["AHMEDABAD"].push_back({"UDAIPUR", 260});

    cityGraph["NOIDA"].push_back({"AGRA", 190});
    cityGraph["AGRA"].push_back({"NOIDA", 190});

    cityGraph["AGRA"].push_back({"MATHURA", 55});
    cityGraph["MATHURA"].push_back({"AGRA", 55});

    cityGraph["MATHURA"].push_back({"DELHI", 160});
    cityGraph["DELHI"].push_back({"MATHURA", 160});

    cityGraph["AGRA"].push_back({"GWALIOR", 120});
    cityGraph["GWALIOR"].push_back({"AGRA", 120});

    cityGraph["AGRA"].push_back({"KANPUR", 280});
    cityGraph["KANPUR"].push_back({"AGRA", 280});

    cityGraph["KANPUR"].push_back({"LUCKNOW", 90});
    cityGraph["LUCKNOW"].push_back({"KANPUR", 90});

    cityGraph["MUMBAI"].push_back({"PUNE", 150});
    cityGraph["PUNE"].push_back({"MUMBAI", 150});

    cityGraph["PUNE"].push_back({"LONAVALA", 65});
    cityGraph["LONAVALA"].push_back({"PUNE", 65});

    cityGraph["LONAVALA"].push_back({"MUMBAI", 85});
    cityGraph["MUMBAI"].push_back({"LONAVALA", 85});
    
    cityGraph["SURAT"].push_back({"MUMBAI", 280});
    cityGraph["MUMBAI"].push_back({"SURAT", 280});

    cityGraph["AHMEDABAD"].push_back({"SURAT", 265});
    cityGraph["SURAT"].push_back({"AHMEDABAD", 265});
}

int getGraphDistance(string startNode, string endNode) {
    if (startNode == endNode) return 0;
    if (cityGraph.find(startNode) == cityGraph.end() || cityGraph.find(endNode) == cityGraph.end()) {
        return 100; 
    }

    map<string, int> distances;
    for (auto const& pair : cityGraph) {
        distances[pair.first] = 1000000; 
    }
    
    distances[startNode] = 0;
    
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
    pq.push({0, startNode});

    while (!pq.empty()) {
        int currentDist = pq.top().first;
        string currentCity = pq.top().second;
        pq.pop();

        if (currentCity == endNode) {
            return currentDist;
        }

        if (currentDist > distances[currentCity]) {
            continue;
        }

        for (auto const& neighbor : cityGraph[currentCity]) {
            string nextCity = neighbor.first;
            int weight = neighbor.second;

            if (currentDist + weight < distances[nextCity]) {
                distances[nextCity] = currentDist + weight;
                pq.push({distances[nextCity], nextCity});
            }
        }
    }

    return 100; 
}

void loadInitialData() {
    buildCityGraph();
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

    userDatabase.push_back({"student", "password123", "DRIVER"});
    
    for (int i = 0; i < driverDatabase.size(); i++) {
        driverProfileDB[driverDatabase[i].driverName] = {40.0, 10, 4.0, 15};
    }
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


string buildBookingsJSON(string targetUser) {
    string finalJson = "[";
    bool first = true;
    for(int i = 0; i < bookingDatabase.size(); i++) {
        if(bookingDatabase[i].accountUsername != targetUser && targetUser != "admin") continue;
        
        if(!first) {
            finalJson = finalJson + ",";
        }
        first = false;
        
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

string executeSearchAlgorithm(string pickupNode, string dropoffNode, string requestedDate, int requestedTimeFormat) {
    int requestedMinutes = (requestedTimeFormat / 100) * 60 + (requestedTimeFormat % 100);
    string resultJson = "[";
    bool isFirstItem = true;

    for (int i = 0; i < driverDatabase.size(); i++) {
        DriverRecord currentDriver = driverDatabase[i];
        
        int pickupIndex = -1;
        int dropoffIndex = -1;
        
        for (int j = 0; j < currentDriver.driverRoute.size(); j++) {
            if (currentDriver.driverRoute[j] == pickupNode) {
                pickupIndex = j;
            }
            if (currentDriver.driverRoute[j] == dropoffNode) {
                dropoffIndex = j;
            }
        }

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

            int calculatedGraphDistance = getGraphDistance(pickupNode, dropoffNode);
            int daysDifference = getDaysDifference(requestedDate, currentDriver.travelDate);

            if (!isFirstItem) {
                resultJson = resultJson + ",";
            }
            
            resultJson = resultJson + "{";
            
            float avgRating = 4.0;
            if (driverProfileDB.find(currentDriver.driverName) != driverProfileDB.end()) {
                avgRating = driverProfileDB[currentDriver.driverName].averageRating;
            }
            
            resultJson = resultJson + "\"driverName\":\"" + currentDriver.driverName + "\",";
            resultJson = resultJson + "\"rating\":" + to_string(avgRating) + ",";
            resultJson = resultJson + "\"sharedPath\":\"" + sharedPathString + "\",";
            resultJson = resultJson + "\"carType\":\"" + currentDriver.carType + "\",";
            resultJson = resultJson + "\"driverStartMinutes\":" + to_string(driverStartMinutes) + ",";
            resultJson = resultJson + "\"requestedMinutes\":" + to_string(requestedMinutes) + ",";
            resultJson = resultJson + "\"extraStops\":" + to_string(extraStops) + ",";
            resultJson = resultJson + "\"sharedDistanceKm\":" + to_string(calculatedGraphDistance) + ",";
            resultJson = resultJson + "\"daysDifference\":" + to_string(daysDifference) + ",";
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

vector<string> splitString(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str);
    string tok;
    while (getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}

void saveUsers() {
    ofstream file("users.txt");
    for (int i = 0; i < userDatabase.size(); i++) {
        file << userDatabase[i].username << "," << userDatabase[i].password << "," << userDatabase[i].accountType << "\n";
    }
    file.close();
}

void loadUsers() {
    ifstream file("users.txt");
    if (!file.is_open()) return; 
    userDatabase.clear();
    string line;
    while (getline(file, line)) {
        vector<string> parts = splitString(line, ',');
        if (parts.size() >= 3) userDatabase.push_back({parts[0], parts[1], parts[2]});
    }
    file.close();
}

void saveBookings() {
    ofstream file("bookings.txt");
    for (int i = 0; i < bookingDatabase.size(); i++) {
        file << bookingDatabase[i].passengerName << "," << bookingDatabase[i].contactNumber << "," 
             << bookingDatabase[i].emailAddress << "," << bookingDatabase[i].travelDate << "," 
             << bookingDatabase[i].passengerCount << "," << bookingDatabase[i].driverName << ","
             << bookingDatabase[i].accountUsername << "\n";
    }
    file.close();
}

void loadBookings() {
    ifstream file("bookings.txt");
    if (!file.is_open()) return;
    bookingDatabase.clear();
    string line;
    while (getline(file, line)) {
        vector<string> parts = splitString(line, ',');
        if (parts.size() >= 6) {
            string accUser = (parts.size() >= 7) ? parts[6] : "unknown";
            bookingDatabase.push_back({parts[0], parts[1], parts[2], parts[3], parts[4], parts[5], accUser});
        }
    }
    file.close();
}

void saveStats() {
    ofstream file("driver_stats.txt");
    for (auto const& x : driverProfileDB) {
        file << x.first << "," << x.second.totalScore << "," << x.second.ratingCount << "," 
             << x.second.averageRating << "," << x.second.ridesAccepted << "\n";
    }
    file.close();
}

void loadStats() {
    ifstream file("driver_stats.txt");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        vector<string> parts = splitString(line, ',');
        if (parts.size() >= 5) {
            driverProfileDB[parts[0]] = {stof(parts[1]), stoi(parts[2]), stof(parts[3]), stoi(parts[4])};
        }
    }
    file.close();
}

int main() {
    loadInitialData();
    loadUsers();
    loadStats();
    loadBookings();
    httplib::Server localServer;

    localServer.Get("/api/signup", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string user = request.get_param_value("u");
        string pass = request.get_param_value("p");
        string type = request.get_param_value("t");
        if (type == "") type = "PASSENGER";
        
        for (int i = 0; i < userDatabase.size(); i++) {
            if (userDatabase[i].username == user) {
                response.set_content("Exists", "text/plain");
                return;
            }
        }
        
        userDatabase.push_back({user, pass, type});
        saveUsers();
        response.set_content("Success", "text/plain");
    });

    localServer.Get("/api/login", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string user = request.get_param_value("u");
        string pass = request.get_param_value("p");
        
        for (int i = 0; i < userDatabase.size(); i++) {
            if (userDatabase[i].username == user && userDatabase[i].password == pass) {
                response.set_content("Success," + userDatabase[i].accountType, "text/plain");
                return;
            }
        }
        
        response.set_content("Failed", "text/plain");
    });

    localServer.Get("/api/search", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*"); 
        
        string pickupInput = request.get_param_value("pickupLocation");
        string dropoffInput = request.get_param_value("dropoffLocation");
        string dateInput = request.get_param_value("travelDate");
        
        string timeString = request.get_param_value("preferredTime");
        int hours = stoi(timeString.substr(0, 2));
        int minutes = stoi(timeString.substr(3, 2));
        int totalTimeFormat = (hours * 100) + minutes;

        string finalResult = executeSearchAlgorithm(pickupInput, dropoffInput, dateInput, totalTimeFormat);
        response.set_content(finalResult, "application/json");
    });

    localServer.Get("/api/drivers", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(buildDriversJSON(), "application/json");
    });

    localServer.Get("/api/add", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        
        string newName = request.get_param_value("nameInput");
        string newRoute = request.get_param_value("routeInput");
        string newDate = request.get_param_value("dateInput");
        string newCar = request.get_param_value("carInput"); 
        
        string timeString = request.get_param_value("startInput");
        int hours = stoi(timeString.substr(0, 2));
        int minutes = stoi(timeString.substr(3, 2));
        int newStartTime = (hours * 100) + minutes;
        
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

        driverDatabase.push_back({newName, finalRouteArray, newDate, newStartTime, 0, newCar});
        response.set_content("Success", "text/plain");
    });

    localServer.Get("/api/bookings", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string user = request.get_param_value("user");
        response.set_content(buildBookingsJSON(user), "application/json");
    });

    localServer.Get("/api/book", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        
        string inputPassengerName = request.get_param_value("passengerName");
        string inputContactNumber = request.get_param_value("contactNumber");
        string inputEmailAddress = request.get_param_value("emailAddress");
        string inputTravelDate = request.get_param_value("travelDate");
        string inputPassengerCount = request.get_param_value("passengerCount");
        string inputDriverName = request.get_param_value("driverName");
        string inputAccountUsername = request.get_param_value("accountUsername");
        
        bookingDatabase.push_back({inputPassengerName, inputContactNumber, inputEmailAddress, inputTravelDate, inputPassengerCount, inputDriverName, inputAccountUsername});
        
        driverProfileDB[inputDriverName].ridesAccepted++;
        saveBookings();
        saveStats();
        
        response.set_content("Success", "text/plain");
    });

    localServer.Get("/api/rate", [](const httplib::Request& request, httplib::Response& response) {
        response.set_header("Access-Control-Allow-Origin", "*");
        string driver = request.get_param_value("driver");
        string scoreStr = request.get_param_value("score");
        
        if (driver != "" && scoreStr != "") {
            float score = stof(scoreStr);
            if (driverProfileDB.find(driver) == driverProfileDB.end()) {
                driverProfileDB[driver] = {0, 0, 0, 0};
            }
            driverProfileDB[driver].totalScore += score;
            driverProfileDB[driver].ratingCount++;
            driverProfileDB[driver].averageRating = driverProfileDB[driver].totalScore / driverProfileDB[driver].ratingCount;
            saveStats();
        }
        response.set_content("Success", "text/plain");
    });

    cout << "C++ Web Server is running on port 8080.\n";
    cout << "You can now open the index.html file in your browser.\n";
    localServer.listen("localhost", 8080);
    return 0;
}