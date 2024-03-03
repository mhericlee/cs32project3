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
  bool marbleHandler(double newX, double newY, int value); // when a player collides with a marble
  bool isOnSameSquareAsSomethingThatCanFillPit(double x, double y, Actor* queryActor); // what objects can fill pits? marbles?
  bool isOnSameSquareAsUser(double x, double y); // the user is probably the avatar
  bool isOnSameSquareAsSomethingThatSteals(double x, double y); // what steals things? thiefbots, probably
  bool checkAndDoSomethingIfItsPeaableAndKillableWhereIAm(double x, double y); // check if smth can interact with peas AND is killable.
  bool checkIfSomethingIsPeaable(double x, double y); // check if smth can interact with peas only.
  Actor* isOnSameSquareAsGoodie(double x, double y);
  int getThiefBotsInArea(double factoryX, double factoryY);
  bool checkIfObstructive(double x, double y); // obstructive to projectiles (PEAS)
  void setCurrentLevelFinished(bool toWhat);
  void decreaseStepsUntilExitOpens();
  int getStepsUntilExitOpens() const;
  void setDisplayText();
  void setAvatarHP(int hp);
  void addAvatarPeas(int peas);
  void createNewPea(double x, double y, int dir);
  void createNewThiefBot(double x, double y, bool producesThingsThatAlsoShoot);
  void setKeyEscape(bool toWhat); // if this is set to true, the user has pressed escape

  // rest are pretty self-explanatory

private:
    Avatar* m_avatar; // separate pointer for avatar
    std::vector<Actor*> m_actors;
    int m_bonus;
    bool m_isCurrentLevelFinished;
    bool m_pressedEscape;
    int m_stepsUntilExitOpens;
};

#endif // STUDENTWORLD_H_
