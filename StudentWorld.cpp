#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include "Level.h"
#include "GraphObject.h"
#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::ostringstream
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_avatar(nullptr), m_bonus(1000), m_stepsUntilExitOpens(0), m_isCurrentLevelFinished(false), m_pressedEscape(false)
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
                case Level::extra_life:
                    m_actors.push_back(new ExtraLifeGoodie(x,y,this));
                    break;
                case Level::restore_health:
                    m_actors.push_back(new RestoreHealthGoodie(x,y,this));
                    break;
                case Level::ammo:
                    m_actors.push_back(new AmmoGoodie(x,y,this));
                    break;
                case Level::horiz_ragebot:
                    m_actors.push_back(new RageBot(x,y, GraphObject::right,this));
//                    m_actors.push_back(new ThiefBotFactory(x,y,this,false));
                    break;
                case Level::vert_ragebot:
                    m_actors.push_back(new RageBot(x,y, GraphObject::down,this));
                    break;
                case Level::thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(x,y,this,false));
                    break;
                case Level::mean_thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(x,y,this,true));
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
    if (m_pressedEscape) {
        decLives();
        m_pressedEscape = false;
        return GWSTATUS_PLAYER_DIED;
    }

    setDisplayText();

    for (auto actor : m_actors) {
        if (actor != nullptr && !actor->dead()) actor->doSomething(); // actor could die during same tick, so we check for it
        if (m_avatar->dead()) { // avatar could die after the actors do something
            delete m_avatar;
            m_avatar = nullptr;
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_isCurrentLevelFinished) { // level is finished; bonus logic is
            m_isCurrentLevelFinished = false; // reset the indicator for the next level.
            m_bonus = 1000; // reset bonus for next level
            increaseScore(m_bonus); // increase score according to bonus
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    m_avatar->doSomething();

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

    if (m_avatar->dead()) { // avatar could die after the actors do something
        delete m_avatar;
        m_avatar = nullptr;
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    if (m_bonus > 0) m_bonus--; // decrease bonus by 1


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

void StudentWorld::setAvatarHP(int hp) {
    m_avatar->setHP(hp);
}

void StudentWorld::addAvatarPeas(int peas) {
    m_avatar->addPeas(peas);
}



void StudentWorld::DamageAnythingWhereIAm(double x, double y) {
    if (m_avatar->getX() == x && m_avatar->getY() == y) { // by design, an avatar is killable by peas
        m_avatar->isAttacked();
    }
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->isKillable()) {
            (*it)->isAttacked();
        }
    }
}

bool StudentWorld::checkIfSomethingIsPeaableAndKillableWhereIAm(double x, double y) {
    bool a = false;
    if (m_avatar->getX() == x && m_avatar->getY() == y) { // by design, an avatar is affected by peas
        m_avatar->isAttacked();
        a = true;
    }
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->isPeaable()) {
            if ((*it)->isKillable()) ((*it)->isAttacked());
            // this way, we can account for when robot AND a factory is on the same square
            // because the loop continues until the end without breaking.
            a = true;
        }
    }
    return a;
}

bool StudentWorld::checkIfSomethingIsPeaable(double x, double y) { // excludes player
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->isPeaable()) {
            return true;
        }
    }
    return false;
}

void StudentWorld::createNewPea(double x, double y, int dir) {
    m_actors.push_back(new Pea(x, y,dir,this));
}

bool StudentWorld::checkIfObstructive(double x, double y) {
    if (m_avatar->getX() == x && m_avatar->getY() == y) { // by design, an avatar is obstructive
        return true;
    }
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->isObstructive()) {
            return true;
        }
    }
    return false;
}

Actor* StudentWorld::isOnSameSquareAsGoodie(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        // but it doesn't give boosts anyway, so it doesn't matter
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->givesBoost()) {
            return *it;
        }
    }
    return nullptr;
}

bool StudentWorld::isOnSameSquareAsThiefBot(double x, double y) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        // but it's not a thiefbot anyway, so it doesn't matter
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->canStealThings()) {
            return true;
        }
    }
    return false;
}

int StudentWorld::getThiefBotsInArea(double factoryX, double factoryY) {
    double startX = std::max(factoryX - 3.0, 0.0);
    double startY = std::max(factoryY - 3.0, 0.0);
    double endX = std::min(factoryX + 3.0, VIEW_WIDTH - 1.0);
    double endY = std::min(factoryY + 3.0, VIEW_HEIGHT - 1.0);
    int cnt = 0;
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) { // avatar is not in this vector
        // but it's not a thiefbot anyway, so it doesn't matter
        double x = (*it)->getX();
        double y = (*it)->getY();
        if (x >= startX && x <= endX && y >= startY && y <= endY && (*it)->canStealThings()) {
            cnt++;
        }
    }
    return cnt;
}

void StudentWorld::createNewThiefBot(double x, double y, bool producesThingsThatAlsoShoot) {
    if (producesThingsThatAlsoShoot) {
        m_actors.push_back(new MeanThiefBot(x,y,this));
    } else {
        m_actors.push_back(new RegularThiefBot(x,y,this));
    }
}

void StudentWorld::setKeyEscape(bool toWhat) {
    m_pressedEscape = toWhat;
}
