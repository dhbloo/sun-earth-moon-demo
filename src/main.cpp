#include "CubeMap.h"
#include "FrameBuffer.h"
#include "Fwd.h"
#include "GLFWWindow.h"
#include "Mesh.h"
#include "MeshPreset.h"
#include "RenderPass.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <filesystem>

class DemoWindow : public GLFWWindowBase
{
public:
    DemoWindow(GLFWwindow *window) : GLFWWindowBase(window)
    {
        SetVsync(true);
        SetResizable(true, false);
        SetCursorCapture(true);
    }

private:
    void Setup() override
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
        glClearColor(0, 0, 0, 1);

        blurPassH                             = New<RenderPass>(MeshPreset::CreateQuad(),
                                    New<ShaderProgram>(VertShader::FromFile("shader/blur.vert"),
                                                       FragShader::FromFile("shader/blur.frag")));
        blurPassH->ShaderUniform("blurDir")   = glm::vec2(2.4, 0);
        blurPassH->ShaderUniform("blurSigma") = 9.0f;

        blurPassV                             = New<RenderPass>(MeshPreset::CreateQuad(),
                                    New<ShaderProgram>(VertShader::FromFile("shader/blur.vert"),
                                                       FragShader::FromFile("shader/blur.frag")));
        blurPassV->ShaderUniform("blurDir")   = glm::vec2(0, 2.4);
        blurPassV->ShaderUniform("blurSigma") = 9.0f;

        postProcessingPass =
            New<RenderPass>(MeshPreset::CreateQuad(),
                            New<ShaderProgram>(VertShader::FromFile("shader/postprocessing.vert"),
                                               FragShader::FromFile("shader/postprocessing.frag")));
        postProcessingPass->ShaderUniform("exposure") = 1.0f;
        postProcessingPass->ShaderUniform("bloom")    = 0.2f;

        FrameSizeCallback(GetWidth(), GetHeight());

        // Environment CubeMap Pass
        auto envCubeMap = New<CubeMap>(std::array<std::string, 6> {
            "envbox/right.jpg",
            "envbox/left.jpg",
            "envbox/top.jpg",
            "envbox/bottom.jpg",
            "envbox/front.jpg",
            "envbox/back.jpg",
        });
        envCubeMap->Setup();

        environmentPass =
            New<RenderPass>(MeshPreset::CreateUnitBox(),
                            New<ShaderProgram>(VertShader::FromFile("shader/cubemap.vert"),
                                               FragShader::FromFile("shader/cubemap.frag")));
        environmentPass->ShaderUniform("cubeMap") = envCubeMap;
        environmentPass->SetCulling(true, GL_FRONT);
        environmentPass->SetDepthTest(true, GL_LEQUAL);

        // Sun
        PRenderPass sun =
            New<RenderPass>(MeshPreset::CreateUnitShpere(6),
                            New<ShaderProgram>(VertShader::FromFile("shader/sun.vert"),
                                               FragShader::FromFile("shader/sun.frag")));
        sun->ShaderUniform("emissionTex")     = New<Texture>("surface/sun.jpg");
        sun->ShaderUniform("brightness")      = 3.2f;
        sun->ShaderUniform("fresnelEmission") = glm::vec3(1.0f, 0.71f, 0.29f);
        sun->SetNeedNormalMatrix(true);
        auto sunAnimFunc = [=](float t) {
            sun->ShaderUniform("time") = t;
            return glm::scale(glm::identity<glm::mat4>(), glm::one<glm::vec3>() * sunSize);
        };

