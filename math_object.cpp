#include "math_object.hpp"

void MathObject::calculate_center() {
    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;

    for (const auto& point : m_points) {
        sum_x += point.x;
        sum_y += point.y;
        sum_z += point.z;
    }

    m_center.x = sum_x / m_points.size();
    m_center.y = sum_y / m_points.size();
    m_center.z = sum_z / m_points.size();
}

void MathObject::load_from_file(const std::string& path, float scale)
{
	m_points.clear();
	m_faces.clear();  // Clear existing face data.

	std::vector<Vertex> vertices;

	std::stringstream ss;
	std::ifstream obj_file(path);
	std::string line;
	std::string prefix;

	if (!obj_file.is_open())
	{
		throw "FATAL: Failed to open file.\n";
	}

	while (std::getline(obj_file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v")
		{
			Point3D tmp_point3d(0.f, 0.f, 0.f);
			ss >> tmp_point3d.x >> tmp_point3d.y >> tmp_point3d.z;
			tmp_point3d.x *= scale;
			tmp_point3d.y *= scale;
			tmp_point3d.z *= scale;
			m_points.push_back(tmp_point3d);
		}
		else if (prefix == "f")
		{
			std::vector<size_t> face_indices;
			size_t tmp_index;
			while (ss >> tmp_index)
			{
				// Adjust to 0-based index by subtracting 1.
				face_indices.push_back(tmp_index - 1);

				// Ignore characters until space or newline is encountered.
				while (ss.peek() != ' ' && ss.peek() != '\n' && ss.peek() != EOF)
					ss.ignore();
			}

			// Ensure the number of face indices is valid.
			if (face_indices.size() >= 3)
			{
				// Add face indices in triplets.
				for (size_t i = 0; i < face_indices.size(); i += 3)
				{
					if (i + 2 < face_indices.size())
					{
						add_faces(face_indices[i], face_indices[i + 1], face_indices[i + 2]);
					}
				}
			}
			else
			{
				std::cerr << "Error: Invalid number of face indices. Each face must have at least 3 vertices.\n";
			}
		}
	}

	obj_file.close();  // Close the file when done reading.
}


MathObject::MathObject(Point3D offset) : m_offset(offset)
{
    m_projection = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };

    m_rotation_x = {
         {1.0f, 0.0f, 0.0f},
         {0.0f, std::cos(m_angle_x), -std::sin(m_angle_x)},
         {0.0f, std::sin(m_angle_x), std::cos(m_angle_x)}
    };

    m_rotation_y = {
         {std::cos(m_angle_y), 0.0f, std::sin(m_angle_y)},
         {0.0f, 1.0f, 0.0f},
         {-std::sin(m_angle_y), 0.0f, std::cos(m_angle_y)}
    };

    m_rotation_z = {
         {std::cos(m_angle_z), -std::sin(m_angle_z), 0.0f},
         {std::sin(m_angle_z), std::cos(m_angle_z), 0.0f},
         {0.0f, 0.0f, 1.0f}
    };

    calculate_center();
}

void MathObject::set_angle_x(float x)
{
    m_angle_x = x;
}

void MathObject::set_angle_y(float y)
{
    m_angle_y = y;
}

void MathObject::set_angle_z(float z)
{
    m_angle_z = z;
}

