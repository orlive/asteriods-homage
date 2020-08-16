#ifndef VECTOR_DEFINE
#define VECTOR_DEFINE

class vector {
  protected:
    float m_x = 0;
    float m_y = 0;

  public:
    vector( float x=0 , float y=0 );
    void stop();
    void setXY( float x, float y );
    void add( float angle,float magnitude );
    float rad();
    float degree();
    float magnitude();
};

#endif
