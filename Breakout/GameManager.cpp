#include "GameManager.h"
#include "Ball.h"
#include "PowerupManager.h"
#include <iostream>

GameManager::GameManager(sf::RenderWindow* window)
    : _window(window), _paddle(nullptr), _ball(nullptr), _brickManager(nullptr), _powerupManager(nullptr),
    _messagingSystem(nullptr), _ui(nullptr), _pause(false), _time(0.f), _lives(3), _pauseHold(0.f), _levelComplete(false),
    _powerupInEffect({ none,0.f }), _timeLastPowerupSpawned(0.f), _restartKeyPressed(false)
{
    _font.loadFromFile("font/montS.ttf");
    _masterText.setFont(_font);
    _masterText.setPosition(50, 400);
    _masterText.setCharacterSize(48);
    _masterText.setFillColor(sf::Color::Yellow);
}

GameManager::~GameManager()
{
    cleanup();
}

// LA: Clean up objects when the game manager is finished with 
// so that they can be properly deallocated and prevent memory leaks
void GameManager::cleanup()
{
    delete _paddle;
    _paddle = nullptr;
    
    delete _ball;
    _ball = nullptr;
    
    delete _brickManager;
    _brickManager = nullptr;
    
    delete _powerupManager;
    _powerupManager = nullptr;
    
    delete _messagingSystem;
    _messagingSystem = nullptr;
    
    delete _ui;
    _ui = nullptr;
}

void GameManager::initialize()
{
    _paddle = new Paddle(_window);
    _brickManager = new BrickManager(_window, this);
    _messagingSystem = new MessagingSystem(_window);
    _ball = new Ball(_window, 400.0f, this); 
    _powerupManager = new PowerupManager(_window, _paddle, _ball);
    _ui = new UI(_window, _lives, this);

    // Create bricks
    _brickManager->createBricks(5, 10, 80.0f, 30.0f, 5.0f);
}

// LA: Reset the game state to its initial conditions
void GameManager::resetGame()
{
    cleanup();
    
    // LA: Reset all values to their initial conditions
    _lives = INITIAL_LIVES;
    _levelComplete = false;
    _pause = false;
    _pauseHold = 0.f;
    _time = 0.f;
    _timeLastPowerupSpawned = 0.f;
    _powerupInEffect = { none,0.f };
    _masterText.setString("");
    
    initialize();
}

void GameManager::update(float dt)
{
    // LA: Handle restart key press
    bool restartPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
    // LA: This bool is used because if you don't check for when the key
    // was just pressed, the game will reset every frame if held down
    bool restartJustPressed = restartPressed && !_restartKeyPressed;
    _restartKeyPressed = restartPressed;
    
    if (restartJustPressed)
    {
        resetGame();
        return;
    }
    
    _powerupInEffect = _powerupManager->getPowerupInEffect();
    _ui->updatePowerupText(_powerupInEffect);
    _powerupInEffect.second -= dt;
    

    if (_lives <= 0)
    {
        _masterText.setString("Game over.\nPress R to restart.");
        return;
    }
    if (_levelComplete)
    {
        _masterText.setString("Level completed.\nPress R to restart.");
        return;
    }
    // pause and pause handling
    if (_pauseHold > 0.f) _pauseHold -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    {
        if (!_pause && _pauseHold <= 0.f)
        {
            _pause = true;
            _masterText.setString("paused.");
            _pauseHold = PAUSE_TIME_BUFFER;
        }
        if (_pause && _pauseHold <= 0.f)
        {
            _pause = false;
            _masterText.setString("");
            _pauseHold = PAUSE_TIME_BUFFER;
        }
    }
    if (_pause)
    {
        return;
    }

    // timer.
    _time += dt;


    if (_time > _timeLastPowerupSpawned + POWERUP_FREQUENCY && rand()%700 == 0)      // TODO parameterise
    {
        _powerupManager->spawnPowerup();
        _timeLastPowerupSpawned = _time;
    }

    // move paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _paddle->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _paddle->moveLeft(dt);

    // update everything 
    _paddle->update(dt);
    _ball->update(dt);
    _powerupManager->update(dt);
}

void GameManager::loseLife()
{
    _lives--;
    _ui->lifeLost(_lives);

    // TODO screen shake.
}

void GameManager::render()
{
    _paddle->render();
    _ball->render();
    _brickManager->render();
    _powerupManager->render();
    _window->draw(_masterText);
    _ui->render();
}

void GameManager::levelComplete()
{
    _levelComplete = true;
}

sf::RenderWindow* GameManager::getWindow() const { return _window; }
UI* GameManager::getUI() const { return _ui; }
Paddle* GameManager::getPaddle() const { return _paddle; }
BrickManager* GameManager::getBrickManager() const { return _brickManager; }
PowerupManager* GameManager::getPowerupManager() const { return _powerupManager; }
