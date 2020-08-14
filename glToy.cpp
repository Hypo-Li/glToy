#include <ys/ys.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//全局变量
ys::FPSCamera* camera;
ys::BodyPosture posture = ys::Body_Posture_Stand;
ys::MoveDirection moveDir = ys::Move_Direction_None;
const float standHeight = 1.7f; //站立视角高度
const float squatHeight = 1.0f; //下蹲视角高度
const float crawlHeight = 0.2f; //匍匐视角高度
const float standSpeed = 4.0f; //站立移动速度
const float squatSpeed = 2.0f; //下蹲移动速度
const float crawlSpeed = 0.5f; //卧倒移动速度
const float standSquat = 0.05f; //站立->下蹲 转换速度
const float squatStand = 0.05f; //下蹲->站立 转换速度
const float squatCrawl = 0.08f; //下蹲->卧倒 转换速度
const float crawlSquat = 0.08f; //卧倒->下蹲 转换速度
const float jumpInitSpeed = 4.0f; //跳跃初速度
const float g = 9.8f; //重力加速度
const float jumpEndTime = 2.0f * jumpInitSpeed / g;
float jumpBeginTime = 0.0f;
float jumpCurtTime = 0.0f;
const float jumpBufferHalfTime = 0.175f;
const float jumpBufferDepth = 0.1f;
const float weaponJumpShaking = 0.0f;
const float weaponBufferShaking = 0.2f;
float weaponBuffer = 0.0f;
float shakingRange = 0.0f; //武器摇晃幅度
float shakingSpeed = 8.0f; //武器摇晃速度
const float inertiaChange = 0.05f;
float inertiaSpeed = 0.0f;
float curtTime = 0.0f; //当前帧时间
float lastTime = 0.0f; //上一帧时间
float deltaTime = 0.0f; //两帧的时间差
float height = standHeight;
float speed = standSpeed;
bool moveTF = false; //角色当前是否在移动

//函数声明
void ProcessInput(GLFWwindow* window);
void ErrorCallback(int error, const char* description);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
GLuint CreateDepthMapFBO(GLuint width, GLuint height, GLuint& depthMap);

