#include "Escaper.h"
#include "Game.h"
#include "Scene.h"


enum EscaperAnims
{
	ESCAPING
};

void Escaper::initAnims(ShaderProgram &shaderProgram) {
	jobSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(1.f / 16, 1.f / 14), &shaderProgram, &Game::spriteSheets().lemmingAnimations, &Game::spriteSheets().rotatedLemmingAnimations);
	jobSprite->setNumberAnimations(1);

	// ESCAPING
	jobSprite->setAnimationSpeed(ESCAPING, 12);
	for (int i = 0; i<7; i++)
		jobSprite->addKeyframe(ESCAPING, glm::vec2(float(i + 1) / 16, 1.0f / 14));

	state = ESCAPING_STATE;
	jobSprite->changeAnimation(ESCAPING);
}

void Escaper::updateStateMachine(int deltaTime) {

	switch (state)
	{
	case ESCAPING_STATE:
		if (jobSprite->isInLastFrame()) {
			isFinished = true;
			nextJob = NULL;
		}
	}
}







diff --git a/Lemmings/02-Lemming.vcxproj b/Lemmings/02-Lemming.vcxproj
index 28e87d1..59184bb 100644
--- a/Lemmings/02-Lemming.vcxproj
+++ b/Lemmings/02-Lemming.vcxproj
@@ -12,13 +12,16 @@
   </ItemGroup>
   <ItemGroup>
     <ClInclude Include="AnimKeyframes.h" />
+    <ClInclude Include="Basher.h" />
     <ClInclude Include="Button.h" />
     <ClInclude Include="ButtonFactory.h" />
     <ClInclude Include="common_defs.h" />
     <ClInclude Include="Digger.h" />
     <ClInclude Include="DoorFactory.h" />
     <ClInclude Include="Escaper.h" />
+    <ClInclude Include="Exploder.h" />
     <ClInclude Include="Game.h" />
+    <ClInclude Include="Miner.h" />
     <ClInclude Include="Job.h" />
     <ClInclude Include="JobFactory.h" />
     <ClInclude Include="Lemming.h" />
@@ -39,6 +42,7 @@
     <ClInclude Include="Walker.h" />
   </ItemGroup>
   <ItemGroup>
+    <ClCompile Include="Basher.cpp" />
     <ClCompile Include="Button.cpp" />
     <ClCompile Include="ButtonFactory.cpp" />
     <ClCompile Include="Digger.cpp" />
@@ -52,12 +56,14 @@
     <ClCompile Include="main.cpp" />
     <ClCompile Include="MaskedTexturedQuad.cpp" />
     <ClCompile Include="Menu.cpp" />
+    <ClCompile Include="Miner.cpp" />
     <ClCompile Include="MouseManager.cpp" />
     <ClCompile Include="NumDisplay.cpp" />
     <ClCompile Include="NumFactory.cpp" />
     <ClCompile Include="Scene.cpp" />
     <ClCompile Include="Shader.cpp" />
     <ClCompile Include="ShaderProgram.cpp" />
+    <ClCompile Include="Exploder.cpp" />
     <ClCompile Include="Sprite.cpp" />
     <ClCompile Include="Texture.cpp" />
     <ClCompile Include="TrapdoorFactory.cpp" />
@@ -69,7 +75,7 @@
     <ProjectGuid>{3D222D6E-523F-40A6-B219-EDBB9426450B}</ProjectGuid>
     <Keyword>Win32Proj</Keyword>
     <RootNamespace>My02Lemming</RootNamespace>
-    <WindowsTargetPlatformVersion>10.0.15063.0</WindowsTargetPlatformVersion>
+    <WindowsTargetPlatformVersion>10.0.16299.0</WindowsTargetPlatformVersion>
   </PropertyGroup>
   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
@@ -126,14 +132,14 @@
       <FunctionLevelLinking>true</FunctionLevelLinking>
       <IntrinsicFunctions>true</IntrinsicFunctions>
       <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
-      <AdditionalIncludeDirectories>..\..\libs\freeglut\include;..\..\libs\glew-1.13.0\include;..\..\libs\glm;..\..\libs\Simple OpenGL Image Library\src</AdditionalIncludeDirectories>
+      <AdditionalIncludeDirectories>..\libs\freeglut\include;..\libs\glew-1.13.0\include;..\libs\glm;..\libs\Simple OpenGL Image Library\src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
     </ClCompile>
     <Link>
       <SubSystem>Console</SubSystem>
       <GenerateDebugInformation>true</GenerateDebugInformation>
       <EnableCOMDATFolding>true</EnableCOMDATFolding>
       <OptimizeReferences>true</OptimizeReferences>
-      <AdditionalLibraryDirectories>..\..\libs\freeglut\lib;..\..\libs\glew-1.13.0\lib\Release\Win32;..\..\libs\Simple OpenGL Image Library\projects\VC9\Debug</AdditionalLibraryDirectories>
+      <AdditionalLibraryDirectories>..\libs\freeglut\lib;..\libs\glew-1.13.0\lib\Release\Win32;..\libs\Simple OpenGL Image Library\projects\VC9\Debug;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
       <AdditionalDependencies>freeglut.lib;glew32.lib;SOIL.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>
     </Link>
   </ItemDefinitionGroup>
