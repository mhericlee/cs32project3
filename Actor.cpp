#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor

Actor::Actor(int imageID, double startX, double startY, int dir, StudentWorld* world) : GraphObject(imageID,startX,startY,dir,1.0), m_world(world), isDead(false) {

}

Actor::~Actor() {

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

bool Actor::givesBoost() const {
    return false;
}

void Actor::setIsPickupAble(bool a) {

}
bool Actor::getIsPickupAble() const {
    return false;
}

bool Actor::canStealThings() const {
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
    double dy[4] = {1, -1, 0, 0}; // combinations of these coords e.g. (dx[0] & dx[0]), when added to the original coordinates,
    // determine the new coords based on the specified direction.
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

KillableActor::~KillableActor() {

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
        killableActorDead();
    } else {
        addHP(-2);
        killableActorAttacked();
    }
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
    double farOpenX = newX, farOpenY = newY; // far open will be 2 spaces (NEXT NEXT) ahead of the player
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

void Marble::killableActorAttacked() {

}

void Marble::killableActorDead() {
}

// Pea

Pea::Pea(double startX, double startY, int dir, StudentWorld *world) : Actor(IID_PEA,startX,startY,dir,world) {

}

void Pea::doSomething() {
    if (dead()) return;
    if (getWorld()->checkAndDoSomethingIfItsPeaableAndKillableWhereIAm(getX(),getY())) { // if it's on the pea's current coordinate, it should die (attack is called inside this function)
        die();
    }
    double x = getX(), y = getY();
    getCoordsWithDirection(x,y,getDirection());
    moveTo(x,y); // move pea to next space
    if (getWorld()->checkAndDoSomethingIfItsPeaableAndKillableWhereIAm(getX(),getY())) {
        die();
    } // per the spec, do it again

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
    if (getWorld()->isOnSameSquareAsSomethingThatCanFillPit(getX(),getY(),this)) {
        die(); // marble is killed in isOnSameSquareAsSomethingThatCanFillPit
    }
}

// Crystal

Crystal::Crystal(double startX, double startY, StudentWorld *world) : Actor(IID_CRYSTAL,startX,startY,none,world) {

}

void Crystal::doSomething() {
    if (dead()) return;
    if (getWorld()->isOnSameSquareAsUser(getX(),getY())) { // give bonuses and die
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
    if (!thisOneExitIsRevealed && getWorld()->getStepsUntilExitOpens() == 0) { // if all crystals are collected, reveal the exit
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        thisOneExitIsRevealed = true;
    }
    if (getWorld()->isOnSameSquareAsUser(getX(),getY()) && isVisible()) { // give bonuses, remind world that level is finished
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->setCurrentLevelFinished(true);
    }
}

// Goodie

Goodie::Goodie(int imageID, double startX, double startY, int dir, StudentWorld* world) : Actor(imageID, startX, startY, dir, world), m_isPickupAble(true) {

}

void Goodie::setIsPickupAble(bool a) {
    m_isPickupAble = a;
}

bool Goodie::getIsPickupAble() const {
    return m_isPickupAble;
}

void Goodie::doSomething() {
    if (dead()) return;
    if (!getIsPickupAble()) return; // if it's not able to be picked up (i.e. a robot stole it)
    if (getWorld()->isOnSameSquareAsUser(getX(),getY())) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        doDifferentiatedStuff();
    }
}

bool Goodie::givesBoost() const {
    return true;
}

// Extra Life Goodie

ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_EXTRA_LIFE, startX, startY, none, world) {

}

void ExtraLifeGoodie::doDifferentiatedStuff() {
    getWorld()->increaseScore(1000);
    getWorld()->incLives();
}

// Restore Health Goodie

RestoreHealthGoodie::RestoreHealthGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_RESTORE_HEALTH, startX, startY, none, world) {

}

void RestoreHealthGoodie::doDifferentiatedStuff() {
    getWorld()->increaseScore(500);
    getWorld()->setAvatarHP(20);
}

// Ammo Goodie

AmmoGoodie::AmmoGoodie(double startX, double startY, StudentWorld *world) : Goodie(IID_AMMO, startX, startY, none, world) {

}

