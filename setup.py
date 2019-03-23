from setuptools import Extension, setup

ext = Extension(
    name='capsule_shape',
    sources=['capsule_shape.cpp'],
)

setup(
    name='capsule_shape',
    version='0.1.0',
    ext_modules=[ext],
)
