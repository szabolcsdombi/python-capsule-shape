#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define v_xyz(obj) &obj.x, &obj.y, &obj.z

struct Vertex {
    glm::vec3 vert;
    glm::vec3 norm;
};

template <typename T>
void swap(T & a, T & b) {
    T t = a;
    a = b;
    b = t;
}

PyObject * meth_capsule_mesh(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"a", "b", "r1", "r2", "segs", "rotate", "base_index", NULL};

    glm::vec3 a;
    glm::vec3 b;
    float r1;
    float r2;
    float segs = 4.0f;
    float rotate = 0.0f;
    int base_index = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(fff)(fff)ff|ffi", keywords, v_xyz(a), v_xyz(b), &r1, &r2, &segs, &rotate, &base_index)) {
        return 0;
    }

    if (r1 < r2) {
        swap(r1, r2);
        swap(a, b);
    }

    float lng = glm::distance(a, b);
    float angle1 = atanf((r1 - r2) / lng) + glm::pi<float>() / 2.0f;
    float angle2 = glm::pi<float>() - angle1;

    int res1 = glm::clamp((int)glm::round(angle1 * segs), 4, 64);
    int res2 = glm::clamp((int)glm::round(angle2 * r2 / r1 * segs), 4, 64);
    int res3 = glm::clamp((int)glm::round(angle2 * lng / 2 / r1 * segs), 1, 64);
    int res4 = glm::clamp((int)glm::round(segs * glm::pi<float>() * 2.0f), 8, 64);
    int res5 = res1 + res2 - 2 + res3 - 1;
    int bottom = res5 * res4;
    int top = bottom + 1;

    int num_vertices = res5 * res4 + 2;
    PyObject * vres = PyBytes_FromStringAndSize(NULL, sizeof(Vertex) * num_vertices);
    Vertex * vertices = (Vertex *)PyBytes_AS_STRING(vres);
    Vertex * vptr = vertices;

    for (int k = 0; k < res4; ++k) {
        const float v = rotate + glm::pi<float>() * 2.0f * k / res4;
        const float cv = cosf(v);
        const float sv = sinf(v);

        for (int i = 1; i < res1; ++i) {
            Vertex & vertex = *vptr++;
            const float a = angle1 * i / (res1 - 1);
            vertex.norm = glm::vec3(cv * sinf(a), sv * sinf(a), -cosf(a));
            vertex.vert = vertex.norm * r1;
        }

        for (int i = 1; i < res3; ++i) {
            Vertex & vertex = *vptr++;
            vertex.norm = glm::vec3(cv * sinf(angle1), sv * sinf(angle1), -cosf(angle1));
            vertex.vert = vertex.norm * (r1 + (r2 - r1) * i / res3) + glm::vec3(0.0f, 0.0f, lng * i / res3);
        }

        for (int i = res2 - 1; i > 0; --i) {
            Vertex & vertex = *vptr++;
            const float a = glm::pi<float>() - angle2 * i / (res2 - 1);
            vertex.norm = glm::vec3(cv * sinf(a), sv * sinf(a), -cosf(a));
            vertex.vert = vertex.norm * r2 + glm::vec3(0.0f, 0.0f, lng);
        }
    }

    Vertex & bottom_vertex = *vptr++;
    bottom_vertex.norm = glm::vec3(0.0f, 0.0f, -1.0f);
    bottom_vertex.vert = glm::vec3(0.0f, 0.0f, -r1);

    Vertex & top_vertex = *vptr++;
    top_vertex.norm = glm::vec3(0.0f, 0.0f, 1.0f);
    top_vertex.vert = glm::vec3(0.0f, 0.0f, lng + r2);

    glm::vec3 forward = glm::normalize(b - a);
    glm::vec3 up = (forward.x || forward.y) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 side1 = glm::normalize(glm::cross(up, forward));
    glm::vec3 side2 = glm::normalize(glm::cross(forward, side1));

    for (int i = 0; i < num_vertices; ++i) {
        Vertex & vertex = vertices[i];
        vertex.vert = a + side1 * vertex.vert.x + side2 * vertex.vert.y + forward * vertex.vert.z;
        vertex.norm = side1 * vertex.norm.x + side2 * vertex.norm.y + forward * vertex.norm.z;
    }

    int num_triangles = res4 * res5 * 2;
    PyObject * ires = PyBytes_FromStringAndSize(NULL, sizeof(glm::ivec3) * num_triangles);
    glm::ivec3 * indexes = (glm::ivec3 *)PyBytes_AS_STRING(ires);
    glm::ivec3 * iptr = indexes;

    for (int i = 0; i < res4; ++i) {
        *iptr++ = glm::ivec3(res5 * i, bottom, res5 * ((i + 1) % res4));
    }
    for (int i = 0; i < res4; ++i) {
        for (int j = 0; j < res5 - 1; ++j) {
            *iptr++ = glm::ivec3(res5 * i + j, res5 * ((i + 1) % res4) + j + 1, res5 * i + j + 1);
            *iptr++ = glm::ivec3(res5 * i + j, res5 * ((i + 1) % res4) + j, res5 * ((i + 1) % res4) + j + 1);
        }
    }
    for (int i = 0; i < res4; ++i) {
        *iptr++ = glm::ivec3(top, res5 * i + res5 - 1, res5 * ((i + 1) % res4) + res5 - 1);
    }

    for (int i = 0; i < num_triangles; ++i) {
        indexes[i] += base_index;
    }

    PyObject * res = PyTuple_New(2);
    PyTuple_SET_ITEM(res, 0, vres);
    PyTuple_SET_ITEM(res, 1, ires);
    return res;
}

    float x, y, z;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "fff", keywords, &x, &y, &z)) {
        return 0;
    }

    Py_RETURN_NONE;
}

PyMethodDef module_methods[] = {
    {"capsule_mesh", (PyCFunction)meth_capsule_mesh, METH_VARARGS | METH_KEYWORDS, 0},
    {0},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "capsule_shape", 0, -1, module_methods, 0, 0, 0, 0};

extern "C" PyObject * PyInit_capsule_shape() {
    PyObject * module = PyModule_Create(&module_def);
    return module;
}
