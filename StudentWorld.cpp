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
: GameWorld(assetPath), m_avatar(nullptr), m_bonus(1000), m_stepsUntilExitOpens(0)
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
                case Level::crystal:
                    m_actors.push_back(new Crystal(x,y,this));
                    m_stepsUntilExitOpens++;
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(x,y,this));
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

    setDisplayText();

    m_avatar->doSomething();

    for (auto actor : m_actors) {
        if (actor != nullptr && !actor->dead()) actor->doSomething(); // actor could die during same tick, so we check for it
        if (m_avatar->dead()) { // avatar could die after the actors do something
            delete m_avatar;
            m_avatar = nullptr;
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_isCurrentLevelFinished) { // level is finished; bonus logic is
            m_isCurrentLevelFinished = false; // reset the indicator for the next level.
            m_bonus = 1000; // reset bonus for next level
            increaseScore(m_bonus); // increase score according to bonus
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    if (m_avatar->dead()) { // avatar could die after the actors do something
        delete m_avatar;
        m_avatar = nullptr;
        return GWSTATUS_PLAYER_DIED;
    }

    if (m_bonus > 0) m_bonus--; // decrease bonus by 1

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

    if (m_isCurrentLevelFinished) { // level is finished; bonus logic is
        m_isCurrentLevelFinished = false; // reset the indicator for the next level.
        m_bonus = 1000; // reset bonus for next level
        increaseScore(m_bonus); // increase score according to bonus
        return GWSTATUS_FINISHED_LEVEL;
    }
    
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

void StudentWorld::setDisplayText() {
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = m_bonus;
    int livesLeft = getLives();
    int healthPercentage = (int)(((double)m_avatar->getHP() / 20.0) * 100);
    int ammo = m_avatar->getPeas();
    // Next, create a string from your statistics, of the form:
    // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34

    std::ostringstream s;
    s << "Score: " << std::setw(7) << std::setfill('0') << score
      << "  Level: " << std::setw(2) << std::setfill('0') << level
      << "  Lives: " << std::setw(2) << std::setfill(' ') << livesLeft
      << "  Health: " << std::setw(3) << std::setfill(' ') << healthPercentage << '%'
      << "  Ammo: " << std::setw(3) << std::setfill(' ') << ammo
      << "  Bonus: " << std::setw(4) << std::setfill(' ') << bonus;

    // Update display text with new string
    setGameStatText(s.str());
}

void StudentWorld::setCurrentLevelFinished(bool toWhat) {
    m_isCurrentLevelFinished = toWhat;
}

void StudentWorld::decreaseStepsUntilExitOpens() {
    m_stepsUntilExitOpens--;
}

int StudentWorld::getStepsUntilExitOpens() const {
    return m_stepsUntilExitOpens;
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
    return m_avatar->getX() == x && m_avatar->getY() == y;
}



