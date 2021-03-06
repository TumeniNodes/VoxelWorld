/*================================================================
Filename: ImmediateScreen2D.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>
#include <string>

#include <OWEShader.hpp>

#include <Utility/Math.h>
#include <Utility/Uncopiable.h>

#include <D3DObject/BasicBuffer.h>
#include <D3DObject/DepthStencilState.h>
#include <D3DObject/InputLayout.h>
#include <D3DObject/Sampler.h>
#include <Texture/Texture2D.h>

class ImmediateScreen2D : public Uncopiable
{
public:
    ImmediateScreen2D(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);
    bool IsAvailable(void) const;

    void DrawRectangle(const Vector2 &LB, const Vector2 &RT, Texture2D tex);

private:
    struct QuadVertex
    {
        Vector2 pos;
    };

    InputLayout quadVtxNorLayout_;
    VertexBuffer<false> quadVtxBuf_;

    DepthStencilState norDepthStencilState_;
    Sampler norSampler_;

    OWE::Shader<SS_VS, SS_PS> norShader_;
    std::unique_ptr<OWE::ShaderUniforms<SS_VS, SS_PS>> norUniforms_;
};
