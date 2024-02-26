#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor

Actor::Actor(int imageID, double startX, double startY, int dir, StudentWorld* world) : GraphObject(imageID,startX,startY,dir,1.0), m_world(world), isDead(false) {

}

bool Actor::canFillPit() const {
    return false;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

bool Actor::blocksPlayerMovement() const {
    return false;
}

bool Actor::blocksMarbleMovement() const {
    return true;
}

bool Actor::push(double newX, double newY, int value) {
    return true;
}

void Actor::isAttacked() {

}

bool Actor::isKillable() const {
    return false;
}

bool Actor::isPeaable() const {
    return false;
}

bool Actor::isObstructive() const {
    return false;
}

bool Actor::dead() const {
    return isDead;
}

void Actor::die() {
    isDead = true;
    setVisible(false);
}

void Actor::doSomething() {

}

void Actor::getCoordsWithDirection(double& x, double& y, int dir) {
    double dx[4] = {0, 0, 1, -1};
    double dy[4] = {1, -1, 0, 0};
    if (dir == right) {
        x += dx[2];
        y += dy[2];
    } else if (dir == left) {
        x += dx[3];
        y += dy[3];
    } else if (dir == up) {
        x += dx[0];
        y += dy[0];
    }
    else if (dir == down) {
        x += dx[1];
        y += dy[1];
    }
}

// KillableActor

KillableActor::KillableActor(int imageID, double startX, double startY, int dir, StudentWorld *world, int hp) : Actor(imageID, startX, startY, dir, world), m_hp(hp) {

}

bool KillableActor::isKillable() const {
    return true;
}

bool KillableActor::isPeaable() const {
    return true;
}

bool KillableActor::isObstructive() const {
    return true;
}

int KillableActor::getHP() const {
    return m_hp;
}

void KillableActor::setHP(int newHp) {
    m_hp = newHp;
}

void KillableActor::addHP(int hpToAdd) {
    m_hp += hpToAdd;
}

void KillableActor::isAttacked() {
    if (getHP() - 2 <= 0) {
        die();
    } else {
        addHP(-2);
    }
}


// Goodie

Goodie::Goodie(int imageID, double startX, double startY, int dir, StudentWorld* world) : Actor(imageID, startX, startY, dir, world) {

}

void Goodie::doSomething() {
    if (dead()) return;
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY())) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
    doDifferentiatedStuff();
}

// Wall

Wall::Wall(double startX, double startY, StudentWorld* world) : Actor(IID_WALL,startX,startY,none,world) {

}

bool Wall::isObstructive() const {
    return true;
}

bool Wall::blocksPlayerMovement() const {
    return true;
}

bool Wall::isPeaable() const {
    return true;
}

// Marble

Marble::Marble(double startX, double startY, StudentWorld* world) : KillableActor(IID_MARBLE,startX,startY,none,world, 10) {

}

bool Marble::canFillPit() const {
    return true;
}

bool Marble::push(double newX, double newY, int value) {
    double farOpenX = newX, farOpenY = newY;
    switch (value) { // calculate the NEXT NEXT position
        case KEY_PRESS_LEFT: farOpenX--; break;
        case KEY_PRESS_RIGHT: farOpenX++; break;
        case KEY_PRESS_UP: farOpenY++; break;
        case KEY_PRESS_DOWN: farOpenY--; break;
        default: break;
    }
    int isBlockedAt = getWorld()->isMarbleMovementBlockedAt(farOpenX,farOpenY);
    if (isBlockedAt == -1) {
        // there's smth in the way we can't do anything (nothing moves/pushes)
        return false;
    } else if (isBlockedAt == 1) {
        // it's a pit
        // move marble on top of pit
        moveTo(farOpenX,farOpenY);
        return true;
    } else {
        // there's nothing there
        // push marble (move it)
        moveTo(farOpenX,farOpenY);
        return true;
    }
}

// Pea

Pea::Pea(double startX, double startY, int dir, StudentWorld *world) : Actor(IID_PEA,startX,startY,dir,world) {

}

void Pea::doSomething() {
    if (dead()) return;
    if (getWorld()->checkIfSomethingIsPeaableAndKillableWhereIAm(getX(),getY())) {
        die();
    }
//    std::cerr << "Stage 1: " << getX() << " " << getY() << std::endl;
    double x = getX(), y = getY();
    getCoordsWithDirection(x,y,getDirection());
    moveTo(x,y);
//    std::cerr << "Stage 3: " << getX() << " " << getY() << std::endl;
    if (getWorld()->checkIfSomethingIsPeaableAndKillableWhereIAm(getX(),getY())) {
        die();
    }
//    std::cerr << "Stage 4: " << getX() << " " << getY() << std::endl;

}


// Pit

Pit::Pit(double startX, double startY, StudentWorld* world) : Actor(IID_PIT,startX,startY,none,world) {

}

bool Pit::blocksMarbleMovement() const {
    return false; // this (pit) is the only actor that doesn't block it
}

bool Pit::blocksPlayerMovement() const {
    return true;
}

bool Pit::isObstructive() const {
    return true;
}

void Pit::doSomething() {
    if (dead()) return;
    if (getWorld()->isOnSameSquareAsMarble(getX(),getY(),this)) {
        die(); // marble is killed in isOnSameSquareAsMarble
    }
}

// Crystal

Crystal::Crystal(double startX, double startY, StudentWorld *world) : Actor(IID_CRYSTAL,startX,startY,none,world) {

}

void Crystal::doSomething() {
    if (dead()) return;
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY())) {
        getWorld()->increaseScore(50);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->decreaseStepsUntilExitOpens();
        die();
    }
}

