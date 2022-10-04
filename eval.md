# Project evaluation

## A - Programm usability (total : 6.5/8 points)

### 1 - Description and easiness of the installation procedure : 2/2

Very good extended documentation. The README.md in the project folder should serve as main documentation, and point to all specific documentation pages (installation, exampes, code documentation, etc ...).

### 2 - Code compilation without error (and warnings) : 2/2 points

Some warning because coming from dependency code, but I won't remove points for that.

### 3 - Quality/efficiency/practicality of the input file format : 1/1 points

Use of YAML files, with complete parameter settings.


### 4 - Usage documentation (with examples of use, tutorials, ...) : 0.5/2 points

You developped quite a lot of features, a GUI, a lot of parameters. And even if there is some examples to get inspired from, it's quite hard to see how to do simple things, or to build a scene from scratch, or to understand the significance of each parameter in a scene setting (for instance, what is the far/fov parameters for the camera ?).
So at the end, it's quite an impressive huge machinery, but I'm not sure that people (other than you) would easily use your program.

### 5 - Execution of the given examples without error : 1/1 points

All right.


## B - Code documentation (total : 3/6 points)


### 1 - UML class diagram : 1/2 points

A bit messy (PlantUML would have been more clear than Mermaid for that), and hard to distringuish between inheritance, composition and aggregation.
Also, there is no interface in C++ : it's a Java concept, which roughly corresponds to a fully abstract class in C++ (only abstract methods, _i.e_ prototype for class inheriting from it).

### 2 - Docstring for functions and classes : 0/2 points

None seen

### 3 - Generation of documentation (_e.g_ with Doxygen) : 2/2 points

Automatically build using CMake (good!), however still missing some main code documentation file.

## C - Development quality (total : 4.5/6 points)

### 1 - Capacity to use external libraries (_e.g_ OpenCV, Eigen, or else ...) : 2/2 points

Lot of additional libraries used.

### 2 - Implementation of required features ([primitive objects](./objects.md#primitive-objects), [transformations](./objects.md#operators-for-geometric-transformations), [combinations](./objects.md#logical-operations-with-objects) and [light model](./lightmodel.md)) : 1.5/2 points

Missing combinations (intersection, union, ...)

### 3 - Automated testing procedures (to be used for further code development) : 0/1 points

None seen

### 4 - Additional features (other primitives, videos generation, etc ...) : 1/1 points

Plenty on that side (GUI, textures, vox reading, ...)

## Conclusion

Final grade :  14 / 20 =>  3.5 bonus points

_General remarks_ : A lot of additional features, which is great. However, the more complex your program is, the more detailled the use documentation (and code) should be.
Don't forget that the best softwares are not necessarily the one that can do gazillion of impressive stuff, but more the one that other people can easily use. So don't go too fast in code development, and update the (user/code) documentation that comes with it.