diff --git a/Lemmings/Escaper.cpp b/Lemmings/Escaper.cpp
index 50cde4d..0168675 100644
--- a/Lemmings/Escaper.cpp
+++ b/Lemmings/Escaper.cpp
@@ -22,7 +22,6 @@ void Escaper::initAnims(ShaderProgram &shaderProgram) {
 }
 
 void Escaper::updateStateMachine(int deltaTime) {
-	int fall;
 
 	switch (state)
 	{
diff --git a/Lemmings/Game.cpp b/Lemmings/Game.cpp
index 7fd61c9..b4814d1 100644
--- a/Lemmings/Game.cpp
+++ b/Lemmings/Game.cpp
@@ -47,12 +47,16 @@ void Game::render() {
 
 void Game::keyPressed(int key)
 {
-	if(key == 27) // Escape code
-		bPlay = false;
-
-	if (key == '0' && currentState == GameState::PLAYING) { // key 0 returns you to menu
-		currentState = GameState::MENU;
-		menu.init();
+	if (key == 27) // Escape code
+	{
+		if (currentState == GameState::PLAYING)
+		{
+			currentState = GameState::MENU;
+			menu.init();
+		}
+
+		else if (currentState == GameState::MENU)
+			bPlay = false;
 	}
 
 	keys[key] = true;
@@ -66,7 +70,7 @@ void Game::keyReleased(int key)
 
 void Game::specialKeyPressed(int key)
 {
-	if (key == GLUT_KEY_F1 && currentState == GameState::MENU) { // key p go to playing
+	if (key == GLUT_KEY_F1 && currentState == GameState::MENU) { // key f1 go to playing
 		currentState = GameState::PLAYING;
 		Scene::getInstance().init("levels/fun-1.txt");
 	}
diff --git a/Lemmings/MouseManager.cpp b/Lemmings/MouseManager.cpp
index f0e46ac..72e0918 100644
--- a/Lemmings/MouseManager.cpp
+++ b/Lemmings/MouseManager.cpp
@@ -115,28 +115,34 @@ void MouseManager::activateButton(int buttonIndex)
 		break;
 	case Button::EXPLODER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
-			jobToAssign = JobFactory::instance().createDiggerJob();
+			jobToAssign = JobFactory::instance().createExploderJob();
 		}
+		break;
 	case Button::BLOCKER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
 			jobToAssign = JobFactory::instance().createDiggerJob();
 		}
+		break;
 	case Button::BUILDER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
 			jobToAssign = JobFactory::instance().createDiggerJob();
 		}
+		break;
 	case Button::BASHER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
 			jobToAssign = JobFactory::instance().createDiggerJob();
 		}
+		break;
 	case Button::MINER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
-			jobToAssign = JobFactory::instance().createDiggerJob();
+			jobToAssign = JobFactory::instance().createMinerJob();
 		}
+		break;
 	case Button::DIGGER_BUTTON:
 		if (UI::getInstance().getSelectedButtonJobCount() > 0) {
 			jobToAssign = JobFactory::instance().createDiggerJob();
 		}
+		break;
 	case Button::PAUSE_BUTTON:
 
 		break;
diff --git a/Lemmings/Scene.cpp b/Lemmings/Scene.cpp
index d6d26f1..04993ea 100644
--- a/Lemmings/Scene.cpp
+++ b/Lemmings/Scene.cpp
@@ -133,6 +133,7 @@ void Scene::initMap()
 {
 	glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(float(LEVEL_WIDTH), float(LEVEL_HEIGHT)) };
 	glm::vec2 texCoords[2] = { glm::vec2(120.f / 512.0, 0.f), glm::vec2((120.f + 320.f) / 512.0f, 160.f / 256.0f) };
+	//glm::vec2 texCoords[2] = { glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f) };
 
 	map = MaskedTexturedQuad::createTexturedQuad(geom, texCoords, maskedTexProgram);
 
diff --git a/Lemmings/Walker.cpp b/Lemmings/Walker.cpp
index 71e901e..dd9da75 100644
--- a/Lemmings/Walker.cpp
+++ b/Lemmings/Walker.cpp
@@ -156,11 +156,4 @@ void Walker::updateStateMachine(int deltaTime) {
 		}
 		break;
 	}
-}
-
-
-
-
-
-
-
+}
\ No newline at end of file
diff --git a/Lemmings/images/levels/fun2.png b/Lemmings/images/levels/fun2.png
index bca8d76..c43068a 100644
Binary files a/Lemmings/images/levels/fun2.png and b/Lemmings/images/levels/fun2.png differ
diff --git a/Lemmings/images/menu/menu-background.png b/Lemmings/images/menu/menu-background.png
index fd4e0b2..bfafb06 100644
Binary files a/Lemmings/images/menu/menu-background.png and b/Lemmings/images/menu/menu-background.png differ
diff --git a/Lemmings/levels/fun-1.txt b/Lemmings/levels/fun-1.txt
index 3357e37..a4fa73f 100644
--- a/Lemmings/levels/fun-1.txt
+++ b/Lemmings/levels/fun-1.txt
@@ -1,5 +1,5 @@
 fun 1
 10 1 300
-50 0 0 0 0 0 0 0 10
+50 0 0 0 0 0 0 10 10
 47 30 217 99
 60 30 230 115
diff --git a/libs/.gitignore b/libs/.gitignore
deleted file mode 100644
index 72e8ffc..0000000
--- a/libs/.gitignore
+++ /dev/null
@@ -1 +0,0 @@
-*