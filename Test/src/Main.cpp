#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_GRAPHICS2D
#include "olcPGEX_Graphics2D.h"

class Application : public olc::PixelGameEngine
{
public:
  Application()
  {
    sAppName = "Test";
  }

private:
  float fShipX = 0.0f;
  float fShipY = 0.0f;

  float fFireRate = 0.1f;
  float fFireRateAcc = 0.0f;

  float fShipSpeed = 100.0f;
  float fBulletSpeed = 200.0f;
  float fScale = 0.5f;

  int nTargetSize = 6;

  olc::Sprite *sprShip;

  struct sBullet
  {
    float px;
    float py;
    float vx;
    float vy;
  };

  std::list<sBullet> listBullets;

public:
  bool OnUserCreate() override
  {
    fShipX = (float)ScreenWidth() / 2;
    fShipY = (float)ScreenHeight() / 2;

    sprShip = new olc::Sprite("assets/spaceship.png");

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    int nMouseX = GetMouseX();
    int nMouseY = GetMouseY();

    float dx = (float)nMouseX - fShipX;
    float dy = (float)nMouseY - fShipY;

    float fAngle = atan2f(-dy, dx);

    //Update Ship

    if (GetKey(olc::Key::A).bHeld) fShipX -= fShipSpeed * fElapsedTime;
    if (GetKey(olc::Key::D).bHeld) fShipX += fShipSpeed * fElapsedTime;
    if (GetKey(olc::Key::W).bHeld) fShipY -= fShipSpeed * fElapsedTime;
    if (GetKey(olc::Key::S).bHeld) fShipY += fShipSpeed * fElapsedTime;

    olc::GFX2D::Transform2D t;

    t.Translate(-(sprShip->width / 2), -(sprShip->height / 2));
    t.Scale(fScale, fScale);
    t.Rotate(fAngle);
    t.Translate(fShipX, fShipY);

    // Update Bullets

    if (fFireRateAcc < fFireRate) fFireRateAcc += fElapsedTime;

    if (GetMouse(0).bHeld)
    {
      if (fFireRateAcc >= fFireRate)
      {
        fFireRateAcc -= fFireRate;
        sBullet b;
        float d = 1.0f / sqrtf(dx * dx + dy * dy);
        b.px = fShipX + cosf(fAngle) * fScale * sprShip->width / 2;
        b.py = fShipY - sinf(fAngle) * fScale * sprShip->width / 2;
        b.vx = dx * d * fBulletSpeed;
        b.vy = dy * d * fBulletSpeed;
        listBullets.push_back(b);
      }
    }

    for (auto &b: listBullets)
    {
      b.px += b.vx * fElapsedTime;
      b.py += b.vy * fElapsedTime;
    }

    listBullets.remove_if([&](const sBullet &b){ return ((b.px < 0) || (b.py < 0) || (b.px > ScreenWidth()) || (b.py > ScreenHeight())); });


    Clear(olc::BLACK);

    //Draw Ship

    SetPixelMode(olc::Pixel::MASK);
    olc::GFX2D::DrawSprite(sprShip, t);
    SetPixelMode(olc::Pixel::NORMAL);

    //Draw Bullets

    for (auto &b: listBullets) FillCircle(b.px, b.py, 1, olc::RED);

    //Draw Target Cross

    DrawCircle(nMouseX, nMouseY, nTargetSize, olc::YELLOW);
    DrawLine(nMouseX, nMouseY - 1.5 * nTargetSize, nMouseX, nMouseY + 1.5f * nTargetSize, olc::YELLOW);
    DrawLine(nMouseX - 1.5 * nTargetSize, nMouseY, nMouseX + 1.5f * nTargetSize, nMouseY, olc::YELLOW);

    return !GetKey(olc::Key::ESCAPE).bPressed;
  }
};


int main()
{
  Application Test;
  if (Test.Construct(640, 360, 2, 2)) Test.Start();

  return 0;
}