//程序入口
int main(void)
{
	//GLFW初始化、创建窗口
    glfwInit();
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //是否为不可调整大小
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE); //是否有窗口修饰
    //glfwWindowHint(GLFW_SAMPLES, 4); //多重采样缓冲，4个采样子
    GLFWwindow* window = glfwCreateWindow(1600, 900, "glToy", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //捕捉鼠标
    glfwSetCursorPosCallback(window, CursorPosCallback); //鼠标移动回调函数
    glfwSetKeyCallback(window, KeyCallback); //键盘回调函数
    glfwSetMouseButtonCallback(window, MouseButtonCallback); //鼠标按键回调函数

    //GLAD初始化
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ys::PutErrorLog("GLAD", "Failed to initialize GLAD");
        exit(-1);
    }

    //杂项变量
    glm::vec3 lightPos(-40.0f, 50.0f, -40.0f);
    glm::mat4 matE(1.0f);
    glm::mat4 lightModelMat = glm::translate(matE, lightPos);
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 curtViewMat, curtProjectionMat;
    glm::vec3 curtPosition, weaponPosition;
    glm::mat4 lightSpaceMat = lightProjection * lightView;
    glm::mat4 weaponFollowMat;

    //各种开关
    glEnable(GL_DEPTH_TEST); //深度测试
    glEnable(GL_CULL_FACE); //反面剔除

    //设置深度贴图
    GLuint depthMapWidth = 3200, depthMapHeight = 11600, depthMap;
    GLuint depthMapFBO = CreateDepthMapFBO(depthMapWidth, depthMapHeight, depthMap);

    //创建摄像机
    ys::CameraParam* cp = new ys::CameraParam;
    cp->position = glm::vec3(0.0f, 1.7f, 0.0f);
    cp->target = glm::vec3(1.0f, 1.7f, 0.0f);
    cp->WorldUpAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    cp->sensit = 0.05f;
    cp->fov = 45.0f;
    cp->yMax = 80.0f;
    cp->yMin = -80.0f;
    cp->projectionMax = 1000.0f;
    cp->projectionMin = 0.001f;
    camera = new ys::FPSCamera(*cp);
    delete cp;

    //加载Shader
    //模型渲染Shader
    ys::Shader* modelShader = new ys::Shader("./shader/model_v.glsl", "./shader/model_f.glsl");
    modelShader->GetDefaultLoc("textureDiffuse", "textureSpecular", "textureNormals");
    GLint modelLoc = modelShader->GetUniformLoc("model");
    GLint viewLoc = modelShader->GetUniformLoc("view");
    GLint projectionLoc = modelShader->GetUniformLoc("projection");
    GLint lightSpaceMatLoc = modelShader->GetUniformLoc("lightSpaceMat");
    GLint depthMapLoc = modelShader->GetUniformLoc("depthMap");
    GLint viewPosLoc = modelShader->GetUniformLoc("viewPos");
    GLint lightPosLoc = modelShader->GetUniformLoc("lightPos");
    //灯光渲染Shader
    ys::Shader* lightShader = new ys::Shader("./shader/light_v.glsl", "./shader/light_f.glsl");
    GLint lmodelLoc = lightShader->GetUniformLoc("model");
    GLint lviewLoc = lightShader->GetUniformLoc("view");
    GLint lprojectionLoc = lightShader->GetUniformLoc("projection");
    //深度贴图渲染Shader
    ys::Shader* depthShader = new ys::Shader("./shader/depth_v.glsl", "./shader/depth_f.glsl");
    GLint dmodelLoc = depthShader->GetUniformLoc("model");
    GLint dlightSpaceMatLoc = depthShader->GetUniformLoc("lightSpaceMat");
    //天空盒渲染Shader
    ys::Shader* skyShader = new ys::Shader("./shader/skybox_v.glsl", "./shader/skybox_f.glsl");
    GLint skyLoc = skyShader->GetUniformLoc("skybox");
    GLint sviewLoc = skyShader->GetUniformLoc("view");
    GLint sprojectionLoc = skyShader->GetUniformLoc("projection");

    //加载模型
    ys::StaticModel* sceneModel = new ys::StaticModel("./res/model/scene.smd", GL_STATIC_DRAW);
    ys::StaticModel* lightModel = new ys::StaticModel("./res/model/light.smd", GL_STATIC_DRAW);
    ys::StaticModel* weaponModel = new ys::StaticModel("./res/model/gun.smd", GL_STATIC_DRAW);

    //加载天空盒
    ys::SkyBox* skyBox = new ys::SkyBox("./res/sky/skybox00.jpg");

    //渲染循环
    while (!glfwWindowShouldClose(window))
    {
        //当前帧时间处理
        curtTime = glfwGetTime(); //获取当前帧时间
        deltaTime = curtTime - lastTime;
        lastTime = curtTime;

        //确定当前y值
        switch (posture)
        {
        case ys::Body_Posture_Stand:
            height = standHeight;
            break;
        case ys::Body_Posture_Squat:
            height = squatHeight;
            break;
        case ys::Body_Posture_Crawl:
            height = crawlHeight;
            break;
        case ys::Body_Posture_Stand_to_Squat:
            if (height - standSquat > squatHeight) {
                height -= standSquat;
            }
            else {
                height = squatHeight;
                posture = ys::Body_Posture_Squat;
                speed = squatSpeed;
            }
            break;
        case ys::Body_Posture_Squat_to_Crawl:
            if (height - squatCrawl > crawlHeight) {
                height -= squatCrawl;
            }
            else {
                height = crawlHeight;
                posture = ys::Body_Posture_Crawl;
                speed = crawlSpeed;
            }
            break;
        case ys::Body_Posture_Stand_to_Crawl:
            if (height - standSquat > squatHeight) {
                height -= standSquat;
            }
            else if (height - squatCrawl > crawlHeight) {
                height -= squatCrawl;
            }
            else {
                height = crawlHeight;
                posture = ys::Body_Posture_Crawl;
                speed = crawlSpeed;
            }
            break;
        case ys::Body_Posture_Crawl_to_Squat:
            if (height + crawlSquat < squatHeight) {
                height += crawlSquat;
            }
            else {
                height = squatHeight;
                posture = ys::Body_Posture_Squat;
                speed = squatSpeed;
            }
            break;
        case ys::Body_Posture_Squat_to_Stand:
            if (height + squatStand < standHeight) {
                height += squatStand;
            }
            else {
                height = standHeight;
                posture = ys::Body_Posture_Stand;
                speed = standSpeed;
            }
            break;
        case ys::Body_Posture_Crawl_to_Stand:
            if (height + crawlSquat < squatHeight) {
                height += crawlSquat;
            }
            else if (height + squatStand < standHeight) {
                height += squatStand;
            }
            else {
                height = standHeight;
                posture = ys::Body_Posture_Stand;
                speed = standSpeed;
            }
            break;
        case ys::Body_Posture_Jump:
            if (jumpBeginTime) {
                jumpCurtTime = curtTime - jumpBeginTime;
                if (jumpCurtTime <= jumpEndTime) { //跳跃阶段
                    height = (jumpInitSpeed - 0.5f * g * jumpCurtTime) * jumpCurtTime;
                    weaponBuffer = height * weaponJumpShaking;
                }
                else { //缓冲阶段
                    height = (jumpBufferDepth / jumpBufferHalfTime) * (jumpCurtTime - jumpEndTime) * ((jumpCurtTime - jumpEndTime) / jumpBufferHalfTime - 2.0f);
                    if (height >= 0.0f) {
                        height = 0.0f;
                        posture = ys::Body_Posture_Stand;
                        speed = standSpeed;
                        jumpBeginTime = 0.0f;
                    }
                    weaponBuffer = height * weaponBufferShaking;
                }
                height += standHeight;
            }
            else { //跳跃开始，设置开始时间
                jumpBeginTime = curtTime;
            }
            break;
        default:
            break;
        }

        //处理键盘输入
        ProcessInput(window);

        //处理跳跃时的移动惯性
        if (jumpBeginTime) {
            if (!moveTF) {
                switch (moveDir)
                {
                case ys::Move_Direction_Front:
                    if (inertiaSpeed >= inertiaChange * deltaTime) {
                        inertiaSpeed -= inertiaChange * deltaTime;
                        camera->MoveFront(inertiaSpeed);
                    }
                    else {
                        moveDir = ys::Move_Direction_None;
                    }
                    break;
                case ys::Move_Direction_Back:
                    if (inertiaSpeed >= inertiaChange * deltaTime) {
                        inertiaSpeed -= inertiaChange * deltaTime;
                        camera->MoveBack(inertiaSpeed);
                    }
                    else {
                        moveDir = ys::Move_Direction_None;
                    }
                    break;
                case ys::Move_Direction_Left:
                    if (inertiaSpeed >= inertiaChange * deltaTime) {
                        inertiaSpeed -= inertiaChange * deltaTime;
                        camera->MoveLeft(inertiaSpeed);
                    }
                    else {
                        moveDir = ys::Move_Direction_None;
                    }
                    break;
                case ys::Move_Direction_Right:
                    if (inertiaSpeed > inertiaChange * deltaTime) {
                        inertiaSpeed -= inertiaChange * deltaTime;
                        camera->MoveRight(inertiaSpeed);
                    }
                    else {
                        moveDir = ys::Move_Direction_None;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        else {
            if(!moveTF)
                moveDir = ys::Move_Direction_None;
        }

        //当前帧变量更新
        curtViewMat = camera->CreateViewMat();
        curtProjectionMat = camera->CreatePerspectiveProjectionMat(16.0f, 9.0f);
        curtPosition = camera->GetPosition();
        weaponPosition = curtPosition;
        weaponPosition.y += weaponBuffer;
        weaponFollowMat = glm::translate(matE, weaponPosition);

        glClearColor(0.223529f, 0.709804f, 0.290196f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //渲染深度贴图
        glCullFace(GL_FRONT);
        glViewport(0, 0, depthMapWidth, depthMapHeight);
        depthShader->Use();
        depthShader->SetMat4(dlightSpaceMatLoc, lightSpaceMat);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader->SetMat4(dmodelLoc, matE);
        sceneModel->Draw(*depthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);

        //渲染灯光源
        glViewport(0, 0, 1600, 900);
        lightShader->Use();
        lightShader->SetMat4(lmodelLoc, lightModelMat);
        lightShader->SetMat4(lviewLoc, curtViewMat);
        lightShader->SetMat4(lprojectionLoc, curtProjectionMat);
        lightModel->Draw(*lightShader);

        //渲染场景
        modelShader->Use();
        modelShader->SetMat4(modelLoc, matE);
        modelShader->SetMat4(viewLoc, curtViewMat);
        modelShader->SetMat4(projectionLoc, curtProjectionMat);
        modelShader->SetMat4(lightSpaceMatLoc, lightSpaceMat);
        modelShader->SetTexture2D(depthMapLoc, 3, depthMap);
        modelShader->SetVec3(viewPosLoc, curtPosition);
        modelShader->SetVec3(lightPosLoc, lightPos);
        sceneModel->Draw(*modelShader);

        //渲染天空盒
        skyShader->Use();
        skyShader->SetMat4(sviewLoc, glm::mat4(glm::mat3(curtViewMat)));
        skyShader->SetMat4(sprojectionLoc, curtProjectionMat);
        skyBox->Draw(*skyShader, skyLoc);

        //渲染武器
        modelShader->Use();
        weaponFollowMat = glm::rotate(weaponFollowMat, glm::radians(-camera->GetYaw()), glm::vec3(0.0f, 1.0f, 0.0f));
        weaponFollowMat = glm::rotate(weaponFollowMat, glm::radians(camera->GetPitch()), glm::vec3(0.0f, 0.0f, 1.0f));
        if (moveTF) //计算当前帧的武器晃动幅度
            if (shakingRange < 1.0f)
                shakingRange += 0.05f;
            else
                shakingRange = 1.0f;
        else
            if (shakingRange > 0.0f)
                shakingRange -= 0.05f;
            else
                shakingRange = 0.0f;
        weaponFollowMat = glm::rotate(weaponFollowMat, glm::radians(glm::cos(curtTime * shakingSpeed) * shakingRange), glm::vec3(1.0f, 1.0f, 0.0f));
        modelShader->SetMat4(modelLoc, weaponFollowMat);
        glClear(GL_DEPTH_BUFFER_BIT);
        weaponModel->Draw(*modelShader);

        //其它处理
        moveTF = false;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void ErrorCallback(int error, const char* description)
{
    ys::PutErrorLog("GLFW", description);
    exit(-1);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && posture == ys::Body_Posture_Stand) {
            camera->MoveFront(speed * deltaTime);
            shakingSpeed = 12.0f;
        }
        else {
            shakingSpeed = 8.0f;
        }
        camera->MoveFront(speed * deltaTime);
        moveTF = true;
        moveDir = ys::Move_Direction_Front;
        inertiaSpeed = speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->MoveBack(0.5f * speed * deltaTime);
        moveTF = true;
        moveDir = ys::Move_Direction_Back;
        inertiaSpeed = 0.5f * speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->MoveLeft(0.8f * speed * deltaTime);
        moveTF = true;
        moveDir = ys::Move_Direction_Left;
        inertiaSpeed = 0.8f * speed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->MoveRight(0.8f * speed * deltaTime);
        moveTF = true;
        moveDir = ys::Move_Direction_Right;
        inertiaSpeed = 0.8f * speed * deltaTime;
    }
    camera->SetPositionY(height);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    camera->MoveView(xpos, ypos);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
        switch (key)
        {
        case GLFW_KEY_SPACE:
            switch (posture)
            {
            case ys::Body_Posture_Crawl:
                posture = ys::Body_Posture_Crawl_to_Stand;
                break;
            case ys::Body_Posture_Squat:
                posture = ys::Body_Posture_Squat_to_Stand;
                break;
            case ys::Body_Posture_Stand:
                posture = ys::Body_Posture_Jump;
                break;
            default:
                break;
            }
            break;
        case GLFW_KEY_C:
            switch (posture)
            {
            case ys::Body_Posture_Stand:
                posture = ys::Body_Posture_Stand_to_Squat;
                break;
            case ys::Body_Posture_Crawl:
                posture = ys::Body_Posture_Crawl_to_Squat;
                break;
            default:
                break;
            }
            break;
        case GLFW_KEY_LEFT_CONTROL:
            switch (posture)
            {
            case ys::Body_Posture_Stand:
                posture = ys::Body_Posture_Stand_to_Crawl;
                break;
            case ys::Body_Posture_Squat:
                posture = ys::Body_Posture_Squat_to_Crawl;
                break;
            default:
                break;
            }
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        default:
            break;
        }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

GLuint CreateDepthMapFBO(GLuint width, GLuint height, GLuint& depthMap)
{
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return depthMapFBO;
}