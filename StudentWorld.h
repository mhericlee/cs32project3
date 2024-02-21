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

private:
    Avatar* m_avatar;
    std::vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
