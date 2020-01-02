#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <assert.h>

#define M_MATH_IMPLEMENTATION
#include "m_math.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define WINDOW_W 600
#define WINDOW_H 600

void close_callback(GLFWwindow * window) {
    printf("close_callback");
}

void size_callback(GLFWwindow * window, int width, int height) {
    printf("size_callback");
}

void cursorpos_callback(GLFWwindow * window, double mx, double my) {}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    printf("key_callback\n");
    glfwSetWindowShouldClose(window, 1);
}

void mousebutton_callback(GLFWwindow * window, int button, int action, int mods) {}

void char_callback(GLFWwindow * window, unsigned int key) {
    printf("char_callback\n");
}

void error_callback(int error, const char* description) {
    printf("%s\n", description);
}

void print_fun(const char* f) {
    printf("%s\n", f);
}

void print_gl_string(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    printf("GL %s = %s\n", name, v);
}

void gl_error(const char *file, int line) {
    bool has_errors = false;
    for (GLint error = glGetError(); error; error = glGetError()) {
        printf("\tGL_ERROR: file:%s:%d -- Hex: 0x%x Dec: %d)\n", file, line, error, error);
        switch (error) {
            case GL_INVALID_ENUM:
                printf("\tINVALID_ENUM\n");
                break;
            case GL_INVALID_VALUE:
                printf("\tINVALID_VALUE\n");
                break;
            case GL_INVALID_OPERATION:
                printf("\tINVALID_OPERATION\n");
                break;
            case GL_OUT_OF_MEMORY:
                printf("\tOUT_OF_MEMORY\n");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("\tINVALID_FRAMEBUFFER_OPERATION\n");
                break;
            default:
                printf("\t__UNEXPECTED_VALUE__)\n");
        }
        has_errors = true;
    }
    assert(!has_errors);
}

#define GL_ERR gl_error(__FILE__, __LINE__)

GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = (GLchar*) malloc(infoLogLength * sizeof(char));
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        printf("Compilation error in shader %s\n", strInfoLog);
        free(strInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    printf("Success!\n");
    return shader;
}

inline float *push_v4_arr(float *v, float x, float y, float z, float a) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = a;
    printf("push_v4_arr: %f, %f, %f, %f\n",  x, y, z, a);
    return v + 4;
}

inline float *push_v5_arr(float *v, float x, float y, float z, float s, float t) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = s;
    v[4] = t;
    printf("push_v5_arr: %f, %f, %f, %f, %f\n",  x, y, z, s, t);
    return v + 5;
}

float* push_textured_quad_arr(float*v, float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1) {
    // quad 1 2 3 4
    // -->
    // triangle 1 2 3
    // triangle 3 4 1

    //v = push_v5_arr(v, x0, y0, 0, s0, t0); // 1
    //v = push_v5_arr(v, x1, y0, 0, s1, t0); // 2
    //v = push_v5_arr(v, x1, y1, 0, s1, t1); // 3
    //v = push_v5_arr(v, x0, y1, 0, s0, t1); // 4

    v = push_v5_arr(v, x0, y0, 1.0, s0, t0); // 1
    v = push_v5_arr(v, x1, y0, 1.0, s1, t0); // 2
    v = push_v5_arr(v, x1, y1, 1.0, s1, t1); // 3

    v = push_v5_arr(v, x1, y1, 1.0, s1, t1); // 3
    v = push_v5_arr(v, x0, y1, 1.0, s0, t1); // 4
    v = push_v5_arr(v, x0, y0, 1.0, s0, t0); // 1

    return v;
}

float* push_textured_quad_scaled_arr(float*v, float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1, float scale_x, float scale_y) {
    return push_textured_quad_arr(v, x0 * scale_x, y0 * scale_y, x1 * scale_x, y1 * scale_y, s0, t0, s1, t1);
}

