#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(sf::RenderWindow* window) : _window(window)
{
    // LA: .resize() ensures the vector has enough capacity to hold all particles
    _particles.resize(MAX_PARTICLES);
    
    // LA: Each particle is 4 vertices
    _vertices.setPrimitiveType(sf::Quads);
}

float ParticleSystem::getRandomRange(float min, float max)
{
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void ParticleSystem::update(float deltaTime)
{
    for (Particle& particle : _particles)
    {
        if (!particle.active) continue;
        
        particle.lifetime -= deltaTime;
        
        if (particle.lifetime <= 0)
        {
            particle.active = false;
            continue;
        }
        
        particle.velocity.y += PARTICLE_GRAVITY * deltaTime;
        particle.position += particle.velocity * deltaTime;
    }
}

void ParticleSystem::render()
{
    _vertices.clear();
    
    for (Particle& particle : _particles)
    {
        if (!particle.active) continue;
        
        sf::Color colour = particle.colour;
        colour.a = static_cast<sf::Uint8>(255 * (particle.lifetime / particle.maxLifetime));
        
        sf::Vector2f particlePosition = particle.position;
        
        // LA: Add the 4 vertices, in order, to the VertexArray
        _vertices.append(sf::Vertex(particlePosition, colour));
        _vertices.append(sf::Vertex(particlePosition + sf::Vector2f(PARTICLE_SIZE, 0.f), colour));
        _vertices.append(sf::Vertex(particlePosition + sf::Vector2f(PARTICLE_SIZE, PARTICLE_SIZE), colour));
        _vertices.append(sf::Vertex(particlePosition + sf::Vector2f(0.f, PARTICLE_SIZE), colour));
    }
    
    _window->draw(_vertices);
}

void ParticleSystem::burst(sf::Vector2f position, sf::Color colour, int count)
{
    int spawned = 0;
    
    for (Particle& particle : _particles)
    {
        if (spawned >= count) break;
        
        if (particle.active) continue;
        
        particle.position = position;
        particle.velocity = {
            getRandomRange((-PARTICLE_SPEED), (PARTICLE_SPEED)),
            getRandomRange((-PARTICLE_SPEED), (PARTICLE_SPEED))
        };
        particle.colour = colour;
        particle.active = true;
        particle.maxLifetime = PARTICLE_LIFETIME;
        particle.lifetime = PARTICLE_LIFETIME;
        
        spawned++;
    }
}

void ParticleSystem::cascade(int count)
{
    float screenWidth = static_cast<float>(_window->getSize().x);
    
    int spawned = 0;
    
    for (Particle& particle : _particles)
    {
        if (spawned >= count) break;
        if (particle.active) continue;

        particle.position = { getRandomRange(0.f, screenWidth), getRandomRange(-400.f, 0.f) };
        particle.velocity = { getRandomRange(-50.f, 50.f), getRandomRange(0.f, 100.f) };
        particle.colour = sf::Color(rand() % 256, rand() % 256, rand() % 256);
        particle.lifetime = 3.f;
        particle.maxLifetime = 3.f;
        particle.active = true;
        spawned++;
    }
}
