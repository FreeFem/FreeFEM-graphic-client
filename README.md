# FreeFEM-graphic-client

How to build :

 &nbsp;&nbsp;&nbsp;&nbsp;Dowload the lastest LunarG vulkan runtime and install it on your computer.
 &nbsp;&nbsp;&nbsp;&nbsp;Find it [here](https://vulkan.lunarg.com/)

 &nbsp;&nbsp;&nbsp;&nbsp;Build the vulkan headers and install them.
 &nbsp;&nbsp;&nbsp;&nbsp;Find it [here](https://github.com/KhronosGroup/Vulkan-Headers).

Optional :

 &nbsp;&nbsp;&nbsp;&nbsp;If you want to have in depth vulkan debug capabilities, install the vulkan validation layers.
 &nbsp;&nbsp;&nbsp;&nbsp;Find it [here](https://github.com/KhronosGroup/Vulkan-ValidationLayers).

 &nbsp;&nbsp;&nbsp;&nbsp;Build using these commands :
  ```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=DEBUG ..
  ```

  Build the documentation :
 ```
 doxygen Doxyfile
 ```
