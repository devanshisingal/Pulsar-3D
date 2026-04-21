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

![Demo](screenshots/demo.png)

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
./pulsar
```

---

## 📂 Assets

Make sure the following structure exists:

```
assets/
├── images/map.jpg
└── skybox/
    ├── right.jpg
    ├── left.jpg
    ├── top.jpg
    ├── bottom.jpg
    ├── front.jpg
    └── back.jpg
```

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

## ⚠️ Common Issues

* **Black screen?**

  * Check texture paths (`assets/` folder)
  * Ensure OpenGL 3.3 support

* **Skybox not visible?**

  * Ensure correct cubemap loading
  * Check depth settings

---

## 🚀 Future Improvements

* Lighting (Phong / PBR)
* Bloom / HDR effects
* Better particle shaders
* Sound + pulsar frequency simulation

---

## 👤 Author

**Alisha Singal**

---

## ⭐ If you like this project

Give it a star ⭐ and consider contributing!
