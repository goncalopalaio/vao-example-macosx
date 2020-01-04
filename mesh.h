typedef struct {
	float* vertices;
	float* uvs;
	int vertices_total;
	int vertices_components;
	int uv_components;
	unsigned int vao_id;
	unsigned int vbo_id[7];
}Mesh;


inline float *mesh_push_v3_arr(float *v, float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    return v + 3;
}

inline float *mesh_push_v2_arr(float *v, float x, float y) {
    v[0] = x;
    v[1] = y;
    return v + 2;
}

inline void mesh_push_textured_quad_arr(float*v, float* t, float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1) {
    // quad 1 2 3 4
    // -->
    // triangle 1 2 3
    // triangle 3 4 1

    v = mesh_push_v3_arr(v, x0, y0, 1.0);
    t = mesh_push_v2_arr(t, s0, t0); // 1
    v = mesh_push_v3_arr(v, x1, y0, 1.0);
    t = mesh_push_v2_arr(t, s1, t0); // 2
    v = mesh_push_v3_arr(v, x1, y1, 1.0);
    t = mesh_push_v2_arr(t, s1, t1); // 3
    v = mesh_push_v3_arr(v, x1, y1, 1.0);
    t = mesh_push_v2_arr(t, s1, t1); // 3
    v = mesh_push_v3_arr(v, x0, y1, 1.0);
    t = mesh_push_v2_arr(t, s0, t1); // 4
    v = mesh_push_v3_arr(v, x0, y0, 1.0);
    t = mesh_push_v2_arr(t, s0, t0); // 1
}

inline void mesh_push_textured_cube_arr(float*v, float* t) {
	v = mesh_push_v3_arr(v, 1, -1, -1); 
	v = mesh_push_v3_arr(v, 1, -1, 1);
	v = mesh_push_v3_arr(v, -1, -1, 1);
	v = mesh_push_v3_arr(v, 1, -1, -1);
	v = mesh_push_v3_arr(v, -1, -1, 1);
	v = mesh_push_v3_arr(v, -1, -1, -1);
	v = mesh_push_v3_arr(v, 1, 1, -1);
	v = mesh_push_v3_arr(v, -1, 1, -1);
	v = mesh_push_v3_arr(v, -1, 1, 1);
	v = mesh_push_v3_arr(v, 1, 1, -1);
	v = mesh_push_v3_arr(v, -1, 1, 1);
	v = mesh_push_v3_arr(v, 1, 1, 1);
	v = mesh_push_v3_arr(v, 1, -1, -1);
	v = mesh_push_v3_arr(v, 1, 1, -1);
	v = mesh_push_v3_arr(v, 1, 1, 1);
	v = mesh_push_v3_arr(v, 1, -1, -1);
	v = mesh_push_v3_arr(v, 1, 1, 1);
	v = mesh_push_v3_arr(v, 1, -1, 1);
	v = mesh_push_v3_arr(v, 1, -1, 1);
	v = mesh_push_v3_arr(v, 1, 1, 1);
	v = mesh_push_v3_arr(v, -1, 1, 1);
	v = mesh_push_v3_arr(v, 1, -1, 1);
	v = mesh_push_v3_arr(v, -1, 1, 1);
	v = mesh_push_v3_arr(v, -1, -1, 1);
	v = mesh_push_v3_arr(v, -1, -1, 1);
	v = mesh_push_v3_arr(v, -1, 1, 1);
	v = mesh_push_v3_arr(v, -1, 1, -1);
	v = mesh_push_v3_arr(v, -1, -1, 1);
	v = mesh_push_v3_arr(v, -1, 1, -1);
	v = mesh_push_v3_arr(v, -1, -1, -1);
	v = mesh_push_v3_arr(v, 1, 1, -1);
	v = mesh_push_v3_arr(v, 1, -1, -1);
	v = mesh_push_v3_arr(v, -1, -1, -1);
	v = mesh_push_v3_arr(v, 1, 1, -1);
	v = mesh_push_v3_arr(v, -1, -1, -1);
	v = mesh_push_v3_arr(v, -1, 1, -1);

	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 1);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 0, 0);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 0, 1);
	t = mesh_push_v2_arr(t, 1, 0);
	t = mesh_push_v2_arr(t, 1, 1);
}

Mesh* mesh_allocate(int vertices_components, int uv_components, int vertices_total) {
	int mesh_vertices_size = sizeof(float) * vertices_total * vertices_components;
    int mesh_uvs_size = sizeof(float) * vertices_total * uv_components;
	
	Mesh* mesh = (Mesh*) malloc(sizeof(Mesh));
	
	mesh->vertices = (float *) malloc(mesh_vertices_size);
    mesh->uvs = (float *) malloc(mesh_uvs_size);
    mesh->vertices_components = vertices_components;
    mesh->uv_components = uv_components;
    mesh->vertices_total = vertices_total;

    return mesh;

}

void mesh_free(Mesh* mesh) {
	free(mesh->vertices);
	free(mesh->uvs);
	free(mesh);
}

Mesh* mesh_create_quad(int width) {
	Mesh* mesh = mesh_allocate(3, 2, 6);

	float*v = mesh->vertices;
	float*t = mesh->uvs;
	mesh_push_textured_quad_arr(v, t, -width, -width, width, width, 0, 0, 1, 1);

	return mesh;
}

Mesh* mesh_create_cube() {
	Mesh* mesh = mesh_allocate(3, 2, 36);	
	
	float*v = mesh->vertices;
	float*t = mesh->uvs;

	mesh_push_textured_cube_arr(v, t);
	return mesh;

}
