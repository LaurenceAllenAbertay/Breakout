#pragma once
#include <SFML/Graphics.hpp>
// LA: Include Audio Features
#include <SFML/Audio.hpp>
#include "CONSTANTS.h"
#include "Paddle.h"
#include "Ball.h"
#include "BrickManager.h"
#include "PowerupManager.h"
#include "MessagingSystem.h"
#include "UI.h"



class GameManager {
public:
    GameManager(sf::RenderWindow* window);
    // LA: Destructor
    ~GameManager();
    void initialize();
    void update(float dt);
    void loseLife();
    void render();
    void levelComplete();
    void powerupEffect(POWERUPS pu, float t);
    
    // LA: Restart game function
    void resetGame();
    
    Paddle* getPaddle() const;
    BrickManager* getBrickManager() const;
    PowerupManager* getPowerupManager() const;
    sf::RenderWindow* getWindow() const;
    UI* getUI() const;


private:
    // LA: Clean up objects
    void cleanup();
    
    // LA: Function to check whether the final brick is in the trajectory of the ball
    bool willBallHitLastBrick();
    
    bool _pause;
    float _pauseHold;
    float _time;
    float _timeLastPowerupSpawned;
    int _lives;
    bool _levelComplete;
    // LA: Check for restart
    bool _restartKeyPressed;
    // LA: Timer for tracking duration of screen shake
    float _shakeTimer;
    // LA: Zoom in on the ball when about to hit the final brick
    bool _zoomOnBall;
    // LA: Whether the zoom just ended
    bool _wasZoomingOnBall;
    // LA: Current zoom percentage
    float _currentZoom;
    // LA: Timer for how long before showing level complete state after winning
    float _levelCompleteTimer;

    // LA: Zooming sounds
    sf::SoundBuffer _zoomInBuffer;
    sf::SoundBuffer _zoomOutBuffer;
    sf::Sound _zoomInSound;
    sf::Sound _zoomOutSound;
    
    std::pair<POWERUPS, float> _powerupInEffect;

    sf::Font _font;
    sf::Text _masterText;

    sf::RenderWindow* _window;
    Paddle* _paddle;
    Ball* _ball;
    BrickManager* _brickManager;
    PowerupManager* _powerupManager;
    MessagingSystem* _messagingSystem;
    UI* _ui;

    static constexpr float PAUSE_TIME_BUFFER = 0.5f;
    static constexpr float POWERUP_FREQUENCY = 7.5f;    // time between minimum powerup spawn
    
    // LA: Screen Shake
    static constexpr float SHAKE_DURATION = 0.2f; // Seconds
    static constexpr float SHAKE_AMOUNT = 10.0f; // Pixels
    
    // LA: Winning Zoom
    static constexpr float ZOOM_AMOUNT = 0.5f;
    static constexpr float ZOOM_SPEED = 1.5f;
    static constexpr float LEVEL_COMPLETE_DELAY = 2.0f;
};
