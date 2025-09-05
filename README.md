# ImViewGuizmo

An **immediate-mode 3D view gizmo** for [Dear ImGui](https://github.com/ocornut/imgui), inspired by the navigation widgets found in **Blender** and **Godot**.  
It provides an interactive axis widget you can click, drag, and snap to, making it easy to control camera orientation in 3D viewports.

<p float="left">
  <img src="https://github.com/user-attachments/assets/44271b01-fe6b-4b21-ac45-6299eacb3e44" height="300" />
  <img src="https://github.com/user-attachments/assets/41afbe61-4ecf-4b87-9927-8f03db4559a6" height="300" />
</p>

## ✨ Features
- Immediate-mode API, following the same principles as Dear ImGui.  
- Axis-aligned snapping with smooth animation.  
- Camera orbit controls by dragging the gizmo.  
- Fully customizable styles (colors, sizes, labels, highlight effects).  
- Single-header implementation.  

## 📦 Installation

ImViewGuizmo is a **single-header library**. Simply add the header to your project:

```cpp
// In exactly one source file:
#define IMVIEWGUIZMO_IMPLEMENTATION
#include "ImViewGuizmo.h"

// In all other places:
#include "ImViewGuizmo.h"
```

## License

ImViewGuizmo is licensed under the MIT License, see [LICENSE](/LICENSE) for more information.
