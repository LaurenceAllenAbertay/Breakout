#pragma once
#include <SFML/Graphics.hpp>
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
};
