#ifndef LIBNTR_SIM_TOUCH_CURSOR_HPP
#define LIBNTR_SIM_TOUCH_CURSOR_HPP

namespace SIM {

class TouchCursor {
 public:
  TouchCursor();
  ~TouchCursor();

  inline int GetX() { return mX; }
  inline int GetY() { return mY; }

  void DeltaX(int x);
  void DeltaY(int y);
 
  void Init();
  void Draw();

  unsigned int mVertexArray;
  unsigned int mVertexBuffer;
  unsigned int mShaderProgramId;
  int mVertexShader;
  int mFragmentShader;

  int mX = 0;
  int mY = 0;

  struct Vertex {
    float coords[3];
    float texCoords[2];
  };
};

TouchCursor& GetTouchCursor();

}

#endif