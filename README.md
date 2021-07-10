This is a simple grey-scale renderer that I made to experiment with camera movement and some classical shading methods:
- flat and Gouraud shading: [http://page.mi.fu-berlin.de/block/htw-lehre/wise2012_2013/bel_und_rend/skripte/gouraud1971.pdf](http://page.mi.fu-berlin.de/block/htw-lehre/wise2012_2013/bel_und_rend/skripte/gouraud1971.pdf)
- Phong shading: [http://www.cs.northwestern.edu/~ago820/cs395/Papers/Phong_1975.pdf](http://www.cs.northwestern.edu/~ago820/cs395/Papers/Phong_1975.pdf)

### Instructions
Install the following dependencies: 
- [GLEW](http://glew.sourceforge.net/) (OpenGL loader)
- [GLFW](https://www.glfw.org/) (windowing/input)
- [GLM](https://github.com/g-truc/glm) (linear algebra)
- [assimp](https://www.assimp.org/) (model loading)

Build the program:
```sh
make
```

Run the program:
```sh
./_build/rendering
```

Controls:
- **escape**: close program
- **f**: switch between free camera and fixed camera
- **mouse**: turn camera (free mode) or turn around target (fixed mode)
- **up/down**: zoom in/out
- **left/right**: move sideways (free mode only)
- **1**: flat shading
- **2**: Gouraud shading
- **3**: Phong shading
