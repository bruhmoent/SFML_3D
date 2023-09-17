#include "math_object.hpp"
#include <random>
#include <chrono>

float generate_random_float(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "3D Rotation Matrix");
	sf::Event event;
	Point3D p = Point3D(0.f, 0.f, 0.f);
	MathObject* m = new MathObject(p);

    m->add_points(
        Point3D(50.0f * 0, 50.0f * 0, 50.0f * 0),
        Point3D(50.0f * 1, 50.0f * 0, 50.0f * 0),
        Point3D(50.0f * 1, 50.0f * 1, 50.0f * 0),
        Point3D(50.0f * 0, 50.0f * 1, 50.0f * 0),
        Point3D(50.0f * 0.5, 50.0f * 0.5, 50.0f * 1.6)
    );

    size_t point_indices[] = {
           4 - 1, 1 - 1, 2 - 1,
           3 - 1, 4 - 1, 2 - 1,
           5 - 1, 2 - 1, 1 - 1,
           4 - 1, 5 - 1, 1 - 1,
           3 - 1, 5 - 1, 4 - 1,
           5 - 1, 3 - 1, 2 - 1
    };

    m->add_faces(
        point_indices[0], point_indices[1], point_indices[2],
        point_indices[3], point_indices[4], point_indices[5],
        point_indices[6], point_indices[7], point_indices[8],
        point_indices[9], point_indices[10], point_indices[11],
        point_indices[12], point_indices[13], point_indices[14],
        point_indices[15], point_indices[16], point_indices[17]
    );

	Point3D p2 = Point3D(100, 200, 0.f);
	m->set_offset(p2);
	m->calculate_center();

    float target_angle_x = 0.f;
    float target_angle_y = 0.f;
    float target_angle_z = 0.f;

    float rotation_speed = 0.00001f;
    float rotation_reset_threshold = .0001f;

    while (window.isOpen()) {
        window.clear(sf::Color(0, 0, 0));
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (std::abs(target_angle_x) > rotation_reset_threshold ||
            std::abs(target_angle_y) > rotation_reset_threshold ||
            std::abs(target_angle_z) > rotation_reset_threshold) {
            target_angle_x = 0.f;
            target_angle_y = 0.f;
            target_angle_z = 0.f;
        }

        m->rotate_around_origin(target_angle_x, target_angle_y, target_angle_z);

        target_angle_x += rotation_speed;
        target_angle_y += rotation_speed;
        target_angle_z += rotation_speed;

        m->draw(window);
        window.display();
    }

    return 0;
}