#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int RADIUS = 80;
const int GRAPH_RADIUS = 10;
const int FPS_LIMIT = 30;
const float PI = 3.14159265;

// global tween function
std::function<float(float, float, float)> tween;

float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }
float easeIn(float a, float b, float t) { return lerp(a, b, t * t); }
float easeOut(float a, float b, float t) { return lerp(a, b, 1 - (1 - t) * (1 - t)); }
float easeInOut(float a, float b, float t) {
    return lerp(a, b, t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2);
}
float easeInCube(float a, float b, float t) { return lerp(a, b, t * t * t); }
float easeOutCube(float a, float b, float t) { return lerp(a, b, 1 - (1 - t) * (1 - t) * (1 - t)); }
float easeIntOutCirc(float a, float b, float t) {
    float y = t < 0.5 ? (1 - sqrt(1 - pow(2 * t, 2))) / 2 : (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
    return lerp(a, b, y);
}
float easeInOutSine(float a, float b, float t) { return lerp(a, b, -(cos(PI * t) - 1) / 2); }
float easeInQuart(float a, float b, float t) { return lerp(a, b, t * t * t * t); }
float easeOutQuart(float a, float b, float t) {
    return lerp(a, b, 1 - (1 - t) * (1 - t) * (1 - t) * (1 - t));
}

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->scancode) {
                case sf::Keyboard::Scan::Num1:
                    tween = lerp;
                    break;
                case sf::Keyboard::Scan::Num2:
                    tween = easeIn;
                    break;
                case sf::Keyboard::Scan::Num3:
                    tween = easeInOut;
                    break;
                case sf::Keyboard::Scan::Num4:
                    tween = easeInCube;
                    break;
                case sf::Keyboard::Scan::Num5:
                    tween = easeOutCube;
                    break;
                case sf::Keyboard::Scan::Num6:
                    tween = easeIntOutCirc;
                    break;
                case sf::Keyboard::Scan::Num7:
                    tween = easeInOutSine;
                    break;
                case sf::Keyboard::Scan::Num8:
                    tween = easeInQuart;
                    break;
                case sf::Keyboard::Scan::Num9:
                    tween = easeOutQuart;
                    break;
            }
        }
    }
}

float timer = 0;
void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);

    auto circle = sf::CircleShape(RADIUS);
    circle.setPosition(sf::Vector2f(tween(50, WINDOW_WIDTH - 50 - RADIUS * 2, timer),
                                    int(WINDOW_HEIGHT / 4) - RADIUS));
    circle.setFillColor(sf::Color::Magenta);
    window.draw(circle);

    sf::VertexArray graphScale(sf::PrimitiveType::LineStrip, 3);

    graphScale[0].position = sf::Vector2f(int(WINDOW_WIDTH / 4), int(WINDOW_HEIGHT * 5 / 8));
    graphScale[1].position = sf::Vector2f(int(WINDOW_WIDTH / 4), int(WINDOW_HEIGHT * 7 / 8));
    graphScale[2].position = sf::Vector2f(int(WINDOW_WIDTH * 3 / 4), int(WINDOW_HEIGHT * 7 / 8));

    window.draw(graphScale);

    sf::VertexArray graph(sf::PrimitiveType::LineStrip, 100);

    for (int i = 0; i < 100; i++) {
        float x = i / 99.0;

        graph[i].position =
            sf::Vector2f(lerp(int(WINDOW_WIDTH / 4), int(WINDOW_WIDTH * 3 / 4), x),
                         tween(int(WINDOW_HEIGHT * 7 / 8), int(WINDOW_HEIGHT * 5 / 8), x));
        graph[i].color = sf::Color::Magenta;
    }

    window.draw(graph);

    auto graphCircle = sf::CircleShape(GRAPH_RADIUS);
    graphCircle.setPosition(sf::Vector2f(
        lerp(int(WINDOW_WIDTH / 4), int(WINDOW_WIDTH * 3 / 4), timer) - GRAPH_RADIUS,
        tween(int(WINDOW_HEIGHT * 7 / 8), int(WINDOW_HEIGHT * 5 / 8), timer) - GRAPH_RADIUS));
    graphCircle.setFillColor(sf::Color::Cyan);
    window.draw(graphCircle);

    timer += 1.0 / FPS_LIMIT;
    while (timer > 1) timer--;

    window.display();
}

int main() {
    sf::RenderWindow window;

    tween = lerp;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
