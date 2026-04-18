# RideEaze Management System

RideEaze is a fast, simplified ride-booking management architecture styled specifically as a highly readable, manual student-project codebase. It demonstrates full-stack connectivity using a minimalist lightweight C++ server and an elegant Violet/Indigo accented Vanilla Javascript frontend.

## ✨ Core Features

- **Live City Mapping:** Integrates real-world OpenStreetMap (Nominatim) endpoints for live Indian city selection.
- **Dynamic Routing Engine:** Injects real-world distances and calculated travel times straight into the front-end via the high-performance OSRM routing API.
- **Built for Readability:** Explicitly avoids complex array/map automation (like Javascript `.maps` or C++ lambdas where possible) in favor of simple, visual `for` loops, so mechanics are incredibly transparent.
- **In-Memory Volatile Sandbox:** Uses pure C++ arrays instead of database connections. Any modifications or new user additions are fully wiped upon a server reset, providing a perfect sandbox testing environment for presentation days.
- **Visual Route Building:** Drivers can intuitively build visual routing arrays natively in the UI using dropdowns avoiding standard string inputs.
- **Simple, Fast Backend API:** Custom C++ HTTP server functioning to provide robust REST-like endpoints sending back fast JSON responses payload.

## 🚀 Tech Stack

- **Backend:** C++ (with `httplib.h` header-only library)
- **Frontend:** HTML5, Modern CSS Variables, Vanilla JavaScript 
- **APIs:** OpenStreetMap (Nominatim), OSRM Distance Matrix

## 💻 Local Setup

1. **Compile Backend**
Use `g++` to compile the server code. 
```bash
g++ -std=c++11 -o main main.cpp
```

2. **Run the Server**
```bash
./main
```
*The backend will boot up and load a set of default dummy drivers. It will start listening on port 8080.*

3. **Open the Front End**
Simply double-click the `RideEaze_UI.html` file in your system to load the dashboard. Allow the scripts to run and make requests to `localhost:8080`.

## 🗺️ Getting Started

When you land on the page, the system will prompt you for authentication.
- **Default Username:** `student`
- **Default Password:** `password123`

To try a test search:
1. Search for `DELHI` as the pickup location from the live dropdown.
2. Search for `JAIPUR` as the dropoff location.
3. Hit Find Rides and watch the algorithm evaluate the dummy drivers using the real-world OSRM routing!

---
*Built to deliver seamless functional presentation for student review.*
