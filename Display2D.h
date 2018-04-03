#ifndef DISPLAY2D_H_INCLUDED
#define DISPLAY2D_H_INCLUDED

class Display2D
{
    public:

    Display2D();
    ~Display2D();
    void init();
    void crosshair();

    private:

    const float m_width;
    const float m_height;
    GLuint m_textureTarget;
};

#endif // DISPLAY2D_H_INCLUDED
