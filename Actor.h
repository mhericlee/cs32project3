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
    virtual bool isPeaable() const; // Peaable = can interact with peas
    virtual bool isKillable() const;
    virtual bool isObstructive() const;
    virtual bool push(double newX, double newY, int value);
    virtual void isAttacked();
    virtual bool canFillPit() const;
    void getCoordsWithDirection(double& x, double& y, int dir);
    void die();
    bool dead() const;
private:
    StudentWorld* m_world;
    bool isDead;
};

class KillableActor : public Actor { // all Killable actors hold peas
public:
    KillableActor(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr, int hp = 0);
    virtual void isAttacked();
    bool isKillable() const;
    bool isPeaable() const;
    bool isObstructive() const;
    int getHP() const;
    void addHP(int hpToAdd);
    void setHP(int newHp);


private:
    int m_hp;
};

class Goodie : public Actor {
public:
    Goodie(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr);
    virtual void doSomething();
private:
    virtual void doDifferentiatedStuff() = 0;
};

class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* world);
    bool isPeaable() const;
    bool isObstructive() const;
    bool blocksPlayerMovement() const;
};

class Marble : public KillableActor {
public:
    Marble(double startX, double startY, StudentWorld* world);
    bool push(double newX, double newY, int value);
    bool canFillPit() const;
};

class Pea : public Actor {
public:
    Pea(double startX, double startY, int dir, StudentWorld* world);
    void doSomething();
};

class Crystal : public Actor {
public:
    Crystal(double startX, double startY, StudentWorld* world);
    void doSomething();

};

class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* world);
    bool blocksPlayerMovement() const;
    bool blocksMarbleMovement() const;
    bool isObstructive() const;
    void doSomething();
};

class Exit : public Actor {
public:
    Exit(double startX, double startY, StudentWorld* world);
    void doSomething();
private:
    bool thisOneExitIsRevealed;
};

class ExtraLifeGoodie: public Goodie {
public:
    ExtraLifeGoodie(double startX, double startY, StudentWorld* world);
private:
    void doDifferentiatedStuff();
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(double startX, double startY, StudentWorld* world);
private:
    void doDifferentiatedStuff();
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(double startX, double startY, StudentWorld* world);
private:
    void doDifferentiatedStuff();
};

class Robot : public KillableActor {
public:
    Robot(int IDIMAGE, double startX, double startY, int dir, StudentWorld* world, int hp);
    int getTicks();
    void doSomething();
    virtual void doDifferentiatedRobotStuff() = 0;
private:
    int m_ticks;
    int m_currentTick;
};

class RageBot : public Robot {
public:
    RageBot(double startX, double startY, int dir, StudentWorld* world);
    void doDifferentiatedRobotStuff();
    void isAttacked();
private:
    bool peaTrajectoryCheck(double x, double y, int dir);
};

class ThiefBotFactory : public Actor {
public:
    bool isObstructive() const;
};

class Avatar : public KillableActor {
public:
    Avatar(double startX, double startY, StudentWorld* world);
    void doSomething();
    int getPeas() const;
    void addPeas(int newPeas);
private:
    int m_peas;
};

#endif // ACTOR_H_