        // Moon
        PRenderPass moon =
            New<RenderPass>(MeshPreset::CreateUnitShpere(5),
                            New<ShaderProgram>(VertShader::FromFile("shader/moon.vert"),
                                               FragShader::FromFile("shader/moon.frag")));
        moon->ShaderUniform("diffuseTex")   = New<Texture>("surface/moon.jpg");
        moon->ShaderUniform("normalMapTex") = New<Texture>("surface/moon_normal.jpg", false);
        moon->ShaderUniform("ambient")      = glm::one<glm::vec3>() * 0.02f;
        moon->ShaderUniform("sunPos")       = glm::vec3(0, 0, 0);
        moon->ShaderUniform("earthRadius")  = earthSize;
        moon->ShaderUniform("sunColor")     = glm::vec3(1.f, 0.99f, 0.94f) * 9000.f;
        moon->SetNeedNormalMatrix(true);
        auto moonAnimFunc = [=](float t) {
            glm::mat4 m = glm::identity<glm::mat4>();
            m           = glm::rotate(m, glm::fract(t / 365.f) * 2 * PI, glm::vec3(0, 1, 0));
            m           = glm::translate(m, glm::vec3(sunEarthDist, 0.f, 0.f));
            moon->ShaderUniform("earthPos") = glm::vec3(m[3]);
            m = glm::rotate(m, glm::cos(t / 90.f) * 0.4f, glm::vec3(0, 0, 1));
            m = glm::rotate(m, glm::fract(t / 30.f) * 2 * PI, glm::vec3(0, 1, 0));
            m = glm::translate(m, glm::vec3(earthMoonDist, 0.f, 0.f));
            m = glm::scale(m, glm::one<glm::vec3>() * moonSize);
            m = glm::rotate(m, glm::fract(t / 30.f) * 2 * PI, glm::vec3(0, -1, 0));
            return m;
        };

        // Earth
        PRenderPass earth =
            New<RenderPass>(MeshPreset::CreateUnitShpere(6),
                            New<ShaderProgram>(VertShader::FromFile("shader/earth.vert"),
                                               FragShader::FromFile("shader/earth.frag")));
        earth->ShaderUniform("diffuseTex")   = New<Texture>("surface/earth_diffuse.jpg");
        earth->ShaderUniform("specularTex")  = New<Texture>("surface/earth_specular.jpg");
        earth->ShaderUniform("normalMapTex") = New<Texture>("surface/earth_normal.jpg", false);
        earth->ShaderUniform("nightTex")     = New<Texture>("surface/earth_nightmap.jpg");
        earth->ShaderUniform("cloudTex")     = New<Texture>("surface/earth_clouds.jpg");
        earth->ShaderUniform("sunPos")       = glm::vec3(0, 0, 0);
        earth->ShaderUniform("sunColor")     = glm::vec3(1.f, 0.99f, 0.94f) * 40000.f;
        earth->ShaderUniform("sunRadius")    = sunSize;
        earth->ShaderUniform("moonRadius")   = moonSize;
        earth->ShaderUniform("earthRadius")  = earthSize;
        earth->ShaderUniform("bumpiness")    = 10.0f;
        earth->ShaderUniform("atmosphereThickness") = 0.05f;
        earth->ShaderUniform("lightSamples")        = 32;
        earth->ShaderUniform("scatterViewSamples")  = 20;
        earth->ShaderUniform("scatterLightSamples") = 10;
        earth->SetNeedNormalMatrix(true);
        auto earthAnimFunc = [=](float t) {
            glm::mat4 m = glm::identity<glm::mat4>();
            m           = glm::rotate(m, glm::fract(t / 365.f) * 2 * PI, glm::vec3(0, 1, 0));
            m           = glm::translate(m, glm::vec3(sunEarthDist, 0.f, 0.f));
            m           = glm::scale(m, glm::one<glm::vec3>() * earthSize);
            m           = glm::rotate(m,
                            glm::fract(t / 6.f) * 2 * PI,
                            glm::normalize(glm::vec3(0.1f, 1, 0.1f)));
            earth->ShaderUniform("earthPos") = glm::vec3(m[3]);
            earth->ShaderUniform("moonPos")  = glm::vec3(moonAnimFunc(t)[3]);
            return m;
        };

