
/*
Jessica Dawson
SFWRENG 3GC3:Computer Graphics
Assignment 1

Draws and animates a simple solar system comprised of the sun, earth and moon

Significant portions of code come from 3GC3 assignment0
A small portion of code comes from: https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html

TO PLAY ANIMATION PRESS THE "L" KEY
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string.h>

//definitions for cube vertices to make typing them out easier
//near top right vert
#define NTR_VERT 1.0f, 1.0f, 1.0f
#define NTL_VERT -1.0f, 1.0f, 1.0f
#define NBR_VERT 1.0f, -1.0f, 1.0f
#define NBL_VERT -1.0f, -1.0f, 1.0f
#define FTR_VERT 1.0f, 1.0f, -1.0f
#define FTL_VERT -1.0f, 1.0f, -1.0f
#define FBR_VERT 1.0f, -1.0f, -1.0f
//far bottom left vert
#define FBL_VERT -1.0f, -1.0f, -1.0f

//definitions for cube colors to make typing them out easier
#define FUCHSIA 1.0f, 0.0f, 1.0f, 1.0f
#define RED 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define YELLOW 1.0f, 1.0f, 0.0f, 1.0f
#define AQUA 0.0f, 1.0f, 1.0f, 1.0f
#define BLUE 0.0f, 0.0f, 1.0f, 1.0f

//take and save a screenshot
static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte * pixels = new GLubyte [totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id ++;
    delete [] pixels;
    fout.flush();
    fout.close();
}

//key board control
static bool play = false;
void processInput(GLFWwindow *window) {
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //press p to capture screen
    if(glfwGetKey(window, GLFW_KEY_P) ==GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("Assignment0-ss", buffer_width, buffer_height);
    }
    //press l to begin animation
    else if(glfwGetKey(window, GLFW_KEY_L) ==GLFW_PRESS && !play)
    {
        std::cout << "Playing Animation" << std::endl;
        play = true;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// function from: https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html
// used for reading in shaders from external files
std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

//rotation functions
//sun rotate around self
float get_sun_rotate_angle_around_itself(float day) {return day * 360.0f/27;}
//earth rotate around self
float get_earth_rotate_angle_around_itself(float day) {return day * 360.0f;}
//earth rotate around sun
float get_earth_rotate_angle_around_sun(float day) {return day * 360.0f/365;}
//moon rotate around self
float get_moon_rotate_angle_around_itself(float day) {return day * 360.0f/28;}
//moon rotate around earth
float get_moon_rotate_angle_around_earth(float day) {return day * 360.0f/28;}

//function to choose camera focus, s for sun, e for earth, m for moon (not case sensitive)
glm::vec3 get_camera_focus_vector(char focus, float day) {
    glm::mat4 Mcamrot;
    glm::vec4 camTarget;
    if (focus == 's' || focus == 'S') {
        //camera target vector for sun focus
        return glm::vec3(0., 0., 0.);
    } else if (focus == 'e' || focus == 'E') {
        //camera target vector for earth focus
        Mcamrot = glm::rotate(glm::mat4(1.0f), glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0., 1., 0.));
        camTarget = Mcamrot * glm::vec4(20., 0., 0., 1.);
        return glm::vec3(camTarget.x, camTarget.y, camTarget.z);
    } else if (focus == 'm' || focus == 'M') {
        //camera target vector for moon focus
        Mcamrot = glm::rotate(glm::mat4(1.0f), glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0., 1., 0.));
        Mcamrot = glm::translate(Mcamrot, glm::vec3(20., 0., 0.));
        Mcamrot = glm::rotate(Mcamrot, glm::radians(get_moon_rotate_angle_around_earth(day)), glm::vec3(0., 1., 0.));
        camTarget = Mcamrot * glm::vec4(10., 0., 0., 1.);
        return glm::vec3(camTarget.x, camTarget.y, camTarget.z);
    } else {
        //default is sun
        return glm::vec3(0., 0., 0.);
    }
}

//main program
int main() {
    //initialize a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    int windowWidth = 1024;
    int windowHeight = 768;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Assignment1", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "GLFW Window Failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return -1;
    }
    
    //enable back face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //shaders
    std::string vertexShaderStr = readFile("./vertex_shader.vert");
    std::string fragmentShaderStr = readFile("./fragment_shader.frag");
    const char *vertexShaderSource = vertexShaderStr.c_str();
    const char *fragmentShaderSource = fragmentShaderStr.c_str();

    int success;
    char error_msg[512];
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, NULL, error_msg);
        std::cout << "Vertex Shader Failed: " << error_msg << std::endl;
    }
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, NULL, error_msg);
        std::cout << "Fragment Shader Failed: " << error_msg << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
        std::cout << "Program Link Error: " << error_msg << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    //cube vertex positions (uses definitions from top of file)
    float cube_verts[] = {
        //front face
        NTR_VERT, NTL_VERT, NBL_VERT,
        NBL_VERT, NBR_VERT, NTR_VERT,

        //back face
        FTR_VERT, FBL_VERT, FTL_VERT,
        FBL_VERT, FTR_VERT, FBR_VERT,

        //right face
        FBR_VERT, FTR_VERT, NTR_VERT,
        NTR_VERT, NBR_VERT, FBR_VERT,

        //left face
        NBL_VERT, NTL_VERT, FTL_VERT,
        FTL_VERT, FBL_VERT, NBL_VERT,

        //top face
        NTL_VERT, NTR_VERT, FTR_VERT,
        FTR_VERT, FTL_VERT, NTL_VERT,

        //bottom face
        FBR_VERT, NBR_VERT, NBL_VERT,
        NBL_VERT, FBL_VERT, FBR_VERT
    };

    //cube colors (uses definitions from top of file)
    float cube_colors[] = {
        //front face
        FUCHSIA, FUCHSIA, FUCHSIA,
        FUCHSIA, FUCHSIA, FUCHSIA,

        //back face
        RED, RED, RED,
        RED, RED, RED,

        //right face
        YELLOW, YELLOW, YELLOW,
        YELLOW, YELLOW, YELLOW,

        //left face
        GREEN, GREEN, GREEN,
        GREEN, GREEN, GREEN,

        //top face
        AQUA, AQUA, AQUA,
        AQUA, AQUA, AQUA,

        //bottom face
        BLUE, BLUE, BLUE,
        BLUE, BLUE, BLUE
    };

    //vao/vbo stuff
    unsigned int VAO;
    unsigned int VBO[2];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO[0]);
    glGenBuffers(1, &VBO[1]);
    glBindVertexArray(VAO);

    //load vertices to attrib location 0
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //load colors to attrib location 1
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //projection matrix
    glm::mat4 Mproj = glm::perspective(glm::radians(30.0f), 4.0f/3.0f, 0.1f, 1000.0f);

    //initialize transformation matrices and camera target matrix
    glm::mat4 Mtotal;
    glm::mat4 Msun;
    glm::mat4 Mearth;
    glm::mat4 Mmoon;

    //starting day and loop increment
    float day = 0, inc = 1.0f/24; // inc = 1.0f/24

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //get uniform id for vertex shader transformation matrix
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "transform");

        //array to store camera vectors
        glm::vec3 camera[] = {
            glm::vec3(50.0f, 50.0f, 100.0f), //position vector
            get_camera_focus_vector('S', day), //target vector
            glm::vec3(0.0f, 1.0f, 0.0f)  //up vector
        };

        //camera matrix
        glm::mat4 Mcam = glm::lookAt(camera[0], camera[1], camera[2]);

        //calculate transformation matrices
        //revolve sun
        Msun = glm::rotate(glm::mat4(1.0f), glm::radians(get_sun_rotate_angle_around_itself(day)), glm::vec3(0., 1., 0.));
        Msun = glm::scale(Msun, glm::vec3(4., 4., 4.));

        //orbit earth
        Mearth = glm::rotate(glm::mat4(1.0f), glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0., 1., 0.));
        Mearth = glm::translate(Mearth, glm::vec3(20., 0., 0.));
        //revolve earth
        Mearth = glm::rotate(Mearth, glm::radians(-get_earth_rotate_angle_around_sun(day)), glm::vec3(0., 1., 0.));
        Mearth = glm::rotate(Mearth, glm::radians(get_earth_rotate_angle_around_itself(day)),
                            glm::vec3(glm::sin(glm::radians(23.4)), glm::cos(glm::radians(23.4)), 0.));
        //tilt earth
        Mearth = glm::rotate(Mearth, glm::radians(-23.4f), glm::vec3(0., 0., 1.));
        Mearth = glm::scale(Mearth, glm::vec3(2.5, 2.5, 2.5));
        
        //orbit moon around sun
        Mmoon = glm::rotate(glm::mat4(1.0f), glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0., 1., 0.));
        Mmoon = glm::translate(Mmoon, glm::vec3(20., 0., 0.));
        //orbit moon around earth
        Mmoon = glm::rotate(Mmoon, glm::radians(get_moon_rotate_angle_around_earth(day)), glm::vec3(0., 1., 0.));
        Mmoon = glm::translate(Mmoon, glm::vec3(10., 0., 0.));
        Mmoon = glm::scale(Mmoon, glm::vec3(1.5, 1.5, 1.5));
        
        //draw sun
        //create total transformation matrix from local to canonical space
        Mtotal = Mproj * Mcam * Msun;
        //pass transformation matrix to vertex shader
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(Mtotal));
        //draw triangles
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //draw earth
        Mtotal = Mproj * Mcam * Mearth;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(Mtotal));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //draw moon
        Mtotal = Mproj * Mcam * Mmoon;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(Mtotal));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //play is a value used to activate the animation
        if (play) {
            day += inc;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO[0]);
    glDeleteBuffers(1, &VBO[1]);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
