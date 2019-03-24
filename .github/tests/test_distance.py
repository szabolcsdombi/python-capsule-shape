import numpy as np
import pytest

import capsule_shape


def test_distance_of_mesh_vertice():
    vdata = capsule_shape.capsule_mesh((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4)[0]
    points = np.frombuffer(vdata, 'f4').reshape(-1, 2, 3)[:, 0]
    for pt in points:
        np.testing.assert_almost_equal(capsule_shape.distance((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, pt), 0.0, decimal=4)


def test_distance_of_lines_vertice():
    vdata = capsule_shape.capsule_lines((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, pad=0.33)[0]
    points = np.frombuffer(vdata, 'f4').reshape(-1, 3)
    for pt in points:
        np.testing.assert_almost_equal(capsule_shape.distance((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, pt), 0.33, decimal=4)


def test_point_below():
    distance = capsule_shape.distance((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, (0.0, 0.0, -1.7))
    np.testing.assert_almost_equal(distance, 0.5, decimal=4)


def test_point_above():
    distance = capsule_shape.distance((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, (0.0, 0.0, 3.9))
    np.testing.assert_almost_equal(distance, 0.5, decimal=4)


def test_point_middle():
    distance = capsule_shape.distance((0.0, 0.0, 0.0), (0.0, 0.0, 3.0), 1.2, 0.4, (2.0, 0.0, 1.771246))
    np.testing.assert_almost_equal(distance, 1.2, decimal=4)


if __name__ == '__main__':
    pytest.main([__file__])
