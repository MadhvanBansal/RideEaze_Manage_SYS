# RideEaze Management System

RideEaze is a modern, high-performance ride-booking management system built to provide an intuitive interface for finding available rides and building an active database of potential travel solutions. By combining a robust C++ backend with a sleek, premium, dark-mode inspired web frontend, the system offers an exceptionally fast and responsive experience.

## ✨ Features

- **Search & Book Rides:** Quickly find available rides between your pickup and dropoff locations considering requested travel dates.
- **Intelligent Matching Algorithm:** Ride matching takes into account the driver's current route, calculating shared path ETA to find the most suitable ride. Matches are ranked by compatibility percent scores (factoring in penalties for extra stops or varied dates) giving you the absolute best ride optimal for your time. 
- **Dynamic Driver Management:** Actively view the driver list in an easy-to-read database view and comfortably add new drivers straight from the intuitive GUI.
- **Simple, Fast Backend API:** Custom C++ HTTP server functioning to provide robust REST-like endpoints sending back fast JSON responses payload.
- **Premium User-Interface:** Visually appealing cards with modern micro-animations, neon accenting features and custom formatting to separate your screen efficiently.

## 🚀 Tech Stack

- **Backend:** C++ (with `httplib.h` header-only library)
- **Frontend:** HTML5, CSS3 styling with Glassmorphism, Vanilla JavaScript
- **Data Architecture:** In-memory vector databases with map-structured city graphs

## 💻 Local Setup

1. **Compile Backend**
Use `g++` to compile the server code. 
```bash
g++ -o main main.cpp
```
*(Note for Linux users: if you face issues, use the `-pthread` flag)*

2. **Run the Server**
```bash
./main
```
*The backend will boot up and start listening on port 8080.*

3. **Open the Front End**
Simply double-click the `RideEaze_UI.html` file in your system, or open it in your favorite web browser to load the dashboard. Allow the scripts to run and make requests to `localhost:8080`. 

## 🗺️ Getting Started

Try adding a new driver or looking for available matching! The system already pre-loads specific dummy city pathing representing distance mapping, such as tracking travel through nodes `A -> B -> C -> D`. 

To try a test search:
1. Enter `A` for your pickup location.
2. Enter `D` for your dropoff location. 
3. Leave the dates matching the system and hit find!

---
*Built meticulously to deliver seamless functional presentation.*
