#include "GameManager.h"
#include "Ball.h"
#include "PowerupManager.h"
#include <algorithm>
#include <iostream>

GameManager::GameManager(sf::RenderWindow* window)
    : _window(window), _paddle(nullptr), _ball(nullptr), _brickManager(nullptr), _powerupManager(nullptr), _particleSystem(nullptr),
    _messagingSystem(nullptr), _ui(nullptr), _pause(false), _time(0.f), _lives(3), _pauseHold(0.f), _levelComplete(false),
    _powerupInEffect({ none,0.f }), _timeLastPowerupSpawned(0.f), _restartKeyPressed(false), _shakeTimer(0.f), 
    _zoomOnBall(false), _wasZoomingOnBall(false) , _currentZoom(1.f), _levelCompleteTimer(0.f)
{
    _font.loadFromFile("font/montS.ttf");
    _masterText.setFont(_font);
    _masterText.setPosition(50, 400);
    _masterText.setCharacterSize(48);
    _masterText.setFillColor(sf::Color::Yellow);
    
    // LA: Load sound effects from file
    if (!_zoomInBuffer.loadFromFile("sounds/zoomIn.wav"))
        std::cout << "Failed to load sounds/zoomIn.wav" << std::endl;
    if (!_zoomOutBuffer.loadFromFile("sounds/zoomOut.wav"))
        std::cout << "Failed to load sounds/zoomOut.wav" << std::endl;
    
    // LA: Be Quiet!
    _zoomInSound.setVolume(50.f);
    _zoomOutSound.setVolume(50.f);
    
    _zoomInSound.setBuffer(_zoomInBuffer);
    _zoomOutSound.setBuffer(_zoomOutBuffer);
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
    
    delete _particleSystem;
    _particleSystem = nullptr;
    
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
    _particleSystem = new ParticleSystem(_window);
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
    // Screen Shake
    _shakeTimer = 0.f;
    // Zoom
    _zoomOnBall = false;
    _wasZoomingOnBall = false;
    _currentZoom = 1.f;
    _levelCompleteTimer = 0.f;
    
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
    
    if (_shakeTimer > 0.f)
        _shakeTimer -= dt;
    
    // LA: Zoom lerp
    float targetZoom = _zoomOnBall ? ZOOM_AMOUNT : 1.f;
    float t = ZOOM_SPEED * dt;
    t = std::min(t, 1.f);
    _currentZoom += (targetZoom - _currentZoom) * t;
    
    // LA: Play sound effects on each kind of zoom
    if (_zoomOnBall && !_wasZoomingOnBall) _zoomInSound.play();
    if (!_zoomOnBall && _wasZoomingOnBall) _zoomOutSound.play();
    _wasZoomingOnBall = _zoomOnBall;
    
    // LA: Update all particles when not paused
    if (!_pause) _particleSystem->update(dt);
    
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
    
    // LA: Start counting down before the level complete state
    if (_levelCompleteTimer > 0.f)
    {
        _levelCompleteTimer -= dt;
        if (_levelCompleteTimer <= 0.f) _levelComplete = true;
    }

    // timer.
    _time += dt;


    if (_time > _timeLastPowerupSpawned + POWERUP_FREQUENCY && rand()%700 == 0) 
    {
        _powerupManager->spawnPowerup();
        _timeLastPowerupSpawned = _time;
    }

    // move paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _paddle->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _paddle->moveLeft(dt);

    // update everything 
    _paddle->update(dt);
    _zoomOnBall = willBallHitLastBrick();
    _ball->update(dt);
    _powerupManager->update(dt);
}

void GameManager::loseLife()
{
    // LA: Can't lose a life if the level is complete
    if (_levelCompleteTimer > 0.f) return;
    
    _lives--;
    _ui->lifeLost(_lives);

    _shakeTimer = SHAKE_DURATION;
}

void GameManager::render()
{
    // LA: Get the view to move it rapidly within a square to create screen shake 
    sf::View camera = _window->getDefaultView();

    float zoomProgress = (1.f - _currentZoom) / (1.f - ZOOM_AMOUNT);
    sf::Vector2f screenCentre = camera.getCenter();
    sf::FloatRect ball = _ball->getBounds();
    sf::Vector2f ballCentre(ball.left + ball.width / 2.f, ball.top + ball.height / 2.f);
    camera.setCenter(screenCentre + (ballCentre - screenCentre) * zoomProgress);
    camera.zoom(_currentZoom);
    
    if (_shakeTimer > 0.f)
    {
        float offsetX = (static_cast<float>(rand()) / RAND_MAX * 2.f - 1.f) * SHAKE_AMOUNT;
        float offsetY = (static_cast<float>(rand()) / RAND_MAX * 2.f - 1.f) * SHAKE_AMOUNT;
        camera.move(offsetX, offsetY);
    }
    _window->setView(camera);
    
    _paddle->render();
    _ball->render();
    _brickManager->render();
    _powerupManager->render();
    _particleSystem->render();
    
    // LA: Screen Space Overlay
    _window->setView(_window->getDefaultView()); 
    _window->draw(_masterText);
    _ui->render();
}

void GameManager::levelComplete()
{
    // LA: Return because the sequence has started or not finished yet
    if (_levelComplete || _levelCompleteTimer > 0.f) return;
    
    // LA: Start zooming out
    _zoomOnBall = false;
    // LA: Start timer before level complete state
    _levelCompleteTimer = LEVEL_COMPLETE_DELAY;
}

bool GameManager::willBallHitLastBrick()
{
    if (_brickManager->getBrickCount() != 1) return false;
    
    // LA: Get the bounds of the ball and brick
    sf::FloatRect brickBounds = _brickManager->getBrickBounds(0);
    sf::FloatRect ballBounds = _ball->getBounds();
    
    // LA: Get the current position and direction of the ball
    sf::Vector2f position(ballBounds.left, ballBounds.top);
    sf::Vector2f direction = _ball->getDirection();
    
    // LA: Get dimensions of the ball, paddle, and window
    float size = ballBounds.width;
    float paddleTop = _paddle->getBounds().top;
    float windowWidth = static_cast<float>(_window->getSize().x);
    
    // LA: Send out a prediction of the ball's path
    for (int step = 0; step < 200; step++)
    {
        position += direction * 2.f;
        
        // LA: Duplicate the bouncing logic of the ball
        if ((position.x >= windowWidth - size && direction.x > 0) || (position.x <= 0 && direction.x < 0))
            direction.x *= -1;
        if (position.y <= 0 && direction.y < 0)
            direction.y *= -1;
        
        // LA: If the prediction ball intersects with the brick, the hit is coming
        if (brickBounds.intersects(sf::FloatRect(position.x, position.y, size, size)))
            return true;
        
        // LA: If the prediction ball is going to the paddle, the hit is not coming
        if (direction.y > 0 && position.y >= paddleTop)
            return false;
    }
    return false;
}

sf::RenderWindow* GameManager::getWindow() const { return _window; }
UI* GameManager::getUI() const { return _ui; }
Paddle* GameManager::getPaddle() const { return _paddle; }
BrickManager* GameManager::getBrickManager() const { return _brickManager; }
ParticleSystem* GameManager::getParticleSystem() const { return _particleSystem; }
PowerupManager* GameManager::getPowerupManager() const { return _powerupManager; }
