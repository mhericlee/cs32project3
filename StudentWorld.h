#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <vector>
#include <string>

class Actor;
class Avatar;
class Wall;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  bool isPlayerMovementBlockedAt(double x, double y);
  int isMarbleMovementBlockedAt(double x, double y);
  void killActorAt(double x, double y);
  bool marbleHandler(double newX, double newY, int value);
  bool isOnSameSquareAsMarble(double x, double y, Actor* queryActor);
  bool isOnSameSquareAsPlayer(double x, double y);
  bool isOnSameSquareAsThiefBot(double x, double y);
  bool checkIfSomethingIsPeaableAndKillableWhereIAm(double x, double y);
  bool checkIfSomethingIsPeaable(double x, double y);
  Actor* isOnSameSquareAsGoodie(double x, double y);
  int getThiefBotsInArea(double factoryX, double factoryY);
  bool checkIfObstructive(double x, double y);
  void DamageAnythingWhereIAm(double x, double y);
  void setCurrentLevelFinished(bool toWhat);
  void decreaseStepsUntilExitOpens();
  int getStepsUntilExitOpens() const;
  void setDisplayText();
  void setAvatarHP(int hp);
  void addAvatarPeas(int peas);
  void createNewPea(double x, double y, int dir);
  void createNewThiefBot(double x, double y, bool producesThingsThatAlsoShoot);
  void setKeyEscape(bool toWhat);

private:
    Avatar* m_avatar;
    std::vector<Actor*> m_actors;
    int m_bonus;
    bool m_isCurrentLevelFinished;
    bool m_pressedEscape;
    int m_stepsUntilExitOpens;
};

#endif // STUDENTWORLD_H_
