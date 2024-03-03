#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Explanations for functions in Actor.cpp.

#include "StudentWorld.h"

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr);
    ~Actor();
    virtual void doSomething();
    StudentWorld* getWorld() const;
    virtual bool blocksPlayerMovement() const;
    virtual bool blocksMarbleMovement() const;
    virtual bool isPeaable() const; // Peaable = can interact with peas
    virtual bool isKillable() const;
    virtual bool isObstructive() const; // is obstructive to bots
    virtual bool push(double newX, double newY, int value);
    virtual void isAttacked();
    virtual bool canFillPit() const; // can fill (and thus remove) the pit
    virtual bool givesBoost() const; // gives a certain boost to a player
    virtual bool canStealThings() const; // can still goodies, more specifically
    void getCoordsWithDirection(double& x, double& y, int dir); // get a new set of coords depending on direction.
    virtual void setIsPickupAble(bool a);
    virtual bool getIsPickupAble() const; // is able to be picked up by a thiefbot or the player.
    void die();
    bool dead() const;
private:
    StudentWorld* m_world;
    bool isDead;
};

class KillableActor : public Actor { // all Killable actors hold peas
public:
    KillableActor(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr, int hp = 0);
    ~KillableActor();
    virtual void isAttacked();
    bool isKillable() const;
    bool isPeaable() const;
    bool isObstructive() const;
    int getHP() const;
    void addHP(int hpToAdd);
    void setHP(int newHp);


private:
    int m_hp;
    virtual void killableActorAttacked() = 0; // auxiliary function for dealing when killable actors are attacked
    virtual void killableActorDead() = 0; // auxiliary function to deal w/ dead killable actors
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
    bool push(double newX, double newY, int value); // function to push (move) the marble itself
    bool canFillPit() const;
private:
    void killableActorAttacked();
    void killableActorDead();
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
    bool thisOneExitIsRevealed; // there's only one exit per game
};

class Goodie : public Actor {
public:
    Goodie(int imageID, double startX, double startY, int dir = 0, StudentWorld* world = nullptr);
    void setIsPickupAble(bool a);
    bool getIsPickupAble() const;
    virtual void doSomething();
    bool givesBoost() const;
private:
    virtual void doDifferentiatedStuff() = 0; // auxiliary function for doSomething() as noted in spec
    bool m_isPickupAble; // can be picked up by a player
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
    ~Robot();
    void doSomething();
    virtual void isAttacked();
    bool shootPea();
    bool peaTrajectoryCheck(double x, double y, int dir);
private:
    virtual void doRobotIsAttackedDifferentiatedStuff() = 0; // another auxiliary function to deal with when robots are attacked, as they have things in common
    virtual void doDifferentiatedRobotStuff() = 0; // another auxiliary function for all robot's doSomething()
    int m_ticks;
    int m_currentTick;
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

class RageBot : public Robot {
public:
    RageBot(double startX, double startY, int dir, StudentWorld* world);
    void doDifferentiatedRobotStuff();
private:
//    bool peaTrajectoryCheck(double x, double y, int dir);
    void doRobotIsAttackedDifferentiatedStuff();
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

class ThiefBot : public Robot {
public:
    ThiefBot(int IDIMAGE, double startX, double startY, StudentWorld *world, int hp);
    ~ThiefBot();
    void setInventory(Actor* a);
    int getDistanceBeforeTurning() const;
    void setDistanceBeforeTurning(int a);
    bool canStealThings() const;
    void releaseGoodie();
    void moveLikeAThiefBot();
private:
    Actor* m_inventory;
    int m_distanceBeforeTurning;
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(double startX, double startY, StudentWorld* world);
private:
    void doDifferentiatedRobotStuff();
    void doRobotIsAttackedDifferentiatedStuff();
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(double startX, double startY, StudentWorld *world);
private:
    void doDifferentiatedRobotStuff();
    void doRobotIsAttackedDifferentiatedStuff();
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

class ThiefBotFactory : public Actor {
public:
    ThiefBotFactory(double startX, double startY, StudentWorld* world, bool shouldILetThemShoot);
    void doSomething();
    bool isObstructive() const;
private:
    bool m_producesThingsThatAlsoShoot;
};

class Avatar : public KillableActor {
public:
    Avatar(double startX, double startY, StudentWorld* world);
    void doSomething();
    int getPeas() const;
    void addPeas(int newPeas);
private:
    int m_peas;
    virtual void killableActorAttacked();
    virtual void killableActorDead();
};

#endif // ACTOR_H_
