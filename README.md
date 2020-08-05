# Rubik's Cube Solver
This program, written in C++ using the GLFW, GLEW, and GLM libraries, will simulate and solve a Rubik's cube.
## Setup
In order to run this program, the user must have the [GLFW](https://www.glfw.org/), [GLEW](http://glew.sourceforge.net/), and [GLM](https://github.com/g-truc/glm) libraries downloaded and linked.
## Controls
- Press **space** to scramble the cube
- Press **enter** to solve the cube
- Press **8** to rotate the cube 90 degrees clockwise along the x-axis
- Press **2** to rotate the cube 90 degrees counter-clockwise along the x-axis
- Press **4** to rotate the cube 90 degrees clockwise along the y-axis
- Press **6** to rotate the cube 90 degrees counter-clockwise along the y-axis
- Press **9** to rotate the cube 90 degrees clockwise along the z-axis
- Press **7** to rotate the cube 90 degrees counter-clockwise along the z-axis
- Press the **button** associated with a face/slice to turn it 90 degrees clockwise
- Hold **shift** and press the **button** associated with a face/slice to turn it 90 degrees counter-clockwise
- Hold **control** and press the **button** associated with a face/slice to turn it 180 degrees
- Below is a list of faces/slices and the button associated with them
  - L Face - A
  - R Face - D
  - U Face - W
  - D Face - S
  - F Face - E
  - B Face - Q
  - M Slice - Z
  - E Slice - X
  - S Slice - C
