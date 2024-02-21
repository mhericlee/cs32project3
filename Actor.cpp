#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor

Actor::Actor(int imageID, double startX, double startY, int dir, StudentWorld* world) : GraphObject(imageID,startX,startY,dir,1.0), m_world(world), isDead(false) {

}

bool Actor::marbleCheck() const {
    return false;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

void Actor::doSomething() {

}

bool Actor::blocksPlayerMovement() const {
    return false;
}

bool Actor::blocksMarbleMovement() const {
    return true;
}

bool Actor::attemptToPush(double newX, double newY, int value) {
    return true;
}

void Actor::isAttacked() {

}

bool Actor::dead() const {
    return isDead;
}

void Actor::die() {
    isDead = true;
    setVisible(false);
}

// Wall

Wall::Wall(double startX, double startY, StudentWorld* world) : Actor(IID_WALL,startX,startY,none,world) {

}

bool Wall::blocksPlayerMovement() const {
    return true;
}

// Marble

Marble::Marble(double startX, double startY, StudentWorld* world) : Actor(IID_MARBLE,startX,startY,none,world), m_hp(20) {

}

void Marble::isAttacked() {
    if (m_hp - 2 <= 0) {
        die();
    } else {
        m_hp -= 2;
    }
}

bool Marble::marbleCheck() const {
    return true;
}

bool Marble::attemptToPush(double newX, double newY, int value) {
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

void Marble::push() {

}

// Pit

Pit::Pit(double startX, double startY, StudentWorld* world) : Actor(IID_PIT,startX,startY,none,world) {

}

bool Pit::blocksMarbleMovement() const {
    return false; // this (pit) is the only actor that doesn't block it
}

void Pit::doSomething() {
    if (dead()) return;
    if (getWorld()->isOnSameSquareAsMarble(getX(),getY(),this)) {
        die(); // marble is killed in isOnSameSquareAsMarble
    }
}


// Avatar


Avatar::Avatar(double startX, double startY, StudentWorld* world) : Actor(IID_PLAYER,startX,startY,right,world), m_hp(20), m_peas(20) {

}

void Avatar::doSomething() {
    int value;
    if (getWorld()->getKey(value)) {
        // Determine target position based on key press
        double newX = getX(), newY = getY();
        switch (value) {
            case KEY_PRESS_LEFT:
                newX--;
                setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                newX++;
                setDirection(right);
                break;
            case KEY_PRESS_UP:
                newY++;
                setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                newY--;
                setDirection(down);
                break;
            default: break;
        }

        // Check if movement is blocked at the new position
        if (!getWorld()->isPlayerMovementBlockedAt(newX, newY)) { // aka is it a wall
            if (getWorld()->marbleHandler(newX,newY,value)) {
                moveTo(newX, newY);
            }
        }
    }
}