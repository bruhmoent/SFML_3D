#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <SFML/Window/VideoMode.hpp>
#include <iostream>
#include <fstream>  
#include <sstream>

struct Point3D {
    float x;
    float y;
    float z;
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Point2D {
    float x;
    float y;
    Point2D(float x, float y) : x(x), y(y) {}
};

struct Vertex
{
	Point3D position();
	Point3D normal();
};

#ifndef MATH_OBJECT_HPP
#define MATH_OBJECT_HPP

class MathObject
{
public:
    MathObject(Point3D offset);

    void draw(sf::RenderWindow& window);

    void add_points() {}

    void add_faces() {}

    std::vector<std::vector<float>> calculate_combined_rotation();

    Point3D matrix_multiply(const std::vector<std::vector<float>>& matrix, const Point3D& point);

    void connect_face(const std::vector<size_t>& face_points, const std::vector<std::vector<float>>& rotation_matrix);

    void connect_points(int index1, int index2, const std::vector<std::vector<float>>& rotation_matrix);

    void rotate_around_origin(float angle_x, float angle_y, float angle_z);

    void set_angle_x(float x);

    void set_angle_y(float y);

    void set_angle_z(float z);

    void set_offset(Point3D offset);

    void calculate_center();

	void load_from_file(const std::string& path, float scale);

    template<typename T, typename... Args>
    void add_points(T arg, Args... args)
    {
        m_points.push_back(arg);
        add_points(args...);
    }

    template<typename... Args>
    void add_faces(Args... args)
    {
        static_assert(sizeof...(args) % 3 == 0, "Number of arguments must be a multiple of 3 (x, y, z for each point).");
        std::vector<size_t> face_indices{ args... };
        m_faces.push_back(face_indices);
    }

private:
    std::vector<Point3D> m_points;
    Point3D m_offset = Point3D(0.f, 0.f, 0.f);
    Point3D m_center = Point3D(0.f, 0.f, 0.f);
    sf::VertexArray m_lines{ sf::Lines };

    float m_angle_x = 0.f;
    float m_angle_y = 0.0f;
    float m_angle_z = 0.0f;

    std::vector<std::vector<float>> m_projection;
    std::vector<std::vector<float>> m_rotation_x;
    std::vector<std::vector<float>> m_rotation_y;
    std::vector<std::vector<float>> m_rotation_z;

    std::vector<std::vector<size_t>> m_faces;
};

#endif //!MATH_OBJECT_HPP
