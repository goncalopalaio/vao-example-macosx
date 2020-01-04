// Note: VAO's are supported in gl 3.3 core but only through extensions in GL ES2
#define VAO_SUPPORTED true

#define ATTRIB_POSITION "position"
#define ATTRIB_UVS "uvs"
#define ATTRIB_TEXTURE "texture_unit"

#define UNIFORM_VIEW_MAT "view_matrix"
#define UNIFORM_PROJ_MAT "projection_matrix"

void renderer_prepare(Mesh* mesh) {
    int draw_hint = GL_STATIC_DRAW;

    if (VAO_SUPPORTED) {
        glGenVertexArrays(1, &mesh->vao_id);
        glBindVertexArray(mesh->vao_id);
    }

    // Enable vertex attributes: position (shader-location = 0)

    glGenBuffers(1, &mesh->vbo_id[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->vertices_total, mesh->vertices, draw_hint);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    glGenBuffers(1, &mesh->vbo_id[1]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->vertices_total, mesh->uvs, draw_hint);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);

    if (VAO_SUPPORTED) {
        if (mesh->vao_id > 0) {
            // log("Mesh uploaded successfully to VRAM (GPU) %i\n", mesh->vao_id);
        } else {
            // log("ERROR: Mesh could not be uploaded to VRAM (GPU)");
        }
    }
}

void renderer_draw(GLuint shader, Mesh* mesh, float* view_matrix, float* projection_matrix) {
	
	GLint texture = glGetAttribLocation(shader, "texture_unit");

	glUniformMatrix4fv(glGetUniformLocation(shader, "view_matrix"),1,GL_FALSE,view_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection_matrix"), 1, GL_FALSE, projection_matrix);
	glUniform1f(texture, 0.0);

	
	if(VAO_SUPPORTED) {
		glBindVertexArray(mesh->vao_id);    
	} else {
		GLint position = glGetAttribLocation(shader, "position");
		GLint uvs = glGetAttribLocation(shader, "uvs");

		 // Bind mesh VBO data: vertex position (shader-location = 0)
		int position_components = mesh->vertices_components;

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[0]);
		glVertexAttribPointer(position, position_components, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(position);

		// Bind mesh VBO data: vertex texcoords (shader-location = 1)
		int uv_components = mesh->uv_components;

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[1]);
		glVertexAttribPointer(uvs, uv_components, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(uvs);
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_total);

}
