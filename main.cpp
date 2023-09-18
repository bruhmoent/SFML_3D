#include "math_object.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <windows.h>
#include <thread>

sf::RenderWindow window(sf::VideoMode(800, 600), "Wireframe Model Loader");
HWND hWnd = window.getSystemHandle();

void OpenFileDialog(std::wstring& file_path)
{
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };

    ofn.hwndOwner = hWnd;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Wavefront OBJ Files\0*.obj\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
    {
        file_path = ofn.lpstrFile;
    }
}

int main()
{
    sf::Event event;
    std::wstring file_path = L"";
    MathObject* m = nullptr;

    window.setFramerateLimit(60);

    Point3D p = Point3D(0.f, 0.f, 0.f);
    Point3D p2 = Point3D(400, 300, 0.f);

    float angle_x = 0.f;
    float angle_y = 0.f;
    float angle_z = 0.f;

    float rotation_speed = 0.f;
    float deceleration_factor = 0.95f;

    sf::Vector2i last_mouse_pos;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                delete m;
                m = nullptr;
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                last_mouse_pos = sf::Mouse::getPosition(window);
            }
            else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2i current_mouse_pos = sf::Mouse::getPosition(window);
                sf::Vector2i delta_mouse_pos = current_mouse_pos - last_mouse_pos;
                angle_y += static_cast<float>(delta_mouse_pos.x) * rotation_speed;
                angle_x += static_cast<float>(delta_mouse_pos.y) * rotation_speed;
                last_mouse_pos = current_mouse_pos;
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)
            {
                angle_x = 0.f;
                angle_y = 0.f;
                angle_z = 0.f;
                rotation_speed = 0.f;
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
            {
                std::thread dialogThread(OpenFileDialog, std::ref(file_path));
                dialogThread.detach();

                delete m;
                m = new MathObject(p);
                m->set_offset(p2);
                m->calculate_center();
                angle_x = 0.f;
                angle_y = 0.f;
                angle_z = 0.f;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            rotation_speed = 0.0001f;
        }
        else
        {
            rotation_speed *= deceleration_factor;
        }

        window.clear(sf::Color(0, 0, 0));

        if (m != nullptr)
        {
            m->rotate_around_origin(angle_x, angle_y, angle_z);

            if (!file_path.empty())
            {
                m->load_from_file(std::string(file_path.begin(), file_path.end()).c_str(), -35.f);
                m->set_offset(p2);
                m->calculate_center();
                file_path = L"";
                angle_x = 0.f;
                angle_y = 0.f;
                angle_z = 0.f;
            }

            m->draw(window);
        }

        window.display();
    }

    delete m;

    return 0;
}