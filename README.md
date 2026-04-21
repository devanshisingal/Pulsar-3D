# 🌌 Pulsar 3D — OpenGL Simulation

A real-time 3D visualization of a pulsar built using **modern OpenGL (3.3 Core)** with an interactive UI powered by **Dear ImGui**.

This project simulates key astrophysical features like **radiation jets**, **magnetic field lines**, and a **rotating neutron star**, all inside a skybox environment.

---

## ✨ Features

* 🌍 Textured pulsar sphere
* 🔁 Real-time rotation with adjustable speed
* 📡 Radiation jets (particle system)
* 🧲 Magnetic field line visualization
* 🌌 Skybox environment (cubemap)
* 🎛️ Interactive UI controls (ImGui)
* 🎥 Orbit camera with zoom + drag
* 📐 Optional grid and axis visualization

---

## 🛠️ Tech Stack

* **OpenGL 3.3 Core**
* **GLFW** — Window & Input
* **GLAD** — OpenGL Loader
* **GLM** — Mathematics
* **Dear ImGui** — UI
* **stb_image** — Texture Loading

---

## 📸 Preview


<img width="1291" height="633" alt="image" src="https://github.com/user-attachments/assets/d020674f-0de8-4870-abeb-37014ac24ff4" />

---

## ⚙️ Build Instructions

### 🔹 Linux / WSL

```bash
g++ src/pulsarr.cpp src/glad.c \
    imgui/*.cpp imgui/backends/imgui_impl_glfw.cpp \
    imgui/backends/imgui_impl_opengl3.cpp \
    -Iinclude -Iimgui -Iimgui/backends -Iglm \
    -lglfw -lGL -ldl \
    -o pulsar -std=c++17
```

### ▶️ Run

```bash
./pulsarr
```

---


## 📸 Preview
https://drive.google.com/file/d/1BrTnft1wZtWEl_noKw_NFsxj5k4XlCzQ/view?usp=sharing

---
## 🎮 Controls

| Key / Action | Function              |
| ------------ | --------------------- |
| Space        | Play / Pause rotation |
| P            | Toggle radiation jets |
| M            | Toggle magnetic field |
| G            | Toggle grid           |
| R            | Toggle axis           |
| + / -        | Adjust rotation speed |
| ↑ / ↓        | Change inclination    |
| Mouse Drag   | Orbit camera          |
| Scroll       | Zoom in / out         |
| ESC          | Exit                  |

---

## 🚀 Future Improvements

* Lighting (Phong / PBR)
* Bloom / HDR effects
* Better particle shaders
* Sound + pulsar frequency simulation

---

---

## ⭐ If you like this project

Give it a star ⭐ and consider contributing!
