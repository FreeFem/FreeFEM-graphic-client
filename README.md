# FreeFEM-graphic-client

How to build :

  Dowload the lastest LunarG vulkan runtime and install it on your computer.
  Find it [here](https://vulkan.lunarg.com/)

  Build the vulkan headers and install them.
  Find it [here](https://github.com/KhronosGroup/Vulkan-Headers).

Optional :

  If you want to have in depth vulkan debug capabilities, install the vulkan validation layers.
  Find it [here](https://github.com/KhronosGroup/Vulkan-ValidationLayers).

  Build using these commands :
  ```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=DEBUG ..
  ```