        objects.push_back(RenderObject {sun, sunAnimFunc});
        objects.push_back(RenderObject {earth, earthAnimFunc});
        objects.push_back(RenderObject {moon, moonAnimFunc});
    }

    void Render(float elapsed) override
    {
        float     theta  = GetCursorPos().x * rotateSpeed;
        float     phi    = glm::clamp(1.f - GetCursorPos().y / GetHeight(), 0.0001f, 0.9999f) * PI;
        glm::vec3 dir    = {glm::cos(theta) * glm::sin(phi),
                         glm::cos(phi),
                         glm::sin(theta) * glm::sin(phi)};
        glm::vec3 center = objects[lootAtIndex].timedModelMatrixFunc(elapsed)[3];
        glm::mat4 proj   = glm::perspective(45.0f, (float)GetWidth() / GetHeight(), 1.0f, 1000.0f);
        glm::mat4 view   = glm::lookAt(center + rotateRadius * dir, center, glm::vec3(0, 1, 0));

        renderBuffer->Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (RenderObject &object : objects) {
            glm::mat4 model = object.timedModelMatrixFunc(elapsed);
            object.renderPass->Render(&model, &view, &proj);
        }

        if (environmentPass) {
            glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(view));
            environmentPass->Render(nullptr, &viewNoTranslate, &proj);
        }

        blurBufferH->Use();
        glClear(GL_COLOR_BUFFER_BIT);
        blurPassH->Render(nullptr, nullptr, nullptr);

        blurBufferV->Use();
        glClear(GL_COLOR_BUFFER_BIT);
        blurPassV->Render(nullptr, nullptr, nullptr);

        FrameBuffer::Finish();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        postProcessingPass->Render(nullptr, nullptr, nullptr);
    }

    void KeyCallback(int key, int scancode, int action, int mods) override
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            Close();
        }
        else if (key == GLFW_KEY_V && action == GLFW_PRESS) {
            lootAtIndex = (lootAtIndex + 1) % (int)objects.size();
        }
    }

    void FrameSizeCallback(int width, int height) override
    {
        renderBuffer          = New<FrameBuffer>(width, height);
        PTexture colorTexture = renderBuffer->AddTextureAttachment(FrameBuffer::RGB16F);
        colorTexture->SetProperty(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        colorTexture->SetProperty(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        colorTexture->Setup(false);
        renderBuffer->AddRenderObjAttachment(FrameBuffer::Depth32);

        blurBufferH           = New<FrameBuffer>(width, height);
        PTexture blurTextureH = blurBufferH->AddTextureAttachment(FrameBuffer::RGB16F);
        blurTextureH->SetProperty(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        blurTextureH->SetProperty(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        blurTextureH->Setup(false);

        blurBufferV           = New<FrameBuffer>(width, height);
        PTexture blurTextureV = blurBufferV->AddTextureAttachment(FrameBuffer::RGB16F);
        blurTextureV->Setup(false);

        blurPassH->ShaderUniform("inputTexture") = colorTexture;
        blurPassH->ShaderUniform("resolution")   = glm::vec2(width, height);

        blurPassV->ShaderUniform("inputTexture") = blurTextureH;
        blurPassV->ShaderUniform("resolution")   = glm::vec2(width, height);

        postProcessingPass->ShaderUniform("inputTexture") = colorTexture;
        postProcessingPass->ShaderUniform("bloomTexture") = blurTextureV;

        glViewport(0, 0, width, height);
        std::cout << "Frame size: " << width << ", " << height << std::endl;
    }

    void ScrollCallback(glm::vec2 offset) override
    {
        rotateRadius = glm::clamp(rotateRadius - offset.y, rotateRadiusMin, rotateRadiusMax);
    }

    struct RenderObject
    {
        PRenderPass                     renderPass;
        std::function<glm::mat4(float)> timedModelMatrixFunc;
    };
    std::vector<RenderObject> objects;
    PFrameBuffer              renderBuffer;
    PFrameBuffer              blurBufferH;
    PFrameBuffer              blurBufferV;
    PRenderPass               environmentPass;
    PRenderPass               blurPassH;
    PRenderPass               blurPassV;
    PRenderPass               postProcessingPass;
    float                     rotateRadius = 30.0f;
    int                       lootAtIndex  = 0;

    static constexpr float rotateSpeed     = 0.003f;
    static constexpr float rotateRadiusMin = 3.0f;
    static constexpr float rotateRadiusMax = 50.0f;

    static constexpr float sunSize       = 6.f;
    static constexpr float earthSize     = 1.f;
    static constexpr float moonSize      = 0.25f;
    static constexpr float sunEarthDist  = 48.f;
    static constexpr float earthMoonDist = 3.f;
};

int main(int argc, char *argv[])
{
    if (argc > 1) {
        std::filesystem::current_path(argv[1]);
    }

    auto demo = DemoWindow::Create<DemoWindow>(1024, 768, "Demo Window");
    if (!demo)
        return -1;

    while (demo->TickFrame()) {
        demo->SetTitle(("Demo FPS: " + std::to_string(demo->GetAverageFps())).c_str());
    }
}
