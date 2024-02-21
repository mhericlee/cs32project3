#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include "Level.h"
#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::ostringstream
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_avatar(nullptr)
{

}

StudentWorld::~StudentWorld() {
    delete m_avatar;
    m_avatar = nullptr;

    for (auto actor : m_actors) {
        delete actor;
    }
    m_actors.clear();
}

int StudentWorld::init() {
    Level lev(assetPath());

    int levelNumber = getLevel();
    std::string levelFile = "level" + string(levelNumber < 10 ? "0" : "") + std::to_string(levelNumber) + ".txt";

    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found) {
        return GWSTATUS_LEVEL_ERROR;
    } else if (result == Level::load_fail_bad_format) {
        return GWSTATUS_LEVEL_ERROR;
    }

    for (int x = 0; x < VIEW_WIDTH; ++x) {
        for (int y = 0; y < VIEW_HEIGHT; ++y) {
            Level::MazeEntry ge = lev.getContentsOf(x, y);
            switch (ge) {
                case Level::player:
                    m_avatar = new Avatar(x, y,this);
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(x, y,this));
                    break;
                case Level::marble:
                    m_actors.push_back(new Marble(x, y,this));
                    break;
                case Level::pit:
                    m_actors.push_back(new Pit(x, y,this));
                    break;
                default:
                    break;
            }
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_avatar->dead()) {
        delete m_avatar;
        m_avatar = nullptr;
    }
    if (m_avatar != nullptr) {
        m_avatar->doSomething();
    }

    for (auto it = m_actors.begin(); it != m_actors.end(); ) {
        Actor* actor = *it;
        // Check if the actor's position matches the given coordinates
        if (actor->dead()) {
            delete actor; // Delete the actor to free memory
            it = m_actors.erase(it); // Remove actor from vector and get new iterator position
        } else {
            ++it; // Move to the next actor
        }
    }

    for (auto actor : m_actors) {
        if (actor != nullptr) actor->doSomething();
    }

    setGameStatText("Game will end when you type q");
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_avatar;
    m_avatar = nullptr;

    for (auto actor : m_actors) {
        delete actor;
    }
    m_actors.clear();
}

bool StudentWorld::isPlayerMovementBlockedAt(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->blocksPlayerMovement()) {
            if ((*it)->getX() == x && (*it)->getY() == y) return true;
        }
    } return false;
}

int StudentWorld::isMarbleMovementBlockedAt(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->blocksMarbleMovement()) {
            if ((*it)->getX() == x && (*it)->getY() == y) return -1; // there's an actor at that coordinate that blocks it
        }
        else {
            if ((*it)->getX() == x && ((*it)->getY() == y)) return 1; // the actor in that coordinate is a pit (aka it doesn't block it)
        }
    } return 0; // there's no actor in those coordinates at all
}

void StudentWorld::killActorAt(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y) {
            (*it)->die();
        }
    }
}

bool StudentWorld::marbleHandler(double newX, double newY, int value) {
    // this function basically decides whether the PLAYER should move a step ahead or not. this is the ONLY function
    // that deals with that logic. i.e. player actually using moveTo() is SOLELY determined by this.
    // if true = player can move, if false = they can't

    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == newX && (*it)->getY() == newY) {
            if ((*it)->push(newX,newY,value)) {
                return true; // we should move player BC its a pit, theres nothing ahead, or it's not a marble
                // which means the marble (and following logic) was dealt with in attemptToPush()
            } else {
                return false; // smth in the way ahead of the marble
            }
        }
    } return true; // just an empty space (no actors), so obviously you can move
}

bool StudentWorld::isOnSameSquareAsMarble(double x, double y, Actor* queryActor) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && *it != queryActor && (*it)->canFillPit()) {
            (*it)->die(); // kill the marble
            return true;
        }
    } return false;
}

bool StudentWorld::isOnSameSquareAsPlayer(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && *it == m_avatar) {
            return true;
        }
    } return false;
}


