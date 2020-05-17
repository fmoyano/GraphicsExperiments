# Welcome

This a summary and actual implementation of topics that I discuss on my [Spanish website](https://rayosypixeles.com/) on graphics programming. I'm not an expert at all, so don't expect these experiments to be jaw-dropping. I'm just trying to build my knowledge from the ground-up and therefore you'll see my first experiments are all about simply rendering triangles/quads without any further processing.

# Building the examples

I'm using Visual Studio 2019 for building the experiments. I use [GLFW](https://www.glfw.org/) for OpenGL context management and input processing, and [Glad](https://glad.dav1d.de/) for accessing OpenGL extensions. The best resource I know to learn how to configure all this stuff is the [Learn OpenGL](https://learnopengl.com/) site.

# Projects

## SetupOpenGL

As the title suggests, this is just a project to test the correct configuration of all you need: GLFW, Glad, GLM, stb_image.h, etc. The outcome is three rotating textured quads (textures taken from [Learn OpenGL](https://learnopengl.com/)), as shown below:

![Outcome of SetupOpenGL Project](https://rayosypixeles.com/wp-content/uploads/2020/05/ezgif-5-1fa61e61d530.gif)

Maybe the most interesting part of this project is the initialization part (for those who are still learning OpenGL). There are three different Initialize*() functions: the first one uses a single VBO for the attributes; the second uses two VBOs, one for each attribute (position and texture coordinate); the last one uses the most modern approach, available since OpenGL 4.6, of using binding points and named buffers (no need to bind buffers to target, except for the element buffer).


# Contact

You can contact me via francis[at]rayosypixeles[dot]com
