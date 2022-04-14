#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#pragma once
class Pipeline
{
protected:

    glm::vec3 m_scale;
    glm::vec3 m_worldPos;
    glm::vec3 m_rotateInfo;
	glm::mat4 m_transformation;

	struct
	{
		float Width; float Height;
		float zNear; float zFar;
		float FOV;
	} m_persProj;

public:

    Pipeline()
    {
        m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
        m_rotateInfo = glm::vec3(0.0f, 0.0f, 0.0f);
		m_transformation = glm::mat4(1.0f);
    }


    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
		m_scale.x = ScaleX;
		m_scale.y = ScaleY;
		m_scale.z = ScaleZ;
    }

    void WorldPos(float x, float y, float z)
    {
		m_worldPos.x = x;
		m_worldPos.y = y;
		m_worldPos.z = z;
    }

    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
		m_rotateInfo.x = RotateX;
		m_rotateInfo.y = RotateY;
		m_rotateInfo.z = RotateZ;
    }

	void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar)
	{
		m_persProj.FOV = FOV;
		m_persProj.Width = Width;
		m_persProj.Height = Height;
		m_persProj.zNear = zNear;
		m_persProj.zFar = zFar;
	}

	glm::mat4* GetTrans()
	{
		glm::mat4 ScaleTrans = glm::mat4(1.0f);
		glm::mat4 RotateTrans = glm::mat4(1.0f);
		glm::mat4 TranslationTrans = glm::mat4(1.0f);
		glm::mat4 PersProjTrans = glm::mat4(1.0f);

		InitScaleTransform(ScaleTrans);
		InitRotateTransform(RotateTrans);
		InitTranslationTransform(TranslationTrans);
		InitPerspectiveProj(PersProjTrans);
		
		m_transformation = PersProjTrans * TranslationTrans * RotateTrans * ScaleTrans;
		return &m_transformation;
	};

protected:

    void InitScaleTransform(glm::mat4& m)
    {
        m[0][0] = m_scale.x; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = m_scale.y; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = m_scale.z; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    void InitRotateTransform(glm::mat4& m)
    {
        glm::mat4 m_x, m_y, m_z;

        const float x = glm::radians(m_rotateInfo.x);
        const float y = glm::radians(m_rotateInfo.y);
        const float z = glm::radians(m_rotateInfo.z);

        m_x[0][0] = 1.0f; m_x[0][1] = 0.0f; m_x[0][2] = 0.0f; m_x[0][3] = 0.0f;
        m_x[1][0] = 0.0f; m_x[1][1] = cosf(x); m_x[1][2] = -sinf(x); m_x[1][3] = 0.0f;
        m_x[2][0] = 0.0f; m_x[2][1] = sinf(x); m_x[2][2] = cosf(x); m_x[2][3] = 0.0f;
        m_x[3][0] = 0.0f; m_x[3][1] = 0.0f; m_x[3][2] = 0.0f; m_x[3][3] = 1.0f;

        m_y[0][0] = cosf(y); m_y[0][1] = 0.0f; m_y[0][2] = -sinf(y); m_y[0][3] = 0.0f;
        m_y[1][0] = 0.0f; m_y[1][1] = 1.0f; m_y[1][2] = 0.0f; m_y[1][3] = 0.0f;
        m_y[2][0] = sinf(y); m_y[2][1] = 0.0f; m_y[2][2] = cosf(y); m_y[2][3] = 0.0f;
        m_y[3][0] = 0.0f; m_y[3][1] = 0.0f; m_y[3][2] = 0.0f; m_y[3][3] = 1.0f;

        m_z[0][0] = cosf(z); m_z[0][1] = -sinf(z); m_z[0][2] = 0.0f; m_z[0][3] = 0.0f;
        m_z[1][0] = sinf(z); m_z[1][1] = cosf(z); m_z[1][2] = 0.0f; m_z[1][3] = 0.0f;
        m_z[2][0] = 0.0f; m_z[2][1] = 0.0f; m_z[2][2] = 1.0f; m_z[2][3] = 0.0f;
        m_z[3][0] = 0.0f; m_z[3][1] = 0.0f; m_z[3][2] = 0.0f; m_z[3][3] = 1.0f;

        m = m_z * m_y * m_x;
    }

    void InitTranslationTransform(glm::mat4& m)
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = m_worldPos.x;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = m_worldPos.y;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = m_worldPos.z;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    void InitPerspectiveProj(glm::mat4& m)  
    {
        const float ar = m_persProj.Width / m_persProj.Height;
        const float zNear = m_persProj.zNear;
        const float zFar = m_persProj.zFar;
        const float zRange = zNear - zFar;
        const float tanHalfFOV = tanf(glm::radians(m_persProj.FOV / 2.0f));

        m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;              m[0][2] = 0.0f;                       m[0][3] = 0.0;
        m[1][0] = 0.0f;                     m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;                     m[1][3] = 0.0;
        m[2][0] = 0.0f;                     m[2][1] = 0.0f;              m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f * zFar * zNear / zRange;
        m[3][0] = 0.0f;                     m[3][1] = 0.0f;              m[3][2] = 1.0f;                     m[3][3] = 0.0;
    }

};

