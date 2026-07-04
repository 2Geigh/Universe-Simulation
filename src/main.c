#include <string.h>
#define GLFW_INCLUDE_NONE
#include "../include/GLFW/glfw3.h"
#include "../include/glad/glad.h"
#include <stdio.h>
#include <stdlib.h>

struct Screen {
  int width;
  int height;
  char title[16];
};

char *get_shader_content(const char *filename) {

  FILE *fp;
  long size = 0;
  char *shaderContent;

  // Read file to get size
  // in "rb", r is "read" and b is "open in binary mode" (no byte conversions
  // (e.g., translation of newlines))
  fp = fopen(filename, "rb");
  if (fp == NULL) {
    return "";
  }

  // Get size of file
  fseek(fp, 0L, SEEK_END); // Aligns the file for reading
  size =
      ftell(fp) +
      1; // Gets length of file based on current position indicator (SEEK_END)
  fclose(fp);

  // Read file for content
  fp = fopen(filename, "r");
  shaderContent = memset(
      malloc(size), '\0',
      size); // memset fills a block of memory with a specified value (pointer
             // to memory block, value to fill, number of bytes to fill)
  fread(shaderContent, 1, size - 1, fp);
  fclose(fp);

  return shaderContent;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  (void)window;
}

int main(void) {
  struct Screen viewport = {
      .width = 500,
      .height = 300,
      .title = "Ooniverse",
  };

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MacOS

  GLFWwindow *window = glfwCreateWindow(viewport.width, viewport.height,
                                        viewport.title, NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    return -1;
  }

  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  // VBO = Vertex buffer object
  // A buffer object is a batch-allocation of GPU memory
  // Allows for faster data transmission to the GPU
  unsigned int VBO;
  glad_glGenBuffers(1, &VBO); // n = 1 is the buffer's unique ID

  // Buffer binding means becoming the target of all buffer configuration
  // calls
  glBindBuffer(GL_ARRAY_BUFFER, // GL_ARRAY_BUFFER is the type of a VBO
               VBO);

  // Copying vertices data into GL_ARRAY_BUFFER (the currently-bounded VBO)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // GL_STATIC_DRAW: the data is set only once and is used often
  // GL_STREAM_DRAW: data is set only once and used very little
  // GL_DYNAMIC_DRAW: data is changed alot and used many times

  unsigned int vertexShader;
  // vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vertexShaderSource =
      "#version 330 core\n"
      // We declare this shader to apply to a
      // single 3D vector (vec3) we'll call aPos
      "layout(location = 0) in vec3 aPos;\n"

      "void main() {\n"
      // What gl_Position is set to at the end of main() is a vertex shader's
      // output
      "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0"; // Because a C string is just an array of characters, \0 is a
             // null
             // character that signifies the end of a string
  // Attaches the above shader source string to the declared vertexShader
  glad_glShaderSource(vertexShader, 1, &vertexShaderSource,
                      NULL); // count is how many strings we're passing as
                             // shader source code (in this case, 1)
  glad_glCompileShader(vertexShader);
  int shaderCompilationSuccess;
  enum { shaderInfoLogBufSize = 512 };
  char infoLog[shaderInfoLogBufSize];
  glad_glGetShaderiv(vertexShader, GL_COMPILE_STATUS,
                     &shaderCompilationSuccess);
  if (!shaderCompilationSuccess) {
    glad_glGetShaderInfoLog(vertexShader, shaderInfoLogBufSize, NULL, infoLog);
    printf("ERROR: SHADER: VERTEX: COMPILATION_FAILED\n%s\n", infoLog);
  }

  // THE OUTPUT OF THE PREVIOUS SHADER BECOMES THE INPUT FOR THE NEXT SHADER
  const char *fragmentShaderSource =
      "#version 330\n"
      // We declare the output of this shader to apply to a single 4D vector
      // called FragColor
      "out vec4 FragColor;\n"

      "void main() {\n"
      // The dimensions of FragColor represent RGBA, respectively
      "FragColor = vec4(1.0f, 0.25f, 1.0f, 1.0f)\n"
      "}\0";
  unsigned int fragmentShader;
  fragmentShader = glad_glCreateShader(fragmentShader);
  glad_glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glad_glCompileShader(fragmentShader);
  glad_glGetShaderiv(fragmentShader, GL_COMPILE_STATUS,
                     &shaderCompilationSuccess);
  if (!shaderCompilationSuccess) {
    glad_glGetShaderInfoLog(vertexShader, shaderInfoLogBufSize, NULL, infoLog);
    printf("ERROR: SHADER: VERTEX: COMPILATION_FAILED\n%s\n", infoLog);
  }

  // A shader program object is the final linkage of multiple shaders
  // (like, say, a vertex-fragment shader pair)
  unsigned int shaderProgram;
  shaderProgram = glad_glCreateProgram();
  glad_glAttachShader(shaderProgram, vertexShader);
  glad_glAttachShader(shaderProgram, fragmentShader);
  glad_glLinkProgram(shaderProgram);
  int shaderProgramLinkSuccess;
  glad_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramLinkSuccess);

  // Shader objects aren't needed after linking them into a shader program
  glad_glDeleteShader(vertexShader);
  glad_glDeleteShader(fragmentShader);

  // All rendering calls after this line'll use the shaderProgram program object
  glad_glUseProgram(shaderProgram);

  glViewport(0, 0, viewport.width, viewport.height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glad_glClearColor(0.1f, 0.3f, 0.9f, 1.0f);
    glad_glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents(); // checks for events (e.g. keyboard or mouse inputs)
  }

  glfwTerminate();
  return 0;
}