std::vector<std::vector<float>> float_matrix_multiply(const std::vector<std::vector<float>>& matrix1, const std::vector<std::vector<float>>& matrix2) {
    size_t m1_rows = matrix1.size();
    size_t m1_cols = matrix1[0].size();
    size_t m2_rows = matrix2.size();
    size_t m2_cols = matrix2[0].size();

    if (m1_cols != m2_rows) {
        std::cerr << "Invalid matrices for multiplication." << std::endl;
        return std::vector<std::vector<float>>();
    }

    // Initialize the result matrix with the proper dimensions.
    std::vector<std::vector<float>> result(m1_rows, std::vector<float>(m2_cols, 0.0f));

    for (size_t i = 0; i < m1_rows; ++i) {
        for (size_t j = 0; j < m2_cols; ++j) {
            for (size_t k = 0; k < m1_cols; ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

void MathObject::rotate_around_origin(float angle_x, float angle_y, float angle_z) {
    // Translate the points to the origin, considering the offset.
    for (auto& point : m_points) {
        point.x -= m_offset.x;
        point.y -= m_offset.y;
        point.z -= m_offset.z;
    }

    // Apply rotation to each point.
    for (auto& point : m_points) {
        float new_x = point.x * std::cos(angle_y) - point.z * std::sin(angle_y);
        float new_z = point.x * std::sin(angle_y) + point.z * std::cos(angle_y);
        point.x = new_x;
        point.z = new_z;

        new_x = point.x * std::cos(angle_x) - point.y * std::sin(angle_x);
        float new_y = point.x * std::sin(angle_x) + point.y * std::cos(angle_x);
        point.x = new_x;
        point.y = new_y;

        new_y = point.y * std::cos(angle_z) - point.z * std::sin(angle_z);
        new_z = point.y * std::sin(angle_z) + point.z * std::cos(angle_z);
        point.y = new_y;
        point.z = new_z;
    }

    // Translate the points back to their original position, considering the offset.
    for (auto& point : m_points) {
        point.x += m_offset.x;
        point.y += m_offset.y;
        point.z += m_offset.z;
    }

    calculate_center();
}

void MathObject::set_offset(Point3D offset) {
    m_offset = offset;

    for (auto& point : m_points) {
        point.x += m_offset.x;
        point.y += m_offset.y;
        point.z += m_offset.z;
    }

    calculate_center();
}

std::vector<std::vector<float>> MathObject::calculate_combined_rotation() {
    auto temp_rotation = float_matrix_multiply(m_rotation_x, m_rotation_y);
    auto combined_rotation = float_matrix_multiply(temp_rotation, m_rotation_z);

    return combined_rotation;
}

Point3D MathObject::matrix_multiply(const std::vector<std::vector<float>>& matrix, const Point3D& point) {
    float x = matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z;
    float y = matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z;
    float z = matrix[2][0] * point.x + matrix[2][1] * point.y + matrix[2][2] * point.z;
    return Point3D(x, y, z);
}

Point2D matrix_multiply_2d(const std::vector<std::vector<float>>& matrix, const Point3D& point) {
    float x = matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z;
    float y = matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z;
    return Point2D(x, y);
}

void MathObject::connect_face(const std::vector<size_t>& face_points, const std::vector<std::vector<float>>& rotation_matrix) {
    if (face_points.empty() || face_points.size() == 1)
        return;

    // Connect the points to form a face.
    for (size_t i = 0; i < face_points.size(); ++i) {
        // Ensure that the indices are valid.
        size_t current_index = face_points[i];
        size_t next_index = face_points[(i + 1) % face_points.size()];

        if (current_index >= m_points.size() || next_index >= m_points.size()) {
            std::cerr << "Invalid index in connect_face: " << current_index << " or " << next_index << std::endl;
            return;
        }

        // Apply rotations and projections to the points.
        Point3D p1_rotated = matrix_multiply(rotation_matrix, m_points[current_index]);
        Point3D p2_rotated = matrix_multiply(rotation_matrix, m_points[next_index]);

        Point2D p1_projected = matrix_multiply_2d(m_projection, p1_rotated);
        Point2D p2_projected = matrix_multiply_2d(m_projection, p2_rotated);

        m_lines.append(sf::Vertex(sf::Vector2f(p1_projected.x, p1_projected.y), sf::Color::Green));
        m_lines.append(sf::Vertex(sf::Vector2f(p2_projected.x, p2_projected.y), sf::Color::Green));
    }
}

void MathObject::draw(sf::RenderWindow& window) {
    m_lines.clear();

    auto combined_rotation = calculate_combined_rotation();

    for (const auto& face : m_faces) {
        connect_face(face, combined_rotation);
    }

    window.draw(m_lines);
}
