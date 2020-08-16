#ifndef VECTOR_DEFINE
#define VECTOR_DEFINE

typedef struct {
  float x;
  float y;
} POINT;

class vector {
  protected:
    POINT m_position = { .x = 0.0f , .y = 0.0f };

  public:
    vector( POINT position = { .x = 0.0f , .y = 0.0f } );
    void stop();
    void setPosition( POINT position ); // vorher setXY
    void add( float angle,float magnitude );
    float rad();
    float degree();
    float magnitude();
};

#endif
