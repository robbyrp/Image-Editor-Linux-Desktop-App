# Linux Image Editor with Zero-Copy Shared Memory IPC
--------
### A modular desktop image editor built with **C++** and **Dear ImGui** for the interface, backed by an optimized image-processing core written in pure **C**. The application uses a hybrid architecture that offloads network image retrieval and pre-processing to an asynchronous **Python FastAPI microservice**, communicating via a **zero-copy POSIX Shared Memory IPC data plane**.

--------
## Screenshots
<img width="2857" height="1707" alt="editor" src="https://github.com/user-attachments/assets/9e163e62-aca6-43bb-8cf7-3a76c48504dc" />

<img width="2874" height="1703" alt="equalize" src="https://github.com/user-attachments/assets/fb05d5cb-59c7-4ca1-b19e-c2f6c9fd77c6" />



## Architecture
*   **Modular MVC Separation:** Fully decoupled architecture. The state model, OpenGL rendering engine, and ImGui views/callbacks are strictly isolated into distinct, compilation-independent modules.
*   **Zero-Copy POSIX Shared Memory IPC:** Eliminates HTTP network and serialization overhead. The Python backend streams raw, uncompressed pixel arrays directly into a POSIX shared memory block, allowing the C++ frontend to read the image memory directly with zero copies.
*   **Concurrent AsyncIO Streaming:** The FastAPI microservice queries multiple image URLs concurrently using `asyncio.gather`. It streams only the first few kilobytes (headers) of each image, extracting resolutions in parallel to choose the optimal asset, reducing lookup times from 1.6 seconds to sub-400ms.
*   **Contiguous Memory Processing Core:** The C backend implements flat contiguous heap allocation for pixel matrices, completely avoiding heap fragmentation and improving cache locality during crop, rotate, and filter operations.


--------
## Features
* ##### Fetch images from external APIs
* ##### Crop, Rotation, Selection
* ##### Edge detect, Equalize effect, Prewitt operators
* ##### Undo button
* ##### Saving and Loading images from filesystem
--------
## Pre-requisites
### 1. System Dependencies (C++ Frontend)
Before building, make sure to install the required system libraries for windowing, native file dialogs, and low-level HTTP networking [1].

*   **On Debian/Ubuntu-based systems:**
    ```sh
    sudo apt update
    sudo apt install zenity libglfw3-dev libcurl4-openssl-dev
    ```
*   **On Fedora:**
    ```sh
    sudo dnf install zenity glfw-devel libcurl-devel
    ```

### 2. Python Dependencies (FastAPI Backend)
Navigate to the `server/` directory and install the required microservice packages [12]:
```sh
cd server
pip install -r requirements.txt
```
## Building
Start the FastAPI Backend
```sh
cd server
uvicorn main:app --reload 
```
On a separate terminal window, navigate to the `frontend/` directory and compile:
```sh
cd frontend
make
```
The app can then be run:
```sh
./editor
```
- **To then clean the project, run ```make clean```.**

## External Dependencies
* imgui - https://github.com/ocornut/imgui
* glfw3 - https://github.com/glfw/glfw
