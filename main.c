#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <time.h>
#include "perlin.h"
#include "math.h"

float gSize = 2;
size_t gridWidth = 800; 
size_t gridHeight = 800;

char* readFile(FILE *file, size_t *size) {
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *buffer = malloc(*size + 1);
    fread(buffer, 1, *size, file);
    buffer[*size] = 0;
    return buffer; 
}

unsigned int loadShader(GLenum type, char *filename) {
    printf("Trying to load file %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to load shader %s\n", filename);
        exit(1);
    }
    
    size_t sourceSize; 
    char *source = readFile(file, &sourceSize);
    
    printf("Shader source file:\n %s\n", source);
    
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char * const *)&source, NULL); 
    glCompileShader(shader);
    
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    
    if (!result) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        
        printf("Shader compilation failed error:\n%s\n", log);
        exit(1);    
    }
    
    free(source);
    return shader;
}

void linkProgram(unsigned int shader) {
    glLinkProgram(shader);
    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shader, 512, NULL, infoLog);     
        printf("Shader linking failed error:\n%s\n", infoLog); 
        exit(1);
    }
}

void pushVec(float **cursor, fvec3 p) {
    *(*cursor)++ = p.x;
    *(*cursor)++ = p.y;
    *(*cursor)++ = p.z;
}


fvec3 calculateNormal(fvec3 a, fvec3 b, fvec3 c) {
    fvec3 an = (fvec3) {a.x - c.x, a.y - c.y, a.z - c.z}; 
    fvec3 bn = (fvec3) {b.x - c.x, b.y - c.y, b.z - c.z}; 
    
    fvec3 normal = crossProduct(an, bn);

    if (normal.y < 0) {
        normal.x = -normal.x;
        normal.y = -normal.y;
        normal.z = -normal.z;
    }
    return normal;
}

void pushTriangle(float **cursor, fvec3 a, fvec3 b, fvec3 c) {
    fvec3 normal = calculateNormal(a, b, c); 

//    fvec3 normal = (fvec3) {0, 1, 0};
    pushVec(cursor, a);
    pushVec(cursor, normal);
    pushVec(cursor, b);
    pushVec(cursor, normal);
    pushVec(cursor, c);
    pushVec(cursor, normal);
}

unsigned int shaderProgram;
unsigned int VBO;
unsigned int VAO;

size_t vertCount;

unsigned int timeCounter = 0;

float *gridBuffer = NULL;

void computeGrid(size_t w, size_t h, float gSize, float *grid) {
    //   *---*
    //   | \ |
    //   *---*
    //   a   b
    // Triangles will not share vertexes.
    
    size_t cellCount = w * h; 
    vertCount = cellCount * 6;
   
    size_t vertexSize = 6 * sizeof(float); 
    size_t size = vertCount * vertexSize; 
    
    if (!gridBuffer)
        gridBuffer = malloc(size); 
 
    float *cursor = gridBuffer;
    
    for (size_t x = 1; x < w; x++) {
        for (size_t y = 1; y < h; y++) {
    
            float rX = x * gSize;
            float rY = y * gSize;
            #define BM(x, y) fmax(5, grid[(y) * w + (x)] * gSize * 50)
            float a = BM(x - 1, y - 1);
            float b = BM(x    , y - 1); 
            float c = BM(x    , y); 
            float d = BM(x - 1, y); 
            #undef BM

            fvec3 pA1 = (fvec3) {rX,         b, rY - gSize};
            fvec3 pB1 = (fvec3) {rX,         c, rY};
            fvec3 pC1 = (fvec3) {rX - gSize, d, rY};
            pushTriangle(&cursor, pA1, pB1, pC1);
 
            fvec3 pA2 = (fvec3) {rX - gSize, d, rY};
            fvec3 pB2 = (fvec3) {rX - gSize, a, rY - gSize};
            fvec3 pC2 = (fvec3) {rX        , b, rY - gSize};
                
            pushTriangle(&cursor, pA2, pB2, pC2);
        }
    }
}