void AmmoGoodie::doDifferentiatedStuff() {
    getWorld()->increaseScore(100);
    getWorld()->setAvatarHP(20);
    getWorld()->addAvatarPeas(20);
}

// Robot

Robot::Robot(int IDIMAGE, double startX, double startY, int dir, StudentWorld *world, int hp) : KillableActor(IDIMAGE, startX, startY, dir, world, hp), m_ticks(0), m_currentTick(1) {
    m_ticks = (28 - getWorld()->getLevel()) / 4;
    if (m_ticks < 3) m_ticks = 3;
}

Robot::~Robot() {

}

void Robot::doSomething() {
    if (dead()) return;
    if (m_currentTick < m_ticks) { // if in a "resting" tick, just update the count and move on
        m_currentTick++;
        return;
    }
    if (m_currentTick == m_ticks) { // else, do something
        doDifferentiatedRobotStuff();
        m_currentTick = 1;
    }
}

void Robot::isAttacked() {
    if (getHP() - 2 <= 0) {
        die();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        doRobotIsAttackedDifferentiatedStuff(); // only executes when the robot is dead
    } else {
        addHP(-2); // decrease hp by 2
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}

void Robot::doRobotIsAttackedDifferentiatedStuff() {

}

bool Robot::shootPea() {
    double x = getX(), y = getY();
    getCoordsWithDirection(x,y,getDirection());
    double newX = getX(), newY = getY();
    getCoordsWithDirection(newX,newY,getDirection());
    if (peaTrajectoryCheck(x,y,getDirection())) { // check if pea can be launched per spec requirements
        getWorld()->createNewPea(newX, newY, getDirection());
        getWorld()->playSound(SOUND_ENEMY_FIRE);
        return true;
    } else {
        return false;
    }
}

bool Robot::peaTrajectoryCheck(double x, double y, int dir) {
    if (getWorld()->isOnSameSquareAsUser(x,y)) { // check for player
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

void Robot::killableActorAttacked() {

}

void Robot::killableActorDead() {
}


// RageBot
RageBot::RageBot(double startX, double startY, int dir, StudentWorld *world) : Robot(IID_RAGEBOT, startX, startY, dir, world, 10) {
}

void RageBot::doDifferentiatedRobotStuff() {
    if (!shootPea()) {
        double newX = getX(), newY = getY();
        getCoordsWithDirection(newX,newY,getDirection());
        if (!getWorld()->checkIfObstructive(newX,newY)) { // if it's not obstructive, move
            moveTo(newX,newY);
        } else { // else, change to opposite direction
            if (getDirection() == right) setDirection(left);
            else if (getDirection() == left) setDirection(right);
            else if (getDirection() == up) setDirection(down);
            else if (getDirection() == down) setDirection(up);
        }
    }
}


void RageBot::doRobotIsAttackedDifferentiatedStuff() {
    getWorld()->increaseScore(100);
}

void RageBot::killableActorAttacked() {

}

void RageBot::killableActorDead() {

}

// ThiefBot

ThiefBot::ThiefBot(int IDIMAGE, double startX, double startY, StudentWorld *world, int hp) : Robot(IDIMAGE,startX,startY,right,world,hp), m_distanceBeforeTurning(randInt(1,6)), m_inventory(nullptr) {
}

ThiefBot::~ThiefBot() {
}

void ThiefBot::setInventory(Actor *a) {
    m_inventory = a;
}

void ThiefBot::releaseGoodie() {
    if (m_inventory != nullptr) {
        m_inventory->setVisible(true);
        m_inventory->setIsPickupAble(true);
        m_inventory->moveTo(getX(),getY()); // goodie's position should be updated to where it originally was
        m_inventory = nullptr;
    }
}

void ThiefBot::setDistanceBeforeTurning(int a) {
    m_distanceBeforeTurning = a;
}

int ThiefBot::getDistanceBeforeTurning() const {
    return m_distanceBeforeTurning;
}

bool ThiefBot::canStealThings() const {
    return true;
}

void ThiefBot::moveLikeAThiefBot() {
    Actor* a = getWorld()->isOnSameSquareAsGoodie(getX(),getY());
    double newX = getX(), newY = getY();
    getCoordsWithDirection(newX, newY,getDirection());
    bool encounteredObstruction = getWorld()->checkIfObstructive(newX,newY);
    if (a != nullptr && a->getIsPickupAble() && m_inventory == nullptr) { // it is on the same square as a goodie AND inventory is not full
        if (randInt(1,10) == 1) { // 1 in 10 chance randInt() returns 1
            setInventory(a);
            a->setVisible(false);
            a->setIsPickupAble(false);
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
        }
    } else if (getDistanceBeforeTurning() > 0 && !encounteredObstruction) { // not moved fully & no obstructions
        moveTo(newX,newY);
        setDistanceBeforeTurning(getDistanceBeforeTurning()-1);
    } else { // encountered obstruction or fully moved
        setDistanceBeforeTurning(randInt(1,6));
        int directions[4] = {up,down,right,left};
        int d = randInt(0,3);
        bool consideredDirections[4] = {false, false, false, false}; // true = has been considered
        while (true) {
            // check if all four directions have been considered
            bool allFalse = true; // if this is true, we've considered all
            for (bool consideredDirection : consideredDirections) {
                if (!consideredDirection) allFalse = false; // if we haven't considered it, allFalse shouldn't be true
            }
            if (allFalse) {
                setDirection(directions[d]);
                break;
            }
            double evenNewerX = getX(), evenNewerY = getY(); // new coord to be considered with new direction
            getCoordsWithDirection(evenNewerX,evenNewerY,directions[d]);
            if (getWorld()->checkIfObstructive(evenNewerX,evenNewerY)) { // is obstructed
                consideredDirections[d] = true; // we've considered this one
                d = randInt(0,3); // time to set a new direction
            } else { // not obstructed, we have room to move
                setDirection(directions[d]); // set the new direction
                moveTo(evenNewerX,evenNewerY);
                break;
            }
        }
    }
}

void ThiefBot::killableActorAttacked() {

}

void ThiefBot::killableActorDead() {

}


// RegularThiefBot

RegularThiefBot::RegularThiefBot(double startX, double startY, StudentWorld *world) : ThiefBot(IID_THIEFBOT,startX,startY,world,5) {

}

void RegularThiefBot::doDifferentiatedRobotStuff() {
    moveLikeAThiefBot();
}

void RegularThiefBot::doRobotIsAttackedDifferentiatedStuff() {
    getWorld()->increaseScore(20);
    releaseGoodie();
}

void RegularThiefBot::killableActorAttacked() {

}

void RegularThiefBot::killableActorDead() {

}

// MeanThiefBot

MeanThiefBot::MeanThiefBot(double startX, double startY, StudentWorld *world) : ThiefBot(IID_MEAN_THIEFBOT,startX, startY, world, 8) {

}

void MeanThiefBot::doDifferentiatedRobotStuff() {
    if (dead()) return;
    if (!shootPea()) {
        moveLikeAThiefBot();
    }
}

void MeanThiefBot::doRobotIsAttackedDifferentiatedStuff() {
    getWorld()->increaseScore(10);
    releaseGoodie();
}

void MeanThiefBot::killableActorAttacked() {

}

void MeanThiefBot::killableActorDead() {

}

// ThiefBotFactory

ThiefBotFactory::ThiefBotFactory(double startX, double startY, StudentWorld *world, bool shouldILetThemShoot) : Actor(IID_ROBOT_FACTORY,startX,startY,none,world), m_producesThingsThatAlsoShoot(shouldILetThemShoot) {

}

bool ThiefBotFactory::isObstructive() const {
    return true;
}

void ThiefBotFactory::doSomething() {
    if (dead()) return;
    if (getWorld()->getThiefBotsInArea(getX(),getY()) < 3 && !getWorld()->isOnSameSquareAsSomethingThatSteals(getX(),getY())) {
        if (randInt(1,50) == 1) {
            getWorld()->createNewThiefBot(getX(),getY(),m_producesThingsThatAlsoShoot);
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
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
            case KEY_PRESS_ESCAPE:
                getWorld()->setKeyEscape(true);
                break;
            case KEY_PRESS_SPACE:
                if (getPeas() > 0) { // launch pea
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

void Avatar::killableActorAttacked() {
    getWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Avatar::killableActorDead() {
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

// player gets damaged, ragebot gets damaged, thiefbot gets killed