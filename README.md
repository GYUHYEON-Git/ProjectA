# ProjectA

Developed with Unreal Engine 5

## 📌 Project Overview
- **Title:** ProjectA
- **Genre:** Action RPG / Souls-like
- **Engine:** Unreal Engine 5 (Strictly C++ Based)
- **Development Period:** 2026.02 ~ Present (Development in Progress)

---

## 🎯 Core Mechanics Implemented
- **Modular Stamina Component**
  - Designed a decoupled, reusable C++ component that manages dynamic stamina depletion and recovery. It calculates precise action costs in real-time for sprinting, dodging, and attacking.
- **Data-Driven Weapon System**
  - Implemented a flexible weapon equipment framework utilizing native C++ structures. This handles dynamic weapon equipping, stat scaling, and triggers distinct animation overlays based on the current weapon state.
- **State-Based Combo Attack System**
  - Built a responsive combo system driven by strict execution windows and player states. It enables fluid transitions between Light, Heavy, Charged, and Sprinting attacks while preventing unwanted action cancels.
- **Trace-Based Collision System**
  - Developed a precise weapon hit-detection framework using optimized line-tracing/sweeping sweeps during attack animation windows. Features real-time debug visualizers for accurate hitbox validation.
- **Camera-Interpolated Lock-On System**
  - Created a smooth target-locking mechanic that dynamically finds the optimal enemy target and interpolates the camera's rotation using strict math logic, while supporting quick target-switching capabilities.

---

## 🕹️ Controls

| Action | Keyboard |
| :--- | :--- |
| **Move** | `W`, `A`, `S`, `D` |
| **Look Around** | Mouse |
| **Roll** | `Spacebar` |
| **Sprint** | `Spacebar` (Hold) |
| **Interaction** | `F` |
| **Toggle Combat** | `R` |
| **Light Attack** | Left Mouse Click |
| **Heavy Attack** | `Left Shift` + Left Mouse Click |
| **Special Attack** | Left Mouse (Hold) |
| **Running Attack** | `Sprint` + Left Mouse Click |
| **Lock-On** | `Middle Mouse Click` (Scroll Wheel) |
| **Lock-On Left** | `Q` |
| **Lock-On Right** | `E` |