void calculateGrid(size_t w, size_t h, float gSize) {
    size_t cellCount = w * h; 

    vertCount = cellCount * 6;
   
    // A vertex is 3 floats for position and 3 floats for normal.
    size_t vertexSize = 6 * sizeof(float); 
    size_t size = vertCount * vertexSize; 

    static int printedCount = 0;
    
    float *grid = malloc(sizeof(float) * w * h);
    perlin(w, h, 40, grid);
    computeGrid(w, h, gSize, grid);

    if (!printedCount) {
        printedCount = 1;    
        printf("Triangle count: %d\n", cellCount * 2);
    }


    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, gridBuffer, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


unsigned int uPerspectiveLocation;
unsigned int uWorldLocation;
unsigned int uCameraLocation;

void init() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    unsigned int vertShader = loadShader(GL_VERTEX_SHADER, "simple.vert");
    unsigned int fragShader = loadShader(GL_FRAGMENT_SHADER, "simple.frag");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    linkProgram(shaderProgram); 

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    uPerspectiveLocation = glGetUniformLocation(shaderProgram, "perspective"); 
    uWorldLocation = glGetUniformLocation(shaderProgram, "world"); 
    uCameraLocation = glGetUniformLocation(shaderProgram, "camera"); 
 
    glBindAttribLocation(shaderProgram, 0, "pos");
    glBindAttribLocation(shaderProgram, 1, "normal");

    calculateGrid(gridWidth, gridHeight, gSize);
}


void render() {
    timeCounter++;
    glClearColor(0.1, 0.1, 0.1, 1.0);        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram); 

    matrix4 modelTranslation = createTranslation(-(gridWidth / 2.0) * gSize, 0, -(gridHeight / 2.0) * gSize);
    matrix4 rotation = createRotation(0, 0, 0);
   

    // rotate the camera around the center. 
    matrix4 camera = M4CALLOC(); 
    matrix4 cameraTranslation = createTranslation(0, 150, 200);

    matrix4 cameraRotation = createRotation(0, 0, RAD(-25));
    matrix4 cameraSecond = createRotation(0, RAD(timeCounter / 5.0), 0);

    matrix4 cameraTmp = M4CALLOC();
    multiplyMatrix(cameraTranslation, cameraRotation, cameraTmp);
    multiplyMatrix(cameraSecond, cameraTmp, camera);
    free(cameraTmp);

    matrix4 model = M4CALLOC(); 
    multiplyMatrix(rotation, modelTranslation, model);
    free(modelTranslation);
    free(rotation);    

    matrix4 world = M4CALLOC();
    matrix4 translation = createTranslation(0, 0, 0);
    multiplyMatrix(translation, model, world); 
    free(translation);
 
    matrix4 projection = createProjectionMatrix(RAD(45), 1, 10000, 0.1);
 
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
 
    glUniformMatrix4fv(uWorldLocation, 1, GL_FALSE, world);
    glUniformMatrix4fv(uPerspectiveLocation, 1, GL_FALSE, projection);
    glUniformMatrix4fv(uCameraLocation, 1, GL_FALSE, camera);
    free(world);
    free(camera);
    free(projection);

   //computeGrid(gridWidth, gridHeight, gSize);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    glDrawArrays(GL_TRIANGLES, 0, vertCount);

    glUseProgram(0); 
}

void errorCallback(int eCode, const char *error) {
   printf("GLFW Error:%s", error); 
}
int main() {
    glfwSetErrorCallback(errorCallback);

    GLFWwindow *window;
    if (!glfwInit()) {
        puts("Failed to initialize glfw");
        return -1;    
    }

    window = glfwCreateWindow(900, 900, "Terrain", NULL, NULL);  
    if (!window) {
        puts("Failed to initialize window");
        glfwTerminate();
        return -1;
    }    

    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit(); 
    if (err != GLEW_OK) {
        puts("Failed to init glew");
        printf("Error %s\n", glewGetErrorString(err));
        glfwTerminate();    
        return -1;
    }

    init();
    
    size_t frameCounter = 0;
    time_t frameStart; 
    time(&frameStart); 

    while (!glfwWindowShouldClose(window)) {
   
         render();

        frameCounter++;
        if (frameCounter == 60) {
            frameCounter = 0;
            time_t now;
            time(&now);            
            
            double diff = difftime(now, frameStart);
            diff /= 60;
            time(&frameStart);
            
            printf("time: %f\n", diff);
            printf("FPS: %f\n",  1.0 / diff);
        }


        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