// Exit

Exit::Exit(double startX, double startY, StudentWorld *world) : Actor(IID_EXIT,startX,startY,none,world), thisOneExitIsRevealed(false) {
    setVisible(false);
}

void Exit::doSomething() {
    if (!thisOneExitIsRevealed && getWorld()->getStepsUntilExitOpens() == 0) {
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        thisOneExitIsRevealed = true;
    }
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY()) && isVisible()) {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->setCurrentLevelFinished(true);
    }
}

// Extra Life Goodie

ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_EXTRA_LIFE, startX, startY, none, world) {

}

void ExtraLifeGoodie::doDifferentiatedStuff() {
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY())) {
        getWorld()->increaseScore(1000);
        getWorld()->incLives();
    }
}

// Restore Health Goodie

RestoreHealthGoodie::RestoreHealthGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_RESTORE_HEALTH, startX, startY, none, world) {

}

void RestoreHealthGoodie::doDifferentiatedStuff() {
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY())) {
        getWorld()->increaseScore(500);
        getWorld()->setAvatarHP(20);
    }
}

// Ammo Goodie

AmmoGoodie::AmmoGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_AMMO, startX, startY, none, world) {

}

void AmmoGoodie::doDifferentiatedStuff() {
    if (getWorld()->isOnSameSquareAsPlayer(getX(),getY())) {
        getWorld()->increaseScore(100);
        getWorld()->setAvatarHP(20);
        getWorld()->addAvatarPeas(20);
    }
}

// Robot

Robot::Robot(int IDIMAGE, double startX, double startY, int dir, StudentWorld *world, int hp) : KillableActor(IDIMAGE, startX, startY, dir, world, hp), m_ticks(0), m_currentTick(1) {
    m_ticks = (28 - getWorld()->getLevel()) / 4;
    if (m_ticks < 3) m_ticks = 3;
}

void Robot::doSomething() {
    if (dead()) return;
    if (m_currentTick < m_ticks) {
        m_currentTick++;
        return;
    }
    if (m_currentTick == m_ticks) {
        doDifferentiatedRobotStuff();
        m_currentTick = 1;
    }
}


// RageBot
RageBot::RageBot(double startX, double startY, int dir, StudentWorld *world) : Robot(IID_RAGEBOT, startX, startY, dir, world, 10) {
}

void RageBot::doDifferentiatedRobotStuff() {
    double x = getX(), y = getY();
    getCoordsWithDirection(x,y,getDirection());
    double newX = getX(), newY = getY();
    getCoordsWithDirection(newX,newY,getDirection());
    if (peaTrajectoryCheck(x,y,getDirection())) {
        getWorld()->createNewPea(newX, newY, getDirection());
        getWorld()->playSound(SOUND_ENEMY_FIRE);
    } else {
        if (!getWorld()->checkIfObstructive(newX,newY)) {
            moveTo(newX,newY);
        } else {
            if (getDirection() == right) setDirection(left);
            else if (getDirection() == left) setDirection(right);
            else if (getDirection() == up) setDirection(down);
            else if (getDirection() == down) setDirection(up);
        }
    }
}

bool RageBot::peaTrajectoryCheck(double x, double y, int dir) {
    if (getWorld()->isOnSameSquareAsPlayer(x,y)) { // check for player
        return true;
    }
    if (getWorld()->checkIfSomethingIsPeaable(x,y)) { // this code will never be reached if it's a player
        return false; // if it's an obstacle, abandon the entire operation and return false
    }
    getCoordsWithDirection(x,y,dir); // update with new coords
    if (peaTrajectoryCheck(x,y,getDirection())) {
        return true;
    }
    return false;
}

void RageBot::isAttacked() {
    if (getHP() - 2 <= 0) {
        die();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100);
    } else {
        addHP(-2);
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}


// ThiefBotFactory

bool ThiefBotFactory::isObstructive() const {
    return true;
}

// Avatar

Avatar::Avatar(double startX, double startY, StudentWorld* world) : KillableActor(IID_PLAYER,startX,startY,right,world,20), m_peas(20) {

}

void Avatar::doSomething() {
    if (dead()) return;

    // ESCAPE KEY LOGIC HERE


    int value;
//    std::cerr << "XY:" << getX() << " " << getY() << std::endl;
    if (getWorld()->getKey(value)) {
        // Determine target position based on key press
        double newX = getX(), newY = getY();
        bool newMovement = false;
        switch (value) {
            case KEY_PRESS_SPACE:
                if (getPeas() > 0) {
                    getCoordsWithDirection(newX, newY, getDirection());
                    getWorld()->createNewPea(newX,newY,getDirection());
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    addPeas(-1);
                }
                break;
            case KEY_PRESS_LEFT:
                newX--;
                setDirection(left);
                newMovement = true;
                break;
            case KEY_PRESS_RIGHT:
                newX++;
                setDirection(right);
                newMovement = true;
                break;
            case KEY_PRESS_UP:
                newY++;
                setDirection(up);
                newMovement = true;
                break;
            case KEY_PRESS_DOWN:
                newY--;
                setDirection(down);
                newMovement = true;
                break;
            default: break;
        }

        // Check if movement is blocked at the new position
        if (newMovement && !getWorld()->isPlayerMovementBlockedAt(newX, newY)) { // aka is it a wall
            if (getWorld()->marbleHandler(newX,newY,value)) {
                moveTo(newX, newY);
            }
        }
    }
}

int Avatar::getPeas() const {
    return m_peas;
}

void Avatar::addPeas(int newPeas) {
    m_peas += newPeas;
}