#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// LA: Lovely little struct for particle parameters
struct Particle
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    sf::Color colour;
    bool active = false;
};

class ParticleSystem
{
public:
    ParticleSystem(sf::RenderWindow* window);
    
    void update(float dt);
    void render();
    
    // LA: Spawn a burst of particles
    void burst(sf::Vector2f position, sf::Color colour, int count);
private:
    float getRandomRange(float min, float max);
    sf::RenderWindow* _window;
    std::vector<Particle> _particles;
    // LA: Store all the particles vertices in a VertexArray to avoid re-drawing hundreds of particles every frame
    sf::VertexArray _vertices;
    
    // LA: Particle System Constants
    static constexpr int MAX_PARTICLES = 1000;
    static constexpr float PARTICLE_LIFETIME = 1.0f;
    static constexpr float PARTICLE_SIZE = 3.0f;
    static constexpr float PARTICLE_SPEED = 100.0f;
    static constexpr float PARTICLE_GRAVITY = 600.0f;
};
