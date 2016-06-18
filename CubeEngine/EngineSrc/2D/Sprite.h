#ifndef TZW_SPRITE_H
#define TZW_SPRITE_H
#include "../Base/Node.h"
#include "../Interface/Drawable.h"
#include "../Texture/TextureMgr.h"
#include "../Mesh/Mesh.h"
#include "../Math/vec2.h"
#include "../Math/vec4.h"
#include <string>
namespace tzw {

/**
  @class Sprite
  @brief 精灵——用以描述显示在屏幕空间上的2D图形元素
*/
class Sprite : public Drawable
{
public:
    Sprite();
    static Sprite * create(std::string texturePath);
    static Sprite * createWithColor(vec4 color,vec2 contentSize);
    void initWithTexture(std::string texturePath);
    void initWithTexture(Texture * texture);
    void initWithColor(vec4 color, vec2 contentSize);
    virtual void draw();
    void setRenderRect(vec2 size, vec2 lb = vec2(0,0), vec2 rt= vec2(1,1));
    void setRenderRect(vec4 lt, vec4 v2, vec4 v3, vec4 v4);
    vec2 contentSize() const;
    void setContentSize(const vec2 &contentSize);
    Texture *texture() const;
    void setTexture(Texture *texture);
    void setTexture(std::string texturePath);
    bool isUseTexture() const;
    void setIsUseTexture(bool isUseTexture);
    void setUpTechnique();

protected:
    bool m_isUseTexture;
    Texture * m_texture;
    Mesh * m_mesh;
    vec2 m_contentSize;
};

} // namespace tzw

#endif // TZW_SPRITE_H
