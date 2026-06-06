# Sole

Sole is a cross platform software rendering engine (CPU side rendering) that I made to learn about the modern graphics pipeline and how to implement it using projection matrices, quaternions, etc.

It is not very optimised or efficient however it does have a relational system for objects, where objects can be the children of other objects.

## Building

Sole has a small demo in it of a cube spinning with flat shading.

To run this, pull the repo, enter the directory in the terminal and run:

```
cmake -S , -B build
cmake --build build
```

Then to run:

```
build/demo/demo.exe
```

Ensure your terminal window is large enough to fit the rendered output to avoid visual artifacts.
You may encounter some visual artifacts if your terminal emulator does not support UTF-8 character.

Please report any errors via an issue, detailing your OS and system specs.
