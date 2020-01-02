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

// For glGenVertexArrays in OSx
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif


#define WINDOW_W 600
#define WINDOW_H 600


// Rendering based on:
// https://github.com/raysan5/raylib/blob/dc6136e820a5793fb469e78154b57a46ac619139/src/rlgl.h


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
        printf("\nCompilation error in shader %s\n", strInfoLog);
        free(strInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    printf("Success!\n");
    return shader;
}

inline float *push_v3_arr(float *v, float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    printf("push_v3_arr: %f, %f, %f\n",  x, y, z);
    return v + 3;
}

inline float *push_v2_arr(float *v, float x, float y) {
    v[0] = x;
    v[1] = y;
    printf("push_v2_arr: %f, %f\n",  x, y);
    return v + 2;
}

void push_textured_quad_arr(float*v, float* t, float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1) {
    // quad 1 2 3 4
    // -->
    // triangle 1 2 3
    // triangle 3 4 1

    //v = push_v5_arr(v, x0, y0, 0, s0, t0); // 1
    //v = push_v5_arr(v, x1, y0, 0, s1, t0); // 2
    //v = push_v5_arr(v, x1, y1, 0, s1, t1); // 3
    //v = push_v5_arr(v, x0, y1, 0, s0, t1); // 4

    v = push_v3_arr(v, x0, y0, 1.0);
    t = push_v2_arr(t, s0, t0); // 1
    v = push_v3_arr(v, x1, y0, 1.0);
    t = push_v2_arr(t, s1, t0); // 2
    v = push_v3_arr(v, x1, y1, 1.0);
    t = push_v2_arr(t, s1, t1); // 3
    v = push_v3_arr(v, x1, y1, 1.0);
    t = push_v2_arr(t, s1, t1); // 3
    v = push_v3_arr(v, x0, y1, 1.0);
    t = push_v2_arr(t, s0, t1); // 4
    v = push_v3_arr(v, x0, y0, 1.0);
    t = push_v2_arr(t, s0, t0); // 1
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_W, WINDOW_H, "vao-example", NULL, NULL);
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


    /*
    char vs_source_es2[] =
        "attribute vec4 position;"
        "attribute vec2 uvs;"
        "uniform mat4 view_matrix;"
        "uniform mat4 projection_matrix;"
        "varying vec2 vuvs;"
        "void main(){"
        "vuvs = uvs;"
        "gl_Position =  projection_matrix * view_matrix * position;"
        "}";

    char fs_source_es2[] =
        "varying vec2 vuvs;"
        "uniform sampler2D texture;"
        "void main() {"
        "vec4 c = texture2D(texture, vuvs);"
        "gl_FragColor = c;"
        "}";
    */

	char vs_source[] =
        "#version 330 core\n"
        ""
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec2 uvs;\n"
        "uniform mat4 view_matrix;\n"
        "uniform mat4 projection_matrix;\n"
        "out vec2 vuvs;\n"
        "void main(){"
        "vuvs = uvs;\n"
        "gl_Position =  projection_matrix * view_matrix * vec4(position, 1.0);\n"
        "}";

    char fs_source[] =
        "#version 330 core\n"
        ""
        "in vec2 vuvs;\n"
		"uniform sampler2D texture_unit;\n"
        "out vec4 frag_color;"
        "void main() {\n"
        "vec4 c = texture(texture_unit, vuvs);\n"
        "// vec4 c = vec4(1.0,0.0,0.0,1.0);\n"
        "frag_color = c;\n"
        "}";
	printf("Compiling shader\n");
    GLuint main_shader = compile_shader_program(vs_source,
    													  fs_source,
    													  "position", "color");

    GL_ERR;

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
    int mesh_vertices_count = 6;
	int mesh_vertices_size = sizeof(float) * mesh_vertices_count * 3;
    int mesh_texcoords_size = sizeof(float) * mesh_vertices_count * 2;
	float* mesh_vertices = (float *) malloc(mesh_vertices_size);
    float* mesh_texcoords = (float *) malloc(mesh_texcoords_size);

	float* mv = mesh_vertices;
    float* mt = mesh_texcoords;
    {
		
        push_textured_quad_arr(mv, mt, -10, -10, 10, 10, 0, 0, 1, 1);
		// float scale = 1.0;
        //buf = push_v5_arr(buf, -scale, scale, 0.0, 0.0, 1.0);
		//buf = push_v5_arr(buf, scale, -scale, 0.0, 1.0, 0.0);
		//buf = push_v5_arr(buf, scale, scale, 0.0, 1.0, 1.0);
		//buf = push_v5_arr(buf, -scale, scale, 0.0, 0.0, 1.0);
		//buf = push_v5_arr(buf, -scale, -scale, 0.0, 0.0, 0.0);
		//buf = push_v5_arr(buf, scale, -scale, 0.0, 1.0, 0.0);
    }

	// glEnable(GL_TEXTURE_2D); GL_ERR; // Not applicable in gl 3.3 core

	printf("Loading font texture from file\n");
	GLuint placeholder_texture = 0;
	{
		int width, height, channels;
	    stbi_set_flip_vertically_on_load(true);
	    unsigned char *pixels = stbi_load("placeholder_texture.png", &width, &height, &channels, 0);
	    assert(pixels != NULL);

        placeholder_texture = upload_new_texture(width, height, channels, pixels);

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

    GL_ERR;

    // Load mesh

    bool vao_supported = true;

    unsigned int mesh_vaoId = 0;
    unsigned int mesh_vboId[2];
    int drawHint = GL_STATIC_DRAW;

    if (vao_supported) {
        glGenVertexArrays(1, &mesh_vaoId);
        glBindVertexArray(mesh_vaoId);
    }

    // Enable vertex attributes: position (shader-location = 0)
    glGenBuffers(1, &mesh_vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh_vertices_count, mesh_vertices, drawHint);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    glGenBuffers(1, &mesh_vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh_vertices_count, mesh_texcoords, drawHint);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);

    if (vao_supported) {
        if (mesh_vaoId > 0) {
            printf("Mesh uploaded successfully to VRAM (GPU) %i\n", mesh_vaoId);
        } else {
            printf("ERROR: Mesh could not be uploaded to VRAM (GPU)");
        }
    }
    GL_ERR;
    //


    while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		bool display_from_file = true;
        if (display_from_file) {
            glUseProgram(main_shader);
            {

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, placeholder_texture);

                
                GLint position = glGetAttribLocation(main_shader, "position"); GL_ERR;
                GLint uvs = glGetAttribLocation(main_shader, "uvs"); GL_ERR;
                GLint texture = glGetAttribLocation(main_shader, "texture_unit"); GL_ERR;

                glUniformMatrix4fv(glGetUniformLocation(main_shader, "view_matrix"),1,GL_FALSE,view_matrix);
                glUniformMatrix4fv(glGetUniformLocation(main_shader, "projection_matrix"), 1, GL_FALSE, projection_matrix);
                glUniform1f(texture, 0.0);


                int bytes_per_float = sizeof(float);
                int stride = bytes_per_float * (5);

                if(vao_supported) {
                    glBindVertexArray(mesh_vaoId);    
                } else {
                    int material_shader_LOC_VERTEX_POSITION = glGetAttribLocation(main_shader, "vertexPosition");
                    int material_shader_LOC_VERTEX_TEXCOORD01 = glGetAttribLocation(main_shader, "vertexTexCoord");

                     // Bind mesh VBO data: vertex position (shader-location = 0)
                    glBindBuffer(GL_ARRAY_BUFFER, mesh_vboId[0]);
                    glVertexAttribPointer(material_shader_LOC_VERTEX_POSITION, 3, GL_FLOAT, 0, 0, 0);
                    glEnableVertexAttribArray(material_shader_LOC_VERTEX_POSITION);

                    // Bind mesh VBO data: vertex texcoords (shader-location = 1)
                    glBindBuffer(GL_ARRAY_BUFFER, mesh_vboId[1]);
                    glVertexAttribPointer(material_shader_LOC_VERTEX_TEXCOORD01, 2, GL_FLOAT, 0, 0, 0);
                    glEnableVertexAttribArray(material_shader_LOC_VERTEX_TEXCOORD01);
                }

                glDrawArrays(GL_TRIANGLES, 0, mesh_vertices_count);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);

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
