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
        colour.a = static_cast<sf::Uint8>(255 * (particle.lifetime / PARTICLE_LIFETIME));
        
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
        particle.lifetime = PARTICLE_LIFETIME;
        
        spawned++;
    }
}
