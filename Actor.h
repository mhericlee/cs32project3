#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include "StudentWorld.h"

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr);
    virtual void doSomething();
    StudentWorld* getWorld() const;
    virtual bool blocksPlayerMovement() const;
    virtual bool blocksMarbleMovement() const;
    virtual bool attemptToPush(double newX, double newY, int value);
    virtual void isAttacked();
    virtual bool marbleCheck() const;
    void die();
    bool dead() const;
private:
    StudentWorld* m_world;
    bool isDead;
};

class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* world);
    bool blocksPlayerMovement() const;
};

class Marble : public Actor {
public:
    Marble(double startX, double startY, StudentWorld* world);
    void isAttacked();
    bool attemptToPush(double newX, double newY, int value);
    void push();
    bool marbleCheck() const;
private:
    int m_hp;
};

class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* world);
    bool blocksMarbleMovement() const;
    void doSomething();
};

class Avatar : public Actor {
public:
    Avatar(double startX, double startY, StudentWorld* world);
    void doSomething();

private:
    int m_peas;
    int m_hp;
};

#endif // ACTOR_H_