void set_float3(float3 *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void set_float2(float2 *v, float x, float y) {
    v->x = x;
    v->y = y;
}

const unsigned char* read_entire_file(char* filename) {
    unsigned char *result = 0;
    
    FILE *file = fopen(filename, "r");
    if(file) {
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        result = (unsigned char *)malloc(file_size + 1);
        fread(result, file_size, 1, file);
        result[file_size] = 0;
        
        fclose(file);
    } else {
    	printf("FAILED TO OPEN %s\n", filename);
    }

    return result;
}

int compile_shader_program(const char* str_vert_shader, const char* str_frag_shader, const char* attrib_name_0, const char* attrib_name_1) {
    GLuint vert_shader;
    GLuint frag_shader;
    GLuint prog_object;

    vert_shader = compile_shader(GL_VERTEX_SHADER, str_vert_shader);
    if(vert_shader == 0) {
        printf("Error compiling vert shader");
        return 1;
    }

    frag_shader = compile_shader(GL_FRAGMENT_SHADER, str_frag_shader);
    if(frag_shader == 0) {
        printf("Error compiling frag shader");
        return 1;
    }

    printf("Creating shader program");

    prog_object = glCreateProgram();
    glAttachShader(prog_object, vert_shader);
    glAttachShader(prog_object, frag_shader);

    if (attrib_name_0 != NULL) {
        printf("Binding attrib 0");
        glBindAttribLocation(prog_object, 0, attrib_name_0);
    }

    if (attrib_name_1 != NULL) {
        printf("Binding attrib 1");
        glBindAttribLocation(prog_object, 1, attrib_name_1);
    }

    printf("Linking shader program");
    glLinkProgram(prog_object);

    return prog_object;
}


GLuint upload_new_texture(int width, int height, int channels, unsigned char* pixels) {
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if (channels == 3) {
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    } else if(channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

int main(int argc, char const *argv[]) {
	printf("Hello!\n");

    GLFWwindow* window;
    if(!glfwInit()) {
        printf("glfw init failed");
    }
    else {
        printf("Glfw initialized");
    }
    window = glfwCreateWindow(WINDOW_W, WINDOW_H, "stb-true-type-demo", NULL, NULL);
    if(!window) {
        printf("Create window failed");
        glfwTerminate();
        return -1;
    }
	
	printf("Setting callbacks");
    glfwSetWindowCloseCallback(window, close_callback);
    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetCursorPosCallback(window, cursorpos_callback);
    glfwSetErrorCallback(error_callback);
    printf("Setting context");
    glfwMakeContextCurrent(window);

	char vs_source[] =
        "attribute vec4 position;"
        "attribute vec2 uvs;"
        "uniform mat4 view_matrix;"
        "uniform mat4 projection_matrix;"
        "varying vec2 vuvs;"
        "void main(){"
        "vuvs = uvs;"
        "gl_Position =  projection_matrix * view_matrix * position;"
        "}";

    char fs_source[] =
        "varying vec2 vuvs;"
		"uniform sampler2D texture;"
        "void main() {"
        "vec4 c = texture2D(texture, vuvs);"
        "gl_FragColor = c;"
        "}";
	printf("Compiling shader\n");
    GLuint main_shader = compile_shader_program(vs_source,
    													  fs_source,
    													  "position", "color");


	printf("Setting up camera\n");
    float3 camera_position;
    float3 camera_direction;
    float3 camera_up;

	float view_matrix[] = M_MAT4_IDENTITY();
	float projection_matrix[] = M_MAT4_IDENTITY();
	float model_matrix[] = M_MAT4_IDENTITY();

    float aspect = WINDOW_W / (float)WINDOW_H;
    m_mat4_perspective(projection_matrix, 10.0, aspect, 0.1, 999.0);

	set_float3(&camera_position, 0, 1, 100);
	set_float3(&camera_direction, 0 - camera_position.x ,0 - camera_position.y , 0 - camera_position.z);
	set_float3(&camera_up, 0,1,0);
	
	m_mat4_lookat(view_matrix, &camera_position, &camera_direction, &camera_up);

	printf("Creating mesh\n");
	int vertex_data_size = sizeof(float) * 6 * 5;
	float* vertex_data = (float *) malloc(vertex_data_size);

	float* buf = vertex_data;
    {
		
        buf = push_textured_quad_arr(buf, -10, -10, 10, 10, 0, 0, 1, 1);
		// float scale = 1.0;
        //buf = push_v5_arr(buf, -scale, scale, 0.0, 0.0, 1.0);
		//buf = push_v5_arr(buf, scale, -scale, 0.0, 1.0, 0.0);
		//buf = push_v5_arr(buf, scale, scale, 0.0, 1.0, 1.0);
		//buf = push_v5_arr(buf, -scale, scale, 0.0, 0.0, 1.0);
		//buf = push_v5_arr(buf, -scale, -scale, 0.0, 0.0, 0.0);
		//buf = push_v5_arr(buf, scale, -scale, 0.0, 1.0, 0.0);
    }

    
	glEnable(GL_TEXTURE_2D);	

	printf("Loading font texture from file\n");
	GLuint font_texture_from_file = 0;
	{
		int width, height, channels;
	    stbi_set_flip_vertically_on_load(true);
	    unsigned char *pixels = stbi_load("placeholder_texture.png", &width, &height, &channels, 0);
	    assert(pixels != NULL);

        font_texture_from_file = upload_new_texture(width, height, channels, pixels);

	    stbi_image_free(pixels);

	    printf("Loaded font texture from file: %d %d %d\n", width, height, channels);
	}

    printf("GL_VERSION: %s\n", glGetString(GL_VERSION));

    printf("Entering Render Loop\n");
    GL_ERR;
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		bool display_from_file = true;
        if (display_from_file) {
            glUseProgram(main_shader);
            {
                
                GLint position = glGetAttribLocation(main_shader, "position");
                GLint uvs = glGetAttribLocation(main_shader, "uvs");
                GLint texture = glGetAttribLocation(main_shader, "texture_unit");

                glUniformMatrix4fv(glGetUniformLocation(main_shader, "view_matrix"),1,GL_FALSE,view_matrix);
                glUniformMatrix4fv(glGetUniformLocation(main_shader, "projection_matrix"), 1, GL_FALSE, projection_matrix);
                glUniform1f(texture, 0.0);

                // @NOTE Texture loading from the ttf file gives a flipped texture which will certainly lead to incorrect results. Let's debug from a file written to disk for now.

                //glBindTexture(GL_TEXTURE_2D, font_texture);
                glBindTexture(GL_TEXTURE_2D, font_texture_from_file);
                //glBindTexture(GL_TEXTURE_2D, test_texture);

                int bytes_per_float = sizeof(float);
                int stride = bytes_per_float * (5);

                glEnableVertexAttribArray(position);
                glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, vertex_data);
                glEnableVertexAttribArray(uvs);
                glVertexAttribPointer(uvs, 2, GL_FLOAT, GL_FALSE, stride, vertex_data + 3);

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glUseProgram(0);    
        }


		GL_ERR;
		glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);

	return 0;
}
