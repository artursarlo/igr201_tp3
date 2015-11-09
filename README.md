# IGR201_TP_OpenGL 

This is a LAB exercise covering the basics of OpenGl part for the course IGR201.
I tried to well comment the code, so that it would be easier to understand what
have been done without additional documentation. The following comments are just
a personal description of the main challenges encountered during the lab sessions
as well as a description of main strategies adopted to solve them.

The document called "TP_OpenGL.pdf" at this folder contains the questions for the
exercises.

## Exercise 1

### A
Nothing really difficult here. The only relevant point here is the fact that the
given piece of code draws a triangle in the counter-clock-wise direction.
Something that doesn´t work for the following exercise.

### B
The main difficult here, for me, was thinking on the strategy to draw a sphere
using triangles in a space that i didn´t understand well. I mean, I didn´t know
the xyz-axes orientation.
After the teacher hint, I went using sphere coordinates, and taking 4 points at
each time on the surface of the sphere for drawing 2 triangles at each passage.
Even after that, I had some bugs because the triangles were drawn just when their
3 points were declared in a clock-wise direction.

### C
Easy step. Nothing really challenging here.

## Exercise 2

### A
I had a hard time first trying to understand the purpose of a Normal definition
for each Vertex. After the explanation of the functioning of the lightning
equations inside OpenGL I was able to modify the glSphere equation and continue.

### B
Easy step. Nothing really challenging here.

### C
I really spent a huge time in this step because of a relative simple problem.
Basically the (u,v) coordinate system used for the textures wasn´t nearly clear
to me. In addition to that, the coordinates that the teacher indicated me, were
different from those that I saw on the internet. Well, after solving this, I
managed to see the correct colors from my texture.

## Exercise 3

### A
Easy step. Nothing really challenging here.

### B
The concept of interaction wasn´t really clear to me. After understanding that
we were supposed to determine acceleration from user interaction and calculate
speed and position based on that, everything went good.

### C
I can say that it was just after this step, that I understand the coordinate
system in OpenGL and the camera functioning.
The tricky thing at this point was the mouse positions returned from the mouse
and motion functions. The returned y coordinates are inverted.

## Final Considerations
The program was developed using a git repository.
If you want to access older versions before the code clean and etc you can go to:
https://github.com/artursarlo/igr201_tp3

Thanks for all the support.
Artur Andrade Motta Sarlo
aandrade@telecom-paristech.fr
