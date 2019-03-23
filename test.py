import json

import GLWindow
import numpy as np

import moderngl
import capsule_shape
import mesh_normals
from camera import camera

wnd = GLWindow.create_window()
ctx = moderngl.create_context()

vdata1, idata1 = capsule_shape.capsule_mesh((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4)
vdata2, idata2 = capsule_shape.capsule_lines((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4)
# points = np.frombuffer(vdata, 'f4').reshape(-1, 2, 3)
# index = np.frombuffer(idata, 'i4').reshape(-1, 3)

# points = points.copy()
# points[:, 1] =  np.frombuffer(mesh_normals.calculate_normals(points[:, 0].tobytes(), index, 12), 'f4').reshape(-1, 3)

prog = ctx.program(
    vertex_shader='''
        #version 330
        uniform mat4 Mvp;
        in vec3 in_vert;
        in vec3 in_norm;
        out vec3 v_vert;
        out vec3 v_norm;
        void main() {
            v_vert = in_vert;
            v_norm = in_norm;
            gl_Position = Mvp * vec4(v_vert, 1.0);
        }
    ''',
    fragment_shader='''
        #version 330
        uniform vec4 Color;
        uniform vec3 Light;
        in vec3 v_vert;
        in vec3 v_norm;
        out vec4 f_color;
        void main() {
            float lum = dot(normalize(v_norm), normalize(Light - v_vert));
            lum = lum * 0.8 + 0.2;
            f_color = vec4(Color.rgb * lum, 1.0);
        }
    ''',
)

light = prog['Light']
color = prog['Color']
mvp = prog['Mvp']

vbo1 = ctx.buffer(vdata1)
ibo1 = ctx.buffer(idata1)
vao1 = ctx.simple_vertex_array(prog, vbo1, 'in_vert', 'in_norm', index_buffer=ibo1)

vbo2 = ctx.buffer(vdata2)
ibo2 = ctx.buffer(idata2)
vao2 = ctx.simple_vertex_array(prog, vbo2, 'in_vert', index_buffer=ibo2)

while wnd.update():
    width, height = wnd.size
    ctx.viewport = wnd.viewport
    ctx.clear(1.0, 1.0, 1.0)
    ctx.enable(moderngl.DEPTH_TEST | moderngl.CULL_FACE)
    # ctx.wireframe = True

    light.value = (5.0, 5.0, 5.0)
    color.value = (1.0, 1.0, 1.0, 0.25)
    mvp.write(camera((6.0, 6.0, 6.0), (0.0, 0.0, 1.0)).matrix(ratio=width / height))
    vao1.render()
    vao2.render(moderngl.LINES)
