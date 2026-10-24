# Breakout

W Kavanagh & N Merchant. Summer 2024 (last maintained, Oct 26)

## Controls

A/D to move the paddle right and left.
P to pause.

## Powerups

big/small paddle (blue)
fast/slow ball (fancy yellow)
fire ball (green)

# Tasklist

You are to spend between three and eight hours improving this simple Breakout implementation in a way to best demonstrate your capabilities as a games programmer. 

## Suggested tasks

* Implement mouse input for pad
* Improved VFX (ball trail, paddle hit response, brick particle destruction)
* Better UI (progress bar rather than timer for countdown).
* Add an actual game loop
* Better ball physics (Box2D)
* Leaderboards
* More ball types (e.g., multiball, sticky ball, brick-tracking ball)
    * Note: smallPaddle is too simple to be impressive.
* Sounds with increasing tone between bounces.
* Implement commentary with calls to an LLM such as Ollama

# Time Details and Change list

* Added R to restart button and prompts in Game Over / Level Complete. All game objects are deleted then recreated and added a destructor so restarts don't leak memory. (20 Mins)

#### Time Total: 20 mins

---

### Change log
* 2025 - Naman made it harder
* 2026 - William made it easier again

---

Curse you, fast ball!