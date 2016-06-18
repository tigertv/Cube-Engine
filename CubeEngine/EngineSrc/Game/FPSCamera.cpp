#include "FPSCamera.h"
#include <QVector3D>
#include <Qt>
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"
#include "../Scene/SceneMgr.h"
#include "../Interface/Drawable3D.h"
namespace tzw {

const float HeightThreadHold = 0.01;
FPSCamera::FPSCamera()
    :collideCheck(nullptr),m_maxFallSpeed(6),m_distToside(0.3), m_isEnableGravity(true),m_forward(0),m_slide(0)
    ,m_speed(vec3(6,2,6)),m_rotateSpeed(vec3(0.1,0.1,0.1)),m_isFirstLoop(true)
    ,gravity_speed(0),m_gravity(0.3),distToGround(1.5),m_isStopUpdate(false)
{
    m_enableFPSFeature = true;
    setUseCustomFrustumUpdate(true);
}

FPSCamera *FPSCamera::create(Camera *cloneObj)
{
    auto camera =new  FPSCamera();
    camera->setProjection(cloneObj->projection());
    camera->setPos(cloneObj->getPos());
    camera->setRotate(cloneObj->getRotate());
    camera->setScale(cloneObj->getScale());
    EventMgr::shared()->addEventListener(camera);
    return camera;
}

void FPSCamera::onKeyPress(int keyCode)
{
    switch(keyCode)
    {
    case Qt::Key_W:
        m_forward = 1;
        break;
    case Qt::Key_A:
        m_slide = -1;
        break;
    case Qt::Key_S:
        m_forward = -1;
        break;
    case Qt::Key_D:
        m_slide = 1;
        break;
    case Qt::Key_Q:
        m_isStopUpdate = !m_isStopUpdate;
        break;
    case Qt::Key_Space:
        if(m_isEnableGravity && fabs(gravity_speed)<0.000000001)//now falling
        {
            //do nothing
            gravity_speed = 10*Engine::shared()->deltaTime();
        }
        break;
    }
}

void FPSCamera::onKeyRelease(int keyCode)
{
    switch(keyCode)
    {
    case Qt::Key_W:
        m_forward = 0;
        break;
    case Qt::Key_A:
        m_slide = 0;
        break;
    case Qt::Key_S:
        m_forward = 0;
        break;
    case Qt::Key_D:
        m_slide = 0;
        break;
    }
}

void FPSCamera::onMouseRelease(int button, vec2 pos)
{

}

void FPSCamera::onMousePress(int button, vec2 pos)
{
    m_oldPosition = vec3(pos.x,pos.y,0.0);
}

void FPSCamera::onMouseMove(vec2 pos)
{
    if(m_isFirstLoop)
    {
        m_oldPosition = vec3(pos.x,pos.y,0.0);
        m_isFirstLoop = false;
    }
    auto newPosition = vec3(pos.x,pos.y,0.0);

    auto mouseForce = newPosition - m_oldPosition;
    if(abs(mouseForce.x)<2)
    {
        mouseForce.x = 0;
    }
    if(abs(mouseForce.y)<2)
    {
        mouseForce.y = 0;
    }
    vec3 currentRotate = getRotate();
    currentRotate = currentRotate + vec3(-1*mouseForce.y*m_rotateSpeed.x,-1*mouseForce.x*m_rotateSpeed.y,0);
    setRotate(currentRotate);
    m_oldPosition = newPosition;
}

void FPSCamera::update(float dt)
{
    if(!m_enableFPSFeature) return;
    auto m = getTransform().data();
    vec3 forwardDirection,rightDirection;
    if(m_isEnableGravity)
    {
        forwardDirection = vec3(-1*m[8],0,-1*m[10]);
        rightDirection = vec3(m[0],0,m[2]);
    }else
    {
        forwardDirection = vec3(-1*m[8],-1*m[9],-1*m[10]);
        rightDirection = vec3(m[0],m[1],m[2]);
    }
    forwardDirection.normalize();
    rightDirection.normalize();
    auto pos = this->getPos();

    std::vector<Drawable3D *> list;
    AABB aabb;
    aabb.update(vec3(pos.x -3,pos.y- 10,pos.z - 3));
    aabb.update(vec3(pos.x +3,pos.y + 10 ,pos.z + 3));

    SceneMgr::shared()->currentScene()->getRange(&list,aabb);
    Drawable3DGroup group(&list[0],list.size());

    AABB playerAABB;
    vec3 overLap;
    vec3 totalSpeed = forwardDirection*dt*m_speed.z *m_forward;
    totalSpeed += rightDirection*dt*m_speed.x*m_slide;
    totalSpeed += vec3(0,gravity_speed,0);
    vec3 test = pos + totalSpeed;
    playerAABB.update(vec3(test.x -m_distToside,test.y- distToGround,test.z - m_distToside));
    playerAABB.update(vec3(test.x +m_distToside,test.y ,test.z + m_distToside));
    if(!group.hitByAABB(playerAABB,overLap))//if no collid just go through
    {
        pos = test;
    }else //if collid detect, try as best as we can(6 times loop) to let the camera can move more()
    {
        vec3 tmpPos;
        vec3 resultSpeed;
        bool is_x_pass = false,is_y_pass = false, is_z_pass = false;
        float speed_x = totalSpeed.x;
        float speed_y = totalSpeed.y;
        float speed_z = totalSpeed.z;

        //try if we can move the camera along with the x-axis
        for(int i =0;i<6;i++)
        {
            playerAABB.reset();
            tmpPos = pos + vec3(speed_x,0,0);
            playerAABB.update(vec3(tmpPos.x -m_distToside,tmpPos.y- distToGround*0.9,tmpPos.z - m_distToside));
            playerAABB.update(vec3(tmpPos.x +m_distToside,tmpPos.y  ,tmpPos.z + m_distToside));
            if(!group.hitByAABB(playerAABB,overLap))
            {
                is_x_pass = true;
                break;
            }else
            {   //smaller the x speed, try again
                speed_x *=0.8;
            }
        }
        //try if we can move the camera along with the y-axis
        for(int i =0;i<6;i++)
        {
            playerAABB.reset();
            tmpPos = pos + vec3(0,speed_y,0);
            playerAABB.update(vec3(tmpPos.x -m_distToside,tmpPos.y - distToGround*0.9,tmpPos.z - m_distToside));
            playerAABB.update(vec3(tmpPos.x +m_distToside,tmpPos.y ,tmpPos.z + m_distToside));
            if(!group.hitByAABB(playerAABB,overLap))
            {
                is_y_pass = true;
                break;
            }else
            {
                //smaller the y speed, try again
                speed_y *=0.8;
            }
        }

        for(int i =0;i<6;i++)
        {
            playerAABB.reset();
            tmpPos = pos + vec3(0,0,speed_z);
            playerAABB.update(vec3(tmpPos.x -m_distToside,tmpPos.y- distToGround*0.9,tmpPos.z - m_distToside));
            playerAABB.update(vec3(tmpPos.x +m_distToside,tmpPos.y ,tmpPos.z + m_distToside));
            if(!group.hitByAABB(playerAABB,overLap))
            {
                is_z_pass = true;
                break;
            }else
            {
                speed_z *=0.8;
            }
        }
        if(is_x_pass)
        {
            resultSpeed.x = speed_x;
        }
        if(is_y_pass)
        {
            resultSpeed.y = speed_y;
        }
        if(is_z_pass)
        {
            resultSpeed.z = speed_z;
        }

        pos = pos + resultSpeed;
    }

    if(m_isEnableGravity)
    {
        Ray ray(pos,vec3(0,-1,0));
        vec3 hitPoint;
        //qDebug()<<"aaaa"<<gravity_speed;
        auto isHit =group.hitByRay(ray,hitPoint);
        if(isHit)
        {
                if((pos.y > hitPoint.y)&&((pos.y - hitPoint.y) > (distToGround + HeightThreadHold)))
                {
                    if(gravity_speed > -dt *m_maxFallSpeed)
                    {
                        gravity_speed -= dt*m_gravity;
                    }
                    else
                    {
                        gravity_speed = -dt *m_maxFallSpeed;
                    }
                }else if((pos.y - hitPoint.y) <=distToGround)//if stuck in ground, lift the camera a little bit.
                {
                    //pos.y = hitPoint.y + distToGround;
                    gravity_speed = 0.3*dt;
                }else
                {
                    gravity_speed = 0;
                }
        }else
        {
            gravity_speed = -5*dt;
        }
    }
    setPos(pos);
}


vec3 FPSCamera::speed() const
{
    return m_speed;
}

void FPSCamera::setSpeed(const vec3 &speed)
{
    m_speed = speed;
}
vec3 FPSCamera::rotateSpeed() const
{
    return m_rotateSpeed;
}

void FPSCamera::setRotateSpeed(const vec3 &rotateSpeed)
{
    m_rotateSpeed = rotateSpeed;
}

float FPSCamera::getDistToGround() const
{
    return distToGround;
}

void FPSCamera::setDistToGround(float value)
{
    distToGround = value;
}
float FPSCamera::getGravity() const
{
    return m_gravity;
}

void FPSCamera::setGravity(float gravity)
{
    m_gravity = gravity;
}
bool FPSCamera::getIsEnableGravity() const
{
    return m_isEnableGravity;
}

void FPSCamera::setIsEnableGravity(bool isEnableGravity)
{
    m_isEnableGravity = isEnableGravity;
}
float FPSCamera::getDistToside() const
{
    return m_distToside;
}

void FPSCamera::setDistToside(float distToside)
{
    m_distToside = distToside;
}
float FPSCamera::getMaxFallSpeed() const
{
    return m_maxFallSpeed;
}

void FPSCamera::setMaxFallSpeed(float maxFallSpeed)
{
    m_maxFallSpeed = maxFallSpeed;
}
bool FPSCamera::getEnableFPSFeature() const
{
    return m_enableFPSFeature;
}

void FPSCamera::setEnableFPSFeature(bool enableFPSFeature)
{
    m_enableFPSFeature = enableFPSFeature;
}


void FPSCamera::reCache()
{
    Node::reCache();
    if(!m_isStopUpdate)
    {
        Camera::updateFrustum();
    }
}




} // namespace tzw

